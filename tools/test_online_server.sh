#!/bin/sh
# The whole online loop against mh1-server (tools/server, docs/server.md), on 127.0.0.1 only: two headless ONLINE=1
# clients log in through the game's screens, meet in the town, ANNA posts quest 1 at the guild counter, BOB joins,
# the match goes through mh1-server's session relay (nobody hosts), ANNA clears the quest (delivery, RT_PL_ITEMS),
# both get the reward screen, the game saves the reward to their cards and both are back in the town, each showing
# the other. Each plays the hunter of its own card (tools/test_coop.sh saves). The room is for 2 (the rule sheet's
# players, room rule 0): CARL is refused. About 5 minutes.
#   tools/test_online_server.sh                                                   the Linux build
#   RUN=wine BIN=build/win/mhview_online.exe tools/test_online_server.sh         the Windows build under Wine
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
RUN=${RUN:-}
OUT=build/show/online_server; rm -rf $OUT; mkdir -p $OUT
SP=""; PA=""
fail() { echo "mh1-server test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -f "$BIN" ] || fail "$BIN is missing (ONLINE=1 tools/build_pc.sh, or ONLINE=1 tools/build_win.sh)"
for nm in ANNA BOB; do
    [ -f build/coop/save_$nm/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_coop.sh saves $nm >/dev/null || fail "could not make $nm's card"
    rm -rf $OUT/card_$nm; mkdir -p $OUT/card_$nm; cp -r build/coop/save_$nm/. $OUT/card_$nm/
done
gold() { python3 -c "import sys; sys.path.insert(0, 'tools'); from test_coop_hunt import card_gold; print(card_gold('$OUT/card_$1'))"; }
a_gold0=$(gold ANNA); b_gold0=$(gold BOB)
# the server as an operator would run it: a config file, an account store, one account per player (invite-only)
cat > $OUT/mh1-server.conf <<CONF
[server]
bind = 127.0.0.1
lobby-port = 0
lobby-relay = yes
relay-ports = 10380-10389
db = accounts.sqlite3
verbose = yes
CONF
for nm in ANNA BOB CARL; do
    python3 tools/server/mh1_server.py account add --db $OUT/accounts.sqlite3 --note "test $nm" > $OUT/account_$nm.txt || fail "account add"
done
id_of() { sed -n 's/^login \([0-9]*\) password .*/\1/p' $OUT/account_$1.txt; }
pw_of() { sed -n 's/^login [0-9]* password \(.*\)/\1/p' $OUT/account_$1.txt; }
start_server() {     # log file
    python3 tools/server/mh1_server.py serve --config $OUT/mh1-server.conf > $1 2>&1 &
    SP=$!
    i=0; PORT=""
    while [ $i -lt 50 ] && [ -z "$PORT" ]; do
        PORT=$(sed -n 's/^mh1-server lobby on 127.0.0.1:\([0-9]*\).*/\1/p' $1)
        [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
    done
}
stop_server() { kill $SP 2>/dev/null; wait $SP 2>/dev/null; SP=""; }
start_server $OUT/server.log
[ -n "$PORT" ] || fail "mh1-server did not start"
LOGIN="200:circle*2;302:circle*2;404:circle*2;556:circle*2;738:square*3"
TALK=""; t=1041; while [ $t -le 1441 ]; do TALK="$TALK;$t:circle*2"; t=$((t + 40)); done
REW=""; t=3300; while [ $t -le 4800 ]; do
    REW="$REW;$t:circle*2;$((t+30)):circle*2;$((t+60)):circle*2;$((t+90)):circle*2;$((t+120)):circle*2;$((t+150)):circle*2;$((t+180)):cross*2;$((t+200)):ddown*2;$((t+220)):circle*2"
    t=$((t + 250))
done
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK;1521:dleft*2;1543:dleft*2;1565:ddown*2;1587:ddown*2;1609:ddown*2;1631:circle*2;1691:circle*2;1751:circle*2;2800:square*3;2860:circle*2;2920:circle*2;2980:circle*2$REW" 6300)
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2700:square*3;2760:circle*2;2820:circle*2$REW;5600:up*45;5650:left*30" 6300)
run() {     # name seconds input warps [env...]
    nm=$1; t=$2; in=$3; w=$4; shift 4
    env "$@" MH1_SAVE_DIR="$PWD/$OUT/card_$nm" RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_QUEST_TRACE=1 \
        RT_NOMOVIE=1 RT_MC_TRACE=1 RT_NET_REGISTERED=1 RT_NET_PORT=$PORT RT_NET_ID=$(id_of $nm) RT_NET_PASS=$(pw_of $nm) RT_LB_WARP="$w" RT_SHOTS=${SHOTS:-5900} timeout ${TMO:-360} $RUN $BIN \
        disc/mh1 --quest 10 --play --online --mute --input "$in" --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
run ANNA 207 "$IA" "60,5545,2750,8000;300,1300,1600,C000;2200,3100,2100,8000" RT_PL_ITEMS=77:15 RT_PL_WARP="150,10350,10640,7000" $ENVA &
PA=$!
sleep 2
# CARL (no card, a new hunter) tries the same room from the quest board after BOB: the room is for 2 (ANNA's rule
# sheet, sent as room rule 0), the server refuses him ("満員のため参加できません。", shown by the game)
CJOIN=""; t=2480; while [ $t -le 2880 ]; do CJOIN="$CJOIN;$t:circle*2"; t=$((t + 40)); done
SHOTS=2640,2660,2680,2700 run CARL 118 "$(python3 tools/mk_input.py - "$LOGIN;2440:square*3$CJOIN" 3500)" "60,5545,2750,8000;300,1850,1500,8000" RT_NAME=CARL &
PC=$!
run BOB 205 "$IB" "60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" $ENVB || fail "client BOB stopped early"
wait $PC || fail "client CARL stopped early"
wait $PA || { PA=""; fail "client ANNA stopped early"; }
PA=""
stop_server
grep -q "ＡＮＮＡ creates room" $OUT/server.log || fail "ANNA did not create a room"
grep -q "ＢＯＢ joins room" $OUT/server.log || fail "BOB did not join the room"
grep -q "room 1 rule 0 = 1 (２人)" $OUT/server.log || fail "ANNA's rule sheet did not set room rule 0 to 2 players"
grep -q "cmd 6403 .* 000100020002$" $OUT/server.log || fail "the lobby was not told that room 1 is full (2 of 2)"
grep -q "CARL cannot join room 1: full (2 players)" $OUT/server.log || fail "CARL was not refused from the full 2-player room"
grep -q "CARL joins room" $OUT/server.log && fail "CARL joined the full room"
grep -q "quest 1 started with 2 player(s)" $OUT/server.log || fail "the relay did not start the hunt"
for nm in ANNA BOB; do
    grep -aq "the game server is a session relay: joining it" $OUT/$nm.log || fail "$nm did not join the relay"
    grep -aq "co-op: quest 1, 2 player(s)" $OUT/$nm.log || fail "$nm did not start the co-op quest"
    grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/$nm.log || fail "$nm did not see the quest cleared"
    grep -aq "rt_flow: rewards: [0-9]" $OUT/$nm.log || fail "$nm got no reward items"
    grep -aq "online: back to the town after the quest" $OUT/$nm.log || fail "$nm did not go back to the town"
    grep -aq "back to the village" $OUT/$nm.log && fail "$nm went to the offline village"
    sed -n '/back to the town after the quest/,$p' $OUT/$nm.log | grep -aq "rt_mc: open .*card_$nm/BISLPM-65495MH/BISLPM-65495MH mode 2" \
        || fail "the game did not save $nm's card on the second login"
done
grep -q "ＡＮＮＡ returns to plaza 1 lobby 1" $OUT/server.log || fail "ANNA did not return to her lobby"
grep -q "ＢＯＢ returns to plaza 1 lobby 1" $OUT/server.log || fail "BOB did not return to his lobby"
a_gold1=$(gold ANNA); b_gold1=$(gold BOB)
[ "$a_gold1" -gt "$a_gold0" ] || fail "ANNA's card has $a_gold1 zenny after the quest, $a_gold0 before"
[ "$b_gold1" -gt "$b_gold0" ] || fail "BOB's card has $b_gold1 zenny after the quest, $b_gold0 before"
for nm in ANNA BOB; do
    sed -n '/back to the town after the quest/,$p' $OUT/$nm.log | grep -a 'online: tick [0-9]* slot [0-9] "' | grep -aq ' shown$' \
        || fail "$nm does not show the other in the town after the quest"
done
# a crash, and the server restarted: the same store, the same hunters (their ids), the place kept. Both log in again
# (the id screen offers the stored hunter: circle logs in with it); BOB's game is killed in the town (kill -9: no
# logout), ANNA is told he left; BOB starts again and his login puts him straight back into the lobby (6891), where
# ANNA shows him again
a_id=$(grep -a "ＡＮＮＡ" $OUT/server.log | sed -n "s/.*account '\([A-Z0-9]*\)' handle 'ＡＮＮＡ'.*/\1/p" | head -1)
b_id=$(grep -a "ＢＯＢ" $OUT/server.log | sed -n "s/.*account '\([A-Z0-9]*\)' handle 'ＢＯＢ'.*/\1/p" | head -1)
[ -n "$a_id" ] && [ -n "$b_id" ] || fail "no hunter ids in the server log"
start_server $OUT/server_restart.log
[ -n "$PORT" ] || fail "mh1-server did not start again"
LOGIN2="200:circle*2;302:circle*2;404:circle*2;556:circle*2"
run ANNA 75 "$(python3 tools/mk_input.py - "$LOGIN2" 2300)" "" &
PA=$!
sleep 2
# BOB's game is killed (SIGKILL, as a crash: no logout) 35 s after its start, in the town
TMO="-s KILL 35" run BOB 60 "$(python3 tools/mk_input.py - "$LOGIN2" 1800)" ""
mv $OUT/BOB.log $OUT/BOB_crashed.log
sleep 3
run BOB 30 "$(python3 tools/mk_input.py - "$LOGIN2" 900)" "" || fail "BOB's second start stopped early"
wait $PA || { PA=""; fail "client ANNA stopped early (restart)"; }
PA=""
stop_server
grep -q "account '$a_id' handle 'ＡＮＮＡ'" $OUT/server_restart.log || fail "ANNA did not get her stored hunter ($a_id) after the restart"
grep -q "account '$b_id' handle 'ＢＯＢ'" $OUT/server_restart.log || fail "BOB did not get his stored hunter ($b_id) after the restart"
grep -aq "town step 4" $OUT/BOB_crashed.log || fail "BOB did not reach the town before the crash"
grep -q "6410" $OUT/server_restart.log || fail "nobody was told that BOB left (6410) after his crash"
[ "$(grep -c "ＢＯＢ returns to plaza 1 lobby" $OUT/server_restart.log)" -ge 2 ] || fail "BOB's login after the crash did not go back to his lobby"
grep -a 'slot [0-9] "' $OUT/BOB.log | grep -aq ' shown$' || fail "BOB does not show ANNA after coming back"
echo "mh1-server OK${RUN:+ ($RUN $BIN)}: accounts (invite-only store, config file), login, town, a room for 2 (room rule 0; a third hunter refused), the hunt through the relay to the clear and the reward (ANNA $a_gold0 -> $a_gold1, BOB $b_gold0 -> $b_gold1 zenny, saved), both back in the town; after a server restart the same hunters ($a_id, $b_id), a crashed client back in his lobby"
