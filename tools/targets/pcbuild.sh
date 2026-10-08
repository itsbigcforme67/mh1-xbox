#!/bin/sh
# pcbuild.sh : run tools/build_pc.sh until it links (the weak-symbol requests of a new link set need up to 3 passes)
cd "$(dirname "$0")/../.."
for i in 1 2 3 4; do
  sh tools/build_pc.sh > build/pcb$i.log 2>&1
  if tail -1 build/pcb$i.log | grep -q "^built build/pc/mhview"; then echo "built after $i pass(es)"; exit 0; fi
done
echo "pcbuild FAILED"; grep -n "multiple def\|undefined ref\|rror:" build/pcb4.log | head -10 | cut -c1-250; exit 1
