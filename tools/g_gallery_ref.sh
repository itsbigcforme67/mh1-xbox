#!/bin/sh
# Real Monster Hunter G reference videos of the GALLERY's event scenes (and the credits), for the PC port's gallery
# (src/main/omake/omake_g.c, docs/RELEASE_G.md GB2). Agent E, 20 Oct 2026.
#
# The owner's G disc runs in PCSX2 (hidden, tools/run_pcsx2.sh, PINE memory access through tools/pcsx2/pine.py). Once:
# the opening, START, the GAME MENU's GALLERY row with every omake flag poked open (option_w+0x2C4C), saved as state
# slot 7. Each SCENE then loads that state, pokes the gallery's page and cursor (G 0x541E00 page, 0x541E06 cursor),
# presses circle and records the hidden screen (30 fps) to OUT/scene_N.mp4, with every 30th frame as OUT/scene_N_SSS.png.
#
# usage: tools/g_gallery_ref.sh [--out DIR] [--secs S] PAGE:CELL[:TAG[:SECS[:T1,T2,..]]] ...  (T: as TICKS, per entry)
#   PAGE 0-3, CELL the cell on that page (pages 1-3 hold the scenes: page 1 cells 0-7 = scenes 0,1,2,3,4,5,10,6;
#   page 2: 0 = scene 7, 2 = scene 8, 3 = scene 11, 5 = scene 9, 6 = the credits; page 3: 0-2 = scenes 12,13,14)
#   --secs S  how long to record (default 50)      NEWSTATE=1  make the gallery state again
#   TICKS="t1 t2 ..."  instead of a video: a picture at each of these ticks of the scene (its own frame counter, read
#             through PINE: exact even when PCSX2 runs slower than real time), OUT/TAG_tN.png
# Nothing here goes to git: the videos and the state hold Capcom data (default OUT build/E/galref). Run uncapped (PCSX2
# does not start under ulimit -v), alone: the script takes the shared emulator lock (xemu.lock next to the main checkout).
cd "$(dirname "$0")/.."
LOCK=${EMU_LOCK:-$(cd "$(git rev-parse --git-common-dir)/../.." && pwd)/xemu.lock}
if [ -z "$G_GAL_LOCKED" ]; then
    G_GAL_LOCKED=1 exec flock "$LOCK" "$0" "$@"
fi
OUT=build/E/galref; SECS=50
while [ $# -gt 0 ]; do
    case "$1" in
    --out) OUT=$2; shift 2 ;;
    --secs) SECS=$2; shift 2 ;;
    -*) echo "unknown option $1"; exit 2 ;;
    *) break ;;
    esac
done
[ $# -gt 0 ] || { sed -n '2,17p' "$0"; exit 2; }
[ "$(ulimit -v)" = unlimited ] || { echo "PCSX2 cannot start under a memory cap (ulimit -v); run uncapped"; exit 1; }
mkdir -p "$OUT"
export PCSX2_HOME=${PCSX2_HOME:-$PWD/build/E/pcsx2home}
mkdir -p "$PCSX2_HOME/PCSX2"
[ -e "$PCSX2_HOME/PCSX2/bios" ] || ln -s "$HOME/ps2dev/pcsx2/data/PCSX2/bios" "$PCSX2_HOME/PCSX2/bios"
export PINE=${PINE:-28057} DNUM=${DNUM:-97}
SLOT=7
OMAKE=0x35E9AC      # option_w (0x35BD60) + 0x2C4C: the omake flags
GW=0x541E00         # the gallery's work block: +0 page, +6 cursor
ST=$PCSX2_HOME/PCSX2/sstates
K=build/pcsx2/xkey
p() { python3 tools/pcsx2/pine.py --slot $PINE "$@"; }
grab() { ffmpeg -nostdin -v error -y -f x11grab -video_size 640x480 -i :$DNUM -frames:v 1 "$1" </dev/null; }
k() {   # k WAIT KEY...: press the keys, then wait WAIT seconds
    w=$1; shift; ks=""
    for a in "$@"; do ks="$ks $a:150:400"; done
    [ -z "$ks" ] || DISPLAY=:$DNUM $K $ks
    sleep "$w"
}
. tools/emu_lib.sh
RP=
cleanup() { emu_stop $RP; RP=; }
emu_traps cleanup
tools/run_pcsx2.sh --iso "disc/mhg/Monster Hunter G (Japan).iso" --seconds 7200 --log "$OUT/emulog.txt" \
    > "$OUT/pcsx2.txt" 2>&1 &
RP=$!
i=0
until p status >/dev/null 2>&1; do
    i=$((i + 1)); [ $i -lt 60 ] || { echo "PCSX2 did not start (see $OUT/pcsx2.txt)"; exit 1; }
    sleep 1
done
echo "PCSX2 up"
if [ -n "$NEWSTATE" ] || ! ls "$ST"/*.0$SLOT.p2s >/dev/null 2>&1; then
    # the opening movie, START on the title, then the GAME MENU: GALLERY is the third row (two downs from NEW GAME on
    # a card without a save)
    k 35; k 5 Return
    grab "$OUT/menu.png"
    p w32 $OMAKE 0xFFFFFFFF
    k 1 Down; k 1 Down; k 6 c
    grab "$OUT/gallery.png"
    p save $SLOT
    sleep 5
    echo "saved the gallery state ($OUT/gallery.png)"
fi
for S in "$@"; do
    IFS=: read PG CELL TAG SS TK <<EOF
$S
EOF
    TAG=${TAG:-p${PG}c${CELL}}; SS=${SS:-$SECS}
    # the first load after PCSX2 starts can fail (PINE op 10): then PCSX2 runs on from power-on; try again
    p load $SLOT 2>/dev/null || { sleep 5; p load $SLOT; } || { echo "$TAG: the save state did not load"; continue; }
    sleep 3
    p w8 $GW "$PG" ";" w16 $((GW + 6)) "$CELL"
    k 1
    rm -f "$OUT/${TAG}"_*.png
    [ -z "$TK" ] || TICKS=$(echo "$TK" | tr ',' ' ')
    if [ -n "$TICKS" ]; then    # TICKS="t1 t2 ...": a picture at each of these ticks of the scene (its frame counter, G 0x541E02)
        DISPLAY=:$DNUM $K c:150:0
        for T in $TICKS; do
            i=0
            while :; do
                v=$(p r16 0x541E02 2>/dev/null) || { echo "PCSX2 is gone"; exit 1; }
                [ "$(echo "$v" | awk '{printf "%d", $2}')" -lt "$T" ] || break
                i=$((i + 1)); [ $i -lt 3000 ] || break
                sleep 0.05
            done
            grab "$OUT/${TAG}_t$T.png"
        done
        echo "$TAG: pictures at scene ticks $TICKS: $OUT/${TAG}_tN.png"
        sleep 4
        continue
    fi
    ffmpeg -nostdin -v error -y -f x11grab -framerate 30 -video_size 640x480 -i :$DNUM -t "$SS" "$OUT/$TAG.mp4" </dev/null &
    FP=$!
    sleep 0.5
    DISPLAY=:$DNUM $K c:150:0
    wait $FP
    ffmpeg -nostdin -v error -y -i "$OUT/$TAG.mp4" -vf "select=not(mod(n\\,30))" -vsync vfr "$OUT/${TAG}_%03d.png"
    echo "$TAG: $OUT/$TAG.mp4, $(ls "$OUT/${TAG}"_*.png | wc -l) pictures $OUT/${TAG}_NNN.png (one per second)"
done
cleanup
