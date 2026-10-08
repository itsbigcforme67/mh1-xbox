#!/bin/sh
# bt.sh SKIPREGEX : build the PC with those matched files skipped (twice: the host-symbol weakening needs a second pass), run the quest loop
cd "$(dirname "$0")/../.."
export MATCHED_SKIP="$1"
sh tools/build_pc.sh > build/bt1.log 2>&1
sh tools/build_pc.sh > build/bt2.log 2>&1
tail -1 build/bt2.log | cut -c1-60
sh tools/test_quest_loop.sh 2>&1 | tail -2
