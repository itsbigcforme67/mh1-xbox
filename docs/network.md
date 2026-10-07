# Online play (network) for the PC / Xbox port

Written 7 Oct 2026 by agent E. Single player has priority; everything here is behind `ONLINE=1`
(PC build option) and changes nothing in a normal build.

**Rule (CLAUDE.md): nothing in this repository connects to MH Oldschool.** The network backend refuses the
known MH Oldschool addresses and every public address (see "Safety"). All development and tests use
`tools/mh1_testserver.py` on 127.0.0.1. The draft message to the MH Oldschool operators is at the end.

## 1. Where it stands

| | |
|---|---|
| Build | `ONLINE=1 tools/build_pc.sh` makes `build/pc/mhview_online`; a plain build is unchanged |
| Backend | `src/pc/net/net_cpinet.c` (the game's `CpInet*` TCP / DNS calls on POSIX or Winsock sockets), `net_dnas.c` (DNAS answers success), `net_netcnf.c` (an empty connection-settings store) |
| Client under test | the game's own lobby-server client, Capcom's `cnet` / `cnLBS_*` code (`src/lobby/cnet`) and `tcp_init`, `select_ps2`, `connect_ps2` (`src/lobby/b`), compiled for the PC from the decompiled C |
| Server | `tools/mh1_testserver.py`, Python standard library only, listens on 127.0.0.1, written from the client code (section 5) |
| Test | `tools/test_online.sh` (about 12 s, headless): connect by address and by name, login, mini data, login finish, top information, current place, plaza list and plaza entry, lobby list and lobby entry, lobby member list, logout, then two clients at once with join / leave notices and chat, and three refused destinations |
| Furthest point | inside a lobby: member list, notices, chat. Not reached: rooms, matching, a game session, patches, the portal web pages (section 7) |
| Co-op (direct connect) | `--host` / `--join IP`: 2-4 players on one quest stage see each other walk (section 3.4, `tools/test_coop.sh`). Monsters, quest results and the supply box are not shared yet |

What "reaches the lobby" means here, honestly: the headless test (`--nettest`, `src/pc/rt/rt_net.c`) calls the
game's `cnLBS_*` functions in the order the lobby UI (`src/lobby/f/lb_cli.c`: `lbc_login_init`, `lbc_login_*`,
`lbc_top_menu_*`, `lbc_in_plaza_*`) calls them, with the screens replaced by a small state machine. The packets on
the wire are built and parsed entirely by the game's C. The village overlay itself is **not** running online yet:
the town stages, the portal pages and the plaza / lobby screens still need the lobby overlay's online mode
(`Online_ck()`) to be switched on and fed by this layer.

## 1a. Co-op over direct connect: findings (agent E, 7 Oct 2026, round 2)

Goal (owner): bare-minimum 4-player co-op by direct connect. A host picks a quest, up to 3 others join by IP, all
hunters are on the same quest stage. No lobby, town, matchmaking or chat. Everything stays behind `ONLINE=1`.

"Confirmed" below means read in the decompiled C or the asm (function and file named). "Guess" means inferred.

### Is the hunt peer-to-peer?

**In the logic, yes. On the wire, no: every packet goes through a game server that only relays.**

* Confirmed: after matching, each client opens **one TCP connection** to the address the lobby gives in the 6916
  answer (`_cnet_RecvFromLbs_MatchGameServerAddr`, cnlbsg.c; `cmcs_01` -> `connect_ps2`). `cmcs_04` answers the
  server's 0x1031 with a 0x82 packet (the login key, `mmbbc_encode`); that is the **only** 0x82 packet the client ever
  sends. `cmcs_06` -> `AQSession_init_online` -> `CngSessionStart_online` (cng_nm.c) -> `mcsls_init` runs the whole
  session on that same socket.
* Confirmed: there is **no UDP** and **no connection to another player's address**. `udp_send_buff` / `rudp_send_buff`
  are only cleared (`mcsls_init`). All sends are `CpInetTcpSend(mcsls_w.sock)` (`mcsls_send`), all receives
  `CpInetTcpRecv` (`CnInetMcsReceive`).
* Confirmed: the "P2P" functions are leftovers. `CngNetMcsP2PPoll` (0x22E450) runs every frame but in the retail state
  it only calls `mcsls_move`; its connect states call `InetConnectStart`, a stub returning 1 (cnmsg_nm.c), and nothing
  sets them. `CngNetAQHostIPSet` stores one of four Capcom LAN addresses (`adrs_tbl`) that no code reads.
* Confirmed: the session stream is not 0x82 packets (section 5.5 said so; corrected there). It is records
  `[u8 len][u8 cmd<<4 | sender]`: 0x20 ping, 0x30 pong, 0x40 / 0x50 app data (whole / fragment), 0x70 / 0x80 the same
  padded, 0x90 sync flag, 0xF0 keep-alive. Records carry **no destination**: every record is for everyone. The receiver
  takes the sender from the low nibble and ignores its own id, and loops its own app data back locally. The client
  never makes a record whose length byte is 0x28, because server packets start with 0x28 (12-byte header, command
  big endian at 2-3: 0x1021 health check, 0x1032). So the server must **forward each client's records unchanged to the
  other members** and must not echo them back.
* What the server does during a quest (confirmed from the client side): relay, the 0x1021 health check, and
  presumably closing a session whose sockets drop. **Nothing in the game logic is decided by the server**: sync flags,
  the 8-round ping, the start timing, the quest timer, clear / fail, the supply box, monster control and the host
  change are all decided by the clients. The lowest-numbered player still connected is the session master and the
  "host" (`mcsls_calc_master_id`, `host_change`, aq_nm.c).
* Battle result 6138: the client answers with `CnetSys_w.batres`, which the decompiled code only ever clears (0xFF).
  Guess: quest results were not reported to the server in MH1, or somewhere we have not found.
* Guess: the real server also consumed / forwarded the 0xF0 keep-alives and the 0x1032 is an ack.

Consequence for the port: the in-quest protocol is **"a dumb relay + clients that agree among themselves"**. A
direct-connect port can let the hosting player's game do the relaying; no server program is needed.

### The layers, and where the port plugs in

    game code: net_send_pl / net_send_em / net_send_sys / net_send_host / net_send_chat   (src/main/net/netsyn*.c)
        -> AQ_data_put(channel, packet)                     (aq_nm.c; timestamped queue, "AQ")
        -> CngNetAQ* (cng*.c, aqcmd*.c)                      (batching, time order)
        -> mcsls_* (mcsls_*.c)                               (records, ping, sync, fragments)
        -> CpInetTcp* to the game server                     (net_cpinet.c on the PC)
    and back up: ... -> AQ_exec -> AQ_recv -> self_data_ctrl(channel) -> net_receive_pl / _em / _sys / _host / _chat

Channels (confirmed, `self_data_ctrl`): 1-4 = player slot + 1 (`net_send_pl`), 6 chat, 7 host (supply box),
8 monsters (`net_send_em`), 10 sys (`net_send_sys`). Byte 2 of every packet is the sender's slot (`AQ_data_put` writes
`game_w.master` there) and the receiver drops packets with its own slot.

**Decision for the port (M1):** cut at `AQ_data_put` / `self_data_ctrl`, not at `CpInetTcp*`. The AQ / Cng / mcsls
stack exists to time-order and fragment packets over a slow relayed modem link and to talk to Capcom's server; on a
LAN the game's own packets can go straight between the players over one TCP connection per joiner, with the host
relaying (`src/pc/net/net_peer.c`). The game's packet builders and handlers (`netsyn*.c`) are used unchanged. The full
mcsls stack stays available for a later "talk to a PS2" mode.

### What an online quest start needs (confirmed, `Game_task` / `game0`-`game13`, f_game*.c, flow_nm.c)

| State | Where it lives | Set by (PS2) | Read by |
|---|---|---|---|
| online flag | `system_w.online` (+0x10), `Online_ck()` | `mode_sel_end` (omake_nm.c) | everything below |
| player count | `game_w.pl_num` (+0xD3) | `Game_task` step 3 = `AQ_join_num_get()` | monster sight / hate loops, `Em_Master_Change`, quest judging |
| per-slot state | `game_w.pl_state[8]` (+0x208): 1 in, 0 empty, 0xFF dropped | `Game_task` step 3 | `stage_load`, `net_start_ck`, `host_change` |
| local slot | `game_w.master` (+0xD1). Despite the name: **this machine's** slot | lobby member list, then `AQSession_wait` = `CngNetAQConnectIdGet()` | `Pl_master_ck`, `pl_sw_set`, `game2`, every send |
| host slot | `game_w.x21B` (guess from `host_change`) | `host_change` | supply box (`net_send_host`), sys kinds 7 / 10 |
| user id per slot | `game_w.x1E8[slot][8]` | own: `Copy_user_id`; others: `net_receive_sys` kind 2 | `Set_mini_data_to_pl` |
| room members | `room_member_id[4][8]`, `_handle[4][0x11]`, `_mini_data[4][0x40]` (0x3A36D0-0x3A3840) | `lbc_game_ready_01`, `CallBack_Event_MatchStart` | `Get_pl_id` / `Set_mini_data_to_pl` (f_ud.c) |
| mini data (0x40, 0x18 used) | weapon job, sex, hair colour (PLW+0x5FC), weapon triple (+0x35E/0x360/0x362), armour + face (+0x352[6]), +0x8D3 ... | `Lb_set_mini_data` (lb_village_nm.c) | `Set_mini_data_to_pl` -> remote PLWs in `init_pl_work` |
| quest number | `select_w.xAC` -> `game_w.quest`, `quest_w.no` | room creator / quest board / joiner (`Lb_join`); re-sent by slot 0 in sys kind 2 | `Quest_start` (f_quest01.c) |
| character select | `select_w.x8C[i]`, `x54[i]` | `player_sel` / `sel_default_set`, sys kind 2 | `game11` |
| start sync / delay | `game_w.xD6` steps, `game_w.x1B0` (frames of network delay) | `net_start_ck` (netsyn07_nm.c, from `game13`) | `net_plpos_set`, `net_emact_set` |
| random seed | none exchanged: `Rnd_w` is seeded from the clock (`init_ran_suu`) | | the monster's owner decides, so no shared seed is needed |

Spawning (confirmed, flow_nm.c): `stage_load` -> `init_pl_work` gives every slot `i < pl_num` with `pl_state == 1`
`be_flag`, `id = i`, then the local slot `Set_userdata`, the others `Set_mini_data_to_pl(game_w.x1E8[i])`; then
`player_all_load(i)` loads each slot's models and motions. `pl_init` (pl01.c) initialises every slot with `be_flag`.
`TimingValue` 6890 is only used by the town (shops, talk), not by the quest.

### What the quest code does online, and its PC status

Remote hunters are **driven by replayed state, not by pad input** (confirmed): `pl_sw_set` reads the pad only into
`player_work[game_w.master]`; `basic_com_ck` returns at once for other slots. The local hunter sends `net_send_pl`
(netsyn01.c) kind 1 on every action change (`act_set` / `Pl_act_set`, `to_normal`), kind 2 every 20 ticks (position,
vital), 3 at start / respawn (`pl_init`), 4 on fainting, 5 on an area change (guess), 6 item use, 7 / 8 giving an
item. `net_receive_pl` (netsyn02_nm.c) writes into `player_work[slot]`: position (snapped, or a target that
`Pl_adj_calc` slides towards), angle, `Pl_act_set` (the remote hunter plays the same action and motion locally),
vital and the rest.

Monsters (confirmed): ownership is per monster. `EMW+0x8C3 == 0` = this machine runs its AI, `+0x88E` = owner slot.
`Em_Master_Change` (em_master_nm.c, every frame, only when `pl_num > 1`) hands a monster on when its owner drops or
leaves its area, and sends `net_send_em` kind 4. The owner sends kinds 1 / 3 on each action (`net_act_set` in
`em_act_set2`), with position, angles, **HP** and status. Hate is per hunter (`Em_Hate_Add`, `EMW+0x918[pl]`). Guess
(not traced hit by hit): there is no "damage dealt" packet; remote hunters replay their attacks locally, so the
owner's machine computes the hits itself and its HP overwrites everyone else's.

Quest level (confirmed): sys kind 6 (`Quest_net_sub`, f_quest_nm.c): 1 delivery, 2 / 11 clear, 4 / 10 fail, 5 timer
(the lower one wins), 7 monster killed (only the owner sends), 8 captured, 9 shared item, 12 / 13 reward data;
sys 0xC abandon. Supply box: host arbitrated (channel 7, `box_get` -> `net_send_host(1)`).

| Part | Functions | Source | PC status before this round |
|---|---|---|---|
| online flag | `Online_ck` | rt_main.c | stand-in, always 0 |
| hunter send / receive | `net_send_pl`, `net_receive_pl` | netsyn01.c (matched), netsyn02_nm.c (near-match) | not compiled; no-op stand-in for the send |
| position delay | `net_plpos_set`, `net_receive_pl_pos_set` | netsyn03.c (matched) | not compiled; no-op stand-in |
| callers of the send | `act_set`, `Pl_act_set`, `pl_init`, `pl_die000`, `pl_mv014`, `trade_get_ck` | pl51.c, pl01.c, pl39.c, pl14.c, pl10.c | matched C, linked |
| local-hunter gates | `Pl_master_ck`, `pl_sw_set`, `basic_com_ck`, `pl_dm_value_sub` | matched / pl_nm.c near-match | linked |
| monster ownership / sends | `Em_Master_Change`, `net_act_set`, `em_type_act_set`, `em_cmd_em_master_ck`, hate | em_master_nm.c, em_core_nm.c, em_cmd_nm.c (near-matches) | linked, never run online |
| monster send / receive | `net_send_em`, `net_receive_em`, `net_emact_set`, `net_receive_em_act` | netsyn10.c, netsyn11_nm.c, netsyn03.c | not compiled; weak no-ops in rt_em.c |
| sys / quest sync | `net_send_sys`, `net_receive_sys`, `net_start_ck`, `Quest_net_sub` | netsyn08.c, netsyn09.c, netsyn07_nm.c, f_quest_nm.c | send no-op, receive not compiled, `net_start_ck` returns 0 |
| supply box / chat | `net_send_host` / `_receive_host`, `net_send_chat` / `_receive_chat` | netsyn05.c, netsyn06*.c | stubs / not compiled |
| session pump | `AQ_*`, `CngNetAQ*`, `mcsls_*` | aq_nm.c, cng*.c, aqcmd*.c, mcsls_*.c | not compiled (replaced, see above) |
| session loop | `Game_task` steps 1-3 | f_game_nm.c | the PC has its own flow (rt_boot.c / rt_flow.c) |

### Estimates

* **M1, two players see each other walk on a quest stage**: about one agent session. Transport (TCP, host relays),
  flags, slot set-up from the host's choice, link netsyn01/02/03, draw the remote hunters, a headless two-instance
  test. Monsters run separately on each machine.
* **M2, four players**: small once M1 works (the slot code is generic): about half a session, mostly the test and
  the joiners' start sync.
* **M3, a full hunt to clear with shared monster health**: several sessions (estimate 3-6). Needs: true global slots
  (each machine's local hunter in its own slot, so monster owner ids and the host slot agree everywhere; the PC glue
  assumes the local hunter is slot 0 in many places), `net_send_em` / `net_receive_em` and the ownership hand-over,
  sys kinds 2-6 and `net_start_ck` (start sync and the network delay), quest clear / fail / timer (`Quest_net_sub`),
  the host-arbitrated supply box, remote weapons drawn, and testing the near-match monster code on paths it has never
  run (risk: they were matched for size, not tested online).

## 2. Research

Web pages were read as data. Facts that are only on the web are marked with their source; facts read from the
game are marked "(game)" and say where.

### 2.1 How MH Oldschool works for PS2 players

| Fact | Source |
|---|---|
| MH Oldschool runs private servers for the NTSC-J versions of MH1, G, G Korean and MH2. US and EU versions use "a different server structure" that has not been worked out. | https://mholdschool.com/viewtopic.php?t=121 (also RESEARCH.md) |
| Players connect by changing the console's (or PCSX2's) **Primary DNS to 34.75.107.68** in the game's own network settings (Add Setting, hardware, IP Auto, DNS Manual); the secondary DNS can stay empty. No patch, no DNAS patch and no port setting is asked of the player. Works on real PS2s and in PCSX2. NTSC-J discs only. | https://mholdschool.com/viewtopic.php?t=120 |
| The DNAS hosts are answered by MH Oldschool too: the guide's troubleshooting lists DNAS errors -611 (DNS server not reachable, often routers / ISPs blocking redirected DNS; VPNs do not always help), -610 (SSL handshake, retry) and -402 (disc authentication, burned discs). So the stock DNAS libraries run on the player's console and talk to an MH Oldschool host; the "bypass" is done on the server side by DNS redirection, not by patching the game. | https://mholdschool.com/viewtopic.php?t=121 |
| Their DNS software is public: `MH-Oldschool/mhosdns`, Python, **GPL-3.0**, "a local DNS server pre-configured to connect to all of our infrastructure, including game and DLC servers". Its `domains.conf` maps host patterns to addresses (below). | https://github.com/MH-Oldschool/mhosdns |
| The original service ran on KDDI's Multi-Matching BB; event quests were implemented server side by Dec 2024. | RESEARCH.md; https://mholdschool.com/viewtopic.php?t=120, ?p=3355 |

Host patterns in `mhosdns/domains.conf` (public GPL-3.0 configuration; copied here only as facts about where the
client's names go; **do not connect to them**):

| Host pattern | Address |
|---|---|
| `*.dnas.playstation.org` | 34.75.107.68 (the same address as the public DNS) |
| `*.kddi-mmbb.jp` | 151.80.238.101 |
| `corsair`, `skyhawk`, `viper`, `crusader`, `raptor`, `strike-raptor` `.capcom.co.jp` | 151.80.238.99 |
| `goshawk`, `spector`, `meteor`, `voodoo` `.capcom.co.jp` | 151.80.238.104 |

Reading: `*.kddi-mmbb.jp` is the Multi-Matching BB portal (web pages, HTTPS), and the aircraft-named `*.capcom.co.jp`
hosts are the original lobby / game servers. The game (game, `lobby.bin` strings) contains the portal URL
`https://www01.kddi-mmbb.jp/00000008/CRS-top.jsp`, the browser user agent `KDDI/Multi-Matching-BB_Browser` and
`mmbb://` commands such as `GAME_MENU_`, `REGIST_CHECK?`, `BB_DOD_KIYAKU`, `BACK_TO_MENU`. **The ports are not in any
document found**: the client reads `host:port` of each lobby server from the portal's page (section 3.2), so a DNS
redirect is enough for PS2 players. The port the MH Oldschool lobby servers listen on is therefore unknown to us
(question 1 in the message to the operators).

Not found: any description of MH Oldschool's server software, protocol or source. Their GitHub organisation holds
`mhosdns` (GPL-3.0), `MHSaveRepair` (C#, GPL-3.0), `MHMultitool`, and forks of MH2 quest tools; no game server.
(https://github.com/MH-Oldschool, read 7 Oct 2026).

### 2.2 Open-source servers and protocol documentation

* **No open-source MH1 or MHG server or protocol document was found** (searches 7 Oct 2026; MH Oldschool's organisation
  has none). The protocol in section 5 is therefore derived from the game itself.
* Related but not usable as a source for MH1: `sepalani/MH3SP` (Monster Hunter 3 / Tri for Wii, Python, **AGPL-3.0**,
  https://github.com/sepalani/MH3SP): a different game and a different protocol; useful only as an example of how such
  a private server is organised. `MH-Oldschool/mhosdns` (GPL-3.0) is DNS only.
* Licences matter for reuse: GPL / AGPL code must not be copied into this repository's tools unless the whole
  distribution is compatible; nothing from these projects was copied.

### 2.3 What of the game's network stack is decompiled, and what is not

The online game is a stack; the boundary between "Capcom C we decompile" and "library we do not" sits low.

| Layer | Where (SLPM_654.95 / overlays) | Status |
|---|---|---|
| Screens and flow: login, top menu, plaza, lobby, rooms (`lbc_*`, `Lbs_*`, `lb_cli.c`); network menu (`ms*`, `netwk*`, `ncmreq`, `netbgm`) | lobby.bin, main | decompiled (matching or near-match) |
| **Lobby-server client** (`cnLBS_*`, `__cnet_*`, `_cnet_RecvFromLbs_*`, 0x5A2A20-0x5AE8CC in lobby.bin) and its socket helpers `tcp_init`, `connect_ps2`, `select_ps2` (0x5B53D0-0x5B5974) | lobby.bin | decompiled; this is the PS2 wire protocol |
| In-game session between players (`mcsls_*`, `Cng*`, `CnInet*Mcs*`, `aqcmd`) | main 0x22C670-0x23A000 | decompiled, **not linked** in the online PC build yet |
| Capcom's network wrappers `CpInet*` (TCP, DNS, PPP, DHCP, route, device), `netdev*`, `Ave_*` | main | decompiled; thin. `CpInet*` is where the port plugs in |
| The `Ave_*` layer: argument packing and `sceSifCallRpc` to the IOP RPC server **0x01270030**, which is a Capcom wrapper IOP module (`Capcom_AVE_TCP_Wrapper`, `AVEWRAPM.IRX` for modem, `AVEWRAPE.IRX` for Ethernet) in front of **AVETCP.IRX** (TCP/IP), **AVEPPP.IRX**, **AVEDHCP.IRX** | IOP | the Ave wrappers on the EE side are decompiled; the IRX modules are third-party / closed and **not decompiled** |
| Network drivers: SMAP (Sony), AN986, LANEGG, MINIJPS2, ATERMAS / ATERMCS, CXTMDM, OSTMDM_A, USBSRCH, DEVGLUE, LOADHIGH, DEV9, USBD | IOP | not decompiled, not needed |
| Connection settings: `sceNetcnfif*` (Sony libnetcnfif, linked into the yn overlay), with Capcom's `yn_netcnf_*` and `yn_hard_*` on top | yn.bin | library not decompiled; the PC replaces it with an empty store |
| HTTP / HTTPS and SSL for the in-game web browser: `sceHTTP*`, `sceHTTPS*`, `sceNetGlue*` (BSD socket glue under it) | lobby.bin, main | Sony libraries, not decompiled. The owner paused the browser (DECISIONS.md) |
| **DNAS**: `sceDNAS2*` in the `dnas_net.bin` / `dnas_ins.bin` overlays and the IOP image `DNAS280.IMG`; Capcom glue `InetDNAS2ConnectNetStart`, `InetDNAS2ConnectInstall`, `InetDNAS2ConnectGetError` | overlays | Sony; not decompiled; replaced by `net_dnas.c` |
| Modem: `Modem_device_list`, PPP dial scripts (`PppAtScript0/1`), `CpInetPpp*` | main | not needed on the PC |

#### Library entry points the game's network code calls

**1. `CpInet*` (Capcom, main) -> the PC backend implements these.** Used by the linked online code (`src/pc/net/net_cpinet.c`):
`CpInetInitialize`, `CpInetTerminate`, `CpInetTcpInitialize`, `CpInetTcpTerminate`, `CpInetTcpOpen(&{addr, port, lport})`,
`CpInetTcpGetStatus(sock, st)`, `CpInetTcpRecv`, `CpInetTcpSend`, `CpInetTcpClose`, `CpInetTcpAbort`, `CpInetTcpDelete`,
`CpInetTcpGetOption`, `CpInetTcpSetOption`, `CpInetDnsInitialize`, `CpInetDnsDispose`, `CpInetDnsGetTicket`,
`CpInetDnsLookUp`, `CpInetDnsReleaseTicket`, `InetIPAddrFromString` (dotted quad parser, asm-only on the PS2),
`CpInetGetStatus`, `CpInetInterfaceProblem(Enable)`, `CpInetInterfaceGetStatus`, `CpInetTcpConnected`,
`CpInetTcpNbCallEnd`, `CpInetDevChanged`, `CpInetDevSelect`, `CpInetPppGetStatus`, `CpInetHttpInitialize`.
Not implemented (PPP / DHCP / route / device set-up, used only by the not-linked connect state machine `prot_00/01`):
`CpInetPpp*`, `CpInetNdg*`, `CpInetDhcp*`, `CpInetRoute*`.

Semantics, read from the callers (`select_ps2`, `tcp_init`, `CpInetTcpSelect2`, `InetDnsGetIPAddress`):
* `TcpOpen` returns a handle >= 0 (< 0 error) and connects in the background; the address is the PS2's `u32`
  (first octet in the low byte), the ports are already in network byte order (`tcp_init` swaps them).
* `TcpGetStatus(sock, s16 st[4])`: `st[0]` state (4 = established, 2 connecting, 0 closed, 11 error), `st[2]` free send
  space (`select_ps2` wants > 0x400), `st[3]` bytes waiting to be read.
* `TcpRecv(sock, buf, len)`: bytes copied, 0 if nothing yet, < 0 on error; at most 0x3CA per call (one RPC transfer).
* `DnsGetTicket(name)` -> ticket; `DnsLookUp(ticket, &ip)`: 1 done, -3 pending, -2 / -1 error; `DnsReleaseTicket`.

**2. The RPC layer under it (not used by the port; documented because the Xbox or a faithful emulation may want it).**
`Ave_*` call `sceSifCallRpc` on server 0x01270030 with these command numbers (arguments at offset 0x1C of the 1 KB
`SifRpcWork` buffer, result `s16` at 0x18): 1 Ifconfig, 2 Ifdown, 3 SetOpt, 4 GetOpt, 5 TcpInitialize, 6 TcpTerminate,
7 TcpOpen, 0xB Close, 0xD Abort, 0xF Stat, 0x10 Send, 0x13 Recv, 0x16 Delete, 0x1E / 0x1F RouteAdd / Del, 0x21-0x27 Ppp
(Init, Disp, Start, Finish, Status, GetUsbDeviceId, SetOption), 0x29-0x2D Ndg (Init, Disp, Start, Polling, Finish),
0x2E-0x3A Dhcp (Init, Disp, GetDns 0x30, GetGateway 0x31, RequestNb 0x36, GetIfInfo 0x37, Timer 0x38, ReleaseNb 0x3A),
0x3B-0x3F Dns (Init, Disp, GetTicket, ReleaseTicket, LookUp), 0x46-0x4A device (GetDeviceInfoNum 0x46, GetDeviceInfo 0x47,
SelectDevice 0x48, GetOption 0x4A).
Why the port does not emulate this RPC: the matching C of `cpinet*.c` and `cnlbs*.c` often calls a function with fewer
arguments than the callee reads (the PS2 leaves the value in the argument register, `tools/pc_patch.py` explains), so
the layer has to be re-written or patched anyway, and `CpInet*` is the smallest clean API above the IOP.

**3. Sony library calls** around it: `sceSifBindRpc`, `sceSifCallRpc`, `sceSifCheckStatRpc`, `sceSifLoadModule`,
`sceSifLoadStartModule`, `sceSifStopModule`, `sceSifUnloadModule`, `sceSifSearchModuleByName` (the game loads the IRX
network modules itself in `DeviceLoadDriver*`), `sceDevctl` (device status), `sceCdPowerOff`; semaphores and threads.
Not needed on the PC (no IOP).

**4. `sceNetcnfif*`** (yn overlay): `Init`, `Setup`, `Term`, `Sync`, `Check`, `GetResult`, `AllocWorkarea`,
`FreeWorkarea`, `SetFNoDecode`, `SetEnv`, `GetCount`, `GetList`, `LoadEntry`, `LoadEntryAuto`, `AddEntry`, `EditEntry`,
`DeleteEntry`, `DeleteAll`, `SetLatestEntry`, `CheckCapacity`, `CheckAdditionalAT`, `CheckSpecialProvider`, `GetAddr`,
`ConvAuthname`, `DataInit`, `DataDump`. Convention (from `yn_netcnf_init1/2`, `get_num`, `get_list`): a request returns
>= 0 when accepted, the caller polls `Check()` until 0, then `GetResult(&res)`. `net_netcnf.c` completes everything at
once and keeps no entries.

**5. DNAS**: only the three Capcom entry points are reached from game C: `InetDNAS2ConnectNetStart(&state, &x06,
&progress)` (called every frame by `ms_network_bb_authentication`), `InetDNAS2ConnectInstall`, `InetDNAS2ConnectGetError`.
Read from the asm of `InetDNAS2ConnectCore` (dnas_net.bin 0xA769E0): a state machine on the byte `*state`; when it
reaches 0x7F it sets `*state = 0`, raises `*progress` to 100 and returns **1**; 0 while working; **-1** on failure (the
error code is then in `InetDNAS2ConnectGetError`). The community's DNAS-bypass behaviour (the game sees "authenticated")
is exactly "return 1 at once", which `net_dnas.c` does. The internal `sceDNAS2*` calls (`AuthNetStart`, `AuthGetUniqueID`,
`GetStatus`, `Abort`, `Shutdown`, ...) are never reached.

**6. HTTP / SSL** (`sceHTTP*`, `sceHTTPS*` with certificate loading, `sceNetGlue*`): used by the in-game browser
(`Bs*`, `CpInetHttpInitialize`). Not ported; see section 7.

### 2.4 How the original connects (read from the code; the web part is untested)

1. Network settings (netcnf, yn overlay) and device / IOP module loading (`DeviceLoadDriver*`, `prot_00/01` state machines).
2. DNAS authentication (`ms_network_bb_authentication`).
3. The portal: the in-game browser loads Multi-Matching BB pages over HTTPS and delivers a server table into the work
   buffer `bsCsvWork` (first 16 bytes = the password the client sends in the login, later fields = per-server id,
   `host:port`, current / total users, see `lb_tcp01.c`, `create_server_table.c`). The key in the login
   (`D_3A3B71`, 10 characters) is the MMBB id.
4. `tcp_init`: finds the chosen server's `host:port` in that table, resolves the name, opens the TCP connection.
5. The lobby-server protocol (section 5): login, plaza / lobby / room lists, chat, mail, matching.
6. For a match: the lobby tells both clients a game server address (`cnLBS_Get_GameServerAddress`); the clients open TCP to
   it and run the `mcsls` session protocol (records relayed by the server; section 1a).

## 3. The port

### 3.1 Design

* The boundary is `CpInet*` (above). The game's `cnet` / `lb_tcp01` / `select_ps2` C runs unchanged on the PC (with
  the argument fixes in `tools/pc_patch.py`: the completion callback of `cnLBS_Send_UserMiniData`, `cnLBS_SendMessage`,
  `cnLBS_RoomEntry`, the argument of `cnLBS_Answer_LoginWarningMessage` and `cnLBS_Send_ChatBinary`; found with
  `tools/argregs.py --check`).
* `tools/build_pc.sh` with `ONLINE=1` adds `src/lobby/cnet/*.c`, `lb_bz20.c` (`init_select_flags`), `lb_c509.c`
  (`select_ps2`), `lb_bz81.c` (`connect_ps2`), `lb_tcp01.c` (`tcp_init`), `main/net/netdev01.c`, `netdev17.c`
  (`InetDnsGetIPAddress`), `cnlbs01-03.c`, the three backend files and `src/pc/rt/rt_net.c`; output
  `build/pc/mhview_online`. A plain build links none of it.
* The Xbox port will use the same `CpInet*` boundary with nxdk's lwIP / BSD sockets (`net_cpinet.c` has a Winsock path;
  not compiled or tested yet).

### 3.2 Configuration and safety

* Target: `RT_NET_HOST` (default `127.0.0.1`), `RT_NET_PORT` (default 10200). There is no default that names any
  public server. The test builds the one-entry server table the portal would deliver.
* `net_cpinet.c` refuses, with a message and without opening a socket: the MH Oldschool addresses (34.75.107.68,
  151.80.238.99 / .101 / .104), and everything that is not loopback / RFC 1918 / link-local unless `RT_NET_ALLOW_PUBLIC=1`.
  `tools/test_online.sh` checks three refusals. When the operators agree, the deny list is the one line to change.
* `tools/mh1_testserver.py` binds loopback by default and refuses a public bind address without `--allow-public`.

### 3.3 Files

| File | |
|---|---|
| `src/pc/net/net_cpinet.c/.h` | sockets, DNS, interface stand-ins |
| `src/pc/net/net_dnas.c` | DNAS success |
| `src/pc/net/net_netcnf.c` | empty connection-settings store |
| `src/pc/rt/rt_net.c` | `--nettest SCENARIO` driver: `connect`, `login`, `top`, `plaza`, `lobby`, `full`; env `RT_NET_HOST`, `RT_NET_PORT`, `RT_NET_HOLD_MS`, `RT_NET_CHAT` |
| `src/pc/viewer.c` | `--nettest` option (only under `MH1_ONLINE`) |
| `tools/mh1_testserver.py` | the test server |
| `tools/test_online.sh` | the test |
| `src/pc/net/net_peer.c/.h` | co-op transport: host / join, the host relays (3.4) |
| `src/pc/rt/rt_np.c` | co-op glue: options, slots, `AQ_data_put`, received packets -> `net_receive_pl`, `Online_ck` |
| `tools/test_coop.sh` | the co-op test (2 and 4 instances) |
| `tools/lbs_cmdtab.py` | prints the lobby-server command table from the user's disc (`disc/mh1/split/lobby.bin`) |
| `tools/pc_patch.py` | argument fixes for the cnet files |

### 3.4 Co-op over direct connect (agent E, 7 Oct 2026, round 2)

Milestones M1 (two players see each other walk on a quest stage) and M2 (four players) are reached, tested headless on
127.0.0.1. M3 (a whole hunt with shared monster health) is not started. Why it is built this way: section 1a.

Run it (ONLINE=1 build only):

    build/pc/mhview_online disc/mh1 --host --quest 131 --players 2      # host: waits for 1 more player, then starts
    build/pc/mhview_online disc/mh1 --join 192.168.1.20                  # a joiner, by the host's LAN address

| Option | |
|---|---|
| `--host [IP]` | host the quest; listens on 127.0.0.1, or on the given private LAN address of this machine. Without `--quest` it prints the Elder's quests on the console and asks for a number |
| `--join IP` | join the host at IP (loopback or private addresses only; MH Oldschool and public addresses are refused) |
| `--port N` | TCP port, default 10300 (both sides) |
| `--players N` | host: start when N players (2-4) are in; after `RT_NP_WAIT` seconds (default 300) it starts with whoever came |
| `RT_WEAPON=id` | each player's weapon (Ken_data id), sent to the others at joining |
| `RT_NP_TRACE=1`, `RT_NP_POS=n` | test aids: every packet; every player's position, angle and action every n ticks |

What happens:

1. `net_peer.c`: joiners open one TCP connection to the host and send their weapon. The host gives each joiner the next
   slot (1-3, joining order), then sends everyone the quest number, the player count and every slot's weapon. After that the
   host passes each frame on to the other joiners and hands it to its own game (as Capcom's game server relayed records).
   Frames: `u16 n, u8 sender slot, u8 type`, then the game's packet; type 1-10 is the AQ channel.
2. `rt_np.c`: `rt_player_game_init` -> `rt_np_init_slots` does what `Game_task` step 3 and `init_pl_work` do online:
   `game_w.master` = this machine's slot, `pl_num`, `pl_state[]`, the other slots' player works (`be_flag`, `id`, stage,
   weapon), the network delay `game_w+0x1B0` = 2 frames, and switches `Online_ck()` on. `pl_init` then initialises every
   slot.
3. Sending is the game's own: `net_send_pl` (netsyn01.c) builds the packet and calls `AQ_data_put`, which on the PC goes to
   the transport. The local hunter sends kind 3 at start, kind 1 on every action change, kind 2 every 20 ticks
   (`System_timer % 20 == slot`; the PC now counts `System_timer` during a co-op quest).
4. Receiving: each tick before the players move, `rt_np_tick` takes the frames and calls the game's `net_receive_pl`
   (netsyn02_nm.c) for channels 1-4, as `self_data_ctrl` does. The remote hunter then replays the action (`Pl_act_set`),
   turns toward the sent angle and slides to the sent position (`Pl_adj_calc` / `Pl_pos_adj`).
5. The viewer draws the other hunters on this player's area (`remote_hunters`), posed by the game's motion player, and
   gives the game C their joint matrices. The local hunter is `player_work[game_w.master]` (`lp` in viewer.c), no longer
   always slot 0.
6. A co-op quest runs at 30 ticks a second also headless (otherwise test instances run at different speeds). A player
   whose connection ends gets `pl_state = 0xFF` and is hidden (the host tells the others).

Tested (`tools/test_coop.sh`, ~25 s): 2 and then 4 headless instances on quest 131, each walking a different scripted
path, one with a great sword; at the end every instance sees every player at the position that player has itself
(0 units off), on the same stage; screenshots `build/show/coop_N_slotK.png` (the HUD shows the party's bars and map
arrows, the other hunters are drawn). Also: a joiner leaving early, and refusals of `--join 8.8.8.8`, `--join 34.75.107.68`
and `--host 8.8.8.8`. While a hunter runs and turns, the others see it up to a few hundred units off for up to 20 ticks
(only kind 2 corrects the position); this is how the PS2 code works too, not measured against a PS2.

Not done yet (M3 and polish), in the order they matter:
* **Monsters are not shared**: each machine runs its own. `Em_Master_Change` already runs (pl_num > 1) and hands monsters
  between slots, but `net_send_em` / `net_receive_em` (netsyn10.c, netsyn11_nm.c) are not linked: next step. Same for
  channel 10 (`net_send_sys` / `net_receive_sys`: quest clear / fail / timer / kills, sys kinds 2-6) and channel 7 (the
  host's supply box). Those packets are already carried; `rt_np_tick` counts them as unused.
* The other hunters' **look** is the default armour (no mini data is exchanged; the host's table carries only the weapon),
  and their **weapon is not drawn**.
* Area changes: a hunter on another area is hidden; switching areas has not been tested with two players.
* Connection UI: command line plus the console quest list; no window. The Windows build is untested (Winsock code is in
  `net_peer.c`).
* Results after the quest (reward screen, back to the village) are each machine's own.

## 4. Run it

    ONLINE=1 tools/build_pc.sh                       # build/pc/mhview_online
    tools/test_online.sh                             # server + clients, ~12 s
    python3 tools/mh1_testserver.py -v --port 10200  # by hand
    RT_NET_PORT=10200 build/pc/mhview_online disc/mh1 --nettest full

## 5. The lobby-server protocol (PS2 wire format, derived from the client)

All of it is read from the decompiled client (`src/lobby/cnet`, `lb_c509.c`) and the data tables of `lobby.bin`,
and checked by the test server against that client. Where the real server may differ it is said. **Nothing here was
checked against an MH Oldschool or Capcom server.**

### 5.1 Framing

TCP stream of packets. Every packet, both directions, has a 12 byte header followed by `len` bytes:

    0      magic: 0x81 (0x82 for the in-game "Mcs" session packets)
    1      category: 1 request, 2 answer, 16 (0x10) notice
    2-3    command code (big endian), e.g. 0x6203
    4-5    payload length (big endian), at most 0x300 from the client
    6-7    sequence number (big endian)
    8      result: 0 ok; non-zero = error, the payload is then a server message string
    9-11   0xFF 0xFF 0xFF

* Numbers in payloads are big endian (`GetRecvData16/32`, `SetSendData16/32`).
* Strings from the server: `u16 length` + bytes (`GetRecvDataString` / `GetRecvDataOption3`, which truncates to a
  maximum: ids 8, handles 16, mini data 0x40, chat 0x100, server message 0x300).
* A packet longer than the client's 0x600 buffer arrives in chunks; the client treats a chunk as final when its
  receive step returns 1 (`select_ps2`); top-level lists in practice fit.
* Sequence numbers: the client numbers its requests with a 16-bit counter that starts at 1; the server answers a
  request with the **same sequence number** (the client matches answers to its background slot by it); the client
  answers a server request (category 1) with the server's number.
* Which code goes with which direction and category is in the table in the appendix (generated by
  `tools/lbs_cmdtab.py` from `lbs_command_tbl_h/l`, `lbs_category_tbl`, `lbs_fromto_tbl`, `lbs_command_jmp`).
  `fn = 0` means the client never receives that row.

### 5.2 Obfuscated strings (client to server)

Secrets and names the client sends are not plain: `SetSendStringData2` writes `u16 len+2`, `u16 sum` (the byte sum of
the plain text & 0x7FFF), then `len` bytes with `out[i] = in[i] XOR "LOCKROCK"[i & 7] XOR ((xfee + (seq & 0xFF) + i) & 0xFF)`,
where `xfee` is the 16-bit value the server sent in its first packet and `seq` the packet's sequence number. The 8-byte
table is a constant in the game (`encrypt_str`). The test server decodes with this and gets the password back
("LOCALTEST0000000"), which confirms the reading.

### 5.3 Login conversation (verified with the test server)

| Dir | Code, cat | Payload |
|---|---|---|
| S->C | 6101 req | `u16 xfee` (first packet after the TCP connect) |
| C->S | 6101 ans | `u16 10`, 10 ASCII digits (two 5-digit numbers = `seq` + the digits in `key[0:4]` and `key[4:8]`, `mmbbc_encode`), obfuscated password (16 chars) |
| S->C | 6102 req | none (telephone number) |
| C->S | 6102 ans | obfuscated telephone string (empty on the PC) |
| S->C | 6103 req | none (first data) |
| C->S | 600E req x4 | obfuscated "0": an echo test the client does when `login.x00 != 0`; S->C 600E ans each time; the client averages the round trip into the first data |
| C->S | 6103 ans | 3 bytes (0, 1, 4 = client / protocol kind; 4 means "MH": join counts are three `u16`), obfuscated 10 byte version string, 8 `u16` (device status, ping) |
| S->C | 6105 req (optional) | warning message (`loginbuf`), client answers 6105 with `u8 ok` |
| S->C | 6131 notice | `u8 n`, then n x (`str id`, `str handle`, `str mini[0x40]`): the hunters of this account; n = 0 means a new hunter |
| C->S | 6132 req | obfuscated id (6 chars, or "******" for a new hunter), obfuscated handle |
| S->C | 6132 ans | `str id` (the id the server gave, 6-8 chars); non-zero result = rejected, `str message` |
| S->C | 6104 notice | none: login ok (the client's login callback gets id 0) |
| C->S | 6190 req | obfuscated mini data (0x40 bytes, the hunter's public data); S->C 6190 ans empty |
| C->S | 6141 notice | none: login finished |
| C->S | 614C req | none (top information); S->C 614C ans: `u8 level`, `str html` |

Callback ids of `CallBack_Result_LoginLobbyServer` (lb_cli.c), for reference: 0 login ok, 1 account list, 2 account
accepted, 3 patch, 4 warning message, 5 regulation agreement, 7 / 8 errors with a server message.
Other server requests the client handles but the test server never sends: 6110 regulation version, 6121-6125 patch
(start, data, footer, line check, finish), 6138 battle result, 6111 regulation address.

### 5.4 Plazas, lobbies, rooms ("pieces") (verified for plazas and lobbies)

The client asks the same things for the three kinds; the codes differ:

| | count | name | join users | status | explanation | enter | leave |
|---|---|---|---|---|---|---|---|
| plaza | 6203 | 6204 | 6205 | 6206 | 620A | 6207 | 6306 |
| lobby | 6301 | 6302 | 6303 | 6304 | 6308 | 6305 | 6408 |
| room | 6401 | 6402 | 6403 | 6404 | 640D | 6406 | 6501 |

Requests carry `u16 id` (1-based; count and leave carry nothing). Answers (the same code, category 2, the request's
sequence): count `u16 n`; name `u16 id`, `str`; join `u16 id`, `u16 joined`, `u16 max` (three values because the client
declared kind 4); status `u16 id`, `u8 status` (3 = open; the top menu picks the first plaza with status 3);
explanation `u16 id`, `str`; enter / leave: empty. Notices with the same codes (category 16) push changes.
`cnLBS_Read_PlazaAllocation(0, 7, cb)` / `Read_LobbyAllocation` / `Read_RoomAllocation` are multi-request jobs in the
client (count, then name + status + join users for each, bit mask 7); their callback is called with result 2 as a
progress report and 0 at the end.

Also verified: current place 6891 req -> ans three `u16`; lobby member list 630A req (`u16 lobby id`) -> ans `u16 0`,
`u8 3`, `u8 n`, n x (`str id`, `str handle`, `str mini`); lobby commer 6411 / leaver 6410 notices (`str id`, `str handle`,
`str mini` / `str id`); chat: client 6701 notice (obfuscated text, `u8`), server 6702 notice (`str from`, `str handle`,
`str text`, 4 bytes); logout 6002 req -> 6002 ans.

Read from the code but **not exercised** by the test: rooms (create 6407, rules 6601-660F, set name / password / rule /
finish, entry 6406, member 640A, join info 640B, property 650A / 6509, match entry 6504, match entry users 6412 / 6506,
match start 6508 -> 6910, join / sides / rules / opponent / battle code / game server address 6911-6917), mail and
user search (6703-6709), condition search, personal data (6181-6189), file download (6881 / 6882), patches (6121-6125),
the shutdown / line check codes (6001-6007).

### 5.5 The in-game session (`mcsls`, not linked)

After matching the clients connect by TCP to the game server address (`cnLBS_Get_GameServerAddress`). The server sends
0x1031 and the client answers once with a 0x82 packet (`cmcs_04`); after that the stream is `mcsls` records
(`[u8 len][u8 cmd<<4 | sender]`), which the server relays to the other members. Section 1a has the details (an
earlier version of this section called the whole session "0x82 packets", which was wrong).

## 6. Test results (7 Oct 2026)

`tools/test_online.sh`: passes. The game's client logs in, reads top information and the plaza and lobby tables,
enters a plaza and a lobby, lists the members, and in the two-client run is told that another hunter came in, hears
chat and sees the other leave.

## 7. What is missing, and ideas

1. **The lobby overlay running online** (`Online_ck() == 1`): the town stages, plaza / lobby screens and the login
   screens (`lbc_*`) on top of this layer; `rt_lb_reload` resets lobby data (including `CnetSys_w`) on every village
   entry, which has to be taken into account.
2. **The portal**: HTTPS (SSL with Sony's certificate handling), the MMBB login, the HTML pages and the `mmbb://`
   commands. Idea: replace the pages with a native dialog that delivers the server table and the id / password,
   so the DNS redirect to MH Oldschool is the only dependency (the owner may have another plan, see DECISIONS.md).
3. **Rooms, matching and the in-game session** (`mcsls`, magic 0x82), patch download, regulations, personal data,
   mail, user search, file (quest) download: formats are in the code, not in the test server.
4. **DNAS**: the bypass is local. If MH Oldschool's server still expects the DNAS traffic from a console (their DNS
   answers `*.dnas.playstation.org`), a port that skips it must be acceptable to them.
5. Winsock / Xbox (nxdk) builds of `net_cpinet.c`: written, untested.
6. The real servers may require things the client does not show (heartbeats, `6001` line checks, 6007 lobby full).
   The test server is lenient (checks no checksums, accepts any password).

## 8. Draft message for the MH Oldschool operators

(For the owner to send. It does not connect anything by itself.)

> Subject: PC / Xbox port of Monster Hunter (NTSC-J): asking permission before connecting to MH Oldschool
>
> Hello MH Oldschool team,
>
> I am working on a hobby preservation project that ports the first Monster Hunter (PS2, NTSC-J) to PC and to the
> original Xbox, from the owner's own disc: https://github.com/itsbigcforme67/mh1-xbox (the code is decompiled from
> the game; no Capcom data is included, so players need their own disc). Thank you for keeping the PS2 games alive; the online part of the port depends on your servers.
>
> The port's network code is meant to behave exactly like a PS2 client on the wire (the game's own lobby-server
> client, compiled for PC), with the DNAS step skipped locally. So far I have only tested against a small private
> test server I wrote on my own machine. **Nothing in the project has connected to your servers and nothing will
> until you agree.**
>
> I would like to ask:
> 1. May a PC / Xbox build of the game, started from a legitimate NTSC-J disc, connect to your public servers? Under
>    what conditions (rate limits, a way to identify the port in your logs, a request to keep it off event
>    quests, etc.)?
> 2. Should the port reach you by changing DNS (as PS2 players do, 34.75.107.68), or would you rather give it direct
>    host names / addresses? Which ports do the lobby servers listen on (the game reads `host:port` from the
>    portal page)?
> 3. The PS2 flow goes DNAS (`*.dnas.playstation.org`) -> portal (`*.kddi-mmbb.jp`, HTTPS) -> lobby server ->
>    game server. The port does not run Sony's DNAS libraries. Is it acceptable to skip DNAS from the client side, or
>    does your DNAS host need to see the usual exchange? Is anything on your side keyed to the console (for example
>    the disc id or a console id that a PC build would not have)?
> 4. The port needs the portal's server table and login. Are the portal pages documented anywhere, or may I read what
>    your portal sends so the port can show a native menu instead of the web browser?
> 5. Is there any protocol documentation, or a description of how your servers differ from the original Capcom ones,
>    that you are willing to share? I am happy to share what I learned from the client (lobby protocol notes in
>    docs/network.md) in return, and to keep any answer private if you prefer.
> 6. Who should I contact if the port misbehaves toward your servers?
>
> Thank you, and no hurry.

## Appendix: lobby-server command table

Generated by `python3 tools/lbs_cmdtab.py` from the user's own `lobby.bin` (names from `docs/survey/mh1_symbols.csv`,
senders from `src/lobby/cnet`). `idx` is the internal command number the client code passes to `SetSendCommand`;
`code` is the wire code; the handler column is what the client runs when it receives that (code, category).

| idx | code | cat | server->client handler | client->server sender |
|---|---|---|---|---|
| 0 (0x00) | 6001 | request | _cnet_RecvFromLbs_RequestLineCheck |  |
| 1 (0x01) | 6001 | answer |  | __cnet_SendSet_LineCheck |
| 2 (0x02) | 6002 | request |  | __cnet_SendSet_Logout |
| 3 (0x03) | 6002 | answer | _cnet_RecvFromLbs_AnswerLogOut |  |
| 4 (0x04) | 6006 | request |  | __cnet_SendSet_ShutDown |
| 5 (0x05) | 6006 | answer | _cnet_RecvFromLbs_AnswerShutDown |  |
| 6 (0x06) | 6003 | notice | _cnet_RecvFromLbs_NoticeShutDown |  |
| 7 (0x07) | 6004 | notice | _cnet_RecvFromLbs_NoticeShutDownOpponent |  |
| 8 (0x08) | 6005 | notice | _cnet_RecvFromLbs_NoticeMatchCancel |  |
| 9 (0x09) | 6007 | notice | _cnet_RecvFromLbs_NoticeLobbyFull |  |
| 10 (0x0A) | 600E | request |  | __cnet_SendReq_EchoPacket |
| 11 (0x0B) | 600E | answer | _cnet_RecvFromLbs_AnswerEchoPacket |  |
| 12 (0x0C) | 6101 | request | _cnet_RecvFromLbs_RequestConnectionPair |  |
| 13 (0x0D) | 6101 | answer |  | __cnet_SendSet_ConnectionPair |
| 14 (0x0E) | 6102 | request | _cnet_RecvFromLbs_RequestTelephoneNumber |  |
| 15 (0x0F) | 6102 | answer |  | __cnet_SendSet_TelephoneNumber |
| 16 (0x10) | 6103 | request | _cnet_RecvFromLbs_RequestFirstData |  |
| 17 (0x11) | 6103 | answer |  | __cnet_SendSet_FirstData |
| 18 (0x12) | 6104 | notice | _cnet_RecvFromLbs_NoticeLoginOk |  |
| 19 (0x13) | 6105 | request | _cnet_RecvFromLbs_RequestWarningMessage |  |
| 20 (0x14) | 6105 | answer |  | __cnet_SendAns_WarningMessage |
| 21 (0x15) | 6131 | notice | _cnet_RecvFromLbs_NoticeUserId |  |
| 22 (0x16) | 6132 | request |  | __cnet_SendReq_UserID |
| 23 (0x17) | 6132 | answer | _cnet_RecvFromLbs_AnswerUserId |  |
| 24 (0x18) | 6138 | request | _cnet_RecvFromLbs_RequestBattleResult |  |
| 25 (0x19) | 6138 | answer |  | __cnet_SendAns_BattleResult |
| 26 (0x1A) | 6141 | notice |  | __cnet_SendSet_LoginFinish |
| 27 (0x1B) | 6142 | request |  |  |
| 28 (0x1C) | 6142 | answer | _cnet_RecvFromLbs_AnswerBillEstimate |  |
| 29 (0x1D) | 6143 | request |  |  |
| 30 (0x1E) | 6143 | answer | _cnet_RecvFromLbs_AnswerUserBinary |  |
| 31 (0x1F) | 614C | request |  | __cnet_SendReq_TopInformation |
| 32 (0x20) | 614C | answer | _cnet_RecvFromLbs_AnswerTopInformation |  |
| 33 (0x21) | 6190 | request |  | __cnet_SendSet_MiniDataRegist |
| 34 (0x22) | 6190 | answer | _cnet_RecvFromLbs_AnswerMiniDataRegist |  |
| 35 (0x23) | 6191 | notice | _cnet_RecvFromLbs_NoticeMiniData |  |
| 36 (0x24) | 6144 | request |  |  |
| 37 (0x25) | 6144 | answer | _cnet_RecvFromLbs_AnswerPersonalRecordHeader |  |
| 38 (0x26) | 6145 | request |  |  |
| 39 (0x27) | 6145 | answer | _cnet_RecvFromLbs_AnswerPersonalRecordData |  |
| 40 (0x28) | 6146 | request |  |  |
| 41 (0x29) | 6146 | answer | _cnet_RecvFromLbs_AnswerPersonalRecordVide |  |
| 42 (0x2A) | 6202 | notice | _cnet_RecvFromLbs_BothGameJoin |  |
| 43 (0x2B) | 6202 | answer | _cnet_RecvFromLbs_BothGameJoin |  |
| 44 (0x2C) | 6208 | request |  | __cnet_SendReq_TopPageJump |
| 45 (0x2D) | 6208 | answer | _cnet_RecvFromLbs_AnswerTopPageJump |  |
| 46 (0x2E) | 6209 | request |  |  |
| 47 (0x2F) | 6209 | answer | _cnet_RecvFromLbs_AnswerAppointJump |  |
| 48 (0x30) | 6207 | request |  | __cnet_SendReq_PieceEntry |
| 49 (0x31) | 6207 | answer | _cnet_RecvFromLbs_AnswerPlazaEntry |  |
| 50 (0x32) | 6203 | request |  | __cnet_SendReq_PieceCount |
| 51 (0x33) | 6203 | answer | _cnet_RecvFromLbs_AnswerPlazaNumOfPlaza |  |
| 52 (0x34) | 6204 | request |  | __cnet_SendReq_PieceName |
| 53 (0x35) | 6204 | answer | _cnet_RecvFromLbs_AnswerPlazaName |  |
| 54 (0x36) | 6205 | request |  | __cnet_SendReq_PieceJoinUser |
| 55 (0x37) | 6205 | answer | _cnet_RecvFromLbs_BothPlazaJoinUser |  |
| 56 (0x38) | 6205 | notice | _cnet_RecvFromLbs_BothPlazaJoinUser |  |
| 57 (0x39) | 6206 | request |  | __cnet_SendReq_PieceStatus |
| 58 (0x3A) | 6206 | answer | _cnet_RecvFromLbs_BothPlazaStatus |  |
| 59 (0x3B) | 6206 | notice | _cnet_RecvFromLbs_BothPlazaStatus |  |
| 60 (0x3C) | 620A | request |  | __cnet_SendReq_PieceExplain |
| 61 (0x3D) | 620A | answer | _cnet_RecvFromLbs_BothPlazaExplain |  |
| 62 (0x3E) | 620A | notice | _cnet_RecvFromLbs_BothPlazaExplain |  |
| 63 (0x3F) | 6306 | request |  | __cnet_SendReq_PieceExit |
| 64 (0x40) | 6306 | answer | _cnet_RecvFromLbs_AnswerPlazaExit |  |
| 65 (0x41) | 6307 | notice | _cnet_RecvFromLbs_NoticePlazaRemove |  |
| 66 (0x42) | 6305 | request |  | __cnet_SendReq_PieceEntry |
| 67 (0x43) | 6305 | answer | _cnet_RecvFromLbs_AnswerLobbyEntry |  |
| 68 (0x44) | 6408 | request |  | __cnet_SendReq_PieceExit |
| 69 (0x45) | 6408 | answer | _cnet_RecvFromLbs_AnswerLobbyExit |  |
| 70 (0x46) | 6301 | request |  | __cnet_SendReq_PieceCount |
| 71 (0x47) | 6301 | answer | _cnet_RecvFromLbs_AnswerLobbyNumOfLobby |  |
| 72 (0x48) | 6302 | request |  | __cnet_SendReq_PieceName |
| 73 (0x49) | 6302 | answer | _cnet_RecvFromLbs_AnswerLobbyName |  |
| 74 (0x4A) | 6303 | request |  | __cnet_SendReq_PieceJoinUser |
| 75 (0x4B) | 6303 | answer | _cnet_RecvFromLbs_BothLobbyJoinUser |  |
| 76 (0x4C) | 6303 | notice | _cnet_RecvFromLbs_BothLobbyJoinUser |  |
| 77 (0x4D) | 6304 | request |  | __cnet_SendReq_PieceStatus |
| 78 (0x4E) | 6304 | answer | _cnet_RecvFromLbs_BothLobbyStatus |  |
| 79 (0x4F) | 6304 | notice | _cnet_RecvFromLbs_BothLobbyStatus |  |
| 80 (0x50) | 6308 | request |  | __cnet_SendReq_PieceExplain |
| 81 (0x51) | 6308 | answer | _cnet_RecvFromLbs_AnswerLobbyExplain |  |
| 82 (0x52) | 640C | notice | _cnet_RecvFromLbs_NoticeLobbyRemove |  |
| 83 (0x53) | 6407 | request |  | __cnet_SendReq_RoomCreate |
| 84 (0x54) | 6407 | answer | _cnet_RecvFromLbs_AnswerRoomCreate |  |
| 85 (0x55) | 6601 | request |  | __cnet_SendReq_RoomNamePermission |
| 86 (0x56) | 6601 | answer | _cnet_RecvFromLbs_AnswerRoomNamePermission |  |
| 87 (0x57) | 6602 | request |  | __cnet_SendReq_RoomPasswordPermission |
| 88 (0x58) | 6602 | answer | _cnet_RecvFromLbs_AnswerRoomPasswordPermission |  |
| 89 (0x59) | 6603 | request |  | __cnet_SendReq_NumOfRule |
| 90 (0x5A) | 6603 | answer | _cnet_RecvFromLbs_AnswerRoomNumOfRule |  |
| 91 (0x5B) | 6604 | request |  | __cnet_SendReq_RuleListHeadWord |
| 92 (0x5C) | 6604 | answer | _cnet_RecvFromLbs_AnswerRuleListHeadWord |  |
| 93 (0x5D) | 6605 | request |  | __cnet_SendReq_RuleListPermission |
| 94 (0x5E) | 6605 | answer | _cnet_RecvFromLbs_AnswerRuleListPermission |  |
| 95 (0x5F) | 6606 | request |  | __cnet_SendReq_RuleListNow |
| 96 (0x60) | 6606 | answer | _cnet_RecvFromLbs_AnswerRuleListNow |  |
| 97 (0x61) | 6607 | request |  | __cnet_SendReq_RuleNumOfChoice |
| 98 (0x62) | 6607 | answer | _cnet_RecvFromLbs_AnswerRuleListNumOf |  |
| 99 (0x63) | 6608 | request |  | __cnet_SendReq_RuleListName |
| 100 (0x64) | 6608 | answer | _cnet_RecvFromLbs_AnswerRuleListName |  |
| 101 (0x65) | 660E | request |  | __cnet_SendReq_RuleControl |
| 102 (0x66) | 660E | answer | _cnet_RecvFromLbs_AnswerRuleControl |  |
| 103 (0x67) | 6609 | request |  | __cnet_SendReq_RoomSetName |
| 104 (0x68) | 6609 | answer | _cnet_RecvFromLbs_AnswerRoomSetName |  |
| 105 (0x69) | 660A | request |  | __cnet_SendReq_RoomSetPassword |
| 106 (0x6A) | 660A | answer |  |  |
| 107 (0x6B) | 660B | request |  | __cnet_SendReq_RoomSetRule |
| 108 (0x6C) | 660B | answer | _cnet_RecvFromLbs_AnswerRoomSetRule |  |
| 109 (0x6D) | 660C | request |  | __cnet_SendReq_RoomSetFinish |
| 110 (0x6E) | 660C | answer | _cnet_RecvFromLbs_AnswerRoomSetFinish |  |
| 111 (0x6F) | 660F | request |  | __cnet_SendReq_RoomExplainPermission |
| 112 (0x70) | 660F | answer | _cnet_RecvFromLbs_AnswerRoomExplainPermission |  |
| 113 (0x71) | 6610 | request |  | __cnet_SendReq_RoomSetExplain |
| 114 (0x72) | 6610 | answer | _cnet_RecvFromLbs_AnswerRoomSetExplain |  |
| 115 (0x73) | 6406 | request |  | __cnet_SendReq_PieceEntry, __cnet_SendReq_RoomEntry |
| 116 (0x74) | 6406 | answer | _cnet_RecvFromLbs_AnswerRoomEntry |  |
| 117 (0x75) | 6501 | request |  | __cnet_SendReq_PieceExit |
| 118 (0x76) | 6501 | answer | _cnet_RecvFromLbs_BothRoomExit |  |
| 119 (0x77) | 6501 | notice |  |  |
| 120 (0x78) | 6502 | notice | _cnet_RecvFromLbs_NoticeRoomLeaver |  |
| 121 (0x79) | 6503 | notice | _cnet_RecvFromLbs_NoticeRoomCommer |  |
| 122 (0x7A) | 6505 | notice | _cnet_RecvFromLbs_NoticeRoomRemove |  |
| 123 (0x7B) | 6401 | request |  | __cnet_SendReq_PieceCount |
| 124 (0x7C) | 6401 | answer | _cnet_RecvFromLbs_AnswerRoomNumOfRoom |  |
| 125 (0x7D) | 6402 | request |  | __cnet_SendReq_PieceName |
| 126 (0x7E) | 6402 | answer | _cnet_RecvFromLbs_BothRoomName |  |
| 127 (0x7F) | 6402 | notice | _cnet_RecvFromLbs_BothRoomName |  |
| 128 (0x80) | 6403 | request |  | __cnet_SendReq_PieceJoinUser |
| 129 (0x81) | 6403 | answer | _cnet_RecvFromLbs_BothRoomJoinUser |  |
| 130 (0x82) | 6403 | notice | _cnet_RecvFromLbs_BothRoomJoinUser |  |
| 131 (0x83) | 6404 | request |  | __cnet_SendReq_PieceStatus |
| 132 (0x84) | 6404 | answer | _cnet_RecvFromLbs_BothRoomStatus |  |
| 133 (0x85) | 6404 | notice | _cnet_RecvFromLbs_BothRoomStatus |  |
| 134 (0x86) | 6405 | request |  | __cnet_SendReq_RoomPasswordInfo |
| 135 (0x87) | 6405 | answer | _cnet_RecvFromLbs_BothRoomPasswordInfo |  |
| 136 (0x88) | 6405 | notice | _cnet_RecvFromLbs_BothRoomPasswordInfo |  |
| 137 (0x89) | 6409 | request |  |  |
| 138 (0x8A) | 6409 | answer | _cnet_RecvFromLbs_AnswerRoomRestTime |  |
| 139 (0x8B) | 640A | request |  | __cnet_SendReq_RoomMember |
| 140 (0x8C) | 640A | answer | _cnet_RecvFromLbs_AnswerRoomMember |  |
| 141 (0x8D) | 640B | request |  | __cnet_SendReq_RoomJoinInfo |
| 142 (0x8E) | 640B | answer | _cnet_RecvFromLbs_BothRoomJoinInfo |  |
| 143 (0x8F) | 640B | notice | _cnet_RecvFromLbs_BothRoomJoinInfo |  |
| 144 (0x90) | 640D | request |  | __cnet_SendReq_PieceExplain |
| 145 (0x91) | 640D | answer | _cnet_RecvFromLbs_BothRoomExplain |  |
| 146 (0x92) | 640D | notice | _cnet_RecvFromLbs_BothRoomExplain |  |
| 147 (0x93) | 6413 | request |  |  |
| 148 (0x94) | 6413 | answer | _cnet_RecvFromLbs_AnswerRoomMatchEntryTypeList |  |
| 149 (0x95) | 6414 | notice | _cnet_RecvFromLbs_NoticeRoomMatchEntryTypeList |  |
| 150 (0x96) | 6509 | request |  | __cnet_SendReq_SetRoomProperty |
| 151 (0x97) | 6509 | answer | _cnet_RecvFromLbs_AnswerSetRoomProperty |  |
| 152 (0x98) | 650A | request |  | __cnet_SendReq_RoomProperty |
| 153 (0x99) | 650A | answer | _cnet_RecvFromLbs_BothRoomProperty |  |
| 154 (0x9A) | 650A | notice | _cnet_RecvFromLbs_BothRoomProperty |  |
| 155 (0x9B) | 6504 | request |  | __cnet_SendReq_MatchEntry |
| 156 (0x9C) | 6504 | answer | _cnet_RecvFromLbs_MatchEntry |  |
| 157 (0x9D) | 6412 | request |  | __cnet_SendReq_MatchEntryUser |
| 158 (0x9E) | 6412 | answer | _cnet_RecvFromLbs_AnswerMatchEntryUser |  |
| 159 (0x9F) | 6412 | notice | _cnet_RecvFromLbs_AnswerMatchEntryUser |  |
| 160 (0xA0) | 6506 | notice | _cnet_RecvFromLbs_NoticeMatchEntryUser |  |
| 161 (0xA1) | 6508 | notice |  | cnLBS_MatchStart |
| 162 (0xA2) | 6910 | notice | _cnet_RecvFromLbs_MatchStart |  |
| 163 (0xA3) | 6911 | request |  | __cnet_SendReq_MatchJoin |
| 164 (0xA4) | 6911 | answer | _cnet_RecvFromLbs_MatchJoin |  |
| 165 (0xA5) | 6912 | request |  | __cnet_SendReq_MatchPlSide |
| 166 (0xA6) | 6912 | answer | _cnet_RecvFromLbs_MatchPlSide |  |
| 167 (0xA7) | 6914 | request |  | __cnet_SendReq_MatchGameRule |
| 168 (0xA8) | 6914 | answer | _cnet_RecvFromLbs_MatchGameRule |  |
| 169 (0xA9) | 6913 | request |  | __cnet_SendReq_MatchOpponentInfo |
| 170 (0xAA) | 6913 | answer | _cnet_RecvFromLbs_MatchOpponentInfo |  |
| 171 (0xAB) | 6917 | request |  | __cnet_SendReq_MatchOpponentStatus |
| 172 (0xAC) | 6917 | answer | _cnet_RecvFromLbs_MatchOpponentStatus |  |
| 173 (0xAD) | 6918 | notice |  | __cnet_SendReq_MatchRejection |
| 174 (0xAE) | 6915 | request |  | __cnet_SendReq_MatchBattleCode |
| 175 (0xAF) | 6915 | answer | _cnet_RecvFromLbs_MatchBattleCode |  |
| 176 (0xB0) | 6916 | request |  | __cnet_SendReq_MatchMcsIpAddr |
| 177 (0xB1) | 6916 | answer | _cnet_RecvFromLbs_MatchGameServerAddr |  |
| 178 (0xB2) | 6181 | request |  | __cnet_SendReq_PersonalDataChange |
| 179 (0xB3) | 6181 | answer | _cnet_RecvFromLbs_AnswerPersonalDataChange |  |
| 180 (0xB4) | 6182 | notice |  | __cnet_SendSet_PersonalDataName |
| 181 (0xB5) | 6183 | notice |  | __cnet_SendSet_PersonalDataZip |
| 182 (0xB6) | 6184 | notice |  | __cnet_SendSet_PersonalDataAddress |
| 183 (0xB7) | 6185 | notice |  | __cnet_SendSet_PersonalDataTelephone |
| 184 (0xB8) | 6186 | notice |  | __cnet_SendSet_PersonalDataAge |
| 185 (0xB9) | 6187 | notice |  | __cnet_SendSet_PersonalDataMailAddress |
| 186 (0xBA) | 6188 | request |  | __cnet_SendReq_PersonalDataRegisted |
| 187 (0xBB) | 6188 | answer | _cnet_RecvFromLbs_AnswerPersonalDataRegisted |  |
| 188 (0xBC) | 6189 | request | _cnet_RecvFromLbs_RequestPersonalDataRegist |  |
| 189 (0xBD) | 6189 | answer |  |  |
| 190 (0xBE) | 6121 | notice | _cnet_RecvFromLbs_NoticePatchStart |  |
| 191 (0xBF) | 6122 | notice | _cnet_RecvFromLbs_NoticePatchData |  |
| 192 (0xC0) | 6123 | notice | _cnet_RecvFromLbs_NoticePatchFooter |  |
| 193 (0xC1) | 6124 | request | _cnet_RecvFromLbs_ReqestPatchLineCheck |  |
| 194 (0xC2) | 6124 | answer |  | __cnet_Send_PatchLineCheck |
| 195 (0xC3) | 6125 | request | _cnet_RecvFromLbs_RequestPatchFinish |  |
| 196 (0xC4) | 6125 | answer |  | __cnet_Send_PatchFinish |
| 197 (0xC5) | 6110 | request | _cnet_RecvFromLbs_RequestRegurationVersion |  |
| 198 (0xC6) | 6110 | answer |  |  |
| 199 (0xC7) | 6111 | notice | _cnet_RecvFromLbs_NoticeRegurationAddress |  |
| 200 (0xC8) | 6112 | request |  |  |
| 201 (0xC9) | 6112 | answer | _cnet_RecvFromLbs_AnswerRegurationAgree |  |
| 202 (0xCA) | 6801 | request |  |  |
| 203 (0xCB) | 6801 | answer | _cnet_RecvFromLbs_AnswerRegurationData |  |
| 204 (0xCC) | 6801 | request |  |  |
| 205 (0xCD) | 6802 | request |  |  |
| 206 (0xCE) | 6802 | answer | _cnet_RecvFromLbs_AnswerBrowserMethodGet |  |
| 207 (0xCF) | 6881 | request |  |  |
| 208 (0xD0) | 6881 | answer | 0027D030 |  |
| 209 (0xD1) | 6882 | request |  |  |
| 210 (0xD2) | 6882 | answer | 0027D2B0 |  |
| 211 (0xD3) | 6890 | request |  | __cnet_SendReq_TimingValue |
| 212 (0xD4) | 6890 | answer | _cnet_RecvFromLbs_BothTimingValue |  |
| 213 (0xD5) | 6890 | notice | _cnet_RecvFromLbs_BothTimingValue |  |
| 214 (0xD6) | 6891 | request |  | __cnet_SendReq_CurrentPlace |
| 215 (0xD7) | 6891 | answer | _cnet_RecvFromLbs_AnswerCurrentPlace |  |
| 216 (0xD8) | 640E | request |  |  |
| 217 (0xD9) | 640E | answer | _cnet_RecvFromLbs_AnswerLobbyMatchEntry |  |
| 218 (0xDA) | 640F | request |  |  |
| 219 (0xDB) | 640F | answer | _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser |  |
| 220 (0xDC) | 640F | notice | _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser |  |
| 221 (0xDD) | 6210 | request |  |  |
| 222 (0xDE) | 6210 | answer | _cnet_RecvFromLbs_AnswerAnnexEntry |  |
| 223 (0xDF) | 6211 | request |  |  |
| 224 (0xE0) | 6211 | answer | _cnet_RecvFromLbs_AnswerAnnexExit |  |
| 225 (0xE1) | 6212 | request |  |  |
| 226 (0xE2) | 6212 | answer | _cnet_RecvFromLbs_BothAnnexJoinUser |  |
| 227 (0xE3) | 6212 | notice | _cnet_RecvFromLbs_BothAnnexJoinUser |  |
| 228 (0xE4) | 6213 | request |  |  |
| 229 (0xE5) | 6213 | answer | _cnet_RecvFromLbs_AnswerAnnexMember |  |
| 230 (0xE6) | 6214 | notice | _cnet_RecvFromLbs_NoticeAnnexLeaver |  |
| 231 (0xE7) | 6215 | notice | _cnet_RecvFromLbs_NoticeAnnexCommer |  |
| 232 (0xE8) | 6701 | notice |  | __cnet_SendSet_ChatMessage |
| 233 (0xE9) | 6702 | notice | _cnet_RecvFromLbs_NoticeChatMessage |  |
| 234 (0xEA) | 6703 | request |  | __cnet_SendReq_SearchUser |
| 235 (0xEB) | 6703 | answer | _cnet_RecvFromLbs_AnswerSearchUser |  |
| 236 (0xEC) | 6709 | request |  | __cnet_SendReq_ConditionSearchUser |
| 237 (0xED) | 6709 | answer | _cnet_RecvFromLbs_AnswerConditionSearchUser |  |
| 238 (0xEE) | 6709 | notice |  | __cnet_Send_ConditionSearchUserCertify |
| 239 (0xEF) | 670A | request |  |  |
| 240 (0xF0) | 670A | answer | _cnet_RecvFromLbs_AnswerConditionSearchUser |  |
| 241 (0xF1) | 6704 | request |  | __cnet_SendReq_SendMail |
| 242 (0xF2) | 6704 | answer | _cnet_RecvFromLbs_AnswerSendMail |  |
| 243 (0xF3) | 6705 | notice | _cnet_RecvFromLbs_NoticeMailMessage |  |
| 244 (0xF4) | 6706 | request | _cnet_RecvFromLbs_RequestAdminMessage |  |
| 245 (0xF5) | 6706 | answer |  | cnLBS_AnswerAdminMessage |
| 246 (0xF6) | 6708 | notice |  | __cnet_SendSet_ChatBinary |
| 247 (0xF7) | 6708 | notice | _cnet_RecvFromLbs_NoticeChatBinary |  |
| 248 (0xF8) | 670B | request |  | __cnet_SendSet_ChatMessageTU |
| 249 (0xF9) | 670B | answer | _cnet_RecvFromLbs_AnswerChatMessageTU |  |
| 250 (0xFA) | 670C | notice | _cnet_RecvFromLbs_NoticeChatMessageTU |  |
| 251 (0xFB) | 670D | request |  | __cnet_SendSet_ChatBinaryTU |
| 252 (0xFC) | 670D | answer | _cnet_RecvFromLbs_AnswerChatBinaryTU |  |
| 253 (0xFD) | 670E | notice | _cnet_RecvFromLbs_NoticeChatBinaryTU |  |
| 254 (0xFE) | 630A | request |  | __cnet_SendReq_LobbyMemberList |
| 255 (0xFF) | 630A | answer | _cnet_RecvFromLbs_AnswerLobbyMember |  |
| 256 (0x100) | 6410 | notice | _cnet_RecvFromLbs_NoticeLobbyLeaver |  |
| 257 (0x101) | 6411 | notice | _cnet_RecvFromLbs_NoticeLobbyCommer |  |
