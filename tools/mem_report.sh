#!/bin/sh
# Memory report of the PC build at the three points used in docs/xbox.md:
# title (--boot, tick 500), village (CONTINUE of the test_quest_loop.sh
# save, tick 2500), Rathian nest (--quest 10, RT_QUEST_STAGE=1, tick 300).
# Prints the rt_memstat table per run (KB now / peak). Needs a save from
# tools/test_quest_loop.sh for the village run. Headless, ~1 min.
cd "$(dirname "$0")/.."
OUT=build/show/mem; mkdir -p $OUT
run() {   # name tick args...
    n=$1; t=$2; shift 2
    RT_MEM=$t "$@" --shot $OUT/$n.png --time $((t / 30 + 2)) 2> $OUT/$n.log >/dev/null
    echo "== $n (tick $t)"; grep '^memstat:' $OUT/$n.log | sed 's/^memstat: //'
}
run title 500 build/pc/mhview disc/mh1 --boot
[ -d build/show/loop/card ] && MH1_SAVE_DIR="$PWD/build/show/loop/card" \
    run village 2500 build/pc/mhview disc/mh1 --boot --input "$(cat tools/pc_scripts/continue.txt)"
RT_QUEST_STAGE=1 run rathian 300 build/pc/mhview disc/mh1 --quest 10 --play
