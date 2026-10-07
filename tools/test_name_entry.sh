#!/bin/sh
# Scripted check of the game's own on-screen keyboard on the PC build:
# NEW GAME -> character screen -> NAME -> the soft keyboard is driven with
# the pad only (d-pad moves the cursor, circle types: A, then B; start
# confirms). RT_NAME is NOT set. Passes when the keyboard buffer reached
# "AAB" and the confirmed name is "AAB" (RT_SK_TRACE log lines); the two
# screenshots in build/show/name/ show the keyboard and the NAME row
# (look at them). Headless, about 1 minute.
cd "$(dirname "$0")/.."
OUT=build/show/name; mkdir -p $OUT
D=tools/pc_scripts
# the base script is cut where the keyboard is up (tick 1010)
K="1040:dright*2;1070:circle*4;1110:dright*2;1140:circle*4"
S=$(CUT=1010 python3 tools/mk_input.py $D/newgame.txt "$K" 1200)
RT_SK_TRACE=1 timeout 200 build/pc/mhview disc/mh1 --boot --input "$S" --shot $OUT/keyboard.png --time 42 \
    2> $OUT/run1.log >/dev/null
S=$(CUT=1010 python3 tools/mk_input.py $D/newgame.txt "$K;1180:start*2" 1260)
RT_SK_TRACE=1 timeout 200 build/pc/mhview disc/mh1 --boot --input "$S" --shot $OUT/name.png --time 44 \
    2> $OUT/run2.log >/dev/null
grep -q "sk: text now 3 bytes: 41 41 42" $OUT/run1.log || { echo "name entry FAILED: AAB not typed (see $OUT)"; exit 1; }
grep -q 'sk: confirmed "AAB"' $OUT/run2.log || { echo "name entry FAILED: start did not confirm AAB (see $OUT)"; exit 1; }
echo "name entry OK: typed AAB with the pad, start confirmed it"
