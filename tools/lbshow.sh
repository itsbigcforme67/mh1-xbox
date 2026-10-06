#!/bin/sh
# lbshow.sh NAME... : print build/lbauto/NAME.c and its align diff
for n in "$@"; do
  echo "=================== $n"
  sed -n '2,$p' build/lbauto/$n.c | grep -v '^$'
  echo "--- diff"
  cp build/lbauto/$n.c src/lobby/zz_show_$n.c
  CHK_ARGS="--module lobby" python3 tools/align.py src/lobby/zz_show_$n.c $n 2>&1 | head -${LINES_MAX:-30}
  rm -f src/lobby/zz_show_$n.c
done
