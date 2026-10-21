#!/bin/sh
# Monster Hunter G online on the PC (docs/network.md 3.12 "The PC G online build"): headless G online clients
# (build/mhg/pc/mhview_g_online --online, VERSION=mhg ONLINE=1) against tools/mh1_testserver.py on 127.0.0.1 (never any
# other host) go through G's own lobby screens: login (61A0 / 61A1), Capcom ID select (a new ID), the info file (6881 /
# 6883 / 6882), the plaza class menu, the plaza (G Plaza 1), its town list (6303 / 6304 / 6302 / 630D per town), and
# town 1 (6305). TWO=1: a second client (BOB) does the same while the first is there.
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
        RT_NET_PORT=$PORT RT_SHOTS=${SHOTS:-1500,2300,3200,4200} "$@" timeout 400 $BIN disc/mhg --quest 10 --play --online --mute \
        --input "$IN" --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
if [ -n "$TWO" ]; then
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
done
grep -q "game: Monster Hunter G" $OUT/server.log || fail "the server did not take the client for G (61A0)"
grep -q "cmd 6882" $OUT/server.log || fail "no info file download (6882)"
grep -q "cmd 6207" $OUT/server.log || fail "no plaza entry (6207)"
grep -q "cmd 630D" $OUT/server.log || fail "no town list (630D)"
grep -q "cmd 6305" $OUT/server.log || fail "no town entry (6305)"
echo "G online PC test: OK ($NAMES: G's login, Capcom ID, info file, plaza, town list, town entry; server 127.0.0.1:$PORT)"
