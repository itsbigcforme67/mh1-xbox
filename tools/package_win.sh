#!/bin/sh
# Package the Windows build as a test release zip: build/release/mh1pc-win32-<date>-<githash>.zip with
# mhview.exe, SDL2.dll, play.bat, bug_report.bat (+ .ps1), README.txt and the licenses. Nothing is
# uploaded or published. Run tools/build_win.sh first. The script REFUSES to zip when a file is larger
# than expected or is named like one of the game's disc files (a guard against shipping Capcom data).
set -e
cd "$(dirname "$0")/.."
W=${WINROOT:-$HOME/mh1win}
SDLLIC=$(ls -d "$W"/SDL2-[0-9]* 2>/dev/null | grep -v '\.tar' | tail -1)/LICENSE.txt
[ -f build/win/mhview.exe ] && [ -f build/win/SDL2.dll ] || { echo "run tools/build_win.sh first"; exit 1; }
[ -f "$SDLLIC" ] || { echo "SDL2 license not found ($SDLLIC)"; exit 1; }
HASH=$(git rev-parse --short=8 HEAD)
git diff --quiet HEAD 2>/dev/null || { echo "note: the working tree has uncommitted changes (the build says so with a + in its log)"; }
NAME=mh1pc-win32-$(date +%Y%m%d)-$HASH
S=build/release/$NAME
rm -rf "$S"; mkdir -p "$S/licenses"
cp build/win/mhview.exe "$S/mhview.exe"
LLVM=$(ls -d "$W"/llvm-mingw-*msvcrt* 2>/dev/null | grep -v '\.tar' | head -1)
"$LLVM/bin/llvm-strip" --strip-all "$S/mhview.exe"   # no debug info / symbol table: smaller, no build paths (the crash report uses its own table)
cp build/win/SDL2.dll build/win/play.bat build/win/bug_report.bat build/win/bug_report.ps1 "$S/"
# no debug info in a release: it holds the build machine's paths (crash backtraces use the
# program's own symbol table, rt_host_symname, which stripping keeps)
STRIP=$(ls "$HOME"/mh1win/llvm-mingw-*/bin/llvm-strip 2>/dev/null | head -1)
[ -n "$STRIP" ] || { echo "llvm-strip not found in ~/mh1win"; exit 1; }
"$STRIP" --strip-debug "$S/mhview.exe"
if strings "$S/mhview.exe" "$S/SDL2.dll" | grep -q "$HOME"; then echo "REFUSED: build paths in the binaries"; exit 1; fi
cp third_party/libmpeg2/COPYING "$S/licenses/libmpeg2-COPYING.txt"
cp "$SDLLIC" "$S/licenses/SDL2-LICENSE.txt"
cat > "$S/licenses/NOTICE-GPL.txt" <<'EOT'
This program contains libmpeg2 (Copyright Aaron Holtzman, Michel Lespinasse, Silicon Graphics and others),
which is licensed under the GNU General Public License version 2 (see libmpeg2-COPYING.txt). Because of
it the source code of this port is available under the GPL: https://github.com/itsbigcforme67/mh1-xbox
(the src/pc/ part and the build scripts; the game data is NOT included and stays Capcom's).
SDL2 is under the zlib license (SDL2-LICENSE.txt).
EOT
cat > "$S/README.txt" <<'EOT'
Monster Hunter (PS2, Japan) - PC test build for Windows
=======================================================

THIS IS A TEST BUILD. It runs the original game's logic (decompiled from the Japanese PS2 version) on your
PC. Things are missing or wrong: no online play, approximate sound reverb, some effects not done. Please
report what you find (see "Bug reports").

What you need
  Your own disc image of the Japanese Monster Hunter for PS2 (SLPM-65495), as a .iso file. The game's data
  is NOT part of this download and never will be.

First start (once)
  Drag your .iso file onto mhview.exe (or onto play.bat). The program reads the ISO itself and copies the
  game files it needs (about 925 MB) next to mhview.exe in the folder "data" (or, if that folder is not
  writable, into %APPDATA%\mh1pc\data). A progress bar is shown. Afterwards the ISO is not needed any more.
  You can also just run play.bat: if no data is found it asks for the ISO.
  A wrong disc or a damaged image is detected (sizes and checksums are compared) and reported.

Playing
  play.bat                   from power-on: logos, title, new hunter / continue, the village
  play.bat quest             straight into quest 10 (Rathian)      play.bat easy      same, hunter cannot faint
  play.bat village           straight into the village
  Keyboard: W/A/S/D move, arrow keys attack, K roll (cross), L sheathe (circle = confirm in menus),
  J item (square), I triangle, E guard, Q camera reset, Z/C L2/R2, T/F/G/H d-pad, Enter = Start (pause menu),
  Backspace = Select, Esc quits. Xbox-style controllers work (left stick move, right stick attack, A roll,
  B sheathe, X item, RB guard, LB camera reset, Start pause). In menus B (circle) confirms, A cancels,
  as on the Japanese PS2.
  Saves: %APPDATA%\mh1pc\memcard0 (save in your house's bed or after a quest, load with CONTINUE).

Using your real PS2 save
  Drag your save file onto mhview.exe (or onto play.bat), or onto the game window while it runs. Accepted:
  .psu (uLaunchELF / EMS), .max (Action Replay Max), .cbs (CodeBreaker), .sps / .xps (SharkPort / X-Port),
  and a whole memory card image (.ps2, .mcd, .mc2, .bin; for example from PCSX2 or a card dumped with
  mymc / uLaunchELF), from which the Monster Hunter save (BISLPM-65495MH) is taken. Then choose CONTINUE.
  A save you already had on the PC is first copied to %APPDATA%\mh1pc\memcard0.backups. The save is
  checked with the game's own checksum first; a damaged or foreign file is refused and nothing changes.
  Back to a PS2 or PCSX2: from a command prompt run  mhview.exe --export-save MyHunter.psu  (or .max,
  .cbs, .sps, .xps, or .ps2 for a new 8 MB memory card image holding just this save).
  The command line forms are  mhview.exe --import-save FILE  and  mhview.exe --export-save FILE.

Display options
  Alt+Enter (or F11) toggles fullscreen. The window can be resized; its size and position are remembered in
  %APPDATA%\mh1pc\mh1pc.ini (a plain text file you can edit). The picture is the original 4:3 by default.
  Flags (put them after the game name in play.bat's line, or set MH_ARGS=... before running play.bat):
    --fullscreen / --windowed      --size 1280x720           --widescreen (16:9, wider view, HUD at the screen edges;
    --vsync / --no-vsync           --fps-cap 60               menus stay 4:3 in the middle)
    --msaa 2|4 (smoother edges)    --aniso 1..16 (sharper ground textures at a slant)
    --filter2d nearest|linear      --ini FILE / --no-ini
  The game itself always runs at its original 30 steps per second, whatever the frame rate.

Bug reports
  Press F8 in the game (or hold Back/View and press Start on a controller) when you see something wrong. The
  game freezes. Click the broken things with the mouse (or drag a box around them; right-click undoes a pick),
  type what is wrong, press Enter. Esc cancels. This saves a report folder with screenshots, which objects you
  picked, the game state, the last seconds as a small clip and your inputs (so the problem can be replayed), in
  %APPDATA%\mh1pc\reports. The screenshots show the game's graphics; nothing is sent anywhere.
  Every run also writes a debug log to %APPDATA%\mh1pc\logs (the last 20 are kept). It holds the build, your
  Windows and graphics driver, game events, warnings and, after a crash, a backtrace and the last lines.
  It contains no user name or home folder, no game data, and nothing is ever sent anywhere.
  After a problem run bug_report.bat: it makes mh1_bug_report_<date>.zip (the last two logs, your last three
  F8 reports and a list of your save folder's file names; not the save itself). Then open
  https://github.com/itsbigcforme67/mh1-xbox/issues/new?template=bug_report.md and attach the zip.

Credits and legal
  Monster Hunter is (c) CAPCOM CO., LTD. This project is not affiliated with, endorsed by or sponsored by
  Capcom. The program is built from C code reconstructed (decompiled) from the Japanese PS2 game; none of
  the game's data (models, textures, sound, movies, text) is included, so it needs your own copy of the
  game. See the licenses folder (libmpeg2 is GPL, SDL2 is zlib).
EOT
# ---- guard: no home directory or user name in any file (build paths in debug info, scripts ...)
ME=$(id -un)
if grep -rl -a -e "$ME" "$S" >/dev/null 2>&1; then
    echo "REFUSING: the user name '$ME' (a home path?) is inside the package:"; grep -rl -a -e "$ME" "$S"; exit 1
fi
# ---- guard: nothing large, nothing named like game data
MAXFILE=$((25 * 1024 * 1024)); MAXTOTAL=$((40 * 1024 * 1024)); TOTAL=0
for f in $(cd "$S" && find . -type f | sed 's|^\./||'); do
    sz=$(wc -c < "$S/$f"); TOTAL=$((TOTAL + sz))
    base=$(basename "$f" | tr 'A-Z' 'a-z')
    case "$base" in
        afs*|slpm*|system.cnf|*.iso|*.bin|*.img|*.afs|main.elf|*.ahi|*.amo|*.sfd|*.adx|*.mpg)
            echo "REFUSING: $f is named like game data"; exit 1 ;;
    esac
    [ "$sz" -le "$MAXFILE" ] || { echo "REFUSING: $f is $sz bytes (limit $MAXFILE)"; exit 1; }
done
[ "$TOTAL" -le "$MAXTOTAL" ] || { echo "REFUSING: package is $TOTAL bytes (limit $MAXTOTAL)"; exit 1; }
ZIP=$PWD/build/release/$NAME.zip
rm -f "$ZIP"
python3 - "$S" "$ZIP" <<'PY'
import os, sys, zipfile
root, out = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    for d, _, fs in sorted(os.walk(root)):
        for f in sorted(fs):
            p = os.path.join(d, f)
            z.write(p, os.path.join(os.path.basename(root), os.path.relpath(p, root)))
PY
# ---- guard again on the finished zip
python3 - "$ZIP" <<'PY'
import sys, zipfile
z = zipfile.ZipFile(sys.argv[1])
bad = [i.filename for i in z.infolist() if i.file_size > 25 * 1024 * 1024]
if bad or sum(i.file_size for i in z.infolist()) > 40 * 1024 * 1024:
    print("REFUSING: the zip holds oversized files", bad); sys.exit(1)
print("zip holds:"); [print("  %9d  %s" % (i.file_size, i.filename)) for i in z.infolist()]
PY
echo "made $ZIP ($(wc -c < "$ZIP") bytes). Not published; test it on Windows first (docs/pc.md)."
