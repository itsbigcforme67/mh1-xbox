# MH1 (Japanese) release readiness, re-checked 4 Nov 2026

Research agent, 4 Nov 2026. A re-check of docs/RELEASE.md (agent A, 13 Oct 2026; not edited) against today's code and
docs. Nothing was built or run: no game, emulator, test or Xbox. RELEASE.md stays the record of 13 Oct; this file
says what changed since and what is left.

Marks: **[docs]** taken from a doc (named), not re-run. **[code-read]** read in the source or a script today.
**[inferred]** my reading, not checked. Lanes as in RELEASE.md / RELEASE_G.md: **decomp** (game C), **platform**
(src/pc host code, PC build, packaging), **text** (English), **Xbox** (nxdk, nv2a, loader, hardware), **server**,
**owner** (a decision or a hardware check).

Sources read: docs/RELEASE.md, RELEASE_G.md, STATUS.md (top and 13 Oct - 4 Nov), DECISIONS.md, agents/COORDINATOR.md,
REVIEW.md (13 Oct - 1 Nov entries), xbox.md (from "Installed by hand" on), xbox_coldstart.md, PLAYING.md, TESTERS.md,
english.md section 8, text_coverage.md, network.md sections 1, 3.4, 3.10, 7, audit_cross_game.md; `git log` 389c1d7c..HEAD
(18 Oct audit to today, 153 code commits outside MH2) and the diffs of the candidates named below; ./mh1,
tools/make_release.sh, tools/text_map.py, src/pc/gfx/gfx_opts.c, src/main/chat/chat_nm.c, src/pc/rt/rt_player.c,
src/pc/xbox/mc_xbox.c, tools/full_check.sh, tools/check_kr_returns.py, tools/argregs.py.

## 0. In short

- MH1 is in much better shape than on 13 Oct. Of RELEASE.md's six blockers, two are closed (the bed "Save?" box; an
  Xbox install path) and one mostly closed (player install steps). Most "noticeable" items are done: loading screens,
  monster rumble, flmatInit33, the game's own lighting, the boot-logo and village music, the Xbox settings menu.
- Nearly every G fix since 13 Oct was made in shared code, so MH1 already has it. The cross-game audit (18 Oct) found
  three G faults MH1 also had and fixed them; since then I found **no G-only branch that hides a generic MH1 bug**.
  What MH1 still lacks from G's work is **delivery and checking**, not code: the English table, a hardware session,
  a fresh coverage run.
- What still blocks a player release of MH1: (1) no human has played it through; (2) **MH1 has not been on the real
  Xbox since about 15 Oct**, and about fifteen shared Xbox changes since were checked on hardware only with G, or only
  in xemu; (3) **the English the owner chose for MH1 (its own US disc text) does not reach a player through `./mh1`**,
  and the table tool needs a G disc too; (4) PLAYING.md still describes the 13 Oct state. (5) Online on the real
  Xbox is untested, a blocker only if online is in the release.

## 1. Criteria, then and now

| # | Criterion | 13 Oct (RELEASE.md) | 4 Nov | Basis |
|---|---|---|---|---|
| R1 | Boots from the player's own disc data | PC done; Xbox: no player path | **Done on both.** `./mh1 linux\|windows\|xbox DISC` checks the disc, builds and copies the data; Xbox: `tools/xbox_install.sh` (FTP with resume) or the loader's HTTP install, and a start-up size check that names a missing file. Testers' kit `tools/make_release.sh` (needs a G disc first, below). Not checked: a hand-installed default.xbe started from a real dashboard (xbox.md "Installed by hand": "Not checked: a dashboard launch from E:") | [code-read: mh1 script], [docs: PLAYING.md, xbox.md] |
| R2 | Every quest playable from the board to the reward | 38/38 + urgent pass with test aids; no human play-through | Same, plus the failure paths (three faints, time-out on quest 139) in the full sweep since 18 Oct. Full check 46 ok on 29 Oct includes the MH1 tests. **Still no human play-through**; nothing in STATUS / COORDINATOR records an MH1 play session since 13 Oct | [docs: RELEASE.md 7, COORDINATOR 29 Oct] |
| R3 | Saves reliable | PC pass; Xbox "Save?" invisible | **Fixed and owner-confirmed on hardware (13-14 Oct).** Since: card saves flushed to disk per write (21 Oct, both games); xemu relaunch test `--save` for MH1: bed save 70736 bytes, quit, CONTINUE loads the same bytes (28 Oct); corrupt save gets the game's own "damaged" prompt. Memory units not supported (E:\UDATA only) | [docs: STATUS 13-14 Oct, xbox.md "Second start"] |
| R4 | Music, effects, ambience right | PC pass; Xbox sound works; two owner reports open | Both reports fixed and confirmed (silent logos, no clear music at the bed save). **New, shared risk:** G's house music turned to static on hardware (17 Oct, unexplained, in the ADX music path MH1 uses too; xbox_coldstart.md 6). Audio setup changed on 29 Oct (codec only written) and was checked on hardware with G only; whether it is audible needs a person. A "bed sleep jingle has no sample" note (COORDINATOR, 15 Oct; game not named) is not closed anywhere | [docs] |
| R5 | No stand-in on a player path | 46 reached, 8 matter | 7 of the 8 fixed on 13 Oct (vib_set_pl, vib_stop_all, Disp_NowLoading2 via the game's loading screens, flmatInit33, two aliases; init_*_work stay on purpose). Left: the kanji dictionary of name entry (`apiask_*` weak stand-ins, build/pc/rt_gen.c). **The last MH1 coverage run is from 13 Oct**; ~400 commits since | [code-read: rt_gen.c], [docs] |
| R6 | Menus and options complete | PC done; Xbox no menu | **Done.** Xbox Back + L3 settings (language, 4:3 / 16:9, HUD, button layout, volumes, rumble, Skip intros), settings.ini on E:; language "auto" (English when a table is present) since 29 Oct | [docs: xbox.md, STATUS 29 Oct], [code-read: gfx_opts.c] |
| R7 | Frame rate on the real Xbox | ~30 fps field / village; fights, movies, loads not measured | MH1 hardware numbers are still the 12-13 Oct ones. Area loads 0.25-0.5 s on hardware (14 Oct). The nv2a changes of 27-28 Oct (static-clay near-plane cut, GPU frame overlap, vblank only when needed, one kick per draw) were measured for MH1 in xemu only (field ~12 -> 26) and on hardware with G only (30 fps, 29 Oct). Big MH1 fights on hardware: never measured | [docs: STATUS, REVIEW 28 Oct, COORDINATOR] |
| R8 | Online town against our own server | PC + Wine work; Xbox never run | PC, Wine and **xemu** work, including a PS2 + PC + Xbox hunt on the game's own protocol (16 Oct). G online ran on the real Xbox on 30 Oct through the same network code, so MH1's online XBE is likely to work there [inferred]; **never run on hardware** (PLAYING.md says so) | [docs: network.md 3.10, xbox.md 30 Oct] |
| R9 | Controls | no rebinding; rumble partial | Rumble complete (monster shakes, stop at 0), owner-confirmed on the Xbox 13-14 Oct. Keyboard and mouse on the Xbox through the RetroKM dongle (HID part built 16 Oct). **Still no key rebinding on PC** (menu.c has only the circle / cross layout) | [code-read: src/pc/menu.c], [docs] |
| R10 | Settings remembered | PC done; Xbox none | **Done**: settings.ini (E:\Games\MH1, or the folder of a hand-installed default.xbe) | [docs: REVIEW 28 Oct] |
| R11 | Crash-free over long sessions | PC soak flat; xemu -130 KB per loop; no long hardware run | Freeze watchdog and crash catcher (21 Oct, both games; tuned 28 Oct so the exit screen and long waits do not count). The 130 KB a loop loss in xemu is still unexplained. Still no multi-hour hardware session of MH1 | [docs: xbox.md] |
| R12 | Install / build steps a player can follow | Windows zip README only | `./mh1`, docs/PLAYING.md (MH1 + G, all three platforms), docs/TESTERS.md, the testers' kit. **PLAYING.md is out of date for MH1**: it still says the Xbox build has no settings menu, area changes have no loading screen, lighting is a fixed set, and no English table is included (with no way to make one); "Known issues" is there twice; "Online on the Xbox (not G yet)" is stale since 30 Oct | [code-read: PLAYING.md lines 121, 484-497, 374] |
| R13 | Language | Japanese only; layer exists | MH1's table from the owner's US disc: **83% of MH1's strings have official English** (7033 of 8404; text_coverage.md); MH1 in-game tour and fixes (english.md 8, 21 Oct); shared G work (chat rows at 28 bytes and size 19 as in the US game, the quest-end banner, `%s` lookup, colour codes) applies to MH1. **Done later on 4 Nov:** `./mh1 ... MH1.iso --english US.iso` and `make_release.sh --mh1 ... --english` make and install it without a G disc (X1, X2) | [code-read: mh1, make_release.sh, text_map.py], [docs] |

## 2. Remaining gaps, ranked by what a player meets

Numbers are new (prefix 4N); the RELEASE.md number is given where an item carries over.

### Blockers

| # | What the player sees / risks | Where | Lane | Basis |
|---|---|---|---|---|
| 4N-B1 (was B4) | Nobody has finished MH1 on the port. Every quest passes only with test aids (warps, slay, god mode). The referee (RELEASE.md 6) shows fights match the PS2 for hundreds of ticks, which lowers the risk but is not a play-through | tools/test_all_quests.py; RELEASE.md 6 | owner (play with F8 / Back + Black reports), decomp + platform (fixes) | [docs] |
| 4N-B2 (new) | **MH1 on the real Xbox is three weeks behind.** Shared Xbox changes since ~15 Oct that MH1 has only in xemu, or that were checked on hardware with G only: own AC97 driver idle-on-exit and time-outs (21 Oct), codec writes only (29 Oct), watchdog and crash catcher, card flush per write, log written through + 4 MB log reserve, push-buffer rewind, static-clay near cut, frame overlap, vblank wait only when needed, one kick per draw, static draw-function table (rt_codetab), settings next to a hand-installed XBE, Skip intros, language auto. Any of these can behave differently on MH1's scenes (different stages, the house, online town) | src/pc/xbox/*, src/pc/gfx/gfx_nv2a.c; COORDINATOR 29 Oct | Xbox + owner (one bench session, section 4) | [docs], [code-read: git log] |
| 4N-B3 (was B3) | **No English for an ordinary MH1 player.** `./mh1 linux\|windows\|xbox` with the MH1 disc never copies an English table (only the G branch copies build/text/en_mhg.txt). Only `tools/make_release.sh G.iso --mh1 MH1.iso --english US.iso` writes MH1's text/en.txt, and it refuses to run without a G disc. tools/text_map.py itself opens G's executable unconditionally (`Img("g", args.g, ...)`), so an MH1 + US player without G cannot make the table at all [code-read; not run]. Since 29 Oct the game picks English by itself when the table is there, so only the delivery is missing | mh1 (xbox branch ~line 280, linux / windows branches), tools/make_release.sh, tools/text_map.py:3186 | text + platform | [code-read] |
| 4N-B4 (was B5, mostly done) | The player page misleads (R12): it tells MH1 players about missing features that now exist and gives no way to get English | docs/PLAYING.md | platform (docs) | [code-read] |
| 4N-B5 (was B6) | Online on the real Xbox never run for MH1 (xemu only). A blocker only if online is in the MH1 release (owner) | dist/xbox/MH1-online; network.md 3.10 | Xbox + owner | [docs] |

### Noticeable

| # | What the player sees | Where | Status | Lane | Basis |
|---|---|---|---|---|---|
| 4N-N1 | Music may turn to static on hardware (seen in G's house on 17 Oct; same ADX decoder and stream ring in MH1). Cheap first step in xbox_coldstart.md 6: log the ADX predictor values at each stream start on the Xbox and compare with the PC | src/pc/audio/audio_mix.c, rt_snd.c, fmt/snd.c | open, unexplained | Xbox | [docs] |
| 4N-N2 | Rare black screen on a cold power-on (1 in ~25) or a second start without a power cycle | src/pc/xbox/xbox_ac97.c | "likely fixed, not proven": 15/15 cold starts with G on 29 Oct; G second start 3/3. MH1: not tried on hardware since | Xbox + owner | [docs: xbox.md 29 Oct, xbox_coldstart.md] |
| 4N-N3 (was N3) | Great sword, lance, hammer and bowgun models do not change shape per motion (weapon_dat_make node scaling) | src/pc/rt/rt_player.c ~778 ("is not done") | not started | platform | [code-read] |
| 4N-N4 (was N7) | Near-match C in play. Still "not decided" in docs/agents/targets.md: eft20_m, eft06_m, set13_trans, Pl_light_set, light_init, em_neck_move_sub, neck_ang_set, Em_Master_Change, shell08_m / _trans. G now has its own eft20 and shell08 from G's asm; MH1's stay unreviewed | src/**/*_nm.c | open | decomp | [docs] |
| 4N-N5 (new) | MH1's coverage run (which stand-ins and unmatched functions a player reaches) is from 13 Oct. Since then the referee rounds, the lighting, the Elder's tutorial port and the text layer all touched MH1's paths. A COV=1 rerun of the 13 Oct set (RELEASE.md 3) would refresh R5 and N4 | tools/cov_report.py | not run | platform | [docs] |
| 4N-N6 | English screens: two titles stay Japanese though the table has them (the equipment details window, the pause menu title row; MH1 frame structs, english.md 8 item 6); labels that run into their values (quest info rows, "Improve Wpn", element resistances); lines with no US twin stay Japanese: online 22%, memory card / yes-no 61%, code literals 38%, quests 82% (village quests 131-177 were rebalanced in the US) | rt_text.c, rt_data.c; text_coverage.md | open; fill policy is an owner decision (section 4) | text + owner | [docs] |
| 4N-N7 (was N13) | Big fights, movies and area loads of MH1 on hardware never timed; the skinned-model near-clip smear has no fix (candidates in xbox.md) | gfx_nv2a.c | measure | Xbox + owner | [docs] |
| 4N-N8 (was N10) | Xbox memory falls ~130 KB per quest loop in xemu (~100 loops of headroom) | allocator suspected | open | Xbox | [docs: xbox.md 1080] |
| 4N-N9 (was N12) | The Windows build has only run under Wine | tools/package_win.sh | owner or a tester | owner | [docs: PLAYING.md] |
| 4N-N10 (was N8) | Co-op gaps of the direct-connect mode (capture, giving items, in-quest chat, two big monsters in one area, start at the quest not the board, host leaving splits it). The server-relayed mode runs the game's own session code, so these may work there, but only a quest delivery and a Kut-Ku hunt were tested | network.md 3.4, 3.10, 3.11 | open | platform + server | [docs], [inferred] |
| 4N-N11 (was N9) | A public server is not safe yet (passwords in the clear, no TLS, no limits, no moderation, no sign-up). Fine for a private server | docs/server.md 15 | owner decisions | server + owner | [docs, not re-read in full] |

### Minor

| # | What | Status | Lane | Basis |
|---|---|---|---|---|
| 4N-M1 (M1) | Name entry and chat cannot convert kana to kanji (`apiask_*` stand-ins) | open | platform | [code-read: build/pc/rt_gen.c] |
| 4N-M2 (M5) | Xbox dashboard shows no save icon (SaveImage.xbx / TitleImage.xbx not written) | open, needs a reference .xbx | Xbox | [code-read: mc_xbox.c:15] |
| 4N-M3 (M6) | No key rebinding on PC | open | platform | [code-read: menu.c] |
| 4N-M4 (M3, M4) | Approximate reverb; hunter hair / cloth bones keep their bind offset (not re-checked since 7 Oct) | accepted / unchecked | platform | [docs] |
| 4N-M5 (M7, M8) | Frog-bait fishing only bites if the Plesioth is idle (maybe faithful); untested shop variants (sell from the pouch at the house box, organise items, trader beyond one case, shock trap, a capture quest) | unverified | owner / platform | [docs] |
| 4N-M6 (M9) | Xbox picture darker on the capture card than PC / xemu (probably the video chain) | owner check with a retail game | owner | [docs] |
| 4N-M7 (new) | English chat: rows break at a space where the US game cuts anywhere; rows of 23-28 bytes spill past the box as in the US game | kept different on purpose, owner's call (REVIEW 1 Nov) | owner | [docs] |
| 4N-M8 | Dev-only: loader download freeze after Evolution X starts it (M10); the weak em*_local_area_move_init / em10/em18_act_set stand-ins in rt_em.c look dead (RELEASE.md 3) | open / tidy-up | Xbox / platform | [docs] |

### Closed since 13 Oct (for the record)

B1 Xbox install path (./mh1 xbox, xbox_install.sh, loader HTTP install, start-up data check); B2 bed "Save?" box (13 Oct,
owner-confirmed); N1 loading screens (13 Oct, owner-confirmed); N2 game lighting (27 Oct); N4 monster rumble; N5
flmatInit33; N6 boot-logo ambience and bed-save music; N11 Xbox settings menu and 16:9; M2 rarity colours / page arrows;
M12 weapon model in the village; the "quit the game" at the bed (16 Oct). [docs: STATUS, RELEASE.md, COORDINATOR]

## 3. G fixes and MH1

### 3a. Already in MH1 (shared code; nothing to do but test on MH1's hardware)

From the commit messages and diffs since the 18 Oct audit [code-read: git log 389c1d7c..HEAD]:
- Game / host: PS2 float rounding default, joints built in move(), the Elder's tutorial window, PS2 sine / cosine /
  atan2f, wall pass and CameraMove order, monster necks and motion blends on the host skeleton, frame counter and play
  time every tick, no area-exit check on an area's first tick (viewer.c 07f294bc: not gated by version), hidden monsters
  not drawn, area_mdlw sub models, stale set clays, game lights per actor, stage fog after an area change, god-mode red
  bar reset, village lockstep with MH1's walkers.
- Return-type hazard (Xbox ABI): 906 K&R declarations corrected in both games; `check_kr_returns.py --mwcc mh1` is a
  full_check step (`kr_returns`), so MH1 stays clean. The trigger was G's Elder crash; MH1's lb_x01.c / lb_v06.c got the
  same Lb_get_quest_type fix (155de896).
- English layer: `%s` arguments, ~C colour codes, full-width digits drawn half-width in English, talk rows cut on a
  character boundary, the quest-end banner text and centring (f_game.c, not gated), chat rows at 28 bytes and size 19
  in English (chat_nm.c: CHAT_SIZE is MH1's own branch; MH1 has no window-size option, so English uses the US default).
- Xbox: everything listed in 4N-B2 (all in src/pc/xbox and gfx_nv2a.c, shared), `--ps2-output`, the static
  draw-function table (93dd2b2a, "both games").

### 3b. Worth doing for MH1 (from G's work)

| # | What G got | What MH1 needs | Lane | Basis |
|---|---|---|---|---|
| X1 (**done 4 Nov**) | `./mh1 xbox` (G) copies the English table into the game folder; the game picks English by itself (57e1ab1f) | the same for MH1 in all three `./mh1` branches (build/text/en_mh1.txt -> text/en.txt, which gfx_text_table_find already looks for), and a way to make en_mh1.txt from the JP + US discs without G | text + platform | [code-read: mh1, gfx_opts.c:312-358, text_map.py:3186] |
| X2 | Testers' kit for G with MH1 as an add-on (make_release.sh) | an MH1-only kit (`--mh1` without a G disc), or say plainly that the kit needs G | platform | [code-read: make_release.sh] |
| X3 | Our own lines for G strings no official source has (text/mhg_en_own.tsv, tools/text_own.py) | MH1's 1371 strings without US English stay Japanese; the same layer could fill MH1 gaps (memory card, code literals, the save-in-progress line), **if** the owner allows it (DECISIONS: "MH1 JP keeps its own US disc text, unchanged") | text + owner | [docs] |
| X4 | Lost-argument fixes in G's generated lobby code (0a5cf909, 4e0c805f, ee8ff891: m2c dropped an a0 still live, a junk argument on x86, a crash on the real Xbox 30 Oct) | MH1's lobby is hand / matched C and its calls of the same functions look right (e.g. every SoftKeyboard_pos_set call passes (float, int)) [code-read]. But tools/argregs.py `--check` (written 5 Oct for exactly this) is not a full_check step, and a matched MWCC function can omit an argument the callee reads from a left-over register without changing the PS2 bytes [inferred]. A one-time `argregs.py --check` sweep over MH1's C (main, game, lobby), then a full_check step like kr_returns | decomp + platform | [code-read: argregs.py, full_check.sh] |
| X5 | G's coverage report and zero-table check (cov_report_g.py, check_g_tables) | MH1's own coverage rerun (4N-N5); MH1 has no zero-table problem (its tables come from the symbol table) | platform | [docs: audit_cross_game.md] |
| X6 | G Xbox hardware checks of 29 Oct-1 Nov (frame rate count with the capture card, cold-start loop, idle village, second starts) with bench scripts (scratchpad / tools/bench) | the same scripts with MH1's dashboard entry ("Monster Hunter") | Xbox | [docs: COORDINATOR] |

### 3c. G-only by nature (checked, nothing for MH1)

Lb_get_armor_num over 100 / 200 / 300 box pieces (f6860607: G's box size; MH1's matched code loops 64); the forge's
"equip now?" starting on yes (5c386d53: G stores 0, MH1 1); G's six sharpness levels and 12-byte bands (d32c830e); G's
0xE00 save slot (a1344000); G's Options rows, pause menu rows, chat window-size option (G layouts; MH1 keeps its own
branch); pull_enemy_work / check_gwx (raw MH1 offsets inside G code); G table sizes (92 stages, hunter titles,
LobbyInfo, BsProxyUrlstr); the "1P".."4P" room-rule strings left untranslated (tools/text_map.py G_WIRE, G only; MH1's
English online rooms were not checked for the same kind of string match [inferred: worth one look when 4N-B5 is
tested]); all G monster / effect ports (em01, em02, em16, em17, eft04/13/15/20, shell08, set14...). The 391 G-only
branches added since 18 Oct were judged from their files and comments, not line by line.

## 4. For the owner

### Decisions

1. **Is online part of the MH1 release?** If yes: PC only, or Xbox too (4N-B5); relay mode kept or dropped (DECISIONS
   "Open: mh1-relay mode"); private server only (4N-N11).
2. **MH1 English gaps (X3):** keep the 17% without US English in Japanese, or fill them with our own lines as G does?
3. **The English table needs a G disc today (4N-B3):** fix the tool so JP + US is enough (recommended, since MH1's
   English is meant to come from its own US disc), or require G.
4. Short forms for English labels that do not fit (pending since 28 Oct: PSP short forms vs US long forms); chat breaks
   at spaces vs anywhere (4N-M7).
5. Ship without: kana-to-kanji conversion (4N-M1), save icons (4N-M2), key rebinding (4N-M3)? All three look fine to
   leave for later [inferred].

### Hardware tests (MH1 build of today, one bench session)

1. Cold power-on 10x and Back + Start then a second start 3x: title with sound each time (4N-N2; tools/bench coldloop
   with MH1's entry).
2. Listen: music in the village, the house and back out, a field, a fight (4N-N1), and that the 29 Oct audio change is
   audible at all.
3. Bed save, quit, CONTINUE (R3 on the new flush code).
4. Frame rate with the capture card: village, a field, a Rathian or Kut-Ku fight, an area change (R7, 4N-N7).
5. 16:9 and the settings menu; English with a table installed by hand at E:\Games\MH1\text\en.txt (R13).
6. A bug report with Back + Black.
7. A hand-installed default.xbe started from the dashboard, no loader (R1, the testers' path).
8. A long session (an hour or more) with the log's free-memory lines (R11, 4N-N8).
9. If online is in: the MH1 online XBE in the town with a PC on our server (4N-B5).
10. Afterwards: power the Xbox off and check it is off.

Also: one real Windows PC run (4N-N9), and the human play-through, one star level at a time (4N-B1).

## 5. Suggested order [inferred]

1. Cheap fixes with no hardware: X1 + X2 (English and kit delivery), PLAYING.md for MH1 (4N-B4), the X4 argument sweep,
   the coverage rerun (4N-N5).
2. One MH1 hardware session (section 4), then fix what it shows (4N-N1, 4N-N2).
3. Owner play-through with reports; agents fix.
4. 4N-N3 weapon node scaling, 4N-N4 near-match reviews, 4N-N6 English screens.
5. Online items if the owner puts online in the release.
