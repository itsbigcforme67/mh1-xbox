#!/bin/bash
# xemu_relaunch_test.sh: the game started a SECOND time without a power cycle, the owner's way, in xemu (hidden), on a
# SCRATCH hard disk (never xbox_files/hdd.img). Owner (G, real Xbox, Oct 2026): after Back + Start and a relaunch the
# screen stayed black (docs/xbox.md "Black screen at "ac97 open" after a Back + Start relaunch", RELEASE_G GB5).
#
#   tools/xemu_relaunch_test.sh [--version mh1|mhg] [--online] [--save | --powercut]      PASS / FAIL per check
#
# The disk holds what the owner's Xbox has: the game's network loader as E:\Games\<MH1|MHG>\default.xbe (+ loader.txt)
# and the game data in E:\Games\<MH1|MHG>\data. This script's own tools/xbox_serve.py (PORT, default 8796; never the
# owner's 8765 server) serves the game build. The test program src/pc/xbox/testlaunch stands in for the dashboard (the
# DVD at power-on, C:\xboxdash.xbe after that): it starts the next line of E:\launch.txt each time a program goes "back
# to the dashboard". So the chain is
#   DVD -> loader (downloads the build) -> game, start 1 -> Back + Start -> loader (sends the log) -> "dashboard"
#   -> loader -> game, start 2 -> Back + Start -> loader (sends the log) -> "dashboard" (no line left: the end)
# as on the Xbox, except that the "dashboard" is our stand-in (started by the kernel after XLaunchXBE(NULL)'s quick
# reboot, as Evolution X is), not an XDK program: what the real dashboard resets or leaves (D3D, DirectSound) is missing.
# The pad script (served by the loader's /test, played by the game from its data folder) waits, then holds
# select + start (Back + Start, pad_script_quit_held). Checks: both starts sent a log that reached the quit; start 2
# drew frames; no freeze / crash report; the watchdog's longest waits (RT_WATCHDOG_MS, default the hardware's 5000).
#   --save: start 1 goes straight to the village with a fresh hunter (RT_VILLAGE_START), is moved to the house bed (RT_LB_WARP) and saves
#           on the empty card (MH1: tools/pc_scripts/bed_save.txt + "create game data: yes"; G: tools/test_save_g.sh's
#           box equip + bed save), then quits; start 2 boots from power-on and CONTINUEs (tools/pc_scripts/continue.txt,
#           G: test_save_g.sh's). Checks: the file loaded is the file saved (byte count and check sum in the logs), the
#           loader sent the save (POST /save), the village was reached from the save.
#   --powercut: start 1 of --save only; xemu is killed 3 s after the game logs the save (a power cut, as the owner
#           switching off), then the save file on the scratch disk must have the check sum the game logged.
#   --kit DIR: the testers' kit's game folder instead (tools/make_release.sh: DIR = <kit>/xbox/MHG or MH1, its default.xbe
#           and data/), installed as the testers do: E:\Games\<MHG|MH1>\ holds default.xbe = the game itself, no loader
#           and no laptop. The quit goes straight to the dashboard; the stand-in dashboard copies the next start's pad
#           script / env.txt into data\ ("copy" lines of launch.txt) and keeps start 1's log (mh1log.txt) for the checks.
#           Works with --save and --powercut. (tools/make_release.sh --xemu-test runs it.)
#   --online: the online build (build/[mhg/]xbox/online/nv2a); its loader and quit stop the NIC.
# Needs: the nv2a build, disc/mh1 or disc/mhg, the owner's mcpx.bin / bios.bin (XBOX_FILES_SRC, default the main
# checkout's xbox_files; read only), ~1.2 GB of disk for the scratch disk (removed at the end unless KEEP=1).
# About 8-11 minutes a run. Run it alone, under the shared emulator lock:
#   flock "/home/james/claude projects/MH XBOX/xemu.lock" tools/xemu_relaunch_test.sh --version mhg
set -e
cd "$(dirname "$0")/.."
VER=mh1; ONL=; MODE=plain; KIT=
while [ $# -gt 0 ]; do
    case "$1" in
    --version) VER=$2; shift 2 ;;
    --online) ONL=1; shift ;;
    --save) MODE=save; shift ;;
    --powercut) MODE=powercut; shift ;;
    --kit) KIT=$(realpath "$2"); shift 2 ;;
    *) echo "unknown option $1"; exit 2 ;;
    esac
done
if [ -n "$KIT" ]; then
    if [ -f "$KIT/data/SLPM_658.69" ]; then VER=mhg; elif [ -f "$KIT/data/SLPM_654.95" ]; then VER=mh1
    else echo "--kit $KIT: no data/SLPM_658.69 or SLPM_654.95 (a kit game folder: default.xbe + data/)"; exit 2; fi
fi
case $VER in
mh1) FOLDER=MH1; GID=mh1; XB=build/xbox; DISC=disc/mh1; PROG=SLPM_654.95; SERVE=--xbe; CARD=BISLPM-65495MH; TID=4D480001 ;;
mhg) FOLDER=MHG; GID=mhg; XB=build/mhg/xbox; DISC=disc/mhg; PROG=SLPM_658.69; SERVE=--xbe-g; CARD=BISLPM-65869MHG; TID=4D480002 ;;
*) echo "--version mh1|mhg"; exit 2 ;;
esac
[ -z "$ONL" ] || XB=$XB/online
XBE=$XB/nv2a/default.xbe
[ -z "$KIT" ] || { XBE=$KIT/default.xbe; DISC=$KIT/data; }
T=$(realpath -m "${RELAUNCH_TEST_DIR:-build/xbox/relaunch_xemu_$GID${ONL:+_online}${KIT:+_kit}_$MODE}")
SRC=${XBOX_FILES_SRC:-$(git rev-parse --path-format=absolute --git-common-dir)/../xbox_files}
PORT=${PORT:-8796}
WD=${WATCHDOG_MS:-5000}
WAIT=${WAIT_TICKS:-1500}        # ticks of the game before the quit combo (the power-on boot reaches the title)
TOTAL=${TOTAL_S:-600}           # seconds of xemu in all
FATX="python3 tools/xbox_fatx.py"
for f in "$XBE" "$SRC/mcpx.bin" "$SRC/bios.bin" "$DISC/$PROG"; do [ -f "$f" ] || { echo "missing $f"; exit 1; }; done
export MH1_HIDDEN=1
. tools/emu_lib.sh
while xemu_running; do echo "another xemu is running: waiting"; sleep 30; done

. ~/xboxdev/env.sh
rm -f src/pc/xbox/loader/*.obj src/pc/xbox/xnet_http.obj src/pc/xbox/xbox_report.obj
if [ $VER = mhg ]; then
    make -C src/pc/xbox/loader -s OUTPUT_DIR=../../../../build/xbox/loader_g XBE_TITLE='Monster\ Hunter\ G'
    LOADER=build/xbox/loader_g/default.xbe
else
    make -C src/pc/xbox/loader -s
    LOADER=build/xbox/loader/default.xbe
fi
make -C src/pc/xbox/testlaunch -s
rm -f src/pc/xbox/loader/*.obj src/pc/xbox/xnet_http.obj src/pc/xbox/xbox_report.obj

rm -rf "$T"; mkdir -p "$T"
ln -s "$SRC/mcpx.bin" "$T/mcpx.bin"; ln -s "$SRC/bios.bin" "$T/bios.bin"
truncate -s 8G "$T/hdd.img"
$FATX format "$T/hdd.img" >/dev/null
if [ -n "$KIT" ]; then           # the kit's folder as the testers copy it: the game is default.xbe, data/ all of it
    $FATX put "$T/hdd.img" "$XBE" Games/$FOLDER/default.xbe >/dev/null
    for f in "$KIT"/data/*; do [ -f "$f" ] && $FATX put "$T/hdd.img" "$f" "Games/$FOLDER/data/$(basename "$f")" >/dev/null; done
else
    printf '10.0.2.2:%s\ngame=%s\n' "$PORT" "$GID" > "$T/loader.txt"
    $FATX put "$T/hdd.img" "$LOADER" Games/$FOLDER/default.xbe >/dev/null
    $FATX put "$T/hdd.img" "$T/loader.txt" Games/$FOLDER/loader.txt >/dev/null
    for f in AFS_DATA.AFS AFS00.AFS AFS01.AFS $PROG; do $FATX put "$T/hdd.img" "$DISC/$f" Games/$FOLDER/data/$f >/dev/null; done
fi
# the stand-in dashboard: XLaunchXBE(NULL) makes the kernel start C:\xboxdash.xbe (a modded BIOS may look for evoxdash.xbe);
# without one xemu stops at the kernel's error 21 screen
FATX_PART=c $FATX format "$T/hdd.img" >/dev/null
for d in xboxdash.xbe evoxdash.xbe; do FATX_PART=c $FATX put "$T/hdd.img" build/xbox/testlaunch/default.xbe $d >/dev/null; done
$FATX put "$T/hdd.img" build/xbox/testlaunch/default.xbe evoxdash.xbe >/dev/null
L="E:\\Games\\$FOLDER\\default.xbe"
printf '%s\n%s\n' "$L" "$L" > "$T/launch.txt"
QUIT="select+start*400,idle*900"     # (the quit needs 60 drawn frames held; xemu draws ~15 a second in the house)
printf 'RT_WATCHDOG_MS=%s\n' "$WD" > "$T/env2.txt"
if [ $MODE = plain ]; then
    echo "idle*$WAIT,$QUIT" > "$T/input1.txt"
    cp "$T/env2.txt" "$T/env1.txt"; cp "$T/input1.txt" "$T/input2.txt"
else
    # start 1: a fresh hunter in the village, the house, the bed save (as tools/test_save_g.sh / the MH1 PC check)
    { cat "$T/env2.txt"; echo "RT_ARGS=--quest 1 --play"; echo "RT_VILLAGE_START=1"; echo "RT_VILLAGE_SKIP_INTRO=1"
      echo "RT_NAME=TEST"; echo "RT_NOMOVIE=1"; } > "$T/env1.txt"
    printf 'RT_NOMOVIE=1\n' >> "$T/env2.txt"
    if [ $VER = mhg ]; then
        { echo "RT_LB_WARP=30,11225,14400,0;150,1990,1160,4001;600,2259,745,4001"; echo "RT_WARE=6:1,6:2,2:1,2:5"
          echo "RT_POUCH=129:5"; } >> "$T/env1.txt"
        B="@village1,idle*45,square*2,idle*153,square*2,idle*48,ddown*2,idle*13,ddown*2,idle*23,circle*2,idle*48,dright*2"
        B="$B,idle*28,circle*2,idle*48,circle*2,idle*48,circle*2,idle*38,cross*2,idle*28,cross*2,idle*28,cross*2,idle*78"
        B="$B,square*2,idle*98,circle*2,idle*98,circle*2,idle*98,circle*2,idle*98,circle*2,idle*48,dleft*2,idle*28,circle*2"
        echo "$B,idle*600,$QUIT" > "$T/input1.txt"
        C="idle*80,circle*2,idle*600,start*2,idle*80,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2"
        C="$C,idle*100,circle*2,idle*250,ddown*2,idle*20,circle*2,idle*100,circle*2,idle*100,circle*2,idle*900"
        echo "$C,$QUIT" > "$T/input2.txt"
    else
        echo "RT_LB_WARP=30,11225,14400,0;150,2259,745,4001" >> "$T/env1.txt"
        echo "@village1,idle*45,square*2,idle*200,$(cat tools/pc_scripts/bed_save.txt),idle*300,circle*2,idle*200,dleft*2,idle*40,circle*2,idle*400,circle*2,idle*200,circle*2,idle*300,$QUIT" > "$T/input1.txt"
        echo "$(cat tools/pc_scripts/continue.txt),idle*600,$QUIT" > "$T/input2.txt"
    fi
fi
cp "$T/input1.txt" "$T/input.txt"; cp "$T/env1.txt" "$T/env.txt"     # what xbox_serve.py sends now (read per request)
if [ -n "$KIT" ]; then           # no loader: the stand-in dashboard puts each start's script in place, keeps start 1's log
    D="E:\\Games\\$FOLDER\\data"
    for k in 1 2; do
        $FATX put "$T/hdd.img" "$T/input$k.txt" t/input$k.txt >/dev/null
        $FATX put "$T/hdd.img" "$T/env$k.txt" t/env$k.txt >/dev/null
    done
    { printf 'copy E:\\t\\input1.txt %s\\input.txt\ncopy E:\\t\\env1.txt %s\\env.txt\n%s\n' "$D" "$D" "$L"
      printf 'copy E:\\Games\\%s\\mh1log.txt E:\\t\\start1.txt\n' "$FOLDER"
      printf 'copy E:\\t\\input2.txt %s\\input.txt\ncopy E:\\t\\env2.txt %s\\env.txt\n%s\n' "$D" "$D" "$L"; } > "$T/launch.txt"
fi
$FATX put "$T/hdd.img" "$T/launch.txt" launch.txt >/dev/null

SPID=
LOGS0=$(date +%s)
cleanup() {
    [ -z "$SPID" ] || { kill "$SPID" 2>/dev/null || :; wait "$SPID" 2>/dev/null || :; }
    [ -n "${KEEP:-}" ] || rm -f "$T/hdd.img"
}
emu_traps cleanup      # (and run_xemu.sh, which stops xemu and its Xvfb: tools/emu_lib.sh)
if [ -z "$KIT" ]; then
    python3 tools/xbox_serve.py --port "$PORT" $SERVE "$XBE" --input "$T/input.txt" --env "$T/env.txt" --test-game $GID \
        > "$T/serve.log" 2>&1 &
    SPID=$!
    sleep 2
fi
SHOTS=""; for s in $(seq 30 30 "$TOTAL"); do SHOTS="$SHOTS $s"; done
XBOX_FILES="$T" tools/run_xemu.sh --xbe build/xbox/testlaunch/default.xbe ${KIT:---net} --shot "$T/shot.png" \
    --after "${SHOTS# }" > "$T/run.log" 2>&1 &
XPID=$!
if [ -z "$KIT" ]; then
    # start 2 gets its own script: the loader asks for /test once per start; after the first, the files change
    until grep -q 'GET /test' "$T/serve.log" 2>/dev/null || ! emu_alive $XPID; do sleep 2; done
    cp "$T/input2.txt" "$T/input.txt"; cp "$T/env2.txt" "$T/env.txt"
fi
if [ $MODE = powercut ]; then
    until grep -q 'memory card: saved' "$T/xemu_debug.txt" 2>/dev/null || ! emu_alive $XPID; do sleep 1; done
    sleep 3
    echo "power cut: xemu stopped 3 s after the save"
    emu_stop $XPID
fi
wait $XPID || :
cp "$T/xemu_debug.txt" "$T/serial.txt"
[ -z "$SPID" ] || { kill "$SPID" 2>/dev/null || :; wait "$SPID" 2>/dev/null || :; SPID=; }

# the logs the loader sent (xbox_serve.py: build/xbox/logs/<time>-<game>.txt), oldest first
mkdir -p "$T/logs"
n=0
if [ -n "$KIT" ]; then           # start 1's log as the stand-in dashboard kept it, start 2's where the game left it
    $FATX get "$T/hdd.img" t/start1.txt "$T/logs/start1.txt" >/dev/null 2>&1 && n=1
    $FATX get "$T/hdd.img" Games/$FOLDER/mh1log.txt "$T/logs/start2.txt" >/dev/null 2>&1 && [ $n = 1 ] && n=2
else
    for f in $(ls -tr build/xbox/logs/*-$GID.txt 2>/dev/null); do
        [ "$(stat -c %Y "$f")" -ge "$LOGS0" ] || continue
        n=$((n + 1)); cp "$f" "$T/logs/start$n.txt"
    done
fi
$FATX get "$T/hdd.img" Games/$FOLDER/mh1log.txt "$T/logs/on_disk.txt" >/dev/null 2>&1 || :
FAILS=0
check() { if eval "$2"; then echo "PASS $1"; else echo "FAIL $1"; FAILS=$((FAILS + 1)); fi; }
# the "memory card: saved/loaded NAME, N bytes, check H" lines of the game's data file
mcline() { grep -ah "memory card: $1 $CARD/$CARD," "$2" 2>/dev/null | tail -1 | sed 's/.*, \([0-9]*\) bytes, check \([0-9A-F]*\).*/\1 \2/'; }
if [ $MODE = powercut ]; then
    SAVED=$(mcline saved "$T/serial.txt")
    echo "   the game logged: $SAVED (bytes, check)"
    $FATX get "$T/hdd.img" "UDATA/$TID/${TID}00000001/$CARD" "$T/card_after_cut" >/dev/null 2>&1 || :
    DISK=$(python3 -c "
import sys
b = open(sys.argv[1], 'rb').read()
h = 2166136261
for c in b: h = ((h ^ c) * 16777619) & 0xFFFFFFFF
print(len(b), '%08X' % h)" "$T/card_after_cut" 2>/dev/null || :)
    echo "   on the disk after the cut: $DISK"
    check "the game logged a save" "[ -n '$SAVED' ]"
    check "after a power cut 3 s later the save on the disk is the one written (size and check sum)" "[ -n '$SAVED' ] && [ '$SAVED' = '$DISK' ]"
    [ -n "${KEEP:-}" ] || rm -f "$T/hdd.img"
    [ "$FAILS" = 0 ] && echo "all checks passed" || { echo "$FAILS checks failed"; exit 1; }
    exit 0
fi
if [ -z "$KIT" ]; then
    check "the loader downloaded the build once" "[ \"\$(grep -c 'GET /$GID.xbe' '$T/serve.log')\" = 1 ]"
    check "two game starts sent their logs (got $n)" "[ $n -ge 2 ]"
else
    check "two game starts left their logs (got $n)" "[ $n -ge 2 ]"
fi
for k in 1 2; do
    f="$T/logs/start$k.txt"
    check "start $k: reached the Back + Start quit" "grep -q 'Back + Start held: exit at frame' '$f' 2>/dev/null"
    check "start $k: audio opened (the warm relaunch hang was there on hardware)" "grep -q 'audio: driver' '$f' 2>/dev/null"
    check "start $k: no freeze or crash report" "! grep -qE '^FREEZE|^CRASH' '$f' 2>/dev/null"
    grep -h 'watchdog: longest wait\|ac97: init: codec\|Back + Start held\|summary:' "$f" 2>/dev/null | sed "s/^/   start $k: /" || :
done
check "start 2 drew more than 300 frames" \
    "[ \"\$(sed -n 's/.*exit at frame \\([0-9]*\\).*/\\1/p' '$T/logs/start2.txt' 2>/dev/null | head -1)\" -gt 300 ] 2>/dev/null"
if [ $MODE = save ]; then
    SAVED=$(mcline saved "$T/logs/start1.txt"); LOADED=$(mcline loaded "$T/logs/start2.txt")
    echo "   saved: $SAVED   loaded on start 2: $LOADED   (bytes, check)"
    check "start 1 saved the hunter" "[ -n '$SAVED' ]"
    check "start 2 loaded exactly what start 1 saved" "[ -n '$SAVED' ] && [ '$SAVED' = '$LOADED' ]"
    check "start 2 went on from the save into the village" "grep -q 'load finished' '$T/logs/start2.txt'"
    if [ -z "$KIT" ]; then
        check "the loader sent the save to the laptop" "grep -q 'POST /save?game=$GID' '$T/serve.log'"
    else
        check "the save is on the disk (E:\\UDATA)" "$FATX ls '$T/hdd.img' UDATA/$TID/${TID}00000001 2>/dev/null | grep -q '^$CARD '"
    fi
fi
echo "screens (shot_*s.png), serial log, server log, game logs: $T"
[ "$FAILS" = 0 ] && echo "all checks passed" || { echo "$FAILS checks failed"; exit 1; }
