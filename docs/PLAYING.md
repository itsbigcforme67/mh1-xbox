# Playing Monster Hunter and Monster Hunter G on PC and original Xbox

This project runs the Japanese PlayStation 2 games **Monster Hunter** (2004) and **Monster Hunter G** (2005) on a
Linux PC, a Windows PC and the original Xbox. It is a fan-made port that uses code rebuilt (decompiled) from the
games. It contains none of Capcom's game data, so **you need your own copy of the Japanese PS2 disc** (product
code SLPM-65495 for Monster Hunter, SLPM-65869 for Monster Hunter G). Monster Hunter is (c) CAPCOM. This project
is not affiliated with Capcom. Everything below applies to both games unless it says "G"; section "Monster Hunter G"
lists what differs.

It is a work in progress: the whole single-player game is playable (village, all offline quests, shops, forge,
fishing, carving, the star-level progression), but expect rough edges (see "Known issues").

## What you need

* The disc as an **.iso file** (a dump of your own disc, 2048-byte sectors), or a folder holding the files copied
  off it. Only the Japanese releases of Monster Hunter and Monster Hunter G work: the US/EU Monster Hunter and the PSP
  versions are different games. The tool checks the files by size and checksum and tells you if something is wrong.
  Monster Hunter G's disc (SLPM-65869) is accepted as well; `./mh1` tells the two apart by itself.
* About 1.5 GB of free disk space (the game files are about 925 MB) plus room for the build.
* To build the game from this repository you need a Linux computer (Ubuntu or Debian is what it is tested on).
  Windows and Xbox versions are built on that Linux machine and copied over.
* Per target:
  * **Linux**: `gcc`, `make`, the SDL2 development files and 32-bit support (the game code is 32-bit):
    `sudo apt install build-essential libsdl2-dev gcc-multilib libc6:i386 libsdl2-2.0-0:i386 libgl1:i386`
    (on Ubuntu/Debian you may first need `sudo dpkg --add-architecture i386 && sudo apt update`).
    Without root, `./mh1 linux DISC --setup` fetches the compiler side for you; the three `:i386` libraries must
    still be installed.
  * **Windows**: nothing on the Windows PC. On the Linux machine, `./mh1 windows DISC --setup` downloads the
    cross-compiler (about 100 MB, into `~/mh1win`). The result runs on Windows 7 or newer (32-bit program).
  * **Xbox**: a modded original Xbox with a dashboard that can start games from the hard disk (for example
    Evolution X; the tested one) and an FTP server, and the Xbox SDK **nxdk** on the Linux machine (it is large and
    needs system packages: see docs/xbox.md, "Toolchain"). The Xbox build also needs the Linux requirements above.

## Make the game: one command

Run it in the repository folder. `DISC` is your .iso (or the folder with the disc's files).

    ./mh1 linux   DISC          # result: dist/linux/     play with  dist/linux/play.sh
    ./mh1 windows DISC          # result: dist/windows/   copy the folder to the PC, run play.bat in it
    ./mh1 xbox    DISC          # result: dist/xbox/MH1/  copy to the Xbox as E:\Games\MH1
    ./mh1 check   DISC          # only check that the disc is the right one

With a Monster Hunter G disc the same commands build G instead: `dist/linux/MHG/` (play with `play.sh`),
`dist/windows/MHG/` (`play.bat`), `dist/xbox/MHG/` (see "Monster Hunter G" below). `--online` is not available
for G.

What it does, in order: checks the disc (a wrong region or a damaged dump is reported and nothing is built),
checks that the build tools are there (and says what to install if not), builds the game, copies the needed disc
files (`AFS_DATA.AFS`, `AFS00.AFS`, `AFS01.AFS`, `SLPM_654.95`, about 925 MB) next to it. The first build takes
roughly 10 to 40 minutes; a second run is much faster. Options: `--out FOLDER` (another output folder),
`--online` (also build the co-op / online version), `--setup` (download missing build tools where possible).
The `dist/` folder holds a copy of your disc files: do not upload or share it.

### Linux

Run `dist/linux/play.sh`. It starts at power-on: logos, opening movie, title screen, then **new hunter**
(character creation) or **continue**. Extra options can be added: `./play.sh --fullscreen`, `--size 1280x720`,
`--widescreen`. The first run opens a window; Esc quits.

### Windows

Copy the whole `dist/windows` folder anywhere you can write to, and double-click **play.bat**. `play.bat quest`
jumps into a Rathian hunt, `play.bat village` into the village. The package is a test build and untested on real
Windows so far (see Known issues). Windows may warn about an unsigned program. If the game stops with an error,
run `bug_report.bat`.

You can also build without the data and let the program install it: drag your .iso onto `mhview.exe` once and
it copies the files out of the image itself.

### Xbox

The recommended route is the loader's HTTP install (step 2, "If the FTP copy keeps stalling", works the same for G
and is the one the owner's console needed): FTP is only used once for the small loader, the big files come over HTTP
with resume and checksums. Copying every file by FTP is the fallback.

1. **Where the files go.** On the Xbox hard disk, in `E:\Games\MH1`, exactly this:

       E:\Games\MH1\default.xbe
       E:\Games\MH1\data\AFS_DATA.AFS   (116 MB)
       E:\Games\MH1\data\AFS00.AFS      (740 MB)
       E:\Games\MH1\data\AFS01.AFS      (20 MB)
       E:\Games\MH1\data\SLPM_654.95    (5.4 MB)

   That is the content of `dist/xbox/MH1` (the game also finds the data folder next to the XBE, however it was
   started). Your Xbox needs an FTP server running (Evolution X and most softmod dashboards have one: look for
   its address on the dashboard's network or system page, and its login in its settings; you type that login, the
   scripts never know it).
2. **Copy them.** Either use any FTP program (FileZilla etc.: connect, open `E:`, make `Games\MH1`, drop the
   folder's contents in), or run `tools/xbox_install.sh XBOX_IP dist/xbox/MH1` on the Linux machine: it asks for
   the login, copies each file with curl, skips files that are already complete and can simply be run again if it
   is interrupted. Some dashboards' FTP servers need active mode (the script's default; `XBOX_PASSIVE=1` switches).
   The data is 925 MB: a home Wi-Fi link can take most of an hour. Copy `default.xbe` last or first, it does not
   matter.
   **Recommended: the loader's HTTP install** (Evolution X's FTP server often drops big files part way and cannot
   resume them, so FTP is the fallback for the big files): copy only the small network loader by FTP and let it fetch the data from your computer over
   HTTP instead, with resume and checksums:
   - `tools/xbox_upload.sh --loader` (set `XBOX_IP=` to your Xbox's address first): puts one small loader in
     `E:\Games\MH1` (it shows as **Monster Hunter**) and one in `E:\Games\MHG` (**Monster Hunter G**), each with a
     `loader.txt` holding this computer's address. You only do this once (again only if your computer's address
     changes);
   - `tools/xbox_serve.py --data dist/xbox/MH1` (or `--data dist/xbox/MHG` for Monster Hunter G, or both
     `--data` options at once) and leave it running; your firewall must let the Xbox reach port 8765;
   - start **Monster Hunter** (or **Monster Hunter G**) on the Xbox. It copies that game's files into its own
     folder (`E:\Games\MH1\data` or `E:\Games\MHG\data`, the game itself as `mh1.xbe` / `mhg.xbe`), checks
     every 4 MB piece, shows the progress, and starts the game when done. If the copy stops, start it again: it
     goes on from where it stopped.
   Afterwards each start of Monster Hunter or Monster Hunter G asks the computer (when `tools/xbox_serve.py` runs,
   with or without `--data`) for a newer build of that game, updates it and starts it; without the computer it
   simply starts the copy already on the disk. Leaving a game goes back through its loader (which sends the
   game's log to the computer) to the dashboard.
   Check afterwards that the sizes match the list above. The game itself checks at start-up: if a file is missing
   or has the wrong size (a cut-short copy) it shows which one, says where the files belong and returns to the
   dashboard after ten seconds; fix the copy and start again. (Only the sizes are checked there; the checksum check of
   the disc was done by `./mh1` on the Linux machine.)
3. On the Xbox dashboard open the games list ("Play Games from Hard Drive" or similar) and start **Monster
   Hunter**.
4. Hold **Back + Start** for 2 seconds to leave the game (a log is shown for a few seconds first).

The Xbox build has no settings menu or window options. It has been tested on one console and in the xemu emulator,
not on every model.

## Controls

The game keeps the PS2 controls. Left stick moves (push lightly to walk), the **right stick draws and attacks**
(the direction of the flick picks the attack), cross rolls, R1 guards, circle sheathes the weapon, square uses an
item, the d-pad turns and zooms the camera and L1 resets it behind you. In menus **circle confirms and cross
cancels**, as on the Japanese PS2 (the settings menu can swap this).

**Keyboard** (PC)

| key | PS2 button | | key | PS2 button |
|---|---|---|---|---|
| W A S D | left stick | | K | cross (roll, cancel) |
| arrow keys | right stick (attack) | | L | circle (sheathe, confirm) |
| J | square (use item) | | I | triangle |
| Q | L1 (camera reset) | | E | R1 (guard) |
| Z / C | L2 / R2 | | T F G H | d-pad up / left / down / right |
| Enter | Start (pause menu) | | Backspace | Select |

Esc quits the game. **F10** opens the settings menu, **F9** the debug menu for testers (below), **F11** or **Alt+Enter** toggles fullscreen, **F8** reports a
bug (see below). Name entry: type with the keyboard, Enter finishes.

**Gamepad** (PC; any controller SDL knows: Xbox, PlayStation and most others; plug in before or during the game)

| Xbox-style pad | PS2 button |
|---|---|
| A / B / X / Y | cross / circle / square / triangle |
| LB / RB | L1 / R1 |
| LT / RT | L2 / R2 |
| Back / Start | Select / Start |
| sticks, d-pad | sticks, d-pad |

**Original Xbox controller**

| Xbox | PS2 button |
|---|---|
| A / B / X / Y | cross / circle / square / triangle |
| White / Black | L1 / R1 |
| left / right trigger | L2 / R2 |
| Back / Start | Select / Start |
| stick clicks | L3 / R3 |
| d-pad, sticks | d-pad, sticks |

Guard is R1 = Black, which is a small button; if that feels bad, tell us (it is easy to swap on request).

Xbox shortcuts (hold **Back**, then press the second button): **Back + Black** reports a bug (below), Back + L3 the
settings, Back + R3 the testers' debug menu, Back + Start held 2 seconds leaves the game.

## Settings (PC)

Press **F10** at any time. The game pauses while the menu is open. Rows: windowed / fullscreen, 4:3 or 16:9
(widescreen shows more to the sides; menus and the HUD keep their 4:3 layout), vsync, frame-rate cap, anti-aliasing
(next start), anisotropic filtering, 2D art filter (smooth / sharp / nearest), text language (Japanese only unless
you supply a translation table, see docs/english.md), button layout (circle or cross confirms), music and effects
volume. The choices are saved in `mh1pc.ini` (Linux `~/.local/share/mh1pc/mh1pc.ini`, Windows
`%APPDATA%\mh1pc\mh1pc.ini`), a plain text file; the same options exist as command-line flags (docs/pc.md,
"Display options"). The game logic always runs at its original 30 steps per second, whatever the frame rate.

Skip intros is in the PC settings menu (F10) too: `skip_intros = 1` in `mh1pc.ini` / `mhgpc.ini`.

## Settings (Xbox)

Hold **Back** and click the **left stick** (Back + L3) at any time; the title screen reminds you. The game pauses
while the menu is open. Rows: text language (English only when you put a translation table at
`E:\Games\MH1\text\en.txt`, MHG: `E:\Games\MHG\text\en.txt`; applies at the next start), 4:3 or 16:9 (for a
widescreen TV set to stretch the picture; menus stay 4:3, the HUD moves to the screen edges), the HUD (original or
the prototype gunner HUD for bowgun users), which button confirms (B as in Japan, or A), music and effects volume,
rumble on/off, the testers' debug menu on/off, and **Skip intros** (from the next start, and at the debug menu's
soft reset, power-on goes straight to the title screen: no logos, warnings or opening movie; the memory card is still
read first). D-pad moves, left/right changes, B or Start closes. The choices are
saved in `settings.ini` in the game's folder on E: and read at every start; until you change the aspect ratio there,
the dashboard's widescreen setting decides it.

## Debug menu for testers

Testers can jump straight to any quest, area or screen without scripts or settings. Press **Back + R3** (click the
right stick while holding Back) on any controller, original Xbox pad included, or **F9** on the PC keyboard. The game
pauses while the menu is open and keeps the picture underneath. Press the same again, **Start**, or circle on the first
page to close it.

Moving around: d-pad or left stick up/down picks a row, left/right changes a value (in long lists it jumps 10 rows,
L1/R1 jump 100), **cross** chooses, **circle** goes back, **square** empties an inventory slot. On the PC the arrow keys,
PgUp/PgDn, Enter, Backspace/Esc, Delete and the mouse (wheel, click, right click = back) work too. The names are shown
as the game writes them (Japanese, or English with a translation table).

| row | what it does |
|---|---|
| Start a quest | every quest on the disc: number, the Elder's star level (MH1; G shows the quest group), title and goal. The quest starts at once on its base camp, as if accepted at the counter |
| Area warp | (in a quest) every area the quest file links, through the game's own area change |
| Go to the village | with or without the first-visit intro |
| Title screen | back to the logos and title (like a reset) |
| Item pouch / Item box | pick an item for each slot by name, left/right sets the count. In a quest the pouch is the one you carry now |
| Money, Equipment | money in steps of 1000 (L1/R1: 100000); weapon (blade or bowgun) and armour by name. New equipment shows from the next area or quest |
| Hunter rank, Mark all offline quests cleared | MH1 only |
| Can't faint, One-hit kills, Damage x1..x50 | for fights |
| Set monster HP, Kill the monsters in this area, Warp to the monster | (in a quest) for the monsters of the area you are in |
| Quest timer | stop the clock (free roaming) |
| Show hit volumes | the monsters' body spheres (red) and attack volumes (yellow) |
| Frame step | the game advances one step per R3 click (or F7 / . on the PC); hold R3 to run |
| Free camera | PC: fly with W A S D, Space / C, Shift faster, look with the mouse; the controller still moves the hunter |

The menu is on in test builds. A release build (made with `MH1_RELEASE=1`) has it off. To turn it on or off by hand:
on the PC put `debug_menu = 1` (or `0`) in `mh1pc.ini`, or start with `--debug-menu` / `--no-debug-menu`; on the Xbox
put `RT_DEBUG_MENU=1` (or `0`) in `data\env.txt`. Changes you make with it are real: a save made afterwards keeps
them. Use a copy of your memory card for testing.

## Reporting a bug (PC and Xbox)

When you see something broken (a glitch, a wrong texture, a monster stuck in a wall), report it right there:

**PC:** press **F8** (or Back/View + Start on a controller). The game freezes. Click each broken thing (it lights up
and gets a number), or drag a box around a broken area; right click undoes the last pick. Type what is wrong, then
**Enter** saves the report; **Esc** or F8 cancels. The report folder (screenshots, game state, your inputs) is saved
under the game's data folder (`.../mh1pc/reports`); nothing is sent anywhere. Frozen with the controller, the pad
works as on the Xbox below.

**Xbox:** hold **Back** and press **Black** (the small black button). The game freezes and a panel at the top shows
the keys:

| button | what it does |
|---|---|
| right stick (or left stick) | moves the white cross (the pointer); a light push moves it slowly; the d-pad moves it one dot |
| **A** | picks the thing under the cross (it lights up and gets a number) |
| hold **A** and move | draws a box: everything inside is picked |
| **X** | undoes the last pick |
| **Y** | opens a keyboard on the screen to type a note (see below) |
| **White** | hides / shows the panel (if it covers what you want to pick) |
| **Start** | saves the report and the game goes on ("Bug report saved" at the bottom for a few seconds) |
| **B** (or Back + Black again) | cancels: nothing is saved |

The on-screen keyboard: d-pad or left stick moves, **A** types the key, **X** deletes a letter, **Y** is a space,
"Shift" switches to capitals and signs, **B** or "Done" closes it; Start still saves. A note is optional, but one
line ("the river flickers when I stand here") helps a lot.

A **USB keyboard or mouse** works too when one is plugged into a pad port (through a pad-to-USB adapter) before the
game starts: type the note directly, Enter saves, Esc cancels, F8 freezes; the mouse moves the pointer, left click
picks (drag = box), right click undoes. (Written but not tried yet with a real keyboard; tell us if it does nothing.)

**Where Xbox reports go:** each one is saved on the Xbox in `E:\Games\MH1\reports\report_<date>_<time>`
(Monster Hunter G: `E:\Games\MHG\reports`). When you leave the game (Back + Start for 2 s) its loader sends the new
reports to your computer together with the log, as long as `tools/xbox_serve.py` is running there; otherwise at the next
start with the computer on. They land in `build/xbox/reports/<time>-mh1` (or `-mhg`) on the computer:
`screenshot.png`, `annotated.png` (your picks marked), `report.json` (your note, what you picked, the game state),
`log_tail.txt`, `input.txt`. `python3 tools/show_report.py build/xbox/reports/<folder>` prints one readably. Reports
already sent stay on the Xbox (with a `sent.txt` in the folder); delete old ones by FTP when you like. The online
build of Monster Hunter sends a report at once when you are connected.

**Once:** the loaders on the Xbox must be the new ones that know about reports: run `tools/xbox_upload.sh --loader`
again (as when you installed them).

## Saves

The game saves to a virtual PS2 memory card, the same way as the original: in your house's **bed** in the village
and after quests; load with CONTINUE.

* Linux: `~/.local/share/mh1pc/memcard0/` (set `MH1_SAVE_DIR` to change it)
* Windows: `%APPDATA%\mh1pc\memcard0\`
* Xbox: `E:\UDATA\4D480001\` (shows under "Monster Hunter" in the dashboard's save list, without an icon)

Back up that folder if you care about the hunter.

**Bringing a real PS2 save**: drag the file onto the game (PC: onto `mhview` / `mhview.exe` / `play.bat`, or onto the
open window), or run `mhview --import-save FILE` (on Linux `dist/linux/mhview --import-save FILE`). Accepted:
.psu, .max, .cbs, .sps, .xps and whole memory card images (.ps2, .mcd, .mc2, .bin; the Monster Hunter save is picked
out). The save is checked first; a damaged or foreign file is refused, and an existing save is copied to a
`.backups` folder first. Then choose CONTINUE. To take a hunter back to a PS2 or PCSX2: `mhview --export-save
MyHunter.psu` (also .max, .cbs, .sps, .xps, or .ps2 for a new memory card image). Importing on the Xbox is not
supported yet. A real PS2 save has not been tested by us (only generated ones), so keep your original.

## Monster Hunter G

G is the project's main target and a work in progress. `./mh1 linux|windows|xbox DISC` with the G disc builds it
(first build 10 to 40 minutes), checks the disc's files by size and checksum, and puts them next to the program:

    dist/linux/MHG/play.sh        dist/windows/MHG/play.bat        dist/xbox/MHG/ (E:\Games\MHG, below)

Monster Hunter G on Linux, step by step (the same as Monster Hunter 1, with the G disc):

1. `./mh1 linux /path/to/MonsterHunterG.iso` (or a folder with the disc's files). It checks the disc, builds G
   (`tools/build_pc_mhg.sh`) and makes `dist/linux/MHG/` with `mhview_g`, `play.sh`, `PLAYER.txt`, this guide and
   `data/` (your disc's `AFS_DATA.AFS`, `AFS00.AFS`, `AFS01.AFS`, `SLPM_658.69`, about 1.1 GB).
2. `dist/linux/MHG/play.sh` from power-on (logos, title, NEW GAME / CONTINUE, Kokoto village); `play.sh quest`
   straight into G quest 10 (Rathian), `play.sh easy` the same with a hunter who cannot faint. Extra options go to
   the game (`play.sh --fullscreen`, `--size 1280x720`, ...). You can move the folder anywhere.
3. Without the `data/` folder (e.g. you copied only the program), give the `.iso` once: `play.sh /path/to/G.iso`
   installs its files into `~/.local/share/mhgpc/data` and starts the game; later starts find them there.

Monster Hunter G on Windows: `./mh1 windows DISC` (cross-built on Linux, as for Monster Hunter 1; `--setup` fetches
the cross-compiler) makes `dist/windows/MHG/` with `mhview_g.exe`, `SDL2.dll`, `play.bat`, `bug_report.bat`,
`README.txt`, `PLAYER.txt` and `data/`. Copy that folder to the Windows PC and run `play.bat` (`play.bat quest`,
`play.bat easy` as above). The program alone (the zip of `tools/package_win.sh` with `VERSION=mhg`) needs the disc
once: drag the G `.iso` onto `mhview_g.exe` or `play.bat`; it copies the files into `data` next to the program, or
`%APPDATA%\mhgpc\data` when that folder is not writable.

G keeps its own files, apart from Monster Hunter 1's, so both games can be installed at once:

| | Linux | Windows |
|---|---|---|
| saves (memory card folder) | `~/.local/share/mhgpc/memcard0/` | `%APPDATA%\mhgpc\memcard0\` |
| settings (F10 menu, window size) | `~/.local/share/mhgpc/mhgpc.ini` | `%APPDATA%\mhgpc\mhgpc.ini` |
| debug logs, F8 bug reports | `~/.local/share/mhgpc/logs/`, `reports/` | `%APPDATA%\mhgpc\logs\`, `reports\` |
| data installed from an ISO | `~/.local/share/mhgpc/data/` | `data\` next to `mhview_g.exe`, else `%APPDATA%\mhgpc\data\` |

`MH1_SAVE_DIR` moves the save folder (settings and logs follow it). The window is titled "Monster Hunter G".

**PLAYER.txt** (keep it next to the program): G is a work in progress that test scripts and agents run all the
time, so the G program opens no window when it is started from the build folder (`build/mhg/pc/mhview_g`); such runs
must make a screenshot (`--shot`) and end. `./mh1` and `tools/package_win.sh` put `PLAYER.txt` next to the program,
and next to that file G opens its window as normal. `MH1_HIDDEN=1` (set by test scripts and agents) keeps even a
player package hidden; `MH1_SHOW=1` shows a window anywhere (the owner's `tools/play_g.sh` sets it). An older G
settings file `mh1pc.ini` in G's folder is renamed to `mhgpc.ini` on the first start.

Xbox: files go to `E:\Games\MHG` (`default.xbe`,
`data\AFS_DATA.AFS` 144 MB, `AFS00.AFS` 884 MB, `AFS01.AFS` 24 MB, `SLPM_658.69` 2 MB); use the loader's HTTP install
with `tools/xbox_serve.py --data dist/xbox/MHG` as described above, FTP as the fallback; G's own loader (shown as
Monster Hunter G, `E:\Games\MHG\default.xbe`) installs G beside MH1, and the game itself is `mhg.xbe` there.
Xbox saves: `E:\UDATA\` under G's own title id.

What works (checked in scripted, hidden runs, not yet by a full human play-through): title, NEW GAME, hunter
creation, saving, CONTINUE, the village (Kokoto) with its shops, forge, item box and the elder's quests, all 98
offline quests and the carts from start to reward, the debug menu and the settings menu (F10 on the PC, Back + L3 on
the Xbox).

What is still rough:
* **Japanese text.** About half is English-capable (official English where it exists, see docs/text_coverage.md);
  G-only weapons, armour, items and NPC lines are Japanese. A translation table can be loaded (docs/english.md).
* The EXTRA row of the game menu is not done; equipment description windows (attack, sharpness bar, skill pages)
  in shops and the box are incomplete; some monsters (Diablos, Gravios) still use Monster Hunter 1's move choice.
* No online play (plaza, gathering hall, co-op) for G.
* Movie gallery unlocks, a few sound effects and the soft keyboard's kanji conversion are missing.
* Xbox frame rate and memory have only been measured in the emulator.

Debug menu (F9 / Back + R3): as above; "Start a quest" lists G's quests by quest group; the Hunter rank rows are
MH1 only. Details of what is open: docs/RELEASE_G.md.

## Playing with friends

**Co-op over a direct connection (up to 4 players, PC)**: build with `--online` (this gives `mhview_online`), then
every player runs `mhview_online DATA_FOLDER --coop`. A small window asks the host for a quest and player count;
the others type the host's address. Each player uses his own hunter. On a home network use the host's LAN address and
allow the program in the firewall. Over the internet the host forwards TCP port 10300 and everyone names the other
side's address on purpose: `mhview_online DATA_FOLDER --allow 203.0.113.7 --coop`. Details: docs/pc.md, "How to play
co-op".

**Your own server (lobby, town, rooms, chat)**: run `python3 tools/server/mh1_server.py` on any machine with
Python 3.8 or newer; it is invite-only (you make an account for each friend) and needs no other software.
Step by step: docs/server.md, "Run your own server". Each player puts the server's address and his account in
`mh1pc.ini` (`online_server`, `online_login`, `online_password`) and chooses the network mode on the title screen.
This talks only to your server. The port does **not** connect to public servers of the original game, and must
not be pointed at them without their operators' permission. The Xbox can join the same server: next section.

### Online on the Xbox (Monster Hunter, not G yet)

The online Xbox game is a separate XBE (`./mh1 xbox DISC --online` puts it in `dist/xbox/MH1-online/default.xbe`;
by hand: `ONLINE=1 tools/build_pc.sh`, then `python3 tools/build_xbox.py --online --gfx nv2a`, giving
`build/xbox/online/nv2a/default.xbe`). It uses the same data folder `E:\Games\MH1\data` as the normal game. It talks
only to the server you give it, the same server the PC version uses (lobby, town, chat, rooms). Status: tested in the
xemu emulator against a server on this laptop (login, town, a PC player in the same town); not yet on a real Xbox.

1. **The server, on the laptop.** Make one account for the Xbox and start the server bound to the laptop's LAN
   address (docs/server.md, "Run your own server"):

       python3 tools/server/mh1_server.py account add --db mh1-server.sqlite3 --note "Xbox"
       python3 tools/server/mh1_server.py serve --bind 192.168.1.105 --lobby-port 10200 --db mh1-server.sqlite3

   `192.168.1.105` is the laptop's address on the home network today; it can change (DHCP): `ip -4 addr` shows the
   current one. The firewall must let the Xbox reach port 10200 (and 10301-10399 for hunts through the relay).
2. **Tell the Xbox the server and the account.** A small text file `settings.ini` with these three lines (the login
   and password that `account add` printed):

       online_server = 192.168.1.105:10200
       online_login = 12345678
       online_password = THE16CHARPASSWRD

   Copy it to the Xbox as `E:\Games\MH1\settings.ini`: `XBOX_IP=<the Xbox's address> tools/xbox_upload.sh --settings
   settings.ini`, or any FTP program. (This replaces the file the settings menu writes; the menu's choices go back
   to their defaults once, and the menu keeps the online lines when it saves.) Without an `online_server` line the
   game tries the laptop named in the loader's `loader.txt`, port 10200.
3. **Start the online XBE through the loader.** Stop the loader server that is running now (Ctrl+C in its terminal),
   then serve the online build instead:

       tools/xbox_serve.py --xbe dist/xbox/MH1-online/default.xbe

   (the MH1 data must already be on the Xbox: if not, add `--data dist/xbox/MH1` once). Start **Monster Hunter** on
   the Xbox: the loader fetches the online XBE and starts it. To go back to the normal game, stop it and start
   `xbox_serve.py` the way you did before. Monster Hunter G is not affected (it has its own loader).
4. **In the game:** on the title screen choose the network mode, as on the PC. The game starts the Xbox's network
   (the dashboard's network settings; DHCP is fine, it waits up to 10 s), connects, and goes through the Capcom ID
   screens into the plaza and the town. PC players on the same server see the Xbox hunter and the other way round.
   The Xbox log (`E:\Games\MH1\mh1log.txt`, sent to the laptop by the loader at its next start) has `net:` and
   `online:` lines that show where it stopped if something fails (no network, the server not reachable, the login
   refused).

### Playing online on a PS2 or in PCSX2 (patch your own disc)

A real PS2 (or PCSX2) can join the same server as the PC version: lobby, town, chat, rooms. **Hunting together does
not work yet for PS2 players** (the PS2 speaks the original in-hunt protocol, which the server does not relay yet:
docs/network.md 3.7). Only the Japanese Monster Hunter disc (SLPM-65495); not G yet.

1. Ask the server's operator for an account (docs/server.md, "Run your own server"): an 8-digit login and a
   16-character password.
2. On the Linux machine, set up the PS2 rebuild of your own disc once (README.md, Phase 1: the PS2 compiler and
   binutils; `tools/rebuild.sh main lobby` must print OK for both).
3. Patch a copy of your disc image (the original is not changed):

       python3 tools/ps2_patch.py "Monster Hunter (Japan).iso" --server 192.168.1.20:10200 --login 12345678 --password YOURPASSWORD

   `--server` is the server's address (or name) and lobby port. The result is `Monster Hunter (Japan) (mh1-server).iso`
   next to the original (`--out` for another name). `--ppf FILE.ppf` writes only a small PPF patch for your .iso
   instead. The patch skips DNAS (Sony's shut-down network check) and replaces the shut-down KDDI login portal by
   your server and account; nothing else in the game changes. The patched image contains your login and your copy
   of the game: do not share it.
4. PCSX2: Settings > Network & HDD: enable the Ethernet adaptor, device type **Sockets** (no administrator rights
   needed), then boot the patched .iso. A real PS2: the patched .iso through OPL (USB, network or HDD) with the
   network adaptor. When the server runs on the same PC as PCSX2, give its **LAN address** (192.168.x.y) in
   `--server`, not 127.0.0.1.
5. In the game: CONTINUE, your hunter, then **街へ行く** (network mode). The first time it asks to make a NET file
   ("はい", slot 1), shows a notice (進む), then on the "ネットワーク接続" screen choose **ネットワーク設定の編集**:
   Sony's network settings open. Add Setting -> memory card slot 1 -> SCE/Ethernet (Network Adaptor) -> PPPoE "Not
   Required" -> IP address **Auto** -> DNS **Auto** -> save -> quit. The game restarts; go to 街へ行く again, load the
   NET file, pick "1.Setting 1": it connects, logs in to your server and shows the plaza. A new hunter is asked about
   Capcom's user registration (choose **登録しない**, do not register) and a Capcom ID (take the first free slot).
   Then 街に入る -> a lobby -> the town.

Tested in PCSX2 (15 Oct 2026, docs/network.md 3.6): login, plaza, town, and a PC player in the same town (each sees
the other, chat both ways). Not tested on a real PS2 yet. The patch refuses the addresses of MH Oldschool, which serves
unpatched discs itself.

### Playing online on a real PS2 without a patched disc (memory-card launcher)

For a PS2 that runs homebrew from the memory card (**Free McBoot** or **OpenTuna**) and boots the **original disc**:
`MH1LAUNCH.ELF` loads the game from your disc, patches it in the console's memory (the same two changes as above:
DNAS skipped, your server instead of the KDDI portal) and starts it. The disc and your saves are not changed. Same
limits as above: lobby, town, chat and rooms; no hunting together yet; Japanese Monster Hunter (SLPM-65495) only.

1. Build it once on Linux (no disc needed): get the ps2sdk toolchain (the prebuilt "latest" release of
   github.com/ps2dev/ps2dev, `ps2dev-ubuntu-latest.tar.gz`, unpacked anywhere), then
   `PS2DEV=/path/to/ps2dev tools/ps2launch/build.sh` -> `build/ps2launch/MH1LAUNCH.ELF`.
2. Write `MH1LAUNCH.CNF`, a plain text file with your server and account:

       server = 192.168.1.20:10200
       login = 12345678
       password = YOURPASSWORD

3. Copy both files to the memory card, into the same folder, e.g. `mc0:/APPS/` (with uLaunchELF's file browser from a
   USB stick, or any memory-card manager). The config can also go to `mc0:/MH1LAUNCH/MH1LAUNCH.CNF`.
4. Start it by hand from uLaunchELF / the FMCB menu with the Monster Hunter disc in the drive, or let it start by
   itself at power-on (below). It shows a few lines of text and then the game starts as usual. Make the network
   setting in the game as in step 5 above; **after making it, restart the console** (Sony's settings program restarts
   the game from the disc without the patch).

**Auto-launch at power-on** (then "run homebrew once" is enough: every power-on with the MH disc goes online with
your server). The launcher is safe for every disc: any other game (and Monster Hunter G, not supported yet) is
started exactly as the console would, untouched; with no disc or a PS1 / DVD video disc it goes back to the browser.
* Free McBoot 1.966 (and OpenTuna installs, which come with the same FMCB configurator): FMCB menu -> **FMCB
  Configurator** -> "Configure launcher keys" (or edit `mc0:/SYS-CONF/FREEMCB.CNF`) -> set the **Auto** entry to
  `mc0:/APPS/MH1LAUNCH.ELF` (the line `LK_Auto_E1 = mc0:/APPS/MH1LAUNCH.ELF`), keep the auto-launch delay short,
  save. Holding a pad button at power-on still opens the FMCB menu as before.
* We do not ship Free McBoot or OpenTuna; install them from their own releases. **Untested on a console** (written
  from FMCB's documented settings; tested only in PCSX2, where the launcher was started directly).

## Known issues
## Known issues

* The game is in **Japanese**. An English translation table can be loaded (docs/english.md) but none is included.
* Windows and Linux builds have mostly been tested headless (automatic scripted play and screenshots) plus owner play
  sessions on Linux; the Windows package has only run under Wine, with no real Windows, GPU driver or audio device
  tested. Reports welcome.
* Lighting uses a fixed light set instead of the game's stage lights, and sound reverb is approximate; nothing has
  been compared frame by frame with a real PS2, so some effects look different.
* Xbox: about 30 fps in the village and hunts on the tested console; long pauses when changing areas (no loading
  screen); some effects may differ; no in-game settings; the dashboard shows a generic save icon.
* Some rare monster/stage combinations may still have glitches. Press **F8** (PC) or **Back + Black** (Xbox) when
  you see one: see "Reporting a bug" above. A report folder (screenshots, game state, your
  inputs) is saved under the game's data folder (`.../mh1pc/reports`). Every run also writes a debug log
  (`.../mh1pc/logs`); nothing is ever sent anywhere. Attach them to an issue at
  https://github.com/itsbigcforme67/mh1-xbox/issues (Windows: `bug_report.bat` packs them).

## Credits and licences

* Monster Hunter is (c) CAPCOM CO., LTD. The game's data is **not** part of this project; you supply your own disc.
* The port is made by reconstructing the game's code (matching decompilation against the Japanese executable)
  and writing a new platform layer. Source: https://github.com/itsbigcforme67/mh1-xbox
* libmpeg2 (GPL, movie decoding) and SDL2 (zlib licence) are bundled; nxdk (Xbox SDK) builds the Xbox version.
* Community knowledge about the game's formats and network protocol came from public research (docs/RESEARCH.md).
