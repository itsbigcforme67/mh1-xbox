#!/bin/sh
# Regenerate the split for the given modules (default: all) and build.
# Usage: tools/rebuild.sh [main game ...]
cd "$(dirname "$0")/.." || exit 1
mods="${*:-main select game yn lobby}"
python3 tools/setup_split.py > /dev/null || exit 1
for m in $mods; do
    rm -rf "asm/$m" "build/asm/$m"
    .venv/bin/python -m splat split "config/$m.yaml" > "build/split_$m.log" 2>&1 || { echo "splat failed for $m (build/split_$m.log)"; exit 1; }
done
python3 tools/build.py 2>&1 | grep -v "Warning\|Assembler messages\|macro instruction"
