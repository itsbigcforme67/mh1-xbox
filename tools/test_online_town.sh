#!/bin/sh
# The online town on screen (docs/network.md "The online town"): two headless ONLINE=1 clients
# (build/pc/mhview_online --online) against tools/mh1_testserver.py on 127.0.0.1 (never any other host) go through
# the game's own screens: connecting, Capcom ID select (a new ID), login, the plaza with its lobby list, the
# lobby = the town stage 0x4C. Then: both see each other's hunter at the positions the other has itself (B walks),
# each hears the other's chat in the game's chat log, and a public address is refused (back to the village).
# Then a room is made and joined and its quest starts as a co-op quest. Screenshots in build/show/online_town (never
# committed). About 3 minutes. Build first:
# ONLINE=1 tools/build_pc.sh
# The Windows build under Wine: RUN=wine BIN=build/win/mhview_online.exe tools/test_online_town.sh (ONLINE=1 tools/build_win.sh)
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
RUN=${RUN:-}       # RUN=wine BIN=build/win/mhview_online.exe: the Windows build under Wine
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
        RT_NET_PORT=$PORT "$@" timeout 200 $RUN $BIN disc/mh1 --quest 10 --play --online --mute --input "$in" \
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
# the match is made and the room's quest starts as a co-op quest, ANNA hosting (docs/network.md "The online town").
# The quest (1: deliver 15 of item 77) is cleared by ANNA (RT_PL_ITEMS, warped to the camp's delivery box); after the
# reward screen both log in again (CnetWork+5 = 3), the game saves the hunter to the card and both are back in the
# town, where each sees the other (BOB walks). Each plays the hunter of its own card (tools/test_coop.sh saves).
for nm in ANNA BOB; do
    [ -f build/coop/save_$nm/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_coop.sh saves $nm >/dev/null || fail "could not make $nm's card"
    rm -rf $OUT/card_$nm; mkdir -p $OUT/card_$nm; cp -r build/coop/save_$nm/. $OUT/card_$nm/
done
gold0=$(python3 -c "import sys; sys.path.insert(0, 'tools'); from test_coop_hunt import card_gold; print(card_gold('$OUT/card_ANNA'))")
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
REW=""; t=3300; while [ $t -le 4800 ]; do    # the quest's end and the reward screen (take, end receiving)
    REW="$REW;$t:circle*2;$((t+30)):circle*2;$((t+60)):circle*2;$((t+90)):circle*2;$((t+120)):circle*2;$((t+150)):circle*2;$((t+180)):cross*2;$((t+200)):ddown*2;$((t+220)):circle*2"
    t=$((t + 250))
done
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK;1521:dleft*2;1543:dleft*2;1565:ddown*2;1587:ddown*2;1609:ddown*2;1631:circle*2;1691:circle*2;1751:circle*2;2800:square*3;2860:circle*2;2920:circle*2;2980:circle*2$REW" 6300)
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2700:square*3;2760:circle*2;2820:circle*2$REW;5600:up*45;5650:left*30" 6300)
QUEST="RT_NOMOVIE=1 RT_QUEST_TRACE=1 RT_MC_TRACE=1"
env $QUEST RT_PL_ITEMS=77:15 RT_PL_WARP="150,10350,10640,7000" MH1_SAVE_DIR="$PWD/$OUT/card_ANNA" RT_VILLAGE_START=1 \
    RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_NET_PORT=$PORT RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1300,1600,C000;2200,3100,2100,8000" \
    RT_SHOTS=2700,5900 timeout 300 $RUN $BIN disc/mh1 --quest 10 --play --online --mute --input "$IA" --shot $OUT/ANNA.png --time 207 \
    --size 640x480 > $OUT/ANNA.log 2>&1 &
PA=$!
sleep 2
env $QUEST MH1_SAVE_DIR="$PWD/$OUT/card_BOB" RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_NET_PORT=$PORT \
    RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" RT_SHOTS=2600,5900 timeout 300 $RUN $BIN \
    disc/mh1 --quest 10 --play --online --mute --input "$IB" --shot $OUT/BOB.png --time 205 --size 640x480 > $OUT/BOB.log 2>&1 \
    || fail "client BOB stopped early (room)"
wait $PA || { PA=""; fail "client ANNA stopped early (room)"; }
PA=""
kill $SP 2>/dev/null; SP=""
grep -q "ＡＮＮＡ creates room" $OUT/server2.log || fail "ANNA did not create a room"
grep -q "ＢＯＢ joins room" $OUT/server2.log || fail "BOB did not join the room"
grep -q "match start in room .*ＡＮＮＡ, ＢＯＢ" $OUT/server2.log || fail "no match start"
grep -aq "this machine hosts the session (slot 0)" $OUT/ANNA.log || fail "ANNA is not the session host"
grep -aq "match: quest .*slot 1, game server 127.0.0.1" $OUT/BOB.log || fail "BOB did not get the game server address"
for nm in ANNA BOB; do
    grep -aq "co-op: quest [0-9]*, 2 player(s)" $OUT/$nm.log || fail "$nm did not start the co-op quest"
    grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/$nm.log || fail "$nm did not see the quest cleared"
    grep -aq "online: back to the town after the quest" $OUT/$nm.log || fail "$nm did not go back to the town"
    grep -aq "back to the village" $OUT/$nm.log && fail "$nm went to the offline village after the quest"
done
grep -q "ＡＮＮＡ returns to plaza 1 lobby 1" $OUT/server2.log || fail "ANNA did not return to her lobby"
grep -q "ＢＯＢ returns to plaza 1 lobby 1" $OUT/server2.log || fail "BOB did not return to his lobby"
# the game's own save on the second login (lbc_login_finish_after: McOperationSet(7, 2)) wrote the card
for nm in ANNA BOB; do
    sed -n '/back to the town after the quest/,$p' $OUT/$nm.log | grep -aq "rt_mc: open .*card_$nm/BISLPM-65495MH/BISLPM-65495MH mode 2" \
        || fail "the game did not save $nm's card on the second login"
done
gold1=$(python3 -c "import sys; sys.path.insert(0, 'tools'); from test_coop_hunt import card_gold; print(card_gold('$OUT/card_ANNA'))")
[ "$gold1" -gt "$gold0" ] || fail "ANNA's card has $gold1 zenny after the quest, $gold0 before: the reward was not saved"
# back in the town: each sees the other where the other is (the last trace lines after the return)
after() { sed -n '/back to the town after the quest/,$p' $OUT/$1.log; }
a_bob=$(after ANNA | grep -a 'online: tick [0-9]* slot [0-9] "' | tail -1)
b_me=$(after BOB | grep -a 'online: tick [0-9]* slot [0-9] (me)' | tail -1)
b_anna=$(after BOB | grep -a 'online: tick [0-9]* slot [0-9] "' | tail -1)
[ -n "$a_bob" ] || fail "ANNA does not see BOB in the town after the quest"
[ -n "$b_anna" ] || fail "BOB does not see ANNA in the town after the quest"
[ -n "$b_me" ] || fail "no position of BOB after the quest"
python3 - "$a_bob" "$b_me" <<'PY' || fail "after the quest ANNA sees BOB at '$a_bob', BOB says '$b_me'"
import sys
a = [float(v) for v in sys.argv[1].split(' pos ')[1].split()]
b = [float(v) for v in sys.argv[2].split(' pos ')[1].split()]
sys.exit(0 if max(abs(x - y) for x, y in zip(a, b)) <= 2 else 1)
PY
[ "$(after BOB | grep -a 'slot [0-9] (me)' | sed 's/.* pos //' | sort -u | wc -l)" -gt 1 ] || fail "BOB did not walk after the quest"
# the same room through mh1-server (tools/server, docs/server.md): its lobby (the test server's handling) with
# --lobby-relay answers 6914 with "mh1-relay" and 6916 with its session relay; both players join the relay
python3 tools/server/mh1_server.py serve --lobby-port 0 --lobby-relay --relay-ports 10370-10379 > $OUT/server3.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1-server lobby on 127.0.0.1:\([0-9]*\).*/\1/p' $OUT/server3.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "mh1-server did not start"
run ANNA 125 "$IA" RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1300,1600,C000;2200,3100,2100,8000" RT_SHOTS=3500 &
PA=$!
sleep 2
run BOB 120 "$IB" RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" RT_SHOTS=3400 || fail "client BOB stopped early (relay)"
wait $PA || { PA=""; fail "client ANNA stopped early (relay)"; }
PA=""
kill $SP 2>/dev/null; SP=""
for nm in ANNA BOB; do
    grep -aq "the game server is a session relay: joining it" $OUT/$nm.log || fail "$nm did not take the relay hand-off"
    grep -aq "co-op: quest [0-9]*, 2 player(s)" $OUT/$nm.log || fail "$nm did not start the co-op quest through the relay"
done
grep -aq "this machine hosts" $OUT/ANNA.log && fail "ANNA hosted although the server relays"
# safety: a public address is refused before a socket is opened; the game goes back to the village
env MH1_SAVE_DIR="$PWD/$OUT/card_X" RT_NAME=X RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_NET_HOST=8.8.8.8 RT_NET_PORT=10200 \
    timeout 100 $RUN $BIN disc/mh1 --quest 10 --play --online --mute --shot $OUT/refused.png --time 5 --size 320x240 > $OUT/refused.log 2>&1
grep -aq "net: refusing 8.8.8.8" $OUT/refused.log || fail "8.8.8.8 was not refused"
grep -aq "back to the village" $OUT/refused.log || fail "no fallback to the village after the refusal"
# a public server set in the settings file (online_server) is the player's choice and allowed (192.0.2.1: TEST-NET-1,
# nobody answers); an MH Oldschool address there is still refused
printf 'online_server = 192.0.2.1:10200\nonline_login = 12345678\n' > $OUT/public.ini
env MH1_SAVE_DIR="$PWD/$OUT/card_X" RT_NAME=X RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 \
    timeout 100 $RUN $BIN disc/mh1 --ini $OUT/public.ini --quest 10 --play --online --mute --shot $OUT/public.png --time 3 --size 320x240 > $OUT/public.log 2>&1
grep -aq "connecting to 192.0.2.1 port 10200" $OUT/public.log || fail "the configured public server was not tried"
grep -aq "refusing 192.0.2.1" $OUT/public.log && fail "the configured public server was refused"
printf 'online_server = 34.75.107.68:10200\n' > $OUT/mho.ini
env MH1_SAVE_DIR="$PWD/$OUT/card_X" RT_NAME=X RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 \
    timeout 100 $RUN $BIN disc/mh1 --ini $OUT/mho.ini --quest 10 --play --online --mute --shot $OUT/mho.png --time 3 --size 320x240 > $OUT/mho.log 2>&1
grep -aq "net: refusing 34.75.107.68" $OUT/mho.log || fail "an MH Oldschool address in the settings was not refused"
echo "online town OK: two clients logged in through the game's screens, the plaza, the town; each sees the other; chat both ways; a room made, joined, matched, its quest played as a co-op quest and cleared, the reward saved ($gold0 -> $gold1 zenny), both back in the town seeing each other; the room's quest started through mh1-server's relay; public address refused, a configured public server allowed, MH Oldschool refused"
