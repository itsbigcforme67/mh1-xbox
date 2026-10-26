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
# HUNT=1 (two clients, ~20 min): ROOM=1 on mh1-server with --lobby-mcs (an account each, the game's own in-hunt protocol
# through the mcs relay, as MH1's tools/test_online_mcs.sh KUTKU=1): ANNA posts quest 4 (the Kut-Ku, card 4 of level 1,
# fee 200z: RT_MONEY), both log in to the relay, ANNA fights alone, then BOB, then both (owner ANNA -> BOB -> ANNA,
# tools/hunt_sync.py), the kill, both carve, the clear, the result screens, the relogin (each client starts from its own G
# card, made once at the village bed: the save writes) and both back in G's guild hall; then each card loads back
# offline (CONTINUE) with the hunt's money. GDB=FILE runs the clients under gdb -x FILE.
# Screenshots in build/show/g_online_pc (never committed). Build first: ONLINE=1 tools/build_pc_mhg.sh
cd "$(dirname "$0")/.."
BIN=${BIN:-build/mhg/pc/mhview_g_online}
OUT=build/show/g_online_pc; mkdir -p $OUT; [ "$1" = mkgsave ] || rm -f $OUT/*.log
SP=""; PA=""
fail() { echo "G online PC test FAILED: $1 (see $OUT)"; [ -n "$PA" ] && kill $PA 2>/dev/null; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc_mhg.sh"
[ -d disc/mhg ] || fail "no disc/mhg (the owner's G disc files)"
QN=1; [ -n "$HUNT" ] && QN=4
# a G card with one hunter named NAME, saved at the village bed (offline, the same binary; tools/test_save_g.sh's steps):
# build/show/g_online_pc_cards/NAME, remade when the binary is newer. The online runs start from a copy, so G's relogin
# save after a quest (lbc_login_finish_after, McOperationSet(7)) writes the card instead of "continue without saving?".
CARDS=build/show/g_online_pc_cards
mkgsave() {
    f=$CARDS/$1/BISLPM-65869MHG/BISLPM-65869MHG
    [ -f "$f" ] && [ "$f" -nt "$BIN" ] && return 0
    rm -rf $CARDS/$1; mkdir -p $CARDS/$1
    S=$(python3 - <<'PY'
def evs(d, end):
    out, t = [], 0
    for k in sorted(d):
        if k > t: out.append("idle*%d" % (k - t)); t = k
        out.append(d[k]); t += int(d[k].split("*")[1])
    out.append("idle*%d" % (end - t)); return out
# house door, then (warped to the bed) square, the save prompts (yes), "create game data" yes
print(",".join(["@village1"] + evs({45: 'square*2', 650: 'square*2', 750: 'circle*2', 850: 'circle*2', 950: 'circle*2',
                                     1050: 'circle*2', 1100: 'dleft*2', 1130: 'circle*2', 1250: 'circle*2'}, 1500)))
PY
)
    ( ulimit -v 3000000 2>/dev/null
      env MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$CARDS/$1" MH1_LOG_DIR="$PWD/$CARDS/$1.logs" RT_NOMOVIE=1 RT_VILLAGE_START=1 \
          RT_VILLAGE_SKIP_INTRO=1 RT_NAME=$1 RT_LB_WARP="30,11225,14400,0;600,2259,745,4001" \
          timeout 300 $BIN disc/mhg --quest 1 --play --mute --input "$S" --shot $CARDS/$1.png --time 60 > $CARDS/$1.log 2>&1 )
    grep -aqs "memory card: saved BISLPM-65869MHG/BISLPM-65869MHG" $CARDS/$1.logs/*.log && [ -f "$f" ] ||
        fail "could not make $1's G card (see $CARDS/$1.log)"
    touch "$f"
}
# tools/test_online_g.sh mkgsave NAME: only that card (tools/test_ps2_g_hunt.sh uses it)
if [ "$1" = mkgsave ]; then mkgsave "$2"; echo "$CARDS/$2"; exit 0; fi
if [ -n "$HUNT" ]; then     # HUNT=1: mh1-server with the game server of the game's own in-hunt protocol (mcs relay), as
    # tools/test_online_mcs.sh; an account per hunter (the relay knows the match's players by their logins)
    ROOM=1
    rm -f $OUT/accounts.sqlite3
    cat > $OUT/mh1-server.conf <<CONF
[server]
bind = 127.0.0.1
lobby-port = 0
lobby-mcs = yes
mcs-port = ${MCSPORT:-10296}
db = accounts.sqlite3
verbose = yes
CONF
    for nm in ANNA BOB; do
        python3 tools/server/mh1_server.py account add --db $OUT/accounts.sqlite3 --note "test $nm" > $OUT/account_$nm.txt || fail "account add"
    done
    python3 tools/server/mh1_server.py serve --config $OUT/mh1-server.conf > $OUT/server.log 2>&1 &
    PAT='s/^mh1-server lobby on 127.0.0.1:\([0-9]*\).*/\1/p'
else
    python3 tools/mh1_testserver.py -v --port 0 > $OUT/server.log 2>&1 &
    PAT='s/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p'
fi
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n "$PAT" $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the server did not start"
acct() {    # NAME: RT_NET_ID / RT_NET_PASS of its account (HUNT=1)
    [ -n "$HUNT" ] && sed -n 's/^login \([0-9]*\) password \(.*\)/RT_NET_ID=\1 RT_NET_PASS=\2/p' $OUT/account_$1.txt
}
# input: circle through login, the Capcom ID ("new ID", yes), the class menu, the plaza menu (街に入る) and the town
# list (town 1); then "yes" (left, circle) on the town's confirmation
IN="idle*200$(for i in $(seq 14); do printf ',circle*2,idle*150'; done)$(for i in $(seq 12); do printf ',circle*2,idle*120,dleft*2,idle*30,circle*2,idle*150'; done),idle*${HOLD:-900}${TAIL:+,$TAIL}"
run() {     # name seconds extra-env...
    nm=$1; t=$2; shift 2
    rm -rf $OUT/card_$nm $OUT/logs_$nm
    if [ -n "$HUNT" ]; then mkgsave $nm; mkdir -p $OUT/card_$nm; cp -r $CARDS/$nm/. $OUT/card_$nm/; fi
    env MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$OUT/card_$nm" MH1_LOG_DIR="$PWD/$OUT/logs_$nm" RT_NAME=$nm RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 \
        RT_NET_PORT=$PORT RT_SHOTS=${SHOTS:-1500,2300,3200,4200} "$@" timeout ${TMO:-400} ${GDB:+gdb -q -batch -x $GDB --args} $BIN disc/mhg --quest 10 --play --online --mute \
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
    # HUNT=1: the card of quest 4 (the Kut-Ku: CARD right presses on level 1's list), then the hunt as test_online_mcs.sh
    # KUTKU=1 (from the quest's start, for both: attacks every 30 ticks, circle = carve, the reward screens' keys)
    # (the 5th circle of the counter talk takes the card shown: the quest list's right presses go before it)
    CARDK=""; [ -n "$HUNT" ] && CARDK=$(rep ${CARD:-3} ',dright*2,idle*48')
    IA="$BASE,square*2,idle*270$(rep 4 ',circle*2,idle*148')$CARDK$(rep 5 ',circle*2,idle*148')"
    for k in circle circle dleft dleft ddown ddown ddown circle circle circle; do IA="$IA,$k*2,idle*100"; done
    END=",idle*1500"; SA=380; SB=378; A_AID=""; B_AID=""
    if [ -n "$HUNT" ]; then
        END=$(rep ${HUNTN:-36} ',circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,cross*2,idle*18,ddown*2,idle*18,circle*2,idle*13,cam_u*2,idle*13')
        SA=900; SB=898; TMOH=1100
        # back to the town: the login saves the hunter to the card (lbc_login_finish_after, McOperationSet(7)); each client
        # starts from its own G card (mkgsave), so the save writes; circles take the card screen's messages (before round 7
        # the cards held no G save and the test answered "continue without saving?" with left + circle)
        END="$END$(rep ${TAILN:-60} ',circle*2,idle*91')"
        # quest ticks (as KUTKU=1): both follow the Kut-Ku; ANNA fights alone (600-1500) and goes back to the camp so the
        # monster passes to BOB, who fights alone (1600-2600) and goes back in turn; then both fight to the kill and carve
        F="RT_PL_GOD=1 RT_PL_TARGET=k${KIND:-6} RT_PL_GOTO=60,f RT_DMG_MUL=${MUL:-2} RT_GOTO_TRACE=1 RT_PL_CARVE=10 RT_QUEST_TRACE=1 RT_NOMOVIE=1 RT_NP_POS=150 RT_NP_EM=30 RT_MONEY=5000"
        A_AID="$F RT_PL_GOTO2=1500,21;2500,-1 RT_PL_WARP_EM=600-1500,2600-9000"
        B_AID="$F RT_PL_GOTO2=2600,21;3400,-1 RT_PL_WARP_EM=1600-2600,3500-9000"
    fi
    IA="$IA,idle*2200$(rep 4 ',square*2,idle*120,circle*2,idle*120,circle*2,idle*160')$END"
    IB="$BASE,square*2,idle*2678,square*2,idle*60$(rep 20 ',circle*2,idle*58'),idle*300$(rep 3 ',square*2,idle*100,circle*2,idle*100')$END"
    TMO=${TMOH:-640}; IN="$IA" run ANNA $SA RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,0;300,1120,1950;2850,2920,1840,8000" \
        RT_SHOTS=${SHOTS:-5300,7700,9400} $(acct ANNA) $A_AID &
    PA=$!
    sleep 1
    IN="$IB" run BOB $SB RT_NET_REGISTERED=1 RT_LB_WARP="60,5545,2750,0;2700,1850,1500,8000;3400,2920,1840,8000" \
        RT_SHOTS=${SHOTS:-5600,7400,9200} $(acct BOB) $B_AID || fail "client BOB stopped early (see $OUT/BOB.log)"
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
    run ANNA ${T1:-160} || fail "the client stopped early (see $OUT/ANNA.log)"
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
    grep -q "cmd 6415 seq [0-9A-F]* res 0 len 6 0001$(printf %02X $QN)00[0-9A-F]*" $OUT/server.log || fail "no 6415 (quest attributes) for room 1"
    grep -q "BOB joins room 1 of lobby 1 (2 members)" $OUT/server.log || fail "BOB did not join ANNA's room"
    grep -q "match start in room 1: ANNA, BOB" $OUT/server.log || fail "no match for two"
    for nm in ANNA BOB; do
        grep -aq "guild step\|town state x68 [0-9]* talk" $OUT/$nm.log || fail "$nm: no counter trace"
        if [ -n "$HUNT" ]; then
            grep -aq "online: match: quest 4, 2 players" $OUT/$nm.log || fail "$nm did not get the match (quest 4, 2 players)"
            grep -aq "mcs: logged in to the game server: 2 players" $OUT/$nm.log || fail "$nm: no game-server login"
            grep -aq "mcs: every player is in" $OUT/$nm.log || fail "$nm: the players' select did not complete"
            grep -aq "mcs: start agreed" $OUT/$nm.log || fail "$nm: net_start_ck did not agree on the start"
            grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $OUT/$nm.log || fail "$nm did not see the quest cleared"
            grep -aq "online: back to the town after the quest" $OUT/$nm.log || fail "$nm did not go back to the town"
            # (G puts the hunter back in the guild hall, stage 0x4D, where the quest was taken)
            sed -n '/back to the town after the quest/,$p' $OUT/$nm.log | grep -aq "slot [0-9] (me) \"$nm\" stage 7[67] pos [1-9]" ||
                fail "$nm is not back in G's town / guild hall after the quest"
        else
            grep -aq "online: match: quest 1, 2 players" $OUT/$nm.log || fail "$nm did not get the match (quest 1, 2 players)"
            grep -aq "co-op: quest 1, 2 player(s)" $OUT/$nm.log || fail "$nm did not start quest 1 with the other"
        fi
    done
fi
if [ -n "$HUNT" ]; then
    grep -q "mcs: match room 1" $OUT/server.log || fail "the lobby did not hand the match to the mcs relay"
    grep -q "mcs: login .*: player 0 of match" $OUT/server.log || fail "ANNA did not log in to the mcs relay"
    grep -q "mcs: login .*: player 1 of match" $OUT/server.log || fail "BOB did not log in to the mcs relay"
    # the relogin's save went to the card (the card started with the fresh hunter's 0z), and the card loads back (offline,
    # CONTINUE as tools/test_save_g.sh) with the money of the hunt (RT_MONEY=5000 - the fee + the reward)
    C="idle*80,circle*2,idle*600,start*2,idle*80,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2,idle*250,ddown*2,idle*20,circle*2,idle*100,circle*2,idle*100,circle*2,idle*400"
    for nm in ANNA BOB; do
        grep -aqs "memory card: saved BISLPM-65869MHG/BISLPM-65869MHG" $OUT/logs_$nm/*.log ||
            fail "$nm: the relogin did not save the hunter to the card"
        ( ulimit -v 3000000 2>/dev/null
          env MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$OUT/card_$nm" MH1_LOG_DIR="$PWD/$OUT/logs_${nm}_reload" RT_NOMOVIE=1 RT_QUEST_TRACE=1 \
              timeout 300 $BIN disc/mhg --boot --mute --input "$C" --shot $OUT/${nm}_reload.png --time 75 > $OUT/${nm}_reload.log 2>&1 )
        r=$(grep -a "rt_village: tick [0-9]* ud" $OUT/${nm}_reload.log | head -1)
        [ -n "$r" ] || fail "$nm: the saved card did not load back into the village (see $OUT/${nm}_reload.log)"
        echo "$r" | grep -q "ud money [1-9]" || fail "$nm: the reloaded hunter has not the hunt's money: $r"
        echo "$nm reloaded: $(echo "$r" | sed 's/ pouch.*//')"
    done
    # the HP from its owner on both, the owner ANNA -> BOB -> ANNA, the kill, both carve (as test_online_mcs.sh KUTKU=1)
    python3 tools/hunt_sync.py --kind ${KIND:-6} --masters 0,1,0 --kill --carve ANNA,BOB ANNA=$OUT/ANNA.log BOB=$OUT/BOB.log \
        > $OUT/sync.txt; r=$?; cat $OUT/sync.txt; [ $r = 0 ] || fail "the hunt was not the same on both"
fi
grep -q "game: Monster Hunter G" $OUT/server.log || fail "the server did not take the client for G (61A0)"
grep -q "cmd 6882" $OUT/server.log || fail "no info file download (6882)"
grep -q "cmd 6207" $OUT/server.log || fail "no plaza entry (6207)"
grep -q "cmd 630D" $OUT/server.log || fail "no town list (630D)"
grep -q "cmd 6305" $OUT/server.log || fail "no town entry (6305)"
echo "G online PC test: OK ($NAMES: G's login, Capcom ID, info file, plaza, town list, in the town${TWO:+, each sees the other}${ROOM:+, the guild hall counter: a room for 2, BOB joins, the match, both start quest $QN}${HUNT:+ on mh1-server's mcs relay: the Kut-Ku handed ANNA -> BOB -> ANNA, HP the same on both, killed, both carved, the clear, both back in G's guild hall, the relogin saved to each G card and it loads back}; server 127.0.0.1:$PORT)"
