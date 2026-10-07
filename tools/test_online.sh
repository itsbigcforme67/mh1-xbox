#!/bin/sh
# Online play test (docs/network.md): the ONLINE=1 build (build/pc/mhview_online) runs the game's
# own lobby-server client against tools/mh1_testserver.py on 127.0.0.1 (never any other host):
# connect (the game's tcp_init: server table, DNS, TCP), login (obfuscated key / password, echo
# test, account list, account id, login ok), mini data, login finish, top information, current
# place, the plaza list (names, status, users), plaza entry, the lobby list, lobby entry, the
# lobby member list, logout. Headless, ~3 s. Build first: ONLINE=1 tools/build_pc.sh
# Then two clients at once: A stays in lobby 1 for 3 s, B joins, says something and leaves; A is told
# that B came in, hears the chat and sees B leave; B sees A in the member list.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview_online}
OUT=build/show/online; rm -rf $OUT; mkdir -p $OUT
export MH1_SAVE_DIR="$PWD/$OUT/card"
fail() { echo "online test FAILED: $1 (see $OUT)"; [ -n "$SP" ] && kill $SP 2>/dev/null; exit 1; }
[ -x "$BIN" ] || fail "$BIN is missing: run ONLINE=1 tools/build_pc.sh"
python3 tools/mh1_testserver.py -v --port 0 > $OUT/server.log 2>&1 &
SP=$!
i=0; PORT=""
while [ $i -lt 50 ] && [ -z "$PORT" ]; do
    PORT=$(sed -n 's/^mh1_testserver listening on 127.0.0.1:\([0-9]*\)$/\1/p' $OUT/server.log)
    [ -n "$PORT" ] || { sleep 0.1; i=$((i + 1)); }
done
[ -n "$PORT" ] || fail "the test server did not start"
RT_NET_PORT=$PORT timeout 60 $BIN disc/mh1 --nettest full > $OUT/client.out 2>&1
RC=$?
[ $RC -eq 0 ] || { tail -5 $OUT/client.out; fail "the client stopped early (rc $RC)"; }
for s in "tcp connected" "login ok" "top information: level" "plazas: 2" 'plaza 1 "Test Plaza 1" status 3' \
         "lobbies: 4" 'member 0: id "0000' 'stopped in phase "done"'; do
    grep -q "$s" $OUT/client.out || fail "missing: $s"
done
sleep 0.3
grep -q "client gone" $OUT/server.log || fail "the server never saw the client leave"
RT_NET_PORT=$PORT RT_NET_HOLD_MS=3500 timeout 60 $BIN disc/mh1 --nettest full > $OUT/a.out 2>&1 &
AP=$!
sleep 2.0
RT_NET_PORT=$PORT RT_NET_CHAT="hello from B" timeout 60 $BIN disc/mh1 --nettest full > $OUT/b.out 2>&1 || fail "client B stopped early"
wait $AP || fail "client A stopped early"
grep -q "came into the lobby" $OUT/a.out || fail "A was not told that B came in"
grep -q 'chat from .*"hello from B"' $OUT/a.out || fail "A did not hear the chat"
grep -q "left the lobby" $OUT/a.out || fail "A was not told that B left"
[ "$(grep -c 'member [0-9]: id' $OUT/b.out)" = 2 ] || fail "B did not see two members"
# the name path: the server table names "localhost", resolved by the backend's DNS (CpInetDnsGetTicket / LookUp)
RT_NET_HOST=localhost RT_NET_PORT=$PORT timeout 60 $BIN disc/mh1 --nettest top > $OUT/dns.out 2>&1 || fail "the connection by name failed"
grep -q "tcp connected" $OUT/dns.out || fail "no connection by name"
kill $SP 2>/dev/null
# safety: the MH Oldschool addresses and any public address are refused before a socket is opened
for h in 34.75.107.68 151.80.238.99 8.8.8.8; do
    RT_NET_HOST=$h RT_NET_PORT=$PORT timeout 30 $BIN disc/mh1 --nettest connect > $OUT/refuse.out 2>&1 && fail "$h was not refused"
    grep -q "net: refusing $h" $OUT/refuse.out || fail "$h: no refusal message"
done
echo "online OK: login, top information, plaza and lobby lists, lobby members, two clients with notices and chat, connection by name, public / MH Oldschool addresses refused (server 127.0.0.1:$PORT)"
