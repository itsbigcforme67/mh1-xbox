# Testing Monster Hunter G on your own original Xbox

Thanks for helping. This is a fan-made port of the Japanese PS2 game **Monster Hunter G** (and optionally the first
**Monster Hunter**) to the original Xbox and the PC. It contains no game data: you build it from **your own disc**, and
what you build stays with you. Please do not share the folders you make (they hold your copy of the game files).

## 1. What you need

* **Your own disc image** of the Japanese Monster Hunter G for PS2 (SLPM-65869), as an `.iso` file (or a folder with the
  disc's files). Optional: the Japanese Monster Hunter (SLPM-65495) to test that one too. US/EU discs do not work.
* **A Linux computer** to build on (tested on Ubuntu 24.04; a Windows PC can use a Linux virtual machine or WSL2,
  untested). About **5 GB** free disk space per game (build + 1.1 GB of game files), 4 GB of RAM, 20-60 minutes.
* **A modded original Xbox** with a dashboard that starts programs from the hard disk (Evolution X, UnleashX, XBMC,
  ...), and a way to copy files to it (FTP from the dashboard is the usual one).
* These programs on the Linux computer. Copy and paste:

      sudo dpkg --add-architecture i386 && sudo apt update
      sudo apt install git python3 build-essential libsdl2-dev gcc-multilib libc6:i386 libsdl2-2.0-0:i386 libgl1:i386
      sudo apt install clang lld llvm bison flex cmake      # for nxdk, the Xbox SDK

  and **nxdk** (the open-source Xbox SDK), built once:

      git clone --recursive https://github.com/XboxDev/nxdk ~/xboxdev/nxdk
      cd ~/xboxdev/nxdk && export NXDK_DIR=$PWD PATH=$PWD/bin:$PATH
      make NXDK_ONLY=y -j4 && make tools

  The build looks for nxdk in `~/xboxdev/nxdk` (or set `NXDK_DIR`). Use clang 18 (Ubuntu 24.04's `clang`): nxdk warns
  that clang 19.x up to 20.1.2 miscompiles some code. docs/xbox.md, "Toolchain", has more detail and a
  recipe without root. No Python packages are needed (standard library only). Nothing from Microsoft is needed.

You never need: a PS2, an Xbox BIOS or dashboard file, or any download of game data.

## 2. Build it

    git clone https://github.com/itsbigcforme67/mh1-xbox
    cd mh1-xbox
    tools/make_release.sh /path/to/MonsterHunterG.iso
    # with Monster Hunter 1 too:  tools/make_release.sh G.iso --mh1 MonsterHunter.iso

It checks the disc first (a wrong or damaged image is reported and nothing is built), then checks the tools and says
what is missing, then builds. Add `--setup` if it tells you to (it fetches small parts it can get without root).
The result is in `dist/release/`:

| folder | what |
|---|---|
| `xbox/MHG/` | `default.xbe` + `data/` (about 1.1 GB): the Xbox game. Copy this whole folder to the Xbox |
| `xbox/MH1/` | the same for Monster Hunter 1 (with `--mh1`) |
| `linux/MHG/` | the PC version: run `./play.sh` (useful to compare with the Xbox) |
| `VERSION.txt` | which build this is: put it in every bug report |
| `make_release.log` | the whole build log (send it if the build fails) |

Optional: `--online` also builds the online version (section 4, test 6); `--windows` a Windows PC version. English
text needs more of your own discs (the US Monster Hunter, and Monster Hunter Freedom + Portable for PSP):
`--english US.iso --freedom Freedom.iso --portable Portable.iso` (needs `--mh1`). Without it the game is in Japanese.

## 3. Put it on the Xbox

1. Copy the **whole folder** `dist/release/xbox/MHG` to the Xbox hard disk, for example as **`E:\Games\MHG`**, so
   that `E:\Games\MHG\default.xbe` and `E:\Games\MHG\data\AFS00.AFS` exist. Any FTP program works (FileZilla:
   connect to the Xbox's address with the dashboard's FTP login, open `E:`, drag the folder in). **`E:\Games\MHG` is
   the safe place**: the game looks for its files in the `data` folder next to its `default.xbe` first and in
   `E:\Games\MHG\data` second. Another folder such as `E:\Apps\MHG` works when your dashboard starts programs with
   their own folder as drive D: (most do); if the game then says files are missing, move the folder to `E:\Games\MHG`
   and tell us your dashboard. The online version (test 6) needs `E:\Games\MHG`.
2. The files are big (AFS00.AFS is 884 MB). Some dashboards' FTP servers drop big files part way: afterwards check the
   sizes, `AFS_DATA.AFS` 150,759,424 bytes, `AFS00.AFS` 926,939,136, `AFS01.AFS` 25,186,304, `SLPM_658.69` 2,029,336.
   The game checks them itself at start-up and tells you which file is missing or cut short.
3. Start **Monster Hunter G** from the dashboard's games or applications list.
4. Leave the game: hold **Back + Start** for 2 seconds (goes back to the dashboard).

No network loader and no computer are needed while you play. (The developer's loader, `tools/xbox_serve.py`, is only a
shortcut for the developer.)

Where things are on the Xbox (for a folder `E:\Games\MHG`):

| | where |
|---|---|
| log file | `E:\Games\MHG\mh1log.txt`, **rewritten at every start**: copy it off before you start the game again |
| bug reports (Back + Black) | `E:\Games\MHG\reports\report_<date>_<time>\` |
| settings (Back + L3 menu) | `E:\Games\MHG\settings.ini` |
| saves | `E:\UDATA\4D480002\` ("Monster Hunter G"; Monster Hunter 1: `E:\UDATA\4D480001\`) |
| English table (if you made one) | `E:\Games\MHG\text\en.txt` (`make_release.sh --english` puts it in the folder) |

Buttons: the game uses the PS2 layout on the Xbox pad. **Back + R3**: testers' debug menu (jump to quests, areas,
items). **Back + L3**: settings. **Back + Black**: bug report (freezes the picture; **A** marks what is wrong, **Y**
types a note, **Start** saves it). Full controls: docs/PLAYING.md.

## 4. What to test

Do what you like, but these are the things we most need checked on real consoles. Note the result of each (works /
broken / not tried) in your report.

1. **Saves.** NEW GAME, make a hunter, save when the game asks. Play a little, save in the bed of your house in the
   village. Leave the game (Back + Start), start it again, CONTINUE: is your hunter back, with the same money and items?
   Does the dashboard's memory / saved games page list "Monster Hunter G"?
2. **Fog in the first meat quest.** From the village elder, the first quest (deliver 2 Raw Meat). Is the fog in the
   first area much thicker than you remember from the PS2, or does it look right? A photo of the screen helps.
3. **Starting a second time.** After playing, leave with Back + Start, then start Monster Hunter G again from the
   dashboard (do this a few times, also after a long session). Does it start every time? Any black screen or hang at
   start-up? If it hangs, note how long you waited and whether a power cycle fixed it.
4. **The watchdog.** If the game ever freezes, it should notice within about 5 seconds, write a freeze report into
   `mh1log.txt` and return to the dashboard by itself. After a crash it should show "crashed" and do the same. Tell us
   if a freeze did NOT end by itself (and how long you waited), and send the log in either case.
5. **Picture: tearing and flicker.** The way frames are shown was changed recently. Watch for a picture split
   horizontally (tearing) while the camera turns, flicker, or stutter, in the village and in a quest. Note your TV
   connection (composite, component, HDMI adapter) and 480i / 480p.
6. **Online lobby** (only with `--online`, and only on your own server: never point it at someone else's server).
   Needs `tools/server/mh1_server.py` running on your computer: docs/PLAYING.md, "Online on the Xbox", has the steps
   (an account, then a `settings.ini` with `online_server`, `online_login`, `online_password`). Copy the folder
   `xbox/MHG-online` (just `default.xbe`) to the Xbox as `E:\Games\MHG-online` and put that `settings.ini` in it; it
   uses the game files in `E:\Games\MHG\data`, so the normal game must be in `E:\Games\MHG`. Does it log in, show the
   plaza and the town? Can a PC player on the same server see you?
7. **English text read-through** (only if you made the English table). Read menus, shop texts, quest descriptions and
   villager talk. Note text that is still Japanese, cut off, overlapping its box, or plainly wrong English, with where
   you saw it (a bug report with Back + Black is ideal: it records the place).
8. **Everything else.** Frame rate in the village and in a busy fight, sound and music, missing or wrong graphics,
   the hunter falling through the ground, menus that do nothing.

## 5. Reporting a bug

One report per problem. Copy this into a message (or a GitHub issue) and fill it in:

    Build (VERSION.txt):            e.g. 1a2b3c4d5e
    Game:                           Monster Hunter G / Monster Hunter
    Xbox revision:                  e.g. 1.0, 1.1, 1.2, 1.3, 1.4, 1.6, or "don't know" + the manufacture date on the label under the console
    RAM:                            64 MB (stock) or 128 MB (upgraded); the dashboard's system info shows it
    Mod / BIOS type:                e.g. modchip (name if known) or softmod; just the name, never send a BIOS file
    Dashboard:                      e.g. Evolution X 3935, UnleashX 0.39
    Video:                          TV connection and 480i / 480p / 720p; widescreen on or off
    Where the game is installed:    e.g. E:\Games\MHG
    What happened:
    Steps to make it happen:        1. ... 2. ... 3. ...
    How often:                      every time / sometimes / once
    Log file attached:              mh1log.txt from the game's folder (copied BEFORE starting the game again)
    Bug report folder attached:     E:\Games\MHG\reports\report_... (if you made one with Back + Black)
    Photo / video:                  a phone photo of the TV is fine

What not to send: game files, your disc image, BIOS or EEPROM files, your dashboard's FTP password.

## 6. Problems while building

* "The disc is not usable": a wrong region (US/EU), a damaged image, or a different game. The message says which file
  failed. Re-dump the disc if a checksum fails.
* "'gcc' is missing", "A 32-bit program cannot be built", "nxdk was not found": the message says what to install;
  see section 1.
* Anything else: send `dist/release/make_release.log` and the build logs it names (`build/*.log`).
