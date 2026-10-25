#!/bin/sh
# A real PS2 Monster Hunter G and a PC G hunt together (docs/network.md 3.12 "round 8"): the owner's G disc, unpatched,
# in PCSX2 (hidden) through the memory-card launcher (as tools/test_ps2_g_online.sh), the PC G online build
# (build/mhg/pc/mhview_g_online, as ANNA of tools/test_online_g.sh HUNT=1), mh1-server with --lobby-mcs on this
# machine's LAN address (an account each; never any other server).
#
# The PC (ANNA) posts quest 4 (the Kut-Ku, 怪鳥イャンクック襲来！, card 4 of level 1) for 2 hunters at the guild hall's
# middle counter; the PS2 joins the room at the left counter, gets ready at the departure spot, ANNA starts the match.
# Both log in to the game server (tools/server/mcs.py) and run G's own in-hunt protocol (the PS2: G's code; the PC:
# rt_mcs.c with MH1's mcsls / AQ C and G's packets). The PC follows and fights the Kut-Ku alone (quest ticks 600-1500),
# goes back to the camp (the monster passes to the PS2, which fights alone 1600-2600), the PS2 goes to the camp in turn
# (the monster comes back to the PC), then both fight to the kill and carve; the clear, the result screens, the relogin
# (each saves its hunter: the PC to its G card, the PS2 to its memory card) and both back in G's guild hall.
# The PS2 is steered through PCSX2's PINE socket only (tools/pcsx2/ps2_hunt_aid.py --game mhg: RAM writes as the PC's
# RT_PL_* test aids, a trace of the monster / hunters as the PS2 has them); the game's code is unchanged.
#
# Checks (agent C's round 7 list): the character select completes on both (the PC's "mcs: every player is in"; the PS2
# leaves its select wait and reaches the quest's stages), the PS2 loads the room's quest from slot 0 (game_w+0x2C = 4),
# the PS2 has the PC's hunter with the PC's own equipment (np-look lines of both), the start round agrees (the PC's
# "mcs: start agreed"; the PS2 gets past the quest-start text: it plays), the monster's HP and owner hand-overs the same
# on both (tools/hunt_sync.py), the clear on both (game_w x0D5 = 3), both back in the guild hall (stage 0x4D), the PC's
# card save after the relogin and the PS2's memory card written after it.
#
# usage: tools/test_ps2_g_hunt.sh     (~30 min; uncapped: PCSX2 cannot start under ulimit -v; takes the shared emulator
#                                      lock itself; PCSX2 alone)
#   MUL=n     the PC's damage factor (RT_DMG_MUL; the PS2 hits at 1x), default 6
#   STEPSHOTS=1  a PS2 picture after each scripted key step
# Needs: PCSX2 + the owner's BIOS (tools/run_pcsx2.sh), disc/mhg/Monster Hunter G (Japan).iso and disc/mhg, the launcher
# (tools/ps2launch/build.sh), build/ps2online/card_g_net.ps2 (tools/test_ps2_g_online.sh MAKECARD=1),
# build/mhg/pc/mhview_g_online (ONLINE=1 tools/build_pc_mhg.sh). Pictures and logs in build/show/ps2_g_hunt (never
# committed).
cd "$(dirname "$0")/.."
O=${OUTDIR:-build/show/ps2_g_hunt}
LOCK=${EMU_LOCK:-$(cd "$(git rev-parse --git-common-dir)/../.." && pwd)/xemu.lock}
if [ -z "$G_HUNT_LOCKED" ]; then
    exec env G_HUNT_LOCKED=1 flock "$LOCK" "$0" "$@"
fi
rm -rf $O; mkdir -p $O
. tools/emu_lib.sh
RP=; SP=; PCP=; AP=; KP=
cleanup() { rm -f $O/ps2_atk; emu_stop $KP $AP $PCP $RP $SP; RP=; SP=; PCP=; AP=; KP=; }
emu_traps cleanup
fail() { echo "ps2 G hunt test FAILED: $1 (see $O)"; shot fail; cleanup; exit 1; }
BIN=build/mhg/pc/mhview_g_online
CARD=build/ps2online/card_g_net.ps2
ISO="disc/mhg/Monster Hunter G (Japan).iso"
[ "$(ulimit -v)" = unlimited ] || { echo "a memory cap (ulimit -v) is set: PCSX2 cannot start under it"; exit 1; }
[ -f build/ps2launch/MH1LAUNCH.ELF ] || { echo "no launcher (tools/ps2launch/build.sh)"; exit 1; }
[ -f $CARD ] || { echo "no $CARD (tools/test_ps2_g_online.sh MAKECARD=1)"; exit 1; }
[ -x $BIN ] || { echo "no $BIN (ONLINE=1 tools/build_pc_mhg.sh)"; exit 1; }
pcsx2_running && { echo "another PCSX2 is running"; exit 1; }
IP=$(ip -4 route get 192.168.0.1 2>/dev/null | sed -n 's/.* src \([0-9.]*\).*/\1/p')
[ -n "$IP" ] || IP=$(hostname -I | awk '{print $1}')
case "$IP" in 10.*|192.168.*|172.1[6-9].*|172.2?.*|172.3[01].*) ;; *) echo "no private LAN address ($IP)"; exit 1;; esac
DNUM=${DNUM:-96}; PSLOT=${PSLOT:-28054}
export DNUM
shot() { ffmpeg -nostdin -v error -y -f x11grab -video_size 640x480 -i :$DNUM -frames:v 1 "$O/ps2_$1.png" </dev/null; }
NK=0
k() {   # k WAIT KEYS...: press the keys (each 150 ms, 450 ms apart), then wait WAIT s
    w=$1; shift; ks=""
    for a in "$@"; do ks="$ks $a:150:450"; done
    [ -z "$ks" ] || DISPLAY=:$DNUM build/pcsx2/xkey $ks
    sleep "$w"; NK=$((NK + 1))
    [ -z "$STEPSHOTS" ] || shot step_$(printf %03d $NK)
}
P="python3 tools/pcsx2/pine.py --slot $PSLOT"
PL=$((0x34D450))                     # G player_work (slot 0 before the hunt: the PS2's own hunter in the lobby)
warp() { $P wf $((PL + 0xAC)) "$1" 25 "$2" ";" w16 $((PL + 0x570)) "$3" > /dev/null; sleep 2; }
stage() { $P r8 0x35ED34 | awk '{print $2}'; }       # G game_w.stage (before the aid runs)
astage() { grep -a "ps2-stage:" $O/ps2_trace.txt | tail -1 | awk '{print $5}'; }   # the same from the aid's trace

# ---- the PC's G card (made once offline at the village bed, tools/test_online_g.sh), the server, the accounts
tools/test_online_g.sh mkgsave ANNA > $O/mkgsave.txt 2>&1 || { cat $O/mkgsave.txt; exit 1; }
rm -rf $O/card_ANNA; mkdir -p $O/card_ANNA; cp -r build/show/g_online_pc_cards/ANNA/. $O/card_ANNA/
cat > $O/mh1-server.conf <<CONF
[server]
bind = $IP
lobby-port = 10200
lobby-mcs = yes
mcs-port = 10298
db = accounts.sqlite3
verbose = yes
CONF
for a in ps2 pc; do
    python3 tools/server/mh1_server.py account add --db $O/accounts.sqlite3 --note "g $a" > $O/account_$a.txt || fail "account"
done
id_of() { sed -n 's/^login \([0-9]*\) password .*/\1/p' $O/account_$1.txt; }
pw_of() { sed -n 's/^login [0-9]* password \(.*\)/\1/p' $O/account_$1.txt; }
python3 tools/server/mh1_server.py serve --config $O/mh1-server.conf > $O/server.log 2>&1 &
SP=$!
sleep 2; grep -q "lobby on $IP:10200" $O/server.log || fail "server did not start"
printf 'server = %s\nlogin = %s\npassword = %s\n' $IP:10200 $(id_of ps2) "$(pw_of ps2)" > build/ps2launch/MH1LAUNCH.CNF

# ---- the PS2: G to the town and into the guild hall (tools/test_ps2_g_online.sh's steps)
export PCSX2_HOME="$PWD/build/C/g_hunt_pcsx2"; mkdir -p "$PCSX2_HOME/PCSX2"
[ -L "$PCSX2_HOME/PCSX2/bios" ] || { rm -rf "$PCSX2_HOME/PCSX2/bios"; ln -s "$HOME/ps2dev/pcsx2/data/PCSX2/bios" "$PCSX2_HOME/PCSX2/bios"; }
MCD="$PCSX2_HOME/PCSX2/memcards/Mcd001.ps2"
PINE=$PSLOT tools/run_pcsx2.sh --net --iso "$ISO" --elf build/ps2launch/MH1LAUNCH.ELF --card $CARD --seconds 3000 \
    --log $O/ps2_emulog.txt > $O/ps2_run.txt 2>&1 &
RP=$!
sleep 45
grep -aq "Monster Hunter G (Japan): online patch in RAM" $O/ps2_emulog.txt || fail "the launcher did not patch G"
k 6 Return; k 6 Return; k 4 c; k 4 c; k 4 c; k 9 c; k 9 Up c
k 6 c; k 6 c; k 6 c; k 8 c; k 8 c
k 30 Down Down c
grep -q "login $(id_of ps2) from .*: ok" $O/server.log || fail "the PS2 did not log in"
k 8 Right c; k 6 c; k 15 c; k 12 c
k 6 c; k 6 c; k 30 Left c
shot 01_town
grep -q "binary type .* in lobby 1" $O/server.log || fail "the PS2 did not reach the town"
warp 5545 2750 0x8000; k 8 z                         # the guild hall's door (square)
[ "$(stage)" = 0x4D ] || fail "the PS2 is not in the guild hall"
V=$($P r16 0x32C234 | awk '{print $2}'); $P w16 0x32C234 $((V | 0x10)) > /dev/null   # guild registration (User_data+0x24 bit 4)
shot 02_hall
echo "the PS2 is in the guild hall"

# ---- the PC (ANNA, tools/test_online_g.sh HUNT=1's input): to the town, the hall, the middle counter: quest 4 for 2,
# then square + yes at the departure spot (tried 5 times, ~13 s apart, from ~100 s after the room is up)
rep() { i=0; while [ $i -lt $1 ]; do printf "%s" "$2"; i=$((i + 1)); done; }
BASE="idle*200$(rep 14 ',circle*2,idle*150')$(rep 1 ',circle*2,idle*120,dleft*2,idle*30,circle*2,idle*136')"
IA="$BASE,square*2,idle*270$(rep 4 ',circle*2,idle*148')$(rep 3 ',dright*2,idle*48')$(rep 5 ',circle*2,idle*148')"
for key in circle circle dleft dleft ddown ddown ddown circle circle circle; do IA="$IA,$key*2,idle*100"; done
# (each try ends with attacks: the quest may start during the later tries, and its first fight must have them)
IA="$IA,idle*${AWAIT:-3000}$(rep 5 ',square*2,idle*120,circle*2,idle*120,circle*2,idle*40,cam_u*2,idle*28,cam_u*2,idle*28,cam_u*2,idle*28,cam_u*2,idle*28')"
IA="$IA$(rep 36 ',circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,cross*2,idle*18,ddown*2,idle*18,circle*2,idle*13,cam_u*2,idle*13')"
IA="$IA$(rep 60 ',circle*2,idle*91')"
F="RT_PL_GOD=1 RT_PL_TARGET=k6 RT_PL_GOTO=60,f RT_DMG_MUL=${MUL:-6} RT_GOTO_TRACE=1 RT_PL_CARVE=10 RT_QUEST_TRACE=1 RT_NOMOVIE=1 RT_NP_TRACE=1 RT_NP_POS=150 RT_NP_EM=30 RT_NP_RXEM=1 RT_MONEY=5000"
(env MH1_HIDDEN=1 MH1_SAVE_DIR="$PWD/$O/card_ANNA" MH1_LOG_DIR="$PWD/$O/logs_ANNA" RT_NAME=ANNA RT_VILLAGE_START=1 \
    RT_VILLAGE_SKIP_INTRO=1 RT_ONLINE_TRACE=1 RT_NET_REGISTERED=1 RT_NET_HOST=$IP RT_NET_PORT=10200 \
    RT_NET_ID=$(id_of pc) RT_NET_PASS=$(pw_of pc) RT_LB_WARP="60,5545,2750,0;300,1120,1950;2850,2920,1840,8000" \
    RT_SHOTS=${SHOTS:-5300,9000,12000,15000,18000,21000,24000,27000} $F RT_PL_GOTO2="1500,21;2500,-1" \
    RT_PL_WARP_EM="600-1500,2600-9000" \
    timeout 1300 $BIN disc/mhg --quest 10 --play --online --mute --input "$IA" --shot $O/pc.png --time 1000 \
    --size 640x480 > $O/pc.log 2>&1) &
PCP=$!

# ---- the PS2 joins ANNA's room at the left counter (square, then circle through the room list, room 1, yes)
i=0
while [ $i -lt 400 ] && ! grep -q "creates room 1" $O/server.log; do sleep 2; i=$((i + 2)); done
grep -q "creates room 1" $O/server.log || fail "ANNA did not make a room"
echo "ANNA's room is up"
sleep 5
warp 1850 1500 0x8000; k 3 z
i=0
while [ $i -lt 30 ] && ! grep -q "joins room 1" $O/server.log; do k 2 c; i=$((i + 1)); done
shot 03_room
grep -q "joins room 1" $O/server.log || fail "the PS2 did not join ANNA's room"
k 3 c                                                # "クエストに参加しました ... □ at the departure spot": はい
n0=$(grep -c "cmd 6504" $O/server.log)
warp 2920 1840 0x8000                                # ready at the departure spot: square, yes
for i in 1 2 3 4 5 6; do
    k 3 z; k 3 c
    [ "$(grep -c 'cmd 6504' $O/server.log)" -gt "$n0" ] && break
done
shot 04_ready
[ "$(grep -c 'cmd 6504' $O/server.log)" -gt "$n0" ] || fail "the PS2 did not get ready (no 6504)"
i=0
while [ $i -lt 400 ] && ! grep -q "mcs: login $(id_of ps2): player" $O/server.log; do sleep 2; i=$((i + 2)); done
grep -q "mcs: login $(id_of ps2): player" $O/server.log || fail "the PS2 did not log in to the game server"
PS2SLOT=$(sed -n "s/.*mcs: login $(id_of ps2): player \([0-9]\) .*/\1/p" $O/server.log | head -1)
echo "the PS2 is on the game server (player $PS2SLOT)"
cp "$MCD" $O/ps2_card_before.ps2
# the PS2's test aid from here on (PINE serves one client at a time: the warps above used pine.py on their own; from
# here on only the aid talks to PCSX2, and stage() reads its trace)
echo idle > $O/ps2_ctl
python3 tools/pcsx2/ps2_hunt_aid.py --game mhg --slot $PSLOT --ctl $O/ps2_ctl --kind 6 --exits $O/pc.log > $O/ps2_trace.txt 2>&1 &
AP=$!

# ---- the hunt: the PS2's part, timed from the PC's quest start (30 ticks a second)
i=0
while [ $i -lt 400 ] && ! grep -aq "village: quest 4 starts" $O/pc.log; do sleep 2; i=$((i + 2)); done
grep -aq "village: quest 4 starts" $O/pc.log || fail "the PC did not start the Kut-Ku quest (quest 4)"
T0=$(date +%s); echo "quest start at $T0 (PC's quest tick 0)" > $O/ps2_t0.txt
atq() { while [ $(( ($(date +%s) - T0) * 30 )) -lt $1 ]; do sleep 1; done; }
( while :; do       # keys while $O/ps2_atk exists (t = right stick up: attack); circle only once the monster is dead
    if [ -f $O/ps2_atk ]; then
        hp=$(grep -a 'np-em:' $O/ps2_trace.txt | tail -1 | awk '{print $13}')
        keys=$(cat $O/ps2_atk 2>/dev/null); [ -n "$hp" ] && [ "$hp" -le 0 ] && keys=c
        for key in $keys; do DISPLAY=:$DNUM build/pcsx2/xkey $key:150:150; done
    else sleep 0.5; fi
  done ) &
KP=$!
echo follow > $O/ps2_ctl
atq 300; shot kk_0300
atq 1200; shot kk_1200_pc_fights
atq 1600; echo fight > $O/ps2_ctl; echo t > $O/ps2_atk
for q in 1700 1750 1800 1850 1900 1950 2000 2050; do atq $q; shot kk_${q}_ps2_fights; done
atq 2100; shot kk_2100_ps2_fights
atq 2600; echo "goto 21" > $O/ps2_ctl; rm -f $O/ps2_atk
atq 3000; shot kk_3000_ps2_camp
atq 3400; echo fight > $O/ps2_ctl; echo t > $O/ps2_atk
for q in 4000 4800 5600 6400 7200 8000 8800; do atq $q; shot kk_$q; done
# after the hunt: circles now and then (the carve, the result screens, the relogin's save) until the PS2 stands in the
# guild hall again after its second login
rm -f $O/ps2_atk; echo idle > $O/ps2_ctl
i=0
while [ $i -lt 120 ]; do
    [ "$(grep -c "login $(id_of ps2) from .*: ok" $O/server.log)" -ge 2 ] && [ "$(astage)" = 0x4D ] && break
    k 3 c; i=$((i + 1))
    [ $((i % 10)) = 0 ] && shot after_$i
done
shot 05_back
STG_END=$(astage)
k 20; shot 06_back_later
cp "$MCD" $O/ps2_card_after.ps2
emu_stop $KP; KP=
wait $PCP; PCP=
cleanup

# ---- checks
grep -aq "mcs: every player is in" $O/pc.log || fail "the PC: the players' select did not complete (sys 2)"
grep -aq "mcs: start agreed" $O/pc.log || fail "the PC: the start was not agreed (sys 3)"
grep -aq "mcs: logged in to the game server: 2 players" $O/pc.log || fail "the PC: not 2 players in the session"
grep -aq "rt_flow: tick [0-9]* mode .* quest 4$" $O/ps2_trace.txt || fail "the PS2 did not load the room's quest (4)"
grep -aq "np-pos: .* me $PS2SLOT slot $PS2SLOT stg [0-9] \|np-pos: .* me $PS2SLOT slot $PS2SLOT stg [1-6][0-9] " $O/ps2_trace.txt ||
    fail "the PS2 never stood on a quest stage (stuck in its select or the start)"
# the PC's hunter as the PS2 has it: the PC's own equipment (np-look of the PC's slot on both machines)
PCSLOT=$((1 - PS2SLOT))
lk() { grep -a "np-look: .* me $2 slot $3 stg [0-9]* " $1 | sed 's/.* weapon /weapon /' | sort | uniq -c | sort -rn | head -1 | sed 's/^ *[0-9]* //'; }
a=$(lk $O/pc.log $PCSLOT $PCSLOT); b=$(lk $O/ps2_trace.txt $PS2SLOT $PCSLOT)
echo "the PC's hunter: on the PC: $a; on the PS2: $b"
[ -n "$a" ] && [ "$a" = "$b" ] || fail "the PS2 does not have the PC's hunter's equipment"
a=$(lk $O/ps2_trace.txt $PS2SLOT $PS2SLOT); b=$(lk $O/pc.log $PCSLOT $PS2SLOT)
echo "the PS2's hunter: on the PS2: $a; on the PC: $b"
[ -n "$a" ] && [ "$a" = "$b" ] || fail "the PC does not have the PS2's hunter's equipment"
python3 tools/hunt_sync.py --kind 6 --masters $PCSLOT,$PS2SLOT,$PCSLOT --kill --carve PC,PS2 PC=$O/pc.log PS2=$O/ps2_trace.txt \
    > $O/sync.txt; r=$?; cat $O/sync.txt; [ $r = 0 ] || fail "the machines did not see the same hunt"
grep -aq "rt_flow: tick [0-9]* mode 2 step 0 D5 3" $O/pc.log || fail "the PC did not see the quest cleared"
grep -aq "rt_flow: tick [0-9]* mode [0-9]* step [0-9]* D5 3 " $O/ps2_trace.txt || fail "the PS2 did not see the quest cleared"
grep -aq "online: back to the town after the quest" $O/pc.log || fail "the PC did not go back to the town"
sed -n '/back to the town after the quest/,$p' $O/pc.log | grep -aq "slot [0-9] (me) \"ANNA\" stage 7[67] pos [1-9]" ||
    fail "the PC is not back in G's town / guild hall after the quest"
grep -aqs "memory card: saved BISLPM-65869MHG/BISLPM-65869MHG" $O/logs_ANNA/*.log || fail "the PC's relogin did not save"
[ "$(grep -c "login $(id_of ps2) from .*: ok" $O/server.log)" -ge 2 ] || fail "the PS2 did not log in again after the quest"
[ "$STG_END" = 0x4D ] || fail "the PS2 is not back in the guild hall (stage $STG_END)"
cmp -s $O/ps2_card_before.ps2 $O/ps2_card_after.ps2 && fail "the PS2's memory card was not written after the quest"
echo "ps2 G hunt test: OK (the real G disc and the PC G: room, select, quest 4 from slot 0, each other's equipment, the start, the Kut-Ku's HP / owners the same on both, the kill, carves, the clear, the relogin saves, back in the guild hall; see $O)"
