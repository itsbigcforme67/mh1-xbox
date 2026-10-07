#!/bin/sh
# Co-op over direct connect (ONLINE=1 build, docs/network.md 3.4): N headless instances on
# 127.0.0.1 (a host and N-1 joiners) play quest 131 together; each walks a scripted path.
# Passes when every instance ends with every player in the quest, on the same stage, and
# each player's position as the others see it within 60 units of where that player itself
# says it is. Screenshots: build/show/coop_N_slotK.png (the host's from a high follow camera).
#   tools/test_coop.sh [N]      (N = 2..4, default runs 2 then 4)
# Starts only its own processes and stops them (by PID).
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
DISC=${DISC:-disc/mh1}
[ -x "$BIN" ] || { echo "no $BIN: ONLINE=1 tools/build_pc.sh"; exit 1; }
OUT=build/coop; mkdir -p $OUT build/show
PORT=${PORT:-10310}
export RT_NO_GUI=1 RT_NP_POS=30

run() {
    n=$1
    port=$((PORT + n))
    pids=""
    # paths: host up, the joiners left / right / down (stick directions)
    set -- "idle*20,up*60,idle*400" "idle*25,left*70,idle*400" "idle*30,right*50,idle*400" "idle*35,down*40,idle*400"
    timeout 120 "$BIN" "$DISC" --host --quest 131 --players $n --port $port --mute --input "$1" \
        --cam 0,0,0,3.6,0 --follow 1500,1100,-0.6 --shot build/show/coop_${n}_slot0.png --time 8 > $OUT/p${n}_0.log 2>&1 &
    pids="$!"
    sleep 1
    k=1
    shift
    while [ $k -lt $n ]; do
        w=156; [ $k = 2 ] && w=1          # player 2 carries a great sword (another weapon class's motions)
        RT_WEAPON=$w timeout 120 "$BIN" "$DISC" --join 127.0.0.1 --port $port --mute --input "$1" \
            --shot build/show/coop_${n}_slot$k.png --time 8 > $OUT/p${n}_$k.log 2>&1 &
        pids="$pids $!"
        k=$((k + 1))
        shift
        sleep 0.3
    done
    fail=0
    for p in $pids; do
        wait $p || fail=1
    done
    [ $fail = 0 ] || { echo "coop $n: an instance failed"; tail -5 $OUT/p${n}_*.log; return 1; }
    python3 - "$n" "$OUT" <<'EOF'
import re, sys
n, out = int(sys.argv[1]), sys.argv[2]
last = {}     # (me, slot) -> (stg, x, y, z) at the last traced tick
for me in range(n):
    for line in open(f"{out}/p{n}_{me}.log", errors="replace"):
        m = re.match(r"np-pos: tick (\d+) me (\d+) slot (\d+) stg (\d+) pos (\S+) (\S+) (\S+)", line)
        if m:
            last[(int(m[2]), int(m[3]))] = (int(m[4]), float(m[5]), float(m[6]), float(m[7]))
ok = True
for me in range(n):
    for s in range(n):
        if (me, s) not in last:
            print(f"coop {n}: instance {me} never saw player {s}"); ok = False; continue
        st, x, y, z = last[(me, s)]
        st0, x0, y0, z0 = last[(s, s)]
        d = ((x - x0) ** 2 + (z - z0) ** 2) ** 0.5
        print(f"coop {n}: instance {me} sees player {s} at {x:.0f} {z:.0f} stage {st} (player {s} itself: {x0:.0f} {z0:.0f}, off by {d:.0f})")
        if st != st0 or d > 60:
            ok = False
moved = all(((last[(s, s)][1] - 9500) ** 2 + (last[(s, s)][3] - 9850) ** 2) ** 0.5 > 150 for s in range(n))
if not moved:
    print(f"coop {n}: some player did not walk away from the start"); ok = False
print(f"coop {n}: {'OK' if ok else 'FAILED'}")
sys.exit(0 if ok else 1)
EOF
}

refusals() {
    # a public address and an MH Oldschool address are refused before any socket is opened
    for a in 8.8.8.8 34.75.107.68; do
        timeout 30 "$BIN" "$DISC" --join $a --mute > $OUT/refuse.log 2>&1
        grep -q "refusing $a" $OUT/refuse.log || { echo "coop: --join $a was not refused"; return 1; }
    done
    timeout 30 "$BIN" "$DISC" --host 8.8.8.8 --quest 131 --mute > $OUT/refuse.log 2>&1
    grep -q "will not listen on 8.8.8.8" $OUT/refuse.log || { echo "coop: --host 8.8.8.8 was not refused"; return 1; }
    echo "coop: public / MH Oldschool addresses refused"
}

if [ -n "$1" ]; then
    run "$1" && refusals
else
    run 2 && run 4 && refusals
fi
