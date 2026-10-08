#!/bin/sh
# bt2.sh SKIPREGEX TEST : like bt.sh but runs tools/TEST.sh
cd "$(dirname "$0")/../.."
export MATCHED_SKIP="$1"
sh tools/build_pc.sh > build/bt1.log 2>&1
sh tools/build_pc.sh > build/bt2.log 2>&1
tail -1 build/bt2.log | cut -c1-60
sh tools/$2.sh 2>&1 | tail -2
