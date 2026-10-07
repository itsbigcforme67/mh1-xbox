#!/bin/sh
# Co-op over direct connect (ONLINE=1 build, docs/network.md 3.4): N headless instances on
# 127.0.0.1 (a host and N-1 joiners) play quest 131 together; each walks a scripted path.
# Passes when every instance ends with every player in the quest, on the same stage, and
# each player's position as the others see it within 60 units of where that player itself
# says it is. Screenshots: build/show/coop_N_slotK.png (the host's from a high follow camera).
#   tools/test_coop.sh [N|hunt2|hunt4|handover|leave|box]
#     N = 2..4 walking; hunt2 / hunt4 / handover / leave = hunts of quest 137 to the clear, the reward and the
#     village with each player's own saved hunter (tools/test_coop_hunt.py); box = the supply box decided by the
#     host; default: all (about 6 minutes)
# Starts only its own processes and stops them (by PID).
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
DISC=${DISC:-disc/mh1}
[ -x "$BIN" ] || { echo "no $BIN: ONLINE=1 tools/build_pc.sh"; exit 1; }
OUT=build/coop; mkdir -p $OUT build/show
PORT=${PORT:-10310}
export RT_NO_GUI=1 RT_NP_POS=30
# never the player's own card: an empty one (no save: RT_WEAPON / RT_PL_LOOK); the hunts copy their own cards
export MH1_SAVE_DIR="$PWD/$OUT/nocard"; rm -rf "$MH1_SAVE_DIR"

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

box() {
    # the supply box is decided by the host (net_send_host / net_receive_host): the joiner takes the first item, then
    # the host tries the same one. Expected: the joiner has it, both see it taken, the host gets nothing.
    port=$((PORT + 10))
    RT_NP_POS=30 RT_PL_WARP="200,9500,9500,7000" timeout 120 "$BIN" "$DISC" --host --quest 137 --players 2 --port $port --mute \
        --input "idle*260,circle*2,idle*20,circle*2,idle*60,circle*2,idle*300" --shot build/show/coop_box_slot0.png --time 20 > $OUT/box_0.log 2>&1 &
    p0=$!
    sleep 1
    RT_NP_POS=30 RT_PL_WARP="40,9500,9500,7000" timeout 120 "$BIN" "$DISC" --join 127.0.0.1 --port $port --mute \
        --input "idle*80,circle*2,idle*20,circle*2,idle*60,circle*2,idle*300" --shot build/show/coop_box_slot1.png --time 20 > $OUT/box_1.log 2>&1 &
    p1=$!
    wait $p0 || { echo "coop box: host failed"; return 1; }
    wait $p1 || { echo "coop box: joiner failed"; return 1; }
    h=$(grep "me 0 slot 0 " $OUT/box_0.log | tail -1 | sed 's/.*box \([0-9A-F]*\) pouch \(.*\)/\1 \2/')
    j=$(grep "me 1 slot 1 " $OUT/box_1.log | tail -1 | sed 's/.*box \([0-9A-F]*\) pouch \(.*\)/\1 \2/')
    echo "coop box: host sees box bits / own first pouch slot: $h; joiner: $j"
    [ "$h" = "00000001 0:0" ] && [ "${j%% *}" = "00000001" ] && [ "${j#* }" != "0:0" ] || { echo "coop box: FAILED"; return 1; }
    echo "coop box: OK"
}

mksave() {
    # a card with one hunter named $1 after quest 131 (test_quest_loop.sh's first run): build/coop/save_$1
    [ -f "$OUT/save_$1/BISLPM-65495MH/BISLPM-65495MH" ] && [ "$OUT/save_$1/BISLPM-65495MH/BISLPM-65495MH" -nt "$BIN" ] && return 0
    D=tools/pc_scripts
    EV="2860:square*2"
    i=0; while [ $i -lt 30 ]; do EV="$EV;$((2930 + 27 * i)):circle*2"; i=$((i + 1)); done
    EV="$EV;3770:square*2"
    S="$(python3 tools/mk_input.py $D/newgame.txt "$EV" 3787),$(cat $D/quest131_hunt.txt)"
    S=$(echo "$S" > $OUT/chain.txt; CUT=8100 python3 tools/mk_input.py $OUT/chain.txt "8129:square*2;8229:$(cat $D/bed_save.txt)" 9500)
    rm -rf "$OUT/save_$1"
    (unset RT_NP_POS; MH1_SAVE_DIR="$PWD/$OUT/save_$1" RT_NOMOVIE=1 RT_NAME=$1 \
     RT_LB_WARP="1300,2290,1000,4000;1360,10901,12409,38AB;2210,10650,15225;2301,11225,14400,0;2391,2259,745,4001" \
     RT_PL_TARGET="0:0,1250:1" RT_PL_WARP="10,12250,10000;2400,6750,11900;2500,10350,10500" \
     RT_PL_WARP_EM=90-520,600,1270-1700,1780 RT_DMG_MUL=40 \
        "$BIN" "$DISC" --boot --input "$S" --shot $OUT/save_$1.png --time 316 2> $OUT/save_$1.log >/dev/null)
    [ -f "$OUT/save_$1/BISLPM-65495MH/BISLPM-65495MH" ] || { echo "coop: could not make the save of $1"; return 1; }
}

hunts() {
    # co-op hunts with each player's own saved hunter (tools/test_coop_hunt.py): $@ = scenarios
    for nm in ANNA BOB CARL DAVE; do mksave $nm || return 1; done
    python3 tools/test_coop_hunt.py "$@" | grep -v "^$"
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

case "$1" in
box) box ;;
hunt2|hunt4|handover|leave) shift 0; hunts "$@" ;;
"") run 2 && run 4 && hunts hunt2 hunt4 handover leave && box && refusals ;;
*) run "$1" && refusals ;;
esac
