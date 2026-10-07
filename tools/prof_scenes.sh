#!/bin/sh
# CPU time per subsystem (RT_PROF=1, src/pc/rt/rt_prof.c) at the busy points
# used in docs/xbox.md, one game tick per drawn frame (RT_STEP=1):
#   village   CONTINUE of the test_quest_loop.sh save, walking about (ticks 2100-2700)
#   rathian   quest 10 on the Rathian's stage, hunter warped to her, GOD, attacking
#   fatalis   quest 103 on the Fatalis' stage, the same
#   movie     the opening movie (title, no input)
# Prints the last 300-tick window of each. Headless, a few minutes.
cd "$(dirname "$0")/.."
OUT=build/show/prof; mkdir -p $OUT
show() { echo "== $1"; grep '^prof:' $OUT/$1.log | tail -17 | sed 's/^prof: //'; }
ATK=$(python3 -c "print(','.join(['idle*40'] + ['triangle*2,idle*14'] * 120))")
fight() {   # name quest
    RT_STEP=1 RT_PROF=1 RT_QUEST_STAGE=1 RT_PL_WARP_EM=30-1900 RT_PL_GOD=1 RT_PL_TARGET=0:0 RT_NOMOVIE=1 \
        build/pc/mhview disc/mh1 --quest $2 --play --input "$ATK" --size 640x480 --audio-dump $OUT/$1.wav --shot $OUT/$1.png --time 40 \
        2> $OUT/$1.log >/dev/null
    show $1
}
[ -d build/show/loop/card ] && {
    WALK=$(python3 tools/mk_input.py tools/pc_scripts/continue.txt "2100:up*200;2300:left*150;2450:down*200" 2700)
    RT_STEP=1 RT_PROF=1 MH1_SAVE_DIR="$PWD/build/show/loop/card" build/pc/mhview disc/mh1 --boot \
        --input "$WALK" --size 640x480 --audio-dump $OUT/village.wav --shot $OUT/village.png --time 90 2> $OUT/village.log >/dev/null
    show village
}
fight rathian 10
fight fatalis 103
RT_STEP=1 RT_PROF=1 build/pc/mhview disc/mh1 --boot --size 640x480 --audio-dump $OUT/movie.wav \
    --shot $OUT/movie.png --time 40 2> $OUT/movie.log >/dev/null
show movie
