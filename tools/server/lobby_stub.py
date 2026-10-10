"""lobby_stub.py - the lobby service of mh1-server: tools/mh1_testserver.py's protocol handling plus accounts.

The lobby protocol (login, plazas, lobbies, rooms, chat, search, mail, matching, event quests; docs/network.md 5) lives
in tools/mh1_testserver.py; this module subclasses its connection handler and replaces its in-memory storage hooks with
the SQLite store (accounts.py, docs/server.md 3.2):

  * the login (6101: 8-digit key + obfuscated password) is checked against the store; unknown logins, wrong passwords,
    banned accounts and rate-limited addresses are refused (the connection is closed: the real refusal is not known);
  * the account's hunters come from the store (6131), a new hunter gets a stored id (6132; at most 3 per account), an
    id of another account is refused, the mini data (6190) is kept: ids stay the same across logins, so the friend
    lists the players keep on their cards stay valid;
  * mail to a hunter who is away is kept in the store and delivered at his next login (6704 / 6705);
  * where a hunter was when his connection ended inside a lobby (a crash, a lost line, the match's logout) is kept
    for 15 minutes: his next login goes back there (6891);
  * the relay hand-off (with `relay_matches=True`): 6914 says "mh1-relay", 6916 gives the relay's address and a session
    made for that room.
Without a store (`accounts=None`, `mh1_server.py serve --open`) any login is accepted and the storage is the test
server's memory: for tests on 127.0.0.1 only.
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


def ts_strs(p, n, off=0):
    """n strings (u16 length + bytes) from p"""
    out = []
    for _ in range(n):
        ln = struct.unpack(">H", p[off:off + 2])[0]
        out.append(p[off + 2:off + 2 + ln])
        off += 2 + ln
    return out, off


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

        if accounts is not None:
            def hunter_list(self):
                return [(i, bytes(h), bytes(m) if m else b"") for i, h, m in accounts.hunters(self.login)]

            def hunter_select(self, ident, handle):
                if not ident:
                    try:
                        hid = accounts.add_hunter(self.login, handle)
                    except ValueError as e:
                        return "!" + str(e)
                    log("lobby: account %s: new hunter %s" % (self.login, hid))
                    return hid
                if accounts.hunter_owner(ident) != self.login:
                    return "!this hunter belongs to another account"
                accounts.set_handle(ident, handle)
                return ident

            def mini_saved(self):
                accounts.set_mini(self.user_id, self.mini)

            def place_save(self, plaza, lobby):
                if accounts.hunter_owner(self.user_id):
                    accounts.place_save(self.user_id, plaza, lobby)

            def place_take(self):
                return accounts.place_take(self.user_id)

            def hunter_seen(self):
                pass

            def mail_keep(self, ident, payload):
                if accounts.hunter_handle(ident) is None:
                    return False
                (frm, handle, text), _ = ts_strs(payload, 3)
                accounts.queue_mail(ident, frm.decode("ascii", "replace"), text)
                return True

            def mail_pending(self):
                out = []
                for frm, body, sent in accounts.take_mail(self.user_id):
                    out.append(ts.str16(frm) + ts.str16((accounts.hunter_handle(frm) or b"?")[:16]) + ts.str16(body[:0x7E]))
                return out

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
