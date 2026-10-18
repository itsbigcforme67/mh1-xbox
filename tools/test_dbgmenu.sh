#!/bin/sh
# The testers' debug menu (src/pc/dbgmenu.c, docs/pc.md "Debug menu"), driven by scripted pad input (Back + R3 opens it),
# headless:
#  1. MH1 village (the save of tools/test_quest_loop.sh): money +2000, an item put in the item box, a blade equipped, then
#     quest 131 started from the menu's quest list; the game is paused while the menu is open (every menu line has the
#     tick it opened at);
#  2. MH1 quest 10 on the Rathian's area: hit volumes on, "kill the monsters in this area" -> the quest is cleared;
#  2b. frame step: one tick per R3 press;
#  3. Monster Hunter G (when build/mhg/pc/mhview_g and disc/mhg exist): quest list, quest 4 started, an item in the pouch;
#     hunter rank and mark-all-cleared rows.
# Pictures of every menu page: build/show/dbgmenu/*_NN.png (RT_DBG_SHOTS). ~1 min.
cd "$(dirname "$0")/.."
export MH1_HIDDEN=1   # no window on the owner's screen, no real controller
BIN=${BIN:-build/pc/mhview}
export RT_NOMOVIE=1
OUT=build/show/dbgmenu; rm -rf $OUT; mkdir -p $OUT/logs
export MH1_LOG_DIR=$OUT/logs
[ -d build/show/loop/card ] || { echo "run tools/test_quest_loop.sh first (it makes the save)"; exit 1; }
cp -r build/show/loop/card $OUT/card
K=tools/dbg_keys.py
fail=0
dlog() { grep -h "debug menu" "$(ls -t $OUT/logs/*.log | head -1)"; }

# 1. village: main rows in the village: 0 quest, 1-2 village, 3 title, 4 pouch, 5 box, 6 money, 7 equipment
S="$(cat tools/pc_scripts/continue.txt),@village1,$(python3 $K "w300 open d6 r r d x x d2 x o u2 x x d5 x o u5 x x w300")"
MH1_SAVE_DIR="$PWD/$OUT/card" RT_DBG_SHOTS=$OUT/v RT_QUEST_TRACE=1 $BIN disc/mh1 --boot --input "$S" --shot $OUT/v_end.png --time 120 \
    2> $OUT/v.err >/dev/null
dlog > $OUT/v.txt
# the "opened" line is written inside the tick that saw the combo (the log's tick count moves on at its end); every
# line after it up to "closed" must carry one and the same tick
t_open=$(sed -n '/opened/,/closed/p' $OUT/v.txt | sed 1d | awk '{print $2}' | sort -u | head -1)
t_close=$(sed -n '/opened/,/closed/p' $OUT/v.txt | sed 1d | awk '{print $2}' | sort -u | tail -1)
if grep -q "equipped kind 6 id 158" $OUT/v.txt && grep -q "item box slot 0 = item 6 x 1" $OUT/v.txt && grep -q "money 3550" $OUT/v.txt; then
    echo "village edits OK: money 1550 -> 3550, item box slot 1 = item 6, blade 158 equipped"
else echo "village edits FAILED (see $OUT/v.txt, $OUT/v.err)"; fail=1; fi
if [ -n "$t_open" ] && [ "$t_open" = "$t_close" ]; then echo "pause OK: every menu step at game tick ${t_open#t}"
else echo "pause FAILED: opened at $t_open, closed at $t_close"; fail=1; fi
if grep -q "start quest 131" $OUT/v.txt && grep -q "village: quest 131 starts on stage 21" $OUT/v.err; then echo "quest start OK: 131 from the menu's list"
else echo "quest start FAILED"; fail=1; fi

# 2. quest 10 on the Rathian's stage (40): main rows in a quest: 15 kill, 18 hit volumes
S="$(python3 $K "w60 open d18 x u3 x w300")"
MH1_SAVE_DIR="$PWD/$OUT/card" RT_DBG_SHOTS=$OUT/q RT_SHOTS=70 RT_QUEST_STAGE=1 RT_QUEST_TRACE=1 $BIN disc/mh1 --quest 10 --input "$S" \
    --shot $OUT/q_end.png --time 12 2> $OUT/q.err >/dev/null
dlog > $OUT/q.txt
if grep -q "the monsters of area 40 take a lethal hit" $OUT/q.txt && grep -q "x06 3 .*monsters 0" $OUT/q.err; then echo "kill OK: quest 10 cleared from the menu"
else echo "kill FAILED (see $OUT/q.txt)"; fail=1; fi

# 2b. frame step (main row 19 in a quest): after it is on, the game runs one tick per R3 press, none while idle
S="$(python3 $K "w60 open d19 x st w10 r3 r3 r3 w20")"
MH1_SAVE_DIR="$PWD/$OUT/card" RT_TICK_TRACE=1 $BIN disc/mh1 --quest 10 --input "$S" --shot $OUT/s_end.png --time 10 --frames 100 \
    2> $OUT/s.err >/dev/null
last=$(grep -E "^T [0-9]+ " $OUT/s.err | tail -1 | awk '{print $2}')
if [ "$last" -ge 63 ] && [ "$last" -le 65 ]; then echo "frame step OK: the game stopped at tick $last (3 steps after the menu)"
else echo "frame step FAILED: last tick $last"; fail=1; fi

# 3. G: main rows 0 quest, 5 pouch
if [ -x build/mhg/pc/mhview_g ] && [ -d disc/mhg ]; then
    S="$(python3 $K "w60 open x d3 x w300 open d5 x x d3 r x w30")"
    MH1_SAVE_DIR="$PWD/$OUT/gcard" RT_DBG_SHOTS=$OUT/g RT_QUEST_TRACE=1 build/mhg/pc/mhview_g disc/mhg --quest 10 --input "$S" \
        --shot $OUT/g_end.png --time 16 2> $OUT/g.err >/dev/null
    dlog > $OUT/g.txt
    if grep -q "start quest 4" $OUT/g.txt && grep -q "village: quest 4 starts" $OUT/g.err && grep -q "pouch (quest) slot 0 = item 14 x 1" $OUT/g.txt; then
        echo "G OK: $(grep -o '[0-9]* quests found' $OUT/g.txt), quest 4 started, item 14 in the pouch"
    else echo "G FAILED (see $OUT/g.txt)"; fail=1; fi
    # 3b. G hunter rank (row 9) and "mark all offline quests cleared" (row 10): MH1-only rows until 18 Oct 2026
    S="$(python3 $K "w60 open d9 r r d x w30")"
    MH1_SAVE_DIR="$PWD/$OUT/gcard" build/mhg/pc/mhview_g disc/mhg --quest 10 --input "$S" --shot $OUT/gr_end.png --time 8 \
        2> $OUT/gr.err >/dev/null
    dlog > $OUT/gr.txt
    if grep -q "hunter rank 2" $OUT/gr.txt && grep -q " 98 offline quests marked cleared" $OUT/gr.txt; then echo "G OK: hunter rank 0 -> 2, 98 offline quests marked cleared"
    else echo "G rank/clear FAILED (see $OUT/gr.txt)"; fail=1; fi
else
    echo "G skipped (no build/mhg/pc/mhview_g or disc/mhg)"
fi
[ $fail = 0 ] && echo "debug menu test OK (pictures in $OUT)" || { echo "debug menu test FAILED"; exit 1; }
