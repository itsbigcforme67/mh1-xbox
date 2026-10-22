#!/bin/sh
# Monster Hunter G, PC: the GALLERY's event scenes and the credits' movie clips (src/main/omake/omake_g.c, the host side
# in src/pc/viewer.c gal_scene_*; docs/RELEASE_G.md GB2). Hidden runs from power-on: START, the GAME MENU's third row,
# every gallery entry open (RT_OMAKE), the cursor put on the entry (RT_GAL_PICK), circle.
#   scenes: each of the 15 must load its quest and stage, run to its length less 16 ticks (+ the fade) and return to the
#           gallery; pictures at 60 ticks, the middle and 60 ticks before the end: build/show/gallery/sN_TICK.png
#   credits: the staff roll must start its six movie clips (ROLL01..06.sfd) as their pages come up
# usage: tools/test_gallery_g.sh [SCENE...]   (default all 15 and the credits; "credits" for the credits alone)
# About 4 minutes. Pictures to compare with the real G: tools/g_gallery_ref.sh (PCSX2).
cd "$(dirname "$0")/.."
BIN=${BIN:-build/mhg/pc/mhview_g}
[ -x "$BIN" ] || { echo "no $BIN (tools/build_pc_mhg.sh)"; exit 1; }
OUT=build/show/gallery; mkdir -p $OUT
export MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$OUT/card" RT_OMAKE=0xFFFFFFFF
rm -rf "$MH1_SAVE_DIR"
# START (the opening skipped), the GAME MENU from NEW GAME down to GALLERY, circle; then circle on the picked entry
NAV="idle*600,start*2,idle*200,ddown*2,idle*10,ddown*2,idle*10,circle*2,idle*100,circle*2"
# scene page:cell length (G sub_main 0x4FD100 lengths; pages from 0x4FD300)
TABLE="0:1:0:1055 1:1:1:460 2:1:2:663 3:1:3:725 4:1:4:605 5:1:5:745 10:1:6:1419 6:1:7:1115 7:2:0:1270 8:2:2:1204
11:2:3:2013 9:2:5:942 12:3:0:1830 13:3:1:1103 14:3:2:2013"
WANT="$*"; [ -n "$WANT" ] || WANT="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 credits"
fail=0
for w in $WANT; do
    if [ "$w" = credits ]; then
        RT_GAL_PICK=2:6 timeout 600 $BIN disc/mhg --boot --play --input "$NAV,idle*9000" \
            --shot $OUT/credits.png --time 330 > $OUT/credits.out 2>&1
        n=$(grep -c "credits clip: ROLL0" "$(ls -t build/test_logs/*.log | head -1)")
        if [ "$n" -ge 6 ]; then echo "credits OK ($n clips)"; else echo "credits FAILED ($n clips, $OUT/credits.out)"; fail=1; fi
        continue
    fi
    e=$(echo $TABLE | tr ' ' '\n' | grep "^$w:")
    [ -n "$e" ] || { echo "no scene $w"; continue; }
    IFS=: read S PG CELL LEN <<EOF
$e
EOF
    secs=$((40 + LEN / 30 + 8))
    rm -f $OUT/s${S}_*.png
    RT_NOMOVIE=1 RT_GAL_PICK=$PG:$CELL RT_GAL_SHOTS="60,$((LEN / 2)),$((LEN - 60))" timeout 600 $BIN disc/mhg --boot --play \
        --input "$NAV,idle*$((LEN + 300))" --shot $OUT/s$S.png --time $secs > $OUT/s$S.out 2>&1
    log=$(ls -t build/test_logs/*.log | head -1)
    st=$(sed -n 's/^gallery scene starts at tick \([0-9]*\).*/\1/p' $OUT/s$S.out)
    over=$(sed -n 's/.*gallery: event scene over after \([0-9]*\) ticks.*/\1/p' "$log")
    [ -z "$st" ] || for f in $OUT/s${S}_*.png; do
        [ -f "$f" ] || continue
        t=${f##*_}; t=${t%.png}; mv -f "$f" "$OUT/s${S}_$((t - st)).png"
    done
    if [ -n "$over" ] && [ "$over" -ge $((LEN - 16)) ] && [ "$over" -le $((LEN + 60)) ]; then
        echo "scene $S OK: page $PG cell $CELL, $(grep -o 'quest [0-9]*, stage [0-9]*' "$log" | head -1), ran $over of $LEN ticks"
    else
        echo "scene $S FAILED: page $PG cell $CELL, ran '${over}' of $LEN ticks ($OUT/s$S.out, $log)"; fail=1
    fi
done
exit $fail
