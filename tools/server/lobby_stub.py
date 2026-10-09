"""lobby_stub.py - the lobby service of mh1-server, for now a thin layer over tools/mh1_testserver.py.

The lobby protocol (login, plazas, lobbies, rooms, chat, matching) is being grown by agent B in tools/mh1_testserver.py
(docs/network.md 5). This stub does not copy it: it imports that module and subclasses its connection handler, adding
only what a public server needs on top (docs/server.md 3.2):

  * accounts: the login (6101: 8-digit key + obfuscated password) is checked against the SQLite store; unknown logins,
    wrong passwords, banned accounts and rate-limited addresses are refused (the connection is closed: how the real
    server refused a login is not known);
  * the relay hand-off (only with `relay_matches=True`): the game server address of a match (6916 MatchMcsIpAddr) is
    the relay's, with a session made for that room, and 6914 MatchGameRule says "mh1-relay" so a client can tell that
    every player joins (the PC client does not read it yet: docs/server.md 4.3).

Everything else is whatever mh1_testserver.Client does today. When B's server grows, this stub gets it for free.
"""
import asyncio
import os
import struct
import sys
import threading

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
import mh1_testserver as ts     # noqa: E402  (tools/mh1_testserver.py)


def decode_login_key(key_digits, seq):
    """undo mmbbc_encode (cnlbsh.c): two 5-digit numbers, each = seq + 4 digits of the MMBB id -> the 8 digits"""
    s = key_digits.decode("ascii", "replace")
    try:
        v1, v2 = int(s[0:5]) - (seq & 0xFFFF), int(s[5:10]) - (seq & 0xFFFF)
    except ValueError:
        return None
    if not (0 <= v1 <= 9999 and 0 <= v2 <= 9999):
        return None
    return "%04d%04d" % (v1, v2)


def make_handler(accounts=None, relay=None, relay_host="127.0.0.1", relay_matches=False, loop=None, log=print):
    base = ts.Client

    class Client(base):
        login = None

        def on_6101_2(self, seq, p):
            if accounts is not None:
                n = struct.unpack(">H", p[:2])[0]
                key = p[2:2 + n]
                pw, _ = self.enc_string(p, 2 + n, seq)
                login = decode_login_key(key, seq)
                pw = pw.split(b"\0")[0].decode("ascii", "replace")
                res = accounts.verify(login, pw, self.client_address[0]) if login else "bad"
                log("lobby: login %s from %s: %s" % (login, self.client_address[0], res))
                if res != "ok":
                    self.request.close()        # refused (docs/server.md 3.1: the real refusal packet is unknown)
                    raise ConnectionError("login refused")
                self.login = login
            base.on_6101_2(self, seq, p)

        if relay_matches and relay is not None:
            def on_6914_1(self, seq, p):        # MatchGameRule: tell the client that the "game server" is a relay
                self.send(ts.ANS, 0x6914, ts.str16("mh1-relay"), seq=seq)

            def on_6916_1(self, seq, p):        # MatchMcsIpAddr: the relay's address and this match's session port
                r = self.match_room() if hasattr(self, "match_room") else None
                if r is None:
                    self.send(ts.ANS, 0x6916, ts.str16(bytes(4)) + ts.str16(bytes(2)), seq=seq)
                    return
                with ts.ROOMS_LOCK:
                    port = getattr(r, "relay_port", None)
                    if port is None or getattr(r, "relay_match", None) is not r.match:
                        quest = (getattr(r, "prop", 0) >> 1) & 0xFF
                        names = [m.user_handle.encode("cp932", "replace").hex(" ").upper() for m in r.match]
                        fut = asyncio.run_coroutine_threadsafe(relay.create_session(quest, len(r.match), names), loop)
                        port = fut.result(5)
                        r.relay_port, r.relay_match = port, r.match
                ip = bytes(int(x) for x in relay_host.split("."))
                self.send(ts.ANS, 0x6916, ts.str16(ip) + ts.str16(struct.pack(">H", port)), seq=seq)

    return Client


class Lobby:
    """the lobby on its own thread (mh1_testserver is socketserver-based); start() / stop()"""
    def __init__(self, host, port, **kw):
        self.server = ts.Server((host, port), make_handler(**kw))
        self.port = self.server.server_address[1]
        self.thread = threading.Thread(target=self.server.serve_forever, daemon=True)

    def start(self):
        self.thread.start()
        return self.port

    def stop(self):
        self.server.shutdown()
        self.server.server_close()
