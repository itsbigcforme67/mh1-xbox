#!/bin/sh
# The online town on screen (docs/network.md "The online town"): two headless ONLINE=1 clients
# (build/pc/mhview_online --online) against tools/mh1_testserver.py on 127.0.0.1 (never any other host) go through
# the game's own screens: connecting, Capcom ID select (a new ID), login, the plaza with its lobby list, the
# lobby = the town stage 0x4C. Then: both see each other's hunter at the positions the other has itself (B walks),
# each hears the other's chat in the game's chat log, and a public address is refused (back to the village).
# Screenshots: build/show/online_town_A.png / _B.png (never committed). About 60 s. Build first:
# ONLINE=1 tools/build_pc.sh
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
OUT=build/show/online_town; rm -rf $OUT; mkdir -p $OUT
SP=""; PA=""
fail() { echo "online town test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc.sh"
python3 tools/mh1_testserver.py -v --port 0 > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start"
# the screens: ID select (circle on "new ID"), the handle (circle), confirm (circle), the plaza's first lobby (circle)
IN="idle*200,circle*2,idle*100,circle*2,idle*100,circle*2,idle*150,circle*2,idle*900"
INB="idle*200,circle*2,idle*100,circle*2,idle*100,circle*2,idle*150,circle*2,idle*250,up*45,left*30,idle*600"
run() {     # name seconds input extra-env...
    nm=$1; t=$2; in=$3; shift 3
    env MH1_SAVE_DIR="$PWD/$OUT/card_$nm" RT_NAME=$nm RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_NET_PORT=$PORT "$@" timeout 200 $BIN disc/mh1 --quest 10 --play --online --mute --input "$in" \
        --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
run ANNA 42 "$IN" RT_NET_SAY="450:hello from ANNA" &
PA=$!
sleep 2
run BOB 38 "$INB" RT_NET_SAY="300:hi ANNA, BOB here" || fail "client BOB stopped early"
wait $PA || { PA=""; fail "client ANNA stopped early"; }
PA=""
kill $SP 2>/dev/null; SP=""
for nm in ANNA BOB; do
    grep -q "handle '$nm'" $OUT/server.log || fail "$nm did not log in"
    grep -q "town step 4" $OUT/$nm.log || fail "$nm did not reach the town"
done
grep -q 'browser page .* skipped' $OUT/ANNA.log || fail "no browser replacement"
grep -q 'quest download: none' $OUT/ANNA.log || fail "no lobby entry"
# positions: what each sees of the other at the end = what the other has itself
a_bob=$(grep 'slot [0-9] "BOB"' $OUT/ANNA.log | tail -1 | sed 's/.* pos //')
b_bob=$(grep 'slot [0-9] (me) "BOB"' $OUT/BOB.log | tail -1 | sed 's/.* pos //')
b_anna=$(grep 'slot [0-9] "ANNA"' $OUT/BOB.log | tail -1 | sed 's/.* pos //')
a_anna=$(grep 'slot [0-9] (me) "ANNA"' $OUT/ANNA.log | tail -1 | sed 's/.* pos //')
first_bob=$(grep 'slot [0-9] (me) "BOB"' $OUT/BOB.log | head -1 | sed 's/.* pos //')
[ -n "$a_bob" ] && [ "$a_bob" = "$b_bob" ] || fail "ANNA sees BOB at '$a_bob', BOB is at '$b_bob'"
[ -n "$b_anna" ] && [ "$b_anna" = "$a_anna" ] || fail "BOB sees ANNA at '$b_anna', ANNA is at '$a_anna'"
[ "$first_bob" != "$b_bob" ] || fail "BOB did not move ($b_bob)"
grep -q 'chat log: from "BOB".*"hi ANNA, BOB here"' $OUT/ANNA.log || fail "ANNA did not get BOB's chat"
grep -q 'chat log: from "ANNA".*"hello from ANNA"' $OUT/BOB.log || fail "BOB did not get ANNA's chat"
grep -q 'chat log: from "ANNA".*"hello from ANNA"' $OUT/ANNA.log || fail "ANNA's own chat did not come back"
# safety: a public address is refused before a socket is opened; the game goes back to the village
env MH1_SAVE_DIR="$PWD/$OUT/card_X" RT_NAME=X RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_NET_HOST=8.8.8.8 RT_NET_PORT=10200 \
    timeout 100 $BIN disc/mh1 --quest 10 --play --online --mute --shot $OUT/refused.png --time 5 --size 320x240 > $OUT/refused.log 2>&1
grep -q "net: refusing 8.8.8.8" $OUT/refused.log || fail "8.8.8.8 was not refused"
grep -q "back to the village" $OUT/refused.log || fail "no fallback to the village after the refusal"
echo "online town OK: two clients logged in through the game's screens, the plaza, the town; each sees the other at $a_bob / $a_anna; chat both ways; public address refused (server 127.0.0.1:$PORT)"
