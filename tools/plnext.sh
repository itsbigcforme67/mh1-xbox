#!/bin/sh
# plnext.sh FUNC... : draft the functions into src/main/pl/pl_wip.c (m2c), convert accessors, clean plf.h junk
cd "$(dirname "$0")/.."
for f in "$@"; do python3 tools/pl_draft.py $f --add >/dev/null 2>&1; done
python3 tools/plconv.py src/main/pl/pl_wip.c >/dev/null 2>&1
python3 tools/plclean.py src/main/pl/pl_wip.c
