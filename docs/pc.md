# PC viewer (first piece of the PC port)

## Handover summary (agent A, 7 Oct 2026; read this first)

The PC build (`tools/build_pc.sh` -> `build/pc/mhview`, 32-bit x86; ARM via
`tools/build_arm.sh`) plays MH1 offline from power-on: logos, title, new
hunter / continue, memory card on host files, the village (Elder, shops,
forge, item box, house bed save), quests from the Elder, every monster kind,
items (gathering, fishing, bombs), quest clear / failure, reward, and the
star-level progression. Game logic is the decompiled C (matched files and
*_nm near-matches); src/pc/ holds the platform side and the glue.

Checks to run after changes (all headless, about a minute together):
- `tools/test_quest_loop.sh`: power-on -> new game -> quest 131 -> reward ->
  bed save -> CONTINUE (1550z).
- `tools/test_progression.sh`: star levels 1 -> 3 with marked clears, kept by
  the save.
- `tools/test_urgent.sh`: urgent quests 136 and 137 hunted for real; each
  clear opens the next star level.
- `tools/test_audio.sh` (agent D, 7 Oct 2026; ~10 s): headless audio dumps of the
  title, the village (walking) and a quest fight; fails on a near-silent
  second (RMS < 150) or a BGM stream that stops partway. Caught: entering the
  house calls str_stop_all and lobby_bgm_set then "kept" a stream that no
  longer played (the village stayed silent for good; bgm_nm.c now checks
  str_getstat).
- The whole PC test set, run it in this order after any change (build_pc.sh
  alone first): test_quest_loop, test_progression, test_urgent,
  test_name_entry, test_movie, test_frog, test_audio, test_activities, test_all_quests
  (~2.5 min), then `. ~/xboxdev/env.sh; python3 tools/build_xbox.py` and
  `tools/rebuild.sh`.
- `tools/test_save.sh` and `tools/test_save_import.sh` (agent C, 8 Oct 2026): PS2 save import / export, section
  "Importing a PS2 save".
- `tools/rebuild.sh`: the PS2 rebuild (all five OK) when game C was touched.
- `tools/test_menu.sh` (agent F, 9 Oct 2026; ~5 s, needs the save of test_quest_loop.sh): the F10 settings menu pauses the game, resumes after
  Esc and writes its changes to the ini (section "Settings menu").
- `tools/test_log.sh` (agent B, 7 Oct 2026; ~3 s): the automatic debug log (below) is created, rotated
  and, with RT_CRASH_TEST=1, gets a crash section. `RUN=wine BIN=build/win/mhview.exe` runs it (and
  test_quest_loop.sh) on the Windows build.
- Every run writes a debug log automatically (`~/.local/share/mh1pc/logs`, Windows `%APPDATA%\mh1pc\logs`),
  and `tools/win/` + `tools/build_win.sh` make a Windows exe: see "Debug log" and "Windows build".

How the PC wires game C (where most bugs were): no-op stand-ins generated
for missing functions (build/pc/rt_gen.c, tools/gen_rt_auto.py) — grep them
first when a feature does nothing; per-file ABI adaptors for calls whose
PS2 argument registers differ from the C prototype (src/pc/rt/rt_abi.c, ABI=
lines in build_pc.sh; tools/argregs.py); fields the PS2 fills in trans()
that move-side code reads (world matrices at EMW/PLW+0x60 are rebuilt per
tick). Test aids are environment variables (RT_*), listed in the "Run"
section and the round sections below; RT_PL_GOTO, RT_PL_TARGET=kN and
RT_QCLEAR are the newest.

Movies (agent B, 7 Oct 2026): the opening, the extras' movies and the title's
idle loop (logos -> opening -> title, the game's own demo task) play from
AFS00.AFS; see "Movies (libmpeg2)" below. The soft keyboard is the game's own.

- `tools/test_activities.sh` (agent C, 7 Oct 2026; ~15 s headless, 8 runs at a time): the activities the quest sweep does not
  cover, one PASS/FAIL line each (gathering, fishing, carving, village shops / forge / item box, item combining, items used
  in a quest, the field trader). Section "Activity tests" at the end of this file.
- `tools/test_all_quests.sh` (agent D, 7 Oct 2026; ~2.5 minutes, headless): every offline quest of the Elder's star
  levels 1-5 (131-171) started with `--quest N`, played to quest clear, reward screen, money, village. Table below.

Known gaps: reverb is an
approximation, online play, ARM frame rate measured only up to round 20
(25-28 fps at 960x720), nothing systematically compared with the PS2.

`build/pc/mhview` is a real-time viewer written in C99. It loads MH1 data
straight from the user's disc files at run time, with nothing extracted to
disk, and shows a stage (default 4, st04; `--stage N` for others) with the Rathian
(em01) and a hunter standing in it. Both play their motions in real time. Camera is free-fly.

Screenshot caveats (agent D, 7 Oct 2026, gallery pass): `--follow` switches to the host follow camera, which has
no wall collision, so it ends up inside rock in caves and nests (stages 40, 36, 37, 18, 14); use the game camera
(no `--follow`) for shots. RT_PL_WARP / RT_PL_WARP_EM teleport the hunter without resetting the game camera, so the
eye stays pinned thousands of units away (k_HitWallCamera/GetWallHitBit2 pushing it back); a real stage change calls
rt_cam_init and is fine. Free-cam `--stage N` shots (no `--quest`) show the village minimap, a flat blue sky, and
near-black views for stages 21/42/20/48 where the camera sits in geometry; with `--quest N --stage S` and the game
camera every stage is lit normally. The Fortress start (stage 14, quest 101) is a rampart: the game's own intro
camera pulls out below the wall at tick ~200, which looks like a wall niche but is not a spawn bug.

## Display options (agent F, 8 Oct 2026)

Window, fullscreen, quality and widescreen. Defaults keep the original look: a 4:3 picture (the PS2 frame is
512 x 448 shown at 4:3), vsync on, no MSAA, bilinear 2D art. Code: src/pc/gfx/gfx_gl.c (layout, MSAA, anisotropic,
2D filter, fullscreen), src/pc/gfx/gfx_opts.c (settings file, flags, frame cap), src/pc/gfx/gfx_gl.h. The Xbox
build does not use any of it (it stays 4:3; gfx_nv2a.c ignores the new render state 0x105).

Settings file `mh1pc.ini`, next to the save folder (Linux `~/.local/share/mh1pc/mh1pc.ini`, Windows
`%APPDATA%\mh1pc\mh1pc.ini`; with MH1_SAVE_DIR it sits in that folder's parent). `key = value`, written back on a
normal exit (not by --shot runs), so the window size / position and Alt+Enter fullscreen are remembered.
Precedence: built-in defaults < file < command-line flags.

| flag | ini key | meaning |
|----|----|----|
| `--fullscreen` / `--windowed` | `fullscreen` | desktop fullscreen (borderless, the 3D is drawn at the desktop resolution). Alt+Enter or F11 toggles at run time |
| `--size WxH` | `window_w`, `window_h`, `window_x`, `window_y` | windowed size / position (remembered; default 960x720, 1280x720 with widescreen). The window is resizable; the 3D is always drawn at its real pixel size |
| `--vsync` / `--no-vsync` | `vsync` (1/0) | swap interval |
| `--fps-cap N` | `fps_cap` | most drawn frames per second, 0 = off. Only drawing is limited: the game logic runs from a clock at 30 Hz, so 60 fps or uncapped never speeds the game up (the motions are interpolated per frame, positions change per tick) |
| `--msaa N` | `msaa` | 0, 2 or 4 samples (falls back to 0 if the driver refuses; the bug reporter's id pass switches it off) |
| `--aniso N` | `aniso` | 1 (off) to 16: anisotropic + trilinear for the 3D textures (mip levels are generated only when N > 1; the 2D art never uses them) |
| `--widescreen` or `--aspect 16:9` / `--aspect 4:3` | `widescreen` | see below |
| `--filter2d nearest\|linear\|sharp` | `filter2d` | HUD / menu / text art: nearest keeps the original pixels hard at any scale; sharp is nearest at the whole-number part of the scale and bilinear for the rest (below) |
| `--english`, `--japanese`, `--text-table FILE` | `language` (`ja`/`en`), `text_table` | text language, see "Settings menu" |
| `--confirm circle\|cross` | `confirm` | button layout, see "Settings menu" |
| `--ini FILE`, `--no-ini` | | use another settings file / ignore it |

Layout (gfx_gl.c `layout()`): `rc` is the largest 4:3 rectangle in the window, `sc` the 3D scene rectangle.
- 4:3 mode (default): 3D and 2D both on `rc`, black bars around it in any other window shape.
- Widescreen (windows at least 4:3 wide; narrower ones fall back to 4:3): the 3D fills the window. The game's
  field of view is the vertical one (flmat_perspective takes it as fovy), so only the aspect ratio changes and the
  horizontal view widens (Hor+); the hunter and the framing stay as in 4:3. Menus, the title, movies and all
  2D screens stay on the centred 4:3 rectangle with black bars; fades and full-frame untextured rectangles
  (dimming) cover the whole window (rt_2d.c `quad`, rt_boot.c `rt_fade_draw`).
- HUD anchoring: new render state `GFX_RS_2D_ANCHOR` (gfx.h: GFX_A_CENTER / LEFT / RIGHT / STRETCH). The in-quest HUD
  is trans_pit_0/1/2 of the matched menu18.c, which cannot be edited, so tools/build_pc.sh renames its calls
  (`-Ddisp_timer=rt_hud_disp_timer ...`) to wrappers in rt_2d.c that set the anchor: timer / vitals / sharpness / other
  players' info keep to the left edge, the map and the item bar (and the ammo select) to the right edge. Text of those
  widgets sits on the font stacks and is drawn later, so rt_font.c stores the anchor with each print. Pause menus
  and everything else stay centred. Draws whose virtual size equals the window (the bug reporter) are stretched.
- Culling: nothing to adjust. flCheckMeshFOV and Create_FOV are stand-ins on the PC (rt_main.c: everything is
  visible, the GPU clips), so there is no 4:3 frustum to pop objects at the new edges. The bug reporter's unproject
  uses the scene rectangle (gfx_pick_viewport).
- Village (9 Oct 2026): the lobby draws through trans_pit_1_lb / trans_pit_2_lb (the same menu18.c as the quest HUD). What it
  puts on screen while walking is the talk window (Disp_NPC_message), the chat and message lines (Pit_disp_chat,
  Pit_disp_receive_mes) and the item-pickup / info banner; the village menus, shops, forge, item box and the quest board
  (the Elder's list) are whole 4:3 layouts and stay centred on purpose. chat_nm.c is compiled with the three chat
  functions renamed (tools/build_pc.sh, `rt_real_*`) and rt_2d.c defines them again with the left anchor, so every caller
  (the lb and the quest-side trans_pit_2) gets it: the talk window sits at the window's left edge, no longer 190 px in.
  The offline village has no other HUD (no map, timer or vitals); the green info banner already reached the left edge.
- flvecrRotTransPers (world point -> screen; 9 Oct 2026): rt_2d.c now calls gfx_project (gfx.h), which multiplies the point
  by the current world (flSetRenderState 0x1A, which every caller sets to identity first), view and projection states and
  returns x, y in the 512 x 448 frame (the frame the HUD draws on, so a tag lands on its hunter in 4:3 and in widescreen,
  where x runs below 0 and above 512), z 0..1, w = clip w (> 0 in front, which is what the callers test). The PS2 routine
  (src/main/fl/flm04.c) does view * world * (projection * viewport) in its register file; same result here, from the gfx
  backend's matrices (GL, nv2a; gfx_null returns zeros). Users: the player name tags (lb_ai.c, Pit name display in
  menu_disp_nm.c, lb_v08), the ballista / bow sights (weapon2.c) and the sound pan (snd_nm.c: before, every 3D sound
  was panned as if at x = 0). Checked: the hunter's own "TEST" tag sits on its head in the village at 1280x720 4:3 and
  16:9. The village NPCs have no name tags offline (nothing to anchor); the tags the game draws for other players only
  appear online / co-op, with the same projection.
- 2D filter "sharp" (9 Oct 2026): a GLSL 1.20 fragment shader (functions fetched at run time; without shaders the filter
  is bilinear) on the 2D draws. With N = floor(window pixels per game pixel) the texture coordinate is pulled to the texel
  centre except within 1/N texel of an edge, so the art is nearest at the integer part of the scale and the bilinear
  blend is confined to 1..2 window pixels at each texel edge (at an exact integer scale it is nearest). Scales below 2
  are plain bilinear. Compared at 1600x900: crisper window borders and glyph edges than linear, without nearest's
  uneven pixel widths.
- RT_2D_TRACE=1 prints every 2D draw (virtual size, anchor, box, texture size) to stderr: for widescreen work.

### Settings menu (agent F, 9 Oct 2026)

F10 opens it at any time (on the controller Back + L3; the title / mode-select screen shows a small "F10" hint). The game
does not tick while it is open (viewer.c asks menu_hold_ticks, as for the bug reporter; the clock is shifted so nothing
catches up afterwards), the music is paused and the last picture stays drawn under a dimmed overlay, so live changes show
at once. Keyboard: Up / Down, Left / Right, Enter / Space (next value), Esc / F10 close. Controller: D-pad or left stick,
cross changes, circle / start / back close (on release, so the game does not see the press). Mouse: click a row (right
click = previous), wheel. Code: src/pc/menu.c (own 5x7 font from pick.c on an atlas texture, window-pixel 2D draws),
menu.h (stubs on the Xbox), hooks in viewer.c.

| row | what | live? |
|---|---|---|
| Display | windowed / fullscreen (same as Alt+Enter) | yes |
| Aspect ratio | 4:3 / 16:9 wide (Hor+, as above) | yes |
| VSync | swap interval | yes |
| Frame rate cap | off, 30, 60, 72, 90, 120, 144, 165, 240 | yes |
| Anti-aliasing | off / 2x / 4x | at the next start (the GL context decides it) |
| Anisotropic filter | off .. driver limit | yes: textures made before get their mip levels with glGenerateMipmap (a registry of live textures in gfx_gl.c) |
| 2D art filter | smooth / sharp (integer) / nearest | yes |
| Text language | Japanese / English (English only when a table file exists) | at the next start |
| Button layout | circle confirms (Japan) / cross confirms (West) | yes |
| Music / Effects volume | the game's own options: option_w[1] / option_w[2], 0..7 | yes |
| Restore display defaults / Close | | |

Notes. The volumes are not a copy: they edit the same bytes the game's OPTION screen edits (option_w, system_w 0x36 / 0x37,
str_master_vol(1), as option_nm.c does), and the memory card keeps them when the game saves. Language: the text layer
(rt_text.c, docs/english.md) re-points the game's string tables while the data is imported, so the choice is read at
start-up: `language = en` in the ini hands the table to RT_TEXT_TABLE when one is found (the `text_table` key or
`--text-table`, else RT_TEXT_TABLE, else text/en.txt next to mh1pc.ini, in the working folder or next to the program); without
a table the row stays Japanese and says what is missing. Button layout: rt_pad_set swaps the cross and circle bits (one
of them held) for the whole game, so the in-game menus and the controls both move; the button icons the game draws in its
text keep the Japanese shapes (they are font glyphs). The settings are written to mh1pc.ini when the menu closes (and at exit).

Test: `tools/test_menu.sh` (uses the save of test_quest_loop.sh): RT_MENU_AT=tick opens the menu at that tick,
RT_MENU_KEYS="down,right,enter,esc,..." feeds one key per two frames, RT_MENU_SAVE=1 lets a --shot run write the ini. It checks
that the game does not tick while the menu is open and runs on after Esc, and that the changes reach the ini.

Tested 8 Oct 2026 (headless shots, build/show/F, never committed): village, quest 131 and the pause menu at
960x720 4:3 and 1280x720 widescreen, plus --msaa 4 --aniso 8 --filter2d nearest; the PC test set at the defaults.

## Build

The build is 32-bit (`gcc -m32`, see "Port runtime" below and
docs/DECISIONS.md). Needs gcc, the 32-bit runtime libraries (libc6:i386,
libsdl2-2.0-0:i386, libgl1:i386) and either gcc-multilib or the no-root
sysroot made by tools/setup_pc32.sh (downloads libc6-dev-i386 and
lib32gcc-13-dev into build/sysroot32). No other libraries.

    tools/setup_pc32.sh          # once, only without gcc-multilib
    tools/build_pc.sh            # -> build/pc/mhview

## Run

    build/pc/mhview disc/mh1

`disc/mh1` must contain `AFS_DATA.AFS` and `SLPM_654.95` (and `AFS00.AFS` /
`AFS01.AFS` for sound; without them the viewer runs silent). The ELF and the
game.bin overlay (an AFS_DATA entry, stored uncompressed) are read for the
hunter's part-to-bone table (ptmat_tbl, 0x3018F0) and the game data tables
the decompiled C uses (src/pc/rt/rt_data.c).

Controls (free camera, the default):
- WASD move, mouse look.
- Space / C move up / down; Shift moves faster.
- Esc quits.

Controls with `--play` (the pad drives the hunter, camera follows):
- An SDL game controller (Xbox layout: A cross, B circle, X square,
  Y triangle, LB/RB L1/R1, LT/RT L2/R2, Back select, Start start), and the
  keyboard: W/A/S/D left stick, arrow keys right stick (turns the camera),
  K cross, L circle, J square, I triangle, Q L1, E R1, Z L2, C R2, Enter
  start, Backspace select, T/F/G/H d-pad.
- The hunter is driven by the game's own player code (see "Player"
  below), with the PS2 controls: left stick move (push lightly to walk),
  right stick draw / attack (flick direction picks the attack), cross
  roll, R1 guard, circle sheathe (weapon out), square use item. The camera
  is the game's: d-pad left/right turn it, up/down zoom, L1 resets it
  behind the hunter. Note: the arrow keys are the right stick, so they
  attack as on the PS2.

Screenshot mode, for checking without looking at the window: it renders
offscreen in a hidden window, reads the back buffer and writes a PNG.

    build/pc/mhview disc/mh1 --shot build/show/pc_viewer.png --frames 2 --time 1.0
    build/pc/mhview disc/mh1 --shot out.png --size 640x480 --cam 11250,260,8150,0.72,-0.12 --time 2

| option | meaning |
|----|----|
| `--time S` | freeze the animation clock at S seconds |
| `--frames N` | frames to render before the screenshot |
| `--cam x,y,z,yaw,pitch` | camera position and angles (radians) |
| `--size WxH` | window size |
| `--stage N` | stage number (game_w.stage, 0-87, hex with 0x), default 4 |
| `--quest N` | load quest N's mission file (questName[N], 1-0xB1) and start the hunt as the game does: on the quest's start stage (base camp; quest 10: stage 21, the Rathian in her nest on stage 40), the supply box filled. `RT_QUEST_STAGE=1` starts on the stage of the quest's own monster instead (scripted fight tests); `--stage` overrides both |
| `--play` | the pad (controller + keyboard) drives the hunter; follow camera |
| `--input SCRIPT` | scripted pad for tests, implies --play: `idle*10,up*50,left+cross*15` = ticks per step; names in src/pc/pad/pad.h |
| `--follow D,H,P` | `--play` camera: distance D behind, H above the hunter, pitch P (default 900,450,-0.3); the yaw is `--cam`'s |
| `--audio-dump FILE.wav` | no audio device; mix 1/30 s per game tick into a 48 kHz stereo wav (for checking sound offscreen) |
| `--mute` | no sound at all |
| `--sw-trace` | print, per tick, the pad state the game's sw_set_sub gave player 0 and the player's position/angle |

Environment variables for the player: `RT_WEAPON=id` (Ken_data id, default
156, the first sword and shield; 1 = the first great sword), `RT_PL_TRACE=1`
(action, step, motions, frame, position per tick), `RT_PL_STANDIN=1` (the
old host stand-in), `RT_EM_POS=x,z` (put the Rathian there, for hit tests),
`RT_HIT_DM=1` (print damage the Rathian takes; `2` also lists live attack
shells and their hit volumes), `RT_SKIP_TYPE=n` (do not draw prims of
effects/sets of type n).
Monster: `RT_EM_TRACE=1` (per tick: enemy_mv step, action, motion, frame,
position, angle, hit points, mode 0 calm / 1 attack; at spawn the part
durabilities), `RT_EM_STANDIN=1` (the old host stand-in: root motion and
collision only, no AI). Test aids that change the game (scripted fights
only): `RT_PL_GOD=1` (hunter vital back to 100 each tick), `RT_PL_AIM=1`
(hunter faces monster 0 while standing), `RT_DMG_MUL=n` (damage to
monster 0 times n). `RT_EM_POS` also moves a `--quest` monster.

Verified 5 Oct 2026 with build/show/pc_viewer.png and
pc_viewer_close_0.5.png / _2.0.png:
- the stage is textured, with sky, waterfalls and ruins;
- the Rathian stands on the plateau, and the two times show different
  frames of its motion;
- the hunter stands on the stone path in its idle motion.

## Debug log (agent B, 7 Oct 2026)

Every run of mhview (Linux, Windows, Xbox) writes `logs/mh1_YYYYMMDD_HHMMSS.log` with no switch, in a
folder next to the save directory: `~/.local/share/mh1pc/logs` (`$MH1_SAVE_DIR/../logs` when that is set),
`%APPDATA%\mh1pc\logs` on Windows, `E:\Games\MH1\logs` on the Xbox (not in UDATA: the dashboard lists
every folder there as a save). `MH1_LOG_DIR` overrides it. The newest 20 files are kept (only files named
`mh1_*.log` are ever deleted). Code: src/pc/rt/rt_log.c / rt_log.h (also built into the Xbox XBE).

- Header: build (git hash baked in by build_pc.sh, "+" when the tree had changes), platform and OS version
  (Wine is named), CPU count, disc / save / log paths, arguments, GPU / OpenGL, SDL version, window size,
  controller name (and plug / unplug), audio driver, device, rate.
- Events (`I`): boot steps, game mode changes (init / loading / quest / quest clear / result / village), stage
  changes, `quest N START`, `quest N CLEAR (result code 4)` / `FAILED` (codes 6, 7; game_w+0xD5), reward
  screen, back in the village, memory card open / save / load / folder / delete, movie start / end / skipped.
  The tick number (`tNNN`) is the game tick, the first column seconds since start.
- Warnings (`W`, flushed at once): missing files (AFS00 / AFS01, a mission file, an AFS_DATA entry that
  does not read), save write failures, the first call of every no-op stand-in function
  (`stand-in called (not ported, does nothing): NAME`: every NOP / STUB macro in the rt_*.c files and the
  generated rt_gen.c call rt_log_standin), audio underruns (the SDL callback measures the gap between calls and
  its own mixing time; the main loop reports "audio underrun: N late callbacks").
- Once a minute: `summary: fps, worst frame, game ticks, CPU % of one core, mode / stage / quest, warnings`.
- Crash (SIGSEGV / SIGBUS / SIGFPE / SIGILL / SIGABRT on Linux; an unhandled exception or abort() on
  Windows): a `===== CRASH =====` section with the reason (and fault address), the build, uptime, the game's
  mode / step / stage / quest, a backtrace with function names (from the build's own symbol table,
  rt_symtab.c: `rt_host_symname`; a static function shows as the global symbol before it, and frames
  without a name are libc or the like), and the last 200 log lines (the stdio buffer is not flushed on a
  crash, so these repeat the tail). Then fsync, and the process ends itself: no core file, no Windows error
  report, no memory dump (it would hold the game's data). The Windows backtrace is a scan of the stack
  for return addresses in the exe (no DbgHelp / PDB). The Xbox build has no crash handler.
- Privacy: nothing is sent anywhere. Every line passes a scrub that turns `$HOME` / `%USERPROFILE%` and any
  `/home/NAME/`, `/Users/NAME/`, `X:\Users\NAME\` into `~`; no environment values are written.
- Cost: a 32 KB stdio buffer, flushed on warnings and every 3 s; about 60 fps with 16% of a core in
  the windowed title screen on this machine (about the same as before).
- Test aid: `RT_CRASH_TEST=1` writes a null pointer after 60 drawn frames (`=2`: abort()).
- `tools/bug_report.sh [--with-save]` (Linux) / `bug_report.bat` (Windows, PowerShell) zip the newest two
  logs and a listing of the save folder (names, sizes, dates; the save itself only with `--with-save`) into
  `mh1_bug_report_<date>.zip` in the current folder and print the issue link
  (https://github.com/itsbigcforme67/mh1-xbox/issues/new?template=bug_report.md; the template is
  .github/ISSUE_TEMPLATE/bug_report.md).
- Not verified: the Xbox part (rt_log.c compiles and links into the XBE, never ran in xemu / on a console);
  Windows paths with a non-ASCII user name.

## Windows build (agent B, 7 Oct 2026)

`tools/build_win.sh` cross-compiles the PC build for 32-bit Windows on Linux (no root): it runs
tools/build_pc.sh with llvm-mingw's `i686-w64-mingw32-clang` (the way build_arm.sh does for ARM), in the
symlink tree `build/win/tree` so that build/pc (the Linux build) is not touched. Result: `build/win/`
with `mhview.exe`, `SDL2.dll`, `play.bat`, `bug_report.bat` + `.ps1` (about 14 MB; the game's data is not in it).

Tools, outside the repo in `~/mh1win` (`WINROOT` overrides; both are plain downloads, no install):
- `llvm-mingw-20261006-msvcrt-ubuntu-22.04-x86_64` from github.com/mstorsjo/llvm-mingw/releases (the
  msvcrt flavour: it runs on every Windows from 7 on. The ucrt flavour needs `WINCRT=ucrt`; it ran on
  Wine 9 only up to a missing `powf`, a Wine gap, so msvcrt was chosen.)
- `SDL2-2.32.10` from `SDL2-devel-2.32.10-mingw.tar.gz` (github.com/libsdl-org/SDL/releases), the
  `i686-w64-mingw32` folder inside.

What had to change (Windows side only, behind `MH1_WINDOWS` = `_WIN32` + `-DMH1_WIN`, see rt_plat.h; the
Xbox is `_WIN32` without it):
- build_pc.sh: `LIBS`, `EXE`, `EXTRA_CFLAGS`, `GAME_NOAGG` (clang has no `-fno-aggressive-loop-optimizations`),
  `SYMTAB_ARGS` (COFF `_` prefix; the table is made from the objects, not the exe, whose C runtime symbols
  clash with libc declarations), `nmc` (nm without the `_` prefix for the weak / alias requests),
  `LINK1_OPTS` / `LINK1_TOLERANT` (lld has no `--warn-unresolved-symbols`: the first link's "undefined
  symbol" errors are read instead). The recorded `-rdynamic` / dlsym of the old builds is gone since the
  generated symbol table (rt_symtab.c) serves rt_data.c on all platforms.
- Game C flags: `-fno-builtin` (the game headers declare `memset()` K&R; as nxdk-cc does) and the relaxed
  clang 18 errors (implicit function declarations etc., as build_xbox.py).
- rt_mc.c (memory card on host files): `_mkdir`, `localtime_s`, an own `*`/`?` glob instead of fnmatch,
  `%APPDATA%\mh1pc\memcard0`. rt_prof.c: `GetThreadTimes` instead of clock_gettime. gfx_gl.c:
  `GL_CLAMP_TO_EDGE` (opengl32's GL 1.1 header). The console subsystem stays (stderr is visible);
  `-DSDL_MAIN_HANDLED`.

Running it under Wine (checked 7 Oct 2026; `export WINEPREFIX=~/mh1win/prefix WINEDEBUG=-all`):
`RUN=wine BIN=build/win/mhview.exe tools/test_quest_loop.sh` passes (power-on, new game, quest 131 played
and cleared, reward, bed save, CONTINUE 1550z; the log shows the whole sequence), and `tools/test_log.sh`
passes with a Windows crash section. The Windows exe starts the same game C (same stand-in counts as Linux).

Installing from an ISO (src/pc/install.c; Linux and Windows, same code): the player's own Japanese
disc image is read directly (a small ISO9660 reader, no external tools; 64-bit seeks because the ISO is
4 GB) and AFS_DATA.AFS, AFS00.AFS, AFS01.AFS, SLPM_654.95 and SYSTEM.CNF (about 925 MB; nothing else is read by
the PC build) are copied into a data folder: `data` next to the exe if writable (Windows), else
`%APPDATA%\mh1pc\data` / `~/.local/share/mh1pc/data`. Checks: SLPM_654.95 must be on the image, sizes and
CRC32 of every file must be the known ones (a wrong disc or a damaged image gives a message box / stderr
text and installs nothing; a copy goes to `*.part` and is renamed after its checksum matched). A small
progress bar window shows the copy; `installed.ok` lists the files and CRC32s. Later launches find the
folder themselves (exe/data, exe/disc, the user folder) and never touch the ISO. Ways in:
`mhview --install FILE.iso [--install-dir DIR]` (installs, exits), an `.iso` as the disc argument or dropped on
mhview.exe / play.bat (installs, then starts the game from power-on), and with no data found a prompt
(message box, then a window that takes a dropped file via SDL_DROPFILE; on Windows Enter opens a file
dialog). `RT_NO_GUI=1` keeps it to stderr. The result is in the debug log (`install: ...`).
Checked 7 Oct 2026: Linux `--install` of the owner's ISO into /tmp (3.8 s) gives files byte-identical to
disc/mh1 and the game boots from it; the same under Wine (9 s, incl. the >2 GB seeks); a text file named
.iso is refused. Not checked: the file dialog, the drop window with a real drag (only the
progress window ran), a read-only exe folder, non-ASCII paths (fopen with UTF-8 names on Windows).

How a Windows player runs it (nothing of Capcom's is in the build):
1. Drag their own Japanese Monster Hunter PS2 ISO (SLPM-65495) onto `mhview.exe` or `play.bat` once.
2. `play.bat` afterwards (power-on, title, new game / continue), `play.bat quest`, `play.bat easy`,
   `play.bat village` (same as tools/play.sh; window 1024x768, `set MH_SIZE=1280x720` first to change).
   An extracted folder works too: `play.bat D:\mh\files` or its path in `disc_dir.txt`. Saves in
   `%APPDATA%\mh1pc\memcard0`, logs in `%APPDATA%\mh1pc\logs`; after a problem run `bug_report.bat`.

Test release zip: `tools/package_win.sh` (after build_win.sh) makes `build/release/mh1pc-win32-<date>-<hash>.zip`
with the exe, SDL2.dll, play.bat, bug_report.bat/.ps1, a README.txt for testers and the licenses (libmpeg2 GPL,
SDL2 zlib, a GPL notice). It refuses to zip a file over 25 MB, a total over 40 MB, or any name like game data
(AFS*, SLPM*, SYSTEM.CNF, *.iso, *.bin ...), and checks the finished zip again. It does not publish anything.

Hardening (7 Oct 2026, second pass): the whole single-player set passes on the Windows exe under Wine
(`RUN=wine BIN=build/win/mhview.exe tools/test_<name>.sh`, all scripts take RUN / BIN; test_all_quests.py too).
Windows-only bugs found and fixed: stdout / stderr were text mode (CRLF in redirected logs broke the
`$`-anchored greps of test_progression / test_urgent: now `_setmode` binary); paths were ANSI (new
src/pc/rt/win_utf8.h, force-included into the host-side C of the Windows build: fopen / remove / rename /
mkdir / stat / opendir / getenv go through the wide-character functions, and main converts the command line
to UTF-8; checked with a save, log and screenshot folder named "utf8 José 日本" under Wine). All other fopen
calls were already "rb" / "wb". Struct alignment of double / long long: no failure found (all 38 quests and the
tests pass), nothing changed. play.bat checked under `wine cmd /c` (quest mode with a disc folder: log shows the
window, WASAPI audio device, controller line); bug_report.bat could not run (Wine 9 has no PowerShell), the
.ps1 got a fix by reading (extra args with a param block would have failed).
tools/package_win.sh now strips the exe (llvm-strip: 12 MB -> 2.5 MB, no build paths) and refuses when the
user name appears anywhere in the package.

Untested on real Windows: everything. Only Wine 9 on Linux (its OpenGL is the host Mesa) has run the exe; no
real Windows, no real GPU driver of Windows (the GL path asks for a 2.1 context and uses the fixed
pipeline, so any driver should do), no Windows audio (the hidden-window test runs have no audio device),
no controller, `play.bat` / `bug_report.bat` / `bug_report.ps1` never executed (written from memory of
batch and PowerShell syntax), the movie decoder (plain C libmpeg2, no MMX), Windows 7 / 8, antivirus
reactions to an unsigned exe, paths with spaces or non-ASCII characters.
Differences that may matter: the Win32 ABI aligns `double` / `long long` in structs to 8, the Linux i386 ABI
to 4 (the Xbox build is the same as Windows here and works); clang instead of gcc compiled the game C
(the test quests all ran under Wine, but only test_quest_loop and test_log, not all_quests / audio).

## How to play co-op (agent E, 7 Oct 2026)

Up to four players hunt one quest together, each on his own PC (Linux or Windows), connected directly: one player
hosts, the others join him. There is no lobby or town; afterwards everyone is back in his own (single-player)
village. Details and what is not done yet: docs/network.md, sections 1a and 3.4.

**1. Build the online version** (the normal build has no network code at all):

    ONLINE=1 tools/build_pc.sh        # Linux: build/pc/mhview_online
    ONLINE=1 tools/build_win.sh       # Windows: build/win/mhview_online.exe (+ SDL2.dll)

**2. Your hunter.** Each player plays the hunter of his own save (the memory card folder: Linux
`~/.local/share/mh1pc/memcard0`, Windows `%APPDATA%\mh1pc\memcard0`): name, look, armour, weapon and pouch. Make one
by playing normally first (new game, save in the house bed). `--hunter 2` picks save slot 2 (default: the first one).
After the quest, the reward and the money are written to that save automatically, and you are in the village.

**3. Start.** Everyone runs `mhview_online DISC_DIR --coop`. A small window asks:
* host: the quest (the Elder's quests, 1 to 5 stars) and the number of players (2-4). The host waits until everyone
  is in, then the quest starts for all;
* join: the host's address.
On Linux the window needs zenity or kdialog (else the questions come in the terminal). Without the window:

    mhview_online disc --host --quest 137 --players 2       # host
    mhview_online disc --join 192.168.1.20                   # the others: the host's address

`--port N` changes the port (default 10300, both sides).

**4. Same house / same network (LAN):** the joiners use the host PC's LAN address (e.g. 192.168.1.20: `ip addr` on
Linux, `ipconfig` on Windows). Allow mhview_online in the firewall (Windows asks the first time).

**5. Over the internet:** the host's router must forward **TCP port 10300** (or the `--port` chosen) to the host's
PC, and the joiners use the host's public address.
The port is safe by default: **it only talks to local addresses** (127.x, 10.x, 172.16-31.x, 192.168.x). To play with
a friend over the internet, name his address on purpose, on both sides:

    mhview_online disc --allow 203.0.113.7 --coop             # host: accepts the friend at 203.0.113.7
    mhview_online disc --allow 198.51.100.4 --coop            # friend: may connect to the host at 198.51.100.4

`--allow` can be given several times (or `RT_NET_ALLOW=a,b`). The MH Oldschool servers are refused even when named
(CLAUDE.md: the project does not connect to them without their operators' permission).

**What works / what to expect:** everyone sees the others with their armour and weapons and their names; monsters
are shared (one PC runs each monster, the others follow it, and it moves to another player when that PC leaves the
area or the game); kills, quest clear, the 3-cart failure, time-out and abandoning are shared; the supply box is
decided by the host. If the **host** leaves, the joiners can no longer see each other (the host relays everything).
No chat yet. The tests: `tools/test_coop.sh` (about 15 minutes, all headless on 127.0.0.1).

## Bug reporter (F8) (agent B, 8 Oct 2026)

In the GL builds (Linux, Windows) F8 or Back/View + Start on a controller freezes the game: no game ticks, the
audio device is paused, the game clock stops (and continues where it was after Esc / Enter). The frame is grabbed,
then drawn once more into an id buffer (below). The mouse picks broken things: click = the draw call under the
cursor, drag = a box (every id inside), right click = undo the last pick, typing = the note (Ctrl+V pastes),
Enter saves the report, Esc or F8 cancels. Picked objects are tinted and outlined on the frozen picture and
listed in a panel (descriptions like "monster kind 16 slot 2 part 0", "stage 21 area part 2", "HUD/2D element
tex 36 256x256 at 313,267 trans_pit_1"). A pointer line goes to stderr and the debug log.

**Id buffer.** While `gfx_pick_pass` is set the GL backend draws every clay / 2D call flat in an id colour (no
blend / fog / dither; textures keep their alpha test; the game's z test and write stay, so an effect that does
not write depth does not hide what is behind it) and asks `gfx_pick_cb` (rt_pick.c `pick_register`) for the id,
passing the draw's render states (texture, blend src / dst / operation, z test / write / func, alpha function and
reference, filter, clamp, fog, fade colour, UV scroll matrix, world matrix, 2D box). The frame is the normal
draw code run again (the viewer's `goto redraw` after the frame; the game draws several frames per tick anyway,
so it is repeatable). Each draw site tags what it draws with `PICK(kind, a, b, c, d)` (rt_pick.h; one branch
when no pass runs): stage area / sky parts and set-model parts through the game's clay handles
(rt_bind_stage_model / rt_bind_set_model register them: kind, stage, part), game prims by their owner
(effect work index / type / arg, set object work, the draw function's symbol name), host-drawn models
(monsters: slot, kind, part, skeleton; hunters: player, slot legs / face / hair / body / arms / waist, part;
weapons; village NPCs), the HUD prims and the sprite list (the prim's trans function name), font glyphs (code,
palette), the movie, and the screen fade (not drawn in the pass). Depth is read too, so a click also yields the world
position of the hit point and, for skinned models, the nearest bone (distance to the skeleton's joints).

**Report folder** `reports/report_<date>_<time>/` next to the `logs` folder (`~/.local/share/mh1pc/reports`,
`%APPDATA%\mh1pc\reports`; never in the repository): `screenshot.png`, `annotated.png` (tints, outlines, click and box
marks with numbers), `idbuffer.png` (false colours), `report.json`, `log_tail.txt` (the last 300 log lines),
`clip.gif` (the last ~10 s, 320x240, ~6 fps, kept in a 14 MB ring that is allocated on the first capture and
costs one glReadPixels every 170 ms), `input.txt`. report.json: note, build, window, arguments, random seed, marks,
the picked objects (kind, description, ids a/b/c/d and clay handle, draw function, skeleton frame, world
position, render states, hit position, nearest bone, per-kind fields: motion id / frame / mode / step / HP of
the monster or hunter), and the game state (tick, mode, step, stage, quest, map areas, camera eye / target / fov,
the hunter's position / angle / motion / HP, every monster of the area with position, motion, mode, HP).
`python3 tools/show_report.py REPORT_DIR [--log] [--replay]` prints it readably.

**Replay.** The pad state of every game tick since the start (one `rt_pad_set` call per tick, run-length coded) is
saved as `input.txt` in the --input script format, extended with `xBITS:lx:ly:rx:ry*n` (analog sticks, hex fl pad
bits; the script reader now grows without the old 256-step limit, and `--input @file` reads a script from a
file). With the seed (`RT_SEED=n`, in the report) and the same `--boot` / `--quest` / `--stage` arguments a headless
`--input @input.txt --time <ticks/30>` run reproduces the session; `RT_PICK_AT=<tick>` freezes (and with
RT_PICK_CLICKS reports) at the reported tick, and test_pick.sh checks that this reaches the same game state.
Not recorded: typed text of the name entry (use RT_NAME), the mouse (free camera), the wall clock.

**Test aids.** `RT_PICK_AT=tick` (freeze at that tick), `RT_PICK_CLICKS="x,y;x0,y0,x1,y1;..."` (a click and a box in
window pixels), `RT_PICK_NOTE=text`, `RT_PICK_UI=1` (the same picks as real SDL events pushed one per frozen frame:
mouse down / up, text input, Enter; `RT_PICK_UI_SHOT=file.png` writes the frozen frame with the panel),
`RT_PICK_RING_MS=0` (clip frame every drawn frame). `tools/test_pick.sh` (~20 s, RUN / BIN for Wine) checks the
files and JSON, the event path, the replay and show_report.py. Checked 8 Oct 2026 on Linux and the Windows exe
under Wine.

`tools/bug_report.sh` / `bug_report.bat` pack the newest three reports with the logs. The screenshots show the
game's graphics (the player sends them on purpose; nothing is uploaded by the program). Compiled out on the Xbox
(`pick.h` has empty stand-ins under XBOX; rt_pick.c only holds the tags and the id table, and links there).
Not verified: a real mouse and keyboard session (the event path is driven by pushed events), the controller
combo (the pad_combo_held read is 5 lines; no pad press was made), high-DPI displays (mouse coordinates are taken as
framebuffer pixels), the clip's colours (3-3-2 palette, no dithering), reports of the movie / boot screens (the
boot screens are one tagged replay, so a click picks the whole picture).

## Importing a PS2 save (agent C, 8 Oct 2026)

Players can bring a real PS2 save to the PC and take it back. Formats and sources: docs/formats/saves.md.

- `mhview --import-save FILE`: FILE is a .psu, .max, .cbs, .sps, .xps or a raw memory card image
  (.ps2/.mcd/.mc2/.bin, with or without ECC); the format is found from the bytes, not the name. From a card
  image the save `BISLPM-65495MH` is picked (the error lists what the card holds if it is missing).
  The data is checked with the game's own test (version word, 16-bit sum) before anything changes. An
  existing save moves to `<card folder>.backups/BISLPM-65495MH-<date>-<time>` (outside the card folder, so
  the game never sees it); the new one is written to `<card folder>.import` first and swapped in, so a
  failure leaves the old save as it was.
- A save file dropped on the exe (or play.bat) is imported and the game starts; one dropped on the window
  while playing is imported with a message box (CONTINUE reads the card when chosen; quit first if you are
  already in the game, saving would replace it).
- `mhview --export-save FILE`: .psu, .max, .cbs, .sps/.xps, or .ps2/.mcd (a new standard 8 MB card with ECC,
  as PCSX2 writes it, holding only this save). Refuses a save that fails the game's check.
- No console / card binding: the save data has only its own checksum (`decode_data`); `check_sum_ck` compares
  a time stamp stored in the data itself. So no id needs to be faked and a PS2 save loads as is.
- Code: `src/pc/fmt/ps2save.c` + `lzari.c` (portable, no file calls: the Xbox build can use them once it has a
  glue like `src/pc/rt/rt_save.c`, which takes the card folder as an argument), `rt_save.c` (host folder,
  backup, CLI), hooks in `viewer.c` (`rt_save_cli`, dropped files).
- Tests: `tools/test_save.sh` (a few seconds, sanitizers, needs nothing; builds its own made-up save in every
  container, round trips, truncation / bit-flip survival, import / export with backups, refusals) and
  `tools/test_save_import.sh` (~1.5 min, needs disc/mh1: the game writes a save, it is exported to all six
  formats, imported, and CONTINUE must show 1550z each time). One-off cross-check with mymc+ is in
  saves.md. Not tested: a real PS2 save (none available), real CBS / SPS / XPS files from the tools
  themselves; the owner's save will tell. Known limits: no export into an existing card image, no .npo / .psv.

## Layout

| path | what |
|----|----|
| `src/pc/fmt/` | format readers, pure C, no graphics. Every reader takes a byte order (`FMT_LE` PS2, `FMT_BE` Wii MHG, which uses the same formats word-swapped). Files: `afs.c` AFS archive, `melt.c` Meltw + link files, `amo.c` models, `apx.c` textures, `ahi.c` skeletons, `aan.c` motion tables and curves, `hits.c` HITS collision (ground height). Formats are in docs/formats/. |
| `src/pc/gfx/gfx.h` | the graphics interface: textures, render states, clays |
| `src/pc/gfx/gfx_gl.c` | its OpenGL 1.x fixed-function implementation (SDL2 window) |
| `src/pc/fl/` | the port's "fl" layer: `fl_model` (AMO → clays, CPU skinning, VU1-style lighting), `fl_skel` (AHI + AAN motions → bone matrices), `flmat.h` (fl row-vector matrices) |
| `src/pc/audio/` | the audio interface: `audio.h`, `audio_mix.c` (portable mixer: 48 voices + 2 streams, 48 kHz stereo), `audio_sdl.c` (SDL2 device) |
| `src/pc/fmt/snd.c` | sound packs (SCEI HD/BD + TSBD, PS2 ADPCM) and ADX decoding (docs/formats/audio.md) |
| `src/pc/rt/` | the port runtime: what decompiled game C expects from the PS2 side (see below) |
| `src/pc/viewer.c` | the app: scene setup, hunter assembly (SetPartsTrans), camera, screenshot PNG writer |
| `tools/build_pc.sh` | build script; output in build/pc/ (gitignored) |

`src/pc/` is not part of the matching build and is not in c_files.txt.

## Port runtime (game C running natively)

The decompiled game C is compiled unchanged with the game's own include/
headers and linked into the viewer (list in tools/build_pc.sh, GAME=).
Running natively now:

| file | what it does |
|----|----|
| src/main/stage/stage_set.c | stage_set_set, the per-stage spawn list (matches the PS2 code). For st04 it spawns set00, Set13_set(0) and set14 |
| src/game/set/set00.c | light shafts: st04_1 clay 1, additive, scrolling, turned to the camera (rview_matY) at its two table positions |
| src/main/set/set13*.c (+ set13_nm.c) | sun glare: st04_1 clay 0 drawn towards sun_pos_tbl[4], faded out when the stage_sphr_tbl spheres hide the sun |
| src/game/set/set14_nm.c | UV-scrolled waterfalls (st04_1 clays 2 and 3 at 11060,0,1566) |
| src/game/set/set09.c | ambient creatures (butterflies etc.) on stages 5, 0x10, 0x21, 0x33... |
| src/game/set/set17.c | plant tiles on stages 1, 2, 3, 46 |
| set03/04/05_nm/07/08/10/11/15/16/18/19/20_nm/22.c, main set12.c | every other set object the spawn list can start (see each file's header) |
| src/main/stage/trans_stage.c | trans_stage: draws the area model and the set-model parts the stage places (see "Stage drawing") |
| all decompiled eft*/shell* (game and main), list EFT= in build_pc.sh | effects and shells: what set objects and stage_set_set spawn (Eft14_set2 camp fire on st21, Shell10_set barrels on stage 0x11, Shell22_set2, Eft17_set_ex, Eft13_set_pos ...) now run as the real C |
| src/main/hit/hit2.c, hit2c.c | sphere/capsule tests set13 uses |
| src/main/hit/shit*_nm.c, shit2.c, tri_nm.c, hitw_nm.c | the stage collision (f_sphr, agent D): load_stage_hit, GetGroundHit*, GetWaterHit, GetFloorSlide, HitWallPlayer -> GetWallHitBitPl/Em -> sphr_face_o3/o4 -> PushAdjust3, GetWallHitLine/GetEyeHitLine (see "Collision" below) |

The `_nm.c` files are near-matches on the PS2 side (logic believed
equivalent), so they run here too. For split files the whole-file `_nm.c`
is used when it holds every function; otherwise the matching pieces plus
the `_nm.c` (eft06, eft13, eft20, shell06, shell08). Files in WEAK=
(shell06_nm, eft20_nm) repeat some matching functions, so their symbols are
made weak (objcopy --weaken) and the matching copies win.

src/pc/rt/:
- `rt_game.c`: game_w, player_work, stage_work (timer counts up each tick),
  set_mdlw; the set object pool (pull/push_set_work, 64 entries of 0x80: a
  guess; pull_set_work(n) also gives n 512-byte heap blocks as sw->u.work,
  like 0x155290); prims and ordering tables ot0..ot4 (get_prim,
  release_prim, add_prim; drawn ot0 first, ot4 last, low priority first
  inside a table: a guess); ran_suu (same generator as 0x161230);
  rt_game_init (calls stage_set_set) / move / draw. Static asserts check
  the struct layouts.
- `rt_fl.c`: flSetRenderState (0x0D blend op, 0x19 texture matrix, 0x1A
  world, 0x5E blend factors, 0x60 alpha ref, 0x63 filter, 0x64 texture
  clamp, 0x67 fade, 0x6C z-write; 0x6D accepted and ignored; others print
  a one-time warning), flExecuteClay (handle -> gfx clay), and the clay
  attribute path, ported from the asm: clay_attr_set (0x121E20),
  clay_attr_reset, SetTrnslMode, SetOpeMode, SetFilterMode. Their tables
  (src_mode, dst_mode, ope_mode, filter_mode, aa_alpha_src, aa_alpha_ope,
  aa_filt, aa_addr) are read from the ELF. rt_clay_attr_word() packs the
  CLAY+0x88 word from an AMO part's 0xF0000 chunk like Attribute_from_amo,
  so set-model clays and the host's own draws use the game's blend modes.
- `rt_flmat.c`: fl matrix/vector/maths helpers (flmatRot*33, Mul33_2,
  ScaleFactor33, SetXYZ33, flvec*, flSqrt, flArcCos...) written from the
  VU0 asm, and rview_mat / rview_matY (rt_set_camera, as View_move builds
  them: rview_mat = camera world matrix, rview_matY = Ry(camera yaw + 90°)).
- `rt_main.c`: small main-program functions not decompiled yet, written
  natively from the asm: clr_flash, hit_cap_pk, Pl_stg_ck/Em_stg_ck,
  frame_check2, flvecApplyMat33_2. Stubs: hit_point_cyl, Create_FOV /
  flCheckMeshFOV (everything counts as visible; the GPU clips),
  reload_tex (textures stay resident), camera quake, monster sound.
- `rt_eft.c`: effects and shells. The effect list (eft_work, 128 x 0x40,
  free stack + linked list from eft_w_top: pull_eft_work/2, push_eft_work,
  move_eft, trans_eft/trans_eft_up), the shell list (64 x 0xD4,
  pull/push_shell_work, move_shell with the +0x7B hit-stop counter,
  trans_shell), the second prim pool (get_prim2), the senko/smoke/smell
  stacks (kept, not drawn), the effect models eft_mdlw[0..4] (ef_00,
  kage04-06, ef_01 from main's effect_model_data/EFT_TEX tables, loaded by
  the viewer) and the helpers the eft C calls: eft_vec/alpha/rgba_linear,
  make_mat_srt, eft_trans_sub(_col/_opa), Eft_rendope_set, shell_rate_add,
  vectors, GetGroundHit (host collision callback; GetWaterHit says "no
  water"). Joint queries (get_joint_pos/wmat) return the actor's position:
  no skeletons run as game C yet. Player/monster-only helpers (sound,
  vibration, attack data, skinned-model drawing flCalcTrans/flSetSkinTrans,
  shell08_trans) are stubs; RT_TRACE lists them.
  `RT_SPAWN="eft17:4,eft14:3,..."` spawns test effects at the hunter.
- `rt_overlay.c`: main C calls overlay functions by address
  (func_6229B0 = set14_set, func_54B8C0 = Eft14_set2, ...). Each is routed
  to the ported function in the definition's argument order.
- `rt_data.c` + `tables.txt`: Capcom data tables are declared empty and
  filled at start-up from the user's SLPM_654.95 / game.bin by address
  (nothing copied into the repo). Most are listed by name in
  src/pc/rt/tables.txt; tools/gen_rt_tables.py looks up their address and
  size in config/symbols/ and writes build/pc/rt_tables.c (run by
  build_pc.sh). `NAME work` lines are zeroed work areas (em_work,
  quest_w). Tables in .bss (past the file data of the ELF or overlay)
  start as zeros. **Pointers inside tables:** the ELF keeps its link
  relocations (.relmain, .relgame.bin); every R_MIPS_32 entry is a data
  word holding an address. After the copy, each such word in a host table
  is turned into a host pointer: into the host copy of a table if it points
  into one, else to the host symbol of that name (dlsym; the viewer is
  linked -rdynamic; the ELF's .symtab names the target), else to the same
  bytes in the loaded image. The images themselves are relocated the same
  way, so pointer chains (eft*_data keyframe lists, fade tables) work.
  Pointers to code that is not ported become NULL. `rt_ptr_at(va)` reads a
  relocated pointer (the viewer's ptmat_tbl). Unnamed data (D_3F2090 =
  rview_mat row 3, ...) is defined with --defsym in build_pc.sh.
- `rt_mem.c`: PS2 address lookup in the ELF and the overlay; relocations
  and symbol lookup for the above.

### Motion system (frame_init / frame_move)

The hunter is animated by the game's own motion code: src/main/frame/
f_frame_nm.c (main 0x125340-0x1267BC; 16 of its 18 functions match the PS2
bytes, see docs/agents/agent-A.md) runs unchanged. create_plcom_motion
turns every AAN in plcom_tbl.bin into a motion-set handle
(motion_set_handle_tbl, com_mot_han_ofs); frame_init picks the handle from
PLW.char0/char1, frame_move steps the layers, cross-fades, loops, and moves
the player by the root motion (pl_velocity_sub). Below it, src/pc/rt/
rt_motion.c implements the fl motion layer natively (read from the asm,
main 0x173A50-0x1746A0): a motion-set handle is an index into a host table
of parsed AANs; the two motion players at model work +0x44 / +0x54 are
RT_MPLAYs that record per group the set, the frame and the blend, with the
+0xD0 word flCalcTransVelocity is given pointing back at the player.
flCalcTransVelocity returns how far AAN bone 1 of group 0 (the root's
child node on the PS2) moves between two frames: the run loop plcom 3 moves
it 537 units forward over 78 frames. The host poses the hunter's skeleton
from the player with rt_motion_pose (fl_skel_pose_groups: per-group frame,
channel blend), keeping that bone's X/Z translation at its bind value
because the game moves the actor instead (`root_lock`; how the PS2 cancels
it at draw time is not traced yet [guess]).

The Rathian runs the same way: create_em_motion builds em01_tbl.bin's
handles (Em_max_parts_get, ported in rt_main.c from main 0x10B770: 3 part
groups for kind 1), em_work[0] (not in use, be_flag 0) holds its motion
layers, ids 1003/1203/1403 (slot 3 of banks 0/2/4). Verified: with
`RT_HOST_MOTION=1` (the viewer's old AAN player for both actors) the shot
build/show/A/em_host_view.png matches em_game_view.png except for a
one-frame phase difference.

`RT_MOTION_SCAN=1` lists every common motion with its length, loop and root
travel (how plcom 3 was found).

### Player and pad

- rt_pad.c is the PS2 pad driver step (ioRead_sub, main 0x11FAC0): it turns
  the host pad (fl pad bits + sticks, src/pc/pad/pad.h; SDL backend
  src/pc/pad/pad_sdl.c) into Psw[0] (buttons, triggers, stick direction
  bits, stick angle and power with the 45 dead zone, repeat), then runs the
  decompiled swset() (pad_get.c). pl_sw_set / sw_set_sub (pl_normal2.c)
  then fill player_work[0].sw as on the PS2. fl bit meanings were read
  from ps2pad_hard_to_soft_ds2 (0x306500) [inferred; the mapping to the
  game's bits is ioRead_sub's and is exact]. Stick angle: 0 = right,
  0x4000 = up.
- rt_player.c runs the game's player code (see "Player"); with
  RT_PL_STANDIN=1 the old host stand-in (turn and run only) is used.
- Verified 5 Oct 2026: `--input "idle*10,up*50,left*15" --sw-trace --time
  2.5` prints sw.ang 0x4000 / pow 127 for "up" and 0x8000 for "left", the
  hunter turns to the camera's forward direction and runs about 300 units
  (build/show/A/play_run.png shows it mid-stride, turned left).

### Player (game C)

Since 6 Oct 2026 the hunter is run by the decompiled player code (agent
F's src/main/pl, see docs/agents/agent-F.md); rt_player.c only sets it
up and calls it:
- Set-up (rt_player_game_init) does init_pl_work's offline-master part
  (main 0x1116E0): equipment type/id at PLW+0x35F/+0x360, +0x34C =
  Ken_data[id][0], job PLW+2 = Battle_type[+0x34C] (0 great sword, 1/5
  bowguns, 2 hammer, 3 lance, 4 sword and shield, from menu_stat_job_str),
  User_data +0x3CD/+0x3CE for Get_equip_value; then the game's pl_init(0):
  start position from stage_start_pos, idle motion. The weapon class's
  motion table w<job>_tbl.bin goes through create_pl_motion (ids >= 1000).
- Each tick: rt_pad_tick, then pl_move (pl48.c: pl_sw_set, pl_move_sub
  for 8 players, hit_timer_calc_shl). pl_move_sub (pl_nm.c near-match)
  runs timers, Pl_damage_sub (game.bin pl_damage), the action state
  machines (pl_normal / pl_attack / pl_damage ... through their jump
  tables), pl_turn_sub, the motion step pl_chr_sub (frame_init /
  frame_move at speed 2: the 30 Hz tick plays 60 fps motion data), the
  per-motion hook pl01_effect_move -> ef_move_sub (src/main/sound/
  f_sound_nm.c: footsteps, swing and voice sounds, dust), wall, floor
  and ground collision, World_calc.
- Then (viewer, per tick) sync_joints poses the host skeletons and gives
  the joint world matrices to the game C (part blocks PLW+0x110.. for
  parts_init's 32 slots = skeleton nodes, get_joint_pos, hit_data_expand),
  and rt_hit_check runs hit_check (hit_nm.c), the order of game_core
  (move, trans, hit_check).
- src/pc/rt/rt_pl.c: helpers not decompiled yet, written from the asm with
  their addresses (Pl_act_set and friends, flags, motion requests,
  stamina/vital/sharpness, rates, front_land_ck, item counts, attack data,
  Code_Make ...); network, items picked from the stage, quest and message
  functions are stubs. Get_Active_itemnum reads its player from a0 on the
  PS2 (its caller passes nothing): the host uses the master player.
- Weapon model: weapon_model_data / WEAPON_TEX[PLW+0x34C] (AFS entries,
  weNNN_amh / _tex), posed like weapon_trans (weapon3_nm.c): hand part
  0x12 / 0xE or sheathed part 9 (sword and shield) / 10 with the
  weapon_disp_tbl_r/l/b[job] offset and XYZ rotation; the shield bones
  (AHI group 1) on joint 0x11. Per-motion node scaling of great sword,
  lance, hammer and bowguns (weapon_dat_make) is not done.
- Hits: shell00 (game.bin) is the sword's attack shell; its body volumes
  are expanded on the player's joints, the Rathian's from em_body_tbl on
  hers. A hit fills the monster's damage fields, plays the hit sounds,
  starts the 2-tick hit stop (PLW+0x610: motion speed 0.2) and the hit
  marks (eft16, eft05 slash trail via the skinned ef_01 model). The
  Rathian's HP comes from em01_init and she reacts (see "Monster").
- x86 hazards: several matched files declare a callee with the float
  argument in another position than the definition (fine on the PS2,
  where floats use their own registers). build_pc.sh compiles those files
  with -DNAME=rtabi_NAME and src/pc/rt/rt_abi.c re-orders (frame_check,
  frame_check2/3, Eft06_set, Eft02_set6 from plf.h; hit_point_cbd in
  f_stage.c; pl_move_sub's four-argument GetGroundHitStatusAreaPl). They
  were found with an LTO build: copy tools/build_pc.sh, add -flto to
  CFLAGS/GAMEFLAGS and read the -Wlto-type-mismatch warnings whose
  declarations differ in where the f32 arguments are.
- Verified 6 Oct 2026 (scripted --input, RT_PL_TRACE, shots in
  build/show/A/pl/): run 0/1 with the run loop moving ~11 units a tick;
  roll 0/0x1C (cross); draw 0/4 (right stick) -> attack 1/0x30 -> combo
  1/0x37 (motions 1401/1402 of w04_tbl); weapon-out walk 0/3 (1004);
  guard 2/3 (R1); sword in hand and shield on the arm (ws_sheet.png),
  great sword overhead swing (gs_2.6.png); stages 1, 5, 0x10, 0x21
  (stages_play.png); sound trace: weapon draw (snd_weapon07 code 1), swing
  (code 4) with voice (snd_vo_m00 0x24), footsteps on the ground
  material; hit on the Rathian (hit_sheet2.png, RT_HIT_DM output).
  Nobody has compared any of it with the PS2 side by side.

### Monster (game C, enemy_mv)

The Rathian runs the game's own monster code since 6 Oct 2026: each tick
rt_monster_tick calls enemy_mv (src/main/em/f_em_nm.c, written from the
asm) -> em_move (sight, smell, hate, anger, status upkeep from em_core /
em_master / em_taisei) -> em01_main (agent B's em01_ai_nm.c) and the
command interpreter (agent D's em_cmd_nm.c, still on branch agent-D:
build_pc.sh exports it with that branch's headers to build/pc/ext) ->
frame_move -> HitWallPlayer / GetGroundHitStatusAreaEm. Her per-animation
sound/effect script is the game's (em_prog_tbl[1][3] = em01_effect_move).
- Set-up (rt_em.c): `--quest N` reads the mission file into a host
  mission_area and points quest_w.x64/x74/x78/x80/x94/x14E at its tables
  as Quest_init does; the quest's own monsters are the QEM list at
  Em_data_com_adrs_get(x78, 1) (0x3C bytes each: kind, variant, stage,
  hunger/thirst/sleep, angle, position). rt_monster_spawn does what
  Em_direct_set does (free em_work, fields from the QEM, enemy_mv step 0 =
  em_init; em01_init sets the hit points: 2500 for quest 10). Without
  `--quest` (free hunt, quest_w.no 0) a stand-in QEM at the viewer's spot is
  used; em01 then starts with a fly-in.
- Quests with the Rathian (kind 1) as their own monster: 10 (stage 40),
  12 (52), 21/24/27 (9), 44/45 (19), 60 (40); kind 11 (em01 code too) in
  6-9, 46 and 56-59. Read from the mission files with a throw-away script; stage
  numbers are QEM+7.
- x86 fixes needed on the way (none touch PS2-built code): game_w.pl_num
  = 1 (sight/hate loop over it); Em_Master_Change and NextStage_No_Set get
  em (a0 left over in the asm); GetGroundHitStatusAreaEm's fifth argument
  em+0x7E4 (t0, set in the delay slot); argument-order adaptors in
  rt_abi.c (em_frame_check, Eft13_set_em_scl, Eft15_set3, Eft02_set3);
  em_sleep_eff_set on the PC (rt_em.c, the PS2 one leaves the scale in
  f12); `-fno-aggressive-loop-optimizations` (Em_Dmg_Sys reads
  EMW.hagi[8] with i == 8 and gcc dropped the loop exit); absolute
  game_w/quest_w addresses in m2c-based files rewritten at build time
  (rt_ps2abs.h); weak-NULL data tables (eft20, fade_type25/26, shell06)
  added to tables.txt.
- make_mat_srt (host, rt_eft.c) had the rotation flags swapped (asm: 2 = Z,
  8 = X): eft16's blood streak (flag 2, only rot[2] set) was turned by an
  uninitialised X angle into the screen-wide red smear of the earlier hit
  shots. 71 effect call sites use flag 2. eft16_nm.c itself checks OK
  against the asm except eft16_m's spill order (its float immediates match).
- Verified 6 Oct 2026 (scripted --input, RT_EM_TRACE / RT_PL_TRACE /
  RT_HIT_DM, shots in build/show/A/em/): quest 10, stage 40: she turns
  and walks (1/3, 1/0) while calm; when the hunter comes close she roars
  (1/7; the hunter covers his ears, 2/25), mode 1, then charges and bites
  (3/4, 3/18, 3/6); a hit takes 49 of the hunter's 100 (knock-down 2/2);
  his sword hits take 3 per slash off her 2500; with RT_DMG_MUL=40 a 120
  hit on part 6 (durability 100) makes her flinch (4/2, motion 1063).
  2100-tick runs on stage 40 and stage 4 without crashes. Nobody has
  compared any of it with the PS2 side by side.
- Not done: carving points (Em_hagi_point_set returns -1), quest clear /
  monster death handling (Quest_enemy_die prints), map marker
  (WyvernAreaMove), event flags (no save data), Quest_restart after the
  hunter dies (stub), other quests' small monsters (QEM lists per stage at
  x74) and stage changes.

### Quest loop (game modes, f_quest, HUD, reward)

With `--quest N` the PC runs the game's own quest flow (6 Oct 2026, agent A):
- Start: rt_quest_load does Quest_init + Quest_start (select_w+0xAC = N)
  as game11 does: mission file questName[N] into mission_area, quest_w
  tables, stage, time limit, monster states (quest_em_init). The quest's
  monsters come from station_em_set / Quest_next_em_set -> Em_direct_set
  (src/main/quest/f_quest_nm.c; f_quest0_nm.c holds main
  0x2267F0-0x226C24, written from the asm). Kinds whose program
  (em_prog_tbl) is not ported get no model slot and are not spawned.
- Game modes: rt_flow.c runs game_w.mode each tick through the matched
  f_game.c / f_gameb.c: game2 (quest: game_core, Info_control,
  Quest_condition_judging, Game_clear_ck, stage change steps 2-6), game3
  (the "quest clear" wait), game5 (result_prog). game_core is the viewer's
  host tick (sim_tick: pad, player, camera, hit_check, monsters, HUD).
  Outside game2 the pad is still read every tick.
- Clear: Quest_enemy_die -> quest_condition_prog -> x00 1 (150-tick wait,
  then 60 s to carve, info banner) -> x00 2 -> game_w+0xD5 = 4 ->
  Game_clear_ck(2) -> game3 -> game5 -> result_prog -> remuneration ->
  reward screen (reward_mv/disp_reward, f_reward*): items picked with the
  pad go into the pouch.
- Carving: the hunter's own carve action (0/0x4A, circle at the carcass)
  -> Ext_pick_point_ck2 (Em_hagi_point_set made the point at death) ->
  ItemStockRequest (menu_nm.c) -> Pl_item_stack.
- Hunter faints: Pl_die_set -> death action 3/0 -> pl+0x738 -> game2
  steps 2-6: Quest_next_em_clr, st_model_load (the viewer's
  load_stage_models: area/set models, collision, camera file, sound),
  stage_set_set, Quest_next_em_set (the cart, em18), pl_init(1): the hunter
  is back at the base camp (stage 21 for quest 10).
- HUD ("pit", main f_menu): load_pit (textures), PitWork_init / Pit_init,
  Pit_mv each tick; trans_pit_0/1/2 draw the clock, vital/stamina bars,
  sharpness, map, item bar and its text through the game's own
  menu_disp_nm.c. The info banner is set01.c.
- 2D (src/pc/rt/rt_2d.c): flps0002/4/5/8/9/C screen prims read from the
  asm (layouts in the file header), PS2 frame 512 x 448 stretched over the
  window; textures by handle in mem_tex[] (flCreateTextureFromApx_mem on
  host APX decoding); gfx_draw_2d in the gfx interface. Screen layers in
  trans()'s order: ot5, font 0, ot6, font 1, ot7, font 2, ot8, font 4,
  ot2, font 3 (rt_game_draw_2d).
- Fonts (src/pc/rt/rt_font.c): AFS_DATA 0x6D2, 7808 glyphs of 20 x 20 at
  2 bits (MSB first) in JIS order; flfntPrintf's five stacks by z,
  FontPuts advance rules, Ascii2Sjis (the font's own half-width row
  0x85), palettes from flfntSetPalData; font_print / _ex / _sp (~C / ~A
  codes) / _double / _uf from the asm.
- Test aids (scripted runs only): `RT_QUEST_TRACE=1` (mode/step/clear
  state changes, the pouch when it changes, the reward list),
  `RT_FONT_TRACE=1` (each text drawn), `RT_TEX_TRACE=1`,
  `RT_TEX_DUMP=dir` (raw RGBA of every 2D texture), `RT_EM_HP=n` (monster
  0's hit points), `RT_PL_WARP_EM=tick` (put the hunter at monster 0 at
  that tick), `RT_PL_ITEMS="id:n,..."` (pouch; no save data),
  `RT_PL_GOD` also clears the stun gauge.
- Verified 6 Oct 2026 (quest 10, scripted --input, traces + shots in
  build/show/A/quest/): HUD with items (hud_items.png); kill with
  RT_EM_HP=30 + RT_DMG_MUL=40, carve three times -> Rathian Scale, Spike,
  Flame Sac into the pouch; clear banner (clear_banner.png); 60 s later
  game3 -> game5; reward menu (reward_menu.png) and item grid
  (reward_items.png, icons checked against the decoded icon sheet),
  circle takes a reward into the pouch; potion use 10 -> 9 (drink motion
  406); death -> carted to the base camp, stage 21 loaded
  (carted_to_camp.png); after the reward the money screen (result_prog
  steps 2-3: fee, reward, total, money counted into User_data,
  gold_result.png), then game mode 6, where the host starts the quest
  again (back_to_quest.png; the PS2 goes back to the village, which is not
  ported). Free play (`--play` without `--quest`) runs Quest_init's
  free-hunt tables and shows the HUD too (free_play_hud.png). Nobody
  compared any of it with the PS2 side by side.
- Not done: SpritePut and the sprite prims flps0D00/0F00/1300/1400/1600
  (game3's darkening quad; game3's text sits on the field picture), the
  cart's model, map markers (flvecrRotTransPers), item combining
  (Item_preparation*).

### Village, pause menu, small monsters, quest failure (agent A, 6 Oct 2026)
- Village (game mode 6, offline): rt_village.c does what Game_task does
  (all_reset: monsters and set objects cleared; Clear_lobby_ram) and then
  runs lobby.bin's Local_main every tick (src/lobby/f, src/lobby/lb and
  src/lobby/f/lb_village_nm.c). Local_main returns 1 when a quest was
  accepted and the hunter walked out through the gate; the host then
  starts that quest (select_w+0xAC) as `--quest` does. Kokoto = stage 87,
  the hunter's house = 86; the Village Elder (talk kind 71, npc01) is the
  quest counter; the gate is unique spot kind 6 at (10650, 15225), left
  with square. lobby.bin shares its vram with game.bin: its data and bss
  live in rt_lb_mem (one host block), its symbols are aliases into it
  (tools/gen_rt_auto.py), absolute addresses in C go through
  tools/pc_abs.py. Village motions: com_motion_load(1) (lbcom_tbl); NPC
  models npc00/npc01/em09/em32 (npc_create_model).
- Pause menu (start in the field): menu_nm.c / menu_disp_nm.c with
  ListSelect / PageSelect / Menu_select_mv (listsel_nm.c) and
  DispFrameMessageA (dispframe_nm.c): item list, discard, quest info,
  retire (D5 7 -> game3 -> game5 -> village).
- Small monsters: every em_work slot is ticked (rt_monster_tick) and
  drawn with its own model, texture and motion table (em%02d files by
  kind, loaded from em_create_model). Velociprey (em16) AI is built; its
  game.bin tables are in src/pc/rt/tables.txt.
- Quest failure: the third faint sets D5 5 -> game3 -> game5 shows the
  "quest failed" score -> circle -> village.
- Verified 6 Oct 2026 (scripted runs, shots in build/show/A/): quest 131
  accepted from the Elder and started at the base camp; pause menu pages
  and discard; quest 10 stage 40 Velocipreys attack (small/v_6.png);
  `RT_PL_DIE=60,1400,2700 --quest 10 --input "idle*6600,circle*3,idle*2000"
  --time 250` -> carted twice, failure score (faint/f_200.png), village
  (faint/village.png). Not compared with the PS2.
- Test aids: `RT_VILLAGE_START=1` (start in the village), 
  `RT_VILLAGE_SKIP_INTRO=1` (first-visit event marked seen),
  `RT_VILLAGE_TRACE=1` (NPCs, spots, talk states, camera), `RT_NO_VILLAGE=1`
  (mode 6 restarts the quest as before), `RT_CAM_DEBUG=1`,
  `RT_PL_DIE="t1,t2,..."` (the hunter faints at those player ticks).

### Windowed = headless, village menu, sprites, area exits (agent A, 7 Oct 2026)
- Scripted runs are tick-for-tick the same windowed and with `--shot`:
  the host syncs joint matrices after every tick (not only per drawn
  frame). Check with `RT_TICK_TRACE=1` (per tick: flow mode, stage,
  hunter position/angle, a sum of monster positions) and diff the two
  runs' "T" lines. Checked on: the village accept script, village/quest
  random roams (5 seeds, 2-4 minutes each), village -> quest 131 -> camp
  -> area 1.
- Fixed crashes: windowed segfault at the village -> quest switch
  (rt_game_init now empties the prim queues: a frame drew eft13 prims of
  cleared effects); start in the village (lbmw NULL); eft06_m (the _nm C
  tested the stepped pointer instead of the table, asm s8 vs s6);
  monsters whose program entry [3] is not ported (kind 29 in area 1)
  are no longer spawned.
- Village start menu: Pit_init's lobby branch -> Lb_Menu_Init; Pit_mv_lb
  -> Lb_menu_move_Core, trans_pit_1_lb -> DispLobbyMenu / Disp_lb_menu
  (src/lobby/b/lb_menu_nm.c, from the asm). main's func_5B3D70.. forward
  to the lobby C (rt_menu.c). Quest status, items (discard), combine
  list, data, status and equipment screens checked on screenshots.
  pit_help_str_tbl[4]/[5] point into lobby.bin (mapped in rt_data.c).
  ItemCopy_Pl2Ud / Ud2Pl as udmisc02.c; without save data the user's
  pouch starts as the hunter's (rt_player_game_init).
- Sprites: SpritePut (src/main/sprite/spriteput_nm.c, from the asm) +
  trans_sprite before ot5 + flps0D00: game3's darkening quad fades the
  screen (brightness 46 -> 31 -> 20 over the fade). flps0F00/1300/1400/
  1600 (textured / 3D sprites) are still stubs.
- Area exits: the host calls stage_mv_ck every tick (move_stage's exit
  check; the rest of stage_m, stage sounds and item sparkles, is not run)
  -> pl+0x738 -> game2 loads the next area (camp 21 -> area 1 = 39).
- flSetRenderState 0x0F-0x11 fog values (inert: nothing sets 0x12),
  0x5F Z test (7 = off [guess]); 0x01/0x0E/0x15 known no-ops.
- The cart (em18) is drawn with its Felynes when the hunter is carted
  (cart13.png); nothing more was needed.
- Not checked: comparison with the PS2; textured sprite kinds; other
  areas' exits beyond camp -> area 1; save data.

### Quest start, supply box, playability pass (agent A, 7 Oct 2026)
- `--quest N` starts like the game: Quest_start leaves game_w.stage at the
  quest's start stage (base camp, 21 for quest 10) and the hunter starts
  there (pl_init's start position); the quest's monsters wait on their own
  stages (the Rathian on 40). `RT_QUEST_STAGE=1` keeps the old start on the
  monster's stage for scripted fights. A quest restarted after the reward
  (RT_NO_VILLAGE) also starts at the camp.
- Supply box: rt_quest_load runs Start_item_init after Quest_start (as
  game11 does): the mission file's start items go into game_w+0x128 (32
  slots of {item, count}). At the camp the box is unique spot kind 3
  (stage 21: 9500,40,9500 r 200); circle there -> pl_mv087 ->
  Pl_box_select (menu_nm.c) -> box_get -> Pl_item_stack. Checked with
  RT_PL_WARP=20,9598,9639: box screen with the quest's 22 items, all taken
  into the pouch (stack limits apply: 3 of 4 whetstone stacks fit), item
  bar shows them. Walking there works too (closest reachable point ~170
  from the spot centre, on its +x/+z corner: the crates in front of the box
  keep the hunter ~250 away on the other sides; not compared with the PS2).
- New character: the game's own defaults are select.bin's user_data_copy
  (src/select/edit00.c): User_data cleared, sword and shield 0x9C, money 0,
  pouch empty. So a fresh hunter's pouch is empty on the PS2 too; the items
  come from the supply box. The PC does not run user_data_copy (only the
  weapon matches it); `RT_PL_ITEMS` stays a test aid.
- Monster sounds: rt_snd_stage loads the snd_emNN packs of game_w+0x28's
  kinds (game12's snd_joint_load list) and em_create_model adds a new
  kind's pack (rt_snd_em_add): Velocipreys were silent before.
- Game C is compiled with `-ftrivial-auto-var-init=zero`: matching C can
  read a local the original never wrote on that path (the PS2 then reads a
  stale stack slot). pl_dm001 (guard knock-back, pl33.c) adds sp30[2] to the
  hunter's position after frame 94 without setting it: on the PC the hunter
  was thrown to z = 1e21 and the screen went blank seconds into a guarded
  Rathian attack. gcc's -Wmaybe-uninitialized does not see this case (the
  array goes to flvecApplyMat33 by pointer).
- Test aids: `RT_SPOT_TRACE=1` (each stage's unique spots at stage set-up),
  `RT_PL_WARP=tick,x,z` (move the hunter at that tick); RT_EM_TRACE also
  prints layer 0's motion state (stat/end/blend ticks).
- Checked (scripted, headless and windowed): five 200 s random-input fights
  on stage 40 (no crash, positions sane), a 60 s windowed run at ~60 fps
  (x86), guard blocks (2/3 -> 2/9 / 2/10 with chip damage), SnS chain
  (draw 0/4 -> 1/48 -> 1/55 -> 1/56), Rathian charge (atk 18), single and
  triple fireball (atk 4 / 23, explosions drawn), village -> quest 131
  start with its supply box. Not compared with the PS2.

### Collision (stage HITS, game C)

The game's own collision C (agent D's f_sphr near-matches, list HIT= in
build_pc.sh) runs on the PC; the host reader fmt_hits_ground_y is no
longer used by the viewer.
- Loading: rt_load_stage_hit(stage) runs the game's load_stage_hit
  (shit1_nm.c): load_file_mdl (rt_hit.c) asks the host for the AFS entry
  of stage_hit_data_w / _f[stage] (Meltw-decompressed) and copies it into
  a 4 MB host area (stage_hit_area_w / _f); WallHitInit / GroundHitInit
  then turn the file offsets into pointers (fine in the 32-bit build).
- rt_hit.c also has the small main helpers that are not decompiled,
  written from the asm: NormalClipF3 / NormalClipCheckF3 /
  PointHitCheckF3 (2D point-in-triangle with the original's quirks: one-ulp
  products count as equal, the orientation test truncates to int),
  UnitNormalVectorCCW, NvecFloatAdjust, cpRotMatrixYXZ2, flConvertRtoS,
  Stage_data_get (quest_w+0x80 = St_data, as the default quest setup at
  0x226BD0 sets it; stage_work+0x48 = Stage_data_get(stage) as stage_w_init).
- Player (rt_player.c): pl_move_sub's order (main 0x14C500): old position
  to +0x5A0, move, HitWallPlayer(pl, 0) (one sphere push00: y 60, r 48),
  GetFloorSlide(pl, v, 1), GetGroundHitStatusAreaPl -> +0x5AC; y snaps to
  it when below or less than 30 above, else a host fall (the PS2 starts
  the fall action Pl_act_set(pl, 0, 9)).
- Monster (rt_hit.c rt_monster_collide, from em_move 0x10BF30): old
  position, frame_move (root motion), HitWallPlayer (spheres
  em_hit_push_tbl[kind]: the Rathian, kind 1, has one sphere of radius 500
  at y 160), GetGroundHitStatusAreaEm, y = ground. rt_monster_place puts
  em_work[0] on the stage; the viewer draws the Rathian where the game has
  it (RT_EM_FIXED=1 keeps the old fixed placement).
- Fixes found on the way: PointToPoint is d = a - b (the host had b - a,
  which also affected effect code that uses it); table pointers into PS2
  .bss (wall_tbl_add -> stNN_wall_tbl) now point at zeroed host memory
  (rt_bss_shadow) instead of NULL.
- Verified 5 Oct 2026 (scripted --input, `--sw-trace`, shots in
  build/show/A/hit/): st04 "right" from the start: the hunter runs into the
  invisible wall at the cliff edge (polygon 10919,7513 - 11252,7689) and
  slides along it 48 units (the sphere radius) away instead of dropping to
  y -487 as before (wall_right_top.png); "left": walks up the stone path,
  y 7 -> 306, feet on the ground (st04_slope.png); stage 1 "up": stops at
  the river bank (z 8651, st01_wall.png); stage 5 / 0x21 runs stop or slide
  at walls. Rathian: on stage 0x21 it walks its 1003 loop along its facing
  (34 degrees, matches the angle) on the ground (em_st21_walk.png); on st04
  its 500-radius sphere is pushed out of the camp walls and it stops at the
  cliff wall; on stage 16 it stops at a wall after ~250 units.
- `RT_HIT_TRACE=1` prints the wall polygons of the start cell and, per
  tick, the player's wall sweep (old/new/pushed position, contacts).
  `--follow D,H,P` sets the play camera for such shots.
- Not done: water (GetWaterHit runs but nothing reacts), the fall action,
  the player's pl_wall_mat use (wall-facing actions), monster states 2/4.

### Camera (game C)

With `--play` the view comes from the game's own camera: CameraMove
(src/main/cam/camm.c) and the five camera slots (cam_nm.c / camd.c,
agent D; cam_sub_std, cam_sub_stg are near-matches) run every tick after
the player; cam2view writes eye / target / roll / fov into lpView, and the
viewer builds its look-at camera from that (roll ignored; the game's angle
of view is used as the vertical fov [guess]). `--follow D,H,P` or
`RT_HOST_CAM=1` keep the old host follow camera.
- Stage camera files: LoadCameraData (rt_cam.c, from 0x11F1E0) loads
  camera_data_tbl[stage] (26 stages have one, st04 included) into
  cam_data_area and SetCameraData fixes its pointers; without a file
  default_area_data builds one follow area from stage_camera_data_tbl.
- Camera areas: src/main/cam/camarea_nm.c (main 0x222E20-0x223B50, the
  g_SetAreaData file) written from the asm for this: default_area_data,
  StageCamInit, SetAreaData, Get_cam_grid_XZ, CameraAreaCheck,
  CamAreaAttribChk, Area_XZ_Check, GetPanTarget, GetRailTarget,
  GetRailCamPos, GetNearSection, get_near_point_sub, GetNearPoint. Not
  built for the PS2; check.py: CameraAreaCheck, GetPanTarget,
  GetRailTarget and nlCalcPoint already match, GetRailCamPos 1/33,
  default_area_data 7/117, SetAreaData 12/71 off, the rest further.
- Controls as on the PS2 (read from cam_sub_std): d-pad left/right turn
  the camera, d-pad up/down zoom (4 levels), L1 puts it behind the hunter.
  When the eye-to-target line crosses a wall (GetWallHitLine) the camera
  goes back behind the hunter every tick, so it cannot be turned there
  (the st04 start spot has a wall right behind the camera).
- Verified 5 Oct 2026 (build/show/A/cam/): st04 idle (gc_idle.png, behind
  the hunter, waterfalls ahead); run left, camera follows round the camp
  (gc_run_left.png); after moving off the wall, d-pad left 40 ticks turns
  the camera to the hunter's front (gc_turn_dleft.png); d-pad up zooms in
  (gc_zoom_dup.png); stage 20 (indoor, fov 1.15 from its camera file) and
  stage 5 (jungle) follow without clipping into walls (gc_st20.png,
  gc_st05.png). `RT_CAM_TRACE=1` prints per tick the slot, area, zoom,
  buttons, wanted/current yaw, wall flag and the view.
- Not ported: k_HitEmCamera finds no monster body parts (hit_data_expand
  stub), Game_clear_ck (quest end camera), cockpit chat. Rail / fixed
  stage cameras (area types 1-3) run the near-match cam_sub_stg but were
  not seen in the tested spots.
- x86 hazard found: a callee returning float that a caller declares void
  (k_HitWallCamera in cam_nm.c) leaves a value on the x87 stack; after
  eight calls the FPU stack overflows. Such declarations must match.

### Sound

docs/formats/audio.md has the formats and the game's sound calls;
`tools/snd_dump.py` decodes packs and ADX to .wav (build/audio/).
- src/pc/rt/rt_snd.c: se_req / se_req2 / Pl_se_req2 / Em_se_req2 /
  Pl_se_req2_com and flSndRequest / flSndChange written from the asm
  (distance volume curves, screen pan, random volume/pitch, chained
  codes); host code then does the IOP driver's part (TSBD program + id,
  note -> split -> sample -> VAG, decoded once, played on a mixer voice).
  str_* (ADX streams from AFS00 into mixer streams, fades and str_volume's
  dB table) are host versions of main 0x100910-0x100E18.
- rt_snd_stage loads the ports as game12 does (common00/01, the map pack,
  player 0's weapon + voice, em_blank + snd_em01) and starts the stage
  stream like stage_bgm_set: st04 plays the camp theme S_M6CAMP (its
  stage_bgm_etc_tbl entry, first entry into the stage), other stages
  Snd_bgm_tbl[stage] (ambience such as M6_MORI1, M2_KAZE1).
  `RT_SND_AMBIENT=1` skips the first-entry theme; `RT_SND_MAP=n` forces the
  map pack.
- stage_se_move (from f_stage_nm.c) runs every tick: river / waterfall
  loops on stages 1, 3, 0x1A, 0x30, 0x34, 0x36, 0x3E.
- Footsteps: the host player stand-in calls rt_snd_player_motion before
  frame_move: the run loop's entries of the player's per-motion sound
  list (ef_move_sub, main 0x24A790: ashi_sd_req kind 2 at frames 8, 30,
  54), with the ground material the game's collision wrote to pl+0x70D.
  The Rathian's walk (1003) plays em01's list entries (code 1 at frames 52,
  116) from rt_snd_monster_motion, at the monster's position (no joints).
- `RT_SND_TRACE=1` prints each pack loaded, stream started and sound
  played (port, code, program, note, VAG, volume, pan).
- Verified 5 Oct 2026 (offscreen, `--audio-dump`, nobody listened):
  st04 idle 4 s: the dump equals the Python ADX decode of S_M6CAMP
  sample for sample from 1 s to 4 s (after the 0.5 s fade-in); `--input
  "idle*10,left*110"`: footsteps at ticks 24, 46, 70, 95, 119 (22-25
  ticks apart = the 8/30/54 frames of the 78-frame run loop), programs 3
  then 1 as the hunter crosses from one ground material to another;
  stage 3: waterfall (code 0x22, 3.7 s loop) and river (0x21) start on
  tick 4 and keep playing; stage 0x21: Rathian steps every 64 ticks. The
  SDL device path was checked with a test program (a 1 s tone is consumed
  in real time).
- Not done: attack / weapon / voice sounds (the player has no actions
  yet), other monsters' and motions' lists, joint positions for sound
  sources, reverb, ADSR envelopes, the quest BGM changes (fight,
  clear), menus.

### Stage drawing (trans_stage)

The PS2 draws the area model in trans_stage (main 0x15CD90), not as one
static mesh: world = Trans(stage_work.pos) * Rxyz(stage_work.rot) (both
zero, stage_w_init), but part 0 (sky) and many per-stage parts are
special. Examples [read from the asm]: st04 part 2 (a ring of clouds
around the origin) is drawn at 13200,0,5190 and slowly turned; st05 draws
part 2 twice at set05_pos_tbl1 (10000,0,11500 / 10000,0,14500) and part 3
at 10000,0,8500 - these parts are modelled around the origin, which is why
st05 had "holes" before; skies of stages 0x19/0x3A/0x40-0x42 turn around
a centre point; water and lava parts get UV scrolls from stage_work.timer
or game_w+0x1E (a u16 counter that counts up every tick). After the area
model it draws set-model parts at set??_pos_tbl rows (x, y, z, angle Y).
Set-model parts are no longer drawn by the host at all: on the PS2 only
trans_stage and the set objects draw them. Verified with shots of all 88
stages (build/show/A/stages/sheet0.png, sheet1.png): st05 is a closed
jungle floor (ts_st05_low.png), st04 now shows the camp tent and ruin
wall, which the host's old "background parts first, z-write off" pass had
hidden.

The host's hunter is player_work[0] (rt_set_player: in use, on the stage,
at its position), so set code that follows the master player works.
`RT_TRACE=1` prints each set object as it starts (type, arg) and the prims
queued in the first drawn frame.

Game logic ticks at 30 per second; the host draws its own models (each
part with its clay_attr_set state), then `rt_game_draw()` walks the
ordering tables. Set-model parts the game C has drawn are skipped by the
host's generic draw so they are not drawn twice.

Blend modes (fl state 0x5E, read from flPS2SendRenderState_ALPHA and the
tables above): factor codes 0 zero, 1 one, 2 src alpha, 3 1-src alpha,
4 dst alpha, 5 1-dst alpha; codes 6-9 have no GS form and are ignored, as
on the PS2. Default (clay_attr_reset) is src alpha / 1-src alpha. Blend op
0x400 is subtract (GS A-B with A=Cs); 0x800 is taken as reverse subtract
[guess]. Filter: 0x10000 = point, 0 = bilinear (TEX1 MMAG/MMIN). Clamp:
0 = repeat, else GS REGION_CLAMP (drawn as clamp-to-edge).

Verified 5 Oct 2026 (offscreen shots, `--size 640x480 --frames 3`):
- `--time 1.0 --cam 11060,700,5000,0,-0.1`: identical to the earlier
  hand-spawned set14 shot (build/show/rt_set14_1.0.png), so the spawn list
  reproduces it; between T = 1.0 and 1.5 only waterfall/mist pixels move.
- `--cam 12300,500,8200,4.71,-0.1`: set00's light shafts through the ruin
  arch (build/show/rt_set00_1.0.png); T = 1.0 vs 2.0 differ inside the
  shafts (texture scroll).
- `--cam 11000,500,8000,2.23,0.3`: the sun glare (build/show/rt_set13_sun.png).
  Whether its size and brightness equal the PS2's is not checked.

Porting hazard found: main C sometimes declares a callee with its
arguments in a different order than the callee's own definition (floats
and ints use separate registers on the PS2, so it still matched). On x86
the order matters: set13c.c's hit_cap_sphr_m declaration was fixed for this
(still matches). Check prototypes against the definition when adding C.

Adding more game C: put the file in GAME in tools/build_pc.sh, the data
tables it needs in src/pc/rt/tables.txt (the link errors name them), route
func_XXXXXX calls in rt_overlay.c, and add whatever it calls into rt_*.c.
For split files use the whole-file `_nm.c` (e.g. set05_nm.c), not the
matching pieces.

## Plan: a player and a monster on the runtime with real input

Written 5 Oct 2026 (agent A), not started. Coverage numbers are matched
bytes from config/c_files.txt by address range; near-match `_nm.c` files
add more logic that already runs on the PC.

What the game's own loop does each tick (read from the asm): pad read
(pad_get.c, matched) -> player_mv (pl01.c, matched) -> pl_move (0x14C3E0:
pl_sw_set, pl_move_sub, hit_timer_calc_shl) -> per-weapon state machine
through pl_prog_tbl (0x2F1590) -> motion update (frame_init / frame_move,
0x125920 / 0x125F10) -> enemy_mv (0x10CB20) -> em_move (0x10BF30) ->
per-monster em_prog_tbl (0x2E8330) programs in game.bin -> CameraMove
(0x21F590) -> draw: trans_stage (ported), player_trans (0x1678C0),
enemy_trans (0x168B10), prims (ported), effects/shells (ported).

| piece | where | state |
|----|----|----|
| pad -> sw buffers | main pad_get.c, pl_normal2.c (sw_set_sub) | matched; runs on the PC with the host pad backend (rt_pad.c, src/pc/pad) |
| player loop entry | pl01.c player_mv / pl_init | matched |
| player states (walk, run, roll, weapon, items) | main 0x134950-0x14D1C8 (f_pl) + helpers to 0x155000 | f_pl ~260 functions matched + pl_nm.c (agent F); all of it runs on the PC, the helpers from rt_pl.c (see "Player") |
| motion system | main f_frame 0x125340-0x1267BC (18 functions) + fl motion layer 0x173A50-0x1746A0 | f_frame: 16/18 match, all 18 run on the PC (f_frame_nm.c); fl layer native in rt_motion.c |
| player/monster drawing | player_trans, enemy_trans, 45 functions | ~3 %; the viewer's hunter_pose / fl_model_pose do the same job natively |
| collision | GetGroundHit, wall hits (main 0x111000-0x125000) | f_sphr all in C (agent D, near-matches); runs on the PC for the hunter and the Rathian (see "Collision") |
| monster common (em_core, em_master, em_taisei) | game 0x533980-0x53A000 | ~65 % matched + near-matches |
| Rathian/other monster AI | game em01.. (363 functions, 150 KB) | ~13 % matched; em01.c (Rathian action setters) partly |
| camera | main f_cam, f_cam_223B50 (agent D), g_SetAreaData (camarea_nm.c) | runs on the PC in --play (see "Camera") |

Done (agent A, 5 Oct 2026): steps 1 and 2 below, and a host stand-in for
step 3 (rt_player.c) so the hunter runs and turns with the pad.

Suggested order (each step ends in a screenshot or a short input replay):
1. Host input: map an SDL controller to the PS2 pad bits and fill the
   buffers pad_get.c reads; record/replay pad logs for offscreen tests.
2. Motion bridge: implement frame_init/frame_move/frame_check natively on
   top of fl_skel (same motion ids and frame counters in PLW/EMW), so game
   C that sets char0/char1 animates the viewer's models.
3. Player locomotion first: decompile only the "normal" state family
   (to_normal, walk/run/turn, roll) of f_pl plus pl_move_sub, with
   GetGroundHit on the host collision. Weapons later, one at a time
   (sword and shield first: smallest table).
4. Camera: build cam_t.c into the PC port (CameraMove behind the player)
   instead of the free-fly camera.
5. Monster: em_core/em_master already run; add the Rathian's (em01)
   program table and its action setters, its motion bank, and the
   joint queries (get_joint_pos/wmat) from fl_skel so effects attach.
6. Hits and damage last (pl_damage is matched; attack data tables are
   imported by rt_data already).
No big rewrite is needed: the runtime pattern (decompiled C + rt_* stand-ins
+ imported tables) scales; the work is decompiling the player state and
motion code. Static recompilation of the remaining asm (DECISIONS "Open")
would be the shortcut if steps 3 and 5 turn out too slow.

## Design notes, for the port

- **Small, fixed-function gfx interface.** The original Xbox GPU (NV2A)
  is DirectX 8 class, so gfx.h uses only these:
  - vertex arrays (position, RGBA8 colour, ST), triangle lists, one texture;
  - alpha test/blend, z test/write, fog;
  - three matrices.

  gfx_gl.c uses only GL 1.1-era calls. A D3D8/nxdk backend should be a
  same-size file.
- **Lighting and skinning on the CPU**, as VU1 did them on the PS2. The
  per-vertex lighting is the Vu1Code_0001_0002 model: ambient + 3
  directional lights, each max(0, n·L) × colour, clamped, times the vertex
  colour. Stage parts are pre-lit by their vertex colours. A clay therefore
  only needs pre-lit colours.
- **PS2 names:**
  - A *clay* is one AMO part (flCreateClayHandle / flExecuteClay →
    gfx_create_clay / gfx_execute_clay).
  - `gfx_set_render_state` takes flSetRenderState's numbers where they are
    known: 4 texture, 0xF-0x12 fog, 0x17 view, 0x1A world, 0x60 alpha
    reference, 0x67 fade colour, 0x6C z-write. Port-only states start at
    0x100.

  The game's own calls can later be routed here once the GS register
  meaning of the remaining states is traced.
- **fl matrix layout:** fl matrices are row-vector (`v' = v * M`, translation
  in m[12..14]). This is the same memory layout OpenGL's column-major
  functions take, so they load unchanged.
- **Motion clock:** motions run at 30 frames per second. Motion ids decode
  as in frame_init: bank = (id % 1000) / 100, slot = id % 100.
  - The hunter plays plcom ids 1 (legs, char0) and 101 (upper body, char1).
  - The Rathian plays slot 3 in banks 0/2/4 (body, head, tail).
- **Placement:** each actor is posed at frame 0; its lowest vertex gives
  the offset from the game position (on the ground, GetGroundHit) to the
  model origin.

## Known gaps

- **Render states:** per-part blend, filter and clamp come from the
  0xF0000 chunk (clay_attr_set). Cull, UV-scroll flag, fog and lighting
  type from the same chunk (states 0x00, 0x62, 0x12, 0x01, baked into the
  clay on the PS2) are not applied. Alpha test is > 0x40 for host draws;
  the stage uses the game's own state 0x60 values (0x80 / 0).
- **Rathian:** (fixed 8 Oct 2026, fl_model.c attach_tail_tip; corrected the same day to eft09_t's rule, tree 45/46/47 = nodes 43/43/44, see "Per-kind materials" / tail cutting) the tail tip (AHI tree 1) was not attached, so it lay on the
  ground. No blending between motions.
- **Hunter:** no weapon. Hair and cloth bones (ptmat ≥ 64) keep their bind
  offset.
- **Runtime:** fade colour (state 0x67) is 0xAARRGGBB with alpha 0xFF =
  1.0 (eft05 packs r << 16, eft_trans_sub sends 255 * a). No players/monsters run as game C yet, so player_work is
  zero (set13 uses the master player's position on some stages).
- **Scene:** `--stage N` loads any stage (files from main's per-stage
  tables); em01 and one armour set are fixed. On stages other than 4 the
  hunter stands at stage_start_pos[stage] (main 0x2F2620), the camera 2500
  behind it (on a few room stages, e.g. 20, the camera is then inside a
  wall: use --cam). Stage 0x11 (st11 files) has barrels (Shell10) at
  1400..4100 where the area model has no geometry: probably an unused
  stage [guess].

### ARM (Armbian RK3518 box, 6 Oct 2026)

The same port runs as a 32-bit ARM (armhf) program on a 64-bit ARM Linux box, without
root: `tools/build_arm.sh` cross-builds with Debian's gcc-14-arm-linux-gnueabihf and an armhf
sysroot unpacked in ~/mh1arm, and `tools/run_arm.sh` starts it through the sysroot's
loader (Mesa's lima driver from the sysroot). Setup of ~/mh1arm: a user-level apt config
with `APT::Architectures { arm64; armhf; }` and its own lists/status dirs, `apt-get update`,
`apt-get download` of the armhf closure of libsdl2-2.0-0, libsdl2-dev, libgl1, libglx-mesa0,
libgl1-mesa-dri, libc6-dev (apt-cache depends --recurse) into sysroot/, and of
gcc-14-arm-linux-gnueabihf, cpp-14-..., binutils-arm-linux-gnueabihf, libc6(-dev)-armhf-cross,
libgcc-14-dev-armhf-cross, linux-libc-dev-armhf-cross (+ bases) into cross/, all unpacked
with dpkg-deb -x; the cross libc.so linker script is edited to point at cross/. ARM-specific
flags: -fsigned-char (PS2 char is signed), -fpermissive (gcc 14). `RT_FPS=1` prints drawn
frames per second.
- Measured 6 Oct 2026 (H96 Max, RK3518, Mali-450 GL 2.1, quest 10 at the cave, no fight):
  game logic at full speed (30 ticks/s), about 27 fps drawn at 960x720 and 48 fps at 640x480;
  a --shot screenshot looks the same as on x86. Not tested: long play, fights, the village.
- Re-measured 6 Oct 2026 evening (box idle; current build with -ftrivial-auto-var-init=zero):
  base camp 25-26 fps at 960x720 and 48 fps at 640x480, cave 27-28 fps at 960x720. No
  regression; an earlier 18-19 fps reading was another copy of the game left running on the box.

### Power-on, new game / continue, memory card, village features (agent A, 8 Oct 2026)
`tools/play.sh` (no argument) now starts from power-on: `mhview --boot`.
- Boot (src/pc/rt/rt_boot.c): the game's own task scheduler (tsk_nm.c) runs
  select.bin's Init_task (card check, options auto-load CardAtld), Demo_task
  (rating screen, middleware and Capcom logos, title), main's Select_task
  (omake_nm.c: NEW GAME / CONTINUE / GALLERY / OPTIONS, then the village /
  town choice), Edit_task (character creation) or Cont_task (load a hunter),
  plus Fade_task / Card_task. When a task starts Game_task the host takes
  over: game mode 6 = the village (Game_task's offline path). "Go to town"
  (network) falls back to the village.
- select.bin data: rt_sel_mem (like rt_lb_mem), symbols from
  config/symbols/select.txt aliased by tools/gen_rt_auto.py, pointers from
  .relselect.bin. main calls select functions by address: D_533BE0 /
  D_5367F0 / D_5375F0 (defsyms), func_534650 (user_data_copy) and
  func_533A00 (Init_task) in rt_overlay.c.
- The tasks draw while they run (flps0008, font_draw, trans()); one tick's
  gfx calls are recorded (src/pc/gfx/gfx_rec.c, hooks in the backend) and
  every frame replays the last tick, so frames and 30 Hz ticks stay
  independent. trans() (rt_boot.c) only draws during the boot.
- Fades are real now (fade_nm.c + Fade_task, also outside the boot via
  rt_sys_tick; host quest starts call fade_set(2) as game13 does).
  `RT_NO_FADE=1` hides them. all_reset is a host version (trans list, fade,
  sounds, fonts). The opening movie (Sofdec) is not played: its wait ends
  at once. The online patch check after a load (PatchLoadinDNAS) is done
  at once.
- Name entry: the soft keyboard (sk_nm.c, kana/kanji) is not ported; the
  stand-in in rt_menu.c takes typed ASCII (stored full width via han2zen),
  Enter or the pad's start finishes, empty = "HUNTER". `RT_NAME=x` for
  scripted runs.
- Memory card (src/pc/rt/rt_mc.c): libmc (sceMc*) on a host directory,
  `$MH1_SAVE_DIR` or `~/.local/share/mh1pc/memcard0`; port 1 has no card.
  The game's mclow/mcact/mccomb C runs unchanged on it, so the save is the
  PS2's own BISLPM-65495MH directory (data file 0x11450 bytes, icon.sys,
  icon00.ico). `RT_MC_TRACE=1` prints the commands.
- The hunter's look: armor_create_model runs Pl_model_id_set (written from
  the asm, main 0x123F60) and the bare-part rule of 0x124310; the viewer
  reloads m_/f_<part><n> models when the look changes (sex, face, hair,
  skin colour from the face, armour). Hair colour (PLW+0x5FC) is not applied.
- Village: item box (lobby f/lb_ib.c whole file; main's draw call 0x60CE50
  routed), shops/forge/armour pieces from agent B's b/ and b/nm files
  (LOBBY2 in build_pc.sh, linked weak). D_610370 (NPC body volumes for the
  camera) is lobby.bin's table (the zeroed placeholder crashed the camera).
- Test aids: `RT_BOOT_TRACE=1` (task slots), `RT_LB_WARP="tick,x,z[,ang];..."`
  (village warp), `RT_SHOP_TRACE=1`, `RT_VILLAGE_TRACE` lists each stage's
  spots (house door kind 12 at 11225,14350; in the house bed kind 14 at
  2230,745, item box kind 15 at 1950,1160; spots need square).
- Checked (scripted --input, shots in build/show/A/boot/): power-on ->
  logos -> title -> NEW GAME -> name/sex/face/hair -> save (file written) ->
  village with the first-visit event; restart -> auto-load message ->
  CONTINUE -> character select shows the saved hunter -> village; female
  hunter with face 4 in the village; bed save in the house writes the card
  file; item box store works. Not compared with the PS2.

### First quest loop, shops, matched lobby code (agent A, 6 Oct 2026)
- The Elder's first quest (131, "deliver 2 raw meat") from power-on to the
  next CONTINUE: Aptonoth (em12_nm.c, kind 12) and em29 (a breakable target)
  run on the PC; their tables that config/symbols lacks are imported by
  address (`NAME 0xADDR 0xSIZE` lines in src/pc/rt/tables.txt). Carving
  (pl_mv071 arg 3) gives raw meat; the camp's delivery box is unique spot
  kind 21 (10350,40,10640, circle -> Share_item_conv): "all items delivered",
  quest clear, 20 s, reward screen, money screen (+50z, counted up 1z at a
  time then the rest), village.
- Reward screen: ListSelect(&cur, keys, 2) (the count 2 is a2 left over in
  the asm, 0x292DB8); "end receiving" works.
- Village re-entry reloads lobby.bin's data and zeroes its .bss
  (rt_lb_reload = Load_overlay(3)); before, client_work said "village motions
  loaded" while the quest had replaced them and the hunter walked on the spot.
- Shops: item shop buy (-20z, herb to the pouch) and sell (+1z) checked; owned
  counts printed (font_print_ex count in t0); forge weapon list opens (crash
  fixed: lb_process_drawHelp read 16-bit list fields as s32).
- Spot hints ("square: enter house"): Lb_put_unique_act_hint (lb_ah.c, taken
  with PICK) and main's hint_tbl[0] mapped to lobby 0x64F1F0.
- Matched lobby code: tools/pc_lobby_matched.txt (56 files whose functions
  the PC took from *_nm copies) and LOBBY3 (7 that were gen_rt_auto
  stand-ins, e.g. cnWrap_SoundRequest: the village menu sounds) are linked;
  BMATCH weakens the other copies. PICK="file:sym" links single functions of
  a whole-file C.
- Hair colour: player_trans (0x167C38) writes PLW+0x5FC into the head part's
  first clay's first material; the PC multiplies that material's vertices
  (fl_model tint). Not compared with the PS2.
- Test aids: `RT_SHOTS=t1,t2,...` (with --shot X.png also X_<tick>.png),
  `RT_PL_WARP="t,x,z;t,x,z"`, `RT_PL_WARP_EM="t1,t2-t3"` (next to the
  target's carve point or body), `RT_PL_TARGET="tick:slot,..."` (which
  monster AIM / WARP_EM / DMG_MUL use), RT_SPOT_TRACE lists exits too,
  RT_LB_WARP counts village ticks over all visits; RT_QUEST_TRACE prints the
  quest's condition program and every Gold_add. tools/mk_input.py builds
  --input scripts from absolute ticks; tools/test_quest_loop.sh is the loop
  check (tools/pc_scripts/).
- Not done: opening movie (Sofdec decoding is not cheap: left skipped), the
  character screen's 3D hunter, forge list icons / page title (garbage),
  greeting window under the item shop's buy list, colour streaks over a
  CLEAR!! quest card. Nothing here compared with the PS2.

### Village glitches, character screen hunter, monster breadth (agent A, round 20, 6 Oct 2026)
Fixes (all PC side; PS2 rebuild all five OK):
- Forge list: the yellow page title is main's my_job_str, whose pointers go
  into lobby.bin. rt_import_lobby now finds every main data word whose
  ELF relocation symbol lies in the lobby.bin section (72 words: my_job_str,
  shop tags, menu help, armour shop tables, plaza menus ...) instead of
  three hand-mapped tables. Icons: matched Lb_put_job / Lb_put_icon
  (lb_ag01/02) and Lb_put_itemIcon / Lb_put_materialItem (were stand-ins)
  linked; 30 more matched shop/forge/dialog files in
  tools/pc_lobby_matched.txt (the forge list now has 2 pages of early
  weapons instead of 13 pages of everything). lb_process_drawHelp near-
  match: missing arguments added (argregs.py).
- Item shop greeting window and CLEAR!! card streaks: not seen any more
  after the above (shots of the buy list and the Elder's five ★1 cards, one
  marked CLEAR!!). Each card has a green smudge top left; whether the PS2
  card has it was not compared.
- Character creation / continue screens: player_trans called from the
  screens' prims now records a host draw (gfx_rec_call) of player_work[no]
  with the game's view (lpView). Continue: the save's look; creation: bare
  parts of the chosen sex/face/hair (the PS2 uses editpl_*_amh.bin, the
  same parts in one file — the picture was not compared).
- Monsters: em20 (Kut-Ku, Gypceros), em17 (Gravios, Basarios), em27
  (Velocidrome, Gendrome, Iodrome), em19 (Vespoid, Hornetaur), em04
  (Mosswine, Bullfango), em09 (Felyne, Melynx), em08 (Cephadrome,
  Cephalos), em21 (Plesioth), em14 (Diablos, Monoblos), em15 (Khezu), em03
  (Kelbi) linked (build_pc.sh EM, near-match copies weak via WEAK_EM), their
  game.bin tables in tables.txt. Lessons:
  - game.bin data an overlay C file names but tables.txt lacks becomes a
    *function* stand-in in rt_gen.c, read as data (em20 crashed on its fly
    height table). Check after adding files: weak `int NAME()` stand-ins
    whose symbol has no type:func.
  - em_prog_tbl entries point at file statics whose C carries the address
    suffix; map_ptr now tries NAME_ADDR (Mosswine/Melynx were "not ported").
  - Model / texture / motion files per kind come from main's tables
    0x2EC7A0 / 0x2EEE20 / 0x2EC830 (dromes use em16/em13/em30 models and
    em16 motions; Genprey had no motions before).
  - Monster slot 0 was always drawn with the host's em01 object; all
    monsters shared one joint-matrix buffer (rt_actor_joints keeps the
    pointer), so hit checks used the last monster's skeleton.
  - Per-file ABI adaptors (rt_abi.c) for em_frame_check(2), Eft13_set_em_scl,
    Eft15_set3, Eft02_set3; a0-left-over calls fixed in the drafts.
  - Quest event demos (evdemo.c) were NOPs: first-encounter monsters (Kut-Ku
    148, Cephadrome 154, Monoblos 171) wait for game_w+0x21F and never woke.
- Test aids: `RT_CAM_EM=slot,dist,height,yaw` (free camera on a monster),
  `RT_PROF=1` (host ms per game tick and per drawn frame), the monster trace
  shows act/sub/step.

Monster state (scripted runs from `--quest N` with RT_QUEST_STAGE=1, the
hunter warped next to the monster and slashing with RT_DMG_MUL, GOD mode;
"clear" = monster killed with RT_EM_HP/RT_DMG_MUL test aids, quest clear
(D5 3), carving checked through the pouch). Nothing compared with the PS2.

| kind | monster | code | state |
|---|---|---|---|
| 1 | Rathian | em01 | as before (quest 10, 170) |
| 11 | Rathalos | em01 | runs: sleeps in its nest (138), flies, attacks, flinches; kill not tested in a hunt quest |
| 6 | Yian Kut-Ku | em20 | runs (144, 148, 150): attacks, flies, flinches, flees to another area when weak; killed -> quest clear, 3 carves |
| 20 | Gypceros | em20 | runs (159): attacks, takes damage |
| 22 | Basarios | em17 | runs (173): rock disguise, attacks; killed -> clear, carve |
| 17 | Gravios | em17 | runs (172): attacks |
| 27/28/31 | Velocidrome / Gendrome / Iodrome | em27 | run (137, 156, 160): attack, flinch, die; 137 killed -> clear, carved twice (round 21) |
| 8/34 | Cephadrome / Cephalos | em08 | wake after the intro demo (154), swim in sand, attack; a sound bomb drives it out of the sand (round 21) |
| 14/26 | Diablos / Monoblos | em14 | run (174, 171): burrow, attack; little damage taken in the test |
| 15 | Khezu | em15 | runs (175): attacks |
| 21 | Plesioth | em21 | runs (165): swims; a sound bomb from the shore (11800,9300) while it is surfaced at ~10340,8920 makes it leap and fall back (act 4/17), then it swims on (round 22; without the bomb it does not) |
| 19/24, 4/5/32, 9/23, 3, 13/16/30, 12, 29 | small monsters | em19/em04/em09/em03/em16/em12/em29 | spawn and run without crashes in all village quests |
| 2 | Fatalis | em02 | runs (103-106): attacks (killed the god-mode-less hunter in 15 s); killed -> clear; its three pick points carved twice (round 21) |
| 7 | Lao-Shan Lung | em07 | runs (101, 102, 107): walks 14 -> 30 -> 28 -> 11 -> 12 (stage 12 at tick ~44000 in quest 101); its hit points stop at 1000 outside stage 12; killed on stage 12 -> quest clear -> reward (round 22) |
| 10 | trader NPC (red hair, backpack) | em10 | spawns on stages 5, 16, 41 (Quest_next_em_set adds kind 10 there); circle within 300 talks (hints, random gifts); trading checked on stage 41: a herb-class item (71) traded twice for item 77 with yes/no (round 22) |
| 33 | Kirin | em33 | in no quest on the disc (start positions only for quest 0); runs in free play with RT_EM_KIND=33 |

All quests 1-177 start on their monster's stage and run 450 ticks
(village 131-177: 1800-tick fights) without a crash.

Frame rate (x86, this machine, RT_PROF=1): game logic 0.22-0.33 ms per tick
in big-monster fights (Rathian quest 0.23, Kut-Ku 0.33, Basarios 0.32): cheap.
The host's per-frame work is the CPU skinning of every visible model
(fl_model_pose, per monster and per hunter part) plus the GL calls; the
monster count on a stage is what grows it. The character screen poses and
skins its hunter once per drawn frame (replay), not per tick. Not measured on
the ARM box: run with `RT_PROF=1 RT_FPS=1`.

### Last monsters, items, intro demos, gathering and fishing (agent A, round 21, 6 Oct 2026)
All PC side (src/pc, tools/build_pc.sh, tables.txt); no PS2-built file and
no include/ header changed.
- Monsters: em02 (Fatalis), em07 (Lao-Shan Lung), em10 (the trader) and em33
  (Kirin) linked; every monster kind now has its code on the PC (table
  above). PC versions of main's RedDragonEscapeCamera / F_DragonEscapeCamera
  (0x225E90/0x225EA0, from the asm) and Em_se_req2_com. The scan of all
  quests 1-177 (start stage and monster stage, RT_QEM_DUMP) found kind 33 in
  none of them.
- Items: flash bombs. push_senko now runs Em_Senko_Ck as the PS2 does, and
  move_senko / move_smoke (0x16A670 / 0x16A4A0) count their entries down
  each tick (they were no-ops: flashes and smoke never went away). Checked:
  Genprey that face the flash take their damage reaction; the Rathian in the
  test was looking away and was not blinded. Sound bomb (item 33, shell03
  arg 9 -> Shell09 type 13) drives Cephadrome out of the sand (quest 154).
- Intro demos (first sight of a monster): the camera now also ticks on the
  first two ticks of a stage, so a demo requested on tick 0 plays instead of
  ending at once. Quest 154: HUD hidden, the fin pass, the leap, a close-up.
  For ~60 ticks mid-demo the ground is a flat grey plane (camera at sand
  level); not compared with the PS2.
- Forge greeting window: not reproduced on x86. At the weapon-workshop NPC
  (lobby x68 14, at 9960,12120) the greeting closes 1-5 ticks after "next";
  one earlier run showed it for one frame when the sub-menu opened. Since
  the window is only drawn in shop steps 1 and 3 (Lb_shop_talk), a longer
  linger on the ARM box would come from drawn frames lagging ticks, not from
  the game logic. Not changed; PS2 behaviour not checked.
- Single-player content (quests 131/154, --stage, scripted):
  - herbs (circle at a pick point: 3 herbs, item 71), mining (pickaxe 131
    at a kind-3 point: ore 109, pickaxe broke), bug catching (net 134 at a
    kind-4 point: item 91): work. RT_SPOT_TRACE lists the points.
  - fishing: works now. func_5589F0 (Fish_set) was a no-op stand-in, so
    fishing spots had no fish. Stage 54 (desert): bait 122, cast, bite after
    ~370 ticks, circle -> fish 94, one bait used.
  - Also wired: Bdora_hp_ck (Lao-Shan half-HP quest condition) and
    Em09_item_sub (Melynx's stolen item) instead of stand-ins that returned 0.
  - Farm, Poogie and a training school: none in MH1's offline village (no
    such code or NPC found). lobby.bin has a pig NPC (npcPig*, lbnpc*.c),
    not in the village's NPC list; probably the online town [guess].
- Test aids: `RT_QEM_DUMP=1` (each stage's monster list of a quest),
  `RT_EM_KIND=n` (free play with monster kind n and its own model),
  `RT_PL_WARP="t,x,z,ANG"` (optional facing, hex), RT_PL_WARP_EM goes to a dead
  monster's own pick points, RT_PL_TRACE prints the fishing bite timer.
- Not done: Lao-Shan kill on its last stage, trading with the em10 trader,
  sound bomb next to a swimming Plesioth, frame rate on the ARM box (no new
  per-frame host work except the fish effects on fishing stages).

### Progression, trader, demo camera, Lao-Shan kill, music (agent A, round 22)
All PC side; no include/ or PS2-built file changed.
- Star levels: the game's own code (Lb_make_quest_tbl_local, lb_get_quest_level,
  get_flag_quest; main tables quest_local_tbl / flag_quest_tbl_local /
  key_quest_tbl) already runs. How MH1 offline works, read from the tables:
  1 star = 131-135 (0x83-0x87); clearing all five offers the urgent quest 136
  (0x88, "first monster hunt"); clearing 136 opens 2 stars. 2 stars need 138
  and 142 (0x8A, 0x8E) for the urgent 137 (Velocidrome); 3 stars 0x90/0x93/0x94
  for 154 (0x9A); 4 stars 0x96/0x9C/0x9E for 139 (0x8B); 5 stars
  0xA2/0xA6/0xA7/0x8C for 171 (0xAB), which opens the hidden sixth list.
  The Elder shows the urgent quest as a single card; afterwards the level list
  (cleared levels marked CLEAR!!, locked ones grey, "????" last).
- `tools/test_progression.sh` (~10 s, after test_quest_loop.sh): RT_QCLEAR
  marks quests cleared (hex list, "84-87,8a"), the bed save writes them,
  CONTINUE must show the level and the urgent quest (trace lines
  "rt_village: level N, cleared: ..." and "quest list (key XX)" with
  RT_QUEST_TRACE). Walked: 1 star -> urgent 136 -> 2 stars (kept by the save)
  -> urgent 137 -> 3 stars (kept). Screens: build/show/prog/. The real
  clear path (f_reward's Quest_clear_bit_set) is the one test_quest_loop.sh
  checks with quest 131; the urgent quests 136/137 were not played.
- Trader (em10): talk = circle within 300 units (Sansai_talk_ck -> pl_mv091),
  messages page with circle, yes/no with d-pad left/right. Trade tables per map
  (map2/4/6_trade_sp/_nm) work as written; checked one trade path only.
- Intro demo grey ground fixed: demo cuts placed relative to the monster read
  EMW+0x60 (its world matrix), which only enemy_mk in trans() wrote; the PC
  now builds it (and PLW+0x60 as player_modify does) every tick. Quest 154's
  middle cuts now follow the fin through the sand.
- Flash bomb facing: Em_Senko_Ck / senko_ck work from the monster's head
  joint direction and its search fov (Rathian 0x1555 = 30 degrees each side).
  A flash landing in front of her head blinds her (act 4 + the eye damage
  path); one landing behind her head or 31 degrees off does not. Item 27 is
  the flash bomb.
- Lao-Shan: `RT_PL_GOTO="tick,stage"` walks the hunter through the area exits
  (breadth-first over the STG_MV lists). Quest 101: hunter to stage 12, the
  monster arrives at tick ~44000, killed there (RT_EM_HP=1300 to save time)
  -> clear -> reward. The death dust crashed: Eft10_set needed an ABI adaptor
  for em07/em08 (rtabi_Eft10_set).
- Music: bgm_server and the game's stage_bgm_set (src/main/sound/bgm_nm.c)
  are linked: monster-found and fight music (S_FOUND1 -> S_FIGHT2), quest
  clear (S_CLEAR1), faint (S_DEATH1), ADX one-shots (adx_se_set). Still no
  reverb (flSndSetRev is a no-op). Weapon swings, hunter voices, monster
  calls, ambience and village music were already there.
- Not done: Plesioth sound bomb beyond the one case above; whether the PS2
  Plesioth reacts the same; opening movie (see DECISIONS.md); ARM frame rate.

### Urgent quests for real, Plesioth, reverb (agent A, round 23, 7 Oct 2026)
- `tools/test_urgent.sh`: 136 (three Velociprey, area 40) and 137
  (Velocidrome, area 34) accepted at the Elder, hunted, rewarded, saved; a
  real clear moves 1 -> 2 stars and 2 -> 3 stars (CONTINUE checks the save).
  The required non-urgent quests are marked cleared with RT_QCLEAR (setup
  only). New aid: `RT_PL_TARGET=kN` = the nearest living monster of kind N
  on the hunter's stage (for WARP_EM / AIM / DMG_MUL).
- Plesioth (quest 165, stage 54 cave lake): swims deep (y -1990) and near the
  surface (-660), spits at a hunter on the shore (act 3/4), leaps ashore
  (2/14 -> 0/4), walks and attacks on land (1/x, 3/2), goes back (2/16).
  Hits land while it is ashore (starter sword: 1 damage per hit; the Rathian
  takes 1-5, so plausible). It cannot be reached while submerged, as the
  hunter cannot swim. Kaeru_ck (frog-bait check that lets a hooked Plesioth
  be pulled out) and FishWyvernCameraRequest were no-op stand-ins: now the
  game's (PICK_MAIN in build_pc.sh links one function of a main file).
  Frog fishing itself was not reproduced (which bait item is the frog was
  not found; items 124/125 cast but nothing bit in 3000 ticks).
- Reverb: cheap. The game sets it per stage with flSndSetRev(core, type 4,
  depth) from Snd_rev_set_tbl (caves/nests deeper). audio_mix.c now has a
  small Schroeder reverb (4 combs + 2 allpasses per channel, ~12
  multiply-adds per sample) on the sound-effect voices; music streams stay
  dry. Not the SPU2's reverb program; which voices each SPU2 core carries
  was not traced. `RT_NO_REVERB=1` turns it off. Checked offscreen only
  (quest 10 nest: depth 10240 -> wet 0.19); nobody listened.
- Small fix: `--quest` printed a garbage monster kind for quests without a
  big monster.


## Movies (libmpeg2) (agent B, 7 Oct 2026)

What plays: OPENING.sfd at boot (the game's opening_demo, select/demo.c; it
runs after the logos and again whenever the title sits idle, then Start skips
it through the game's own Select_task), and the extras menu's movies
(omake_play). All nine .sfd in AFS00.AFS decode (sfd_tbl, imported from the
disc, gives AFS entry and size).

Parts:
- third_party/libmpeg2: libmpeg2 0.5.1 (GPL v2, COPYING in the directory),
  the plain-C files only (no asm, no libvo/convert), unmodified except our
  config.h. Built by tools/build_pc.sh into build/pc/mpeg2/ (also in the ARM
  build, which uses the same script; not run on ARM yet).
- src/pc/movie/sfd.c: demux of the .sfd (an MPEG-1 system stream, 2048-byte
  packs; video stream 0xE0 = MPEG-2 video, stream 0xC0 = ADX, header in the
  first packet), libmpeg2 for the video, snd.c's ADX decoder for the audio,
  YUV 4:2:0 -> RGBA (BT.601 limited). No platform calls.
- src/pc/rt/rt_movie.c: the game's movie_reset/start/request/server/draw/
  status_ck/exit on that. movie_draw is f_movie's: the 256x512 picture
  (rows 32..479 shown) stretched over the 512x448 screen, sp_mh.sfd (320x448)
  at its own size. all_reset stops a movie whose task was killed (Start at the
  title).
- Audio is the clock: stream 2 of the mixer (AUDIO_STREAM_MOVIE) counts the
  frames it played (audio_stream_consumed); the video is decoded up to the
  frame that time calls for (at most 4 per tick to catch up). Without an
  audio device the game tick (30/s) is the clock; --audio-dump counts as
  audio (the viewer now also dumps during the boot).
- Env: RT_NOMOVIE=1 skips movies (the old behaviour; the scripted tests
  set it, a 190 s movie would only lengthen them), RT_MOVIE_TRACE=1 logs
  every 600 ticks (frame, audio and wall seconds, decode ms), RT_MOVIE_DUMP=dir
  writes every 100th frame as PPM.
- tools/test_movie.sh: headless boot, checks OPENING.sfd opens and frames
  that are not blank reach the screen.

Checked: sfd_test (src/pc/movie/sfd_test.c, standalone) writes frames as raw
yuv420p. Against ffmpeg's output of the same OPENING.sfd (extracted to a temp
dir, never committed) frames 0-399 match the planes bit for bit while the
picture is black and at 65-70 dB PSNR afterwards (max pixel difference 6-10:
different IDCT rounding; one extra black frame at the start in ours, so ffmpeg
frame n+1 = ours n). The picture on screen (headless screenshot) shows the
opening's sky/Rathalos shot at the right aspect. Audio: ADX output differs
from ffmpeg's adpcm_adx by a small amount (mean 124 of 32768 over the first 20 s):
our decoder (snd.c, used for the BGM too) takes scale+1, ffmpeg's takes the
scale as is; CRI's own decoder (ADX_DecodeMono4, main 0x1F86F8) computes ((word ^ key) & 0x1FFF) + 1 and multiplies the nibble by that, so our scale+1 is right and ffmpeg's differs; snd.c now also masks with 0x1FFF as CRI does (checked by reading the asm, 7 Oct).
Decode time per frame (this PC, -O2, one core): OPENING 0.8-1.7 ms average,
worst 3-29 ms (a few slow outliers); the other eight 1.7-5.5 ms average, worst
13-50 ms (one 194 ms spike while other jobs ran). 256x512 is small: 29.97 fps
needs 33 ms. The Cortex-A53 / 733 MHz Xbox figure is not measured.
Not checked: listening (no audio device here; with SDL's dummy device the
audio clock ran 5% slow against wall time, that is the dummy driver pacing),
the extras menu path by eye (decode and the draw code are shared with the
opening), ARM, the window at other sizes, a real controller's Start skip.

### Frog bait fishing: investigation, not finished (agent B, 7 Oct 2026)
Setup that works: `RT_PL_ITEMS="125:5" RT_PL_WARP="10,11200,10850,C667"` with
`--quest 165 --stage 54 --play` and an input script (square at tick 60): item 125
(0x7D, the only bait that makes pl_mv079 call Eft22_set with arg 1 = frog float)
is cast from the stage-54 spot (10930, 10740, r 400), the hunter waits in act 80
with flag 0x80000 set. Facts read from the C: Plesioth (em21) notices a fisher
only through its command script: em_cmd_pl_fishing_ck (opcode 0x45, any hunter
with flag 0x80000 on its stage) -> em_cmd_target_pl_act_ck -> action 2/0x11
(em_fly17: Kaeru_ck(player) finds the arg-1 float, sets the hunter's x881 = bite)
-> 2/0x12 (em_fly18: pulls the hunter, FishWyvernCameraRequest). In 7000 ticks
(gdb hit counts) em_cmd_ck ran 52 times but em_cmd_pl_fishing_ck only once (before
the cast), so the script that contains the fishing check is not reached from
Plesioth's normal swim loop here (em_cmd_ninshiki_ck 10 times, sensor/find once).
Which precondition (distance, the hunter being sensed, a mind/ikari state) selects
that script was not found. Not done: the bite, the pull, the camera.

Update (frog fishing, same day): what selects the script. em_cmd_ck's main script
(table 0) ends `eye_dmg_ck (0x39), mode_ck (0x0B), main_jump (0x07)`: mode_ck compares
em->x888 (0 = idle, 1 = combat, set by Em_Mode_Chg) and jumps to table 1 (idle: its
`stage_no_sel` 0x15 picks the per-stage block, stage 0x36 block 8 holds
all_pl_same_stage_ck 0x28 -> pl_fishing_ck 0x45) or to tables 2/3/4 (combat: target
select, then the attack loop; none has the fishing check). So the frog bite is only
evaluated while the Plesioth is idle (x888 = 0), once per idle script cycle. Plesioth
leaves idle when pl_ninshiki_ck/the eye test (em_core_nm.c: search table kind 21:
dist 5000 horizontal, fov +-5461 (30 deg), down 1000 / xC 1200 vertical: a hunter more
than 1000 above the Plesioth is not seen, so it must be near the surface; no line-of-
sight test unless game_w.gate_open, which only quests 0x66-0x6A/0xCF set) sets x88F.
On the PC the Plesioth notices the hunter in the second script cycle and then x886
(the 900-tick combat timer) stays at 900 for 4000+ ticks while x88F is set, so the idle
script (and the fishing check) is never reached again. Not found: how the PS2 flow gets
the Plesioth idle while a frog float sits in the water (hunter outside its 30 degree
cone, or the Plesioth deep: at y -1990 the hunter, 2040 above, is not seen), nor why x886
does not run down here. Next test: cast while the Plesioth is deep (first ~60 ticks) or
from behind its cone, with gdb on em_cmd_pl_fishing_ck.
Build note: libmpeg2 compiles through cc_obj (objects build/pc/mpeg2_*.o); sfd.c is in
build_xbox.py's FRONT list; tools/build_xbox.py links (default.xbe built).

## Stand-ins wired to game C (agent F, round 22)

How the list was made: `RT_TRACE=1` run through the three tests prints each stand-in that runs once
("rt: NAME not ported"); the rest of the 414 weak stand-ins in build/pc/rt_gen.c were classified by name.
Online (cnLBS_*, CallBack_*, lm_*, plaza_*, Bs*/sceHTTP*/stock*/tag* browser, ssl) is ignored.

Wired this round (tools/build_pc.sh: `PICK_X` for main/game files, `PICK` for lobby files; only the named
functions are taken from each file; all 3 tests pass after each step):
- Monsters: clr_em_work, push_em_work_all (emw01.c), push_em_yobi, pull_em_yobi (emw02.c),
  em_search_set, get_joint_mat_em (emsrch_nm.c). Monster work push/pop and joint lookups no longer no-ops.
- Quests/village: Lb_make_quest_tbl (lb_v17.c, the elder's quest list), lb_guild_check_keyQuest (lb_gy01.c, key quests
  in the list), get_CA_size (lb_t.c).
- Menus: put_button_help (lb_uif.c, the help line; lbtu3 alias names are renamed back with objcopy in build_pc.sh).
- Sets/objects: Set06_set, Set21_set (set06.c, set21.c).
- Credits: Staff_init, Staff_main (staff_nm.c).
- Sound: Npc_se_req (sndc03.c), sound_req_com, ashi_sd_req_005C4980 (village NPC voices and footsteps).
- Visuals: stage_spr_disp (sun/sky sprites, f_stage_nm.c), lb_pl_item_trans (hunter item model in village, weapon3_nm.c).

Still stand-ins that a player could notice (not wired; reason):
- Village/menus: disp_status (hunter status screen, in lb_plz3.c, online TU), lb_rule_seet_set / lb_rule_seet_trans /
  lb_guild_make_room (room rule sheet, online rooms), lb_member_*Check, Lb_join, DispNameAndIDonDialog, fillRect.
- Items/equipment: EquipmentDescriptionWindowA_s, Equip_moji_color_rare_i, flfntLocate_i, Put_PageArrow_s (rename aliases of
  chat_nm.c functions: need linker aliases), armor_model_free, edit_create_model (model memory is host side).
- Effects/render: trans_shell, trans_set, trans_eft, trans_eft_up, draw_prim, SetDiffuseColor (the host draws these in rt_eft.c / rt_fl.c;
  wiring needs the GS packet layer, not done).
- Engine/loading (harmless on PC, run every start): View_init, init_view_work, light_init, load_eft, load_shadow,
  model_work_init, flAdjustScreen, flExp, setBGcolor, str_outmode, str_master_vol, str_stop_all, release_texture, ADXM_Lock/Unlock.
- Sound: cnWrap_Bgm* are online-side names; BGM goes through the host's rt_snd.c. flPADShockSet (vibration) is not wired.
New stand-ins appear when a wired function calls something else not ported (SetPartsTrans, weapon_dat_make*, light_change_normal, ...);
none of them ran in the tests.


## All offline quests played (agent D, 7 Oct 2026; `tools/test_all_quests.sh`)

Each quest is started with `--quest N` (no village), planned from its own condition program, monster lists
(`RT_QEM_DUMP` now also prints `kind:x04:x05` per entry) and the start stage's spots, and run to the reward and the village.
Delivery: items into the pouch (`RT_PL_ITEMS`), warp to the camp box (spot kind 21), circle. Hunting: `RT_PL_GOTO` (stage list,
or `tick,f` = follow the boss), `RT_PL_TARGET=kN`, `RT_PL_WARP_EM`, `RT_DMG_MUL`, `RT_PL_GOD`, a repeating pad pattern
(attack flicks, circle = carve / take, cross, ddown + circle = "end receiving"). New aids: `RT_PL_SLAY="tick[,kind]"` (a lethal
hit each tick on that kind, for a boss that stays out of reach), `RT_GOTO_TRACE`, `RT_PRIM_TRACE` (prims still held at exit and
where they came from). RT_PL_GOTO takes a list of stages and wraps; after two failed warps to an exit it takes the exit as
stage_mv_ck does (the warp lands on ground above the exit's height window on stage 37 -> 40). RT_PL_TARGET=kN now falls back to
a dead monster of that kind, then to an unused slot (so WARP_EM / DMG_MUL do nothing when none is on the stage).

| quest | stars | goal | result | bug found / fixed |
|---|---|---|---|---|
| 131-135 | 1 | deliver items (18x2; 20; 65x2+119; 1x2; 95) | clear, reward, village | none (131 was already walked for real by test_quest_loop.sh); gathering itself is not scripted except 131 |
| 136 | 2 | 3 Velociprey | OK | none (urgent test hunts it for real) |
| 141, 142, 143 | 2 | deliver 77x7, 242x5, 20x5 | OK | none |
| 138 | 2 | deliver egg (145) | OK | nest pick point (stage 40, stage pick id 131 -> item 145, unlimited), carried to the camp box; see the egg fix below |
| 137, 152 | 3 | Velocidrome (27) | OK | none |
| 148, 144 | 3 | Rathian-class (6) | OK | none |
| 145 | 3 | 10 Velociprey over 3 stages | OK | RT_PL_GOTO could not leave stage 37 for 40 (see above, test aid) |
| 146 | 3 | egg 145 | OK | as 138 |
| 147, 149 | 3 | deliver 219x3, 77x10 | OK | none |
| 150 | 4 | item 144 + monster 6 | OK | reward list is empty (no `rewards:` line) |
| 151 | 4 | 15 Velociprey (4 stages) + Rathalos (11) | OK (boss slain by aid) | none |
| 153 | 4 | 2 eggs (145) | OK | two trips |
| 157 | 4 | 3 eggs (146) | OK | nest on stage 49 (pick id 36, point below the warp's ground height: RT_PL_WARP now takes a 5th field y), three trips |
| 154 | 4 | Diablos-class (8) | OK | none |
| 155 | 4 | 15 kind-13 over 3 desert stages | OK | none; small monsters respawn per `x04` |
| 156 | 4 | kind 28 | OK | none |
| 158 | 4 | 15 kind-19 | OK | none |
| 159 | 4 | deliver 77x10 | OK | none |
| 160, 167 | 4, 5 | kind 31 (boss, stage 1) | OK | **prim pool exhausted -> crash** (set14_m / enemy_mv wrote through a NULL prim, tick ~9000): the PS2 clears all prims at every stage change (prim_init from game2 step 2 / all_reset), the PC had it as a no-op, so every stage change leaked the old stage's slots (set14 never releases its prim). Fixed: PC `prim_init` (rt_game.c) frees the pool; pool is 512 slots like the PS2 (was 256) |
| 161 | 5 | 20 Velociprey + Rathalos | OK | not a PC bug: the monster lists have a second wave (program op 32 sets quest_w.x3A = 1 at "10 left"; `RT_QEM_DUMP` shows waves as stage+100): stage 34 gets 3 entries x10; the plan now visits those stages again; Rathalos dies by the aid |
| 162 | 5 | Velocidrome | OK | none |
| 163 | 5 | 3 eggs (145) | OK | three trips |
| 139 | 5 | Rathalos (11, flies) | OK (boss slain by aid) | the Rathalos does land (about 40% of its ticks it is on the ground, ground attacks 3/x) and hits do count (2000 -> 1840 hp in 4500 ticks with DMG_MUL 40); the aid hunter just rarely reaches it, so the sweep slays it after 8000 ticks. **crash in em12_blood_req**: Eft02_set4 float-first ABI: adaptor rtabi_Eft02_set4 |
| 165 | 5 | 20 kind-13 + Plesioth (21, stage 54) | OK | second wave (stages 49, 54 ...) as 161; the Plesioth is hit every 120 ticks by the aid and dies once ashore (below) |
| 166 | 5 | kind 28 | OK | none |
| 168 | 5 | 24+ kind-19 over 6 stages + kind 21 | OK (boss slain by aid) | none |
| 140 | 5 | item 144 + Rathalos | OK | none |
| 171 | 5 | kind 26 (stage 53) | OK | none |

Counts: 38 offline quests, all 38 OK (no skips, no known failures). "OK (boss slain by aid)" means
the monster that cannot be reached is brought down by RT_PL_SLAY, so those runs test the clear / reward path, not combat
against that monster. The other hunts use real hits (DMG_MUL 40 on the target only). Not covered: real gathering and
fishing for the delivery quests, carving rewards, the eggs, urgent 136/137 clears for real (test_urgent.sh does those).
Frog fishing, round 2 (gdb on em_cmd_pl_fishing_ck; casting at tick 14 with item 125 from (11200, 10850)):
- The idle script runs in 352-tick cycles. em_cmd_pl_fishing_ck (0x45) is evaluated once at the start of each
  idle cycle (tick 1 in a fresh stage: no hunter fishing yet).
- The Plesioth rises 2 units per tick from y -1990 while idle. At tick 352 (y -830, 880 below the hunter, inside
  the 1000 "down" limit; inside its 30 degree cone) the eye test (em_eye_search_set) sets x88C and
  Em_Mode_Chg(1) flips x888 in the same tick the second cycle starts, so main script's mode_ck jumps to the combat
  tables and the fishing check of cycle 2 never runs (checked at 5 hunter positions, every one noticed by tick ~352,
  except positions inside the water where the hunter sinks).
- x886 (combat timer) is NOT a bug: em_move (src/main/em/f_em_nm.c:421) resets it to em_atk_mode_timer_tbl[kind]
  every tick while pl_ninshiki_ck reports the hunter noticed (x88F). em_mode_timer_sub's own code (em_master_b.c, a
  matched file) is identical to the near-match copy. Combat ends only when the hunter is unseen for the
  ninshiki timer plus the combat timer.
- So on the PC a frog bite is only possible if the Plesioth is still idle at a cycle start with the float already
  out. What differs on the PS2 (rise speed, the hunter being outside the cone while the Plesioth is high, or the
  hunter's flag14 == 3, which the eye test skips) is not known; fishing_ck/Kaeru_ck/em_fly17/18 are linked and
  untouched.

### Stand-ins, round 23 (agent F)
- Options sound: str_master_vol / str_stop_all / str_outmode are now host functions in rt_snd.c. Volumes come from system_w+0x36 (BGM) / +0x37 (SE) live (the SE
  volume used a constant 7 before); str_outmode(0) mixes both channels into both (audio_set_mono in audio_mix.c). Init_rev_set / Zero_rev_set: rev01.c from game C
  (calls the host flSndSetRev, so agent A's reverb approximation stays the single place that interprets the settings).
- Put_sprite_rotate (putspr3.c) and Draw_square (putspr_nm.c) were no-ops in rt_menu.c: now game C. smoke_init, smell_init, senko_init, ear_init, em_yobi_init
  (emw02.c; clear the monster state stacks the wired push/pull functions use) were no-ops in rt_flow.c: now game C.
- The "newly exposed" callees (SetPartsTrans*, weapon_dat_make*, sight_disp*, em_trans_sub, flmatAddTrans2, light_change_normal, func_5ACA60/5FCBB0/60E330/618F00) did not run
  in any of the five tests (RT_TRACE=1): they are only referenced by weapon3_nm.c / f_stage_nm.c functions that are weakened. Weapon and armour models are drawn by the host (rt_player.c) so nothing visible is missing there.
- trans_shell / trans_set / trans_eft: no GS packet layer needed. They are three small list walkers that call each object's trans(); the host does the same walks in
  rt_game.c / rt_eft.c (rt_eft_draw), so the stand-ins are never reached. Nothing to wire.
- Lighting is the real gap. The host lights every model with one fixed set (viewer.c, "lighting: the VU1 model"); the game's per-stage lights are not used:
  light_init (original bytes 0x11DB04, not decompiled, a stand-in), light_work (2 x 0x140 bytes, 3 lights of 0x68), light_change_normal / pl_light_change (stage direction rows
  from pl_light_tbl), light_move + flash_move (thunder), Pl_light_set (blend with the player's colour override), and light_set which hands the three light blocks to
  flSetRenderState(0x5A..0x5C) and ambient to state 1. Estimated job: decompile light_init (~0x260 bytes), fix the 0x68-byte light block layout (direction at +4..+0xC is known;
  colour and the VU1 matrix fields are not), have rt_fl.c capture states 0x5A-0x5C/1 into the fl_model Light, and use it for hunter, monsters, NPCs. About one to two days; the
  visible effect is per-stage/time-of-day lighting and the thunder flash on the storm stage.


### Frog fishing works end to end (agent B, round 3)
- flag14 == 3 is the "damaged" action kind (Pl_act_set(pl, 3, ...) in pl_damage.c), not fishing; the fishing hunter has
  flag14 0 (act 0/0x50 waiting), so the eye test does see him. That candidate is refuted.
- With the Plesioth kept idle (new test aid `RT_EM_BLIND=1`: x88B = 0 before enemy_mv each tick, so
  em_eye_search_set clears x88C) the whole chain runs on the game's own code with no PC changes: the idle script's
  stage-54 block (contents 8) passes em_cmd_pl_fishing_ck, em21 starts act 2/17 (em_fly17: Kaeru_ck finds the frog float
  Eft22 arg 1), the hunter reels (circle, act 0/0x53 = 83), act 2/18 pulls the Plesioth out (195 ticks), then 4/15
  (landed, steps 1-7) and it walks on land (1/x). Which fishing check passes is random per idle cycle (x39A): in the run
  FISHCK ran at ticks 496, 617, 1178 and only the last led to 2/17.
- Why it does not happen without the aid: the idle script's first cycle (352 ticks) starts when the stage loads, with
  no float out, so it targets the hunter (target kind player) and its closing act 2/3 turns the Plesioth toward him; the
  30 degree eye cone sweeps over the hunter, x88C is set, Em_Mode_Chg(1) and the Plesioth fights for as long as it
  sees him (x886 is reset every tick while noticed; that is the original). Whether the PS2 shows the same (a real
  player probably leaves its sight, waits for it to calm down, then casts before an idle cycle starts) is not
  verified. Not a PC bug as far as found: the eye angle follows the head joint matrix correctly (checked against
  the bearing), the casting and bite code is unmodified game C.
- tools/test_frog.sh runs it: cast at tick 14, bite at tick ~1179, circle at 1200; passes on the em act log.

### Stand-ins, round 23 (agent F)
- Options sound: str_master_vol / str_stop_all / str_outmode are now host functions in rt_snd.c. Volumes come from system_w+0x36 (BGM) / +0x37 (SE) live (the SE
  volume used a constant 7 before); str_outmode(0) mixes both channels into both (audio_set_mono in audio_mix.c). Init_rev_set / Zero_rev_set: rev01.c from game C
  (calls the host flSndSetRev, so agent A's reverb approximation stays the single place that interprets the settings).
- Put_sprite_rotate (putspr3.c) and Draw_square (putspr_nm.c) were no-ops in rt_menu.c: now game C. smoke_init, smell_init, senko_init, ear_init, em_yobi_init
  (emw02.c; clear the monster state stacks the wired push/pull functions use) were no-ops in rt_flow.c: now game C.
- The "newly exposed" callees (SetPartsTrans*, weapon_dat_make*, sight_disp*, em_trans_sub, flmatAddTrans2, light_change_normal, func_5ACA60/5FCBB0/60E330/618F00) did not run
  in any of the five tests (RT_TRACE=1): they are only referenced by weapon3_nm.c / f_stage_nm.c functions that are weakened. Weapon and armour models are drawn by the host (rt_player.c) so nothing visible is missing there.
- trans_shell / trans_set / trans_eft: no GS packet layer needed. They are three small list walkers that call each object's trans(); the host does the same walks in
  rt_game.c / rt_eft.c (rt_eft_draw), so the stand-ins are never reached. Nothing to wire.
- Lighting is the real gap. The host lights every model with one fixed set (viewer.c, "lighting: the VU1 model"); the game's per-stage lights are not used:
  light_init (original bytes 0x11DB04, not decompiled, a stand-in), light_work (2 x 0x140 bytes, 3 lights of 0x68), light_change_normal / pl_light_change (stage direction rows
  from pl_light_tbl), light_move + flash_move (thunder), Pl_light_set (blend with the player's colour override), and light_set which hands the three light blocks to
  flSetRenderState(0x5A..0x5C) and ambient to state 1. Estimated job: decompile light_init (~0x260 bytes), fix the 0x68-byte light block layout (direction at +4..+0xC is known;
  colour and the VU1 matrix fields are not), have rt_fl.c capture states 0x5A-0x5C/1 into the fl_model Light, and use it for hunter, monsters, NPCs. About one to two days; the
  visible effect is per-stage/time-of-day lighting and the thunder flash on the storm stage.

### Lighting from the game's stage lights (agent F, round 24)
- light_init (0x11DB10, 584 bytes) is decompiled as a near-match copy for the PC: src/main/model/light_init_nm.c (the PS2 build keeps the original bytes; -O4 inlines the helper so an exact match
  would need the two loops written out). With light_change_normal (light_nm.c), light_move (light04.c) and flash_move (light05.c) it is linked through PICK_X; init_light_work (rt_flow.c) and
  viewer.c's load_stage_models call it at every stage load.
- light_work layout: two sets of 0x140 bytes (set 0 = the stage set from stg_light_tbl, colours x10, light_set(0) in trans_stage; set 1 = the actor set from pl_light_tbl[stage], used by hunters,
  monsters, NPCs, effects through pl_light_change + Pl_light_set). Set + 0x10 + 8 + i*0x68 is the LGT block of light i (flSetRenderState 0x5A+i copies it into flLIGHT):
  +0x04 rgb colour (a), +0x14 second row (all 1.0 in the tables; sent to the shader as a per-light row), +0x24 rgb row c, +0x34 direction (the light travels along it; the shader negates it),
  +0x40 fourth row, +0x50 attenuation. Table rows (per stage, 5 pointers): [0] +0x40 row, [1] directions (12 bytes per light), [2] colours a, [3] rows c, [4] second rows (16 bytes per light).
  PS2SHADER_ADD_LIGHTCOL3 hands the a rows to the VU1 as the three light colours and the sum of the c rows as the ambient (docs/formats/graphics.md: mem 12-14 colours, mem 15 ambient).
- Host: viewer.c `rt_light_from_game` reads set 1 (+0x158 + 0x68*i) into the fl_light that fl_model_pose (CPU lighting) uses: dir = block+0x34 normalised, col = block+4, ambient = sum of block+0x24.
  `light_cur()` is used for the hunter, weapon, monsters and NPCs. **Default is the old fixed set again (round 25); `RT_LIGHT_GAME=1` switches to the game's stage lights** (the stage 5 hunter is almost black with them, see below), `RT_LIGHT_TRACE=1` prints the three lights. Lighting is on the CPU (vertex colours), so
  the GL and nv2a backends need nothing.
- Not done: per-actor adjustments (pl_light_change near-monster rows for stages 12/13/14/28/30 and the actor's own light table; Pl_light_set's blend with the player colour override), the stage set (set 0) for
  set objects, and the thunder flash: flash_move is linked but nothing decompiled starts it (no C writes the light_work flag byte; it is set from code not yet ported).
- Before/after (--stage N --play --follow 350,160,-0.15, 640x360, RT_LIGHT_FIXED=1 vs default), hunter in the middle, build/show/light/cmp*.png: stage 4 (waterfall plain): warmer key light from the
  upper right, shadow side a lot darker, more contrast; stage 5 (dark jungle): the table has no ambient row, so the hunter is nearly black on the shadow side (the old fixed set lit him evenly);
  stage 13 / 17 / 28 (cave and rock stages): slightly dimmer and bluer hunter; stage 6 (marsh grass): nearly unchanged. Village hunter (quest tests): a little darker with a visible light side.

### Lighting mapping decoded from the asm (agent F, round 25)
- flSetRenderState(0x5A+i, block) copies the 0x68-byte block into flLIGHT[i] (flrs07_nm.c); the shader packet builders (fladdm_nm.c, `flPS2AddMatrix_0001`: `PS2SHADER_ADD_LIGHTCOL3(AMB, PB(0xE0))`)
  put it into VU1 memory: packet offset 0x20 + 16*mem. mem 0 = flPS2Ambient * AMB (material ambient factor), mem 1-11 matrices and the light-direction matrix (mem 9-11 hold -dir transformed to object space, one light per column).
  PS2SHADER_ADD_LIGHTCOL3 (VU0 macro code, decoded with the COP2 table): mem 12-14 = the three a rows (block+0x04, w zeroed), mem 15 = c0 + c1 + c2 (block+0x24 rows, VADDA/VMADDw), w = 128.0. The rows at block+0x14 (all 1.0) and the
  +vec rows go to other shader families (0003 etc.), not to 0001.
- Vu1Code_0001_0002 (tools/vu_dis.py): MATERIAL: vf24-26 = a_i * material colour (vf29); vf12-14 = min(128, 128 * that); vf15 = min(128, (mem15 + mem0) * material ambient qword vf30). MAIN: per vertex dots = max(0, n.(-dir_k)) for the three lights;
  colour = min(128, sum_k dots_k * vf(12+k) + vf15) (128 = 1.0). So the host mapping (col = a, ambient = sum of c) matches the asm; two things the host does not model: the material's own colour and ambient qword
  (the model's material, flSetRenderState(0x3A+i)), and flPS2Ambient (flAmbient is 0: the game only ever calls flSetRenderState(0xE, 0)).
- The zero ambient rows of 44 stages (pl_light_ambientNN, .bss) are all-zero static tables, so those stages (5 among them) have no ambient on the PS2 either: lit only by the directional rows, hunter dark on the side away from them.
  Whether the PS2 really looks that dark is not verifiable here (no reference screenshots); the likely missing piece is the material ambient qword (vf30), which multiplies a zero ambient anyway, so it would not help stage 5.
  Hence the game lights stay opt-in (RT_LIGHT_GAME=1) until someone compares against a PS2 capture of stage 5.

### Per-actor lighting, thunder, set lights (agent F, round 26; all under RT_LIGHT_GAME=1, default unchanged)
- Per actor (src/pc/rt/rt_light.c, `rt_light_get`): before the host poses a hunter or NPC it runs the game's own steps, as PC copies in src/main/model/light_nm.c (PICK_X): `pl_light_change` (colour rows: stage rows;
  pl_light_tbl2 rows when work+0x613 is set on stages 12/13/14/28/30 (all three lights get the same row); the actor's own rows at work+0x710 for lights 0 and 1) and `Pl_light_set` (new near-match copy
  from the asm: eases each light colour toward the colour it had last time, stored at work+0x578 as 0xFFRRGGBB, new = target + (block - target) / 5, then flSetRenderState(0x5A+i)). rt_fl.c keeps what it got in `rt_light_blk`;
  light_change_normal(1) restores the stage rows afterwards, like the end of the hunter's trans. Monsters and set objects get set 1 as the stage rows leave it: nothing in the game calls pl_light_change for a monster body
  (only eft09, the NPC draw, the hunter draw). The 0x710 ground table of hunters is computed in rt_light.c (GetPlayerDiffuseData logic; the PC stubs GetPlayerMaterialData). The weapon uses the hunter's light.
- `light_move` was never called by the PC outside the village: sim_tick stands in for game_core (where f_frame_nm.c calls it), so light 2 of set 1 (turned with the view matrix) never moved. It is now called in sim_tick.
- Thunder: no game code ever starts flash_move. light_work set + 0x10/0x11 (state, run flag) are written by nothing in the C or asm (searched all of src/ and asm/ for light_work); light_tbl (the data flash_move blends toward) is the old
  stage-1 light01 set. The flash_flag / flash_timer pair in f_stage.c is a different thing (screen flash of the sun glare, set by eft14 type 5). So there is no storm stage that uses it: dead code in the shipped game. Test aid `RT_LIGHT_FLASH=N`
  starts it every N ticks (--stage shots run ~45 game ticks in 90 frames, so N=40): visible as a brighter hunter for about 30 ticks.
- Set 0 / set objects: trans_stage calls light_set(0) for the area model only; set objects are drawn in trans_set after light_set(1), i.e. set 1. Sweep of all 88 stages (RT_LIGHT_TRACE=2 prints each part's attr +0x14 lighting type): every area part has type 0 (unlit);
  the only lit parts are set-model parts of stages 5, 16, 25, 40, 41 (type 2, family 1). So set 0 is unused for lit geometry. Lighting those set parts would need per-instance world normals (the host draws them unlit, vertex colours): NOT done.
- Material qwords (RT_LIGHT_TRACE=3 lists them): diffuse colour B (mat+0x04, the vf29 qword) is 1.0 in every model checked (hunter, Rathian, NPCs, weapons), so the light colours need no factor. The ambient qword A (mat+0x24) is 0.5 everywhere.
  PS2SHADER_ADD_LIGHTCOL3 (decoded again): mem15 = min(sum of c rows * AMB, AMB), AMB = the model's mat+0x24 = 0.5; Vu1Code_0001 then multiplies it by A and adds it to a colour whose 1.0 is 128, so the literal ambient is about 0.002: a nearly black shadow side.
  `RT_LIGHT_VU=1` applies that literal formula (hunter from behind on stage 4 goes almost black). I do not believe the actors really use family 0001 (their parts carry no attribute chunk; the family comes from the render state, which Pl_light_set's flSetRenderState(1,1) clears
  to 0 in flrs07), so the default of RT_LIGHT_GAME stays ambient = sum of c rows. Unverified either way.
- Shots (build/show/light/, game camera, no --follow, 960x540, before = fixed light, after = RT_LIGHT_GAME=1): stage 4 almost identical (key light 1.0 plus ambient 0.6 saturates a lot); stage 5 hunter darker (no ambient row); stages 13 and 28 slightly bluer/brighter with a cool
  tint, light 2 now follows the camera; Rathian in the stage 40 cave a bit darker and greyer; flash: hunter brighter. Monster shots are not tick-identical (different game timing), so only the overall tint is comparable.

### Scrolling textures and translucent effects (agent F, round 27)
- UV scroll (fl state 0x19) only moves parts whose attribute asks for it (attr +0x1C, aa_uvscroll -> state 0x62; on stage 60 exactly parts 4, 6, 8, the ones trans_stage gives a matrix). The PC applied the last matrix to every clay drawn after it,
  so ground and wall parts slid ("shadows that scroll", "weird textures"). gfx_clay_desc.noscroll (set from the part's attribute chunk when it has one with +0x1C = 0) now makes GL and nv2a use the identity matrix; parts with no attribute chunk still take the matrix (set14).
- Draw order: trans() draws stage, actors (GameTrans), then shells, prims, set objects, effects. The viewer drew the game prims (rt_game_draw) before the monsters, hunter, NPCs and weapon, so a translucent effect (dust, fire, sparks) wrote depth first and
  cut holes ("blocks") in the actors behind it. rt_game_draw now runs after the actors. Shots with quest 10 (Rathian, stage 40): the dust puff and the flame on the hunter blend over the body instead of showing a square cut.
- (round 28: 0x6D and 0x5F are now implemented, see below) ZBUF check: RS 0x6C = 1 means z-write ON (ZMSK = (rs & 0x8000) != 0x8000), as the host has it. Still open: RS 0x6D is the GS ZTST (0 never, 1 greater, 3 gequal normal, 7 always: sky layer, set13 glare, sprites); the host ignores it and
  RS 0x5F is the alpha-test compare (not ZTEST, as rt_fl.c comments it; value 4 = greater). Not changed (needs both backends).

Round 28 (agent F): RS 0x6D (GS ZTST: 1 greater = GL less, 3 normal = lequal, 7 always, others never) is GFX_RS_ZFUNC and RS 0x5F (alpha-test compare, the game's 0-7 are the GL compare enums in order; 4 greater is normal, 5 notequal in the yn UI)
is GFX_RS_ALPHA_FUNC, in gfx_gl.c and gfx_nv2a.c; rt_fl_reset_states restores 3 / 4. 0x5F no longer switches the depth test. The sky layer (state 7 then 3), set13 glare and sprites now draw with "always"; shots of stages 4, 5, 21, 26, 39, 60
are unchanged within animation noise (old-vs-old differs as much). UV scroll against the asm: PS2SHADER_ADD_UVSCROLL (0x17CD30) copies rows 0, 1 and 3 of the 0x19 matrix, so st' = st * M like the GL texture matrix (row-vector memory = GL column-major);
the translate 1 - (X1E & 0x7F)/128 falls 1 -> 0 over 128 ticks, game_w.x1E counts +1 per tick on the PC (checked), so direction, speed and period are the PS2's; wrap is the part's own clamp bit (state 0x64). Stage 5 fern: GS TEX1 is
0x60 (bilinear mag and min, no mipmaps) and the GL/nv2a filters are the same, so the blocky leaves are the low-resolution alpha texture magnified, as on the PS2.

Round 29 (agent F): the UV-scroll matrix now applies only to parts whose attribute chunk asks for it (a part with no chunk never scrolls; only 9 parts in the game have none, stages 27, 33, 38, 71, 78, 79, 82, 85, 87, no waterfall).
Quest 131 forest/cave stages 21-24, what really scrolls (RT_UV_TRACE=1 lists every clay drawn with a live matrix; RT_LIGHT_TRACE=2 lists the parts): the sky and canopy layers of 21/24 (set19, tiny drifting offsets), the ground layer of stages 22/23
(trans_stage case 0x16/0x17, clay 6: v = 1 - (stage tick & 63) / 64, a full texture tile every 2 s, with alpha ref 0) and the set13 light-shaft billboards in the caves (u scroll, drawn with ZTST "always", z-write off, over everything as on the PS2).
These are the game's own code; no stale-matrix leak was left. The marsh floor of 22/23 is the likeliest "weirdly scrolling grass patch": it is the part-6 layer, so unless its speed is wrong it is meant to move. Pause/unpause (Start twice, 40 to 400 ticks, stages 22-38)
showed no leftover white shape in any shot (white-pixel count equal with and without the pause). Not reproduced: the owner's white thing; the cave light shaft (set13, ZTST always) is the only white translucent thing on a cave floor.

### First F8 bug report, "fog/light cone is messed up" (agent F, round 30) - and how an agent uses a report
- Using a report: `cp -r ~/.local/share/mh1pc/reports/report_X build/rep/` (read-only original), `python3 tools/show_report.py build/rep/report_X --replay` prints the note, the game state, the picked object (here: stage 35 set-model part 0,
  51 verts, 64x64, SRC_ALPHA/ONE, z-write off, scroll matrix) and the replay command. Replay: `RT_SEED=<seed> RT_PICK_AT=<tick> build/pc/mhview disc/mh1 --play --size 1024x768 --quest 151 --input @build/rep/report_X/input.txt --shot out.png --time 41.9`
  (1257 ticks replay in about a second, headless; RT_SHOTS=t1,t2,... writes a shot at each tick). The replay is close but not identical to the played frame (the hunter faces another way), so look at several ticks around the reported one (1150-1260 here)
  and compare an old binary against the new one. `RT_UV_TRACE=1` lists every clay drawn with a live scroll matrix (that is how the 51-vert clay was found in the replay).
- Cause: stage 35 (0x23) set13 arg 6 is a fog/light veil kept 50 units in front of the camera (set13_m: disp pos = camera + rview_mat[2] * -50, scrolling u, faded by sp). The PC ran `rt_game_move` (move_set, move_eft, move_shell) at the START of sim_tick, before the player and
  CameraMove, so the veil was placed from the previous tick's camera; when the camera swings (hunter turning, 20 to 50 units per tick) the veil is no longer in front of the lens and its straight, slanted edges show ("a huge flat brightened polygon"). The PS2 order (f_frame_nm.c)
  is player_mk, CameraMove, light_move, move_eft, move_shell, move_set. sim_tick now calls rt_game_move after the player and camera block (and before light_move). Shots at ticks 1150-1260 of the replay, old vs new binary: the slanted edges at 1210 and 1240 are gone.
  Side effect: set objects and effects now see this tick's hunter and camera (one tick less lag); the 9 PC tests, test_all_quests and the Xbox link pass.

Replays are exact (agent F, round 31). I earlier wrote that the replay of the owner's report "diverges"; it did not. The recording (the pad state of every game tick, rt_pad_set -> rt_pick_record_pad) plus RT_SEED reproduce the session exactly:
`RT_SEED=3425120 RT_PICK_AT=1269 RT_PICK_EXIT=1 build/pc/mhview disc/mh1 --play --size 1024x768 --quest 151 --input @report/input.txt` writes a new report whose game section (hunter [9467.1, -89.3, 8826.0] angle 4368, camera, the three monsters) equals the owner's
report.json to the last digit, and its screenshot has the same composition. What looked like a divergence was the suggested command: `--shot --time 41.9` stops by frame count at 1257 ticks (input.txt has 1257 pad ticks; the game tick is 1269, the first 12 ticks of a quest
read no pad), so the shot showed the hunter 12 ticks early. Stop with RT_PICK_AT=<game.tick> instead. Changes: show_report.py --replay and report.json's "how" print that command; RT_PICK_EXIT=1 makes the run quit once the report is written (headless);
tools/test_pick.sh's replay step now uses exactly that command and asserts the game section equals the recorded one (`pick OK: ... replay reaches the same state`), and checks that show_report prints the RT_PICK_AT command.
The sources listed in the task do not leak: pad is sampled per game tick in sim_tick, sticks are recorded as signed bytes, the right stick / mouse only move the free camera (not the game camera), and the random state is seeded (RT_SEED) with no wall-clock input in the game tick.

Findings of the second pass (agent D, 7 Oct 2026)
- **161 / 165 "18 of 20"**: the missing monsters are the second wave. Condition program op 32 (`quest_w.x3A = a`, "32/1/0/0" right after
  the "10 left" message) switches the quest to monster-list variant 1 (Em_data_st_adrs_get's last argument); Quest_next_em_set spawns
  that variant's entries when a stage is entered. 161: stage 34 gets three entries of 10 each; 165: stages 49 and 54. Nothing missing on the PC.
- **Plesioth at 1 hp while swimming is the original design**: Em_Dmg_Sys floors hp at 1 while `x8BB != 0`, and em21's swim action
  (em_fly18, em21_nm.c) sets `x8BB = 5` every tick; it can only die while ashore. A lethal hit every tick also keeps re-triggering
  its flinch, so RT_PL_SLAY now hits every 120 ticks.
- **Eggs**: Pl_item_stack burned a held egg (145/146, "hold" items) at once on the PC. Cause: pl_nm.c's `(int)(act_ck(...) << 0x30) >> 0x30`
  (m2c's 64-bit register idiom): gcc -m32 folds a shift by 48 to 0, so the "is the hunter in a pickup/carry act" tests were always
  true and timer_calc_sub_pl broke the held item every tick. Replaced by `(s16)(...)` in src/main/pl/pl_nm.c (4 places, also the
  Stage_env_ck test and the vital_red regeneration) and in the lobby near-matches the village runs (Put_page_num, lb_process_drawHelp,
  Lb_put_armorIcon, lb_normal_material, lb_process_set_armorList / _weaponList: shop and forge lists). The plaza_*.c and yn/ui_nm.c copies
  (online / unused on the PC) and src/main/fl/*_nm.c still have the idiom: grep `<< 0x30` before trusting a PC bug in them.
  Egg quests: a monster's hit makes the hunter drop the egg, so the run slays every monster in the quest first (RT_PL_SLAY takes
  "tick,kind,kind,..."), then per egg: RT_PL_GOTO2 ("tick,stage;..." timed goals) to the nest, warp + circle at the pick point
  (stage 40 (11400,12100) = item 145, stage 49 (9500,-109,11000) = item 146), goal = camp, warp to the box (spot kind 21), circle.
  Gather points: `RT_SPOT_TRACE` now lists the items each pick id gives.

## Idiom sweep (agent D, 7 Oct 2026)

Every object build_pc.sh compiles (the commands it records in
build/pc/cmd/*.sh, 728 of them, including the patched/abs copies) was
compiled again with `-w` replaced by -Wshift-count-overflow,
-Wshift-count-negative, -Woverflow, -Wint-conversion,
-Wincompatible-pointer-types, -Wreturn-type, -Wuninitialized,
-Wimplicit-function-declaration, -Wpointer-to-int-cast,
-Wint-to-pointer-cast and -Wdouble-promotion, plus `-aux-info` to list every
implicit declaration against the real definitions. The game C is normally
built with `-w`, so none of this was visible.

Findings and fixes (PC-built copies only; no file registered in
config/c_files.txt was touched):
- `<< 0x38 >> 0x38` (s8 idiom) on a 32-bit value: only one file still had
  it, src/lobby/b/nm/lb_process_drawHelp.c (2 calls of Lb_put_itemRare in the
  forge/shop item detail: the rarity stars were always 0). Now `(s8)`.
  Grep over every built source (also the abs/patch copies and include/) for
  shifts of 32..63 finds nothing else; the u64 code in src/pc/audio is
  real 64-bit arithmetic.
- Float passed to a function with no prototype (gcc promotes it to a
  double, so the callee reads the wrong 4 bytes and every later argument is
  shifted). `-Wdouble-promotion` finds exactly these:
  * src/main/omake/omake_nm.c `Disp_button();` called with 1.0f: the button
    icons of the mode/extras menu got scale 0 and a wrong kind. Prototype added.
  * src/main/eft/eft13_nm.c `flvecRotY(v, angle)` implicit (the angle went as
    a double): effect directions of eft13 (shell/chr-relative offsets) wrong.
    Prototype added.
  * src/main/cam/camr5_nm.c `hit_cap_sphr_m(..., radius)` implicit, radius as a
    double: the game camera's push-out against capsule hit bodies used a
    garbage radius. Prototype added.
- `(u8)Skill_name[i]`, a pointer cut to 8 bits (src/lobby/f/lb_aa.c, the
  hunter status screen's skill list; -Wpointer-to-int-cast). Cast removed.
- Missing `return` at the end of a function whose switch falls out
  (hit_cap_cap2_m / hit_cap_cap3_m in hit2_nm.c, BsParseCheck, lb_process_select):
  `return 0;` added (the inner switches always return, so this only guards
  out-of-range values). Left alone: Lb_menu_move_Core, lb_tu_ib's
  ItemboxWindowCursorX / item_explanation / kosuu_disp_sub /
  selling_price_disp_sub and lbui_nm's Draw_menu_square are int by m2c's
  default but their results are not used.
- Lb_chat_receipt called Lb_get_plID() without its argument (a0 left over
  on the PS2): tools/pc_patch.py now passes msg (online play only).
- Implicit calls (about 580 names, mostly the generated stand-ins and
  lobby helpers): none returns a float or a 64-bit value and none takes a
  non-pointer float argument other than the ones fixed above. Functions that
  return u8/s8/u16/s16 and are called through an implicit `int` prototype
  work with this gcc (checked in the object code: the value is always
  extended before ret).
- (second round, below) slash_level_bar and the flfntLocate prototype are fixed.
- 64-bit `long`: the only uses are `long int` return types and the u64/s64
  fields the code does want (32-bit gcc: long is 32 bits like the PS2's int,
  long long is 64).

### Idiom sweep, second round (agent D, 7 Oct 2026)
Screens checked (shots in build/show/d1/, scripted with the existing aids):
- Extras/gallery menu: the button icons at the bottom (circle "play movie", cross
  "cancel") draw (omake_nm.c Disp_button fix). The mode menu and options show no icons.
- Weapon workshop buy list (RT_LB_WARP to 9700,12120, then square, circle x3):
  the rarity line under the item icon shows "RARE-1" (was RARE-0 before the s8 fix).
  The detail window (square) is EquipmentCompareWindow, still a no-op stand-in.
  Not reachable offline: the player-status screen Lb_PlayerStatus with the skill
  names (it is the online plaza's view of another hunter); that fix is by the
  compiler warning only.
Real bugs found at run time (no warning shows them):
- Put_page_num (lobby/b/nm): `int sp70` was used as a 0x20-byte string buffer
  (han2zen writes 2 bytes per character): stack smash, the forge/shop list
  crashed with a segfault in han2zen. Now `char sp70[0x20]`. (Only the
  forge page counter; the page number shows as "1/156": the 156 comes from
  lb_num_str+0x2C, not checked against the PS2.)
- slash_level_bar (chat_nm.c, the sword's sharpness bar in equipment
  windows): read from the asm (0x27B3D0). The near-match took x as the pointer
  and the item data as an integer (a crash if ever reached), drew the two end
  caps as quads and stored 5.0f's bits as a coordinate. Rewritten: (pl, y, f32 x),
  caps are flps0009 triangles {6 s16, colour}.
- flfntLocate(f32, int) in hk_all.c / sk_all.c: the asm converts x with cvt.w.s
  into a0 (0x266DF0); prototype is now (int, int).
- Calls that lose the register argument ("a0 left over" on the PS2) and
  read a garbage stack slot on x86: Pl_stg_ck()/Em_stg_ck() in
  Pl_set_quake_sub / Em_set_quake_sub / Pachinger_set_quake_sub (cam_nm.c),
  Npc_se_req(_com) (sndc03.c, via pc_patch.py), Pl_master_ck() in adx_se_set /
  adx_se_stop (bgm_nm.c: the hunter's item/status sounds) and Pile_on
  (f_stage.c, via pc_patch.py), Item_box_get_efct() in box_get, the NPC program's
  init call in lb_npc_init_sub (lb_bz162.c, pc_patch.py; crashed under
  -fstack-protector-all with em = 0). Under
  -fsanitize=undefined 14 quests crashed at start (a null pl in Pl_stg_ck from the
  Gypceros-class quake effects); with the arguments passed all 38 quests pass
  there too. Still missing an argument (found by tools-side scan, not
  fixed, lobby/menu): Ud_item_num_ck/_ck3 in the lbmix* files, Lb_check_newCommer,
  Lb_pl_init, GetAdrsMiniData in lb_e.c, Get_equip_data_ptr in lb_ay, item_to_stack,
  shop_armor2_stack, func_5B4B20 in menu_disp_nm.c, Lbs_GetRoomInfo (online).
- lobby_bgm_set: see "tools/test_audio.sh" in the handover summary (village BGM
  was dead after entering the house).
How it was found: a copy of the tree built with `-fsanitize=undefined` in the
game C and the link (copy tools/build_pc.sh, set GAMEFLAGS and LIBS; the
first full build needs a second build_pc.sh run for pc_link_adapt). The test set
runs under it (about 3x slower). `-fsanitize=address` does not start (the data tables
the host fills by symbol name come up empty: pl01_adr_tbl null); `-fstack-protector-all`
in the game C runs the whole set clean (it would catch a Put_page_num style smash). Findings left as they are (harmless on the PC):
`x << 24` into the sign bit (colours; many files), `1 << 31` masks (shit11_nm),
pointer + offset wraps when the stage hit data is relocated (shit1_nm), reads of
player fields past PLW.part[2] and set00's tbl[i][2] on arrays declared too short
(real members follow), pl05.c:119 `pl->item[255]` (Pl_shell_set returns 0xFF when
the hunter has no item: the PS2 reads the next bytes, as the PC does), the HAGI
tables read with index 8 (see -fno-aggressive-loop-optimizations).
-Wmaybe-uninitialized: the player/monster/menu ones read (pl_nm Pl_item_stack,
f_frame_nm frame_init, f_stage stage_mv_ck, em_core/em_cmd locals, omake_nm
disp_mode_menu, menu_disp_nm Pit_disp_item_list): all are switch paths without a
default or a branch that cannot happen (system_error); none was a live bug.

### Idiom sweep, third round (agent D, 7 Oct 2026)
- Workshop detail window (square on a buy-list weapon): it was never a stand-in. EquipmentCompareWindow
  is chat_nm.c's game C and ran, but (1) its callee EquipmentDescriptionWindowA_s (config/main_aliases.txt:
  the same address as EquipmentDescriptionWindowA with s16 x/y) was a generated no-op on the PC, now an
  ALIASES entry in build_pc.sh, and (2) the callers dropped the 5th argument (t0: page / compare flag).
  Passed from the asm: lb_process_drawHelp (lbShop+0x6E, 0x53B914), Lb_shop_trans2 (0x80, as the matched
  lb_by139), the item box's compare / description windows (lb_ib.c, 0x60D154, 0x60D17C, 0x60CBB8: the
  description call also lost its x = base). Checked with a shot: both weapons' name, attack and
  sharpness gauge draw, current above, the shop's weapon below.
- Dropped register arguments, rest of the list: shop_armor2_stack (shop_process_after: kind/id =
  the shop table entry, sp49/sp4A), func_5B4B20(sw) in disp_menu, Get_equip_data_ptr(e) in lb_ay.c and
  GetAdrsMiniData(id) in lb_e.c (pc_patch.py). Not bugs after all (the scan matched declarations or
  the argument is unused): Ud_item_num_ck/_ck3, Lb_check_newCommer, Lb_pl_init, item_to_stack.
- BGM after entering a house: the question was whether the PS2's str_stop_all only pauses. It does not:
  str_stop_all = str_init = ADXT_Stop + ADXT_Pause(0) + memset of str_w (0x100910), ADXT_Pause does
  nothing on a stopped stream (0x203FA0 acts only in states 3/4), str_getstat is str_w[ch]+0, the
  ADXT_GetStat copied in each tick by str_server (0x100D60), and lobby_bgm_set is as decompiled (the
  same track id keeps "playing" without a restart). The village (stage 87) and the house (stage 86) share
  track 0x1A in Snd_bgm_tbl, so by the code the PS2 would also be silent after the house door; either
  the real game is, or something not decompiled restarts it. The PC keeps the str_getstat guard in
  lobby_bgm_set (music continues): a deliberate deviation.

## Activity tests (agent C, 7 Oct 2026; `tools/test_activities.sh`)

Scripted headless runs (tools/test_activities.py, helpers in tools/act_lib.py; logs and the last screenshot of each run in
build/show/act/<tag>.log / .png). 28 activities, one `PASS|FAIL name: what was measured` line each, 8 runs in parallel,
~15 s in all. `tools/test_activities.sh name ...` runs single ones. The checks are on invariants, not on one lucky result: item
ids must come from the stage's own pick table (RT_SPOT_TRACE), counts and prices are compared with what the shop's own list
and prompt show (RT_FONT_TRACE), recipes are read from the executable's tables (recipe(), mix_recipes(), item_names() in
act_lib.py), money/pouch/box/equipment come from a trace line the village prints after every change
(`rt_village: tick N ud money M pouch: id:n .. | box: .. | ware: kind/id/opt .. | wear: ..`, RT_QUEST_TRACE).

| activity | how it is driven | result |
|---|---|---|
| gather_herb | stage 39 pick id 20 (12200,10300), circle x4, RT_SEED 3/5/7 | PASS: herbs 82 / 87 only (the point's table), at most 3 per point (num 3), depleted afterwards |
| gather_mine | stage 32 id 117, pickaxe 131 x3, square x4 | PASS: ores 104/106/107/109 from the table; the pickaxe breaks (3 -> 2 or 3 -> 0) |
| gather_net | stage 36 id 124, net 134 x3 | PASS: bugs 89-93/124 from the table; net count drops |
| fishing | stage 54 (11200,10850), bait 122, cast, reel on the trace's `bite` tick | PASS: a fish 94-103, bait 5 -> 4 |
| carve_small | quest 131 stage 39, Aptonoth (kind 12) killed with RT_DMG_MUL, circle | PASS: raw meat 18, Aptonoth bone 227 |
| carve_large | quest 10, Rathian (RT_EM_HP=30) | PASS: Rathian scale 183, shell 184 |
| potion | RT_PL_HP=20:30, square | PASS: HP 30 -> 63, potion 3 -> 2 |
| whetstone | RT_PL_POKE=20:87E:50 (sharpness), item 105 and 155 | PASS: sharpness 50 -> 150, stone 3 -> 2 |
| paintball | pinned Rathian (RT_EM_PIN), thrown from 300/650/700 | PASS: monster mark EMW+0x56A set (the arc lands short at 400-600), ball used |
| pitfall | trap 30 (carry limit 1) set, the Rathian walks into it | PASS: trap state 50/6 (x9EA/x959), monster act 4/12 |
| tranq | trap + three tranquilizer balls 159 on the weakened Rathian | PASS: capture sleep (act 6/4), balls used |
| barrel | small barrel bomb 31, large 32 | PASS: small -20 HP; large alone does nothing until something explodes next to it, then -100 in all |
| bbq | spit 129 + raw meat 18, circle at the right frame (270-279 of the roast = well-done) | PASS: 19 (rare) / 20 (well-done) / 21 (burnt); spit stays, meat -1 |
| drinks | stage 45 (Stg_env_type 1) and 54 (type 2), items 160 / 161 | PASS: see below |
| combine | pause menu -> 調合, herb 65 + blue mushroom 79 = potion 1 (recipe table) | PASS after the fix below; a failed mix gives 143 |
| trader | stage 41 em10, 30 talks, yes with d-pad left | PASS: 71 traded for 77 (seed 5); other seeds give gifts |
| shop_buy / shop_sell / shop_qty | item shop NPC: Herb 20z, sells for 2z; quantity picker | PASS: money changes by the listed price; 50z buys 2 herbs, 10z none, carry limit 10 |
| wshop_buy / wshop_sell, ashop_buy / ashop_sell | weapon + armour shop: Iron Sword 2100z, head piece 300z, sell at half | PASS (sell lists crashed before the fix below) |
| forge_weapon / forge_armour / forge_upgrade | workshop: Iron Sword 1050z + 3 ore, head piece 150z, upgrade 1 -> 2 for 2 ore, 1350z | PASS: money, materials (from the pouch only) and the stored equipment match the recipe tables |
| box_store / box_take / box_equip | house item box | PASS: store, take, change the wielded weapon (the compare window draws) |

Bugs found and fixed (all PC side; `rebuild.sh` still OK for all five modules, the Xbox build links):
- **Item combining never worked.** Item_preparation_rate and Item_preparation_list_chk (src/main/item/item_nm.c) call
  `Item_preparation_adrs()` without arguments (K&R, the PS2 passes a0/a1 through); on x86 it read garbage, so the rate was
  -1 and every mix failed (junk item 143), and the pause menu showed "???%". Both now pass (a, b). Herb + blue mushroom now
  makes potions (rate table: item_pre_rate_tbl + the known-recipe list). This is the PS2 near-match copy used only by the PC;
  item02.c (the matching one) is untouched.
- **Segfault in the weapon/armour shop's sell list and in the item box's "take"** (ItemboxWindowX called with garbage). The
  definition (lb_ib.c) is `(int cur, int flags, f32 base)`, the shop code (lb_by139.c, lb_shp.c) and the wrapper
  ItemboxWindow in lb_tu_ib.c declare `(f32 x, int cur, int flags)` (PS2: x in f12). New `rtabi_ItemboxWindowX` in
  src/pc/rt/rt_abi.c, the three files get `-DItemboxWindowX=rtabi_ItemboxWindowX` in tools/build_pc.sh.
- **Page counter garbage in the shop lists** ("1/284" on the item shop's buy list, "1/1568594865" on its sell list):
  src/lobby/b/nm/Put_page_num.c (m2c) built the "%d%s%d" text with two arguments, the page count was a stack leftover. Now
  passes the third argument (the other copy, lb_plz3.c, already did). Pages show 1/5 (buy) and 1/3 (sell, 20 pouch slots).
- **Random numbers:** the PC's ran_suu (rt_game.c) kept a private state starting at 1 and ignored `Rnd_w`, which the co-op
  start seeds (so that seeding did nothing) and init_ran_suu sets. The first draw of the Lehmer generator
  (x * 176 mod 65363) from 1 has all low bits clear, so the very first gather at any point always used it up (the PS2
  checks `ran_suu(1) & 7`). ran_suu now uses Rnd_w; start-up seeds it from the clock (like the PS2's RTC) unless a
  `--input` script runs (tests stay repeatable); `RT_SEED=n` forces a seed (the value is mixed, so 1, 2, 3 differ).

Test aids added (tests only): village `RT_MONEY=n`, `RT_BOX_ITEMS="id:n,.."` (item box, 100 slots), `RT_WARE="kind:id,.."` (stored
equipment; kind 6 weapon, 2 head); `RT_PL_HP="tick:hp,.."`; `RT_PL_POKE="tick:hexoffset:value,.."` (PLW s16, e.g. 87E sharpness,
888 the selected pouch slot); `RT_EM_PIN="x,z"` (monster 0 put back each tick); `RT_SEED=n`. Traces: RT_PL_TRACE lines end
with `sh <sharpness> dr <+0x918>/<+0x91A>/<+0x8C0>`, RT_EM_TRACE em lines with `pt <paint> tr <x9EA>/<x959>`, RT_QUEST_TRACE prints the
`ud money` line in the village. The font trace only sees frames the host draws and a headless run draws few: use `RT_STEP=1`
(a frame per tick) when a check reads texts (the shop tests do, ~7 s each).

Findings that are not bugs (kept as the decompiled code has it):
- Item bar after the last item of a kind is used: the selected slot stays on the now empty slot (Pl_item_erase leaves it;
  only pressing L1/R1 cycles on), so the next square does nothing. The tests that use two items poke the slot.
- Drinks: item 160 (cooler) sets +0x918, 161 (hot) sets +0x91A. Stage type 1 (stage 45, desert, hot) drains HP unless +0x918 is
  set; type 2 (stage 54, cold cave) triples the stamina-gauge drain unless +0x91A is set. Names and effects agree.
- A carried pitfall trap is limited to 1 (Item_data[30][3]); RT_PL_ITEMS="30:3" is clamped to 1 on the first use.
- The large barrel bomb (32) sits until something hits it (a small bomb's blast, shell10 xB4); it does not go off by itself.
- Capture: no offline quest has a capture goal (no program op 3 in quests 1-177); the capture sleep itself works (act 6/4).
- Shock trap: no usable item exists in the offline item table (only 30 = pitfall; Shell12_set arg 1 is reached from netsyn only;
  158 "Trap Tool" is a material). Not tested.
- Farm / gathering spots in the village: none (the village's unique spots are the gate, the house door, a bench, and in the
  house the bed and the box).
- The hunter's gathering needs the weapon sheathed and the first circle after a warp is eaten by the landing action (tests
  wait ~100 ticks).

Carve sweep (one-off, not in the test file: quest/stage per kind from the quest dumps, RT_PL_SLAY + RT_PL_TARGET=kN + RT_PL_WARP_EM,
circle presses): items came out for kinds 1, 4, 5, 6, 11-17, 20-22, 25-28, 30, 34 (Rathian, Rathalos, Kut-Ku, Gypceros, Basarios,
Gravios, Khezu, Plesioth, Diablos/Monoblos, the raptors, Aptonoth, Velocidrome family, Giaprey ...; item names plausible for the
monster). Nothing came out for 2 (Fatalis: carved through its own pick points in round 21), 3, 7 (Lao-Shan), 8, 9, 19, 23, 24, 29, 31:
the dead monster there never got a carve point (small monsters without a carve table, or the sweep's warp missed); not investigated.

Not tested / still open: selling from the pouch at the house box ("持ち物を売る" works as a smoke test: +2z for a stored herb),
"持ち物を整理する" (no visible effect in the smoke run), the trader's buy/sell variants beyond 71 -> 77 and gifts, shock trap
(see above), a quest that actually asks for a capture.

### Owner's play-session bugs (agent C, 7 Oct 2026)
- **Test runs vs the player's logs.** rt_log.c: a run with `--input`, `--shot`, `--headless`, `--audio-dump` or any `RT_*` variable in
  the environment (covers every tools/test_*.sh / .py, the co-op, online and Wine paths) logs to `build/test_logs`, never to
  `~/.local/share/mh1pc/logs`; `MH1_LOG_DIR` still wins (test_log.sh sets it). The 20-newest rotation had deleted real play logs.
  Not done: the memory card folder has no such default (tests set MH1_SAVE_DIR themselves).
- **Map item did nothing.** disp_whole_map (menu_disp_nm.c, the HUD minimap and the full map) drew the explored-area window when the hunter
  HAD item 142; the near-match had the test inverted (asm: `beqz` after Pl_item_num_ck(0x8E) jumps to the explored window, so only
  WITHOUT the map). Fixed; test `map_item` (HUD corner has map lines with the item, none without).
- **Walking during cutscenes.** The PS2's gate is f_framec.c `move()`: `if (game_w.info_stop == 0) player_mv();` (info_stop is set by
  the event demos, EvDemoMove). The PC's rt_player_tick always ran pl_move(). Now skipped while info_stop is set (RT_DEMO_FREE=1 = old
  behaviour, test aid). Quest 131's stage-39 tutorial demo holds the hunter for 509 ticks, quest 154's Cephadrome demo for 746;
  test `demo_input`. test_all_quests: the boss hunts start their warp-to-monster aid at tick 900 (the aid itself used to put the hunter
  next to a Cephadrome that never surfaced because he stood frozen under the sand).
- **Camera after the Rathalos demo (quest 139).** Reproduced the quest (21 -> 39 -> 38 -> 33, demo camera 16 from tick 126 to 849, the event
  releases at 849): the game camera comes back 5 ticks after the demo ends (distance to the hunter 510, normal follow camera, stage 33
  camera data loaded) and stays sane while walking/turning; no broken camera found. A likely cause of the owner's report was the
  walking-in-demo bug above (the hunter left the demo's place while the camera script ran). Not reproduced otherwise: if it still
  happens, note the tick and what the screen shows. RT_CAM_TRACE now also prints the demo slot's `no` / `state`; RT_PL_TRACE ends with `is <info_stop>`.
- **Vine climbing: not located.** The hunter's wall actions are acts 0x27-0x2C (wall hug, kabe_*), the ledge climb-ups 0x15/0x1B/0x1E
  (pl_mv030 moves him 55 units and snaps to the ground above in one tick). No stage unique spot or set object for ivy/vines was found
  (spot kinds present: 2 fishing, 3 box, 4, 16 bed, 17, 21 delivery, 24, 25 bench) and walking into walls on stages 33-40 never
  started a climb. Needs the stage / quest where the owner saw it.

## Per-kind materials: enemy_trans' clays and materials (agent D, 8 Oct 2026)

Started from the owner's PS2 footage: the Velociprey (16) has a smaller crest of a different shape than the Velocidrome (27),
the PC drew the drome's on both. em16_amh (16 and 27; em13 for 13/28, em30 for 30/31) holds BOTH crests and claw sets as
materials 4 (small: prey) and 5 (big: drome) over each other. Not bones or motions (em16_tbl has no scale channel at all).

What the PS2 does (enemy_trans 0x168B10, per clay i of the model):
- clay i is drawn only while EMW+0x4E6+i is set (em_init sets all 32; em29 keeps one of its five variant clays; kind 3
  draws only clay EMW+0x11). Clay 1 of kinds 1/6/8/11/14/15/17/21/22/26 is the tail: em20_init clears its flag and
  eft09 draws it (with the body's tail bones until cut, then where it fell); the PC keeps drawing it with the body.
- then a per-kind material function on the clay's materials (CLAY+8 = the AMO part's 0x50000 list, index m):
  em09_material_sub (game 0x5ACA60) for 9/18/23, em20_material_sub (0x5FCBB0, game C) for 20, em_material_sub (main
  0x10CEA0, asm only) for the rest. Each writes the material's alpha (flMATERIAL +0x10, the diffuse alpha the VU1
  program multiplies into the directional lights) = EMW+0x798, then 0 for the materials not shown.
- what they switch: cut-surface caps of the body and the cut tail (shown once EMW+0x948 bit 0 = tail cut is set;
  an earlier version of this note called it "asleep"), part-break variants by
  hagi[k].cnt (EMW 0x30A + 8k: Rathian 1, Rathalos 11, Lao-Shan 7, Gravios 17, Basarios 22; Diablos / Monoblos 14/26
  horns by EX+0x1A), blinking eyes (raptors: every 98
  ticks; 19/24: every 9), mouths by motion (19/24), Fatalis (2) damage materials by EX+0x52 (its hit points:
  thresholds 0x6400 / 0x4B00 / 0x3200 / 0x1900; below 0x1900 materials m1/m5 of clay 3 swap to texture APX 2), the
  Cephadrome (8, shares em08 with Cephalos 34) gets diffuse (0.396, 0.376, 0.255) on every material, Monoblos (26)
  clay 0 m0 reddens with EX+0x1B / 60, Gypceros (20) per em20_material_sub.
- EMW+0x798 < 1 (the AI counts it down after carving: em04b, em21_r10, em15 ...) fades the whole monster out (alpha
  reference 0 while fading).

PC: rt_em_materials (src/pc/rt/rt_em.c) ports em_material_sub (all cases enemy_trans reaches) and em09_material_sub,
and runs the game's em20_material_sub on a stand-in material table; it returns per material alpha / colour / texture
and whether the clay is drawn. viewer.c draw_model_attr_em (every monster, also the host Rathian) draws each clay in
one pass per distinct material state, the others hidden with GFX_RS_BATCH_HIDE; alpha and colour through
GFX_RS_FADE_COLOR, texture through GFX_RS_BATCH_TEX (GL, NV2A CPU and GPU-skinned paths). A colour override that
covers the whole model (Cephadrome) scales the directional light colours instead, as the VU1 MATERIAL block does
(em_model_col); a partial one (Monoblos) multiplies the vertex colour (approximation: also scales ambient).
Not ported: em_alpha_clay (the clays enemy_trans draws with alpha reference 0 instead of 0xC0; the PC uses one
alpha reference, 0x40, for every host draw, and the scale of state 0x60 against the PC's texture alpha was not
checked, so it was left alone).

Tail cutting (8 Oct 2026). The tailed monsters (kinds 1, 6, 8, 11, 14, 15, 17, 21, 22, 26; all have 48 bones and a
second bone tree 45-47 that carries only clay 1, modelled around the origin):
- the cut: Em_Dmg_Sys breaks hagi part 8 -> x957 -> result 0xB -> the damage action that ends in em_tail_off_sub
  (x948 |= 1, clay flag 1 cleared again, tail_off). eft09_m also calls tail_off itself: kinds 1/11/14/26 in mode 4
  sub 0xF, 17/22 in mode 4 sub 0x11, the others at frame 300 of motion 0x429. tail_off (game C, eft09.c) sets the
  effect's arg = 1, pos = node 43's world position, ang = its yaw, and makes a carving point
  (Em_tail_hagi_point_set; eft09_m moves it to pos every tick until it is carved out). All of this is game C that
  already ran on the PC; what was missing was the drawing.
- before the cut, eft09_t gives the tail tree the body's tail nodes: bones 45, 46 = node 43, bone 47 = node 44.
  fl_model.c attach_tail_tip does that now; the old host version (tree moved with bone 44 from its bind place)
  stood the tail tip up above the Rathian's back (free-play shot, stage 4).
- after the cut, eft09_t draws clay 1 alone, the tree in its bind pose under Scale(EMW+0xB8) * RotY(ang + 0x4000)
  * Trans(pos), while the monster is active (x01) and the effect is on this stage; the body no longer draws clay 1
  (its flag is clear and the effect's arg is set). PC: rt_em_cut_tail (rt_em.c), fl_skel_cut_tail (fl_model.c),
  draw_cut_tail (viewer.c: poses clay 1 alone with those bones, world identity, its materials as enemy_trans).
- not done: nothing moves the cut tail after the cut on the PS2 either (no fall: it stays at node 43's height of
  that moment, about 117 above the ground for the Rathian in her nest, which looks like lying on the ground).
- checked: quest 10 with the poke below: the cut at tick 200, the body ends in a stump, the cut tail lies by the
  nest with its cut-surface cap; the hunter warped there carves 2 items (Rathian scale 183, item 179). Rathalos
  (11, quest 170), Diablos (14, 174) and Gravios (17, 172) cut the same way (trace).
- which kinds can be cut (em_dur_tbl, main 0x356CF0): kinds 1, 11, 14, 17, 22, 26 have part 8 (the tail, durability
  140-200) and damage kind x953 = 9, so a break of part 8 sets x957 and the next hit runs the cut action. Kinds 6, 8,
  15, 21 have no part 8 (-1, x953 = 8) and no tail entry in em_hagi_type_tbl: damage never cuts them; eft09_m would
  cut them at frame 300 of motion 0x429, which their AI does not play (no em_char_set 0x41). So in this version
  Kut-Ku, Cephadrome, Khezu and Plesioth keep their tails.
- Basarios (22): asleep in its rock disguise (act 0/22) a hit only wakes it and uses up x957 without the cut
  action; awake, the same poke cuts (4/4 -> 4/17, tail_off). Real attacks also cut it on the PC: hunter beside
  joint 43 (RT_PL_WARP_JOINT=43), RT_PL_AIM, RT_DMG_MUL=10, part 8 went 140 -> 0 in about 4400 ticks and the
  tail came off (game-camera shot). Monoblos (26): the poke does start the cut action (4/4 -> 4/15) but each time
  it had moved to stage 52 while the hunter was on 53, so the cut tail (drawn only on its own stage) was not seen.
- kinds 6, 8, 15, 21 checked with RT_EM_TAILOFF (tail_off forced): body without clay 1, the cut tail drawn at the
  tail's place (Kut-Ku and Khezu on the ground, Plesioth's in the water, Cephadrome's under the sand where it
  swims); no carving point (no table entry), cut-surface caps hidden (x948 is set only by em_tail_off_sub).
- fixed with it: the cut tail took its materials only when its clay was "drawn"; rt_em_materials now sets them
  for clay 1 even when the body does not draw it.
- test_activities `tail_cut`: the Rathian's cut, body without its tail, a carving point, carving it in a second
  run; and the Basarios' cut (woken first).

Test aids: `RT_EM_ALL_MATS=1` (draw every clay and material, the old behaviour), `RT_EM_MAT_TRACE=1` (each new hidden
mask per kind and part, clays not drawn, the light colour), `RT_EM_POKE="kind:offset:value[:2|4][@tick];..."` (write a
byte / s16 / 32 bits of every monster of that kind each game tick before its AI, or only at that player tick:
broken parts, Fatalis hit points, the 0x798 fade; a tail cut is `1:0x957:1@200;1:0x38D:1@200`),
`RT_CAM_EM=kKIND,dist,height,yaw` (the free camera on the first monster of a kind; `tKIND,...` on its cut tail). `RT_EM_TAILOFF="kind@tick"` (eft09
tail_off forced at that tick), `RT_PL_WARP_JOINT=n` (with RT_PL_WARP_EM: next to joint n of the living target). Check: test_activities
`em_materials` (raptor crests, Cephadrome colour, one rock variant of 29, Rathian broken parts, Fatalis damage).
Verified with free-camera shots, new against RT_EM_ALL_MATS (not committed): Velociprey small crest and dark claws;
the rock monster (29, quest 173) shows one grey rock instead of five overlapping coloured variants; red cut-surface
caps gone from the Cephadrome's tail and the Plesioth's body; the Cephadrome darker; a Kut-Ku at 0x798 = 0.5 is half
transparent; Rathian / Rathalos / Diablos / Khezu / Gravios / Monoblos look as before at full health (their variants
overlap exactly), their masks per part are in the trace. Fatalis damage was checked by trace only (dark stage).

### move() gates audit (agent C, 8 Oct 2026)
Host `sim_tick` (viewer.c) + `rt_game_move` stand in for f_framec.c `move()` (not called). Compared step by step:
- `player_mv` gated by `info_stop`: now matched (above). `item_check` / `body_hit` are the other two steps gated by it.
- Quest timer (`Quest_timer_calc`), monsters (each em AI tests `info_stop` itself), set objects (set13), stage draw, Pit_mv (returns while
  `game_w+0x21F`), bgm: all game C, so they follow the flag already. Checked in a run: the quest timer holds still during quest 131's demo
  (added to `demo_input`).
- `item_check`/`move_item` (dropped-item pool) stay host stand-ins. `body_hit` (hunter-hunter and monster-monster push-apart) is now
  called every tick while info_stop == 0 (viewer.c sim_tick; `RT_BODY_HIT=0` opts out). It is cheap (0.1 s per 2000 ticks: the slow
  runs seen earlier were machine load from other agents). Test changes it needed: `RT_EM_PIN` takes a timeline ("0:x,z;300:x,z");
  pitfall / tranq put the Rathian on the trap at tick 300 (her own walk now depends on the hunter distance body_hit records at
  +0x3AC); carve_small accepts either Aptonoth carve (18 / 227); test_urgent's reward-screen script is cross / ddown / circle /
  circle every 70 ticks (reward lists change with the RNG, grids of 8); test_all_quests: swarm hunts of 20+ get an RT_PL_SLAY fallback at
  tick 15000; test_coop_hunt `multi` runs 300 s with RT_DMG_MUL=160 (each intro demo holds the hunters ~25 s). The warp-to-monster aid
  already stands outside the first body sphere (radius + 40); a search for a push-free spot made the quest loop fail (carves out of
  reach), so it stays as it was.
- Pause menu / quest end: the PS2 does not stop `move()` for the pit menu (only the sw input zeroing in sw_set_sub, `Cockpit_menu_chk`),
  and the PC runs the same game C there; the quest-end states are game modes (game3/5), driven by rt_flow.

### Quest 154 hang and the stand-in sweep (agent C, 8 Oct 2026)
- **Quest 154 (Cephadrome) never cleared after body_hit.** Not a hang (the loop ran): the dead Cephadrome sat in `em_die02` sub 1 forever. Its
  test `pos[1] < x7E4 - x7E0` compared the float y (-401.918365) with the x87 80-bit result (-401.9183578): one rounding off, so it never
  counted as "out of the sand". The PS2 FPU is single precision. Game C (and the host objects) now build with `-msse2 -mfpmath=sse`
  (x86 gcc only; ARM / Windows / Xbox unchanged): the fight passes at every warp offset (`RT_WARP_R=n` aid), where it passed or failed by luck before.
  Other places with the same excess-precision comparison are fixed by the same flag. `RT_SLAY_DEBUG`/`RT_PL_SLAY armed ...` prints help find a boss that
  is dead already.
- **Sweep** (`RT_STANDIN_FILE=path` appends every first-called stand-in; `tools/sweep_random.py [secs]` runs 150 s of random pad input in ten
  quests plus the village at each shop, the forge, the Elder and the house; plus the whole test set). Unique stand-ins seen, by what a player notices:
  1. `flPADShockSet` (controller rumble from vib_set / vib_set_pl: every hit, roar, quake) -> now SDL rumble (pad_sdl.c; vib_tbl strength 1-7, frames).
  2. `sound_call_005C48C0` (66 runs: village NPC sound requests at fixed frames, lb_vs01) -> lobby/b/lbsnd01.c linked (em_frame_check adaptor).
  3. `func_63AFA0` = Tutorial_flag_set (a demo adds "the Elder's teaching", message + sound) -> alias in rt_overlay.c.
  4. `em01_local_area_move_init` (Rathian / Rathalos per-stage stay and run-away timers: when they change area) -> PICKed from em_modechg.c.
  5. Not visible on the PC, left: flAdjustScreen, flCalcTrans(SI), view_reset, set_viewproj, init_*_work, clr_*_work, ot_init, round_init, stage_free,
     load_*, FlushCache, flSndPack*, flSndPortStop, flFlip, setBGcolor, View_init (host owns screen, loading, draw order), em_effect_pull (the host draws monsters),
     lb_member_*Check and text_lobby_trans_ot3_o (online lobby), Equip_moji_color_rare_i (chat list colour), flExp, ADXM_Lock/Unlock, edit_create_model,
     apiask_28_OpenDic, Disp_NowLoading2, release_texture, flReleaseMotionSetHandle, all_model_free, armor_model_free.
  docs/agents/targets.md did not exist in this tree: none of the above are in a claimed file as far as I could see (re-check after merging).
