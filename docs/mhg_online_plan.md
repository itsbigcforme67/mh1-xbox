# Monster Hunter G online: what is left, and the plan

Written 4 Nov 2026 (research agent, reading only: no build, no game run, no network traffic). Sources: docs/network.md
(3.12 is the G online log, rounds 1-10), docs/server.md, docs/xbox_online.md, docs/xbox.md (G online on the real Xbox,
30 Oct - 1 Nov), docs/agents/agent-C.md (G online rounds 1-12), src/lobby/cnet/cnet_g.c, src/pc/rt/rt_online.c,
tools/mh1_testserver.py, tools/mhg_lobby.py, tools/g_info.py, tools/server/, and G's own code (asm/mhg/lobby, the rebuilt
build/mhg/lobby.bin). The command-table comparison and the sender / reader lists below come from two small scripts in
the scratch folder (not in the repo) that read G's and MH1's command tables (G lobby 0x63B910.., MH1 0x616D20..) and
G's asm.

**Owner rule: our own / local server only.** Nothing here connects to MH Oldschool or any public server. Every test
below runs against `tools/mh1_testserver.py` or `tools/server/mh1_server.py` on 127.0.0.1 (the hardware bench uses the
laptop's private LAN address, as it already does). The network backend already refuses public addresses (network.md
"Safety"); that stays as it is for G.

Tags: **[checked]** = seen in a run already recorded in the docs named; **[code]** = read from G's code, not run;
**[guess]** = a reading of the code that has not been confirmed.

## 0. Correction first: G online is not "nothing"

docs/RELEASE_G.md row G10 still says "nothing exists for G". That was true on 15 Oct; it is out of date (this file does
not edit RELEASE_G.md; the coordinator should). What exists today [checked, network.md 3.12 and docs/xbox.md]:

| Piece | State |
|---|---|
| Server tells MH1 and G apart | yes: G sends 61A0 after the connection pair, MH1 6102; places, rooms, searches and stored hunters are kept per game (accounts.py schema 3) |
| G login on our server | PS2 G (MH1LAUNCH RAM patch), PC G, Xbox G (xemu and the real console) |
| G's info file ("file 0") | made by tools/g_info.py; G logs out without it |
| G plaza classes, plaza menu, town list, town (stage 0x4C), guild hall (0x4D) | on all three platforms |
| Chat, rooms, rules, 6415, matching, the mcs relay hunt (Kut-Ku), carve, clear, relogin save, back in the hall | PC+PC, PS2+PC (either leads), PS2+PC+Xbox(xemu); real Xbox + PC hunt (bench script, 30 Oct) |
| G's in-hunt packets in G's layout (sys 2 0x1E bytes, sys 3 + s16, em kinds 50/45, quest_w 0x280 block) | PC/Xbox G online build |
| A player dropping mid-hunt | PS2 or Xbox dropping, checked |
| G pause menu online pages (16 rows) | PC, 15 of 16 rows open; Log Out not opened |
| Build | `ONLINE=1 tools/build_pc_mhg.sh` -> build/mhg/pc/mhview_g_online; `tools/build_xbox.py --version mhg --online --gfx nv2a`; `tools/make_release.sh --online` copies the G online XBE |

So the job now is the rest of G's online features, the parts the tests never reach, and making it something a player
can set up. Everything below is about that.

## 1. G vs MH1, message by message

Same as MH1 [checked or code-identical, network.md 3.12]: framing, obfuscation ("LOCKROCK" table, the server's xfee,
the sequence number), 6101 connection pair, 600E echo, 6131 / 6132 hunters, 6104, 6190 mini data (kept opaque by the
server), 614C top info, 6141, 6891 current place, the room rule family 6601-6610 / 660E / 660F, 6407, 6509 (the
property's content differs, below), 6504 / 6508 / 6506 / 6412, the match 6910-6918, 6002 logout, 6701 / 6702 / 6708 /
670B-670E chat, 6704 / 6705 mail, 6706 admin message, 6121-6125 patches (never sent by our servers), 6181-6189 personal
data, the 0x82 login to the mcs relay and the mcsls stream.

G's table has 293 rows (MH1 258). New codes in G: 6010 6148 6149 61A0 61A1 630B 630C 630D 6310-6317 6415 6416 6417
6707 670F 6710 6711 6883 68A0 68A1. Dropped from G: 6102 6110 6111 6112 6142 6210-6215 6703.

### 1.1 Login (S = server, C = client)

| Code | Dir | G payload | vs MH1 | Server today |
|---|---|---|---|---|
| 6101 | S req, C ans | as MH1 | G's handler sends 61A0 right after | done |
| 6102 | - | not in G | MH1 asks the telephone number | held back 0.5 s, not sent once 61A0 came |
| 61A0 | C req `u8` (7) -> S ans `u32, u32` | the byte is first-data byte 2; the answer goes to CnetSys work and login event 12, which G ignores | G only | sends 0, 0 [meaning unknown] |
| 6103 | S req -> C ans | client kind **7** (MH1 4), version **"0411210839"** | different values | done |
| 61A1 | C req empty -> S ans `u32 a, u32 b` | a: in-hunt send window (a / 60 * 30), b: drop time-out (b / 60) | G only | 0, 0 = client defaults [the real values unknown] |
| 6148 | C req `u8 kind` -> S ans `u8 r` | r = 1-3: server asks for a 336-byte record, 4: a 44-byte one, uploaded with **6149** (`u8 kind, u8 r, obfuscated record`) | G only | answers 0 (asks nothing) [guess: rankings / records] |
| 6881 | C req -> S ans `u8 n`, n x `u32` | **n >= 1 required** (file 0 is G's info file) | MH1: n may be 0 | done |
| 6883 | C req `u8 i` -> S ans `u8 i, u32 size, u32 version` | G re-downloads a file only when the 8 bytes change | G only | CRC as version |
| 6882 | as MH1 | file 0 -> mission_area+0x2414; files 1-3 = 512-byte event quest headers | MH1: file 0 = the event quest itself | done for file 0 |
| 68A0 / 68A1 | C req -> S ans `u8 n` (<= 3) + 4 bytes; per k: 68A1 `u8 k` + 3 bytes -> `str` | news texts after the login | G only | n = 0 (68A1 never sent in a test) |
| 670F | C req `u8` -> S ans empty | sent by Lbc_SendMiniData, lbc_login_top_information, Lbc_GuestReadRoom and one more (G 0x5E6C10) | G only | answered by the server's "unhandled request: empty" fallback [guess: a status flag] |
| 6010 | S notice | a server message (G event 45) | G only | never sent |

### 1.2 Plazas ("areas"), classes ("lands"), towns

| Code | Dir | G payload | vs MH1 | Server today |
|---|---|---|---|---|
| 6203-620A | plaza count / name / join / status / explain / entry | explain asked with mask 15; the explanation's first character '1'..'5' is the plaza's class | MH1 has no classes | G_TABLE: one plaza per class |
| 6205, 6303, 6403 join users | ans `u16 id` + **four u16** | MH1 three | town list shows a/b, in town c, on quests d | done (mhg_lobby.join_counts) [order of a-d partly guess] |
| 630D | C `u16 lobby` -> S `u16, u32` | town record flag 0x80 | G only | 0 [meaning unknown] |
| 630B | C `u16 lobby` -> S `u16, u8` | town record flag 0x10 | G only | 0 [meaning unknown; never seen in a G log] |
| 6305 | C `u16 lobby, u16 2, u16 0` | MH1: the lobby only | | done |
| 6310 | C `u16` -> S ans (empty) | sets the entry floor (floor 1) | G only | **none** (fallback) |
| 6312 / 6313 / 6314 | C `u16` -> S ans `u8` each | read one byte each | G only | **none**: the fallback's empty answer makes the client read a byte past the payload |
| 6311 | C empty -> S ans empty | an exit (sender split-named __cnet_SendReq_PieceExit) | G only | **none** (fallback; empty is what the reader expects) |
| 6316 / 6317 | C obfuscated string -> S ans empty | sent by a burst at G 0x5AE650 with 6311 | G only | **none** (fallback) |
| 6315 | ans empty | no sender found in G's lobby | G only | - |
| 630C | C `u32` -> S ans empty | sent from a plaza-menu step (G 0x57FBB0, callback the plaza-exit one) | G only | **none** (fallback) |

[guess] for 6310-6317 and 630C: they are behind the plaza menu's "area move" and "land move" rows (G 0x569D40, called
from plaza_moveMain, uses 6310 / 6312-6314 via the split's "Lbs_GetLobbyMemberList", 630C and 6416). g_info.py notes
that the town-type name picked in the plaza's town list is sent with 6314 (lbc_in_plaza_03, G 0x57F8F0), while the
sender at G 0x5AE490 writes one u16: the two readings disagree and need a run. None of these codes appears in any G
test log in build/ (checked the server logs of town_menu, g_online_pc, bench/online).

### 1.3 Guild hall, rooms, quests

| Code | G vs MH1 | Server today |
|---|---|---|
| 6401 | G keeps the room count where MH1 keeps the lobby count | done |
| 6403 / 6404 / 640B / 640D / 6405 / 650A per room | 640B: **the first u16 is the maximum** (MH1: members, max) | done (mhg_lobby.on_640B_1) |
| 6406 join | `u16 room, obfuscated password` (G 0x5BBF60) | done |
| 6415 | new: `u16 room, u8 quest low, u8 quest high, u8 attr, u8 attr` before 660C [guess: the quest's board attributes] | **fallback** answers empty (that is what G reads); logged as "unhandled" in every room test |
| 6509 | property = `hunter value (User_data+0x1A) << 16 | quest` (MH1: quest << 1 | HR << 9) | done (room_quest) |
| 6915 battle code | **G seeds its online quests from it** (start area, level bonus, sizes): every member must get the same code | done: one per match |
| G's events | notice event numbers differ (commer / leaver 26/27/36/37/41/42; 6005 = 6; 6007 = 7) | client side only |

Quests: G's counter has three ladies (the middle one by HR range, the right one only for top hunters), G's 8 quest
levels, and the online-only quests on the disc (3004-3007, Fatalis at Castle Schrade, RELEASE_G.md GB notes). Those
quests need no server data: they are on the player's disc. Hunted so far online: quest 1 and 4 (Kut-Ku) only.

### 1.4 Searches, friends, news (G only, none tested)

All from G's asm (sender / reader call order), meanings from the calling screens [guess unless said]. The calling
screens are G functions the PC already compiles from m2c drafts (src/lobby/gonl/gen); none has been run against a
server answer.

| Code | Request | Answer | Screen (caller) |
|---|---|---|---|
| 670A | obfuscated string, 6 bytes (a Capcom ID) (G 0x5E6580, row 270) | read by the same reader as 6709 (G 0x5E63A0, MH1's condition-search result) | Friend list (G 0x56C550, named lm_mail_box_mv by position) and player search (G 0x56ED50): "find this ID". MH1 sends `u8 page` with 670A: G sends the ID instead |
| 6707 | obfuscated string, the ID (>= 6 chars, not one's own) | `u16, u16, u16, u8` [guess: plaza, town, room, state] | player search, friend list (getFriendNow per friend): "where is this hunter". Replaces MH1's 6703 (dropped) |
| 6711 | `u8` (100) + obfuscated string (<= 16 bytes) | `u8 x4, option3 x2, str`; the client then sends 6711 again as a **notice** with one `u8` | player search by hunter name [guess]; one hit per answer, the notice asks for the next [guess] |
| 6416 | `u8, u16, u8, u8, u8, u8` (G 0x5E66A0) | `u8 x8, u16 x4, u8`; then a 6416 **notice** `u8` from the client | quest search (G 0x570320) and the plaza menu's area step (G 0x569D40) [guess: a condition search over rooms] |
| 6417 | `u16, u16` (from a G main table, +8 / +0xA of a 16-byte row, each + 1) | `u16, u16, u8 x4, u16, u16, u8, str, str, str, u8, u8` (G 0x5E2920) [guess: one room record: place, quest, owner, room name, message] | quest search, step 2 |
| 6710 | no sender found in G | reader = __cnet_Recv_SearchUser | probably unused |
| 6704 / 6705 | mail, as MH1 | | short mail page (G's lm_friend_list_* by position) |
| 68A0 / 68A1 | news (1.1) | | after the login |

6144 / 6145 / 6146 (MH1's personal record reads) are still in G's table but G's answer handlers are 16-byte stubs
[code]: G does not use them.

### 1.5 In the hunt (mcs relay)

The relay carries the game's own records, opaque; mcs.py needed no change for G [checked]. G's differences are all
in the client (network.md 3.12 round 7 table): sys 2 is 0x1E bytes with the quest at 14 and the hunter id at 22; sys 3
has an extra s16; em kinds 50 / 45 behave like 7 / 15; keep-alive every 300 frames; MCSLS_SWIN_MINUS from data;
quest_w block at 0x280; cmcs_03 offsets. Still unknown: sys 3's s16 (always 0 so far), select_w 0xE4 / 0xE6 / 0xE8
(always 0, also from the real PS2), the new item helper G 0x4C64D0 in net_receive_pl (not ported: item hand-over between
hunters), which monsters are em kinds 50 / 45.

### 1.6 The browser and nethttp.bin

nethttp.bin is G's OpenSSL / HTTP library (1,425 functions, split out of the lobby; docs/mhg.md). It only serves the
in-game browser: the KDDI portal (`.../00000040/CRS-top.jsp`, G's own portal id) and the `mmbb://` page commands. The
port does not need it: the PC / Xbox skip the portal (rt_online.c: MainBrowser returns "closed" at once) and the PS2
patch fills the server table. Owner decision of 6 Oct (DECISIONS.md): the browser is paused. Effect for G players:
plaza-menu rows that open a web page ("Hunters' Site", maybe "Event Info" [guess: not checked which rows open the
browser]) open and close at once with nothing shown.

## 2. What the server must add

All in tools/mhg_lobby.py (G handlers, reloaded live) and tools/g_info.py; MH1 keeps working unchanged. Rule for every
new handler: a well-formed answer the client can read, `res=1` + a short message where "nothing found" exists (as MH1's
6703 / 6704 do), and a log line.

1. **Explicit handlers for every G request, no fallback.** 670F, 6415, 6311, 6316, 6317, 630C, 6310: empty answers, as
   now but on purpose. 6312 / 6313 / 6314: one byte each (value to be found, 2.3). Then make the fallback for a G client
   log at error level, so a test can fail on it (3, step 1).
2. **Player search and friends**: 6707 (where is ID: the three place numbers from REG.where, state byte), 670A (an ID
   -> one row in 6709's format, which mh1_testserver already writes for MH1), 6711 (name search: walk the logged-in G
   clients, one answer per hit, the next on each 6711 notice, `res=1` when none are left).
3. **Quest search**: 6416 and 6417 over the town's rooms (quest number from the room property, members, max, owner,
   name, message). Needs the field order checked first (step 6 of the plan).
4. **Area / land move**: whatever 6310-6314 / 630C turn out to mean; at least answers that keep the client going.
5. **News**: G_NEWS (68A0 / 68A1), texts in the game's HTML subset, a config file entry for the operator.
6. **Server messages**: 6010 notice (operator broadcast) alongside MH1's 6706.
7. **Event quests for G**: files 1-3 of the download (512-byte headers into quest_title+0x30, the whole file on
   demand by Lbc_DownloadQuest, G 0x581610). Only from the operator's own disc (as MH1's tools/mk_event_quest.py: a
   renumbered mission file, never committed). The G header layout is not read yet.
8. **Optional**: 6148 / 6149 records (a server-side record board) if the owner wants rankings; 61A1 tuning values if a
   hunt over a slow line drops (today: client defaults); 61A0's answer if it turns out to matter.
9. **g_info.py**: the unknown fields (0x98A / 0x98E / 0x992 client values, 0x788 / 0x888 texts, per-class rank limits:
   today every class takes every hunter); town type names in English for the English build (the file is ours).
10. **Config**: G plaza / town names and counts from mh1-server.conf instead of the G_TABLE constant.

## 3. What the PC / Xbox host must add

1. **Check the m2c screens behind the untested rows**: friend list, player search, quest search, area / land move,
   mail, news, Log Out (G's lm_logout was never opened). They are drafts "compared with nothing" (src/lobby/gonl/gen);
   the 30 Oct Xbox crashes (dropped call arguments, a memset over the stack, Visual_main = strlen) show what to look for.
   Use tools/mhg_gonl.py FIXUPS for every fix, so a regeneration keeps it.
2. **cnet_g.c answer handlers** for the new rows where the PC still runs MH1's or a GDONE stub: 6416 / 6417 / 6707 /
   670A / 6711 readers are G functions in gen/ (check that the PC links G's and not MH1's of the same split name; the
   split's names are MH1's by position and often wrong, network.md 3.12).
3. **Event quests on the G PC**: rt_online.c's download job is MH1's (main 0x27D4A0, file 0 = the quest); G's job uses
   6883 versions and files 1-3; quest numbers and Quest_start's online path for a downloaded G quest.
4. **Item hand-over in the hunt**: port G 0x4C64D0 (net_receive_pl) and test it.
5. **The browser rows**: either hide them, or show a fixed local page ("not available on this server"). An owner
   decision (the browser is paused); the simplest is a message box.
6. **Player packaging**: `./mh1 linux|windows|xbox G_DISC --online` (today only make_release.sh builds the G online
   XBE; `./mh1` refuses --online for G), the `online_server` / `online_login` / `online_password` keys in mhgpc.ini and
   settings.ini as for MH1, and a "Monster Hunter G online" section in docs/PLAYING.md (it still says "No online play
   for G"). Not in this job: the coordinator updates those docs.
7. **Xbox**: memory (12.9 MB free in the guild hall with one hunter; ~3 MB more with three), 7.5 -> 59 fps already
   fixed in the town; the Xbox's relogin save after a hunt (never checked: the test folder had no G save); name tags
   over hunters on the Xbox (not seen in captures); the on-screen chat text.
8. **Safety**: nothing new: the G build uses the same net_cpinet backend and its public-address refusal. Add a G line to
   the test that checks refused destinations (test_online.sh's three refused addresses run on MH1 only).

## 4. Build plan (ordered; each step ends with a test on 127.0.0.1)

Every step: `ONLINE=1 tools/build_pc_mhg.sh` (laptop rules: nice, -j4, one heavy job at a time), the server
`python3 tools/mh1_testserver.py` (or mh1_server.py with `--lobby-mcs` for hunts) bound to 127.0.0.1, hidden clients
(MH1_HIDDEN=1), pictures in build/show/ (never committed). After each merge: `HUNT=1 MH1_HIDDEN=1
tools/test_online_g.sh` three times and the full check (COORDINATOR.md rule).

1. **No silent fallbacks** (server only, small). Explicit handlers for 670F, 6415 and the empty-answer codes of 1.2;
   the fallback for G logs "G: unhandled request XXXX" at error level. Test: `ROOM=1 tools/test_online_g.sh`, then grep
   the server log: no "unhandled". Add that grep to test_online_g.sh so it fails on a new unhandled request.
2. **Refused destinations for G** (test only). A G client started with a public address must refuse to connect (the
   same three cases as test_online.sh). Test: the new case in test_online_g.sh; no packet leaves the machine (the
   client never calls connect: check its log).
3. **Read the real PS2 G's requests for the untested rows** (PCSX2 + MH1LAUNCH, our server on 127.0.0.1, the owner's
   disc; `tools/test_ps2_g_online.sh` style, one emulator at a time). Drive each plaza-menu row (area move, land move,
   friend list, player search, quest search, the browser rows, news) and log every request with its payload. This
   settles 6310-6317 / 630C / 6416 / 6417 / 6711 order and meaning from G itself, not from the m2c drafts. Output: a
   table in docs/network.md 3.12 (the coordinator merges docs). The PS2 run is the reference; no server answer is
   invented before it.
4. **Player search and friends** (server 2.2, host 3.1 / 3.2). Test: two G PCs (TWO=1): BOB searches ANNA by ID
   (670A, 6707: the place shown), by name (6711), adds her as a friend, the friend list shows her place; a search for a
   missing ID shows the server's message. Then the same with the PS2 G searching the PC G.
5. **Mail** (MH1's 6704 / 6705 on G's pages). Test: TWO=1, ANNA mails BOB, BOB reads it; BOB offline: kept and
   delivered at his next login (mh1_testserver already keeps mail for absent hunters).
6. **Quest search** (server 2.3, host 3.1). Test: ROOM=1 with ANNA's room posted; BOB uses quest search from the
   plaza menu, finds ANNA's room (6416 / 6417), joins from the result if G offers that.
7. **Area and land move** (server 2.4). Test: one client moves plaza (area) and class (land) from the plaza menu and
   back; the town list counts on a second client follow (6303 notices).
8. **News and server messages** (server 2.5 / 2.6). Test: G_NEWS with 3 texts: all three shown after the login
   (68A1 x3); an operator broadcast (6010) shows on a client in the town and one in the guild hall.
9. **Event quests for G** (server 2.7, host 3.3). Needs reading G's header layout first (512 bytes at quest_title+0x30,
   Lbc_DownloadQuest G 0x581610). Test: like tools/test_online_event.sh: two G PCs download a quest made from the
   owner's own disc, post it at the counter, hunt it, clear, rewards, saved; the file is not downloaded again when its
   6883 version did not change.
10. **Item hand-over and the other hunt leftovers** (host 3.4): port G 0x4C64D0; find which monsters are em kinds 50 /
    45 and hunt one of them online (PC + PC, then PS2 + PC, `tools/test_ps2_g_hunt.sh`). Also one hunt of an online-only
    disc quest (3004-3007) on two PCs.
11. **The browser rows** (host 3.5, after the owner's choice). Test: each browser row opens the message / page and
    goes back to the plaza menu.
12. **Packaging** (host 3.6). `./mh1 linux G_DISC --online` and `./mh1 xbox G_DISC --online` from a fresh clone; the
    player steps in PLAYING.md followed literally on the laptop: own server on 127.0.0.1, two PC clients, then the
    Xbox on the LAN.
13. **Real hardware pass** (bench, owner present for the Xbox; Xbox off after, feedback-xbox-off): steps 4-9 with the
    real Xbox as one of the players, the Xbox starting from a saved G game so the relogin save is checked.

Not planned: rankings (6148 / 6149) and server tuning (61A1) unless the owner asks; unpatched PS2s (they need the
portal and DNAS, docs/server.md S10); any link to MH Oldschool.

## 5. What is guessed

- The meanings of 61A0's answer, 630B, 630D, 670F, 6415's attribute bytes, 6310-6317 and 630C (1.2): only formats are
  known, from G's senders and readers.
- Which screen sends what in 1.4: from the calling functions' places in plaza_moveMain and the split's MH1-by-position
  names (which were wrong several times before); 6711's "name search" and the page-by-page notice, 6416 / 6417 as quest
  search, 6707's three u16 as plaza / town / room.
- 6710 being unused (no sender found by the scan, which looks for a constant row number before SetSendCommand; a
  sender that loads the row another way would be missed; 6315 the same).
- 6148 / 6149 being rankings.
- The join-user order a/b/c/d, the info file's unread fields, per-class rank limits (byte 1 = HR?).
- Which plaza-menu rows open the browser.
- The ordering of steps 4-8 assumes step 3 confirms the formats; if the PS2 shows something else, the PS2 wins.

## 6. Where the facts are

- G's command table and the MH1 diff: G lobby 0x63B910 / 0x63BA40 / 0x63BB70 / 0x63BCA0 / 0x63BDD0 (293 rows), MH1
  lobby 0x616D20.. (258 rows); senders found by `SetSendCommand(&send_work, ROW)` in asm/mhg/lobby/text.
- Senders: 6310 G 0x5ADCF0, 6311 0x5AEF70, 6312-6314 0x5AE230 / 0x5AE360 / 0x5AE490, 630C 0x5AEE80, 6316 / 6317 (split
  names __cnet_SendReq_RuleListHeadWord / RuleNumOfChoice), 6416 0x5E66A0 (+ notice 0x5E67C0), 6417 0x5E6D60, 6707
  0x5E6AB0, 670A 0x5E6580, 670F 0x5E6C10, 6711 0x5E6EE0 (+ notice 0x5E70A0), 6148 0x5C0930, 6149 0x5C0A90.
- Readers: 6416 G 0x5E6830, 6417 0x5E6DF0 -> 0x5E2920, 6707 0x5E6A70 -> 0x5E6B20, 6711 0x5E6F60, 6312-6314 0x5AE1F0 /
  0x5AE320 / 0x5AE450, 6010 0x5C0580, 6144-6146 0x5E27B0.. (stubs).
- Screens: plaza_moveMain's children G 0x569D40 (area step), 0x56C550 (friend list), 0x56ED50 (player search),
  0x570320 (quest search); browser steps 0x57FBB0 / 0x57FEA0 / 0x5802F0 / 0x5804E0 / 0x5807E0 / 0x580910.
- Codes seen from G clients in our server logs (build/B/eng/town_menu, build/show/g_online_pc, build/bench/online):
  6001 6002 6006 600E 6101 6103 6132 6141 6148 614C 6190 61A0 61A1 6203-6207 620A 6301-6305 630A 630D 6401 6403-6407
  640A 640B 640D 6412 6415 6504 6508 6509 650A 6601-660F 6701 6708 670D 670F 6881-6883 6891 68A0 6911-6917.
