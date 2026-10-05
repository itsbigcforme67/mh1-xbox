#!/bin/sh
# plnext.sh FUNC... : draft the functions into src/main/pl/pl_wip.c (m2c) and convert accessors. include/plf.h is restored
# afterwards (pl_draft/plconv append junk prototypes there); the extern declarations m2c found are printed to stdout
# as hints: add proper prototypes to include/plf.h by hand.
cd "$(dirname "$0")/.."
cp include/plf.h /tmp/claude-1000/plf.h.keep
for f in "$@"; do python3 tools/pl_draft.py $f --add >/dev/null 2>&1; done
python3 tools/plconv.py src/main/pl/pl_wip.c >/dev/null 2>&1
python3 tools/plclean.py src/main/pl/pl_wip.c
echo "--- m2c prototypes (new in plf.h, restored): "
diff /tmp/claude-1000/plf.h.keep include/plf.h | grep '^>' | sed 's/^> //' | grep -v "M2C_UNK\\|TODO" 
cp /tmp/claude-1000/plf.h.keep include/plf.h
