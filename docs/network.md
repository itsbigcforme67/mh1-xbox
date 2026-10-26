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
| Furthest point | **on screen** (agent B, 9 Oct 2026, section 3.5): title network mode or `--online` -> connecting -> Capcom ID select -> login -> plaza with its lobby list -> the town (stage 76) with the other hunters walking and chat -> guild hall -> hunter registration -> quest counter (level, quest, rule sheet) -> room posted on the quest board -> joined by another player -> departure door -> match -> the room's quest as a co-op quest (2 players tested). Not reached: the return to the town after the quest, patches, mail, event quests, the portal web pages |
| Test (the town) | `tools/test_online_town.sh` (about 3 minutes, headless, two clients): the game's own screens into the town, positions, chat both ways, a room made / joined / matched, the co-op quest started, a public address refused |
| Co-op (direct connect) | `--coop` (dialogs) or `--host` / `--join IP`: 2-4 players hunt one quest together: hunters with their armour and weapons, shared monsters (one machine owns each, the others follow its packets), kills, quest clear, the host-decided supply box (section 3.4, `tools/test_coop.sh`) |

What "reaches the lobby" means here, honestly: the headless test (`--nettest`, `src/pc/rt/rt_net.c`) calls the
game's `cnLBS_*` functions in the order the lobby UI (`src/lobby/f/lb_cli.c`: `lbc_login_init`, `lbc_login_*`,
`lbc_top_menu_*`, `lbc_in_plaza_*`) calls them, with the screens replaced by a small state machine. The packets on
the wire are built and parsed entirely by the game's C. Since 9 Oct 2026 the real screens run too (section 3.5);
`--nettest` stays as the quick protocol test.

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
| online flag | `system_w.online` (+0x10), `Online_ck()` | `mode_sel_end` (omake.c) | everything below |
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
| `src/pc/rt/rt_online.c` | the online town: config (portal replacement), connect, `internet_lobby_act`, chat keyboard, browser / quest download stand-ins, the hand-off to co-op (3.5) |
| `src/lobby/f/lb_online_nm.c` | PC C for lb_cli.c's asm-only functions (`lbc_login_init`, `Lbc_GetRoomRule`, `tk_logout_message_sub`) |
| `tools/test_online_town.sh` | the town test (two clients, a room hosted by a player and one through mh1-server's relay; ~6 min) |
| `tools/pc_patch.py` | argument fixes for the cnet files |

### 3.4 Co-op over direct connect (agent E, 7 Oct 2026, round 2)

Milestones M1 (two players see each other walk on a quest stage), M2 (four players) and M3 (a whole hunt to the clear
with shared monster health) are reached, tested headless on 127.0.0.1 (round 3 added M3). Why it is built this way:
section 1a.

Run it (ONLINE=1 build only):

    build/pc/mhview_online disc/mh1 --host --quest 131 --players 2      # host: waits for 1 more player, then starts
    build/pc/mhview_online disc/mh1 --join 192.168.1.20                  # a joiner, by the host's LAN address
    build/pc/mhview_online disc/mh1 --coop                               # asks host / join, quest, players or address

| Option | |
|---|---|
| `--host [IP]` | host the quest; listens on every interface (or the given private address of this machine); joiners are accepted only from loopback / private addresses or `--allow`ed ones. Without `--quest` it prints the Elder's quests on the console and asks for a number |
| `--hunter N` | the save slot (1-3) of this player's hunter; default: the first used slot of the player's card |
| `--allow IP` | one public address allowed on purpose (a friend over the internet; repeatable; `RT_NET_ALLOW=a,b`). MH Oldschool stays refused |
| `--coop` | ask in a small window (Windows: a Win32 window; Linux: zenity, else kdialog; else the console): host or join, then the quest and the number of players, or the host's address |
| `--join IP` | join the host at IP (loopback or private addresses only; MH Oldschool and public addresses are refused) |
| `--port N` | TCP port, default 10300 (both sides) |
| `--players N` | host: start when N players (2-4) are in; after `RT_NP_WAIT` seconds (default 300) it starts with whoever came |
| `RT_WEAPON=id` | the player's weapon (Ken_data id), sent to the others in the mini data |
| `RT_PL_LOOK=sex,face,hair,legs,head,body,arms,waist[,hair colour]` | only without a save (or with `RT_NP_NOSAVE=1`): the look and armour (Armor_*_Data rows; default male, armour 5) |
| `RT_QUEST_TIME=ticks`, `RT_QUEST_RETIRE=tick` | test aids: the quest's time left at the start; abandon the quest at that tick (`Quest_retire_set`) |
| `RT_NP_TRACE=1`, `RT_NP_POS=n`, `RT_NP_EM=n` | test aids: every packet; every player's position, action, the box bits and first pouch slot; every monster's HP, owner and action, every n ticks |

What happens:

1. `net_peer.c`: joiners open one TCP connection to the host and send their mini data (the 0x18 used bytes, as
   `Lb_set_mini_data` lays them out: weapon, sex, face, hair, armour). The host gives each joiner the next slot (1-3,
   joining order), then sends everyone the quest number, the player count and every slot's mini data. After that the
   host passes each frame on to the other joiners and hands it to its own game (as Capcom's game server relayed records).
   Frames: `u16 n, u8 sender slot, u8 type`, then the game's packet; type 1-10 is the AQ channel.
2. `rt_np.c`: `rt_player_game_init` -> `rt_np_init_slots` does what `Game_task` step 3 and `init_pl_work` do online:
   `game_w.master` = this machine's slot, `pl_num`, `pl_state[]`, the other slots' player works (`be_flag`, `id`, stage),
   every slot's equipment and look from its mini data (as `Set_mini_data_to_pl`), the HUD's own hunter (`lpPit->pl`), the
   same random seed on every machine, the network delay `game_w+0x1B0` = 2 frames, and switches `Online_ck()` on.
   `pl_init` then initialises every slot, `armor_create_model` builds every hunter's look, and a start barrier (what
   `net_start_ck` does in `game13`) waits until every player has loaded.
3. Sending is the game's own: `net_send_pl` (netsyn01.c) builds the packet and calls `AQ_data_put`, which on the PC goes to
   the transport. The local hunter sends kind 3 at start, kind 1 on every action change, kind 2 every 20 ticks
   (`System_timer % 20 == slot`; the PC now counts `System_timer` during a co-op quest).
4. Receiving: each tick before the players move, `rt_np_tick` takes the frames and dispatches them as `self_data_ctrl`
   does: `net_receive_pl` (netsyn02_nm.c) for channels 1-4, `net_receive_host` (netsyn05.c) for 7, `net_receive_em`
   (netsyn11_nm.c) for 8, `net_receive_sys` (netsyn09.c) for 10. A machine's own channel-7 packets come back to it too
   (mcsls loops a player's own data back; the host answers its own supply-box requests that way). The remote hunter then replays the action (`Pl_act_set`),
   turns toward the sent angle and slides to the sent position (`Pl_adj_calc` / `Pl_pos_adj`).
5. The viewer draws the other hunters on this player's area (`remote_hunters`) in their own armour, with their weapons,
   posed by the game's motion player, and gives the game C their joint matrices. The local hunter is `player_work[game_w.master]` (`lp` in viewer.c), no longer
   always slot 0.
6. A co-op quest runs at 30 ticks a second also headless (otherwise test instances run at different speeds). A player
   whose connection ends gets `pl_state = 0xFF` and is hidden (the host tells the others).

Tested (`tools/test_coop.sh`, ~25 s): 2 and then 4 headless instances on quest 131, each walking a different scripted
path, one with a great sword; at the end every instance sees every player at the position that player has itself
(0 units off), on the same stage; screenshots `build/show/coop_N_slotK.png` (the HUD shows the party's bars and map
arrows, the other hunters are drawn). Also: a joiner leaving early, and refusals of `--join 8.8.8.8`, `--join 34.75.107.68`
and `--host 8.8.8.8`. While a hunter runs and turns, the others see it up to a few hundred units off for up to 20 ticks
(only kind 2 corrects the position); this is how the PS2 code works too, not measured against a PS2.

**Monsters (M3, round 3).** The game's own scheme, now running: each monster has one owner (`EMW+0x8C3 == 0` on the
owning machine; `pull_enemy_work` gives every monster to the host at the start). The owner runs its AI and sends kind 1 / 3
on every action (position, angles, action, **HP**, status flags), the others apply it (`net_receive_em`) and play the
action. `Em_Master_Change` hands a monster to a player on its area when the owner is not there (or leaves), with kind 4.
Kills and the quest's events go over the sys channel (`q_net_send_em_die` -> sys 6 code 7, `Quest_net_sub`).
Finding (confirmed by the tests): damage a non-owner deals to its own copy is overwritten by the owner's next packet;
what counts is the owner's copy, where the other hunters' attacks are replayed (`Pl_act_set`) and hit there. So a hit
only lands if it also lands in the owner's replay (the PS2 game works the same way; the replay is 2 frames late).
Fixed on the way: `netsyn11_nm.c` read HP and four other fields with the wrong width in kinds 3 and 4 (checked against the
asm; HP as a byte), and used the unnamed `D_642160` (= `em_atk_mode_timer_tbl`); `trans_box` (menu_disp_nm.c, the
supply-box screen, **single player too**) passed the `item_str` table instead of `item_str[0]` to `sprintf` and overflowed
its buffer once the cursor slot was empty.

**Round 4 (owner's play-test list).**
* Each player's **own saved hunter**: the card's image is decoded as `decode_data` does, the options and the three slots
  go into `option_w` (`save_data_sub(0, 0xF)`), the slot into `User_data` (`Load_userdata`), `select_w[0xB6]` = slot
  as the Continue screen sets it. The mini data is built from `User_data` (what `Set_userdata` / `Set_equip_data`
  read) plus the name (0x12 bytes, our addition: the PS2 sent handles separately); the own slot gets `Set_userdata`
  (name, pouch, equipment). After the quest (game mode 6) `rt_np_session_end` writes the slot back like the bed save
  (`Save_userdata`, `save_data_sub(1, slot bit)`, `encode_data`; only this slot changes), closes the session and the
  village runs single player.
* **Quest start ownership fixed**: the first monsters are made before the slots are known, so every machine thought it
  owned them; now they get this machine's slot (the host owns them, as on the PS2).
* **Failures**: 3 carts fail the quest on every machine (each machine runs every hunter's faint, `pl_die000` ->
  `Quest_remuneration_calc`, and the reward pool runs out everywhere); time-out (each machine's own timer, the lower one
  sent every 5 minutes); abandon (`Quest_retire_set` -> sys 0xC -> `mcsls_force_drop`, now `pl_state 0xFF` on the
  others). **Capture is not tested** (needs traps and tranquillisers in a scripted fight); note that `Quest_net_sub` has
  no case for code 8 (capture), so the others only see the monster's state through its packets (as on the PS2).
* **Two big monsters at once**: quest 96 (Rathalos + Rathian) could not be finished by the test aids (the Rathian's HP
  stops at 1 under `RT_PL_SLAY`, offline too), quest 97 (two Diablos) is out of the aids' reach (underground); the test
  uses quest 7 instead: three Velocidromes one after another.
* **Timer drift (point 5)**: not a PC artefact. The drift comes from `info_stop` during event demos (a player who walks into
  a boss's intro has his timer stopped for its length, ~15 s) and on the PS2 also from loading (the PS2's game loop does
  not tick while it loads; on the PC an area loads within 5 ticks). The original rule is kept: every machine sends its
  timer every 9000 ticks and at 0, the lower one wins.
* **Bugs found**: the host died of SIGPIPE when a joiner had already closed its socket (`MSG_NOSIGNAL` now, also in the
  lobby client); a quest stage's set object prim was drawn in the village after a co-op quest (`rt_village_enter` now
  clears the prims as `all_reset` does; single player too).
* **Windows**: `ONLINE=1 tools/build_win.sh` builds `build/win/mhview_online.exe` (Winsock, ws2_32); a Wine joiner plays
  with a Linux host in the test. The Win32 dialog is compiled, not opened in a test (it would appear on the desktop).
* **Internet**: the host listens on all interfaces; joiners from public addresses only when `--allow`ed; MH Oldschool
  always refused. The accept filter for a public joiner is not tested (no public address here).

Tested (`tools/test_coop.sh`, about 15 minutes, all headless on 127.0.0.1; parts `2`, `4`, `hunt2`, `hunt4`,
`handover`, `leave`, `carts`, `timeout`, `abandon`, `multi`, `box`, `wine`; the hunts in `tools/test_coop_hunt.py`):
* `2` / `4`: walking, positions exact on every machine.
* `hunt2` / `hunt4`: quest 137 with saved hunters ANNA, BOB, CARL, DAVE (made by the test from a new game, quest 131 and
  a bed save): every machine sees every name, the host's monster HP values (500, 180, 0), the host as the only owner,
  the clear within a tick or so, its own reward screen, the village, and its own save with more money (1550 -> 1725
  for two, 1637 each for four; checked by decoding the card).
* `handover`: the host stays at the camp, the joiner fights, the monster passes to him on both machines, both clear.
* `leave`: 3 players; slot 1 fights and owns the monster, then quits; the monster passes to slot 2 (on the area), who
  kills it; host and slot 2 clear and save.
* `carts`: the host faints twice, the joiner once: both fail at the same tick (D5 6), both reach the village.
* `timeout`: 20 s of quest time: both fail at the same tick.
* `abandon`: the joiner abandons at tick 300: back to the village and saved; the host is told, clears and saves.
* `multi`: quest 7, two of the three Velocidromes killed in 150 s, the same kills left on both machines.
* `box`: the supply box decided by the host. `wine`: the Windows build (Winsock) joins a Linux host.
* Refusals: public and MH Oldschool addresses; `--allow 192.0.2.1` lets that one address through (TEST-NET, nobody
  answers), `--allow 34.75.107.68` still refuses MH Oldschool.

* **Monster owner after a drop** (agent B, 9 Oct 2026): `rt_np_init_slots` gave the first monsters' owner slot
  (`EMW+0x88E`) this machine's slot instead of the host's (0), so when the host left the others never took the monster
  over; and `Em_Master_Change` (em_master_nm.c) hands a monster to a player on its area only if `pl_state[j]` of the
  loop counter is 1 (the original's own test, asm 0x5399E8), which fails forever once slot 0 dropped. The PC checks the
  candidate's `pl_state[i]` instead (identical while nobody has dropped). `RELAY=1 tools/test_coop_hunt.py hostleave`
  (through mh1-server's relay, docs/server.md) passes: slot 1 takes the monster over, hands it to slot 2, both clear.

Not done yet:
* If the host leaves a direct-connect session, the others lose each other (the host relays); they go on alone
  (through mh1-server's relay they stay together, above).
* Capture, giving items between players (kinds 7/8), in-quest chat (channel 6), two big monsters on one area at once.
* The co-op starts at the quest, not from the village's quest board; the village afterwards is single player.
* The zenity / kdialog / Win32 windows were not opened in a test (they would appear on the desktop); the console
  fallback is tested.

### 3.5 The online town on screen (agent B, 9 Oct 2026)

Everything here is the game's own code (lobby.bin's lobby client `src/lobby/f/lb_cli.c`, the plaza TUs `lb_plz2/3.c`,
the town = the village code with `Online_ck() == 1`, `src/lobby/f/lb_village_nm.c` and the `b/` runs), running in the
`ONLINE=1` build; `src/pc/rt/rt_online.c` replaces only what the PS2 did outside the game (the network settings, DNAS,
the KDDI portal and its browser) and hands the matched room to the co-op session.

**How the PS2 gets there** (read from `Game_task`, f_game_nm.c, and `mode_sel_end`, omake.c): the title menu's
"new game / continue" each have a second choice; `sel2 == 0` sets `system_w+0x10` (= `Online_ck()`), and `Game_task`
step 2 runs `ms_network_sub` (settings, device, DNAS), `server_connect` (= `internet_browser`: the portal pages, which
leave the server table in `bsCsvWork`, the MMBB id in `D_3A3B71` and the server list in `BsLbsInfo`;
`internet_server_select`; `internet_connect_10` = `tcp_init`), `net_ToNetworkLobby`, then `internet_lobby_act` every
frame until it returns 0 (a match), then `internet_to_modem` (the game server, `cmcs_*`).

**What the PC does** (`rt_online.c`, called from the viewer's game mode 6 instead of the village):
1. The network mode is asked for by the title menu (`Game_task` with `system_w+0x10`, rt_boot.c) or by `--online`.
2. The portal's place is taken by three keys of `mh1pc.ini` (gfx_opts.c keeps them; defaults written on a normal
   exit): `online_server = 127.0.0.1:10200`, `online_login = 00000000` (the MMBB id; the login packet carries its first
   8 digits, docs/server.md 2.1), `online_password = LOCALTEST0000000`. A public address (or name) in `online_server`
   is the player's choice and is allowed (`net_allow_server`, net_cpinet.c); MH Oldschool's addresses stay refused.
   `RT_NET_HOST` / `RT_NET_PORT` / `RT_NET_ID` / `RT_NET_PASS` override them for tests (an address given that way gets
   no exception). Not in the F10 menu yet (it has no text fields). It fills `bsCsvWork` (password, server id,
   `host:port`, user counts), `BsLbsInfo[0]` (id, name: `Get_ServerName`), `BsLbsCount = 1` (one server: the first
   login goes straight on; with several the client logs out and shows the server list), `CurDevice` (kind 1, a guess:
   the broadband adaptor; `DeviceGetOptionalStatus` reads it for the login's first data). Then `tcp_init` (the game's),
   `net_ToNetworkLobby` (the game's), `Online_ck()` on, and `internet_lobby_act` every tick.
3. The in-game browser: `MainBrowser` returns "closed" at once, so a page (the personal data page of a new account,
   `afs://02/3`) is skipped and nothing is registered; `MainBsInitialize` does nothing.
4. Quest download on lobby entry (`Lbc_DownloadQuest`): its background job `__cnet_bgProg_ReadFileDownloadAllocation`
   (main 0x27D4A0) is not decompiled; the PC ends it at once with no files (no event quests).
5. All online machines run at 30 ticks a second also headless (otherwise a screenshot run does hundreds of ticks
   while one answer is on the way).
6. The town: the viewer draws up to 8 town hunters (`rt_online_visible`: in use, same stage); the local one is
   `player_work[game_w.master]` (the lobby member list's order). Their moves come from the game's own `lb_send_data`
   -> chat binary 6708 -> `Lb_check_receipt`. Name tags: the game's `lb_disp_name` (lb_ai.c) draws them over every
   hunter on the stage (own and others, with the weapon icon and the status / quest marks) now that
   `flvecrRotTransPers` is real (seen on screen, 10 Oct 2026: build/show/online_town/*_5900.png). Weapons: every town
   hunter carries its own (10 Oct 2026): `Lb_set_mini_data_to_pl` puts the weapon triple at PLW+0x35E but not the model
   (+0x34C) and kind the viewer draws from, so `rt_np_town_weapon` derives them as the quest's `Set_mini_data_to_pl`
   (f_ud.c) does, and the viewer reloads a hunter's weapon model when it changes (`weapon_follow`, also the local
   hunter's: before, it kept the model of the `--quest` start).
7. Chat: Tab starts typing on the PC keyboard, Enter sends through the game's `Lb_send_chat` (6701); the game's chat
   log window shows what the server sends back. The soft keyboard paths (`Plaza_chat_move`) are the game's.
8. When `internet_lobby_act` returns 0, `matched()` reads the slot (`USER_PL_ID`, from MatchPlSide), the player count
   (`cw+0x2C47`), the quest (`select_w+0xAC`) and the "game server" (`cnLBS_Get_GameServerAddress`, 6916), closes the
   lobby connection and configures the co-op session (`rt_np_configure`): slot 0 hosts, the others join that
   address (with retries for 10 s). If 6914 (MatchGameRule) said `mh1-relay`, the address is mh1-server's session relay
   (docs/server.md 4.3): every player joins it, nobody hosts. The viewer then starts the quest as `--host` / `--join` would (section 3.4).

The town's places (stage 76 = the square; spot 8 -> 77 the guild hall; spots 9 / 10 -> 78, 11 -> 79, 12 -> 80, 13 =
back to the plaza). In the guild hall (77): the receptionist (NPC talk kind 0, at (1090, 1600)) takes quests
(`Lb_guild`: level by hunter rank, quest card, the rule sheet "players / password / message", then
`lb_guild_make_room`), the quest board (spot 7) lists the posted rooms to join (`Lb_join`), the guild master (talk 72)
does the hunter registration once (event flag 4, kept in the save; until then the board says "hunter registration not
done"), spot 6 is the departure door: a member says ready there (x68 = 26, MatchEntry), the leader starts when all
others are ready (dialog 0x36, x68 = 48 -> 7: MatchEntry + MatchStart).

Test aids (all `RT_*`, ONLINE build): `RT_ONLINE_TRACE=1` (the client's step machine, the town's talk state, every
hunter's position each 2 s, each stage's spots and NPCs, new chat log lines), `RT_NET_SAY="tick:text;..."`,
`RT_LB_WARP="tick,x,z[,ang];..."` (town ticks), `RT_NET_REGISTERED=1` (event flag 4 set), `RT_NAME` (the hunter's name
when started without a save), `--online`.

**Fixes found on the way** (all PC side; the PS2 sources unchanged except `#else` branches of `__MWERKS__`):
`lbc_game_ready_02` is called from a step table without arguments and gcc kept a local in its incoming argument slot,
which overwrote the caller's saved registers (the crash right after the match); `Lb_join`'s quest card and quest-type
byte; `Lb_chat_receipt`, `Lb_get_comment`, `getHandleFromID`, `cnWrap_SetFontColor`, `lm_member_list_mv`,
`net_SetMenu_SelectHandleName` passed no argument where the asm leaves one in a0 (tools/pc_patch.py); chat_nm.c's
chat log name line passed no string ("(null)", also single player); `sound_req_com` (lbsnd02.c) needed the float
argument adaptor (crash on the guild hall's NPCs); the cnLBS file-download C (main cnlbs01-03) read the lobby's work
by absolute address. Written for the PC from the asm (not compared with check.py): `lbc_login_init`,
`tk_logout_message_sub`, `Lbc_GetRoomRule` (`src/lobby/f/lb_online_nm.c`); `plaza_trans_ot0` and
`plaza_enterLobbyTrans` (agent C's readings, `lb_plz3.c` `#else`). Added 10 Oct 2026 (agent B, from the asm, not
compared with check.py): `disp_status` (`lb_plz3.c` `#else`: a hunter's status card for my status, a friend, a search
result), `plaza_mailBoxTrans` (b/nm, rewritten), and links of `lb_select_tag` (the quest board's room tags; its draft
was `static`) and `Plaza_add_friend` (`f/lb_pz12_nm.c`). Fixed on the way: `Lb_PlayerStatus` (lb_aa.c) read the
lb_player entry where the asm reads its PLW (crash on "player list"); the plaza `*Trans` drafts in b/nm computed every
screen position as `(x << 0x30) >> 0x30`, a 64-bit sign extension that 32-bit C evaluates to 0 (gcc folds it), now
`(s16)` casts (tools/pc_shift64.py); calls that lacked the arguments the asm passes
(`disp_status`, `put_member_info`, `font_print_double`'s string, `flfntLocate`'s y) take them from the asm.

**The return to the town after an online quest** (done 10 Oct 2026, `tools/test_online_town.sh`). What the PS2 does
(Game_task mode 6 online, f_game_nm.c; the draft's `*(u8 *)0x6EF8F5` is `CnetWork+5`, 0x6F0000 - 0x670B in the asm):
all_reset, Load_overlay(3), Clear_lobby_ram, AQ_session_exit_online, internet_connect_minimum_cleanup (returns 2),
then `CnetWork+5 = 3` (also set when a match is made) and Game_task step 2 from `game_w.step = 1` again (server
connection, net_ToNetworkLobby, internet_lobby_act). With `CnetWork+5 == 3` the login sends the kept id and handle
without the id screen (`CallBack_Result_LoginLobbyServer` case 1), `lbc_login_finish` rebuilds the quest table,
`lbc_login_finish_after` saves the game to the card (`McOperationSet(7, 2)`: `CardOnsv1*`, which first checks that the
card is the one the game started with: `check_sum_ck`), then `To_MyLobby` -> `lobby_return_to_lobby`: 6891 current place,
the plaza and lobby lists, the lobby's member list (no lobby entry request: the server already has the hunter there),
the quest download, the town. The PC (rt_online.c): after `matched()` the online mode stays on; at the next game mode 6
`rt_online_enter` runs those steps (Clear_lobby_ram, COM_R_No_*, CnetWork+5 = 3) and connects again. Not done:
Load_overlay(3) (`rt_lb_reload` would reset the lobby client and the cnet library; `MH_lobbyClear` in
lobby_return_to_lobby clears what the town needs). The co-op session's own save (`rt_np_session_end`) still runs
first, so a failed reconnect does not lose the reward. Three bugs found: `connect_ps2` returns nothing in C but its
callers use v0 (CpInetTcpOpen's handle): on the PC `tcp_init` kept a stale eax, the second connection polled the first,
closed socket (fixed with a PC `#else`); `rt_np_session_end` ran twice (the town turns Online_ck on again), the second
time turning Online_ck off in the town, so `Lb_make_quest_tbl` read the offline table past its end; the co-op start
reloaded the hunter from the card (what was done in the town, e.g. the guild registration, was lost); and the PC's
own card save left the card's check values stale, so the game's save said the start card was missing (now kept with
`check_sum_set` as the game's saves do). The town after the return: the guild hall (stage 77, where the hunters left),
both hunters see each other.

**Friends, search, mail on screen** (10 Oct 2026, two headless clients, the town menu page 2): player list -> a hunter's
status (2 pages) -> square "add to the friend list" (yes / yes; `Plaza_add_friend`, the net file saved); friend list
(6709 per friend: online), circle = status (`disp_status`: name, id, weapon, HR, where: server, plaza, lobby; 6703
and 670D for the comment); R1 = short mail -> send (6704) -> "mail sent"; the other hunter gets 6705, the mail icon,
and reads it in his mail box (list, then the text). The net file (friends, received mail): the PS2 keeps it on the card
(`SaveNetFile_ForLobby`, main 0x28A3D0, loaded by `ms_network_sub`, which the PC skips); the PC writes CNFile and
RecvMailInfo to `mh1pc_net.bin` next to the save (a PC format) and reads it when the online mode starts.

**Guest rooms** (stages 0x51-0x55; verified on screen 10 Oct 2026, `tools/test_online_guest.sh`). The square's spot
12 (the action button, square on the pad, at 7080,3030) leads into the inn (0x50); there spot kind 18 (1400,2775) is
the free room 0x51, kinds 19 / 20 the rented ones (0x52 / 0x53, 0x54 / 0x55 with R1; `lb_goto_guest_room` /
`Lb_check_hotel`: hunter rank and money, paid once a session), kind 5 the way back. They are local: no lobby-server
message besides the 6708 stage change, and each hunter is alone there: `Lb_Pl_stg_ck` (lb_h.c) shows another hunter
only on my stage and only on the square (0x4C) and the guild hall (0x4D). The PC viewer drew every hunter on the same
stage until 10 Oct 2026; `rt_online_visible` now asks `Lb_Pl_stg_ck`. The test: two clients against mh1-server take
room 0x51 at the same time, neither shows the other there or in the inn, back on the square both are shown again where
the other says it is. (`Lbc_GuestReadRoom` / `Lbs_GuestEnterRoom` in lb_cli.c are the joining side of the guild quest
rooms, in use since round 1.) Annex messages 6210-6215 have handlers but no sender in the client.

Not done yet: the Xbox build of the town was not run; personal data is never sent by the PC (section 5.8). The Windows build runs the
town under Wine (section 4).
### 3.6 PS2 patch: a real PS2 or PCSX2 on our server (agent B, 15 Oct 2026)

Step 1 of the online plan (docs/DECISIONS.md, 15 Oct 2026): the player's own Japanese disc (SLPM-65495), patched on
the player's machine, goes online with **our** server (`tools/server/mh1_server.py`), the same lobby as the PC port.
Tool: `tools/ps2_patch.py`; patch code: `src/ps2patch/online.c` and one `#ifdef MH1_ONLINE_PATCH` in
`src/main/net/ms01.c`; test: `tools/test_ps2_patch.sh`. How a player uses it: docs/PLAYING.md, "Playing online on a
PS2 or in PCSX2".

**What changes, and nothing else** (the tool refuses to write if any other byte differs):

| | Original | Patched | Where |
|---|---|---|---|
| (a) DNAS | `ms_network_bb_authentication` (main 0x268040, ms01.c) loads the dnas_net overlay and calls `InetDNAS2ConnectNetStart` (0xA769C0) every frame until it says 1 (authenticated); Sony's DNAS servers are gone | `r = 1` at once: the success path runs as before (`#ifdef MH1_ONLINE_PATCH` in ms01.c; the function is recompiled, 0x268040-0x2685B4) | SLPM_654.95, 671 bytes |
| (b) server address | `internet_browser` (lobby 0x5B5980, 720 bytes; `server_connect` step 0) runs the in-game browser on the KDDI Multi-Matching BB portal (HTTPS, gone), whose pages leave the password and server table in `bsCsvWork`, the MMBB id in `D_3A3B71` (= bsCsvWork+0x11), the server list in `BsLbsInfo` / `BsLbsCount`, then loads the network models (its steps 4-6) | our `internet_browser` (src/ps2patch/online.c): step 0 as before, then `mh1p_fill_server_table` fills the same work from the player's settings (`--server HOST:PORT`, `--login`, `--password`; kept in the same 720 bytes after the code), sets `MMBB_LOGIN = 1` as a portal login did, and runs steps 4-6 unchanged | AFS_DATA.AFS (lobby.bin), 720 bytes |

Everything after that is the game's own unchanged code: the server select (`server_select_00`, `create_server_table`
with one server), `tcp_init` (a dotted address directly, a name through the console's DNS: `InetDnsGetIPAddress`),
the lobby login (6101 with the 8-digit key and the 16-character password) and the whole town. The PC online build links
the same `mh1p_fill_server_table` (rt_online.c fills its `MH1PCONF` from mh1pc.ini), so every PC online test runs the
patch's server-table code against mh1-server (`tools/test_online_server.sh`: accounts, login, town, a hunt).

**How the patch is made** (no Capcom bytes in the repository, and none in a file we hand out):
1. The player sets up the byte-matching rebuild from his own disc (README.md Phase 1; `tools/rebuild.sh main lobby`
   must say OK). This needs the PS2 compiler (MWCC via wibo) and binutils, as for any PS2 work in this repo.
2. `tools/ps2_patch.py DISC --server HOST:PORT --login L --password P` checks DISC (size and CRC32 of SLPM_654.95 and
   AFS_DATA.AFS, tools/disc_files.py), checks that build/main.bin and build/lobby.bin are byte for byte the disc's
   main section and lobby.bin, writes `build/ps2patch/mh1p_conf.h` (the settings), compiles ms01.c and
   src/ps2patch/online.c with `-DMH1_ONLINE_PATCH`, links each at its original address with the rebuild's symbols
   (`_gp` = 0x38EB70; no small data allowed in the patch: lobby.bin is out of the gp range), checks the size (at most
   the original range; the rest is filled with nops), that only the patched function changed, that the patched code
   no longer calls `InetDNAS2ConnectNetStart` / `MainBsInitialize` / `MainBrowser` / `MainBsDispose` (and the original
   did), and that the server address is in the patched lobby.bin.
3. It writes a copy of the .iso with the changed bytes (the files keep their place and size: the main section's file
   offset from the ELF, lobby.bin's offset in AFS_DATA.AFS, the files' sectors from the ISO9660 root), or with
   `--ppf FILE` only a PPF 3.0 patch for that .iso, or for a disc folder a patched SLPM_654.95 and AFS_DATA.AFS.
   `--dry-run --disasm` builds and checks everything and prints the patched code (ours) and settings.
4. With the options off nothing changes: the normal build does not compile src/ps2patch (tools/build.py), and ms01.c
   without `MH1_ONLINE_PATCH` is the matching file (`tools/rebuild.sh`: 5 OK).

The tool refuses MH Oldschool's addresses and the original service's names (the list in net_cpinet.c), a login that
is not 8-10 digits and a password longer than 16 characters. Each player's patch holds his own login: the patched
image and the .ppf are personal (and the image is the player's copy of the game).

**Verified (15 Oct 2026, without a PS2 or PCSX2)**: `tools/test_ps2_patch.sh` on the owner's disc: rebuild identical
to the disc; the DNAS call and the three browser calls gone; settings in place; MH Oldschool addresses refused; the
PPF applied to the .iso gives exactly the patched files, and those differ from the originals only inside
ms_network_bb_authentication (SLPM_654.95) and internet_browser (AFS_DATA.AFS). Read in the disassembly
(`--disasm`): the patched internet_browser's gp-relative stores hit COM_R_No_2 (0x38A870), Net_Err_No (0x38A128),
MMBB_LOGIN (0x38A150) and BsLbsCount (0x38A12C); its calls go to Net_fade_kill, cnLbc_LoadNetModel /
cnLbc_LoadModelWait (lobby 0x5B2C20 / 0x5B2C30), memset and strncpy. The server side: `tools/test_online_server.sh`
with the shared fill code.

**First real run in PCSX2 (agent B, 15 Oct 2026) [checked]: the patched disc logs in to mh1-server, reaches the plaza
and the town, and meets a PC client there (each sees the other, chat both ways).** Test: `tools/test_ps2_online.sh`
(hidden PCSX2, scripted key presses; ~6 min). What it took and what was seen, in order:
* **PCSX2's network without root**: DEV9 Ethernet in **"Sockets" mode** (`EthApi = Sockets`, `EthDevice = Auto`,
  PCSX2.ini `[DEV9/Eth]`; `tools/run_pcsx2.sh --net`). PCSX2 answers the guest's DHCP itself and turns the guest's TCP
  into ordinary host sockets: no TAP, no pcap, no sudo. The guest reaches the server at the PC's **LAN address**
  (192.168.1.105 here; the server binds it), not 127.0.0.1; the server sees the connection coming from that same
  address. The guess above was right.
* **Input**: PCSX2's pad 1 bound to keyboard keys (run_pcsx2.sh writes `[Pad1]`), keys sent into the hidden Xvfb
  display with XTEST (`tools/pcsx2/xkey.c`, built by run_pcsx2.sh; no xdotool needed). Circle confirms, cross cancels.
* **Title -> network**: CONTINUE, slot 1, the hunter (the PC port's save on a fresh card, `--card-from`), then
  "街へ行く" on MODE SELECT is the network mode. The game asks for a **NET file** on the card ("NETファイルがありません");
  "yes" makes it (slot 1), then a terms notice (進む), then the game's "ネットワーク接続" screen: "ネットワーク設定の編集"
  starts **Sony's own network settings program** (from the disc; English with the US BIOS): Add Setting -> slot 1 ->
  guide -> SCE/Ethernet (Network Adaptor) -> PPPoE not required -> IP Auto (DHCP) -> DNS Auto -> name "Setting 1" ->
  save -> no connection test -> quit. **Quitting restarts the game from the disc** (intro, auto-load); from then on
  the game finds the NET file and the setting ("1.Setting 1"); picking it connects.
* After the NET file is loaded (or made): the autosave notice, on some runs a menu "ネットワーク接続 / 前回と同じ接続 /
  ログインID削除" (its first entry), then the terms notice (進む), then the connection screen.
* **DNAS**: skipped by the patch, no error, no DNAS traffic [checked: the server's log follows at once].
* **Server table / login**: the patched internet_browser filled it; tcp_init connected to 192.168.1.105:10200 and the
  6101 login with the 8-digit key and the 16-character password was accepted (`lobby: login ... ok`), then 6102,
  6103, 600E x4, 6131. No in-game browser page was opened (the open question above: answered, none for this flow).
* **First login of a new hunter**: "ユーザー登録/変更" (Capcom user registration: name and contact data for events);
  "登録しない" (do not register) goes on. Then "カプコンＩＤ選択" with three empty slots: the first one, "yes" to a new
  ID -> the server makes a new hunter id. Both screens are the game's own; nothing is sent beyond the lobby protocol.
* **Plaza**: "mh1-serverTest Plaza 1" (the server table's name and the plaza name run together; the PC shows "Local
  test serverTest Plaza 1" the same way: cosmetic, both clients). 街に入る lists "mh1-serverTest Lobby 1-4", 0 people;
  the quest column shows "(null)人" **[checked on screen]**. Both were our server's data, fixed 16 Oct (agent B): the
  client prints the names as "%s%s" (our names now start with a full-width space), and the lobby list shows the
  lobby join-user answer's two numbers (6303: all, in rooms) through a table of single digits: the server sent
  (count, 100), index 100 past the table. Now each 0..9 (docs/agents/agent-B.md "Small fixes round").
* **Town**: lobby 1 -> the town square (stage 0x4C), the hunter at the fountain; the server logs the PS2's binary
  records (types 1, 2, 3, 16) in lobby 1.
* **PS2 <-> PC**: an ONLINE=1 PC client (headless, its own fresh account and card, `RT_NET_HOST` = the LAN address)
  logs in and enters the same town. The PS2 shows the PC's hunter "PCHUNTER" walking on the square next to its own;
  the PC log shows the PS2's hunter "ＴＥＳＴ" on stage 76 "shown" (screenshots both sides). The PC's chat line
  reached the PS2 (its chat log: "PCHUNTERが街に向かっています / 到着しました", the line, "街から出ました" when it left); the
  PS2's line typed on its software keyboard (△, kana, 決定) reached the server and the PC's chat log
  (`chat log: from "ＴＥＳＴ" ...`). **The first PS2 <-> PC meeting works.**
* Not tried in this run: rooms, matching, mail, friend search, the line check over minutes (the session stayed up
  ~10 minutes without a drop), a second PS2, a real PS2.
* Gotcha for anyone scripting this: an account's town place is kept by the server; a PC test client that left the
  town in the guild hall comes back there (stage 0x4D) and does not see a hunter on the square. Fresh accounts in the
  test. Another: ffmpeg in a background script needs `-nostdin` (it stopped on SIGTTIN and hung the first test run).
**What is not in step 1: the hunt.** After matching, a PS2 connects to the 6916 game server address and speaks
`mcsls` records (section 5.5, docs/server.md 2.4 / 4.2), while mh1-server's relay and the PC / Xbox ports speak
net_peer frames. So a patched PS2 reaches the lobby, the town, rooms and matching, but **cannot hunt yet**. Step 2:

### 3.7 Step 2: in-hunt parity (the plan; built in 3.10)

1. **The faithful game-server relay** (`tools/server/mcs.py`, docs/server.md 4.2, milestone S7): sends 0x1031, reads
   the 0x82 login (`cmcs_04`: `mmbbc_encode(key, seq)`), finds the match by the decoded key, relays records
   `[u8 len][u8 cmd<<4 | sender]` to the other members (checking the sender id), health checks 0x1021, answers or
   ignores 0xF0 keep-alives. Open: the exact header of 0x1031, what 0x1032 means, whether the old server consumed
   keep-alives (all from the client code: `CnInetCheckMcsPacket2`, `mcsls_recv` / `mcsls_send`, main
   0x22C670-0x23A000).
2. **The lobby's hand-off for PS2s**: today 6914 says `mh1-relay` and 6916 gives a net_peer relay port, which a PC
   understands; a PS2 ignores 6914 and speaks mcsls to 6916. So 6916 must point at the mcs relay for any room with a
   PS2 in it (or for every room once the PC speaks mcsls, 3 below). The two can share one port: a net_peer joiner
   sends HELLO at once, a PS2 waits for the server's 0x1031, so "who talks first" tells them apart **[guess, cheap
   to test]**.
3. **The PC / Xbox side** (DECISIONS.md prefers this): link the game's own mcsls stack (`mcsls_*`, `Cng*`,
   `CnInet*Mcs*`, `aqcmd`; decompiled, not linked in the PC build) for online hunts instead of net_peer.c, so PS2, PC
   and Xbox all send the same records. Needs: the missing near-matches of that range as PC C, the `CpInet*` TCP calls
   (already there), the 0x82 login; then `tools/test_online_server.sh` against mcs.py instead of relay.py. net_peer
   stays for direct-connect co-op without a server.
4. The alternative, a translator in the relay between mcsls records and net_peer frames (both carry the same AQ
   packets; mcsls adds its own sync / ping / fragment layer), is more risk and keeps two protocols.
5. Tests: PC-only first (two PC clients over mcsls through mcs.py, the same hunt as test_online_server.sh), then the
   owner with PCSX2 / a PS2 and a PC in one room.

### 3.8 Memory-card patch route (agent B, 15 Oct 2026: research from the code, then run in PCSX2; verdict in 5 below)

Question: can a one-time homebrew on the player's PS2 put **our** patch (the DNAS skip in ms01.c and our
`internet_browser`, i.e. what `tools/ps2_patch.py` changes) into the game's own memory-card patch slot, so that the
plain original disc applies it by itself at every boot? Sources: matched C (`mc/mccard_nm.c` CardAtld,
`mc/mcsave_nm.c`, `net/patch01.c`, `net/patch02.c`, `net/ms04.c` ms_net_patch_set, `ime/imeb.c` Load_overlay,
`fl/flsys02.c`), the asm of PatchExecCS, and a local disassembly of the two DNAS overlays from the disc (not kept, not
committed). No key material was found or needed for this answer; none is in this section.

**1. Boot path: the saved patch is applied long before anything network or DNAS runs [checked].**
* `system_hard_init` (flsys02.c) reboots the IOP with `cdrom0:\IOP\MODULES\DNAS280.IMG` at start-up: Sony's DNAS
  IOP image is the IOP kernel for the whole session, so its IOP side is there with no network at all.
* Title auto-load: `CardAtld11` (card read OK, `decode_to_ck` = the save's own scramble + checksum, which homebrew can
  compute from mcsave_nm.c) -> `save_data_sub(0, 0x1F)` -> `mc_copy_patch(0)`: card image +0x208 (0x10000 bytes) to
  `patch_buff`, +0x10208 (0x40 bytes) to `patch_buff+0x20000` (the "tail": 10-char version, 4-char tag, u32 length at
  +0x14). Then `PatchLoadinDNAS_Init`, and `CardAtld14` runs `PatchLoadinDNAS_Main` until it finishes.
* `PatchLoadinDNAS_Main` (patch02.c): tail length 0 -> nothing to do. Else copy half 0 to half 1, load overlay 2 of
  the network slot (`Net_put_overlay_data(2)`, = dnas_ins.bin at 0xA06200), call `InetDNAS2PersonalDecode` (0xA2FD30)
  on half 1 with the stored length and the limit 0x10000; > 0: `PatchExecCS(0, 0)` (main's entries); < 0:
  `PatchInitCS` (patch_buff and the tail length cleared in RAM).
* After that **every** `Load_overlay(ov, slot)` (imeb.c) calls `PatchExecCS(slot, ov)` right after loading and
  `mwOverlayInit`, before the overlay's code runs. So entries for lobby.bin are applied each time the lobby loads
  (and again after a hunt). Entry key = `slot << 4 | overlay`; main = key 0 [guess, strong: the boot call
  `PatchExecCS(0, 0)`, no overlay is loaded in slot 0]; lobby = probably 0x13 [guess: ms_net_patch_set applies
  `PatchExecCS(1, 3)` right after a decode in the network menu; game.bin is `Load_overlay(2, 1)` = 0x12].
* The DNAS check (`ms_network_bb_authentication`, main) only runs when the player picks the network game, minutes
  later. A main-code entry patching it is in place by then. **Order is fine.**

**2. The decryption: Sony's DNAS-inst "personal" (console-bound) format [checked for the calls; key internals guess].**
* `InetDNAS2PersonalDecode` (Capcom glue in dnas_ins.bin) is a step machine over Sony calls:
  `sceDNAS2InstInit_nohdd()`, `sceDNAS2InstExtractDataLength_nohdd(stored length, buffer, &plain length)` (error if
  the plain length > 0x10000), `sceDNAS2InstExtractData_nohdd(..)` (decrypts in place), `sceDNAS2InstShutdown_nohdd()`.
  Any library error -> -1 -> `PatchInitCS`.
* The game also has the other direction: `InetDNAS2PersonalEncode` (0xA2FBD0) ->
  `sceDNAS2InstPersonalizeDataLength_nohdd` / `sceDNAS2InstPersonalizeData_nohdd`. ms_net_patch_set (network menu,
  after a login that received a patch, 5.8) runs: the patch from 6122 in patch_buff -> `InetDNAS2ConnectInstall`
  (dnas_net.bin: `sceDNAS2InitNoHDD` with the title's auth info, then `sceDNAS2AuthInstall`, which talks to Sony's
  DNAS server; its output replaces the buffer, length in `D_6E9700[0]`) -> load dnas_ins -> **personalize on the
  console** (`InetDNAS2PersonalEncode`; the result's length goes to tail+0x14) -> version and tag into the tail ->
  `SaveGameFileNet2` -> decode it again and `PatchExecCS(0, 0)` / `PatchExecCS(1, 3)`.
* Key material: not in Capcom code and not on the card. The save holds only the encrypted blob and the 0x40 tail
  (mcsave_nm.c layout) [checked]; nothing from a DNAS authentication is stored there. The keys live in Sony's code:
  the EE library is self-decrypting/obfuscated (its strings too) and the IOP part is inside DNAS280.IMG; "personalize"
  binds the data to the console (its unique IDs) [guess, from the function names and the known DNAS design; whether it
  also binds to the memory card is unknown]. A card moved to another PS2 fails to decode; the game then clears the
  patch in RAM and the next save writes the cleared slot back, erasing it [checked: PatchInitCS + mc_copy_patch(1)].
* Checks at boot: only the library's own results (the extract calls; whether the container carries a MAC or RSA
  signature inside is unknown: dnas_ins links RSA-BSAFE code including `AI_PKCS_RSAPublic` [checked that it is
  linked, guess what it is used for]), then the plain text must start with `"M-HUNTER"` (else nothing applied). The
  6121 byte sum only checks the download, never the card [checked]. If the tail version is newer than `MediaVersion`
  it is copied into `MediaVersion`; either way the entries are applied [checked].
* **Can code on the same PS2 make a blob the game accepts?** It hinges on one unknown: what
  `sceDNAS2InstPersonalizeData_nohdd` takes as input.
  * (a) If it takes the plain data (i.e. `sceDNAS2AuthInstall` returned the decrypted patch and personalize only
    re-encrypts it for this console), then the game's own code can personalize **our** plain list on the player's
    console, and no DNAS server and no prior DNAS authentication are needed. This is the reading the call order
    suggests (online step -> local encrypt -> save -> local decrypt -> magic check on the result) [guess].
  * (b) If it takes a server-issued container (still wrapped by Sony's server key), only Sony's DNAS server could
    produce the input: impossible now, on every console.
  * Either way we never need the per-console data from an official authentication: the decode path uses no network
    and no stored authentication data [checked], so case (a) would work on consoles that never went online.

**3. Format limits [checked].**
* Plain list (after decode, in patch_buff+0x10000): `"M-HUNTER"`, u32 count, then entries {u32 key, u32 RAM address,
  u32 length (bit 31: a u32 source address follows instead of the bytes; copy from RAM), bytes, padded to 4}. At most
  0x10000 bytes plain; the encrypted form must also fit the 0x10000 card slot (the personalize overhead is unknown,
  small [guess]). The 0x20040 buffer is two 0x10000 halves + the tail; only half 0 and the tail are saved.
* Overlays loaded later: yes, through the per-load `PatchExecCS` above (the lobby is re-patched at every load).
* Our patch: the DNAS skip (at most the 0x574-byte range of ms_network_bb_authentication, or a few bytes for a
  minimal `r = 1`) + the 720-byte internet_browser (code + settings) + 12-byte entry headers: under 2 KB, far inside
  the limit. One personal blob per player (the server, login and password are inside, as with ps2_patch.py).

**4. Verdict before the test (superseded by 5): possible with a condition, not proven.** Possible if `sceDNAS2InstPersonalizeData_nohdd` accepts plain
data (case a); not possible if it needs the server's container (case b). The code cannot settle it (Sony's code is
obfuscated); one run on a PS2 settles it. Even in case (a) the result is per console (moving the card to another PS2
loses it), and the player needs a way to run homebrew once (FreeMcBoot, uLaunchELF, OPL, or similar).

Installer steps for case (a) (nothing built). Simplest is to let the game do the work itself:
1. A small ELF, started once from homebrew with the MH1 disc in the drive, loads SLPM_654.95 from the disc (no Capcom
   bytes shipped) and applies a few RAM patches before jumping to its entry point.
2. Install mode: at title (after the auto-load), it writes our plain `"M-HUNTER"` list into patch_buff (built by
   ps2_patch.py from the same rebuild: ms01 entry with key 0, internet_browser entry with the lobby key), sets the tail
   (version = the disc's own `MediaVersion` string, so nothing reported to a server changes), and starts
   ms_net_patch_set at its state 0x14 (load dnas_ins -> `InetDNAS2PersonalEncode` -> tail -> `SaveGameFileNet2` ->
   decode -> `PatchExecCS`). If the decode gives back `"M-HUNTER"` the game itself has proven case (a).
3. The player then removes the homebrew and boots the plain disc: auto-load decodes and applies the patch, the network
   menu skips DNAS and connects to our server. A save on another PS2 erases it; running the installer again restores it.
Test it first in PCSX2 (it runs the real DNAS280.IMG; the console IDs come from its BIOS dump), then on a PS2.

**5. Test in PCSX2 (15 Oct 2026): case (b). Personalize does NOT take plain data [checked].**

Setup (all tools committed, nothing Capcom's): PCSX2 v2.8.2 (official AppImage, `~/ps2dev/pcsx2`, outside the repo) on
its own Xvfb display, software renderer, no audio, EE/IOP console to a log: `tools/run_pcsx2.sh`. BIOS: the owner's
dump, SCPH-39001 (US, v1.6), with no NVM/MEC file (PCSX2 makes a default one). The disc: the owner's Japanese .iso in the
drive, the executable replaced by a test build with `-elf` (`tools/ps2_pdtest.py`: `src/ps2patch/pdtest.c`, MWCC, linked
over the dead ms_net_patch_set body, and PatchLoadinDNAS_Main's first two instructions turned into a jump to it; test
only, not built by tools/build.py). Memory card: a fresh image holding the PC port's save (`tools/pcsx2/savecard.c`,
the PC port's own export), so the title's auto-load reads a save and calls PatchLoadinDNAS_Main once, as on a PS2.
`tools/test_pdtest.sh` runs the two boots (~2 min). Logging through the game's own `printf` works (lines `PDTEST`).

Boot 1 (install build; at the title, after the auto-load): a 32-byte plain list (`"M-HUNTER"`, 1 entry, key 0, writes
`"PDTESTOK"` at 0x26A2E0) in patch_buff, dnas_ins.bin loaded with `Net_put_overlay_data(2)` (as PatchLoadinDNAS_Main
does), then the game's `InetDNAS2PersonalEncode` (func_A2FBD0) called every frame as ms_net_patch_set's state 0x15
calls it, **without** InetDNAS2ConnectInstall / sceDNAS2AuthInstall. Its steps and the sce results it stores
(`dnerrno_0038AA50`, what func_A2FBC0 returns; length in `dndst_len`):

| Glue step | Sony call (addresses in dnas_ins.bin) | Result |
|---|---|---|
| 0 -> 10 | `sceDNAS2InstInit_nohdd` (0xA20710) | 0 (OK) |
| 10 -> 11 | `sceDNAS2InstPersonalizeDataLength_nohdd` (0xA267F8; size, buffer, &length) | 32, output length **0** |
| 11 -> 15 | `sceDNAS2InstPersonalizeData_nohdd` (0xA26848) | 0 |
| 15 -> 100 | `sceDNAS2InstShutdown_nohdd` (0xA20740) | (not stored) |
| 100 | the glue returns its length, 0 | 0 = "not finished": the glue starts again at step 0 and loops forever |

So the library "succeeds" with an empty result: nothing is encrypted, nothing could be saved (the test stops there;
boot 2 with the probe build reads the card and logs "tail length 0, nothing to do"). Why, read in the library
(unobfuscated entry code): both personalize calls feed the buffer through a read callback (0xA268F8: copies from the
buffer at 0xA319A0, position 0xA319A4, size 0xA319A8) into a parser (0xA23430) whose first act is to read a
**0x580-byte (1408) header**; with fewer bytes it gives up at once (what our 32 bytes got). The rest of the parser is
self-decrypting code (not read). Second run with the same list behind a 0x580-byte zero header (`PAD=0x580
tools/test_pdtest.sh`): PersonalizeDataLength now reads the header and **rejects it: result -1**, the glue fails
(-1), as ms_net_patch_set would show a DNAS error. So the input must be a Sony DNAS container with a 1408-byte
header (the output of `sceDNAS2AuthInstall`, i.e. of Sony's DNAS server), not plain data: **case (b)**. Making such a
header would need Sony's DNAS key material and format (signatures; RSA-BSAFE is linked), which we neither have nor
want. The boot side (decode) is the same library and needs the personalized form of such a container.

What this does not depend on: the BIOS region (US BIOS, Japanese disc; Init and the calls run normally, the -1 is the
header check) and the NVM (console IDs only matter once there is a container to bind) [reasoned, not tested with a JP
BIOS]. What was not tested, because nothing could be made: boot 2 decoding a saved patch (PatchLoadinDNAS_Main ->
PatchExecCS); the probe build is ready for it (`PDTEST target ... after PatchExecCS(0, 0)`).

**Recommendation: the memory-card launcher (below) is the route for players without OPL**; the one-time installer
is dead (no plain-data personalize, and no DNAS server to make a container). The patched image of 3.6 (OPL, PCSX2)
stays the simplest route. Left open, not ours to pursue: MH Oldschool answers the DNAS hosts for unpatched discs
(2.1), so someone has a working DNAS server answer for this title; whether that includes `AuthInstall` for patches is
unknown, and we do not use their servers.

**Fallback, built (agent B, 15 Oct 2026): the memory-card launcher `MH1LAUNCH.ELF` [checked in PCSX2; not yet on a
PS2].** Sources: `src/ps2patch/launcher.c` (the ELF), `src/ps2patch/launch_hook.c` + `src/ps2patch/online.c` (the
in-game blob); build: `tools/ps2launch/build.sh` with the open ps2sdk (prebuilt ps2dev release, outside the repo;
neither the disc nor the PS2 rebuild is needed: the blob links against `tools/ps2launch/syms.ld`, addresses only).
Player's guide: docs/PLAYING.md. No DNAS crypto of any kind; only RAM patches of the game's own code.
* **Pass-through (safe for "auto launch" at every power-on)**: it reads the disc's SYSTEM.CNF (BOOT2). No disc / not a
  PS2 disc -> back to the browser (`Exit`). A PS2 game not in its table, Monster Hunter G (SLPM_658.69, in the table
  with no patch yet), or an MH1 executable whose size / CRC32 differ -> started untouched through `rom0:PS2LOGO` (as the
  console boots a disc). Checked in PCSX2: MH G's disc -> "no online patch for this game yet" -> PS2LOGO -> G's intro;
  MH1 with the test key `test_force_passthrough = 1` -> PS2LOGO -> SLPM_654.95 unpatched.
* **MH1 (SLPM_654.95, 5648908 bytes, CRC32 0xCF1A62C0)**: the launcher (linked at 0x01800000, out of the game's way)
  reads the executable from the disc, checks size and CRC32, loads its segments, then patches RAM; every patched
  word is first compared with the expected original instruction (else: untouched):
  1. 0x26827C in ms_network_bb_authentication: `jal InetDNAS2ConnectNetStart` -> `addiu v0, zero, 1` (DNAS
     "authenticated"; the same effect as ps2_patch.py's recompiled function, 4 bytes instead of 932);
  2. ms_net_patch_set (0x2687E0, the "patch download" menu; our server never sends a patch) -> `jr ra; v0 = 1`; its
     dead body from 0x268800 holds the blob (984 bytes: mh1p_conf with the player's settings at 0x268808, mh1l_hook,
     mh1p_fill_server_table, mh1l_internet_browser = online.c's internet_browser);
  3. 0x23E5EC in Load_overlay: `jal PatchExecCS` -> `jal mh1l_hook`: PatchExecCS as before, then, if the overlay just
     loaded is lobby.bin (CRC32 of the 720 bytes at 0x5B5980 = the original internet_browser), the first word of
     internet_browser becomes `j mh1l_internet_browser` (+ nop) and the caches are flushed. So every lobby load (also
     after a hunt) is patched; disc and card are never written.
  Then IOP reset and `ExecPS2(entry, argv[0] = "cdrom0:\SLPM_654.95;1")`.
  The blob is gcc (n32) code called from MWCC (EABI) code: the same argument registers for up to four integer
  arguments, the same saved registers, no small data (-G0); it calls the game's memset / strncpy / PatchExecCS and the
  lobby's cnLbc_LoadNetModel / Wait at their addresses.
* **Config** `MH1LAUNCH.CNF` (text: `server = HOST:PORT`, `login = `, `password = `), next to the ELF (argv[0]'s
  folder) or `mc0:/MH1LAUNCH/`, `mc1:/MH1LAUNCH/`. MH Oldschool's addresses and the original service's names are
  refused (the same list as ps2_patch.py). No config -> the game starts untouched.
* **Test** (`tools/test_ps2_online.sh --launcher`): PCSX2 boots MH1LAUNCH.ELF (as `host:MH1LAUNCH.ELF`, config read
  from `host:MH1LAUNCH.CNF`) with the **unpatched** .iso in the drive: patched in RAM, then the same run as 3.6 -
  login on mh1-server, plaza, town, the PC client in the same town, each sees the other, chat both ways [checked].
* **Known limit**: the game's own network-settings program (Sony's "Add Setting", started from the network menu)
  restarts the game from the disc when it quits, which drops the RAM patch. After making a setting the first time,
  restart the console (or the launcher). Also: FMCB / OpenTuna auto-launch was not tried (no console here), and a
  real PS2 run is still to be done by the owner.
* Not done: a table entry with patches for MH G (its addresses differ; the table has the slot), and the "lost key"
  fragility of the scripted test (timed key presses; checked twice in a row, see test_ps2_online.sh's comments).

### 3.9 Three platforms in one town (agent B, 16 Oct 2026)

`tools/test_three_way.sh` (about 10 minutes, PCSX2 and xemu at once, both hidden, run uncapped): one mh1-server on
this machine's LAN address (invite-only, three fresh accounts), then
* the PS2: the owner's unpatched disc in PCSX2 through the memory-card launcher (3.8), the card with the NET file from
  a test_ps2_online run; the same scripted screens as 3.6 up to lobby 1's town;
* the Xbox: the online build in xemu on a scratch disk image whose settings.ini points at the LAN address
  (`online_server = 192.168.x.y:10200` and the account; xemu's NAT reaches the host's LAN address too);
* the PC: build/pc/mhview_online headless (RT_NET_HOST etc.).
All three stand by the fountain in a short column (the PC walks up behind the PS2, the Xbox steps left in front of it:
each camera follows its own hunter and the chat window covers the lower left, so a column keeps everyone in view;
RT_LB_WARP is no use here, it moves the hunter only on its own screen, nothing is sent until it walks); the PC and the Xbox each say a line (RT_NET_SAY), then the PS2 types "いう" on its
software keyboard. Each platform's screen is taken right after (the PC: its own shot just after the PS2's line arrived,
since its chat window closes after a few seconds) and put side by side: build/show/three_way/three_way.png. Checks: the
server got all three lines; the PC shows the Xbox's and the PS2's hunters and got both their lines. [checked, 3 runs on
16 Oct 2026, all OK; in the last each screen shows all three hunters by name and all three chat lines]
* Order matters: the PS2 goes first and alone (its key presses are timed); xemu starts once the PS2 is in the town.
* PCSX2 cannot run under `ulimit -v` (it reserves several GB: "Failed to allocate fastmem area" in its log, black
  screenshots). That was the "no terms notice" failure of test_ps2_online under full_check (agent D): the cap, not the
  game. Both scripts now refuse to start under a cap.

### 3.10 Step 2: hunts on the game's own protocol, PC / Xbox / PS2 (agent C, 16 Oct 2026)

Decision (docs/DECISIONS.md, 16 Oct): the ports run the PS2's own session code. `mh1_server.py serve --lobby-mcs`
answers 6914 with "mh1-mcs" and 6916 with its **mcs relay** (tools/server/mcs.py, one port, default 10298); at 6916
the lobby registers the match's logins there with their player numbers (6912 - 1). rt_online.c sees "mh1-mcs" and
hands the match to src/pc/rt/rt_mcs.c instead of net_peer.c.

What the PC does after the match, with the game's code (rt_mcs.c's header has the details):
1. `internet_to_modem` -> `cmcs_00`..`cmcs_06` (lobby): close the lobby socket, connect to 6916's address, wait for the
   server's 0x1031, answer with the 0x82 login, `LobbyToMcsInit` (players, own number), `AQ_init`,
   `AQSession_init_online` -> `CngSessionStart_online` -> `mcsls_init`;
2. `AQ_exec` until `AQSession_wait`; `pl_num = AQ_join_num_get()`;
3. Plsel's handshake: `Copy_user_id`, `sel_default_set`, the ready flag, `net_send_sys(2)` until every player's flag
   came (each sends its hunter id and select data; slot 0 the quest), once more, 30 frames;
4. the PC's own stage load; the other hunters' look from the lobby's mini data by the hunter id each sent
   (`Set_mini_data_to_pl`, as `init_pl_work`);
5. `net_start_ck` (sys 3 rounds; the network delay from mcsls' ping) until it agrees;
6. every tick `AQ_send` + `AQ_exec` (mcsls_move: records in and out, pings, keep-alives; the AQ packets to
   `self_data_ctrl` -> `net_receive_pl` / `_em` / `_sys` / `_host`).
The ONLINE=1 build links aq_nm.c, aqcmd_nm.c, cng_nm.c, cngmsg_nm.c, cnmsg_nm.c, mcsls_*.c, netsyn04.c,
netsyn07_nm.c and the cmcs_* lobby files; rt_np.c's `AQ_data_put` / `net_start_ck` / `mcsls_force_drop` pick the mcs
or the direct-connect path (the game's are renamed `mcs_*` in build_pc.sh). The clock (`CngNetTimeGet`, the EE timer
on the PS2) is the host's; `CnInetNetworkAveTcpPoll` says "connected".

Found on the way [checked]:
* The 0x82 login's payload is the 10 digits alone (no length in front; cmcs_04 uses `SetSendStringData`).
* Three register pass-throughs that gcc cannot follow (tools/argregs.py --check): `ck_exec_aq` (aq_nm.c) forwards its
  three arguments to `CngNetAQcommandExec`, `AQ_data_put` -> `pl_data_put` -> `send_my_data` the same,
  `CngNetAQDataTrans2Work` (cng_nm.c) the packet's flag byte to `CngNetAQdataToObj`; written out in the _nm files
  (the PC crashed in `CngNetAQcommandSort` before).
* `system_w+0x35` ("loading") stops `AQ_exec` entirely; the title's edit screen leaves it set on the PC (the PS2's
  game11 / game12 clear it): rt_mcs.c clears it. A PS2 does not poll its session at all while it loads a stage
  (DROP_TIMEOUT is 30 s, `mcsls_timeout_default` 100 s): the relay allows 120 s of silence.
* `AQ_data_put` refuses packets while the AQ timer is 0: the first `net_send_sys(2)` needs one `AQ_exec` before it
  (Game_task's step 4 runs one before game0).
* mcsls' sync / ping / start rounds need nothing from the server except passing the records on: no record waits for
  everyone to be connected (sync flag 0 is repeated every 30 frames; ping and flag 2 are only sent once all said 1).

Tested: `tools/test_online_mcs.sh` (two PCs, 127.0.0.1, ~4 min): login to the town, ANNA's room (quest 1, 2 players),
BOB joins, both log in to the mcs relay (players 0 and 1), mcsls reaches state 5 (sync, 8 pings, start), the select
handshake completes, `net_start_ck` agrees (delay 2 frames), each sees the other at the same position (np-pos), the
monster's state from its owner (HP, action), ANNA delivers, BOB sees the clear, both back in the town. The relay saw
the record kinds 0x20/0x30 (16 pings), 0x40/0x70 (app data), 0x90 (sync), 0xF0 (keep-alives).

**PS2 and PC in one hunt** (`tools/test_ps2_hunt.sh`, PCSX2 hidden + one PC, ~20 min, uncapped, by hand; not in
full_check) [checked, 16 Oct 2026]: the owner's unpatched disc through the memory-card launcher, mh1-server
`--lobby-mcs` on the LAN address. ANNA (PC) posts quest 1 for 2 at the counter, the PS2 joins it at the quest board
and gets ready at the departure spot, ANNA starts. Both log in to the mcs relay (the PS2's own cmcs_04 0x82 login is
accepted by mcs.py), mcsls sync / 8 pings / start, the select handshake (sys 2 both ways), net_start_ck agrees (the
PS2 needs ~340 frames to load the stage), and both are in the forest camp: **the PS2 shows ANNA next to its hunter and
both names in its party list; the PC shows the PS2's hunter "TEST", and sees it walk when the PS2 walks**; ANNA
delivers the quest's item and **the PS2 gets the clear and its reward screen** (quest events over sys 6). Screens:
build/show/ps2_hunt/shared_hunt.png (PS2 at the camp | the PC at the camp | the PS2's reward screen). A monster
fight with the PS2, its hand-overs, drop-outs and four players: section 3.11.
**Xbox and PC** (`tools/xemu_mcs_hunt.sh`, xemu hidden, ~10 min) [checked]: the same flow with the online Xbox build
joining (RT_LB_WARP as the PC tests, settings.ini for the server); the Xbox sees ANNA, gets the clear and its reward
screen; the PC sees XBOX (build/show/xemu_mcs/shared_hunt.png).

**PS2, PC and Xbox in one hunt** (`XBOX=1 tools/test_ps2_hunt.sh`, PCSX2 + xemu + PC, ~25 min) [checked, 16 Oct
2026]: a room for 3; the mcs relay has players 0 (PC), 1 (PS2), 2 (Xbox); every screen shows all three hunters at
the camp and all three names in its party list (build/show/ps2_hunt/three_platform_hunt.png: PS2 | PC | Xbox); the
PC's delivery clears the quest on all three.

* Found by this test: the PC said "loaded" (sys 3) once, while the PS2 was still in game10; the PS2's game11 clears
  those flags at the end of its load, so it waited at "クエストを開始します" for ever. rt_mcs.c now repeats it six
  times (once a second) after its own start was agreed.
* Test aids for the PS2 (no game code changed): a PCSX2 cheat file (`tools/run_pcsx2.sh --cheats`, pnach RAM writes)
  holds the hunter on the guild hall's door in the square, puts it at the quest board once in the hall, at the
  departure spot while L2 is held, and sets the guild registration flag (event flag 4) as the PC tests'
  RT_NET_REGISTERED does; without it the board's square does nothing. In the hall the hunter must not be moved every
  frame (it could not walk off); PCSX2 applies cheats by the CRC of the first ELF (the launcher's), run_pcsx2.sh
  installs the file under both.
* The room flow on the PS2, for anyone scripting it: square on the door (into the hall), square at the board, circle
  through the list and the paper, "参加しますか" yes, OK; then at the departure spot square ("出発準備"), circle; the
  leader starts with square at the same spot once everyone is ready (before that the counter only says so).

### 3.11 A monster hunt with a PS2: the Kut-Ku (agent C, 16 Oct 2026)

Quest 4, "怪鳥イャンクック襲来！" (the fifth card of the guild's ★ quests: dleft on the first card; one Yian Kut-Ku, kind
6, 2000 HP, starting on stage 36 of the forest; contract 200z). Run with `HUNT=kutku tools/test_ps2_hunt.sh` (PS2 in
PCSX2 + one PC, ~25 min, uncapped, inside `flock .../xemu.lock`) and, PCs only, `KUTKU=1 tools/test_online_mcs.sh`
(~9 min).

**What the PS2 test does** (quest ticks, 30 a second, from the PC's quest start): both follow the Kut-Ku (the PC's
`RT_PL_GOTO=60,f`, the PS2's aid walking the same area exits); the PC fights alone (600-1500, its damage x25 by
`RT_DMG_MUL`); the PC goes back to the camp (1500) so the monster passes to the PS2, which fights it alone (1550-2700,
no multiplier); the PC comes back and hits it (2600-3300) while the PS2 still owns it and no longer attacks; the PS2
goes to the camp (3300) so the monster comes back to the PC, which kills it; both carve; the clear, the reward screens
and the town on both.

**The PS2's test aid** (tools/pcsx2/ps2_hunt_aid.py): PCSX2's PINE socket (memory read / write, `PINE=slot` in
run_pcsx2.sh), about 30 times a second: the hunter's vital back to 100, "goto stage" / "follow" / "fight" (next to the
monster, facing it; at its carve point once dead) as the PC's RT_PL_* aids, the area exits read from the PC's
RT_GOTO_TRACE log (no stage data copied). And a trace in the PC's formats: `np-hp` (every change of the monster's HP,
with "owner" = whether the PS2 runs it, "master" = the owner slot, EMW+0x8C3 / +0x88E), `np-em`, `np-pos`. Keys
(xkey): right stick up (attack) while it fights, circle once the monster is dead in its own trace. Only RAM is
touched; the game code is the disc's.

**Result** [checked, 16 Oct 2026, one run; its screens were overwritten by the next run (the script now takes
`OUTDIR=`); the same moments are in the runs below]:
* Owner hand-over both ways, the same on both machines: PC (slot 0) -> PS2 (slot 1) when the PC left the area,
  PS2 -> PC when the PS2 left it.
* HP: the same sequence on both (2000 ... 1578 -> 1574 ... 1565 -> ... 205, 40, 0); the PS2 shows only values the PC
  had, and the PC only values the PS2 had while it owned the monster (tools/hunt_sync.py).
* Damage from each player counts: the PC's hits while it owned it (x25); the PS2's own hits while it owned it
  (1578 -> 1568, 2-4 HP a hit: a starting hunter's blade on the Kut-Ku, no multiplier); the PC's hits while the PS2
  owned it (1568 -> 1565 while the PS2 did not attack). The PC's x25 did **not** apply to those: the hit was computed
  on the PS2, the owner. That confirms the guess in section 1a: whoever owns the monster computes every hunter's hits
  (the others replay their attacks there); there is no "damage dealt" packet.
* The kill on the PC (the owner then); both carved: the PC 3 items, the PS2 4 ("怪鳥の甲殻を入手しました" on its
  screen); the clear, the reward screens, both back in the town (the PS2 saved to its card).

**Bugs found and fixed on the way (PC side, any online quest that came from the town)**:
* The first area change crashed both PCs (SIGSEGV in hit_data_expand from body_hit, monster kind 12 on stage 39):
  `game_w+0x1DC` ("in the lobby") stayed 1 from the town, so `body_hit_sub_new` took the town's body tables
  (D_610370 in lobby.bin) for the quest's monsters. The PS2's `game_init` clears game_w at the quest; the PC's online
  quest start now clears it (viewer.c). The in-quest menus also took their lobby variants before.
* Carving did nothing: the carve motion (0x1A3) ended at once because the town had loaded its own common motions
  (lbcom_tbl) and the quest kept them. The online quest start now loads the quest's (plcom_tbl, `com_motion_load(0)`,
  as rt_village_leave does). Gathering and other plcom motions were affected the same way.
Neither showed in the earlier tests: their hunts never left the camp or picked anything.

New test aids: `RT_PL_CARVE=n` (rt_player.c: the script's circle is held back while the target lives, then only circle
every n ticks for 50 s once it lies dead: the carves), `RT_PL_GOTO2` stage -1 (back to RT_PL_GOTO, e.g. following
the monster again), the `np-hp` trace and `items` / `vital` on np-pos (RT_NP_EM / RT_NP_POS), tools/hunt_sync.py.

**Two PCs** (`KUTKU=1 tools/test_online_mcs.sh`) [checked, same day]: the same hand-overs (ANNA -> BOB -> ANNA), HP,
kill, both carve.

**A player dropping out mid-hunt** [checked, 16 Oct 2026, one run each]:
* PC killed (`KUTKU=1 LEAVE=1 tools/test_online_mcs.sh`: BOB's game SIGKILLed at quest tick 2100 while he owned the
  Kut-Ku and ANNA was at the camp): ANNA's session marks him gone (mcsls' time-out, `pl_state` 0xFF: "mcs: player 1
  dropped out"); when she is back on the area her game runs the monster, she kills it, carves, clears and returns to
  the town.
* PS2 closed (`HUNT=kutku DROP=ps2 tools/test_ps2_hunt.sh`: PCSX2 stopped at quest tick 2200 while the PS2 owned the
  monster): the same on the PC (dropped, took the monster over, killed it, carved 6 items, the clear, the town).
  build/show/ps2_hunt_drop_ps2/.
* As in the original: with only one player left, `Em_Master_Change` just takes the monster over (`n < 2`:
  EMW+0x8C3 = 0) and leaves the owner number (+0x88E) at the player who left; nothing is handed on, nothing breaks.
  mcs.py does not tell the others that a member left (the client has no message for it; it notices the silence).
* Xbox stopped: in the four-player run below (xemu stopped at quest tick 4800, mid-fight, not the owner: the three
  others went on; the PS2's party list drops XBOX).
* (The PS2 run's own final check still expected the owner number to come back; the logs pass the corrected check,
  `hunt_sync.py --masters 0,1 --stay PC --runs PC`, now in the script.)

**Three and four players** [checked, 16 Oct 2026, one run each; `HUNT=kutku PC2=1 XBOX=1 DROP=xbox
tools/test_ps2_hunt.sh`, ~45 min, `XBOX_FILES=` the owner's Xbox files; free memory stayed above 2.5 GB with PCSX2,
xemu and two PCs, the 800 MB watchdog never fired]:
* PC (ANNA, slot 0), PS2 (slot 1), PC 2 (BOB, slot 2), Xbox (slot 3) in one room, one mcs session; every screen lists
  all four hunters; the PS2 takes the monster from the PC and gives it back; all four show the same HP values (no
  machine ever shows one no owner had); the Xbox is stopped mid-fight; PC, PS2 and BOB kill it, all three carve
  (3, 4, 3 items), clear and return to the town. build/show/ps2_hunt4b/kutku_four_platforms.png (top: the PS2 with four
  names while it owns the Kut-Ku, the Xbox with four names, the PC's fight; bottom: BOB's screen, the PS2 after the
  Xbox was stopped (three names), the PS2 seeing ANNA and BOB at the dead Kut-Ku).
* An earlier run where the Xbox came to the board too late (xemu runs slower than real time while PCSX2 runs; the
  leader's start is now later with four players) was a three-player hunt (PC, PS2, PC): the monster went PC -> PS2 ->
  BOB (Em_Master_Change hands it to the next slot after the leaving owner that is on the area), all three carved and
  cleared. build/show/ps2_hunt4/kutku_three_players.png.
* PS2 closed mid-hunt: build/show/ps2_hunt_drop_ps2/kutku_ps2_drop.png (the PS2 fighting as the owner; the PC with the
  PS2's hunter; the PC alone after PCSX2 was closed).

**What the server's 0x1032 means** (read from the client, mcsls_recv, main 0x231FF0-0x232050) [checked]: the client
turns every server packet (0x28 ...) into a record of kind 0x60 and reads its command: 0x1021 (health check) sets
`send_health_ans`, **0x1032 has its own case that does nothing**, anything else is ignored too. cmcs_04 / 05 / 06
never wait for an answer to the 0x82 login either (the session starts right after sending it). So 0x1032 is a server
command the client knows and ignores; what Capcom's server meant by it (an acknowledgement of the login is the likely
guess: 0x1032 = 0x1031 + 1, and mcs.py sends it right after the login) cannot be read from the client. mcs.py keeps
sending it: harmless, and closer to the original if it was the login's answer.

**Retire the "mh1-relay" mode? Recommendation (for the coordinator / owner; nothing removed)**: yes, in two steps.
* What it is: `mh1_server.py serve --lobby-relay` answers 6914 with "mh1-relay" and puts every player of a lobby match
  on relay.py, which passes the PC's own net_peer frames; rt_online.c's `matched()` has a branch for it. PS2s cannot
  use it (they speak mcsls), so a server in that mode keeps PS2 players out of PC / Xbox rooms.
* The mcs mode (`--lobby-mcs`) now does everything the relay mode was for, with every platform: no player needs an
  open port, the server is in the middle, monster fights, hand-overs, carves, drop-outs and the return to the town
  work (3.10, 3.11), and it is the game's own protocol, so it is what a real PS2 / Xbox / PC mix needs anyway.
* Step 1 (now, small): make `lobby-mcs = yes` the default in tools/server/mh1-server.conf.example and docs/server.md
  ("Run your own server" still says lobby-relay), keep `--lobby-relay` working as a fallback.
* Step 2 (after): remove the lobby's "mh1-relay" hand-off (lobby_stub.py's 6914 string, rt_online.c's relay branch,
  test_online_town.sh's relay part) once (a) the owner has run a real PS2 / Xbox through mcs, (b) the Windows build
  passes test_online_mcs.sh under Wine (only the relay path was tried with it, 10 Oct), and (c) mcs.py has relay.py's
  per-member limits (rate / burst / backlog / idle; mcs.py has only the sender-id check and a 120 s silence limit).
* Keep relay.py's stand-alone session (`serve --session`, a game started with `--join SERVER`) and net_peer.c's direct
  connect for now: they are the no-lobby / LAN co-op paths, not lobby matches, and test_coop.sh relies on them.
  "hostleave" through the relay (the room's leader leaving) is a case to repeat on mcs before step 2: there, drop-outs
  of the owner were tested (above), not of the room leader (slot 0) with three players left.

### 3.12 Monster Hunter G online (agent C, 18-26 Oct 2026, round 9; GB7, in progress)

G (SLPM-65869) uses the same lobby-server protocol as MH1 (same framing, login, obfuscation, `LOCKROCK`, sequence
rules; `cnLBS_LoginLobbyServer`, `mmbbc_encode`, `tcp_init`, `create_server_table` are identical), with a command table
of 293 rows (MH1 258; G lobby.bin `lbs_command_tbl_h/l` 0x63B910 / 0x63BA40, `lbs_fromto_tbl` 0x63BB70,
`lbs_category_tbl` 0x63BCA0, `lbs_command_jmp` 0x63BDD0) and a different town structure. Everything below was read
from G's client (`tools/mhg_dis.py lobby ADDR`, G addresses; lobby.bin loads at 0x542600) and checked against the real
G disc in PCSX2 on mh1-server unless marked [code only]. Nothing was checked against Capcom's or MH Oldschool's server.

**The PS2 side (step 3 of the task, done first because the real client is the reference):** MH1LAUNCH patches G in
RAM the same way as MH1 (section 3.8): DNAS call in `ms_network_bb_authentication` (G 0x212B7C) -> `v0 = 1`,
`ms_net_patch_set` (G 0x2130B0-0x214754, only `lbc_login_patch` calls it) becomes `return 1` and holds the blob
(linked at 0x2130D0 with tools/ps2launch/syms_mhg.ld), `Load_overlay`'s call of `PatchExecCS` (G 0x1F198C) goes to
the hook, which points G's `internet_browser` (lobby 0x5A2910, 768 bytes, CRC32 0x9482CFD7) at our server-table fill.
G's `internet_browser` has the same steps as MH1's (one more `MMBB_LOGIN == 3` case we never take); `bsCsvWork`'s
layout is the same (G `tcp_init` / `create_server_table` identical). G finds and uses the NET file and the network
setting MH1's test card already holds ("NETファイルが見つかりました"; G's connection screen adds プロキシー設定).
[checked: the real G logs in, picks a Capcom ID, downloads the info file, shows the plaza menu and walks in the town
(build/show/g_online/ps2_g_town.png), 20 Oct 2026]

**How mh1-server tells the games apart:** G has no 6102 (telephone): its 6101 handler (G 0x5AF470) answers the
connection pair and sends **61A0** at once. The server holds MH1's 6102 back 0.5 s; a client that sends 61A0 is G
(`Client.game = "mhg"`). Also different: the first data (6103) says client kind **7** (MH1 4) and the version string
is G's build date **"0411210839"** (MH1 "0401290111"; `MakeMediaVersion` from `media_date_tbl` / `media_time_tbl`).
G and MH1 never share a plaza, lobby, room or user search (`Registry` keys every place by game); an account's hunters
are per game (accounts.py schema 3: column `game`), so a G save shows its own three Capcom ID slots.

**G's login, in order** (codes as on the wire; S = server, C = client):

| Code | Dir | Payload | Notes |
|---|---|---|---|
| 6101 | S req / C ans | as MH1 | |
| 61A0 | C req `u8` (7) -> S ans `u32, u32` | the byte is first-data byte 2 (CnetSys_w+0x10D2) | the answer goes to 0x734DEC/F0 and the login callback gets event 12, which G's callback ignores; meaning unknown, we send 0, 0. Then S sends 6103 |
| 6103 | S req -> C ans | `0, 1, 7`, version "0411210839", 8 `u16` | |
| 600E | C req x4 | echo, as MH1 | |
| 6131 | S notice | the account's G hunters | |
| 61A1 | C req (empty) -> S ans `u32 a, u32 b` | a != 0: `MCSLS_SWIN_LIMIT = a / 60 * 30` (the in-hunt send window); b != 0: `DROP_TIMEOUT` (float, G 0x2EBF58, MH1's neighbour of MCSLS_SWIN_LIMIT) = b / 60 | server tuning of the hunt session [code only for the effect]; we send 0, 0 = the client's defaults |
| 6132 / 6104 / 6190 | | as MH1 | |
| 6148 | C req `u8` login kind (CnetWork+5) -> S ans `u8 r` | r = 0: nothing; 1-3: the server asks for a 336-byte record, 4: a 44-byte one (sub_main 0x4C8F50 / 0x4C8F90, from User_data); the client sends it with **6149** (`u8 kind, u8 r, obfuscated record`, ans `u8`) and asks again | rankings / records upload [guess]; we answer 0 and keep nothing |
| 614C / 6141 | | as MH1 | |
| 6881 | C req -> S ans `u8 n`, n x `u32 size` | **n must be >= 1**: with 0 files and no file 0 kept from before, G logs out (lobby 0x581210 step 99 -> `To_LogOut(4)`) | |
| 6883 | C req `u8 i` -> S ans `u8 i, u32 size, u32 version` | per file; G keeps the 8 bytes (mission_area+0x23F4 + 8 i) and downloads again only when they differ | we send a CRC as the version |
| 6882 | as MH1 | file 0 to mission_area+0x2414; files 1-3: 512 bytes each to quest_title+0x30 (event quest headers; the whole file later on demand, `Lbc_DownloadQuest` G 0x581610) | |
| 68A0 | C req -> S ans `u8 n` (<= 3), 4 bytes | news; for each k < n: **68A1** C req `u8 k` + the 3 last bytes -> S ans `str` (to file0+2504 + 1280 k) | we send n = 0 |
| 670F | C req `u8` -> S ans (empty) | [meaning unknown] | |
| 6891, 6203.. | | current place, plaza list as MH1, but plaza reads ask the explanation too (mask 15) | |

**G's info file ("file 0", 2504 bytes, made by tools/g_info.py):** read by the plaza and the town menus. Layout
(offsets in the file): 16 town types x 32 bytes (name at +8, Shift-JIS, sent to the server when picked); 0x208 a text
colour per type; 0x22E 5 classes x 16 bytes "type open in this class"; 0x27E 5 x (lowest, highest) of the hunter's mini
data byte 1 allowed per class [guess: hunter rank]; 0x288 5 x 256-byte class help texts; 0x788 / 0x888 two more texts;
0x988 64 bytes unread. Capcom's own file is unknown to us; the content is ours.

**Plazas ("エリア") and classes ("ランド"):** after the login G shows a class menu (5 entries, lobby 0x57DDC0; picks
class k only if the hunter's mini-data byte 1 is within the file's limits) and enters a plaza whose explanation starts
with the class's digit ('1'..'5', PlazaInfo+91). mh1-server's G plazas: one per class. The plaza menu (G Plaza 1:
街に入る / エリア移動 / ランド移動 / フレンドリスト / プレイヤー検索 / クエスト検索 / ハンターズサイト / イベント情報) [checked].

**Towns (lobbies):** the town list asks per lobby: 6303 join users (G reads **u16 id + four u16**: shown as
"人数 a/b", "街中 c", "クエスト d"; MH1 reads three), 6304 status, 6302 name, **630D** (`u16 id` -> `u16 id, u32`,
flag 0x80 of G's 356-byte lobby record) and **630B** (`u16 id` -> `u16 id, u8`, flag 0x10) [both: meaning unknown, 0].
Entering: **6305 carries `u16 lobby, u16 2, u16 0`** (MH1: the lobby only), then 630A member list, the file download
again, 6303; the town's game data (6708 types 2, 3, 15, 16 ...) as MH1 [checked: the real G walks in our town].
Other new rows [code only]: 6310 (`u16` -> empty, sets the entry floor), 6312 / 6313 / 6314 (`u16` -> `u8`), 6311,
6315, 6316, 6317 (empty answers; 6315-6317 send an obfuscated string), 630C (`u32` -> empty), 640C notice, 6415-6417,
6707, 6710, 6711 (user / condition searches), 6010 notice (server message, callback 45), 6144-6146.
Dropped from MH1: 6102, 6142 (bill estimate), 6110-6112 (regulation), 6210-6215 (annexes), 6703 (search, now 6710).

**The guild hall, a room and a quest** [checked 20 Oct 2026, the real G disc alone: `HUNT=1 tools/test_ps2_g_online.sh`,
build/show/g_online/ps2_g_counter.png, ps2_g_online_quest.png]: the guild hall is stage 0x4D as in MH1; G's counter has three
ladies (the middle one: quest levels ★..★★★★★ by HR range, ????, イベントクエスト; the right one only for top hunters) and
needs the guild registration (event flag 4, User_data+0x24 bit 4 as in MH1; G User_data 0x32C210). Making a room asks
the same as MH1 (6401 count, then per room 6403 / 6404 / 640B / 640D / 6405 / 650A, 6407 create, the rules 6603 / 6607 /
6601 / 6602 / 660F / 6604 / 6606 / 6605 / 6608 / 660E, 660B the player count) plus **6415** (`u16 room, 4 bytes`, e.g.
`01 00 02 01`; empty answer; [guess] the quest's attributes for the board) before 660C, then 640A / 6403 / 6412 and
**6509 with G's property: `u16 hunter value (User_data+0x1A) << 16 | u16 quest`** (lobby 0x55BBC8; MH1: quest << 1 |
HR << 9). Rooms' join users (6403) carry four u16 in G too (members, max, 0, 0 from us). The departure spot is (2920,
1840) in the hall ("□: クエストに出発", then yes); the match (6504 / 6508 / 6910 / 6911-6917 / 6914 "mh1-mcs" / 6916),
the logout (6002) and the **0x82 login to mcs.py** are as MH1's, and G loads the quest (森のキノコ狩り, the Forest and
Hills camp) on our game server. Not tried yet: two G players in one room, a monster, the return to the town.

**The PC G online build** (`ONLINE=1 tools/build_pc_mhg.sh` -> build/mhg/pc/mhview_g_online). The lobby client's
network layer takes G's command indices and work layout (`MHG_NET`, include/g_alias.h; `LBNET_G` maps CnetSys_w to
CnetSys_g, 0xA8 below G's symbol), G's own answer handlers are src/lobby/cnet/cnet_g.c (gnet_* names per table row in
config/mhg/pc_names.txt), and the receive loop walks G's 293 table rows (MH1 258: 68A0 and later were dropped before).
G's lobby screens are G code the PC does not have (about 570 functions reachable from internet_lobby_act that G edited
or added): tools/mhg_gonl.py writes PC C for each from m2c (src/lobby/gonl/gen, one file per function, linked over
MH1's copies; HAND-FIXED files are kept). What m2c output needed for the PC (all in the tool now):
- G's ABI: arguments 5-8 in $8-$11 (m2c's `mipsee` target; tools/draft.py names those registers a4-a7);
- argument counts: a context of every known signature (the generated functions, the PC's main / cnet packet helpers)
  in build/mhg/gonl_ctx.h; without it m2c lists only the registers set before a call, positionally (a1 alone became
  the first argument); a parameter list that starts at arg1 gets an unused first parameter;
- m2c's pointer arithmetic is in bytes (pointer locals and globals become u8 *, reads keep their width through casts);
  `(s64) (x << 0x38) >> 0x38` is a cast (as written it is undefined and GCC dropped a loop);
- CallBackWaitInit (a leaf that keeps $a1-$a3) is inlined for m2c, so values held across it survive;
- G names that the split took from MH1 by place but are other code get G_RENAME names (the file download burst's
  result callback, the 6205 / 620A plaza handlers);
- G data the PC imported at MH1's size: quest_lv_tbl / flag_quest_tbl (8 levels, MH1 6) in config/mhg/pc_data.txt;
  G's lobby work area (main 0x2EF390, app_mem_top + 0x126200 on the PS2) is a host buffer set in rt_online.c.
More G code under MH1 names that is other code (G_RENAME in the tool): the plaza / town record getters
(cnLBS_Get_RoomStatus / mhRoomJoinUser read a plaza record in G, cnLBS_Get_PlazaStatus / mhPlazaJoinUser a town record);
G's cnLBS_Read_RoomProperty sends 630D, not 650A (src/lobby/cnet/cnlbs.c, MHG_NET); main data pointing into lobby.bin
(G's plaza menu lists, main 0x2EEE70) is also written into the host copies the code reads (rt_main_lobby_patch_named);
G's 14 fades. What m2c cannot know is now taken from G's asm: the access width of data reached by a computed address
(`addiu r, %lo(SYM)` then lb / lh / lw through r), and calls to generated G functions pass all four argument registers
(G hands $a1-$a3 on unnamed). One-line corrections live in the tool (FIXUPS), so regenerating keeps them.
State (22 Oct): on the PC, G's own screens: login, Capcom ID select, info file download (6881 / 6883 / 6882), the
plaza class menu, the plaza menu, its town list (G Lobby 1-4) and the town: 6305 / 630A / 6708, G's Lb_stage_load puts
the hunter at one of G's five town starts (stage 0x4C) and the town is drawn. Also found on the way: m2c drops a tail
jump's delay slot (a store, an argument) - tools/draft.py now runs the slot first; G's 0x59A820 (check_sender0 in the
split) is Lb_send_pl_status(a0, 0). Test: tools/test_online_g.sh (TWO=1: two G PCs).
State (23 Oct): **two G PCs see each other in the town** [checked: `TWO=1 tools/test_online_g.sh`, each client's trace
lists the other in stage 0x4C at the start position]. Found on the way, all G-only:
- G's lobby events are numbered differently from MH1's. The notice handlers the PC shares with MH1 take G's with
  `GV()` / `CNEVT()`: the town / room commer and leaver notices (6410 / 6411 and the room ones, `_sub_InOutRoomMember`,
  G 0x5AC9A0: events 26 / 27 / 36 / 37 / 41 / 42 for kinds 0-5; MH1 38 / 39 / 31 / 32 / 36 / 37), 6005 match cancel 6
  (MH1 7), 6007 lobby full 7 (MH1 8). Every other G row handler was compared with the MH1 C of its name.
- The split's names of the TU chat sends are crossed: G 0x5B08B0 is cnLBS_Send_ChatBinaryTU (row 0x102 = 670D, as MH1),
  G 0x5B0690 is G's whisper (`u8, id, text, len, callback`, row 0xFD, the u8 sent last). A newcomer gets the others'
  status as 670D -> 670E: type 3 (stage), type 10 (`s16 x, s16 z, u8 dir, u8 ?, u8 stage pair, u8`; lobby 0x59AB90).
- The PC G client reaches the guild hall (stage 0x4D) through the town's door (24 Oct: its NPCs are placed and drawn,
  the counter works, see below).
State (24 Oct): **two G PCs make a room at the guild hall counter, join it and start a quest together** [checked:
`ROOM=1 tools/test_online_g.sh`, test server, both load quest 1 at the Forest and Hills camp and see each other]. What G
does differently in the room (all read from G's code, then seen on the wire):
- The room count of the town is kept where MH1 keeps the lobby count; the per-room reads are 6403 / 6404 / 640B / 640D /
  6405 / 650A (val 0xBB). G's room records (G 0x6DE120, 0x15C bytes; the split's RoomInfo symbol is 4 bytes in) hold
  the quest (+0), members (+6), maximum (+8, from 640B's FIRST u16), status (+0x16, 3 = recruiting), password (+0x17),
  message (+0x5B). G's lobby-client room table entries (0x164 bytes) keep the status at +0x1E, the password at +0x1F,
  6403's four u16 at +0xA.., 640B's five at +0xE / +0x12 / +0x16 / +0x14 / +0x18 (in wire order), 6412 / 6506 (match
  entry users) at +0x1A / +0x1C.
- The rule codes 6601-6610 mean what they mean in MH1 (6603 rule count, 6607 choices of a rule, 6601 / 6602 / 660F name /
  password / message permission, 6604 caption, 6606 current, 6605 permission, 6608 / 660E choice name / control; 6609 /
  660A / 660B / 6610 / 660C set); only the table rows moved. The split's names of several G senders are other G
  functions (its "RoomCreate" send is the 630D request, "RoomSetFinish" the 6881 request, "RoomEntry" a 6305 send).
- Room create 6407 (u16 room), then the rules, then **6415** (`u16 room, u8 quest low, u8 quest high, u8 attribute,
  u8 attribute`; G 0x5BC860, e.g. `0001 01 00 02 01`) before 660C, then 640A / 6403 / 6412 and 6509 (property: hunter
  value << 16 | quest). Joining: **6406 `u16 room, obfuscated password`** (G 0x5BBF60), then 640A / 6403 / 6412.
- Ready / depart: a member squares at the departure spot (6504 1, G dialog 103), the owner squares (G dialog 91 when
  everyone is ready, 89 otherwise) and starts the match (6508); then 6910 / 6911-6917 / 6915 / 6914 / 6916 / 6002 as MH1.
The PC parts: src/lobby/cnet/cnet_g.c (G's room handlers, getters and sends), G's lobby functions generated or hand-fixed
in src/lobby/gonl/gen (tools/mhg_gonl.py: G_RENAME for G code under MH1 names, FIXUPS, tex_slots), lb_guild_g.c (the
counter's online steps), lb_village_nm.c (departure), config/mhg/pc_data.txt (LobbyInfo / PlazaInfo / quest_title at G's
sizes). Details and the list of fixes: docs/agents/agent-C.md "G online, round 5".
State (24 Oct, round 6): **two G PCs hunt the Kut-Ku together on mh1-server's mcs relay and return to G's guild hall**
[checked: `HUNT=1 tools/test_online_g.sh`, mh1-server `--lobby-mcs` with accounts: quest 4 posted at the counter, the
mcs login of both, the monster handed ANNA -> BOB -> ANNA with the same HP on both (tools/hunt_sync.py), the kill, both
carve, the clear, the result screens incl. G's online hunter point screen, the relogin, both standing in stage 0x4D].
G's in-hunt protocol on the wire is MH1's (the PC runs MH1's mcsls / AQ C for G and the relay is unchanged); what G
does differently, found on the way (all [code only] unless the test shows it):
- G's mcsls keeps MCSLS_SWIN_MINUS in bss and sets it at session start from the word 8 bytes after MCSLS_SWIN_LIMIT
  (G 0x1E56F0: 100; MH1 stores 8 in MINUS itself). With 0 the send-window check counts up every tick and the session
  ends with error 0x10 after MCSLS_SWIN_LIMIT (450) ticks [checked: the PC's sessions died so before the fix].
- G's AQ_init calls CngNetAQInit instead of MH1's CnInetNetworkAveTcpConfigSet (no ave_cnf in G).
- G's quest_w moved MH1's 0x180.. block (the quest messages of sys 6: clear, everyone's end bits) to 0x280; the room
  owner waits at quest command 0x2D until every member's bit came [checked: the owner hung there with MH1's offsets].
- G's sys 2 (select) packet has another layout than MH1's: the ready flag goes to select_w+0xAC, the quest to +0xD0,
  the hunter id from byte 18 (net_receive_sys G 0x4FB210, net_send_sys G 0x4FAE40). Round 6's PC G client still sent MH1's
  layout (fine between PCs); since round 7 it sends G's (below).
- cmcs_03 takes the player count from cw+0xBB1 and the game-server ticket from cw+0xFA6C (MH1 0x2C47 / 0x35E0).
- After the quest G logs in again and saves the hunter to the card (lbc_login_finish_after, McOperationSet(7)); then the
  party stands in the guild hall (stage 0x4D), not in the town.
State (24 Oct, round 7): **the PC G client sends and reads G's own in-hunt packets** (the PC's net_* C is MH1's; every
packet the AQ channels carry was compared with G's twin, send and receive side; tools/mhg_fdiff.py main FUNC):

| Packet (AQ ch 10 sys / pl / em / chat / host) | G vs MH1 | PC G build now |
|---|---|---|
| sys 1 player count | same | - |
| **sys 2 character select** (net_send_sys G 0x4FAE40 / net_receive_sys G 0x4FB210) | **0x1E bytes (MH1 0x18)**: slot, sw[0x66+s], weapon byte / u16 / byte, stage, ready, sw[0xC+s], sw[0xB6+s], **u16 quest at 14** (MH1 a byte at 10), sw[0xD2+s] at 16, G-only sw+0xE8 / u16 +0xE4 / u16 +0xE6 at 17 / 18 / 20, **the hunter id at 22** (MH1 16); slot 0 hands out the stage, quest and the G-only three | G's layout (netsyn08.c / netsyn09.c, VERSION_MHG) |
| **sys 3 start round** | **+ s16 at 10**: the value G keeps per slot at 0x2EF718 (only net_receive_sys kind 3 writes it, so in practice 0) | sent and stored |
| sys 4/11, 6 (quest, quest_w 0x280 block since round 6), 7/10 (items), 8, 9, 12 | same layout | sys 5's target is game_w+0x1C2 (was a guess, 0x1B6) |
| sys 5 item box | same layout | - |
| pl (net_send_pl identical; net_receive_pl) | same layout; G's receive calls a new item helper (G 0x4C64D0) before Pl_item_stack on an item hand-over | layout as is; the helper is not ported [code only] |
| em (net_send_em / net_receive_em / net_receive_em_act) | same layout; **G treats kinds 50 and 45 like 7 and 15** (the bytes at 0x21/0x22, 0x1E/0x1F) | EMK_7 / EMK_15 (netsyn.h) [which monsters: unverified] |
| chat (net_send_chat; receive identical) | same | (the PC's chat send is a stub) |
| host (identical) | same | - |

Also G, around the packets: net_start_ck sends its keep-alive (sys 8) every 300 frames (MH1 60), clears the network delay
at round 0 and has no 60-frame cap (netsyn07_nm.c); game_w's start-round fields are 0xDA / 0x190.. (GWX, include/gwx.h:
identity placeholders before); Copy_user_id writes the hunter ids at game_w+0x1F6 (MH1 0x1E8, which is x1E2 / x21B /
x21E in G); Set_mini_data_to_pl reads **G's mini data layout** (s32 +0x14, bytes +0x19 / +0x18 / +0x1A, the weapon s16s
at +0xA.., the 6 look bytes at +3; G 0x4C67B0). select_w moved field by field (G arrays have 10 slots): include/swx.h
(SWX) has the map and serves plsel_nm.c, gfs_nm.c, rt_mcs.c, rt_np.c and the packets. The lobby server and the mcs relay
keep mini data and records opaque (tools/server/accounts.py stores 0x40 bytes, mcs.py counts record kinds), so they
need no change; tools/mhg_lobby.py reads nothing of these packets.
[checked: `HUNT=1 tools/test_online_g.sh` with two G PCs; against a PS2 G: not yet, see below]

**pNet (the lobby's network window state) in the G online build:** G re-laid it out (prims first): MH1 0x00 s16 -> G 0x18,
0x02..0x05 -> +0x20, 0x06..0x0F -> +0x21 (G 0x26 is G-only), 0x11..0x13 -> +0x20, the four prim pointers 0x14..0x23 ->
0x00..0x0F (G has a fifth at 0x10), the s16s 0x24 / 0x26 / 0x28 -> 0x1C / 0x1E / 0x20 (`tools/mhg_addrmap.py lobby 0 0
--ptr pNet`, checked in G's lbc_login_id_select / lb_select_room_list / lbc_top_menu_00). PNET_OFS (include/lbsys_g.h)
is that map in the online build (MHG_ONLINE), where G's generated lobby code shares pNet with MH1's lobby C; LB_NETW /
LBNETW_B take G's layout there. The plain G and the PS2 builds keep the identity.

**G save cards:** the HUNT test makes a G card per hunter once (the village bed, offline) and starts each client from a
copy, so G's relogin save after the quest writes the card [checked: "memory card: saved", the card loads back with the
hunt's money].


State (25 Oct, round 8): **the real PS2 G and a PC G hunt the Kut-Ku together** [checked: `tools/test_ps2_g_hunt.sh`, the
owner's G disc in PCSX2 through MH1LAUNCH and the PC G online client on mh1-server `--lobby-mcs`, five passing runs]: the
PC posts quest 4 for 2, the PS2 joins it at the left counter and gets ready, the match, both on the mcs relay; the
character select completes on both, the PS2 loads quest 4 from slot 0, each machine gives the other's hunter its own
equipment and look (np-look: the PLW fields Set_mini_data_to_pl / Set_userdata fill, the same on both), the start round
agrees, the Kut-Ku's HP and owner (PC -> PS2 -> PC) the same on both (tools/hunt_sync.py), the kill, both carve, the clear
on both, both back in G's guild hall, the PC's card and the PS2's memory card saved after the relogin. Found by the real G:
- **G's online quests take their random numbers from the match's battle code** (6915, kept at cw+0xFA6C; lobby 0x583C14
  copies its first 15 characters to quest_rnd_host, which Quest_start uses online instead of its own: the start area, the
  monsters' level bonus, sizes); the info file's size limits go to quest_size_lim (lobby 0x59A258). The PC G online
  build had two host copies at each address (one written by the lobby, the other read by Quest_start); the real G started
  the Kut-Ku at 2200 HP, the PC at 2000. Fixed (one object; tools/mhg_gonl.py reuses pc_data's names). A server must give
  every member of a match the same battle code: mh1_testserver.py made it per request from the clock, now per match.
- (round 8, open then) the PS2's own hunter never hurt the Kut-Ku while the PS2 ran it: settled in round 9 below, a server
  bug (the info file's size limits), not the test aid.

**Round 9 (26 Oct 2026): the monster's size online.** The open question of round 8 was a real bug, in our server's G info
file [checked: a RAM dump of the real PS2 G mid-hunt (ps2_hunt_aid.py `--dump`): the Kut-Ku's EMW+0x9F8 (size) = 0 and
+0xB8 (scale) = 0.0, the Vespoids / Hornetaurs 100 / their base scale]. A monster of scale 0 is not drawn (the PS2's
pictures never showed the Kut-Ku, only its HP bar and the minimap), its hit spheres have no size (the PS2's hunter, which
the PS2 checks against its own monster, never hit), and its motions do not move it (it stood at one spot in whatever
state the AI picked while the PS2 ran it). While the PC ran it, the PC did the hit checks with its own copy, so both
machines' hits counted then. The chain:
- G's title menu "network mode" (mode_sel_end choice 2) sets system_w+0x44; with it set, every online quest rolls a size
  for each big monster (sub_main 0x4B7920 kinds; quest set-up in f_quest.c: header size x random size / 100), where offline
  only quests outside quest_size_flag_tbl (or with its event flag) do.
- The random size (sub_main 0x4B7810) is clamped to quest_size_lim[class] {min, max}; online, quest_size_lim is not reset
  from the offline table but comes from the info file: lobby 0x59A210 copies its bytes 0x998..0x9A1 there unconditionally
  after the download (also: 0x988 u16 = lbSendInterval, 0x98A / 0x98E / 0x992 three more u16 client values, each kept when
  0). tools/g_info.py left them 0, so max 0: size 0.
- Fixed: g_info.py fills 0x998 with {0, 255} for each class, i.e. no clamp: the game's own size distributions (75-125 %)
  decide. Our choice; what Capcom's server sent is not known. The PC G online build now sets system_w+0x44 with the network
  flag as G's title menu does (it only changed the training quests of quest_size_flag_tbl, which roll online on the PS2).
- Checked after the fix (`tools/test_ps2_g_hunt.sh`, which now also checks these): the Kut-Ku size 115 on both machines
  (np-size lines: the PC's RT_NP_EM, the PS2 aid's from RAM), visible on the PS2, the PS2's hit flag set by its own
  hunter while the PS2 runs it (np-dmg with the PS2 as owner, HP 926 -> 919 in that phase), owner PC -> PS2 -> PC, kill,
  carves, clear, saves, back in the hall. MH1 has no monster sizes and no such info file field: not affected.

**Round 9: a PS2 G as the room's leader** (`LEAD=ps2 tools/test_ps2_g_hunt.sh`): the PS2 posts quest 4 for 2 at the
middle counter (it needs the 200z fee: the test gives its hunter 5000z through PINE, as RT_MONEY does on the PC), the
PC joins at the left counter and gets ready, the PS2 starts the match from the departure spot; the PS2 is player 0 on
the mcs relay and runs the Kut-Ku first (its own hits land: np-dmg with owner 0), then the PC, then the PS2 again.
Learned on the way (test timing, not bugs): the first hunter on the monster's area runs it, whoever leads the room (the
PC, following at once, took it from the leader PS2); a monster whose owner leaves its area passes only to a hunter
already on that area, else the owner keeps it; the PC G client runs at about half speed in the guild hall while PCSX2
runs (the joiner's waits are long).

**Round 9: the Xbox G in the party** (`XBOX=1 tools/test_ps2_g_hunt.sh`): the G Xbox online build
(`tools/build_xbox.py --version mhg --online --gfx nv2a`, from the ONLINE=1 G PC build's tree; `tools/run_xemu.sh
--version mhg --online`) joins ANNA's room for 3 at the left counter: PS2 G + PC G + Xbox G in one Kut-Ku hunt, PCSX2 and
xemu at once under the emulator lock [checked: passing run; size 104 on all three, owners PC -> PS2 -> PC, HP the same on
the three, kill, three carves, the clear on all]. Fixed on the way (both games' runtime): a G lobby prim whose draw word
is not code (the PC already skipped it) held 0x1DD921, which on the Xbox is inside the XBE's text: the Xbox called it and
crashed 10 s after connecting. rt_code_ptr (rt_game.c) on Windows / Xbox now wants the exact start of a symbol of the
build. Seen: the G Xbox online build has about 12-17 MB free in xemu and draws at 7.5 fps in the guild hall (the game
ticks at 30); no card on its disk, so it plays a new hunter and its relogin save is not checked.

**Round 9: a G player leaving mid-hunt** (quest tick 4200): `XBOX=1 DROP=xbox` (xemu stopped; the PC and the PS2 kill,
carve, clear) and `DROP=ps2` (PCSX2 closed; the PC alone finishes, "mcs: player 1 dropped out") pass. A PC G leaving is
`test_online_g.sh`'s part (with the real PS2 left alone at 1x damage the Kut-Ku takes too long).

Open after round 9: the PC G client crashed twice (of 8 runs) in lb_select_set_data's map name sprintf (fault address
0x32) joining the real PS2 G's room; the server's answers were byte-identical to passing runs, so it is something on the
PC side (an RT_ONLINE_TRACE line there now prints the room / quest data for the next time). G's select_w 0xE4 / 0xE6 /
0xE8, sys 3's s16, event quests, real hardware.

Next: a PS2 G as the room leader, the Xbox G build in the party, event quests.

## 4. Run it

    ONLINE=1 tools/build_pc.sh                       # build/pc/mhview_online
    tools/test_online.sh                             # server + clients, ~12 s
    python3 tools/mh1_testserver.py -v --port 10200  # by hand
    RT_NET_PORT=10200 build/pc/mhview_online disc/mh1 --nettest full
    tools/test_online_town.sh                        # the screens, the town, a room, its co-op quest to the clear, the
                                                     # return to the town (~7 min; makes the hunters' cards once with
                                                     # tools/test_coop.sh saves, ~10 min)
    RUN=wine BIN=build/win/mhview_online.exe tools/test_online_town.sh   # the same with the Windows build (passes, 10 Oct 2026)
    tools/test_online_guest.sh                       # guest rooms, two clients against mh1-server (~2 min)
    tools/test_online_event.sh                       # an event quest downloaded, posted, cleared; a patch refused (~4.5 min)
    tools/test_online_server.sh                      # login to town to quest to town against mh1-server (relay) (~4 min)
    tools/test_online_mcs.sh                         # the same on the game's own in-hunt protocol (mcs relay, 3.10) (~4 min)
    HUNT=1 tools/test_online_g.sh                    # two PC G clients: room, Kut-Ku on the mcs relay, back to the hall (3.12, ~20 min)
    tools/test_ps2_hunt.sh                           # a PS2 (PCSX2) and a PC hunt together (3.10; uncapped, ~20 min)
    tools/test_ps2_g_hunt.sh                         # the real PS2 G and a PC G: room, Kut-Ku, clear, back in the hall (3.12, ~30 min)
    LEAD=ps2 tools/test_ps2_g_hunt.sh                # the same with the PS2 G as the room's leader (~35 min)
    XBOX=1 tools/test_ps2_g_hunt.sh                  # PS2 G + PC G + Xbox G (xemu) in one hunt (~40 min, 3.5 GB free)
    XBOX=1 DROP=xbox tools/test_ps2_g_hunt.sh        # the same, xemu stopped mid-fight (DROP=ps2: PCSX2 closed, PS2 + PC)
    XBOX=1 tools/test_ps2_hunt.sh                    # PS2 + PC + Xbox (xemu) in one hunt (~25 min)
    tools/xemu_mcs_hunt.sh                           # the Xbox and a PC (~10 min)
    HUNT=kutku tools/test_ps2_hunt.sh                # PS2 + PC fight the Kut-Ku: hand-overs, HP, damage, carves (3.11, ~25 min)
    KUTKU=1 tools/test_online_mcs.sh                 # the same with two PCs (~9 min)
    KUTKU=1 LEAVE=1 tools/test_online_mcs.sh         # BOB's game killed mid-hunt, ANNA finishes (~7 min)
    HUNT=kutku DROP=ps2 tools/test_ps2_hunt.sh       # PCSX2 closed mid-hunt, the PC finishes (~20 min)
    HUNT=kutku PC2=1 XBOX=1 DROP=xbox tools/test_ps2_hunt.sh   # PC + PS2 + PC + Xbox, the Xbox stopped mid-fight (~45 min)
    RUN=wine BIN=build/win/mhview_online.exe tools/test_online_server.sh   # the same with the Windows build (passes, 10 Oct 2026)
    RT_NET_PORT=10200 build/pc/mhview_online disc/mh1 --boot   # then the title's network mode (or: --quest 10 --play --online)

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

Chat (corrected 9 Oct 2026): the server sends 6702 to **every** member of the lobby, the sender too. Reason: when it
chats to the whole lobby (`cw+0x32BE == 0`), `Lb_send_chat` (lb_ad.c) sends 6701 and returns without adding its own
line; the client only logs what comes back (`CallBack_Event_ChatMessage` -> `Lb_chat_receipt` / `Plaza_chat_log_add`).

### 5.6 The town's game data, rooms and matching (verified on screen with the test server, 9 Oct 2026)

All formats read from the client (`src/lobby/cnet`); "meaning not known" fields are sent as noted and nothing on screen
showed a problem. `str` = `u16 length` + bytes; client strings are obfuscated (5.2).

| Code | Dir, cat | Payload | Notes |
|---|---|---|---|
| 6708 | C->S notice | one obfuscated string: `u8 type` + data (`lb_send_data`) | the town's game data: position (type 1 every few ticks, 0x13 bytes), status, stage, chairs, trades. Server: to every **other** member of the lobby as 6708 notice `str sender id`, `str data` (`_cnet_RecvFromLbs_NoticeChatBinary` -> `CallBack_Event_LbsBinary` -> `Lb_check_receipt(id, data)`) |
| 670B / 670D | C->S req | obfuscated id (6), obfuscated text / data (+ `u8` for 670B) | to one hunter: server answers empty, sends 670C (like 6702) / 670E (like 6708) to that id |
| 6401 | req -> ans | `u16 n` | rooms of the current lobby: the test server always says 8 (`Lbc_ReadRoomInfo` preset ids 1-8) |
| 6404 | req `u16` -> ans | `u16 id`, `u8 status` | **1 = free** (the counter creates in the first free one, `Lbc_ReserveRoom`), **3 = open to join** (`lbc_in_lobby_00_05`); anything else shows the server message |
| 6403 | req / notice | `u16 id`, `u16 members`, `u16 max` | max = the room's players (rule 0 below; the quest board shows "players members / max", single digits) |
| 640D, 6402 | req -> ans | `u16 id`, `str` | explanation (the recruiting message), name |
| 6405 | req `u16` -> ans | `u16 id`, `u8` | 1 = has a password |
| 640B | req `u16` -> ans | `u16 id`, 5 x `u16` | join info, meaning not known: sent `members, max, 0, 0, 0` |
| 650A / 6509 | req `u16` -> ans `u16 id`, `u32` / req `u32` -> ans empty | room property: `quest << 1`, the leader's rank `<< 9` (lb_guild_make_room); also pushed as 650A notice |
| 6407 | req `u16 room` -> ans empty | create (reserve) the room; the creator is its leader |
| 6603 | req `u16 room` -> ans `u8 n` | number of server rules (<= 32); see "Room rules" below |
| 6601 / 6602 / 660F | req `u16` -> ans `u8` | may the room have a name / password / explanation: 1 |
| 6609 / 660A / 6610 | req obfuscated string -> ans empty | name, password, explanation; 660B `u8 rule, u8 choice`; 660C (no payload) -> ans empty ends the set (`__cnet_bgProg_RoomSetRule`) |
| 6406 | req `u16 room`, obfuscated password -> ans empty | join; error result + `str` = refused (the client shows the text as an HTML dialog and stops; it never compares passwords itself). The test server refuses a wrong password and a full room. Notice 6503 to the others: `str id`, `str handle`, `str mini`; leaving 6501 -> 6502 `str id` |
| 640A | req `u16 room` -> ans | as 630A | room members |
| 6504 | req `u8` (1 ready / 0 cancel) -> ans empty | notice 6506 to the room: `u16 ready`, `u16 members` (the leader starts when ready + 1 >= members) |
| 6412 | req `u16 room` -> ans `u16 id`, `u16 ready`, `u16 members` | |
| 6508 | C->S notice (leader) | none | server: 6910 notice (no payload) to every member: `CallBack_Event_MatchStart` |
| 6911 | req -> ans `u8 n` | number of players | `cnLBS_Read_MatchInfomation` asks 6911-6916 in a chain |
| 6912 | req `u8 0` -> ans `u8 k` | this player's number, 1-based; `k - 1` = `USER_PL_ID` = the slot. The test server: the leader is 1 |
| 6913 | req `u8 k` -> ans `u8 k`, `u8`, `str id`, `str handle`, `str mini`, `str`, `u8` | player k |
| 6917 | req `u8 k` -> ans `u8 k`, `u16`, 5 x `u32` | meaning not known: `1`, zeros |
| 6915 / 6914 | req -> ans `str` | battle code (16 characters, kept at `cw+0x35E0`) / game rule (empty) |
| 6916 | req -> ans `str` 4 address bytes, `str` 2 port bytes (big endian) | the game server (`cnLBS_Get_GameServerAddress`). The test server gives the room leader's address and `--coop-port` (10300) + room: the PC's co-op host. **A PS2 would expect a real relaying game server here** (5.5) |

After 6916 the client logs out of the lobby server (6002, `lbc_game_ready_04`) and `internet_lobby_act` returns 0.

**Room rules** (11 Oct 2026; read from the client by a research pass, the server side tested with a fake client in
test_server.py and on screen with three game clients in tools/test_online_server.sh). The creator's client reads the
rules of the room it reserved (`Lbc_GetRoomRule`, `__cnet_bgProg_ReadRoomRule`), the joiner's client those of the room it
joins (`Lbc_GuestReadRoom`). All are requests with `u16 room` (+ `u8 k` rule, 0-based, + `u8 c` choice); every answer
echoes k (and c), the client files the answers by them (pipelined, any order); a missing answer hangs the read until
the 2-minute timeout. Strings from the server are plain `str`, at most 0x40 bytes.

| Code | Request | Answer | Meaning |
|---|---|---|---|
| 6603 | `u16 room` | `u8 n` | number of rules (an error result shows the server's message) |
| 6607 | `u16 room, u8 k` | `u8 k, u8 numof` | choices of rule k (<= 32) |
| 6601 / 6602 / 660F | `u16 room` | `u8` | may the room have a name / a password / an explanation (1) |
| 6604 | `u16 room, u8 k` | `u8 k, str` | rule k's name |
| 6606 | `u16 room, u8 k` | `u8 k, u8 now` | its current choice (the default for a new room; the room's for a joiner) |
| 6605 | `u16 room, u8 k` | `u8 k, u8 perm` | 1 = the creator may set it |
| 6608 | `u16 room, u8 k, u8 c` | `u8 k, u8 c, str` | choice c's name |
| 660E | `u16 room, u8 k, u8 c` | `u8 k, u8 c, u8 m`, m x 3 bytes (m <= 32) | meaning not known; 0 |
| 660B | `u8 k, u8 c` (creator, after the reads, for each rule with perm 1) | empty (ignored) | the creator's choice |

What the game does with them: the rule sheet (`lb_rule_seet_set`) has only its own rows (players 1-4, password,
message); server rules never appear on screen. `lb_guild_make_room` matches the sheet's players ("１人".."４人",
`lb_rule_member`, Shift-JIS) against rule 0's choice names and sets rule 0's choice to the match; every other rule keeps
`now`. So **rule 0 is the player count** when the server names its choices "１人".."４人"; nothing else in the client
reads a rule's value (no client-side rank, weapon or quest checks besides `check_room_require`: event quests, quest
0x65 and 0x67-0x6B and 4-star quests by the hunter's online rank, from the 6509 property). The test server and
mh1-server offer one rule, rule 0 "人数" with "１人".."４人" (default 4): the room holds that many (6403 / 640B max;
6406 refused when full: the quest board already shows "players 2/2" and the game's dialog shows the refusal). Other
rules can be added to `ROOM_RULES` in tools/mh1_testserver.py; the server would have to enforce them itself, and the
creator cannot change them. Passwords: the sheet's password (<= 8 characters) goes out as 660A only when 6602 said 1;
6405 tells joiners that a password is needed, the joiner types it (`join_input_password`) and sends it with 6406; the
server compares (the test server refuses "パスワードが違います。"). The room name (6609) is always sent empty.
Tested with game clients (tools/test_online_server.sh, 11 Oct 2026): ANNA's sheet "パスワード制限: あり", its field opens
the keyboard (type 3, 8 characters), the room gets the password; CARL types a wrong one at the join keyboard (type 0)
and gets the game's dialog "パスワードが違います。", BOB types the right one and joins, CARL's second try finds the room
full. The keyboards are filled by the test aid `RT_SK_TEXT="text;text;..."` (rt_menu.c: the n-th soft keyboard opened
gets the n-th text, as if typed on the host keyboard and confirmed; `RT_SK_TRACE=1` logs each keyboard).
Server messages (an error result's `str`, the 6706 administrator message, the 614C top information) are drawn by the
game's HTML text (`nwDispStr_Html` -> `Analysis_StringData` / `Analysis_TagCode` / `Display_StringData`): it draws
nothing unless the text starts with a tag, so servers send `<BODY>text<END>` (tags BODY, SIZE=n, COLOR=n, BR, CENTER,
LEFT, RIGHT, END, LF=n, C=n). On the PC those three functions were do-nothing stand-ins (every message dialog was an
empty frame); written from the asm on 11 Oct 2026, matched C since 12 Oct 2026 (src/lobby/f/lb_cli.c, the end of the
lobby-client TU; check_halfcode is still asm on the PS2, its C for the PC is in src/lobby/f/lb_online_nm.c). The test
server sends its administrator message at the first lobby entry (at the login the client is still behind the login
screens' held fade-out and the dialog would sit under it); circle closes it.
Fixed on the PC on the way: `nwDispStr_Html` (the server message's HTML dialog) had the m2c signature (f32, int):
the string pointer was a float's bits (crash on the refusal); `cnWrap_FontDisp` (lb_bz155.c) took (string, x, y)
where the callers pass (x, y, size, string) (crash on the admin message).

### 5.7 Line check, current place, user search, mail, administrator message (test server, 10 Oct 2026)

| Code | Dir, cat | Payload | Notes |
|---|---|---|---|
| 6001 | S->C req, empty; C->S ans, empty | line check | the client logs out ("connection lost", `To_LogOut(6)`) when nothing came from the server for 0xE10 ticks = 2 minutes (`internet_lobby_act`, `cw+0x35F4`): a hunter alone and idle needs it. The test server sends it every 30 s (the real interval is not known) |
| 6891 | req empty -> ans `u16 plaza`, `u16 lobby`, `u16` (0) | current place | asked at every login (`lobby_return_to_lobby`); zeros = none: the top menu. After a match the client logs out without leaving the lobby; the test server remembers that place by hunter id and gives it at the next login with that id (the return after a quest), and puts the hunter back into the lobby (6411 to the others): the client does not send a lobby entry then |
| 6703 | C->S req: obfuscated id (6) -> ans `str id`, `u16 plaza`, `u16 lobby`, `u16 room`, `u8`, `u8`, `str message` | where a hunter is | `cnLBS_SerchUserPlace`; plaza / lobby 1-based, 0 = none (`disp_status` page 0, `lb_chatMemberCheck`); error result + `str` = not found |
| 6709 | C->S req: `u8 max` (0x50), `u8 n`, n x (`u8 type`, value) | condition search | types: 1 id (obfuscated, 6), 2 handle (obfuscated), 3 `u8` weapon kind (mini +0), 6 `u8 lo, u8 hi` rank range (`lo*4+1 .. hi*4+4`, mini +1, a guess), 4 / 5 `u8` (never sent by the screens); n = 0 = everyone. Friend list: one type-1 search per friend |
| 6709 / 670A | S->C ans: `u8 total`, `u8 start`, `u8 count`, `u8 last`, count x (`str id` <= 8, `str handle` <= 16, `str mini` <= 0x40) | the results | stored from record `start`; with `last == 0` the client sends a **6709 notice** `u8 start+count` and waits for an answer with **that notice's sequence**; the test server sends it as 670A (same handler; which code the real server used is a guess). Pages of 10 records here |
| 6704 | C->S req: obfuscated id (6), obfuscated text (<= 0x7E) -> ans empty | short mail | error result + `str` = shown (the test server: unknown id). A new mail is preceded by a type-1 search; replies and mail to a friend are not, so mail to a hunter who is away is kept and delivered after his next login (test server; a guess about the real one) |
| 6705 | S->C notice: `str from id`, `str handle`, `str text` | a mail arrives | `CallBack_Event_RecvMail`: 8 slots `RecvMailInfo` (0x9A: unread, id, handle, text), the mail icon blinks while one is unread |
| 6706 | S->C req: `str title`, `str html` -> C->S ans empty | administrator message | shown as an HTML dialog; the test server sends one after the login with `--admin-message TEXT` |

All verified with tools/server/test_server.py (`TestServerFeatureTest`: a fake client speaking the real packets) and,
for 6709 / 6703 / 670D / 6704 / 6705, on screen with two game clients (section 3.5).

### 5.8 Event quests (file download), patches, personal data (10 Oct 2026)

**Event quests** (`tools/test_online_event.sh`: two clients download one, post it, play it as a co-op quest to the clear, get the reward screen with items and money, saved to their cards, and are back in the town, where they download it again).
Lbc_DownloadQuest runs at every lobby entry: cnLBS_Read_FileDownload(mission_area, cb), a job in burst slot 11.

| Code | Dir, cat | Payload | Notes |
|---|---|---|---|
| 6881 | C->S req empty -> ans | `u8 n` (<= 32), n x `u32 size` | the files are placed one after the other from mission_area. n = 0: no event quest. Error result: dialog, back to the server list |
| 6882 | C->S req | `u8 file`, `u32 offset`, `u32 0x200` | one per tick while a background slot is free (pipelined) |
| 6882 | S->C ans | `u8 file`, `u32 offset`, `u32 length`, `u16 n` + n bytes | copied to the file + offset; the job waits until `received >= size` per file (a missing chunk hangs until the 2 minute logout) |

File 0 is a mission file in the disc's format with a quest number >= 0xC8 at its info record +0x1D (the record's offset
is the file's first u32, little endian): the guild counter's level menu gets a last line "event quest" (only when
`cw+0x2C2F` = 1: a file was downloaded), the quest card shows it (no contract fee), the room property carries quest
200, and Quest_start uses mission_area as it is for numbers >= 0xC8. Files after file 0 have no reader. The PC: the job
(main 0x27D4A0) and both answer handlers (0x27D030, 0x27D2B0) written from the asm in rt_online.c (not compared with
check.py; before, a stand-in ended the job with no files); `rt_quest_load` accepts a number >= 0xC8 when mission_area
holds that quest. The test server: `--event-quest FILE`; tools/mk_event_quest.py renumbers a mission file dumped from
your own disc (`RT_MISSION_DUMP`): Capcom data, never commit or share one.

**Patches** (login, only while the login job runs; test: the second part of test_online_event.sh):

| Code | Dir, cat | Payload |
|---|---|---|
| 6121 | S->C notice (must be cat 16) | `str name` (4-char id + 10-char version), `u16`, `u32 byte count`, `u32 byte sum` |
| 6122 | S->C notice | `u16 block` (ignored), `u16 n`, n bytes (appended to the patch buffer, 0x20040 bytes, no bounds check) |
| 6123 | S->C notice | footer, ignored |
| 6124 | S->C req `u16` -> C->S ans the same `u16` | line check |
| 6125 | S->C req | finish: the client sums the bytes; a match goes to its patch step, else logout |
| 6125 | C->S ans empty | after the patch was applied and saved (a PS2 only) |

What a patch is (read from the code by a research pass, not run): an encrypted (DNAS personal keys) list
`"M-HUNTER"`, u32 count, entries {u32 overlay key, u32 RAM address, u32 length (bit 31: a source address follows), bytes}
that PatchExecCS copies over the loaded code / data of main and the overlays, kept on the card (save+0x208) and applied
again at each boot. The PC cannot apply PS2 code patches and has no DNAS keys: `ms_net_patch_set` refuses (-1),
lbc_login_patch's error path logs out ("disconnecting"). Bug found: `__cnet_Recv_PatchData` passed the chunk length
read in the same call's argument list (`GetRecvDataOption(ptr, GetRecvData16(&a, ...), a)`); C leaves the order open and
gcc read `a` first (garbage length): the PC branch reads it first. The test server sends a patch with `--patch FILE`
(only to test the client). Our servers never send patches. Regulation messages 6110 / 6111 / 6112 / 6801: no answer
or agree flow in the client; never send them.

**Personal data** (6181-6189): the PS2 sends it after the portal's registration page (`lbc_login_users_personal_data`:
the in-game browser on `afs://02/3`; when the page changed `BrPersonalData`, cnLBS_RegistPersonalData): 6181 req (empty)
-> ans result 0; 6182-6185 and 6187 notices (name, zip, address, telephone, mail address: one obfuscated string each),
6186 notice `u8 age`; 6188 req (empty) -> ans result 0; an error shows the server's message and the login goes on.
6189 (server request) has an empty handler: never send it. The PC skips the browser, so it never sends personal data,
and nothing asks for it (a design choice, as mh1-server's "store nothing", docs/server.md 9). The test server answers
6181 / 6188 with 0 and stores nothing (tested with a fake client in test_server.py).

Still read from the code only: server rules with choices (6604-6608, 660E), the shutdown codes (6002-6007).

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

1. **The return to the town after an online quest**: done (section 3.5), without `rt_lb_reload`.
2. **The portal**: replaced by `mh1online.ini` (section 3.5). The real portal's other pages (account creation,
   personal data, the top page links) are skipped, nothing is registered with a server.
3. **The in-game session** (`mcsls`, magic 0x82) and regulations: formats are in the code, not in the test server.
   Event quests, patches (refused) and personal data: section 5.8.
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
