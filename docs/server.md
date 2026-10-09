# mh1-server: our own server for online play (design, and where the code stands)

Written 9 Oct 2026 by agent A. Option (d) of docs/xbox_online.md: an open-source server that PC and Xbox players (and,
real PS2s with a patched game, section 2.6) can use while MH Oldschool has not agreed to non-PS2 clients. Code: `tools/server/`.

Sources: the game client's own code as decompiled in this repo (file and function named each time), docs/network.md
(the protocol as derived by agents E and B), and general knowledge marked **[known]**. Guesses are marked **[guess]**.
Nothing here was checked against a Capcom or MH Oldschool server, and nothing in this work connected to one.

## 0. Summary

* **What a server has to do** (from the client code): log players in, give them a list of lobby servers, run the lobby
  (plazas, lobbies, rooms, members, chat, mail, matching) and, for a hunt, **relay** each player's in-game records to
  the other players of the match. The hunt itself is decided entirely by the players' games; the server only passes
  bytes on and checks that connections are alive (docs/network.md 1a). That makes the server small.
* **Built so far** (`tools/server/`, Python standard library):
  * `relay.py`, the **session relay**: what a hosting player's game does in today's direct-connect co-op, as a service.
    Every player's game joins it with the existing `--join ADDRESS --port P`; nobody needs an open port. Tested with
    2 and 4 headless PC clients walking, and full hunts to the clear (2 and 4 players, a joiner leaving mid-hunt) on
    127.0.0.1 (section 11). The quest's host (slot 0) leaving no longer ends the session for the others, but the hunt
    then stalls because of a bug in the PC co-op glue (section 11, for agent B).
  * `accounts.py`, accounts, hunters, friends and mail in **SQLite**, scrypt password hashes, rate-limited logins.
  * `lobby_stub.py`, the lobby: a thin layer over agent B's `tools/mh1_testserver.py` that adds the account check at
    login and can hand matches to the relay. It grows with B's server instead of copying it.
  * `mcs.py`, a skeleton of the **PS2-faithful** game-session relay (the 0x1031 / 0x82 login and the mcsls records),
    tested only with synthetic bytes; nothing uses it yet.
* **Owner decisions needed**: section 13 (hosting and who runs it, money, the name, the DNAS legal note of the PS2 patch, PS2 cross-play).

## 1. The pieces, and what talks to what

    PS2 (original)                           PC / Xbox port (today)                 mh1-server
    --------------                           ----------------------                 ----------
    DNS  -> *.dnas.playstation.org (DNAS)    skipped (net_dnas.c says "ok")          (DNS + DNAS only for real PS2s, 3.1)
    HTTPS -> www01.kddi-mmbb.jp portal       skipped: server table + login built     (portal only for real PS2s)
             (login, server table)             in the client (rt_net.c / B's town)
    TCP  -> lobby server (0x81 packets)      the game's own cnLBS_* client            lobby service   (3.2)
    TCP  -> game server (records, relayed)   net_peer.c frames (direct connect)       session relay   (4)
                                              or later the game's mcsls (faithful)     mcs relay       (4.2)

## 2. Services, read from the client code

### 2.1 Account and login (the KDDI portal plus DNAS)

What the PS2 did (docs/network.md 2.4): DNAS authentication (`ms_network_bb_authentication`), then the KDDI
Multi-Matching BB portal in the in-game browser over HTTPS, which delivered the login and the server table into
`bsCsvWork` (first 16 bytes = the password, then per server id, `host:port`, user counts: `lb_tcp01.c`,
`create_server_table.c`). The lobby login then carries (`__cnet_SendSet_ConnectionPair`, `mmbbc_encode` in cnlbsh.c):

* **the key**: the first 8 digits of the MMBB id (`D_3A3B71`), sent as two 5-digit numbers, each = the packet's sequence
  number + 4 digits. Non-digits count as nothing (`read_col_numeric`), so the identity on the wire is **8 digits**;
* **the password**: 16 characters from the portal, obfuscated with "LOCKROCK", the server's `xfee` and the sequence
  number (docs/network.md 5.2). Obfuscated, **not encrypted**: anyone who sees the traffic can read it.

**What a PC / Xbox client sends to mh1-server**: the same packets. The port skips DNAS and the portal; the login (8
digits + password) and the server table come from a settings dialog / ini instead (`server = host:port`,
`login = 12345678`, `password = ...`). mh1-server makes the password up at registration (16 random characters) so a
player never reuses a real password here (section 7.1). Implemented: `accounts.py`, `lobby_stub.py` (the check at 6101;
tested with a fake client speaking the real login packet, `test_server.py`).

**What a real PS2 would need** (all **[guess]** except where docs/network.md says otherwise):

| Step | Needs | Feasible? |
|---|---|---|
| DNS | the player sets the console's primary DNS to our DNS server, which answers the game's names (`*.dnas.playstation.org`, `*.kddi-mmbb.jp`, the `*.capcom.co.jp` lobby names) with our addresses. This is what MH Oldschool does (docs/network.md 2.1) | easy (a small DNS server, or dnsmasq) |
| DNAS | the console runs Sony's DNAS client (`sceDNAS2*`, `DNAS280.IMG`) against our host over **SSL with Sony's certificates**, and must get an answer it accepts. MH Oldschool's troubleshooting lists DNAS SSL errors, so they answer it server side | **hard and legally risky**: needs Sony's certificate chain / keys or a replay of real DNAS answers. Both are Sony material we must not publish. Patching the game disc (DNAS-bypass patches) is the other route and also not ours to distribute |
| Portal | the in-game browser loads `https://www01.kddi-mmbb.jp/00000008/CRS-top.jsp` (`lobby.bin` strings) and follows `mmbb://` commands (`GAME_MENU_`, `REGIST_CHECK?`, ...). We would need an HTTPS server the PS2's old SSL stack accepts (old ciphers, a certificate it trusts: again Sony / KDDI roots) and pages that produce the CSV server table | hard: the certificate problem again; the page formats are in the browser code we paused (DECISIONS.md) |
| Lobby / relay | the same services as for PC / Xbox, with the **mcsls** session (mcs.py) instead of net_peer frames | doable once 4.2 is finished |

**Verdict: serving an unpatched PS2 is not realistic for us** (it needs Sony's certificates, which are not ours). The
owner decided on 9 Oct 2026 that **requiring a patched PS2 game is fine for now**: section 2.6 designs that path. The
PC / Xbox clients skip DNAS and the portal natively and need no patch. MH Oldschool stays the route for unpatched PS2s.

### 2.2 The server table

On the PS2 the portal's CSV gave a list of lobby servers (id, `host:port`, current / total users, password) and the
player picked one (`tcp_init`, `create_server_table.c`). mh1-server: the client's settings give one `host:port`; the
client builds the one-entry table itself (as `rt_net.c` does for the tests). A later `GET /servers.csv` over HTTPS
(section 7.3) can return several servers with user counts in the same layout, filled by the server.

### 2.3 The lobby server

The 258-command table (docs/network.md appendix, generated by `tools/lbs_cmdtab.py`) groups as:

| Group | Codes | State the server keeps | Status in B's test server (9 Oct) |
|---|---|---|---|
| Login, account list, mini data, top info | 6101-6105, 6131/6132, 6141, 614C, 6190/6191 | account (SQLite), hunters and their mini data | done (any login; mh1-server adds accounts) |
| Line check, logout, shutdown, lobby full | 6001-6007, 600E | connection | logout, echo |
| Plazas / lobbies / annexes | 6203-620A, 6301-6308, 630A, 6410/6411, 6210-6215 | who is where, counts, capacities | plazas, lobbies, members |
| Rooms (the quest groups at the guild counter) | 6401-6414, 6501-650A, 6601-6610 | room slots per lobby: name, password, rules, property (quest << 1, leader HR << 9), members | done in B's branch |
| Matching | 6504, 6506, 6508, 6910-6918 | match members, slots, the game server address (6916) | done in B's branch (the leader hosts) |
| Chat | 6701/6702 (lobby), 6708 (binary: the town's game data), 670B-670E (to one hunter) | none (relay with checks) | done |
| Mail, user search | 6703-6705, 6709/670A, 6706 (admin message) | mail queue, friends (SQLite tables exist) | not yet |
| Personal data | 6181-6189 | **none**: PS2-era name / address / phone fields; mh1-server answers "registered" and stores nothing (section 9) | not yet |
| Patches, regulations, file download, browser | 6110-6112, 6121-6125, 6801/6802, 6881/6882 | quest / event files (2.5) | not yet |
| Timing value (town shops, talk) | 6890 | a clock | ? |
| Battle result, bill estimate, records | 6138, 6142-6146 | none (MH1 seems not to report results: `CnetSys_w.batres` is only cleared) | not yet |

**Quest posting and joining** is the room flow: the leader makes a room at the guild counter (6407, rules, 6509 with the
quest number), others list rooms (6401-640D) and enter (6406), everyone says ready (6504), the leader starts (6508); the
server sends 6910 and answers 6911-6917, including 6916, **the game server address**. That is where the lobby hands the
hunt to the relay (section 4.3).

### 2.4 The in-hunt session

docs/network.md 1a (confirmed in the client): one TCP connection per player to the game server; the server sends 0x1031,
the client answers once with a 0x82 packet carrying `mmbbc_encode(login key, seq)` (`cmcs_04`), then only `mcsls`
records `[u8 len][u8 cmd<<4 | sender]` flow, with no destination; the server passes every record to the other members
and never back. The server's own packets in the stream start with 0x28 (`CnInetCheckMcsPacket2`): 0x1021 is a health
check (the client answers with a 0xF0 keep-alive record: `send_health_ans` in `mcsls_recv` / `mcsls_send`), 0x1032 is
read and ignored. **Nothing of the hunt is decided by the server.** The port's direct-connect co-op replaces this layer
with its own frames (net_peer.c) at `AQ_data_put`; mh1-server relays those today (section 4.1) and has a skeleton for
the faithful records (4.2).

### 2.5 Optional extras

* **Event quests / quest download**: the client has file download (6881/6882, handlers at 0x27D030 / 0x27D2B0 not named
  yet) and patch messages (6121-6125). MH Oldschool serves event quests (docs/network.md 2.1). The quest files
  themselves are Capcom data: **mh1-server must not ship any**. Option: the server distributes only community-made
  quests (none exist for MH1 yet) or the operator's own files from their own disc. Low priority.
* **News**: the top information (614C, an HTML string) is the server's news page; it is configurable text, done.
* **Rankings / records** (6144-6146): formats not traced; low priority.

### 2.6 Real PS2s through a patch (owner's decision, 9 Oct 2026)

Design only; nothing of this is built. The patch changes the player's own Japanese game (SLPM_654.95, NTSC-J) so that
it skips DNAS, skips the KDDI portal and goes straight to our lobby server.

**What the patch changes** (all places are in code this repo has decompiled; addresses from the files named):

| Step | Original | Patched | Where |
|---|---|---|---|
| DNAS | `ms_network_bb_authentication` (main, ms_nm.c / ms01.c 0x267FD0-0x2685B4) loads the `dnas_net` overlay and calls `InetDNAS2ConnectNetStart` every frame until it returns 1 | report success at once (exactly what the PC's `net_dnas.c` does: the call returns 1, progress 100) | main ELF |
| Portal | the in-game browser loads the KDDI pages over HTTPS and leaves the login password and the server table in `bsCsvWork`, the MMBB id in `D_3A3B71` (lobby.bin) | skip the browser step and fill `bsCsvWork` (password, one server `host:port`, the lobby id the client connects to) and `D_3A3B71` (the 8-digit login) as `rt_net.c` does for the PC tests | lobby overlay, at the step that starts the browser (`lbc_login_init` and the server-table code `lb_tcp01.c` / `create_server_table.c`) |
| Lobby address | from the portal's table | in the same injected table: our host name (resolved by the console's normal DNS via `CpInetDns*`, so no DNS change is needed) or an address | as above |
| Credentials | the KDDI account | the player's mh1-server login and password, written into the patched copy by the patch tool (each player's patch is personal), or read from a small file on the memory card (more work, but one patch for everyone) | patch tool |

Note: the game contains a leftover Capcom **test-server selection screen** (`test_server_sel_disp`, lobby 0x5C2CF0,
near-match in src/lobby/b/nm/). **[guess]** It may be a way to reach the lobby with a hand-entered server table; worth a
look before writing the portal skip.

**The in-hunt session is the catch**: a PS2 speaks the real mcsls records to the game server (2.4), while the PC / Xbox
co-op speaks net_peer frames. So PS2 players need the faithful relay (`mcs.py`, milestone S7), and **PS2 <-> PC/Xbox
cross-play** needs either the PC to link the game's own mcsls stack for online hunts (the cleaner way: the same
protocol everywhere) or a translator in the relay between mcsls records and net_peer frames (both carry the same AQ
packets, but mcsls adds its own sync / ping / fragment layer: more work and more risk). PS2-only hunts need only S7.

**How the patch is delivered** (never as game data):

1. **A patch tool** (Python, standard library, in this repo): reads the player's own ISO or ELF, checks it is
   SLPM_654.95 by hash, writes the changed bytes (our own small code: a few branches / stores, plus the injected table
   with the player's server and login), and writes a patched ISO next to the original. Works for PCSX2, OPL (ISO on USB /
   SMB / HDD) and modded consoles. The tool and its byte lists contain only our code and addresses, no Capcom bytes.
2. **A PCSX2 `.pnach`** (cheat-style patch: `patch=1,EE,ADDRESS,word,VALUE` lines, applied continuously, which also
   catches the lobby overlay after it is loaded) generated by the same tool for the player's login. Real PS2: OPL's
   cheat support differs between builds **[unverified]**, so the patched ISO (1) is the dependable route there.
3. **xdelta** of the ELF / ISO: possible, but an xdelta of a whole ISO can carry original bytes around the changes;
   the tool in (1) is cleaner and keeps credentials out of a shared file.
4. **From our own source**: main and the overlays rebuild byte-for-byte from this repo (Phase 1, `tools/rebuild.sh`), so
   the patch can be written as C in the decompiled files (`#ifdef MH1_SERVER_PATCH`) and built into a modified ELF /
   lobby overlay from the player's own disc. Drawback: the build needs the Metrowerks MWCC PS2 compiler, which we cannot
   distribute, so players cannot easily build it themselves. Best use: **we** build both variants and the patch tool
   ships only the differing words (our compiled code), with the C source as the readable reference. That keeps the patch
   reviewable and makes it easy to extend later (e.g. a server-address entry screen).

**Work estimate** (agent sessions): patch design and the byte list from the decompiled code (DNAS skip, portal skip,
table injection) 1-2; the patch tool with ISO handling and pnach output 1; tests in PCSX2 (only possible if PCSX2 is
installed on the dev machine, which is unknown; otherwise by the owner) 1-2; plus milestone S7, the mcsls relay, 3-5
(the PS2 cannot hunt without it). Total about **6-10 sessions** for PS2-only online; PS2 <-> PC cross-play adds the
PC mcsls mode (part of S7) or a translator (2-4 more).

**Legal notes (not legal advice)**:
* The patch contains no Capcom data and only works on the player's own copy; patching one's own copy for
  interoperability is the usual community practice (translation patches work the same way).
* **DNAS is also an anti-piracy check** (it rejects burned discs, error -402, docs/network.md 2.1). Skipping it lets
  copied discs go online too, and in some countries circumventing an access-control measure is restricted even when the
  service behind it is gone (e.g. the US DMCA section 1201 and its exemptions, EU copyright directive art. 6). The patch
  should do the minimum (skip the network authentication only, nothing about disc checks for booting), say clearly that
  it needs the player's own disc, and the owner should be comfortable with this before publishing it.
* Sony's and KDDI's certificates and services are not imitated at all on this path, which is its advantage over
  serving unpatched consoles.
* MH Oldschool serves unpatched PS2s without any patch; our patch must not point at their servers.

## 3. Architecture

### 3.1 Processes

**One process** to start with: an asyncio event loop with the relay and (on a thread, because B's server is
`socketserver`-based) the lobby; SQLite in the same process. Split later only if needed: the relay is stateless per hunt
and can run as several processes or on another host (the lobby only needs to know its address for 6916).

### 3.2 Language

**Python 3 (asyncio)**, standard library only (the repo's rule for tools). Reasons: the lobby protocol is already in
Python (B's test server), the load is tiny (3.3), and the community can read it. If one host ever has to relay
thousands of hunts, the relay (the only hot path, ~150 lines) is easy to port to Go; nothing else needs speed.

### 3.3 Expected load

Measured with the relay (`tools/test_coop.sh relay`, section 11): a hunt sends about **1-1.5 frames per second per
player** into the relay, 30-40 bytes each (4 players: 430 frames, 15 KB in 102 s including loading and the reward
screen; the busiest stretches, fights, are a few times that). Per hunt the relay forwards
each record to (players - 1) others, so outgoing traffic is about 3 times incoming for 4 players. Planning figures
(**[guess]** for the number of players): a hobby community of 50-200 players online at peak means at most ~50 hunts at
once, i.e. a few thousand frames per second in and out, well within one Python process on one small vCPU.

Lobby: chat and town data (6708, every few frames per player in the town) are the busiest; plazas hold up to the
capacity the server reports (B uses small numbers; the PS2 client shows join counts as `u16`). **[guess]** 32 per lobby
like the PS2 era is a sensible cap.

### 3.4 Persistence (SQLite, `accounts.py`)

| Table | What | Why |
|---|---|---|
| account | login (8 digits), scrypt hash, created, last login, banned + reason, operator note | login |
| hunter | hunter id (6 chars, the server gives it: 6132), account, handle (Shift-JIS bytes), the 0x40 mini data | 6131 list, lobby member lists, matching |
| friend | hunter -> hunter | the friend list (format of the game's side not traced yet) |
| mail | queued mail between hunters until delivered | 6704/6705 when the player is offline |
| audit | login ok / failed / banned / deletions, with time | abuse handling; pruned after 90 days |

Schema version in `PRAGMA user_version`; WAL mode; one file to back up. Saves stay on the players' machines (the game
keeps the hunter on the memory card / save folder); the server never stores a save.

## 4. The session relay

### 4.1 What is built: the net_peer relay (`tools/server/relay.py`)

Works with **today's PC client, unchanged**: in direct-connect co-op the hosting player's game listens, gives each joiner
a slot, collects their mini data, announces the quest (START) and forwards everyone's frames (net_peer.c). The relay
does exactly that as a service, so every player is a joiner (`--join SERVER --port P`). Slot 0 is the first player in;
the game decides "host" by slot number (`game_w.x21B = 0`, the first monsters' owner, `mcsls_calc_master_id`), not by who
listens, so the supply box and monster ownership behave as with a player hosting (tested: section 11).

Differences from a player hosting:

* **The host may leave**: when slot 0 quits, the others are told (BYE) and stay connected to each other. In
  direct-connect mode the host's leaving ended the session for everyone (docs/network.md 3.4 "Not done yet"). Whether
  the hunt can go on is then up to the game: today it stalls (section 11, `hostleave`).
* **Checks**: frame sizes (2..0x202), only types a player may send (game channels 1-10, HELLO, BYE, READY; never
  WELCOME / START), the sender byte is set by the relay (no impersonation), a token bucket (300 frames/s sustained,
  600 burst: about 10x a hunt's need), 30 s to say HELLO, 120 s silence in a hunt, 256 KB unread backlog. A breach
  drops that player only.
* **Late joiners** are refused once the quest started (as the host does). A session starts when its players have all
  said HELLO, or after `--start-wait` seconds with whoever came.
* **Sessions**: one TCP port per session (the client has no field for a session code). Permanent ones from the command
  line (`--session PORT:QUEST:PLAYERS`; the port takes the next group after a hunt), one-hunt ones from the lobby
  (`Relay.create_session`, from the port pool `--relay-ports`).
* Each hunt's traffic is logged (frames, bytes, frames/s) for capacity planning; no packet contents are logged.

### 4.2 The faithful relay for mcsls (`tools/server/mcs.py`, skeleton)

For real PS2s or a PC mode that links the game's own mcsls stack: sends 0x1031, reads the 0x82 login, finds the match by
the decoded login key, relays records (refusing records whose sender id is not the connection's), sends a 0x1021 health
check every 10 s, drops a connection silent for 40 s. **Untested with a client**; open points: the exact header of the
server's 0x1031 (magic byte and category; `select_ps2` reads it like a lobby packet), what 0x1032 means (the skeleton
sends it as an acknowledgement: **[guess]**), whether the real server consumed the 0xF0 keep-alives (the skeleton
forwards them).

### 4.3 Lobby to relay hand-off (needs a small client change, for agent B / the coordinator)

B's client (rt_online.c `matched`) treats the 6916 address as the room leader's game: slot 0 hosts, the others join.
With the relay every member must **join** the 6916 address. Proposal (not implemented on the client side): the lobby
answers 6914 MatchGameRule with the string `mh1-relay` (lobby_stub.py does this with `--lobby-relay`), and the client
then joins with its 6912 slot instead of hosting. The relay orders the slots by the lobby's member order (hunter names)
so they match 6912.

A cleaner long-term version needs one more client change: a `SESSION` frame (type 0x45, the match's battle code from
6915) sent before HELLO, so one relay port serves every hunt and the relay can tell sessions apart without a port each.

### 4.4 NAT behaviour

Every connection goes **out** from the player to the server (TCP), so home routers need no port forwarding, UPnP or hole
punching; carrier-grade NAT and mobile hotspots work too. Only the server needs open ports. Latency: one extra hop
(player -> server -> player instead of player -> host -> player for joiners); choose a server region near the players.

## 5. The client side (PC / Xbox), what has to change

* A **server address setting**: `server = host:port` in mh1pc.ini (and a field in the online dialog; Xbox: an on-screen
  keyboard or a short list). Today: `RT_NET_HOST` / `RT_NET_PORT` for the lobby and `--join` for co-op.
* **Public addresses**: the client refuses non-private addresses unless `--allow IP` / `RT_NET_ALLOW` (net_cpinet.c),
  and always refuses MH Oldschool. A configured mh1-server address should count as allowed (an explicit choice by the
  player), keeping the MH Oldschool refusal.
* **Login settings**: `login` (8 digits) and `password` for the 6101 packet (today hard-coded test values in rt_net.c).
* The 4.3 hand-off.
None of these were made here (src/pc/net and the ONLINE build are agent B's area right now).

## 6. Compatibility with MH Oldschool

The client keeps speaking the PS2 wire protocol; mh1-server adds nothing to it that a PS2 server would not understand
(the relay speaks net_peer frames only on its own ports, the lobby only the game's packets). So the same client can
target MH Oldschool later by changing the server address and the deny list (docs/network.md 3.2), **if they agree**;
the faithful mcsls mode (4.2, linking the game's mcsls in the client) is what their game servers would expect, since
they serve real PS2s. Nothing in mh1-server should be used to probe or imitate their service.

## 7. Security

### 7.1 Credentials

* The game's password travels obfuscated, not encrypted. Therefore: **server-generated random passwords** (16 chars),
  so a leak exposes only this game account; scrypt hashes (n = 2^14, r = 8, p = 1) with per-account salt; the same
  answer and the same work for "unknown login" and "wrong password"; 5 failures per login or 20 per address in 5
  minutes block further tries for the rest of the window (`accounts.py`, tested).
* Operators reset passwords with `mh1_server.py account passwd`.

### 7.2 Transport (TLS)

The game protocol has no encryption. Options: (a) accept that for a hobby server (chat and hunter data are not
secret; the password is a random game-only token); (b) for PC, wrap the lobby connection in TLS (a client change in
net_cpinet.c, OpenSSL / Schannel); Xbox can do TLS (NevolutionX does) but at a cost in size and CPU. Recommendation:
(a) now, (b) as an optional client setting later; the server can offer both on two ports behind a TLS terminator
(stunnel / Caddy layer 4) without code changes. The relay carries game state only.

### 7.3 Abuse and cheating

* **Cheating cannot be prevented by the server**: the hunt is client-decided (2.4). A modified client can send any HP,
  item or kill. Mitigations: hunts with friends (rooms with passwords), reports (an operator can ban an account), and
  sanity checks later in the relay for impossible values (only once the packet formats are fully known; out of scope).
* **Denial of service**: the relay's limits (4.1), a maximum of sessions and connections per address (to add),
  lobby chat rate limits (to add in the stub), and the host's firewall.
* **Spoofing**: the relay sets the sender slot; mcs.py refuses records with another player's id.
* **Names and chat**: free text from players; an operator word filter and mute / ban commands (section 9).

### 7.4 No copyrighted material

The server contains **no Capcom code or data**: no quest files, no text tables, no binaries. Everything is written from
our reading of the client. Event quests (2.5) only from the operator's own disc, never bundled. No Sony / KDDI
certificates or DNAS material (2.1).

## 8. Hosting

| | |
|---|---|
| Machine | any small Linux VPS: 1 vCPU, 512 MB-1 GB RAM, 10 GB disk. **[known]** typical price 3-6 USD (or EUR) a month (Hetzner, OVH, DigitalOcean, Vultr tiers); traffic of a few hundred hunts a day is far below their included bandwidth |
| Free options | the owner's home PC with port forwarding (exposes the home IP; not recommended for a public server); free cloud tiers (e.g. Oracle Cloud's always-free VMs **[known]**, terms change) |
| Software | Python 3.8+ only; or a Docker image `python:3-slim` + `tools/server` (no Capcom data in the image); systemd unit or `docker run --restart=unless-stopped` |
| Ports (TCP) | lobby 10200; relay sessions 10301-10399 (pool) or a fixed list; later 443 for HTTPS (server list, account page). No UDP |
| Address | a domain name is nicer than an IP (about 10 USD a year **[known]**) and lets the server move |
| Backups | the one SQLite file, daily copy (`sqlite3 .backup`) |
| Run | `python3 tools/server/mh1_server.py serve --bind 0.0.0.0 --allow-public --lobby-port 10200 --db /var/lib/mh1/mh1.sqlite3 --public-address <IP>` |

Players point the client at it with the server address setting (5). Without that setting today: `RT_NET_HOST`,
`RT_NET_PORT` for the lobby, `--join HOST --port P --allow HOST` for a relay session.

## 9. Moderation, privacy, terms

**Moderation**: operator commands (ban / unban with a reason, password reset, account delete) exist in
`mh1_server.py account`; to add: mute, kick, a word filter for chat and names, a report command (the in-game admin
message 6706 can show operator notices), an operator log. One or two trusted moderators are enough for a hobby server.

**Privacy (GDPR-style basics, not legal advice)**:
* Collect the minimum: login number, password hash, hunter names and mini data, friend lists, queued mail, IP addresses
  only in short-lived logs and the audit trail (pruned after 90 days; delivered mail deleted, `AccountDB.prune`).
* **No personal data fields**: the PS2 asked for name, address, phone, age, e-mail (6181-6189); mh1-server stores none.
* A player can get their data (`account export`) and have the account deleted (`account delete`).
* A privacy notice naming the operator and a contact address; no selling or sharing; logs not shared with third parties.
* EU players make GDPR apply to the operator; with this data set it means the notice, deletion on request and basic
  security. Children: the game is rated for teens; a minimum age line in the terms **[known practice]**.

**Terms of service outline**: free, non-commercial fan service, no warranty, may end at any time; not affiliated with
Capcom, Sony, KDDI or MH Oldschool; players need their own copy of the game; no cheating, harassment or hate speech;
moderators may mute / ban; data handling per the privacy notice; the server's source is public (licence: the repo's).

## 10. Legal and feasibility notes (not legal advice)

* A private server for a discontinued game sits in a grey area for Capcom's IP like any such server; keeping it free,
  non-commercial, without game data and with players using their own discs is the usual community stance.
* DNAS and the KDDI portal involve Sony's and KDDI's certificates and services: imitating them for real PS2s means
  handling their material (2.1). Not recommended; the patch path (2.6) avoids it but has its own note on DNAS.
* MH Oldschool: no imitation of their service, no connection to it (CLAUDE.md); cooperation is the route to unpatched PS2 players.

## 11. Tests and measurements

* `python3 tools/server/test_server.py` (about 2 s, no game): relay (a 3-player hunt with slot 0 leaving, frames a
  player may not send, rate limit, frame length, start with whoever came, the lobby's slot order, refused binds and
  addresses), accounts (hashing, wrong password, ban, rate limit, hunters, friends, mail, export, delete), the lobby
  stub's login check (a fake client sending the game's real 6101 packet: right password, wrong password, unknown
  login), the mcs record splitter and relay rule.
* `tools/test_coop.sh relay` (the ONLINE=1 PC build, headless, 127.0.0.1): 2 and 4 players walking through the relay
  (positions exact on every machine), then `RELAY=1` hunts: `hunt2`, `hunt4`, `leave`. `RELAY=1
  tools/test_coop_hunt.py hostleave` is a known failure (below). The default `tools/test_coop.sh` run is unchanged
  (the position check was only moved into a function both modes use).
* The lobby stub with the real client: `mh1_server.py account add 00000000 --password LOCALTEST0000000`, `serve
  --lobby-port 0 --db ...`, then `mhview_online --nettest full` (rt_net.c's test login): the login is accepted and the
  whole nettest runs to "done"; with the same login under another password the stub refuses it and the client stops at
  "login (waiting for the server)". This confirms the key decoding (`mmbbc_encode` with the 6101 sequence number) and
  the password deobfuscation against the game's own C.

### Run log (9 Oct 2026, ONLINE=1 build of agent-A after merging main)

| Test | Result |
|---|---|
| test_server.py (9 unit tests) | OK |
| relay, 2 players walking | OK, every machine sees every player 0 units off; 17 frames / 576 bytes in 8 s |
| relay, 4 players walking (one great sword) | OK, 0 units off everywhere; 35 frames / 1201 bytes in 8 s |
| relay hunt2 (quest 137) | OK: both see HP 500, 420, 100, 0, clear at ticks 1114 / 1115, both saved 1725 z; 243 frames, 9.0 KB in 102 s |
| relay hunt4 | OK: clear at 1114, 1114, 1114, 1099, all saved 1637 z; 430 frames, 14.9 KB in 102 s |
| relay leave (slot 1 owns, quits; slot 2 finishes) | OK: clear at 1313 / 1327, saved 1666 z; 276 frames, 10.0 KB |
| relay hostleave | **FAILED**: see below |
| lobby stub + real client login | OK (right password), refused (wrong password) |

**hostleave, what happens**: the relay tells slots 1 and 2 that slot 0 left (both log "player 0 left"), but nobody
takes the monster over, so slot 2's hits never land and the quest cannot be finished. Cause (confirmed by a test build,
not committed): `rt_np_init_slots` (src/pc/rt/rt_np.c) sets each first monster's owner field `EMW+0x88E` to **this
machine's** slot instead of the host's (0). `Em_Master_Change` (em_master_nm.c) only reassigns a monster when
`pl_state[+0x88E] == 0xFF`, so on the followers the owner's leaving is never noticed. With `+0x88E = 0` the lowest
remaining slot (1) does take the monster on its machine; in that test slot 1 was at the camp, and the monster was not
handed on to slot 2 (fighting it) within the test's time, so the second half needs a look at Em_Master_Change's
"owner not on the area" branch too. The same bug affects direct-connect co-op whenever the host drops (there it is
hidden because the host's leaving ends the session). Fix belongs to rt_np.c (agent B's area): `+0x88E = 0` for the
first monsters on every machine.

## 12. Milestones

| # | What | Effort (agent sessions) | Depends on |
|---|---|---|---|
| S1 | Relay for today's co-op, accounts store, lobby stub, design (**this**) | done | |
| S2 | Client: server address + login settings, a configured server counts as allowed; lobby -> relay hand-off (4.3) | 1 | B's online town |
| S3 | Lobby: the stub takes over B's protocol as it lands; accounts used for 6131/6132 (hunters stored), mini data stored, lobby caps, chat limits, operator commands (mute, kick) | 1-2 | S2 |
| S4 | Deployable: Dockerfile, systemd unit, config file, log rotation, backups, a status page; first private test on a VPS with the owner and friends | 1 | owner's hosting choice |
| S5 | Mail, friends, user search (6703-6709) traced and served | 2-3 | protocol work |
| S6 | Session code frame (one relay port, 4.3), relay limits per address, metrics | 1 | client change |
| S7 | Faithful mcsls mode (4.2) end to end with a PC client linking mcsls | 3-5 | decompiled mcsls linked on PC |
| S8 | Optional: news / top info editor, rankings, operator web page, TLS option on PC | 1-3 | |
| S9 | Real PS2s through a patch (2.6): byte list from the decompiled code, patch tool (ISO + pnach), PCSX2 tests | 3-5 (plus S7) | S7; owner OK on the DNAS legal note |
| S10 | Not planned: unpatched PS2s (DNAS + HTTPS portal server side) | large, needs Sony material | section 2.1 |

## 13. What the owner must decide

1. **Hosting and operator**: who runs the public server (the owner, a community volunteer) and where (a VPS at 3-6 USD
   a month, recommended; a home PC; a free tier). The operator is responsible for the data (section 9).
2. **Money**: who pays the VPS (and a domain, about 10 USD a year), and whether donations are accepted (keep it
   non-commercial).
3. **Name**: "mh1-server" is a working name. It should not use Capcom's marks in a way that looks official; e.g.
   "MH1 Community Server" with a clear "unofficial" line.
4. **Real PS2 clients**: decided 9 Oct 2026: a patched game is acceptable (section 2.6). Still open: whether the owner
   accepts the DNAS legal note in 2.6 before a patch is published, and whether PS2 <-> PC cross-play is wanted (it
   decides between the PC linking mcsls and a translator in the relay).
5. **Accounts**: open sign-up (a web page or an in-game "new account" that returns a login) or invite-only (the
   operator makes accounts) for the first months. Recommendation: invite-only while testing.
6. **TLS** for PC (7.2): optional later, or required from the start.

## 14. Open questions

* How the real server refused a login (the stub closes the connection; the client shows a connection error).
* The 0x1031 header the server sends, the meaning of 0x1032, and whether keep-alives were consumed (4.2).
* Formats of mail, friends and user search (6703-6709), file download (6881/6882), rankings (6144-6146).
* How many players a plaza / lobby held on the original service.
* Whether a client-side session code (4.3) or the 6914 rule string is the better hand-off; agent B's call.
