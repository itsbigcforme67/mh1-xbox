#!/bin/bash
# al.sh FUNC [file]  -> hunk count + head of align
cd "$(cd "$(dirname "$0")/../.." && pwd)"
S=${B_SCRATCH:-$(cd "$(dirname "$0")/../.." && pwd)/build/b_scratch}
F=${3:-$S/full_static.c}
python3 tools/align.py $F $1 > $S/al.out 2>&1
echo "hunks: $(grep -c '^\(replace\|insert\|delete\)' $S/al.out)"
head -${2:-0} $S/al.out
