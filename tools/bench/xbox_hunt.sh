#!/bin/bash
# G online hunt on the real Xbox (ANNA's part of test_online_g.sh HUNT=1), hidden PC as BOB. Ends with the Xbox off.
cd "$(dirname "$0")/../.."
O="$PWD/build/bench/online"; P="../../MULTI KM/xboxpress/xbox_power.py"; T=tools/bench; S=build/bench
mkdir -p "$O"; bash tools/bench/xbox_hunt_input.sh
pick() { found=0; for i in $(seq 1 14); do $T/xb.sh shot v_list >/dev/null 2>&1
python3 -c "
from PIL import Image, ImageChops, ImageStat; import sys
t=Image.open('$S/tmpl_mhg.png'); im=Image.open('$S/v_list.png').convert('L').crop((300,495,660,522))
sys.exit(0 if ImageStat.Stat(ImageChops.difference(im,t)).mean[0] < 15 else 1)" && { found=1; break; }
if [ $i -le 5 ]; then $T/xb.sh tap down 60; else $T/xb.sh tap up 60; fi; sleep 1.5; done
echo found=$found; [ $found = 1 ] && $T/xb.sh tap a 60; }
. ~/xboxdev/env.sh; (ulimit -v 3000000; python3 tools/build_xbox.py --version mhg --online --gfx nv2a > build/mhg_xbox_online_build.log 2>&1; echo xbox build rc=$?; ONLINE=1 tools/build_pc_mhg.sh > build/check/build_online_hunt.log 2>&1; echo pc build rc=$?; tools/test_online_g.sh mkgsave BOB)
kill $(cat "$O/pc.pid") 2>/dev/null; kill $(cat "$O/server.pid") 2>/dev/null; kill $(cat build/xbox/serve.pid) 2>/dev/null; sleep 1
rm -f "$O"/accounts.sqlite3*; rm -rf "$O/card_PC" "$O/logs_PC"; mkdir -p "$O/card_PC" "$O/logs_PC"; cp -r build/show/g_online_pc_cards/BOB/. "$O/card_PC/"
for n in XBOX PC; do python3 tools/server/mh1_server.py account add --db "$O/accounts.sqlite3" --note "bench $n" > "$O/account_$n.txt"; done
sed -i '/^RT_NET_ID\|^RT_NET_PASS\|^RT_PL_\|^RT_DMG\|^RT_GOTO\|^RT_QUEST\|^RT_NP_\|^RT_MONEY/d' "$O/env.txt"
sed -n 's/^login \([0-9]*\) password \(.*\)/RT_NET_ID=\1\nRT_NET_PASS=\2/p' "$O/account_XBOX.txt" >> "$O/env.txt"
F="RT_PL_GOD=1 RT_PL_TARGET=k6 RT_PL_GOTO=60,f RT_DMG_MUL=2 RT_GOTO_TRACE=1 RT_PL_CARVE=10 RT_QUEST_TRACE=1 RT_NP_POS=150 RT_NP_EM=30 RT_NP_RXEM=1 RT_MONEY=5000"
printf '%s\n' $F RT_PL_GOTO2="1500,21;2500,-1" RT_PL_WARP_EM="600-1500,2600-9000" >> "$O/env.txt"
(nohup python3 tools/server/mh1_server.py serve --config "$O/server.conf" > "$O/server.log" 2>&1 & echo $! > "$O/server.pid")
(nohup python3 -u tools/xbox_serve.py --data dist/xbox/MHG --xbe-g build/mhg/xbox/online/nv2a/default.xbe --env build/bench/online/env.txt --input build/bench/online/ia.txt --test-game mhg > build/xbox/serve.log 2>&1 & echo $! > build/xbox/serve.pid)
sleep 2; "$P" on | tail -1; sleep 16; $T/xb.sh tap a 60; sleep 2.5
pick || { "$P" off; exit 1; }; sleep 45
ID=$(sed -n 's/^login \([0-9]*\) password .*/\1/p' "$O/account_PC.txt"); PW=$(sed -n 's/^login [0-9]* password \(.*\)/\1/p' "$O/account_PC.txt"); IB=$(cat "$O/ib.txt")
(ulimit -v 2500000; env MH1_HIDDEN=1 MH1_SAVE_DIR="$O/card_PC" MH1_LOG_DIR="$O/logs_PC" RT_NAME=BOB RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_NET_HOST=192.168.1.166 RT_NET_PORT=10200 RT_NET_ID=$ID RT_NET_PASS=$PW RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,0;2700,1850,1500,8000;3400,2920,1840,8000" RT_SHOTS=5600,7400,9200,14000,20000 $F RT_NOMOVIE=1 RT_PL_GOTO2="2600,21;3400,-1" RT_PL_WARP_EM="1600-2600,3500-9000" nohup timeout 1100 build/mhg/pc/mhview_g_online disc/mhg --quest 10 --play --online --mute --input "$IB" --shot "$O/pc.png" --time 898 --size 640x480 > "$O/pc.log" 2>&1 & echo $! > "$O/pc.pid")
for k in $(seq 1 12); do sleep 75; $T/xb.sh shot hu$k >/dev/null 2>&1; done
while kill -0 $(cat "$O/pc.pid") 2>/dev/null; do sleep 10; done; sleep 20; $T/xb.sh shot hu_end >/dev/null 2>&1
# the Xbox's log comes with the loader's next start: plain server, power cycle, start the game once, off
kill $(cat build/xbox/serve.pid); sleep 1
(nohup python3 -u tools/xbox_serve.py --data dist/xbox/MHG > build/xbox/serve2.log 2>&1 & echo $! > build/xbox/serve.pid)
"$P" off | tail -1; sleep 8; "$P" on | tail -1; sleep 16; $T/xb.sh tap a 60; sleep 2.5; pick; sleep 25
"$P" off | tail -1; sleep 6; kill $(cat "$O/server.pid"); "$P" state; ping -c1 -W1 192.168.1.115 | tail -2 | head -1
grep -a "creates room\|joins room\|match start\|mcs:" "$O/server.log" | cut -c1-110 | tail -n 12
grep -a "mcs:\|co-op\|D5 3\|back to the town\|memory card: saved" "$O/pc.log" | tail -n 10 | cut -c1-110; ls -t build/xbox/logs | head -1
