#!/bin/sh
# Two headless ONLINE=1 PC clients hunt together on the game's OWN in-hunt protocol (docs/network.md 3.10): mh1-server
# with --lobby-mcs (6914 "mh1-mcs", 6916 = its mcs relay, tools/server/mcs.py), the PCs run the PS2's session code
# (rt_mcs.c: cmcs_* login, mcsls sync / ping / start, the AQ packets) instead of net_peer frames. The same scripted
# screens as test_online_server.sh: login, town, ANNA posts quest 1 (a room for 2 with a password), BOB joins, the
# match, both load the quest, ANNA clears it (delivery, RT_PL_ITEMS), both see the clear and go back to the town.
# 127.0.0.1 only. About 4 minutes.
# KUTKU=1: quest 4 instead ("怪鳥イャンクック襲来！", the fifth card): both follow the Kut-Ku, fight it in turns (the
# monster passes ANNA -> BOB -> ANNA as each leaves its area), kill it, carve; tools/hunt_sync.py compares both logs (~9 min).
# KUTKU=1 LEAVE=1: BOB's game is killed (SIGKILL) at quest tick 2100 while he owns the monster; ANNA finishes alone.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
RUN=${RUN:-}
OUT=build/show/online_mcs; rm -rf $OUT; mkdir -p $OUT
SP=""; PA=""
fail() { echo "mcs hunt test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -f "$BIN" ] || fail "$BIN is missing (ONLINE=1 tools/build_pc.sh)"
for nm in ANNA BOB; do
    [ -f build/coop/save_$nm/BISLPM-65495MH/BISLPM-65495MH ] || tools/test_coop.sh saves $nm >/dev/null || fail "could not make $nm's card"
    rm -rf $OUT/card_$nm; mkdir -p $OUT/card_$nm; cp -r build/coop/save_$nm/. $OUT/card_$nm/
done
MCSPORT=${MCSPORT:-$(python3 tools/free_ports.py)}     # a free one (tools/free_ports.py), not a fixed port another run may hold
cat > $OUT/mh1-server.conf <<CONF
[server]
bind = 127.0.0.1
lobby-port = 0
lobby-mcs = yes
mcs-port = $MCSPORT
db = accounts.sqlite3
verbose = yes
CONF
for nm in ANNA BOB; do
    python3 tools/server/mh1_server.py account add --db $OUT/accounts.sqlite3 --note "test $nm" > $OUT/account_$nm.txt || fail "account add"
done
id_of() { sed -n 's/^login \([0-9]*\) password .*/\1/p' $OUT/account_$1.txt; }
pw_of() { sed -n 's/^login [0-9]* password \(.*\)/\1/p' $OUT/account_$1.txt; }
python3 tools/server/mh1_server.py serve --config $OUT/mh1-server.conf > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1-server lobby on 127.0.0.1:\([0-9]*\).*/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "mh1-server did not start"
LOGIN="200:circle*2;302:circle*2;404:circle*2;556:circle*2;738:square*3"
TALK=""; t=1041; while [ $t -le 1441 ]; do TALK="$TALK;$t:circle*2"; t=$((t + 40)); done
REW=""; t=3500; while [ $t -le 7000 ]; do
    REW="$REW;$t:circle*2;$((t+30)):circle*2;$((t+60)):circle*2;$((t+90)):circle*2;$((t+120)):circle*2;$((t+150)):circle*2;$((t+180)):cross*2;$((t+200)):ddown*2;$((t+220)):circle*2"
    t=$((t + 250))
done
if [ -n "$KUTKU" ]; then    # the Kut-Ku quest (card 5 of 5: dleft on the first card), the fight and the carve (KUTKU=1 above)
    CARD=";1262:dleft*2"
    # from the hunt on, for both: attacks (right stick up) every 30 ticks and the reward screens' pattern (circle = carve
    # in the hunt); who hits when is decided by the warps (RT_PL_WARP_EM, below): a hunter away from the monster swings
    # at the air
    REW=""; t=3500; while [ $t -le 15000 ]; do
        REW="$REW;$t:circle*2;$((t+15)):cam_u*2;$((t+30)):circle*2;$((t+45)):cam_u*2;$((t+60)):circle*2;$((t+75)):cam_u*2"
        REW="$REW;$((t+90)):circle*2;$((t+105)):cam_u*2;$((t+120)):circle*2;$((t+135)):cam_u*2;$((t+150)):circle*2"
        REW="$REW;$((t+165)):cam_u*2;$((t+180)):cross*2;$((t+200)):ddown*2;$((t+220)):circle*2;$((t+235)):cam_u*2"
        t=$((t + 250))
    done
    END=15600
fi
IA=$(python3 tools/mk_input.py - "$LOGIN$TALK$CARD;1521:dleft*2;1543:dleft*2;1565:ddown*2;1600:dright*2;1625:ddown*2;1650:circle*2;1720:ddown*2;1745:ddown*2;1770:circle*2;1830:circle*2;1890:circle*2;2800:square*3;2860:circle*2;2920:circle*2;2980:circle*2$REW" ${END:-7600})
JOIN=""; t=2000; while [ $t -le 2450 ]; do JOIN="$JOIN;$t:circle*2"; t=$((t + 50)); done
IB=$(python3 tools/mk_input.py - "$LOGIN;1950:square*3$JOIN;2700:square*3;2760:circle*2;2820:circle*2$REW" ${END:-7600})
run() {     # name seconds input warps [env...]
    nm=$1; t=$2; in=$3; w=$4; shift 4
    env "$@" MH1_SAVE_DIR="$PWD/$OUT/card_$nm" RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_QUEST_TRACE=1 \
        RT_NOMOVIE=1 RT_NET_REGISTERED=1 RT_NET_PORT=$PORT RT_NET_ID=$(id_of $nm) RT_NET_PASS=$(pw_of $nm) RT_LB_WARP="$w" \
        RT_NP_POS=150 RT_NP_EM=${NPEM:-150} RT_SHOTS=${SHOTS:-3300,3420} timeout ${TMO:-400} $RUN $BIN \
        disc/mh1 --quest 10 --play --online --mute --input "$in" --shot $OUT/$nm.png --time $t --size 640x480 > $OUT/$nm.log 2>&1
}
AIDS_A="RT_PL_ITEMS=77:15 RT_PL_WARP=${WARP_A:-150,10350,10640,7000}"; AIDS_B=""; SECS=270
if [ -n "$KUTKU" ]; then    # quest ticks: both follow the Kut-Ku (from 60). ANNA fights alone (600-1500), goes back to the
    # camp (stage 21, 1500) so the monster passes to BOB, who fights alone (1600-2600) and goes back in turn (2600: it passes
    # to ANNA again, who follows it from 2500); BOB follows it again from 3400; both fight to the kill and carve (the warp
    # then puts them at the carve point)
    [ -n "$LEAVE" ] && MUL=${MUL:-40}     # ANNA finishes alone
    F="RT_PL_GOD=1 RT_PL_TARGET=k${KIND:-6} RT_PL_GOTO=60,f RT_DMG_MUL=${MUL:-15} RT_GOTO_TRACE=1 RT_PL_CARVE=10"
    AIDS_A="$F RT_PL_GOTO2=1500,21;2500,-1 RT_PL_WARP_EM=600-1500,2600-9000"
    AIDS_B="$F RT_PL_GOTO2=2600,21;3400,-1 RT_PL_WARP_EM=1600-2600,3500-9000"
    SECS=520; TMO=${TMO:-640}; NPEM=30
fi
run ANNA $SECS "$IA" "60,5545,2750,8000;300,1300,1600,C000;2200,3100,2100,8000" $AIDS_A \
    RT_SK_TEXT=MH1PASS7 RT_SK_TRACE=1 $ENVA &
PA=$!
sleep 2
if [ -n "$LEAVE" ]; then     # KUTKU=1 LEAVE=1: BOB's game is killed (SIGKILL, as a crash or a pulled plug) at quest tick 2100,
    # while he owns the monster and ANNA is at the camp; ANNA comes back at 2500 and must take it over and finish
    run BOB $((SECS - 2)) "$IB" "60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" $AIDS_B RT_SK_TEXT=MH1PASS7 \
        RT_SK_TRACE=1 $ENVB &
    PB=$!
    tree() { for c in $(pgrep -P $1); do tree $c; done; echo $1; }
    i=0
    while [ $i -lt 400 ] && ! grep -aq "rt_player: tick 21[0-9][0-9] warped" $OUT/BOB.log 2>/dev/null; do sleep 1; i=$((i + 1)); done
    grep -aq "rt_player: tick 21[0-9][0-9] warped" $OUT/BOB.log || fail "BOB never fought (quest tick 2100)"
    for p in $(tree $PB); do [ "$(ps -o comm= -p $p)" = mhview_online ] && kill -9 $p && echo "BOB's game (pid $p) killed"; done
    wait $PB 2>/dev/null
else
run BOB $((SECS - 2)) "$IB" "60,5545,2750,8000;300,1850,1500,8000;2100,3100,2100,8000" $AIDS_B RT_SK_TEXT=MH1PASS7 RT_SK_TRACE=1 $ENVB \
    || fail "client BOB stopped early"
fi
wait $PA || { PA=""; fail "client ANNA stopped early"; }
PA=""
kill $SP 2>/dev/null; wait $SP 2>/dev/null; SP=""
grep -q "mcs: match room 1" $OUT/server.log || fail "the lobby did not hand the match to the mcs relay"
grep -q "mcs: login .*: player 0 of match" $OUT/server.log || fail "ANNA did not log in to the mcs relay"
grep -q "mcs: login .*: player 1 of match" $OUT/server.log || fail "BOB did not log in to the mcs relay"
if [ -n "$LEAVE" ]; then
    grep -aq "mcs: player 1 dropped out" $OUT/ANNA.log || fail "ANNA's game never saw BOB drop out"
    grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/ANNA.log || fail "ANNA did not clear the quest alone"
    grep -aq "online: back to the town after the quest" $OUT/ANNA.log || fail "ANNA did not go back to the town"
    # (alone, ANNA's game takes the monster over without renumbering it, Em_Master_Change's n < 2: --runs)
    python3 tools/hunt_sync.py --kind ${KIND:-6} --masters 0,1 --kill --stay ANNA --runs ANNA --carve ANNA \
        ANNA=$OUT/ANNA.log BOB=$OUT/BOB.log > $OUT/sync.txt; r=$?; cat $OUT/sync.txt
    [ $r = 0 ] || fail "ANNA did not finish the hunt alone"
    grep -a "mcs: player 1 dropped out" $OUT/ANNA.log | head -1
    echo "mcs drop-out OK: BOB's game killed while he owned the Kut-Ku; ANNA's session dropped him, took the monster over, killed it, carved, cleared, back to the town"
    exit 0
fi
for nm in ANNA BOB; do
    grep -aq "mcs: logged in to the game server: 2 players" $OUT/$nm.log || fail "$nm: no game-server login"
    grep -aq "mcs: every player is in" $OUT/$nm.log || fail "$nm: the players' select did not complete"
    grep -aq "mcs: start agreed" $OUT/$nm.log || fail "$nm: net_start_ck did not agree on the start"
    grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/$nm.log || fail "$nm did not see the quest cleared"
    grep -aq "online: back to the town after the quest" $OUT/$nm.log || fail "$nm did not go back to the town"
done
if [ -n "$KUTKU" ]; then    # the HP from its owner on both, the owner ANNA -> BOB -> ANNA, the kill, both carve
    python3 tools/hunt_sync.py --kind ${KIND:-6} --masters 0,1,0 --kill --carve ANNA,BOB ANNA=$OUT/ANNA.log BOB=$OUT/BOB.log \
        > $OUT/sync.txt; r=$?; cat $OUT/sync.txt; [ $r = 0 ] || fail "the hunt was not the same on both"
    echo "mcs Kut-Ku hunt OK: the monster handed ANNA -> BOB -> ANNA, its HP the same on both, killed, both carved, the clear"
    exit 0
fi
echo "mcs hunt OK: two PCs on the game's own in-hunt protocol through mh1-server's mcs relay, login, select, start, the clear, back to the town"
