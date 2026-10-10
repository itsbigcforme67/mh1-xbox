#!/bin/sh
# Event quests (file download, docs/network.md 5.8): mh1_testserver.py serves one downloadable quest made from your own
# disc (quest 1's mission file renumbered 0xC8 = 200 by tools/mk_event_quest.py; kept in build/, never committed).
# Two headless ONLINE=1 clients download it at the lobby entry (6881 / 6882, byte for byte), ANNA picks "event quest"
# at the guild counter (the level menu's last line, shown only when a quest was downloaded), posts it, BOB joins from
# the quest board, the match starts quest 200 as a co-op quest on both. Then a patch at the login is received and
# refused (the PC cannot apply PS2 code patches). About 3 minutes. Logs: build/show/online_event.
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
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK;1581:dleft*2;1603:dleft*2;1625:ddown*2;1647:ddown*2;1669:ddown*2;1691:circle*2;1751:circle*2;1811:circle*2;2860:square*3;2920:circle*2;2980:circle*2;3040:circle*2" 3700)
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2760:square*3;2820:circle*2;2880:circle*2" 3600)
run() {     # name seconds input warps
    env MH1_SAVE_DIR="$PWD/$OUT/card_$1" RT_NAME=$1 RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_QUEST_TRACE=1 RT_NOMOVIE=1 RT_NET_REGISTERED=1 RT_NET_PORT=$PORT RT_LB_WARP="$4" timeout 300 $RUN $BIN disc/mh1 \
        --quest 10 --play --online --mute --input "$3" --shot $OUT/$1.png --time $2 --size 640x480 > $OUT/$1.log 2>&1
}
run ANNA 128 "$IA" "60,5545,2750,8000;300,1300,1600,C000;2160,3100,2100,8000" &
PA=$!
sleep 2
run BOB 123 "$IB" "60,5545,2750,8000;300,1850,1500,8000;2060,3100,2100,8000" || fail "client BOB stopped early"
wait $PA || { PA=""; fail "client ANNA stopped early"; }
PA=""
kill $SP 2>/dev/null; SP=""
for nm in ANNA BOB; do
    grep -aq "event quest download: done, check $sum" $OUT/$nm.log || fail "$nm did not download the event quest byte for byte"
done
grep -q "room [0-9] property .*(quest 200)" $OUT/server.log || fail "ANNA did not post the event quest"
grep -q "BOB joins room" $OUT/server.log || fail "BOB did not join the room"
for nm in ANNA BOB; do
    grep -aq "match: quest 200, 2 players" $OUT/$nm.log || fail "$nm: no match for quest 200"
    grep -aq "co-op: quest 200, 2 player(s)" $OUT/$nm.log || fail "$nm did not start event quest 200 as a co-op quest"
    grep -aq "village: quest 200 starts on stage" $OUT/$nm.log || fail "$nm: quest 200 did not start"
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
echo "event quest OK: both downloaded quest 200 ($sum), ANNA posted it at the guild counter, BOB joined, both started it as a co-op quest; a patch arrived intact, was refused, the game logged out"
