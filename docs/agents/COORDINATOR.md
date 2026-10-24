# Coordinator handbook

How the session that runs the worker agents works. Written so a fresh session can
take over without the history of earlier ones. Read CLAUDE.md, docs/DECISIONS.md and
the end of docs/STATUS.md first.

## Goal and priorities (owner)

The owner's standing decisions, newest last. A fresh session reads these first.
- Player experience first: platform layers (renderer, audio, input, files, saves) are
  written from scratch for PC and Xbox; game logic and look stay faithful to the original.
  Byte-matching is a correctness check, never a blocker (docs/DECISIONS.md, 5 Oct 2026).
- Token efficiency matters as much as speed (5 Oct). Pause agents when the owner asks.
- Order of work (9-12 Oct): single player first (human play-testing with F8 reports on PC, plain
  chat reports on Xbox; agents fix, prove correct, add features — no robot/soak play-testing
  agents), then the online town and our own server, then the real Xbox port (now on hardware).
- Keep the codebase tidy (10 Oct): when a fix makes an older workaround/duplicate obsolete,
  remove the old one in the same change.
- Every change that drops old code for a new approach goes into docs/REVIEW.md (11 Oct), even
  when clearly right; the owner reviews later. Mention new entries in updates.
- When unsure a fix solves the real problem, keep it out and note it as a candidate (13 Oct: the
  CPU near clipping cost frame rate on hardware and was removed; docs/xbox.md lists it).
- 13 Oct, later: **Monster Hunter G is the main output** (docs/DECISIONS.md): G in English
  with our modernizations, later a Poogie-style ESP32 minigame. MH1 JP still gets finished;
  fixes are cross-applied between games (MH1 US maybe).
- Next phase (13 Oct): polish MH1 into a release-ready, polished playable game (PC + real Xbox);
  then carry what we learned to MH1 International (US) and to Monster Hunter G (PS2).
  docs/RELEASE.md holds the release criteria and gap list (agent A writing it, 13 Oct).
- Report in plain language; the owner is a hobbyist ("so-so" at RE). Push verified work to the
  public GitHub repo after each step (full check first, see "Merging").
Rules that never change:
- Never commit Capcom or Microsoft files (disc/, build/, xbox_files*, asm/ are git-ignored).
- Never connect a client to the MH Oldschool public server; use the local test server.
- Nothing may open a window on the owner's desktop: PC viewer with --shot or scripted --input
  (hidden), Xbox builds in xemu via tools/run_xemu.sh (own hidden Xvfb display).
- Never `pkill -f` / `killall` a pattern; kill only PIDs you started.
- Never type the Xbox FTP login (the owner runs tools/xbox_upload.sh); never dump the EEPROM.
- On the Xbox's Evolution X menu never select Lock/Unlock Harddisk or Flash BIOS; take a capture
  after every pad press before pressing A.

## Token efficiency rules

1. Keep the coordinator's own context small. Every coordinator action re-reads the whole
   conversation, so a long coordinator conversation is the single biggest cost (one session
   reached ~580k tokens of context; each tool call re-read all of it). Batch work into
   few tool calls (one script per merge, not one command per step). When the conversation
   gets long, compact it or hand over to a fresh session using this file.
2. One assignment per agent instance. Resuming an agent with SendMessage keeps its whole
   old context; for a new assignment, start a fresh agent (Agent tool) pointed at
   docs/agents/BRIEF.md and its notes file instead. Resume only to finish an interrupted task.
3. Decomp workers run on Sonnet (`model: "sonnet"`); the renderer/format work and the
   coordinator use Opus. Matching is self-verifying, so Sonnet's mistakes get caught.
4. Agents report rarely (when a whole assignment is done), in a few lines.
5. No scheduled usage checks (owner, 5 Oct 2026: a waste). Agents commit small, building
   steps often, so hitting the limit loses little; after a reset, merge and start fresh agents.

## Setup

- Worktrees: ../mh1-wt/A..F on branches agent-A..F. Symlinks to disc/, .venv,
  tools/compilers, tools/binutils, tools/m2c, tools/permuter (excluded via .git/info/exclude).
  New worktree: `git worktree add ../mh1-wt/X -b agent-X`, add the symlinks,
  `mkdir build`, `tools/rebuild.sh` (about 5 minutes).
- Shared brief for workers: docs/agents/BRIEF.md. Per-agent notes: docs/agents/agent-X.md.
- Machine: 8 cores, 7 GB RAM: at most 6-7 agents, one permuter each with -j2.

## Merging an agent branch

If a merge conflicts in anything beyond config/c_files.txt and generated config, or the
rebuild fails: `git merge --abort` (main must stay equal to origin) and start a fresh
agent on that branch whose first task is `git merge main` + fix until all five OK.

Note: tools/build.py skips src/pc/ (native platform code, built by tools/build_pc.sh).

    git merge --no-edit agent-X
    # config/c_files.txt conflicts: python3 tools/merge_union.py config/c_files.txt
    #   then remove stale lines that came back from the agent's older base
    #   (check: awk '{print $1, $NF}' config/c_files.txt | sort | uniq -d)
    # config/c_renames.txt, config/*.yaml: git checkout --ours (regenerated by the build)
    # include/*.h struct conflicts: never take one side wholesale (that lost fields once).
    #   git show :2:include/X.h > /tmp/o.h; git show :3:include/X.h > /tmp/t.h
    #   python3 tools/merge_struct.py /tmp/o.h /tmp/t.h STRUCT -o include/X.h
    #   It merges by offset and reports renames, TYPE DIFFERS and lines only in THEIRS
    #   (add those by hand). Pick the type a matched load proves (lb vs lbu, lh vs lhu).
    tools/rebuild.sh            # all five modules must print OK
    git push
    # A failed rebuild can leave generated stubs (src/**/X.c with only INCLUDE_ASM lines) that
    # git does not track; they break the next rebuild after the merge is undone. Check
    # `git status --untracked-files=all src` before blaming a branch (12 Oct 2026).
    # Never `git reset --hard` a merge commit away: keep it on a branch first.

If a module mismatches after a merge: find the object from build/<module>.map at the
first differing offset (base 0x100000 main, 0x533980 overlays), run
`python3 tools/check.py <file>` and `python3 tools/align.py <file> <func>`; it has so far
always been a struct field type or a lost field.

## Current assignments (update when they change)

### Running, 23 Oct 2026 evening
- agent C: G online round 6 merged 25 Oct: two PC G hunters hunt the Kut-Ku together (HUNT=1 tools/test_online_g.sh, ~20 min): hand-overs, HP in sync, kill, carves, clear, back in G's guild hall. Fee 'z' not reproduced (needs the screen). Next: G's character-select net packet layout (PCs send MH1's) for a PS2 G in the hunt; an offset map for MH1 lobby C writing MH1 network-state offsets; G save cards for the tests.
- agent D: GB6 ninth pass merged 24 Oct (eft04/15/16/02, set14, em12/em15; full check 42 ok). Left: em02 Fatalis rewrite (RELEASE_G 16; no offline quest spawns it). area_mdlw (monster sub models) fixed and merged 24 Oct (both games). Red line in Kirin shots: god-mode aid left vital_red at ~15000 (fixed, test aid only).
- agent A: G English merged 24 Oct: 7087 official (53.4%) + 1043 own (text/mhg_en_own.tsv, tools/text_own.py); 4703 wait for Freedom (owner asked for an image of their ULUS-10084 in disc/mhfr/); 97 own lines flagged n use provisional names. Town / online screens not seen on screen yet. Own text is Claude's: a person should read it before release.
- agent B: G gaps GN8/GN12/GM4/GM5 done and merged 23 Oct (full check passed on agent-B).
- agent D: GB6 eighth pass (shell08, eft13 Dual Swords, subspecies kinds 35-50, armor count) merged 24 Oct; what is left: RELEASE_G section 15.

### Hand-over, 23 Oct 2026 (start here)
Main = GitHub (all pushed, full check passes). No agent running. Agent branches A, B, D, E fully merged.
- **(merged 24 Oct, see Running above) agent-C (G online)**: the PC G online build reaches G's
  town on our server (docs/network.md 3.12, docs/agents/agent-C.md round 3). Next: TWO=1 tools/test_online_g.sh (two G
  PCs), the real G disc meeting a PC G client (PCSX2, launcher already patches G), G rooms/hunt, Xbox. Merge only after
  a full check; it touches shared net code (cnlbsg.c, cnlbs.c, cnlbs01.c, rt_data.c, rt_game.c, rt_online.c).
- **Owner's Xbox (G) state:** the laptop serves the newest G (dist/xbox/MHG). Owner still has to run
  `XBOX_IP=192.168.1.165 tools/xbox_upload.sh --loader` once (the loader now also sends the save:
  build/xbox/saves/, tools/xbox_save_to_pc.py). New in that build, untested on hardware: freeze watchdog, crash
  catcher, AC97 time-outs, Skip intros (settings menu, Back + L3), save flush, G save slot 0xE00, sharpness, reward menu.
- **Open G Xbox bugs** (docs/RELEASE_G.md GB5 / section 15): black screen on the 2nd start after Back+Start (hang
  before "audio: driver"; step logging + watchdog should name it next time), supply-box cursor in quests (not
  reproduced: needs an owner photo), Forest area 1 fog "thick" on the Xbox (PC matches the real G:
  build/ref_fog/s21_0x4000.png; check the nv2a fog path / an xemu shot). Debug features 4 (input recorder) and 5
  (on-screen info line) on hold.
- **Owner preferences now:** moderate token use; one heavy job at a time; no emulator unless > 2.5 GB free and no other
  emulator running (the owner's other projects run xemu/pcsx2 too: never touch theirs); hidden emulators with an empty
  /dev/input. Laptop 192.168.1.105/.166, Xbox 192.168.1.165, loader server on :8765 (build/xbox/serve.pid).
- **Ideas queued:** PS2 + Xbox + PC side by side on real hardware (docs/DECISIONS.md), English: Freedom (owner supplies
  ULUS-10084 ISO in disc/mhfr/, one text_map.py run), the referee's next items (RELEASE_G section 11).

As of 13 Oct 2026, evening. A-F's MH1 polish round merged (save box, boot/village music, loading
screens + faster Xbox loads, rumble, packaging ./mh1, RELEASE.md, US survey) plus agent B's G
bring-up (VERSION=mhg rebuild 7 OK, 7.5% matched, docs/mhg.md).
State 15 Oct 2026 (all merged and pushed unless noted). Two agents at a time (OOM rule, BRIEF.md).
- G on the PC: title -> new game -> Kokoto village (shops, forge, item box, elder, quest counter) ->
  all 98 offline quests pass tools/test_all_quests_g.sh; G items, hunting log, revivals, event demos.
  G ~19.8% matched. English: G 47%, MH1 76% official US text (tools/text_map.py, build/text/).
- G on the Xbox: installed on the owner's console through the loader's HTTP data install
  (tools/xbox_serve.py --data dist/xbox/MHG; the server picks up a changed dist/xbox/MHG). Pad fixed;
  owner's hands-on test of G on hardware still to come.
- Extras: debug menu (Back+R3 / F9: level select, inventory, toggles), Xbox settings menu (Back+L3),
  prototype gunner HUD from cockpit2.apx (F10/settings -> HUD), docs/FINDS.md (curiosities, prototypes),
  owner launchers tools/play_g.sh + desktop icons, RT_FILEDEF debug switch.
- Owner decisions pending: English for G-only text — owner's idea: base it on Monster Hunter Freedom (US
  PSP, localization of Portable = G content; needs the owner's ULUS-10084 image) with MH1 US for online text,
  own translation in Freedom's style for the rest; to discuss later. Online plan decided 15 Oct: our own
  server + our own open PS2 patches, PS2/PC/Xbox together (docs/DECISIONS.md). Keyboard+mouse on Xbox via the RetroKM dongle as a composite device (docs/xbox.md).
- Xbox: 192.168.1.165 (DHCP), laptop 192.168.1.166. The loader server runs in data mode for G.
Owner confirmed on the real Xbox (13 Oct afternoon): opening correct with sound, bed Save? box,
loading map on area changes (faster), rumble. Agent F (unmerged, agent-F 05a2dc6f): black frame
during game2 steps 3-6 (fixes the old-area frame after a load) and an own AC97 driver
(src/pc/xbox/xbox_ac97.c: DMA reset, codec cold reset + unmute, ac97 state in the log). OPEN: after
Back+Start quit and relaunch WITHOUT a power cycle the game is silent (0 frames played); the
DMA-only fix did not help on hardware; the codec build awaits the owner's test (serve it with
tools/xbox_serve.py --xbe build/xbox/f_test.xbe; the PC was restarted 13 Oct, so restart the server).
Merge F after that test (it must not hang at audio open; RT_AC97_NOCODEC=1 disables the codec part). Still pending: new loader install (tools/xbox_upload.sh
--loader), load: timing lines from the next returned log.
Queue (owner, 15 Oct evening; start when a slot frees, two agents max):
0d. Cross-game audit (owner, 17 Oct): every G-only change since 13 Oct (#ifdef VERSION_MHG, rt_ver_mhg(), src/mhg/,
   config/mhg; ~900 branches): does MH1 have the same fault? Fix it there too. And the reverse: MH1-only fixes
   (#ifndef VERSION_MHG, !rt_ver_mhg(), MH1-only files) checked against G (BRIEF "Fixes go to both games").
0. DONE 16 Oct (agent D). One loader per game (owner asked): today only E:\Games\MH1\default.xbe is the loader and it
   updates whichever game xbox_serve.py serves. Wanted: launching G updates G, launching Monster Hunter
   updates MH1. Loader learns its own folder (MH1 / MHG) and asks /version?game=..., /<game>.xbe;
   E:\Games\MHG\default.xbe becomes the loader, G's game moves to mhg.xbe (the data install must not
   overwrite the loader: leave default.xbe out of G's manifest); a game's exit returns to its own loader;
   xbox_serve.py serves MH1 and G builds at once (dist/xbox/MH1, dist/xbox/MHG). Delivery: one
   tools/xbox_upload.sh --loader run (owner types the FTP login). Test in xemu (alone) first.
0b. DONE 16 Oct (tools/test_three_way.sh). Three-way picture (owner asked): PS2 (PCSX2, tools/test_ps2_online.sh) + Xbox (xemu online,
   tools/xemu_online_test.sh) + PC client in one town on one mh1-server on the LAN address; one screenshot
   of each plus a combined one. Run with NOTHING else (no agent, no full check): both emulators at once.
0c. DONE 16 Oct (agent B, tools/emu_lib.sh). Was: tools/test_three_way.sh (and maybe other xemu callers) leaves the xemu AppImage running: it kills
   run_xemu.sh's PID, not xemu itself (an orphan ran 1 h at 400% CPU on 16 Oct, stopped by the coordinator).
   Also `pgrep -x xemu` does not match the AppImage's process name, so the "xemu runs alone" guard misses it.
1. DONE 16 Oct (agent A: 7 fixed, 2 not reproduced). G Xbox bugs from the owner's play (docs/RELEASE_G.md GB5): debug-menu skip leaves render state
   (half-screen tint triangle, depth order), village music at the title, no villagers on the normal
   village entry, sky frames in the wake-up cutscene. Reproduce on PC G first.
2. DONE 16 Oct (agent E: Back+Black). Bug reporter on the Xbox (owner asked): port src/pc/pick.c (PC-only today) to the Xbox. Freeze with
   a pad combo that does not clash with Back+Start (quit), Back+R3 (debug), Back+L3 (settings); pointer
   from the right stick / USB mouse, marks with A / click, note via USB keyboard or on-screen keyboard;
   save the report to E: and POST it to the laptop through the loader server (tools/xbox_serve.py) into
   build/xbox/reports/. Step toward the USB keyboard + mouse plan (docs/xbox.md, RetroKM composite).
Laptop is now 192.168.1.105 (DHCP); the owner re-ran tools/xbox_upload.sh --loader 15 Oct.
Known unfixed (the bed "quit game?" -> Forest camp bug was fixed 16 Oct, agent B): bed sleep jingle
has no sample (village sound pack not loaded).
Full check before a push: `tools/full_check.sh` (rebuild 5 OK, PC build, every tools/test_*.sh,
both Xbox builds) plus `VERSION=mhg tools/rebuild.sh` (7 OK).

## Xbox bench (real hardware)

docs/xbox.md "Real Xbox bench setup" onwards. Short version: the owner powers the Xbox; builds
reach it through the network loader (tools/xbox_serve.py on the laptop, port 8765;
`--input/--env` for scripted test runs); the game's log comes back on the next start
(build/xbox/logs/). Capture card: find the node with tools' dev.sh logic (MacroSilicon "USB
Video"), raw yuyv422 1280x720. Pad: RetroKM hub 127.0.0.1:24851 (`goto xbox`, then
`pad <hex>` taps of ~50 ms; the KVM must have the xbox screen active). The Xbox's DHCP address
changes (192.168.1.115/116/118/165 so far; MAC 00:0D:3A:31:08:AB); the laptop keeps 192.168.1.166 as a saved second address.

## Gameplay clips for the owner

Never a window on the desktop. PC: scripted run with --shot series, or xemu with
`tools/run_xemu.sh ... --record out.mp4` / `RECORD=1 tools/xemu_quest_loop.sh` (hidden display).
Real Xbox: ffmpeg from the capture card. Clips stay in build/ (Capcom imagery, never committed).
