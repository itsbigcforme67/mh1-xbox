#!/bin/sh
# Co-op over direct connect (ONLINE=1 build, docs/network.md 3.4): N headless instances on
# 127.0.0.1 (a host and N-1 joiners) play quest 131 together; each walks a scripted path.
# Passes when every instance ends with every player in the quest, on the same stage, and
# each player's position as the others see it within 60 units of where that player itself
# says it is. Screenshots: build/show/coop_N_slotK.png (the host's from a high follow camera).
#   tools/test_coop.sh [N|hunt]   (N = 2..4 walking; hunt = 2 players hunt quest 137 to the clear;
#                                  box = the supply box decided by the host;
#                                  handover = the joiner fights, the monster is handed to it; default: all)
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

hunt() {
    # $1 = handover: the host stays at the camp and the joiner fights; Em_Master_Change then hands the monster to the
    # joiner (the player on its area), so the host's copy is driven by the joiner's packets. Otherwise:
    # 2 players hunt quest 137 (one Velocidrome, kind 27, stage 34): the host walks there and fights it with the
    # test aids of test_all_quests (warp next to it, damage x40, no damage taken); the joiner walks there and only
    # watches. The host's machine owns the monster (EMW+0x8C3), so the joiner's copy moves, loses HP and dies only
    # from the host's packets (net_send_em / net_receive_em), and the kill and clear come over the sys channel.
    port=$((PORT + 9))
    fight="RT_PL_WARP_EM=100-90000 RT_DMG_MUL=40"; watch=""; owner=0; tag=hunt
    if [ "$1" = handover ]; then
        port=$((PORT + 11)); owner=1; tag=handover
    fi
    cyc=$(python3 -c "print('idle*60' + (',cam_u*2,idle*30'*4 + ',circle*2,idle*28'*6)*40)")
    common="RT_NOMOVIE=1 RT_QUEST_TRACE=1 RT_NP_EM=30 RT_NP_POS=30 RT_PL_GOD=1 RT_PL_GOTO=60,f RT_PL_TARGET=k27"
    # the joiner sets off later (RT_PL_GOTO from tick 300): the monster's area must see the host first, or
    # Em_Master_Change hands the monster to the joiner (that case is the handover test)
    hin="$cyc"; jin="idle*99999"; hx="$fight $common"; jx="$common RT_PL_GOTO=300,f"
    [ $owner = 1 ] && { hin="idle*99999"; jin="$cyc"; hx="RT_NOMOVIE=1 RT_QUEST_TRACE=1 RT_NP_EM=30 RT_NP_POS=30 RT_PL_GOD=1"; jx="$fight $common"; }
    env $hx RT_PL_LOOK=0,2,3,5,5,5,5,5 \
        timeout 300 "$BIN" "$DISC" --host --quest 137 --players 2 --port $port --mute --input "$hin" \
        --shot build/show/coop_${tag}_slot0.png --time 60 > $OUT/${tag}_0.log 2>&1 &
    p0=$!
    sleep 1
    env $jx RT_WEAPON=1 RT_PL_LOOK=1,2,2,10,10,10,10,10 \
        timeout 300 "$BIN" "$DISC" --join 127.0.0.1 --port $port --mute --input "$jin" \
        --shot build/show/coop_${tag}_slot1.png --time 60 > $OUT/${tag}_1.log 2>&1 &
    p1=$!
    wait $p0 || { echo "coop hunt: host failed"; return 1; }
    wait $p1 || { echo "coop hunt: joiner failed"; return 1; }
    python3 - "$OUT" $tag $owner <<'EOF2'
import re, sys
out, tag, owner = sys.argv[1], sys.argv[2], int(sys.argv[3])
hp, own, clear, pos = {}, {}, {}, {}
for me in (0, 1):
    hp[me], own[me] = {}, set()
    for l in open(f"{out}/{tag}_{me}.log", errors="replace"):
        m = re.match(r"np-em: tick (\d+) me \d+ em (\d+) kind 27 stg \d+ hp (-?\d+) owner (\d+)", l)
        if m:
            hp[me][int(m[1])] = int(m[3])
            if 150 < int(m[1]) < 1500:
                own[me].add(int(m[4]))
        m = re.match(r"rt_flow: tick (\d+) mode 2 step 0 D5 3", l)
        if m and me not in clear:
            clear[me] = int(m[1])
        m = re.match(r"np-pos: tick \d+ me \d+ slot (\d+) stg (\d+)", l)
        if m:
            pos[(me, int(m[1]))] = int(m[2])
ok = True
seq = {me: [v for t, v in sorted(hp[me].items())] for me in (0, 1)}
dist = {me: sorted(set(seq[me]), reverse=True) for me in (0, 1)}
print(f"coop {tag}: Velocidrome HP on the host {dist[0]}, on the joiner {dist[1]}")
if not seq[0] or seq[0][-1] > 0 or seq[1][-1] > 0:
    print(f"coop {tag}: the monster did not die on both"); ok = False
if dist[0] != dist[1]:
    print(f"coop {tag}: the two machines saw different HP values"); ok = False
for me in (0, 1):
    if own[me] != {owner}:
        print(f"coop {tag}: instance {me} had monster owner(s) {own[me]}, expected slot {owner}"); ok = False
for me in (0, 1):
    if me not in clear:
        print(f"coop {tag}: no quest clear on instance {me}"); ok = False
if len(clear) == 2:
    print(f"coop {tag}: quest clear at tick {clear[0]} (host) and {clear[1]} (joiner)")
    if abs(clear[0] - clear[1]) > 30:
        print(f"coop {tag}: the clears are more than a second apart"); ok = False
if owner == 0 and (pos.get((0, 1)) != 34 or pos.get((1, 0)) != 34):
    print(f"coop {tag}: the players do not see each other on stage 34 ({pos})"); ok = False
print(f"coop {tag}: " + ("OK" if ok else "FAILED"))
sys.exit(0 if ok else 1)
EOF2
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

if [ "$1" = hunt ] || [ "$1" = box ]; then
    $1
elif [ "$1" = handover ]; then
    hunt handover
elif [ -n "$1" ]; then
    run "$1" && refusals
else
    run 2 && run 4 && hunt && hunt handover && box && refusals
fi
