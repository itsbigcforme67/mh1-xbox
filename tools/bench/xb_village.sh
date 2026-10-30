#!/bin/bash
# power on (if needed), start G from the dashboard (list cursor on FOLD), load the save, go to the village
T="$(cd "$(dirname "$0")" && pwd)"; S="$T/../../build/bench"
P="/home/james/claude projects/MULTI KM/xboxpress/xbox_power.py"
"$P" state | grep -q "xbox on" || { "$P" on || exit 1; sleep 12; }
"$T/xb.sh" tap a 60; sleep 2.5
# step down until the highlighted row is "Monster Hunter G" (template tmpl_mhg.png); never launch anything else
found=0
for i in $(seq 1 14); do
  "$T/xb.sh" shot v_list >/dev/null 2>&1
  python3 -c "
from PIL import Image, ImageChops, ImageStat; import sys
t=Image.open('$S/tmpl_mhg.png'); im=Image.open('$S/v_list.png').convert('L').crop((300,495,660,522))
sys.exit(0 if ImageStat.Stat(ImageChops.difference(im,t)).mean[0] < 15 else 1)" && { found=1; break; }
  if [ $i -le 5 ]; then "$T/xb.sh" tap down 60; else "$T/xb.sh" tap up 60; fi; sleep 1.5
done
[ $found = 1 ] || { echo "Monster Hunter G not found in the list"; exit 1; }
"$T/xb.sh" tap a 60; sleep 22
for i in $(seq 1 15); do "$T/xb.sh" shot w >/dev/null 2>&1; python3 -c "
from PIL import Image; im=Image.open('$S/w.png').convert('RGB'); import sys; p=im.getpixel((480,293)); sys.exit(0 if p[1]>100 and p[0]<60 else 1)" && break; "$T/xb.sh" tap start 150; sleep 2; done
for i in 1 2 3 4 5; do "$T/xb.sh" tap b 200; sleep 3; done
"$T/xb.sh" tap down 200; sleep 1.5; "$T/xb.sh" tap b 200; sleep 16
"$T/xb.sh" shot v_done 2>/dev/null | tail -1
