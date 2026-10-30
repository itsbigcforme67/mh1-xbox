#!/bin/bash
# cold-start loop: power on, start G, after 45 s judge started/hung from the screen, power off
T="$(cd "$(dirname "$0")" && pwd)"; S="$T/../../build/bench"
P="/home/james/claude projects/MULTI KM/xboxpress/xbox_power.py"
cd "/home/james/claude projects/MH XBOX/mh1-xbox"
for c in $(seq 1 ${1:-10}); do
  "$P" on >/dev/null || { echo "cycle $c: power on failed"; exit 1; }
  sleep 12
  $T/xb.sh tap a 60; sleep 2.5
  found=0
  for i in $(seq 1 14); do
    $T/xb.sh shot v_list >/dev/null 2>&1
    python3 -c "
from PIL import Image, ImageChops, ImageStat; import sys
t=Image.open("$S/tmpl_mhg.png"); im=Image.open('$S/v_list.png').convert('L').crop((300,495,660,522))
sys.exit(0 if ImageStat.Stat(ImageChops.difference(im,t)).mean[0] < 15 else 1)" && { found=1; break; }
    if [ $i -le 5 ]; then $T/xb.sh tap down 60; else $T/xb.sh tap up 60; fi; sleep 1.5
  done
  [ $found = 1 ] || { echo "cycle $c: Monster Hunter G not found"; "$P" off; exit 1; }
  $T/xb.sh tap a 60; sleep 45
  $T/xb.sh shot cold$c >/dev/null 2>&1
  m=$(python3 -c "
from PIL import Image, ImageStat; print(int(ImageStat.Stat(Image.open('$S/cold$c.png').convert('L')).mean[0]))")
  if [ "$m" -gt 12 ]; then
    echo "cycle $c: started (screen mean $m)"
    "$P" off >/dev/null || echo "cycle $c: power off check failed"
  else
    echo "cycle $c: BLACK SCREEN (mean $m): pressing the button once to switch off"
    echo press | timeout 3 nc -u -w1 xboxpress.local 24854 >/dev/null
    sleep 8
  fi
  sleep 5
done
