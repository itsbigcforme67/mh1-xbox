#!/bin/sh
# Monster Hunter G online on a real disc (docs/network.md 3.12): the owner's G disc, unpatched, booted in PCSX2 (hidden)
# through the memory-card launcher (build/ps2launch/MH1LAUNCH.ELF, which patches G in RAM), logs in to OUR server
# (mh1-server on this machine's LAN address, an account made for the test), picks its Capcom ID, gets G's info file,
# opens the plaza menu, the town list, and walks into town 1. Checks the server log at each step; screenshots in
# build/show/g_online (never committed).
#
# usage: tools/test_ps2_g_online.sh        (~6 min; uncapped: PCSX2 cannot start under ulimit -v; takes the shared
#                                           emulator lock itself)
#   MAKECARD=1  make the G card first (~3 min more): MH1's NET-file card (build/ps2online/card_net.ps2, from
#               tools/test_ps2_online.sh: Sony's network setting + a NET file, which G reads too) plus a new G hunter
#               "aaaa" made and saved by G's own NEW GAME screens -> build/ps2online/card_g_net.ps2
#   HUNT=1      then a quest through our server: into the guild hall, the hunter registered with the guild (event flag
#               4, a RAM write through PINE as the PC tests' RT_NET_REGISTERED), the quest counter: quest level ★, the
#               first quest card (森のキノコ狩り), a room for 1, the departure spot, the match, the game server (mcs.py)
#               login and the quest's camp. (Warps through PINE: G player_work 0x34D450.)
#   KEEP=1      leave PCSX2 (PINE slot 28054, display :96) and the server running in the town (exploring by hand;
#               stop them with the PIDs it prints; the emulator lock is held until then)
# Needs: PCSX2 + the owner's BIOS (tools/run_pcsx2.sh), disc/mhg/Monster Hunter G (Japan).iso, the launcher
# (tools/ps2launch/build.sh).
cd "$(dirname "$0")/.."
O=build/show/g_online; mkdir -p $O
CARD=build/ps2online/card_g_net.ps2
LOCK=${EMU_LOCK:-$(cd "$(git rev-parse --git-common-dir)/../.." && pwd)/xemu.lock}
if [ -z "$G_ONLINE_LOCKED" ]; then
    exec env G_ONLINE_LOCKED=1 flock "$LOCK" "$0" "$@"
fi
. tools/emu_lib.sh
RP=; SP=
cleanup() { emu_stop $RP $SP; RP=; SP=; }
emu_traps cleanup
fail() { echo "ps2 G online test FAILED: $1 (see $O)"; shot fail; cleanup; exit 1; }
[ "$(ulimit -v)" = unlimited ] || { echo "a memory cap (ulimit -v) is set: PCSX2 cannot start under it"; exit 1; }
[ -f build/ps2launch/MH1LAUNCH.ELF ] || { echo "no launcher (tools/ps2launch/build.sh)"; exit 1; }
pcsx2_running && { echo "another PCSX2 is running"; exit 1; }
IP=$(ip -4 route get 192.168.0.1 2>/dev/null | sed -n 's/.* src \([0-9.]*\).*/\1/p')
[ -n "$IP" ] || IP=$(hostname -I | awk '{print $1}')
case "$IP" in 10.*|192.168.*|172.1[6-9].*|172.2?.*|172.3[01].*) ;; *) echo "no private LAN address ($IP)"; exit 1;; esac
export PCSX2_HOME="$PWD/build/C/g_online_pcsx2"; mkdir -p "$PCSX2_HOME/PCSX2"
[ -L "$PCSX2_HOME/PCSX2/bios" ] || { rm -rf "$PCSX2_HOME/PCSX2/bios"; ln -s "$HOME/ps2dev/pcsx2/data/PCSX2/bios" "$PCSX2_HOME/PCSX2/bios"; }
DNUM=${DNUM:-96}
export DNUM
shot() { ffmpeg -nostdin -v error -y -f x11grab -video_size 640x480 -i :$DNUM -frames:v 1 "$O/$1.png" </dev/null; }
# k WAIT KEYS...: press the keys (each 150 ms, 450 ms apart), wait WAIT s; STEPSHOTS=1: a picture after each
NK=0
k() {
    w=$1; shift; ks=""
    for a in "$@"; do ks="$ks $a:150:450"; done
    [ -z "$ks" ] || DISPLAY=:$DNUM build/pcsx2/xkey $ks
    sleep "$w"; NK=$((NK + 1))
    [ -z "$STEPSHOTS" ] || shot step_$(printf %03d $NK)
}
ISO="disc/mhg/Monster Hunter G (Japan).iso"

if [ -n "$MAKECARD" ] || [ ! -f $CARD ]; then
    [ -f build/ps2online/card_net.ps2 ] || fail "no build/ps2online/card_net.ps2 (tools/test_ps2_online.sh makes it)"
    tools/run_pcsx2.sh --iso "$ISO" --card build/ps2online/card_net.ps2 --seconds 600 --log $O/emulog_card.txt \
        > $O/run_card.txt 2>&1 &
    RP=$!
    sleep 45
    # the opening movie -> GAME MENU -> NEW GAME -> character creation: NAME (keyboard: "aaaa", start = done),
    # DECIDE, save it (yes), slot 1, its first file, yes, saved
    k 6 Return; k 6 Return; k 12 c; k 3 Down; k 3 c; k 2 Down c c c c; k 2 Return; k 2 Up c; k 5 Left c; k 5 c; k 5 c
    k 4 c; k 8 Left c; k 10 c
    cleanup
    cp "$PCSX2_HOME/PCSX2/memcards/Mcd001.ps2" $CARD
    echo "made $CARD"
fi

# the server: invite-only, one account for the PS2
cat > $O/mh1-server.conf <<CONF
[server]
bind = $IP
lobby-port = 10200
lobby-mcs = yes
mcs-port = 10298
db = accounts.sqlite3
verbose = yes
CONF
rm -f $O/accounts.sqlite3*
python3 tools/server/mh1_server.py account add --db $O/accounts.sqlite3 --note ps2g > $O/account.txt || fail "account"
LOGIN=$(sed -n 's/^login \([0-9]*\) password .*/\1/p' $O/account.txt)
PASS=$(sed -n 's/^login [0-9]* password \(.*\)/\1/p' $O/account.txt)
python3 tools/server/mh1_server.py serve --config $O/mh1-server.conf > $O/server.log 2>&1 &
SP=$!
sleep 2; grep -q "lobby on $IP:10200" $O/server.log || fail "server did not start"
printf 'server = %s\nlogin = %s\npassword = %s\n' $IP:10200 $LOGIN "$PASS" > build/ps2launch/MH1LAUNCH.CNF

PINE=28054 tools/run_pcsx2.sh --net --iso "$ISO" --elf build/ps2launch/MH1LAUNCH.ELF --card $CARD --seconds $([ -n "$KEEP" ] && echo 36000 || echo 900) \
    --log $O/emulog.txt > $O/run.txt 2>&1 &
RP=$!
sleep 45
grep -aq "Monster Hunter G (Japan): online patch in RAM" $O/emulog.txt || fail "the launcher did not patch G"
# title -> GAME MENU (cursor on CONTINUE) -> slot 1, its file, the hunter, yes -> MODE SELECT: 街へ行く
k 6 Return; k 6 Return; k 4 c; k 4 c; k 4 c; k 9 c; k 9 Up c
# NET file found: load it (yes), slot 1, yes, loaded (O), the autosave notice -> the connection screen: Setting 1
k 6 c; k 6 c; k 6 c; k 8 c; k 8 c
shot 01_connection
k 30 Down Down c
grep -q "login $LOGIN from .*: ok" $O/server.log || fail "G did not log in"
grep -q "game: Monster Hunter G" $O/server.log || fail "the server did not take the client for G (61A0)"
# user registration: do not register; Capcom ID: the first free slot, yes (new ID) -> the class menu -> the plaza
k 8 Right c; k 6 c; k 15 c; k 12 c
shot 02_plaza
grep -q "cmd 6881" $O/server.log || fail "G did not download its info file"
# 街に入る: the town list, then town 1, yes
k 6 c
shot 03_town_list
k 6 c; k 30 Left c
shot 04_town
grep -q "binary type .* in lobby 1" $O/server.log || fail "G did not reach the town"
if [ -n "$HUNT" ]; then
    P="python3 tools/pcsx2/pine.py --slot 28054"
    PL=$((0x34D450))
    warp() { $P wf $((PL + 0xAC)) "$1" 25 "$2" ";" w16 $((PL + 0x570)) "$3" > /dev/null; sleep 2; }
    warp 5545 2750 0x8000; k 8 z                    # the guild hall's door (square)
    [ "$($P r8 0x35ED34 | awk '{print $2}')" = 0x4D ] || fail "not in the guild hall"
    V=$($P r16 0x32C234 | awk '{print $2}'); $P w16 0x32C234 $((V | 0x10)) > /dev/null   # User_data+0x24 bit 4
    warp 1120 1950 0x8000                            # the middle counter lady
    k 3 c; k 2.5 c; k 2.5 c; k 2.5 c; k 2.5 c; k 2.5 c; k 2.5 c
    shot 05_counter
    k 2.5 c; k 3 c                                   # "recruit members?" yes -> the rule sheet
    k 3 c; k 3 c; k 1 Left; k 1 Left; k 1 Left; k 1 Down; k 1 Down; k 1 Down; k 4 c; k 3 c; k 3 c
    shot 06_room
    grep -q "creates room 1 in lobby 1" $O/server.log || fail "no room made"
    warp 2920 1840 0x8000                            # the departure spot: square = depart, yes
    k 3 z; k 25 c
    grep -q "match start in room 1" $O/server.log || fail "no match start"
    grep -q "mcs: login .* (1 of 1 in)" $O/server.log || fail "G did not log in to the game server"
    shot 07_quest
fi
if [ -n "$KEEP" ]; then
    echo "KEEP: PCSX2 run $RP, server $SP (display :$DNUM, PINE 28054); stop: kill $RP $SP"
    trap - EXIT INT TERM HUP
    wait $RP
    exit 0
fi
cleanup
echo "ps2 G online test: OK (the real G disc through MH1LAUNCH on our server: login, info file, plaza, town list, town${HUNT:+, guild counter, a room, the match, the game server, the quest}")"
