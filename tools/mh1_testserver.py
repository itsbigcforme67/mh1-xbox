#!/usr/bin/env python3
"""mh1_testserver.py - a minimal private lobby server for testing the MH1 online client.

NOT an MH Oldschool server and not a re-implementation of Capcom's. It speaks just enough of the
lobby-server protocol of the PS2 client (derived from the decompiled client code, documented
in docs/network.md) to get a client through connection, login and into the plaza / lobby lists,
so the PC (and later Xbox) build's network code can be tested on 127.0.0.1 without touching
anyone's servers. Python 3.8+, standard library only.

    python3 tools/mh1_testserver.py [--host 127.0.0.1] [--port 10200] [-v]

Binds to loopback by default; it refuses non-private bind addresses unless --allow-public.
"""
import argparse
import socketserver
import struct
import sys
import threading
import time

MAGIC = 0x81
KEY = b"LOCKROCK"            # the 8-byte table the client's string obfuscation uses (lbs_encode_ex)

# cat values in the header
REQ, ANS, NOTE = 1, 2, 16

# wire command codes (tools/lbs_cmdtab.py prints the full table from the game)
C_LINECHECK = 0x6001
C_LOGOUT = 0x6002
C_SHUTDOWN = 0x6006
C_ECHO = 0x600E
C_CONNPAIR = 0x6101
C_TEL = 0x6102
C_FIRSTDATA = 0x6103
C_LOGINOK = 0x6104
C_WARNING = 0x6105
C_USERLIST = 0x6131
C_USERID = 0x6132
C_BATTLERES = 0x6138
C_LOGINFIN = 0x6141
C_TOPINFO = 0x614C
C_MINIDATA = 0x6190
C_PIECE = {  # per kind: 0 plaza, 1 lobby, 2 room: entry, count, name, joinuser, status, explain, exit
    0: dict(entry=0x6207, count=0x6203, name=0x6204, join=0x6205, status=0x6206, explain=0x620A, exit=0x6208),
}
C_PLACE = 0x6891

VERBOSE = False


def log(*a):
    if VERBOSE:
        print(time.strftime("%H:%M:%S"), *a, flush=True)


def is_private(host):
    import ipaddress
    try:
        ip = ipaddress.ip_address(host)
    except ValueError:
        return host == "localhost"
    return ip.is_loopback or ip.is_private or ip.is_link_local


def lbs_decode(data, seq, xfee):
    """undo lbs_encode_ex: out[i] = in[i] ^ KEY[i & 7] ^ (xfee + (seq & 0xFF) + i)"""
    return bytes((c ^ KEY[i & 7] ^ ((xfee + (seq & 0xFF) + i) & 0xFF)) for i, c in enumerate(data))


def str16(s):
    """u16 length + bytes: the plain string format the client's GetRecvDataString reads"""
    b = s if isinstance(s, bytes) else s.encode("cp932", "replace")
    return struct.pack(">H", len(b)) + b


class Registry:
    """who is where (kind 0 plaza, 1 lobby, 2 room -> id -> clients), shared by all connections"""
    def __init__(self):
        self.lock = threading.Lock()
        self.where = {}             # client -> {kind: id}

    _next = [0]

    def new_id(self):
        with self.lock:
            self._next[0] += 1
            return "%06d" % self._next[0]

    def enter(self, cl, kind, i):
        with self.lock:
            self.where.setdefault(cl, {})[kind] = i

    def leave(self, cl, kind=None):
        """kind None: the client is gone; else it left that kind of place. Returns the id it left (or None)."""
        with self.lock:
            w = self.where.get(cl)
            if w is None:
                return None
            if kind is None:
                self.where.pop(cl, None)
                return w.get(1)
            return w.pop(kind, None)

    def others(self, cl, kind, i):
        with self.lock:
            return [c for c, w in self.where.items() if c is not cl and w.get(kind) == i]

    def members(self, kind, i):
        with self.lock:
            return [c for c, w in self.where.items() if w.get(kind) == i]

    def count(self, kind, i):
        return len(self.members(kind, i))


REG = Registry()


class Client(socketserver.BaseRequestHandler):
    def setup(self):
        self.sock = self.request
        self.sock.settimeout(60)
        self.seq = 0x100
        self.xfee = 0x1234
        self.user_handle = ""
        self.user_id = REG.new_id()
        self.echoes = 0
        self.buf = b""
        self.lock = threading.Lock()
        self.mini = b"\0" * 0x40
        self.h = {}
        self.build_piece_handlers()

    # ---- packet io ----
    def send(self, cat, cmd, payload=b"", seq=None, res=0):
        if seq is None:
            self.seq = (self.seq + 1) & 0xFFFF
            seq = self.seq
        hdr = struct.pack(">BBHHHBBBB", MAGIC, cat, cmd, len(payload), seq, res, 0xFF, 0xFF, 0xFF)
        with self.lock:
            self.sock.sendall(hdr + payload)
        log("-> cat %d cmd %04X seq %04X res %d len %d %s" % (cat, cmd, seq, res, len(payload), payload[:24].hex()))
        return seq

    def recv_packet(self):
        while len(self.buf) < 12:
            d = self.sock.recv(4096)
            if not d:
                return None
            self.buf += d
        magic, cat, cmd, ln, seq, res = struct.unpack(">BBHHHB", self.buf[:9])
        while len(self.buf) < 12 + ln:
            d = self.sock.recv(4096)
            if not d:
                return None
            self.buf += d
        payload = self.buf[12:12 + ln]
        self.buf = self.buf[12 + ln:]
        log("<- cat %d cmd %04X seq %04X res %d len %d %s" % (cat, cmd, seq, res, ln, payload[:24].hex()))
        return magic, cat, cmd, seq, res, payload

    def enc_string(self, payload, off, seq):
        """one obfuscated string: u16 len+2, u16 checksum, len bytes"""
        total, chk = struct.unpack(">HH", payload[off:off + 4])
        n = total - 2
        raw = payload[off + 4:off + 4 + n]
        return lbs_decode(raw, seq, self.xfee), off + 4 + n

    # ---- the conversation ----
    def handle(self):
        log("client connected", self.client_address)
        try:
            self.login_start()
            while True:
                p = self.recv_packet()
                if p is None:
                    break
                self.dispatch(*p)
        except (ConnectionError, OSError):
            pass
        finally:
            lobby = REG.leave(self)
            if lobby is not None:
                for o in REG.others(self, 1, lobby):
                    o.safe_send(NOTE, 0x6410, str16(self.user_id))
            log("client gone")

    def login_start(self):
        self.send(REQ, C_CONNPAIR, struct.pack(">H", self.xfee))

    def dispatch(self, magic, cat, cmd, seq, res, payload):
        h = self.h.get((cmd, cat)) or getattr(self, "on_%04X_%d" % (cmd, cat), None)
        if h:
            h(seq, payload)
            return
        if cat == REQ:
            log("unhandled request %04X: answering empty" % cmd)
            self.send(ANS, cmd, b"", seq=seq)
        else:
            log("unhandled cat %d cmd %04X" % (cat, cmd))

    # login ---------------------------------------------------------------
    def on_6101_2(self, seq, p):            # connection pair answer: key (10 digits) + obfuscated password
        n = struct.unpack(">H", p[:2])[0]
        pw, _ = self.enc_string(p, 2 + n, seq)
        log("connection pair: password %r" % pw)
        self.send(REQ, C_TEL, b"")

    def on_6102_2(self, seq, p):            # telephone number
        tel, _ = self.enc_string(p, 0, seq)
        log("telephone %r" % tel)
        self.send(REQ, C_FIRSTDATA, b"")

    def on_600E_1(self, seq, p):            # echo packet (the client measures the round trip 4 times)
        self.echoes += 1
        self.send(ANS, C_ECHO, b"", seq=seq)

    def on_6103_2(self, seq, p):            # first data (client type, version, ...)
        log("first data %s" % p.hex())
        # an empty account list: a new hunter, the client then sends 6132
        self.send(NOTE, C_USERLIST, bytes([0]))

    def on_6132_1(self, seq, p):            # account selected / created
        ident, off = self.enc_string(p, 0, seq)
        handle, _ = self.enc_string(p, off, seq)
        self.user_handle = handle.decode("cp932", "replace")
        if ident.strip(b"*\0 ") == b"":
            ident = self.user_id.encode()
        else:
            self.user_id = ident.decode("ascii", "replace")
        log("account %r handle %r" % (ident, self.user_handle))
        self.send(ANS, C_USERID, str16(self.user_id), seq=seq)
        self.send(NOTE, C_LOGINOK, b"")

    def on_6190_1(self, seq, p):            # mini data (the hunter's public data) registered
        n = struct.unpack(">H", p[:2])[0]
        self.mini, _ = self.enc_string(p, 0, seq)
        self.send(ANS, C_MINIDATA, b"", seq=seq)

    def on_6141_16(self, seq, p):           # login finished: nothing to answer
        log("login finished")

    def on_614C_1(self, seq, p):            # top information
        html = "<html><body>MH1 test server<br>private test server, not an MH Oldschool server</body></html>"
        self.send(ANS, C_TOPINFO, bytes([0]) + str16(html), seq=seq)

    def on_6891_1(self, seq, p):            # current place
        self.send(ANS, C_PLACE, struct.pack(">HHH", 0, 0, 0), seq=seq)

    # plazas, lobbies, rooms: "pieces" of the town. The client asks for the same things for each kind
    # (count, name, join user counts, status, explanation); only the wire codes differ.
    PIECE = {
        0: dict(count=0x6203, name=0x6204, join=0x6205, status=0x6206, explain=0x620A, entry=0x6207, exit=0x6306),
        1: dict(count=0x6301, name=0x6302, join=0x6303, status=0x6304, explain=0x6308, entry=0x6305, exit=0x6408),
        2: dict(count=0x6401, name=0x6402, join=0x6403, status=0x6404, explain=0x640D, entry=0x6406, exit=0x6501),
    }
    # (name, status) per piece; status 3 = open. Lobbies are numbered across all plazas.
    TABLE = {
        0: [("Test Plaza 1", 3), ("Test Plaza 2", 3)],
        1: [("Test Lobby 1", 3), ("Test Lobby 2", 3), ("Test Lobby 3", 3), ("Test Lobby 4", 3)],
        2: [],
    }

    def build_piece_handlers(self):
        for kind, c in self.PIECE.items():
            self.h[(c["count"], REQ)] = lambda seq, p, k=kind, c=c: self.send(
                ANS, c["count"], struct.pack(">H", len(self.TABLE[k])), seq=seq)
            self.h[(c["name"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_name(k, c, seq, p)
            self.h[(c["status"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_status(k, c, seq, p)
            self.h[(c["join"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_join(k, c, seq, p)
            self.h[(c["explain"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_explain(k, c, seq, p)
            self.h[(c["entry"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_entry(k, c, seq, p)
            self.h[(c["exit"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_exit(k, c, seq, p)

    def piece_name(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        self.send(ANS, c["name"], struct.pack(">H", i) + str16(self.TABLE[k][i - 1][0]), seq=seq)

    def piece_status(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        self.send(ANS, c["status"], struct.pack(">HB", i, self.TABLE[k][i - 1][1]), seq=seq)

    def piece_join(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        self.send(ANS, c["join"], struct.pack(">HHH", i, REG.count(k, i), 100), seq=seq)

    def piece_explain(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        self.send(ANS, c["explain"], struct.pack(">H", i) + str16("private test " + ("plaza", "lobby", "room")[k]), seq=seq)

    def piece_entry(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        REG.enter(self, k, i)
        self.send(ANS, c["entry"], b"", seq=seq)
        if k == 1:      # tell the others in the lobby (NoticeLobbyCommer)
            for o in REG.others(self, 1, i):
                o.safe_send(NOTE, 0x6411, str16(self.user_id) + str16(self.user_handle[:16]) + str16(self.mini))

    def piece_exit(self, k, c, seq, p):
        i = REG.leave(self, k)
        self.send(ANS, c["exit"], b"", seq=seq)
        if k == 1 and i is not None:
            for o in REG.others(self, 1, i):
                o.safe_send(NOTE, 0x6410, str16(self.user_id))

    def safe_send(self, *a, **kw):
        try:
            self.send(*a, **kw)
        except OSError:
            pass

    def on_6701_16(self, seq, p):           # chat message to the lobby (text obfuscated, then a flag byte)
        text, off = self.enc_string(p, 0, seq)
        lobby = REG.where.get(self, {}).get(1)
        log("chat %r in lobby %s" % (text, lobby))
        if lobby is None:
            return
        out = str16(self.user_id) + str16(self.user_handle[:16]) + str16(text) + bytes([0, 0, 0, 0])
        for o in REG.others(self, 1, lobby):
            o.safe_send(NOTE, 0x6702, out)

    def on_630A_1(self, seq, p):            # lobby member list: u16 ?, u8 fields per entry, u8 count, entries
        i = struct.unpack(">H", p[:2])[0]
        members = REG.members(1, i)
        out = struct.pack(">HBB", 0, 3, len(members))
        for m in members:
            out += str16(m.user_id) + str16(m.user_handle[:16]) + str16(m.mini)
        self.send(ANS, 0x630A, out, seq=seq)

    def on_6002_1(self, seq, p):            # logout
        self.send(ANS, C_LOGOUT, b"", seq=seq)


class Server(socketserver.ThreadingTCPServer):
    allow_reuse_address = True
    daemon_threads = True


def main():
    global VERBOSE
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=10200)
    ap.add_argument("--allow-public", action="store_true", help="bind a non-private address")
    ap.add_argument("-v", "--verbose", action="store_true")
    a = ap.parse_args()
    VERBOSE = a.verbose
    if not is_private(a.host) and not a.allow_public:
        sys.exit("refusing to bind %s: only loopback / private addresses unless --allow-public" % a.host)
    srv = Server((a.host, a.port), Client)
    print("mh1_testserver listening on %s:%d" % (a.host, srv.server_address[1]), flush=True)
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
