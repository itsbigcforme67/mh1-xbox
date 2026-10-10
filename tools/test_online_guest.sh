#!/bin/sh
# Guest rooms in the online town (docs/network.md 3.5 "Guest rooms"): two headless ONLINE=1 clients against mh1-server
# (tools/server/mh1_server.py, its lobby on 127.0.0.1 only). Both log in, go from the square (stage 0x4C) into the inn
# (0x50) and take the free guest room (0x51: spot kind 18, the action button), so both are in "their" room at the same
# time; ANNA leaves first, then BOB. Checks, as the game decides (Lb_Pl_stg_ck): in the inn and the rooms each hunter
# is alone (the other is known, on the same stage, but not shown); back on the square both are shown again, each where
# the other says it is. About 2 minutes. Logs and pictures in build/show/online_guest (never committed).
#   tools/test_online_guest.sh            (RUN=wine BIN=build/win/mhview_online.exe for the Windows build)
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
RUN=${RUN:-}
OUT=build/show/online_guest; rm -rf $OUT; mkdir -p $OUT
SP=""; PA=""
fail() { echo "guest room test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc.sh"
python3 tools/server/mh1_server.py serve --open --lobby-port 0 > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1-server lobby on 127.0.0.1:\([0-9]*\).*/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "mh1-server did not start"
# host ticks: the login screens, then the action button (square) at the inn's door (spot 12 of the square), at the free
# room's door in the inn (spot kind 18), at the room's door (kind 5) and at the inn's door (kind 5); RT_LB_WARP (town
# ticks, about 650 before the host tick) puts the hunter on each spot first
LOGIN="200:circle*2;302:circle*2;404:circle*2;556:circle*2"
IA=$(python3 tools/mk_input.py - "$LOGIN;1000:square*3;1600:square*3;2200:square*3;2600:square*3" 3900)
IB=$(python3 tools/mk_input.py - "$LOGIN;1050:square*3;1650:square*3;2900:square*3;3300:square*3" 3900)
run() {     # name seconds input warps
    env MH1_SAVE_DIR="$PWD/$OUT/card_$1" RT_NAME=$1 RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_NET_REGISTERED=1 RT_NET_PORT=$PORT RT_LB_WARP="$4" RT_SHOTS=1900 timeout 300 $RUN $BIN disc/mh1 --quest 10 --play \
        --online --mute --input "$3" --shot $OUT/$1.png --time $2 --size 640x480 > $OUT/$1.log 2>&1
}
run BOB 129 "$IB" "150,7080,3030,E001;850,1400,2775,0;2100,2090,1285,8000;2500,1875,3100,C001" &
PA=$!
sleep 2
run ANNA 126 "$IA" "100,7080,3030,E001;800,1400,2775,0;1400,2090,1285,8000;1800,1875,3100,C001" || fail "client ANNA stopped early"
wait $PA || { PA=""; fail "client BOB stopped early"; }
PA=""
kill $SP 2>/dev/null; SP=""
for nm in ANNA BOB; do
    grep -aq "online: stage 80 spot kind 18" $OUT/$nm.log || fail "$nm did not enter the inn"
    grep -aq "online: stage 81 spot kind" $OUT/$nm.log || fail "$nm did not enter the guest room"
done
# both in their room at the same time: each knows the other is on stage 81 and does not show him
grep -aq 'slot [0-9] (me) "ANNA" stage 81' $OUT/ANNA.log || fail "no trace of ANNA in her room"
grep -aq 'slot [0-9] "BOB" stage 81 pos [-0-9 ]*$' $OUT/ANNA.log || fail "ANNA never had BOB in the room at the same time (not shown)"
grep -a 'slot [0-9] "[A-Z]*" stage 8[01] .* shown' $OUT/ANNA.log $OUT/BOB.log && fail "a hunter was shown in the inn or a guest room"
# BOB, still in the room, has ANNA back on the square, not shown
grep -aq 'slot [0-9] "ANNA" stage 76 pos [-0-9 ]*$' $OUT/BOB.log || fail "BOB did not see ANNA leave for the square"
# the end: both on the square, shown, each where the other says it is
pos() { sed 's/.* pos \([-0-9]* [-0-9]* [-0-9]*\).*/\1/'; }
a_bob=$(grep -a 'slot [0-9] "BOB" stage 76 .* shown' $OUT/ANNA.log | tail -1 | pos)
b_bob=$(grep -a 'slot [0-9] (me) "BOB" stage 76' $OUT/BOB.log | tail -1 | pos)
b_anna=$(grep -a 'slot [0-9] "ANNA" stage 76 .* shown' $OUT/BOB.log | tail -1 | pos)
a_anna=$(grep -a 'slot [0-9] (me) "ANNA" stage 76' $OUT/ANNA.log | tail -1 | pos)
[ -n "$a_bob" ] && [ "$a_bob" = "$b_bob" ] || fail "back on the square ANNA shows BOB at '$a_bob', BOB is at '$b_bob'"
[ -n "$b_anna" ] && [ "$b_anna" = "$a_anna" ] || fail "back on the square BOB shows ANNA at '$b_anna', ANNA is at '$a_anna'"
echo "guest rooms OK: both took the free guest room (stage 0x51) at the same time, each alone there and in the inn; back on the square each shows the other ($a_bob / $a_anna)"
