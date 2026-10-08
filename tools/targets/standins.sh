#!/bin/sh
# standins.sh : which no-op stand-ins (build/pc/rt_gen.c) are called by which game objects (nm -u)
cd "$(dirname "$0")/../.."
grep -o "weak)) [a-z]* [A-Za-z0-9_]*()" build/pc/rt_gen.c | awk '{print $NF}' | tr -d '()' | sort -u > build/.standins.txt
for o in build/pc/*.o; do
  case "$o" in build/pc/rt_*|build/pc/lb__*|build/pc/sel__*|build/pc/mpeg2_*) continue ;; esac
  nm -u "$o" | awk '{print $2}' | sort -u | comm -12 - build/.standins.txt | sed "s|^|$(basename $o .o) |"
done
