#!/bin/bash
# Regenerate the Monster Hunter 2 split and rebuild it, one heavy job at a time.
# Usage: tools/mh2_rebuild.sh [main sub_main game ...]   (default: all modules)
#        MH2_PY=/path/to/venv/bin/python    if .venv is not in this tree (git worktrees)
#        MH2_NOSPLIT=1                      reuse asm/mh2, only compile, link and compare
cd "$(dirname "$0")/.." || exit 1
ulimit -v 6000000
PY=${MH2_PY:-.venv/bin/python}
mods="${*:-main sub_main select game lobby yn plsel gm_sub lbguild nethttp test dnas_net dnas_ins}"
python3 tools/mh2_setup.py > /dev/null || exit 1
mkdir -p build/mh2
if [ -z "$MH2_NOSPLIT" ]; then
    for m in $mods; do
        rm -rf "asm/mh2/$m" "build/mh2/asm/mh2/$m"
        nice -n 10 "$PY" -m splat split "config/mh2/$m.yaml" > "build/mh2/split_$m.log" 2>&1 \
            || { echo "splat failed for $m (build/mh2/split_$m.log)"; exit 1; }
    done
fi
for m in $mods; do rm -f "build/mh2/$m.bin"; done     # a failed link must not leave an old file for mh2_check.py
VERSION=mh2 nice -n 10 python3 tools/build.py -j 2 $mods 2>&1 | grep -v "Warning\|Assembler messages\|macro instruction"
