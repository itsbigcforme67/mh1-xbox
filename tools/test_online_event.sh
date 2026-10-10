#!/bin/sh
# Event quests (file download, docs/network.md 5.8): mh1_testserver.py serves one downloadable quest made from your own
# disc (quest 1's mission file renumbered 0xC8 = 200 by tools/mk_event_quest.py; kept in build/, never committed).
# Two headless ONLINE=1 clients download it at the lobby entry (6881 / 6882, byte for byte), ANNA picks "event quest"
# at the guild counter (the level menu's last line, shown only when a quest was downloaded), posts it, BOB joins from
# the quest board, the match starts quest 200 as a co-op quest on both, ANNA delivers its items, both get the reward
# screen (items, money) and are back in the town. Then a patch at the login is received and
# refused (the PC cannot apply PS2 code patches). About 4.5 minutes. Logs: build/show/online_event.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
RUN=${RUN:-}
OUT=build/show/online_event; rm -rf $OUT; mkdir -p $OUT
SP=""; PA=""
fail() { echo "event quest test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc.sh"
[ -x build/pc/mhview ] || fail "build/pc/mhview is missing: run tools/build_pc.sh"
# the event quest: quest 1's mission file from the disc, renumbered
RT_NO_GUI=1 RT_NOMOVIE=1 RT_MISSION_DUMP=$OUT/mission1.bin timeout 60 build/pc/mhview disc/mh1 --quest 1 --mute \
    --shot $OUT/dump.png --time 1 --size 320x240 > $OUT/dump.log 2>&1
[ -s $OUT/mission1.bin ] || fail "could not dump quest 1's mission file"
python3 tools/mk_event_quest.py $OUT/mission1.bin $OUT/event200.bin 0xC8 > /dev/null || fail "mk_event_quest.py"
# each plays the hunter of its own card (tools/test_coop.sh saves, made once): the game saves the reward on the second
# login after the quest (as tools/test_online_town.sh)
for nm in ANNA BOB; do
    [ -f build/coop/save_$nm/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_coop.sh saves $nm >/dev/null || fail "could not make $nm's card"
    rm -rf $OUT/card_$nm; mkdir -p $OUT/card_$nm; cp -r build/coop/save_$nm/. $OUT/card_$nm/
done
gold() { python3 -c "import sys; sys.path.insert(0, 'tools'); from test_coop_hunt import card_gold; print(card_gold('$OUT/card_$1'))"; }
a_gold0=$(gold ANNA); b_gold0=$(gold BOB)
sum=$(python3 -c "
s = 0
for c in open('$OUT/event200.bin', 'rb').read(): s = (s * 31 + c) & 0xFFFFFFFF
print('%08X' % s)")
python3 tools/mh1_testserver.py -v --port 0 --event-quest $OUT/event200.bin > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start"
# as tools/test_online_town.sh's room, with the level menu's last line ("event quest", seven lines down) and 60 ticks
# later from there on
LOGIN="200:circle*2;302:circle*2;404:circle*2;556:circle*2;738:square*3"
TALK=";1041:circle*2;1081:circle*2;1121:circle*2"
t=1130; for k in 1 2 3 4 5 6 7; do TALK="$TALK;$t:ddown*2"; t=$((t + 8)); done
t=1221; while [ $t -le 1501 ]; do TALK="$TALK;$t:circle*2"; t=$((t + 40)); done
# the quest (quest 1's goal: deliver 15 of item 77; ANNA has them, RT_PL_ITEMS, and is warped to the camp's box),
# the reward screen (take, end receiving), both back in the town
REW=""; t=3400; while [ $t -le 4900 ]; do
    REW="$REW;$t:circle*2;$((t+30)):circle*2;$((t+60)):circle*2;$((t+90)):circle*2;$((t+120)):circle*2;$((t+150)):circle*2;$((t+180)):cross*2;$((t+200)):ddown*2;$((t+220)):circle*2"
    t=$((t + 250))
done
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK;1581:dleft*2;1603:dleft*2;1625:ddown*2;1647:ddown*2;1669:ddown*2;1691:circle*2;1751:circle*2;1811:circle*2;2860:square*3;2920:circle*2;2980:circle*2;3040:circle*2$REW" 6300)
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2760:square*3;2820:circle*2;2880:circle*2$REW" 6300)
run() {     # name seconds input warps [env...]
    nm=$1; t=$2; in=$3; w=$4; shift 4
    env "$@" MH1_SAVE_DIR="$PWD/$OUT/card_$nm" RT_NAME=$nm RT_MC_TRACE=1 RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_QUEST_TRACE=1 RT_NOMOVIE=1 RT_NET_REGISTERED=1 RT_NET_PORT=$PORT RT_LB_WARP="$w" timeout 300 $RUN $BIN disc/mh1 \
        --quest 10 --play --online --mute --input "$in" --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
run ANNA 212 "$IA" "60,5545,2750,8000;300,1300,1600,C000;2160,3100,2100,8000" RT_PL_ITEMS=77:15 \
    RT_PL_WARP="150,10350,10640,7000" &
PA=$!
sleep 2
run BOB 208 "$IB" "60,5545,2750,8000;300,1850,1500,8000;2060,3100,2100,8000" || fail "client BOB stopped early"
wait $PA || { PA=""; fail "client ANNA stopped early"; }
PA=""
kill $SP 2>/dev/null; SP=""
for nm in ANNA BOB; do
    grep -aq "event quest download: done, check $sum" $OUT/$nm.log || fail "$nm did not download the event quest byte for byte"
done
grep -q "room [0-9] property .*(quest 200)" $OUT/server.log || fail "ANNA did not post the event quest"
grep -q "ＢＯＢ joins room" $OUT/server.log || fail "BOB did not join the room"
for nm in ANNA BOB; do
    grep -aq "match: quest 200, 2 players" $OUT/$nm.log || fail "$nm: no match for quest 200"
    grep -aq "co-op: quest 200, 2 player(s)" $OUT/$nm.log || fail "$nm did not start event quest 200 as a co-op quest"
    grep -aq "village: quest 200 starts on stage" $OUT/$nm.log || fail "$nm: quest 200 did not start"
    grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/$nm.log || fail "$nm did not see event quest 200 cleared"
    grep -aq "rt_flow: tick [0-9]* mode 5 " $OUT/$nm.log || fail "$nm had no reward screen"
    grep -aq "rt_flow: rewards: [0-9]" $OUT/$nm.log || fail "$nm got no reward items"
    grep -aq "rt_quest: Gold_add" $OUT/$nm.log || fail "$nm got no reward money"
    grep -aq "online: back to the town after the quest" $OUT/$nm.log || fail "$nm did not go back to the town"
    grep -aq "back to the village" $OUT/$nm.log && fail "$nm went to the offline village"
done
grep -q "ＡＮＮＡ returns to plaza 1 lobby 1" $OUT/server.log || fail "ANNA did not return to her lobby"
grep -q "ＢＯＢ returns to plaza 1 lobby 1" $OUT/server.log || fail "BOB did not return to his lobby"
a_gold1=$(gold ANNA); b_gold1=$(gold BOB)
[ "$a_gold1" -gt "$a_gold0" ] || fail "ANNA's card has $a_gold1 zenny after the quest, $a_gold0 before"
[ "$b_gold1" -gt "$b_gold0" ] || fail "BOB's card has $b_gold1 zenny after the quest, $b_gold0 before"
after() { sed -n '/back to the town after the quest/,$p' $OUT/$1.log; }
for nm in ANNA BOB; do
    after $nm | grep -aq "event quest download: done, check $sum" || fail "$nm did not download the event quest again at the lobby entry"
    after $nm | grep -a 'online: tick [0-9]* slot [0-9] "' | grep -aq ' shown$' || fail "$nm does not see the other in the town after the quest"
done
# a patch at the login (6121-6125, random bytes): the client receives it intact (its byte sum checks), the PC refuses
# it (PS2 code patches cannot be applied) and the game logs out with its own message, no hang, no crash
head -c 1300 /dev/urandom > $OUT/patch.bin
python3 tools/mh1_testserver.py -v --port 0 --patch $OUT/patch.bin > $OUT/server_patch.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server_patch.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start (patch)"
run PATCH 20 "idle*200" "" || fail "the client stopped early after the patch"
kill $SP 2>/dev/null; SP=""
grep -aq "the server sent a patch: refused" $OUT/PATCH.log || fail "the patch did not reach the patch step (byte sum)"
grep -aq "client 5/" $OUT/PATCH.log || fail "no logout after the refused patch"
echo "event quest OK: both downloaded quest 200 ($sum), ANNA posted it at the guild counter, BOB joined, both played it to the clear, the reward screen (items, money: ANNA $a_gold0 -> $a_gold1, BOB $b_gold0 -> $b_gold1 zenny, saved) and back to the town; a patch arrived intact, was refused, the game logged out"
