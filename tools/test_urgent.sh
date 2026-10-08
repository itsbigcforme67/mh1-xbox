#!/bin/sh
# Scripted check that really clearing the Elder's urgent quests raises the
# star level (PC build). Starts from tools/test_quest_loop.sh's save.
#   setup: 132-135 marked cleared with RT_QCLEAR (the 1-star required quests)
#   136 (3 Velociprey): accepted at the Elder, gate, camp -> area 40 with the
#       RT_PL_GOTO aid, hunted (RT_PL_TARGET=k16, WARP_EM, DMG_MUL, GOD),
#       reward, village, bed save -> CONTINUE must say 2 stars (level 1)
#   setup: 138 and 142 marked cleared (the 2-star required quests)
#   137 (Velocidrome, area 34): the same -> CONTINUE must say 3 stars
# The urgent clears go through the game's own f_reward Quest_clear_bit_set.
# Input timings are absolute ticks; D shifts the post-hunt part (the
# Velocidrome takes ~510 ticks longer to kill). Headless, ~40 s on x86.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview}   # RUN=wine BIN=build/win/mhview.exe: the Windows build under Wine
export RT_NOMOVIE=1   # the opening movie would only lengthen the scripted boot (test_movie.sh covers it)
P=build/show/urg; mkdir -p $P
[ -f build/show/loop/card/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_quest_loop.sh >/dev/null || exit 1
export MH1_SAVE_DIR="$PWD/$P/card"; rm -rf "$MH1_SAVE_DIR"; cp -r build/show/loop/card "$MH1_SAVE_DIR"
C=tools/pc_scripts/continue.txt
fail() { echo "urgent FAILED: $1 (see $P)"; exit 1; }
setup() {   # mark quests cleared, sleep in the bed (save)
    S=$(python3 tools/mk_input.py $C "2090:square*2;2200:$(cat tools/pc_scripts/bed_save.txt)" 3300)
    RT_QCLEAR="$1" RT_LB_WARP="400,11225,14400,0;500,2259,745,4001" $RUN $BIN disc/mh1 --boot \
        --input "$S" --shot $P/setup.png --time 112 2>/dev/null >/dev/null
}
hunt() {    # hunt NAME STAGE KIND D: Elder -> gate -> hunt -> reward -> bed save
    D=$4
    EV=""; for t in $(seq 2110 50 2610); do EV="$EV;$t:circle*2"; done; EV="$EV;2700:square*2"
    for t in $(seq 2900 32 $((3500 + D))); do EV="$EV;$t:cam_u*2"; done
    for t in $(seq $((4700 + D)) 70 $((5480 + D))); do EV="$EV;$t:cross*2;$((t + 15)):ddown*2;$((t + 30)):circle*2;$((t + 45)):circle*2"; done
    EV="$EV;$((5680 + D)):square*2;$((5780 + D)):$(cat tools/pc_scripts/bed_save.txt)"
    S=$(python3 tools/mk_input.py $C "${EV#;}" $((7000 + D)))
    RT_QUEST_TRACE=1 RT_LB_WARP="400,10901,12409,38AB;1000,10650,15225;1145,11225,14400,0;1245,2259,745,4001" \
    RT_PL_GOTO="60,$2" RT_PL_TARGET=k$3 RT_PL_WARP_EM=200-1800 RT_DMG_MUL=40 RT_PL_GOD=1 \
        $RUN $BIN disc/mh1 --boot --input "$S" --shot $P/$1.png --size 640x360 \
        --time $(((7000 + D) / 30 + 1)) 2> $P/$1.log >/dev/null
    RT_QUEST_TRACE=1 $RUN $BIN disc/mh1 --boot --input "$(cat $C)" --shot $P/$1_cont.png --time 60 \
        2> $P/$1_cont.log >/dev/null
    grep -m1 "accepted 1" $P/$1.log | sed 's/.*quest/  accepted quest/'
    grep -m1 "D5 3" $P/$1.log >/dev/null && echo "  quest clear (D5 3)"
    grep -m1 "rt_village: level" $P/$1_cont.log
}
setup 84-87
hunt q136 40 16 0
grep -q "quest 136 (accepted 1)" $P/q136.log || fail "136 not accepted"
grep -q "level 1, cleared: 83 84 85 86 87 88$" $P/q136_cont.log || fail "2 stars after a real 136 clear"
setup 8a,8e
hunt q137 34 27 510
grep -q "quest 137 (accepted 1)" $P/q137.log || fail "137 not accepted"
grep -q "level 2, " $P/q137_cont.log || fail "3 stars after a real 137 clear"
echo "urgent OK: real clears of 136 and 137 open 2 and 3 stars, kept by the save"
