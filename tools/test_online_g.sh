#!/bin/sh
# Monster Hunter G online on the PC (docs/network.md 3.12 "The PC G online build"): headless G online clients
# (build/mhg/pc/mhview_g_online --online, VERSION=mhg ONLINE=1) against tools/mh1_testserver.py on 127.0.0.1 (never any
# other host) go through G's own lobby screens: login (61A0 / 61A1), Capcom ID select (a new ID), the info file (6881 /
# 6883 / 6882), the plaza class menu, the plaza (G Plaza 1), its town list (6303 / 6304 / 6302 / 630D per town), and
# town 1 (6305 / 630A), standing in the town (stage 0x4C). TWO=1: a second client (BOB) does the same while the first is there.
# ROOM=1 (two clients, ~9 min): both go into the guild hall (the town's door, square); ANNA at the middle counter (the
# registration flag set as RT_NET_REGISTERED): quest level, the first quest card, "recruit members?" yes, the rule sheet
# (2 hunters), the room made on the server (6401 room list, 6407, the rules 66xx, 6415, 6509); BOB at the left counter
# (spot kind 7): the room list, room 1's quest sheet, join (6406); BOB ready at the departure spot (6504), ANNA departs
# (dialog yes): the match for two (6508 / 6910 / 6911-6916), both leave the lobby server and load quest 1 together (the
# co-op session, net_peer: the room owner hosts). As MH1's tools/test_online_mcs.sh, on the test server.
# Screenshots in build/show/g_online_pc (never committed). Build first: ONLINE=1 tools/build_pc_mhg.sh
cd "$(dirname "$0")/.."
BIN=${BIN:-build/mhg/pc/mhview_g_online}
OUT=build/show/g_online_pc; mkdir -p $OUT; rm -f $OUT/*.log
SP=""; PA=""
fail() { echo "G online PC test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc_mhg.sh"
[ -d disc/mhg ] || fail "no disc/mhg (the owner's G disc files)"
python3 tools/mh1_testserver.py -v --port 0 > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start"
# input: circle through login, the Capcom ID ("new ID", yes), the class menu, the plaza menu (街に入る) and the town
# list (town 1); then "yes" (left, circle) on the town's confirmation
IN="idle*200$(for i in $(seq 14); do printf ',circle*2,idle*150'; done)$(for i in $(seq 12); do printf ',circle*2,idle*120,dleft*2,idle*30,circle*2,idle*150'; done),idle*${HOLD:-900}"
run() {     # name seconds extra-env...
    nm=$1; t=$2; shift 2
    rm -rf $OUT/card_$nm
    env MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$OUT/card_$nm" RT_NAME=$nm RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_NET_PORT=$PORT RT_SHOTS=${SHOTS:-1500,2300,3200,4200} "$@" timeout ${TMO:-400} $BIN disc/mhg --quest 10 --play --online --mute \
        --input "$IN" --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
if [ -n "$ROOM" ]; then
    # the scripted screens up to the town (frame 2620 of the input above), then per client (frames): ANNA squares at the
    # door, circles through the counter talk, the level and the quest card, yes, the rule sheet (2 hunters: left twice),
    # waits, then square + yes at the departure spot until the match; BOB squares at the door, at the left counter (room
    # list, room 1, join: circles), then square (ready) at the departure spot. Warps (town ticks): door, counter, departure.
    BASE=$(python3 -c "
b='$IN'.split(',')
o=[];f=0
for it in b:
    k,c=it.split('*');c=int(c)
    if f+c>2620:
        o.append('idle*%d'%(2620-f)) if k=='idle' and 2620>f else None
        break
    o.append(it);f+=c
print(','.join(o))")
    rep() { i=0; while [ $i -lt $1 ]; do printf "%s" "$2"; i=$((i + 1)); done; }
    IA="$BASE,square*2,idle*270$(rep 9 ',circle*2,idle*148')"
    for k in circle circle dleft dleft ddown ddown ddown circle circle circle; do IA="$IA,$k*2,idle*100"; done
    IA="$IA,idle*2200$(rep 4 ',square*2,idle*120,circle*2,idle*120,circle*2,idle*160'),idle*1500"
    IB="$BASE,square*2,idle*2678,square*2,idle*60$(rep 20 ',circle*2,idle*58'),idle*300$(rep 3 ',square*2,idle*100,circle*2,idle*100'),idle*1500"
    TMO=640; IN="$IA" run ANNA 380 RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,0;300,1120,1950;2850,2920,1840,8000" \
        RT_SHOTS=${SHOTS:-5300,7700,9400} &
    PA=$!
    sleep 1
    IN="$IB" run BOB 378 RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,0;2700,1850,1500,8000;3400,2920,1840,8000" \
        RT_SHOTS=${SHOTS:-5600,7400,9200} || fail "client BOB stopped early (see $OUT/BOB.log)"
    wait $PA || { PA=""; fail "client ANNA stopped early (see $OUT/ANNA.log)"; }
    PA=""
    NAMES="ANNA BOB"
elif [ -n "$TWO" ]; then
    run ANNA 260 &
    PA=$!
    sleep 40
    run BOB 200 || fail "client BOB stopped early (see $OUT/BOB.log)"
    wait $PA || { PA=""; fail "client ANNA stopped early (see $OUT/ANNA.log)"; }
    PA=""
    NAMES="ANNA BOB"
else
    run ANNA 160 || fail "the client stopped early (see $OUT/ANNA.log)"
    NAMES=ANNA
fi
kill $SP 2>/dev/null; SP=""
for nm in $NAMES; do
    grep -q "handle '$nm'" $OUT/server.log || fail "$nm did not log in"
    grep -aq "town step 4" $OUT/$nm.log || fail "$nm did not reach the town"
    grep -aq "slot [0-9] (me) \"$nm\" stage 76 pos [1-9]" $OUT/$nm.log || fail "$nm is not standing in the town (stage 0x4C)"
done
if [ -n "$TWO" ]; then
    grep -aq 'slot [0-9] "BOB" stage 76' $OUT/ANNA.log || fail "ANNA does not see BOB in the town"
    grep -aq 'slot [0-9] "ANNA" stage 76' $OUT/BOB.log || fail "BOB does not see ANNA in the town"
fi
if [ -n "$ROOM" ]; then
    grep -q "ANNA creates room 1 in lobby 1" $OUT/server.log || fail "ANNA made no room at the counter"
    grep -q "room 1 rule 0 = 1" $OUT/server.log || fail "the room's rules were not set (2 hunters)"
    grep -q "cmd 6415 seq [0-9A-F]* res 0 len 6 000101000201" $OUT/server.log || fail "no 6415 (quest attributes) for room 1"
    grep -q "BOB joins room 1 of lobby 1 (2 members)" $OUT/server.log || fail "BOB did not join ANNA's room"
    grep -q "match start in room 1: ANNA, BOB" $OUT/server.log || fail "no match for two"
    for nm in ANNA BOB; do
        grep -aq "guild step\|town state x68 [0-9]* talk" $OUT/$nm.log || fail "$nm: no counter trace"
        grep -aq "online: match: quest 1, 2 players" $OUT/$nm.log || fail "$nm did not get the match (quest 1, 2 players)"
        grep -aq "co-op: quest 1, 2 player(s)" $OUT/$nm.log || fail "$nm did not start quest 1 with the other"
    done
fi
grep -q "game: Monster Hunter G" $OUT/server.log || fail "the server did not take the client for G (61A0)"
grep -q "cmd 6882" $OUT/server.log || fail "no info file download (6882)"
grep -q "cmd 6207" $OUT/server.log || fail "no plaza entry (6207)"
grep -q "cmd 630D" $OUT/server.log || fail "no town list (630D)"
grep -q "cmd 6305" $OUT/server.log || fail "no town entry (6305)"
echo "G online PC test: OK ($NAMES: G's login, Capcom ID, info file, plaza, town list, in the town${TWO:+, each sees the other}${ROOM:+, the guild hall counter: a room for 2, BOB joins, the match, both start quest 1}; server 127.0.0.1:$PORT)"
