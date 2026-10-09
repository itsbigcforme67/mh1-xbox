#!/bin/sh
# The PC settings menu (src/pc/menu.c), headless with its test aids (RT_MENU_AT / RT_MENU_KEYS / RT_MENU_SAVE):
#  1. the game is paused while the menu is open (the last game tick stays the tick it opened at, then runs on after Esc),
#  2. the changes reach mh1pc.ini (aspect, vsync, fps cap, MSAA, anisotropy, 2D filter, button layout),
#  3. a second start with that file opens the menu once more (back.png shows the saved values read back).
# Uses the save made by tools/test_quest_loop.sh (build/show/loop/card): run that first. Headless, ~30 s.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview}
export RT_NOMOVIE=1
OUT=build/show/menu; mkdir -p $OUT; rm -f $OUT/*.ini $OUT/*.png
[ -d build/show/loop/card ] || { echo "run tools/test_quest_loop.sh first (it makes the save)"; exit 1; }
rm -rf $OUT/card; cp -r build/show/loop/card $OUT/card
export MH1_SAVE_DIR="$PWD/$OUT/card"
INPUT="$(cat tools/pc_scripts/continue.txt),idle*3000"
AT=2500
# 1. paused: no Esc, so the menu stays open to the end of the run
RT_TICK_TRACE=1 RT_MENU_AT=$AT RT_MENU_KEYS="down,down" $BIN disc/mh1 --boot --ini $OUT/a.ini --input "$INPUT" --shot $OUT/open.png --time 130 \
    --size 960x720 2> $OUT/a.log >/dev/null
last=$(grep -E "^T [0-9]+ " $OUT/a.log | tail -1 | awk '{print $2}')
if [ "$last" = "$AT" ]; then echo "pause OK: last game tick $last = the tick the menu opened at"; else echo "pause FAILED: last tick '$last', menu opened at $AT"; exit 1; fi
# 2. all the changes, then Esc (writes the ini; --shot runs only with RT_MENU_SAVE) and the game runs on
KEYS="down,right,down,right,down,right,down,right,down,right,down,right,down,down,right,down,left,esc"
RT_TICK_TRACE=1 RT_MENU_SAVE=1 RT_MENU_AT=$AT RT_MENU_KEYS="$KEYS" $BIN disc/mh1 --boot --ini $OUT/b.ini --input "$INPUT" --shot $OUT/closed.png --frames 60 --time 130 \
    --size 960x720 2> $OUT/b.log >/dev/null
last=$(grep -E "^T [0-9]+ " $OUT/b.log | tail -1 | awk '{print $2}')
if [ "$last" -gt "$AT" ]; then echo "resume OK: ticks went on to $last after Esc"; else echo "resume FAILED: last tick $last"; exit 1; fi
fail=0
for want in "widescreen = 1" "vsync = 0" "fps_cap = 30" "msaa = 2" "filter2d = sharp" "confirm = cross"; do
    grep -q "^$want\$" $OUT/b.ini || { echo "ini FAILED: '$want' not in $OUT/b.ini"; fail=1; }
done
grep -q "^aniso = " $OUT/b.ini || { echo "ini FAILED: no aniso line"; fail=1; }
[ $fail = 0 ] && echo "ini OK: $(grep -E '^(widescreen|vsync|fps_cap|msaa|aniso|filter2d|language|confirm)' $OUT/b.ini | tr '\n' ' ')"
[ $fail = 0 ] || exit 1
# 3. read back
RT_MENU_AT=$AT RT_MENU_KEYS="down" $BIN disc/mh1 --boot --ini $OUT/b.ini --input "$INPUT" --shot $OUT/back.png --time 130 --size 960x720 \
    2> $OUT/c.log >/dev/null
echo "settings test done (pictures in $OUT: open.png closed.png back.png)"
