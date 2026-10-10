#!/usr/bin/env python3
"""Unit tests of mh1-server (tools/server), no game needed: python3 tools/server/test_server.py

Fake clients speak net_peer.c's frames to the relay, the game's login packet to the lobby stub, and mcsls records to
the Mcs skeleton, all on 127.0.0.1. The real-game test is `tools/test_coop.sh relay` (headless PC clients)."""
import asyncio
import os
import socket
import struct
import sys
import threading
import time
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import relay as R            # noqa: E402
import accounts as A         # noqa: E402
import mcs as M              # noqa: E402


def mini(name):
    m = bytearray(R.NP_MINI)
    m[0x18:0x18 + len(name)] = name
    return bytes(m)


class FakePeer:
    """a player's game as net_peer.c's joiner"""
    def __init__(self, reader, writer):
        self.r, self.w = reader, writer

    @classmethod
    async def join(cls, port, name=None):
        r, w = await asyncio.open_connection("127.0.0.1", port)
        p = cls(r, w)
        if name is not None:
            p.send(0xFF, R.HELLO, mini(name))
        return p

    def send(self, frm, typ, d=b""):
        self.w.write(R.frame(frm, typ, d))

    async def recv(self, timeout=2.0):
        hdr = await asyncio.wait_for(self.r.readexactly(4), timeout)
        n, frm, typ = struct.unpack("<HBB", hdr)
        d = await asyncio.wait_for(self.r.readexactly(n - 2), timeout) if n > 2 else b""
        return frm, typ, d

    async def closed(self, timeout=2.0):
        try:
            return await asyncio.wait_for(self.r.read(1), timeout) == b""
        except (ConnectionError, OSError):
            return True


def arun(coro):
    return asyncio.run(asyncio.wait_for(coro, 20))


class RelayTest(unittest.TestCase):
    def setUp(self):
        R.Limits.grace = 0.05
        R.Limits.start_wait = 300.0

    def test_hunt_of_three(self):
        async def go():
            rl = R.Relay(verbose=False)
            s = await rl.open_session(0, 137, 3, permanent=True)
            ps = []
            for k, nm in enumerate([b"ANNA", b"BOB", b"CARL"]):
                p = await FakePeer.join(s.port, nm)
                self.assertEqual(await p.recv(), (0, R.WELCOME, bytes([k])))
                ps.append(p)
            for k, p in enumerate(ps):          # START: quest, players, every slot's mini data in slot order
                frm, typ, d = await p.recv()
                self.assertEqual((typ, d[0], d[1]), (R.START, 137, 3))
                self.assertEqual(d[2 + R.NP_MINI * 1 + 0x18:2 + R.NP_MINI * 1 + 0x1B], b"BOB")
            ps[1].send(1, 2, b"\x02\x08\x01\x00\x11\x22\x33\x44")     # a game packet of slot 1 (channel 2)
            for k in (0, 2):
                self.assertEqual(await ps[k].recv(), (1, 2, b"\x02\x08\x01\x00\x11\x22\x33\x44"))
            ps[2].send(0, R.READY, b"\x01")                              # a forged "from" is replaced by the real slot
            self.assertEqual((await ps[0].recv())[:2], (2, R.READY))
            self.assertEqual((await ps[1].recv())[:2], (2, R.READY))
            ps[0].w.close()                                              # slot 0 (the quest's host) leaves...
            for k in (1, 2):
                self.assertEqual(await ps[k].recv(), (0, R.BYE, b""))
            ps[1].send(1, 8, b"\x08\x06\x01\x00\x05\x06")                # ...and the others hunt on
            self.assertEqual(await ps[2].recv(), (1, 8, b"\x08\x06\x01\x00\x05\x06"))
            late = await FakePeer.join(s.port, b"DAVE")                   # no joining a running hunt
            self.assertTrue(await late.closed())
            for p in ps[1:]:
                p.send(p is ps[1] and 1 or 2, R.BYE)
            await asyncio.sleep(0.2)
            self.assertFalse(s.started)                                  # permanent session: open for the next group
            self.assertEqual(len(rl.history), 1)
            await rl.close()
        arun(go())

    def test_not_a_players_frame(self):
        async def go():
            rl = R.Relay(verbose=False)
            s = await rl.open_session(0, 131, 2)
            a = await FakePeer.join(s.port, b"A")
            await a.recv()
            a.send(0, R.START, b"\x01\x01")         # only the relay starts a quest
            self.assertTrue(await a.closed())
            await rl.close()
        arun(go())

    def test_rate_limit_and_bad_length(self):
        async def go():
            rl = R.Relay(verbose=False)
            s = await rl.open_session(0, 131, 1)
            a = await FakePeer.join(s.port, b"A")
            await a.recv()
            await a.recv()              # START (one player)
            for _ in range(R.Limits.burst + 50):
                a.send(0, 2, b"\0\4\0\0")
            self.assertTrue(await a.closed())
            b = await FakePeer.join((await rl.open_session(0, 131, 2)).port)
            await b.recv()
            b.w.write(struct.pack("<HBB", 0x400, 0, 2))     # longer than any game packet
            self.assertTrue(await b.closed())
            await rl.close()
        arun(go())

    def test_start_with_whoever_came(self):
        async def go():
            R.Limits.start_wait = 0.3
            rl = R.Relay(verbose=False)
            s = await rl.open_session(0, 131, 4)
            a = await FakePeer.join(s.port, b"A")
            b = await FakePeer.join(s.port, b"B")
            await a.recv(), await b.recv()
            frm, typ, d = await a.recv()
            self.assertEqual((typ, d[1]), (R.START, 2))
            await rl.close()
        arun(go())

    def test_lobby_order_from_the_start(self):
        async def go():
            rl = R.Relay(verbose=False)
            port = await rl.create_session(131, 2, names=[b"BOB".hex(" ").upper(), b"ANNA".hex(" ").upper()])
            a = await FakePeer.join(port, b"ANNA")
            self.assertEqual(await a.recv(), (0, R.WELCOME, b"\1"))     # the room's order: ANNA is its second member
            b = await FakePeer.join(port, b"BOB")
            self.assertEqual(await b.recv(), (0, R.WELCOME, b"\0"))     # BOB slot 0, no renumbering at the start
            frm, typ, d = await a.recv()
            self.assertEqual((typ, d[1]), (R.START, 2))
            self.assertEqual((await b.recv())[1], R.START)
            a.w.close(); b.w.close()
            await asyncio.sleep(0.2)
            self.assertNotIn(port, rl.sessions)                          # a lobby session lives for one hunt
            await rl.close()
        arun(go())

    def test_lobby_order_stranger(self):
        async def go():
            rl = R.Relay(verbose=False)
            port = await rl.create_session(131, 2, names=[b"BOB".hex(" ").upper(), b"ANNA".hex(" ").upper()])
            c = await FakePeer.join(port, b"CARL")                      # not in the room's list: a slot no member needs
            b = await FakePeer.join(port, b"BOB")
            self.assertEqual(await b.recv(), (0, R.WELCOME, b"\0"))
            self.assertEqual(await c.recv(), (0, R.WELCOME, b"\1"))
            c.w.close(); b.w.close()
            await asyncio.sleep(0.2)
            await rl.close()
        arun(go())

    def test_refuses_public_bind_and_mhos(self):
        with self.assertRaises(ValueError):
            R.Relay("8.8.8.8")
        with self.assertRaises(ValueError):
            R.Relay("0.0.0.0")
        rl = R.Relay("0.0.0.0", allow_public=True, verbose=False)
        self.assertFalse(rl.peer_allowed("34.75.107.68"))
        self.assertTrue(rl.peer_allowed("8.8.8.8"))
        self.assertFalse(R.Relay(verbose=False).peer_allowed("8.8.8.8"))


class AccountTest(unittest.TestCase):
    def test_accounts(self):
        db = A.AccountDB(":memory:")
        login, pw = db.create_account()
        self.assertTrue(len(login) == 8 and login.isdigit() and len(pw) == 16)
        self.assertEqual(db.verify(login, pw, "1.2.3.4"), "ok")
        self.assertEqual(db.verify(login, "wrong", "1.2.3.4"), "bad")
        self.assertEqual(db.verify("99999999", pw, "1.2.3.4"), "bad")
        stored = db.db.execute("SELECT pw_hash FROM account").fetchone()[0]
        self.assertNotIn(pw, stored)
        db.ban(login, "test")
        self.assertEqual(db.verify(login, pw, "1.2.3.5"), "banned")
        db.ban(login, None, banned=False)
        for _ in range(5):
            db.verify(login, "wrong", "5.6.7.8")
        self.assertEqual(db.verify(login, pw, "5.6.7.9"), "limited")    # 5 failures on this login: wait
        h = db.add_hunter(login, "ＡＮＮＡ".encode("cp932"))
        db.set_mini(h, bytes(range(0x40)))
        self.assertEqual(db.hunters(login)[0][0], h)
        h2 = db.add_hunter(login, b"BOB")
        db.add_friend(h, h2)
        self.assertEqual(db.friends(h), [h2])
        db.queue_mail(h2, h, b"hi")
        self.assertEqual(len(db.take_mail(h2)), 1)
        self.assertEqual(db.take_mail(h2), [])
        self.assertEqual(len(db.export(login)["hunters"]), 2)
        db.delete_account(login)
        self.assertEqual(db.hunters(login), [])
        self.assertEqual(db.db.execute("SELECT COUNT(*) FROM friend").fetchone()[0], 0)
        with self.assertRaises(ValueError):
            db.create_account("1234", "x")


def mmbbc_encode(key, seq):
    """the client's mmbbc_encode (cnlbsh.c): non-digits are skipped by read_col_numeric"""
    def col(s):
        v = 0
        for c in s:
            if c.isdigit():
                v = v * 10 + int(c)
        return v
    return ("%05d%05d" % ((seq & 0xFFFF) + col(key[0:4]), (seq & 0xFFFF) + col(key[4:8]))).encode()


class LobbyStubTest(unittest.TestCase):
    def login(self, port, key, pw):
        import mh1_testserver as ts
        s = socket.create_connection(("127.0.0.1", port), timeout=3)
        h = s.recv(12)
        ln = struct.unpack(">H", h[4:6])[0]
        seq = struct.unpack(">H", h[6:8])[0]
        xfee = struct.unpack(">H", s.recv(ln))[0]
        enc = ts.lbs_decode(pw.encode(), seq, xfee)          # the obfuscation is its own inverse
        body = struct.pack(">H", 10) + mmbbc_encode(key, seq) + struct.pack(">HH", len(enc) + 2, sum(pw.encode()) & 0x7FFF) + enc
        s.sendall(struct.pack(">BBHHHB3s", 0x81, 2, 0x6101, len(body), seq, 0, b"\xff" * 3) + body)
        try:
            r = s.recv(12)
        except OSError:
            r = b""
        s.close()
        return r[2:4] == b"\x61\x02"       # the next step (telephone number request): login accepted

    def test_login_against_accounts(self):
        import lobby_stub as L
        self.assertEqual(L.decode_login_key(mmbbc_encode("12345678", 0x1234), 0x1234), "12345678")
        self.assertEqual(L.decode_login_key(mmbbc_encode("TESTKEY001", 7), 7), "00000000")
        db = A.AccountDB(":memory:")
        login, pw = db.create_account()
        lb = L.Lobby("127.0.0.1", 0, accounts=db, log=lambda *a: None)
        port = lb.start()
        try:
            self.assertTrue(self.login(port, login, pw))
            self.assertFalse(self.login(port, login, "WRONGPASSWORD000"))
            self.assertFalse(self.login(port, "87654321", pw))
        finally:
            lb.stop()


class FakeLobbyClient:
    """the game's lobby client as far as these tests need it: the real login packets (obfuscated strings, docs/network.md
    5.2-5.3), then requests by code"""
    def __init__(self, port, handle, ident=b"******", key="00000000", pw="LOCALTEST0000000", expect_list=None, refused=False):
        import mh1_testserver as ts
        self.ts = ts
        self.s = socket.create_connection(("127.0.0.1", port), timeout=3)
        self.seq = 0x40
        self.buf = b""
        cat, cmd, seq, res, p = self.recv()
        self.xfee = struct.unpack(">H", p)[0]
        self.answer(0x6101, seq, struct.pack(">H", 10) + mmbbc_encode(key, seq) + self.enc(pw.encode(), seq))
        cat, cmd, seq, res, p = self.recv()
        self.answer(0x6102, seq, self.enc(b"", seq))
        cat, cmd, seq, res, p = self.recv()
        self.answer(0x6103, seq, bytes([0, 1, 4]) + self.enc(b"0123456789", seq) + bytes(16))
        lst = self.until(0x6131)[4]
        if expect_list is not None:
            assert lst[0] == expect_list, lst
        self.request(0x6132, lambda q: self.enc(ident, q) + self.enc(handle, q))
        ans = self.until(0x6132)
        self.refused = ans[3] != 0
        if self.refused:
            return
        self.id = ans[4][2:].decode()
        self.until(0x6104)
        m = bytearray(0x40)
        m[0], m[1] = 3, 7       # weapon kind 3, rank 7
        self.request(0x6190, lambda q: self.enc(bytes(m), q))
        self.until(0x6190)
        self.send(16, 0x6141, b"", self.next())

    def enc(self, b, seq):
        e = self.ts.lbs_decode(b, seq, self.xfee)               # the obfuscation is its own inverse
        return struct.pack(">HH", len(e) + 2, sum(b) & 0x7FFF) + e

    def next(self):
        self.seq += 1
        return self.seq

    def send(self, cat, cmd, p, seq):
        self.s.sendall(struct.pack(">BBHHHB3s", 0x81, cat, cmd, len(p), seq, 0, b"\xff" * 3) + p)

    def answer(self, cmd, seq, p):
        self.send(2, cmd, p, seq)

    def request(self, cmd, body):
        q = self.next()
        self.send(1, cmd, body(q) if callable(body) else body, q)
        return q

    def recv(self):
        while len(self.buf) < 12 or len(self.buf) < 12 + struct.unpack(">H", self.buf[4:6])[0]:
            d = self.s.recv(4096)
            if not d:
                raise ConnectionError("closed")
            self.buf += d
        cat, cmd, ln, seq, res = struct.unpack(">BHHHB", self.buf[1:9])
        p, self.buf = self.buf[12:12 + ln], self.buf[12 + ln:]
        return cat, cmd, seq, res, p

    def until(self, cmd):
        while True:
            r = self.recv()
            if r[1] == cmd:
                return r

    def enter(self, plaza=1, lobby=1):
        self.request(0x6207, struct.pack(">H", plaza))
        self.until(0x6207)
        self.request(0x6305, struct.pack(">H", lobby))
        self.until(0x6305)

    def close(self):
        self.s.close()


def strs(p, off, n):
    out = []
    for _ in range(n):
        ln = struct.unpack(">H", p[off:off + 2])[0]
        out.append(p[off + 2:off + 2 + ln])
        off += 2 + ln
    return out, off


class TestServerFeatureTest(unittest.TestCase):
    """tools/mh1_testserver.py: user search, condition search with pages, mail, the return to the lobby after a match"""
    def setUp(self):
        import mh1_testserver as ts
        self.ts = ts
        self.srv = ts.Server(("127.0.0.1", 0), ts.Client)
        self.port = self.srv.server_address[1]
        threading.Thread(target=self.srv.serve_forever, daemon=True).start()

    def tearDown(self):
        self.srv.shutdown()
        self.srv.server_close()

    def test_search_mail_return(self):
        a = FakeLobbyClient(self.port, b"ANNA")
        b = FakeLobbyClient(self.port, b"BOB")
        a.enter(1, 2)
        b.enter(1, 2)
        # 6703: where is BOB
        a.request(0x6703, lambda q: a.enc(b.id.encode(), q))
        cat, cmd, seq, res, p = a.until(0x6703)
        (ident,), off = strs(p, 0, 1)
        self.assertEqual((res, ident.decode()), (0, b.id))
        self.assertEqual(struct.unpack(">HHH", p[off:off + 6]), (1, 2, 0))
        a.request(0x6703, lambda q: a.enc(b"999999", q))
        self.assertEqual(a.until(0x6703)[3], 1)               # not logged in: an error and a message
        # 6709: everyone (no conditions), then by weapon kind and rank; pages of SEARCH_PAGE records
        self.ts.Client.SEARCH_PAGE = 1
        try:
            a.request(0x6709, bytes([0x50, 0]))
            cat, cmd, seq, res, p = a.until(0x6709)
            total, start, count, last = p[:4]
            self.assertEqual((total, start, count, last), (2, 0, 1, 0))
            a.send(16, 0x6709, bytes([1]), 0x99)                # "certify": the next page, answered with this seq
            cat, cmd, seq, res, p = a.until(0x670A)
            self.assertEqual((seq, cat) + tuple(p[:4]), (0x99, 2, 2, 1, 1, 1))
        finally:
            self.ts.Client.SEARCH_PAGE = 10
        a.request(0x6709, lambda q: bytes([0x50, 2, 3, 3, 6, 1, 1]))     # weapon kind 3, rank 5..8
        p = a.until(0x6709)[4]
        self.assertEqual(p[0], 2)
        a.request(0x6709, lambda q: bytes([0x50, 1, 1]) + a.enc(b.id.encode(), q))
        p = a.until(0x6709)[4]
        self.assertEqual(p[0], 1)
        self.assertEqual(strs(p, 4, 2)[0][1], b"BOB")
        # 6704 mail to an online hunter: 6705 to him
        a.request(0x6704, lambda q: a.enc(b.id.encode(), q) + a.enc(b"hello BOB", q))
        self.assertEqual(a.until(0x6704)[3], 0)
        p = b.until(0x6705)[4]
        self.assertEqual(strs(p, 0, 3)[0], [a.id.encode(), b"ANNA", b"hello BOB"])
        # mail to a hunter who logged out is kept until his next login
        bid = b.id
        b.close()
        time.sleep(0.2)
        a.request(0x6704, lambda q: a.enc(bid.encode(), q) + a.enc(b"later", q))
        self.assertEqual(a.until(0x6704)[3], 0)
        a.request(0x6704, lambda q: a.enc(b"424242", q) + a.enc(b"nobody", q))
        self.assertEqual(a.until(0x6704)[3], 1)
        # BOB left inside lobby 2 (as after a match): his next login with his id is put back there (6891)
        b2 = FakeLobbyClient(self.port, b"BOB", bid.encode())
        p = b2.until(0x6705)[4]
        self.assertEqual(strs(p, 0, 3)[0][2], b"later")
        b2.request(0x6891, b"")
        self.assertEqual(struct.unpack(">HHH", b2.until(0x6891)[4]), (1, 2, 0))
        self.assertEqual(a.until(0x6411)[1], 0x6411)           # ANNA is told he is back
        # personal data (6181 may I, 6182-6187 the fields, 6188 done): both answered with result 0, nothing stored
        a.request(0x6181, b"")
        self.assertEqual(a.until(0x6181)[3], 0)
        for code, field in ((0x6182, b"NAME"), (0x6183, b"000-0000"), (0x6184, b"ADDR"), (0x6185, b"000")):
            q = a.next()
            a.send(16, code, a.enc(field, q), q)
        a.send(16, 0x6186, bytes([30]), a.next())
        q = a.next()
        a.send(16, 0x6187, a.enc(b"x@example.invalid", q), q)
        a.request(0x6188, b"")
        self.assertEqual(a.until(0x6188)[3], 0)
        # event quests: none by default (6881: u8 0)
        a.request(0x6881, b"")
        self.assertEqual(a.until(0x6881)[4], bytes([0]))
        a.close()
        b2.close()

    def test_event_quest_download(self):
        self.ts.EVENT_FILES = [bytes(range(256)) * 3]
        try:
            c = FakeLobbyClient(self.port, b"CARL")
            c.request(0x6881, b"")
            p = c.until(0x6881)[4]
            self.assertEqual(p, bytes([1]) + struct.pack(">I", 768))
            got = b""
            for ofs in range(0, 768, 0x200):
                c.request(0x6882, struct.pack(">BII", 0, ofs, 0x200))
                p = c.until(0x6882)[4]
                f, o, n = struct.unpack(">BII", p[:9])
                self.assertEqual((f, o, n, struct.unpack(">H", p[9:11])[0]), (0, ofs, min(0x200, 768 - ofs), n))
                got += p[11:11 + n]
            self.assertEqual(got, bytes(range(256)) * 3)
            c.close()
        finally:
            self.ts.EVENT_FILES = []


class StoredLobbyTest(unittest.TestCase):
    """mh1-server's lobby with its SQLite store: hunters keep their ids, mail and places survive a server restart,
    an id of another account is refused, a crashed client's room and lobby are cleaned up"""
    def lobby(self, path):
        import lobby_stub as L
        db = A.AccountDB(path)
        lb = L.Lobby("127.0.0.1", 0, accounts=db, log=lambda *a: None)
        return lb, lb.start(), db

    def test_store(self):
        import tempfile
        d = tempfile.mkdtemp()
        path = os.path.join(d, "s.sqlite3")
        db = A.AccountDB(path)
        la, pa = db.create_account("11112222", "PASSWORDANNA0000")
        lb_, pb = db.create_account("33334444", "PASSWORDBOB00000")
        del db
        srv, port, db = self.lobby(path)
        try:
            a = FakeLobbyClient(port, b"ANNA", key=la, pw=pa)
            b = FakeLobbyClient(port, b"BOB", key=lb_, pw=pb)
            self.assertEqual(len(a.id), 6)
            a.enter(1, 1)
            b.enter(1, 1)
            # BOB's game dies (the socket just closes): ANNA is told, his place is kept
            b.s.close()
            self.assertEqual(a.until(0x6410)[1], 0x6410)
            bid, aid = b.id, a.id
            # mail to BOB while he is away: kept in the store
            a.request(0x6704, lambda q: a.enc(bid.encode(), q) + a.enc(b"see you", q))
            self.assertEqual(a.until(0x6704)[3], 0)
            a.close()
        finally:
            srv.stop()
        # the server restarts on the same store
        srv, port, db = self.lobby(path)
        try:
            b2 = FakeLobbyClient(port, b"BOB", ident=bid.encode(), key=lb_, pw=pb, expect_list=1)
            self.assertEqual(b2.id, bid)                                    # the stored hunter
            p = b2.until(0x6705)[4]
            self.assertEqual(strs(p, 0, 3)[0], [aid.encode(), b"ANNA", b"see you"])
            b2.request(0x6891, b"")
            self.assertEqual(struct.unpack(">HHH", b2.until(0x6891)[4]), (1, 1, 0))   # back where he was
            b2.close()
            # ANNA's hunter id with BOB's account: refused
            x = FakeLobbyClient(port, b"BOB", ident=aid.encode(), key=lb_, pw=pb, expect_list=1, refused=True)
            self.assertTrue(x.refused)
            x.close()
        finally:
            srv.stop()


class McsTest(unittest.TestCase):
    def test_stream_and_relay(self):
        st = M.ClientStream()
        login = struct.pack(">BBHHHB3s", 0x82, 2, 0x1031, 12, 5, 0, b"\xff" * 3) + struct.pack(">H", 10) + b"0000500005"
        recs = bytes([2, 0xF1]) + bytes([5, 0x41, 1, 2, 3])
        out = list(st.feed(login + recs[:3])) + list(st.feed(recs[3:]))
        self.assertEqual(out[0][:3], ("login", 0x1031, 5))
        self.assertEqual([o[1] for o in out[1:]], [bytes([2, 0xF1]), bytes([5, 0x41, 1, 2, 3])])
        with self.assertRaises(ValueError):
            list(st.feed(bytes([0x28, 0x41])))     # a client never sends a 0x28-long record

        class W:
            def __init__(self):
                self.d = b""

            def write(self, b):
                self.d += b
        s = M.McsSession({"00000000": 0, "00000001": 1, "00000002": 2})
        ms = [M.McsMember(W(), i) for i in range(3)]
        for m in ms:
            s.members[m.id] = m
        self.assertTrue(s.relay(ms[1], bytes([4, 0x41, 9, 9])))
        self.assertEqual((ms[0].writer.d, ms[1].writer.d, ms[2].writer.d), (bytes([4, 0x41, 9, 9]), b"", bytes([4, 0x41, 9, 9])))
        self.assertFalse(s.relay(ms[1], bytes([2, 0xF2])))     # another player's id: refused
        self.assertEqual(M.server_packet(M.CMD_HEALTH)[:4], bytes([0x28, 1, 0x10, 0x21]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
