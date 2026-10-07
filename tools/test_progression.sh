#!/bin/sh
# Scripted check of the Village Elder's star levels and the save (PC build).
# Starts from the save tools/test_quest_loop.sh leaves (quest 131 cleared =
# 1-star level), then marks quests cleared with the RT_QCLEAR test aid (what
# f_reward's Quest_clear_bit_set does after a won quest), sleeps in the house
# bed (card save) and checks after CONTINUE what Lb_make_quest_tbl_local
# offers:
#   131-135 cleared            -> urgent 136 (key 88), still 1 star
#   + 136 cleared, saved       -> CONTINUE: 2 stars, no urgent quest
#   + 138, 142 cleared         -> urgent 137 (key 89)
#   + 137 cleared, saved       -> CONTINUE: 3 stars
# Headless, ~30 s on x86. Saves in build/show/prog/card.
cd "$(dirname "$0")/.."
export RT_NOMOVIE=1   # the opening movie would only lengthen the scripted boot (test_movie.sh covers it)
OUT=build/show/prog; mkdir -p $OUT
[ -f build/show/loop/card/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_quest_loop.sh >/dev/null || exit 1
export MH1_SAVE_DIR="$PWD/$OUT/card"; rm -rf "$MH1_SAVE_DIR"; cp -r build/show/loop/card "$MH1_SAVE_DIR"
C=tools/pc_scripts/continue.txt
SAVE=$(python3 tools/mk_input.py $C "2090:square*2;2200:$(cat tools/pc_scripts/bed_save.txt)" 3300)
fail() { echo "progression FAILED: $1 (see $OUT)"; exit 1; }
run() {     # run NAME QCLEAR SCRIPT SECONDS [warps]
    RT_QCLEAR="$2" RT_QUEST_TRACE=1 RT_LB_WARP="$5" build/pc/mhview disc/mh1 --boot --input "$3" \
        --shot $OUT/$1.png --time $4 2> $OUT/$1.log >/dev/null
    grep -m1 "rt_village: level" $OUT/$1.log; grep -m1 "quest list" $OUT/$1.log | sed 's/.*(key/  key/;s/).*//'
}
HOME_BED="400,11225,14400,0;500,2259,745,4001"
run p1 84-87 "$(cat $C)" 60
grep -q "quest list (key 88)" $OUT/p1.log || fail "no urgent 136 after 131-135"
run p2 84-88 "$SAVE" 112 "$HOME_BED"
run p3 "" "$(cat $C)" 60
grep -q "level 1, cleared: 83 84 85 86 87 88$" $OUT/p3.log || fail "2 stars not kept by the save"
grep -q "quest list (key 00)" $OUT/p3.log || fail "urgent quest after 136"
run p4 8a,8e "$(cat $C)" 60
grep -q "quest list (key 89)" $OUT/p4.log || fail "no urgent 137 after 138/142"
run p5 8a,8e,89 "$SAVE" 112 "$HOME_BED"
run p6 "" "$(cat $C)" 60
grep -q "level 2, " $OUT/p6.log || fail "3 stars not kept by the save"
echo "progression OK: 1 -> 2 -> 3 stars, urgent quests 136/137, kept by the save"
