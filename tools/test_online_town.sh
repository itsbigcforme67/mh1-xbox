#!/bin/sh
# The online town on screen (docs/network.md "The online town"): two headless ONLINE=1 clients
# (build/pc/mhview_online --online) against tools/mh1_testserver.py on 127.0.0.1 (never any other host) go through
# the game's own screens: connecting, Capcom ID select (a new ID), login, the plaza with its lobby list, the
# lobby = the town stage 0x4C. Then: both see each other's hunter at the positions the other has itself (B walks),
# each hears the other's chat in the game's chat log, and a public address is refused (back to the village).
# Then a room is made and joined and its quest starts as a co-op quest. Screenshots in build/show/online_town (never
# committed). About 3 minutes. Build first:
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
    grep -aq "town step 4" $OUT/$nm.log || fail "$nm did not reach the town"
done
grep -aq 'browser page .* skipped' $OUT/ANNA.log || fail "no browser replacement"
grep -aq 'quest download: none' $OUT/ANNA.log || fail "no lobby entry"
# positions: what each sees of the other at the end = what the other has itself
a_bob=$(grep -a 'slot [0-9] "BOB"' $OUT/ANNA.log | tail -1 | sed 's/.* pos //')
b_bob=$(grep -a 'slot [0-9] (me) "BOB"' $OUT/BOB.log | tail -1 | sed 's/.* pos //')
b_anna=$(grep -a 'slot [0-9] "ANNA"' $OUT/BOB.log | tail -1 | sed 's/.* pos //')
a_anna=$(grep -a 'slot [0-9] (me) "ANNA"' $OUT/ANNA.log | tail -1 | sed 's/.* pos //')
first_bob=$(grep -a 'slot [0-9] (me) "BOB"' $OUT/BOB.log | head -1 | sed 's/.* pos //')
[ -n "$a_bob" ] && [ "$a_bob" = "$b_bob" ] || fail "ANNA sees BOB at '$a_bob', BOB is at '$b_bob'"
[ -n "$b_anna" ] && [ "$b_anna" = "$a_anna" ] || fail "BOB sees ANNA at '$b_anna', ANNA is at '$a_anna'"
[ "$first_bob" != "$b_bob" ] || fail "BOB did not move ($b_bob)"
grep -aq 'chat log: from "BOB".*"hi ANNA, BOB here"' $OUT/ANNA.log || fail "ANNA did not get BOB's chat"
grep -aq 'chat log: from "ANNA".*"hello from ANNA"' $OUT/BOB.log || fail "BOB did not get ANNA's chat"
grep -aq 'chat log: from "ANNA".*"hello from ANNA"' $OUT/ANNA.log || fail "ANNA's own chat did not come back"
# a room: ANNA (registered at the guild, RT_NET_REGISTERED) takes a quest at the guild counter and posts it (the
# rule sheet: 2 players), BOB joins it from the quest board, both go to the departure door (BOB ready, ANNA starts):
# the match is made and the room's quest starts as a co-op quest, ANNA hosting (docs/network.md "The online town")
python3 tools/mh1_testserver.py -v --port 0 > $OUT/server2.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server2.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start again"
LOGIN="200:circle*2;302:circle*2;404:circle*2;556:circle*2;738:square*3"
TALK=""; t=1041; while [ $t -le 1441 ]; do TALK="$TALK;$t:circle*2"; t=$((t + 40)); done
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK;1521:dleft*2;1543:dleft*2;1565:ddown*2;1587:ddown*2;1609:ddown*2;1631:circle*2;1691:circle*2;1751:circle*2;2800:square*3;2860:circle*2;2920:circle*2;2980:circle*2" 3600)
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2700:square*3;2760:circle*2;2820:circle*2" 3500)
run ANNA 125 "$IA" RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1300,1600,C000;2200,3100,2100,8000" RT_SHOTS=3500 &
PA=$!
sleep 2
run BOB 120 "$IB" RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" RT_SHOTS=3400 || fail "client BOB stopped early (room)"
wait $PA || { PA=""; fail "client ANNA stopped early (room)"; }
PA=""
kill $SP 2>/dev/null; SP=""
grep -q "ANNA creates room" $OUT/server2.log || fail "ANNA did not create a room"
grep -q "BOB joins room" $OUT/server2.log || fail "BOB did not join the room"
grep -q "match start in room .*ANNA, BOB" $OUT/server2.log || fail "no match start"
grep -aq "this machine hosts the session (slot 0)" $OUT/ANNA.log || fail "ANNA is not the session host"
grep -aq "match: quest .*slot 1, game server 127.0.0.1" $OUT/BOB.log || fail "BOB did not get the game server address"
for nm in ANNA BOB; do
    grep -aq "co-op: quest [0-9]*, 2 player(s)" $OUT/$nm.log || fail "$nm did not start the co-op quest"
done
# safety: a public address is refused before a socket is opened; the game goes back to the village
env MH1_SAVE_DIR="$PWD/$OUT/card_X" RT_NAME=X RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_NET_HOST=8.8.8.8 RT_NET_PORT=10200 \
    timeout 100 $BIN disc/mh1 --quest 10 --play --online --mute --shot $OUT/refused.png --time 5 --size 320x240 > $OUT/refused.log 2>&1
grep -aq "net: refusing 8.8.8.8" $OUT/refused.log || fail "8.8.8.8 was not refused"
grep -aq "back to the village" $OUT/refused.log || fail "no fallback to the village after the refusal"
echo "online town OK: two clients logged in through the game's screens, the plaza, the town; each sees the other at $a_bob / $a_anna; chat both ways; a room made, joined, matched and its quest started as a co-op quest; public address refused"
