#!/bin/bash
# pq.sh SRCFILE SECONDS FUNC... : run permuter on each function sequentially (-j1)
cd "$(cd "$(dirname "$0")/../.." && pwd)"
S=${B_SCRATCH:-$(cd "$(dirname "$0")/../.." && pwd)/build/b_scratch}
SRC=$1; T=$2; shift 2
for f in "$@"; do
  FS=$SRC python3 $S/mkp.py $SRC $f
  PERM_ASM_DIR="/home/james/claude projects/MH XBOX/mh1-xbox/asm" timeout $T python3 tools/perm.py main $f $S/p_$f.c -j1 --stop-on-zero > $S/perm_$f.log 2>&1
  echo "$f done: $(ls build/perm/$f 2>/dev/null | grep output | sort -t- -k2 -n | head -1)" >> $S/pq.out
done
echo ALLDONE >> $S/pq.out
