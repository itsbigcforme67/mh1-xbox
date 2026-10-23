#!/bin/sh
# Hidden G village run (agent D tooling): tools/gvil.sh TAG "pad script" SECS [ENV=val ...]
# warp/money/box aids work as in MH1 (RT_LB_WARP, RT_MONEY, RT_BOX_ITEMS); shots go to build/D/g/TAG*.png, log to TAG.log
# (the last line of the log is "EXIT n" with the exit status)
cd "$(dirname "$0")/.." || exit 1
TAG=$1; IN=$2; SECS=$3; shift 3
mkdir -p build/D/g
( ulimit -v 3000000 2>/dev/null   # (under a lower cap, e.g. full_check's, the cap stays)
  [ -z "$INTRO" ] && export RT_VILLAGE_SKIP_INTRO=1     # INTRO=1: play the first-visit event
  env MH1_SAVE_DIR="$PWD/build/D/g/card" RT_NOMOVIE=1 RT_QUEST_TRACE=1 RT_VILLAGE_START=1 RT_NAME=TEST RT_STEP=1 "$@" \
  timeout 600 build/mhg/pc/mhview_g disc/mhg --quest 1 --play --input "$IN" --shot build/D/g/$TAG.png --time "$SECS" > build/D/g/$TAG.log 2>&1
  echo "EXIT $?" >> build/D/g/$TAG.log )
grep -a -v "flSet\|^font:" build/D/g/$TAG.log | tail -${TAILN:-5}
