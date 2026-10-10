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
COOP_PORT = 10300                   # the co-op session's port: this + the room number (--coop-port)


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


def html_msg(text):
    """a server message the client shows (an error result's text, 6706): the game's HTML text (nwDispStr_Html, the
    PC's src/lobby/b/nm/html_text.c) draws nothing unless the text starts with a tag, so it goes as <BODY>text<END>
    (tags: BODY, SIZE=n, COLOR=n, BR, CENTER, LEFT, RIGHT, END, LF=n, C=n)"""
    return str16("<BODY>" + text + "<END>")


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
# where a hunter was when his connection ended inside a lobby (after a match the client logs out without leaving it):
# user id -> (plaza, lobby). The next login with that id (CnetWork+5 == 3: the return from an online quest sends the
# kept id) asks its current place (6891) and is put back there (lobby_return_to_lobby)
PLACES = {}
CLIENTS = set()                     # every connection (also before it entered a plaza)
KNOWN_IDS = set()                   # ids that logged in since the server started (mail to them is kept)
MAILBOX = {}                        # id -> [6705 payloads] for hunters not logged in
PATCH = None                        # --patch FILE: bytes sent as a patch at the login (client tests only)
EVENT_FILES = []                    # --event-quest FILE: the downloadable (event) quest files, in order
LINE_CHECK = 30.0                   # seconds between 6001 line checks
ADMIN_MESSAGE = ""                  # --admin-message: sent to each hunter at the first lobby entry (6706)


class Room:
    """one of the 8 room slots of a lobby (the quest groups made at the guild counter)"""
    def __init__(self, i):
        self.id = i
        self.reset()

    def reset(self):
        self.members = []           # clients, the creator (leader) first
        self.name = b""
        self.explain = b""
        self.pw = b""
        self.prop = 0               # u32 room property (SetRoomProperty 6509: the quest number and the leader's rank)
        self.rules = {}             # rule index -> the creator's choice (660B)
        self.max = 4                # players: rule 0's choice + 1 (the rule sheet's "players")

    def status(self):
        # 1 = free (the guild counter creates a room in the first free slot, Lbc_ReserveRoom), 3 = open for joining
        # (lbc_in_lobby_00_05: 3 -> join, 1 -> create). Other values make the client show the server message.
        return 3 if self.members else 1


# The room rules this server offers (docs/network.md 5.6 "Room rules"): (name, [choices], default choice). The game
# stores them but shows none; it matches only rule 0 against its own player-count strings (lb_guild_make_room,
# lb_rule_member: "１人".."４人" in Shift-JIS) and then sends the creator's "players" as rule 0's choice (660B 0, n-1).
# The server enforces it: the room holds that many (6403 max, 6406 refused when full). More rules can be added here;
# the creator cannot change them (the sheet has no rows for them): each keeps its default.
ROOM_RULES = [
    ("人数".encode("cp932"), [("%d人" % n).translate({ord(c): 0xFF10 + ord(c) - 0x30 for c in "0123456789"}).encode("cp932")
                               for n in range(1, 5)], 3),
]
ROOMS = {}                          # lobby id -> [Room x 8]
ROOMS_LOCK = threading.Lock()


def rooms_of(lobby):
    with ROOMS_LOCK:
        if lobby not in ROOMS:
            ROOMS[lobby] = [Room(i + 1) for i in range(8)]
        return ROOMS[lobby]


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
        self.search = []
        self.build_piece_handlers()
        CLIENTS.add(self)
        self.alive = True
        threading.Thread(target=self.line_check, daemon=True).start()

    def line_check(self):
        """6001 line check (S->C request, empty; the client answers 6001). The client logs out when it has received
        nothing for 0xE10 ticks = 2 minutes (internet_lobby_act, cw+0x35F4: "connection lost", To_LogOut(6)), so a
        hunter alone and idle in a quiet lobby needs something from the server. How often the real server checked is not
        known: every LINE_CHECK seconds here."""
        while self.alive:
            time.sleep(LINE_CHECK)
            if self.alive:
                self.safe_send(REQ, C_LINECHECK, b"")

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
            self.alive = False
            self.leave_room()
            w = REG.where.get(self, {})
            if w.get(1) is not None and self.user_handle:
                self.place_save(w.get(0) or 1, w[1])
            CLIENTS.discard(self)
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
        if PATCH is not None:       # --patch: a patch instead of the account list (docs/network.md 5.8)
            self.send_patch(PATCH)
            return
        # the account's hunters (6131: u8 n, n x (str id, str handle, str mini)); the client then sends 6132 with one of
        # them or "******" for a new one
        hs = self.hunter_list()
        self.send(NOTE, C_USERLIST, bytes([len(hs)]) + b"".join(str16(i) + str16(h[:16]) + str16((m or b"")[:0x40])
                                                                 for i, h, m in hs))

    def on_6132_1(self, seq, p):            # account selected / created
        ident, off = self.enc_string(p, 0, seq)
        handle, _ = self.enc_string(p, off, seq)
        ident = ident.split(b"\0")[0].strip(b"* ")
        hid = self.hunter_select(ident.decode("ascii", "replace"), handle.split(b"\0")[0])
        if not isinstance(hid, str) or hid.startswith("!"):
            msg = hid[1:] if isinstance(hid, str) else "this hunter cannot log in"
            log("account %r handle %r refused: %s" % (ident, handle, msg))
            self.send(ANS, C_USERID, html_msg(msg), seq=seq, res=1)
            return
        self.user_id = hid
        self.user_handle = handle.split(b"\0")[0].decode("cp932", "replace")
        log("account %r handle %r" % (self.user_id, self.user_handle))
        self.send(ANS, C_USERID, str16(self.user_id), seq=seq)
        self.send(NOTE, C_LOGINOK, b"")

    def hunter_list(self):                  # [(id, handle bytes, mini bytes)]: none here, every login is a new hunter
        return []

    def hunter_select(self, ident, handle): # the id to use; "!message" refuses. Here: a new id, or the one given
        return ident or self.user_id

    def mini_saved(self):                   # 6190 arrived (self.mini)
        pass

    def on_6190_1(self, seq, p):            # mini data (the hunter's public data) registered
        n = struct.unpack(">H", p[:2])[0]
        self.mini, _ = self.enc_string(p, 0, seq)
        self.mini_saved()
        self.send(ANS, C_MINIDATA, b"", seq=seq)

    def on_6141_16(self, seq, p):           # login finished: nothing to answer; mail kept while away
        log("login finished")
        self.hunter_seen()
        self.deliver_mail()


    def on_614C_1(self, seq, p):            # top information
        html = "<html><body>MH1 test server<br>private test server, not an MH Oldschool server</body></html>"
        self.send(ANS, C_TOPINFO, bytes([0]) + str16(html), seq=seq)

    def on_6891_1(self, seq, p):            # current place: u16 plaza, u16 lobby, u16 (0); zeros = none (the top menu)
        place = self.place_take()
        if place is None:
            self.send(ANS, C_PLACE, struct.pack(">HHH", 0, 0, 0), seq=seq)
            return
        plaza, lobby = place
        REG.enter(self, 0, plaza)       # back where he was: the client reads the lobby's members next (630A) and
        REG.enter(self, 1, lobby)       # does not send a lobby entry (Lbs_request_enter_lobby2)
        log("%s returns to plaza %d lobby %d" % (self.user_handle, plaza, lobby))
        self.send(ANS, C_PLACE, struct.pack(">HHH", plaza, lobby, 0), seq=seq)
        for o in REG.others(self, 1, lobby):
            o.safe_send(NOTE, 0x6411, str16(self.user_id) + str16(self.user_handle[:16]) + str16(self.mini))

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
            self.h[(c["count"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_count(k, c, seq, p)
            self.h[(c["name"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_name(k, c, seq, p)
            self.h[(c["status"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_status(k, c, seq, p)
            self.h[(c["join"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_join(k, c, seq, p)
            self.h[(c["explain"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_explain(k, c, seq, p)
            self.h[(c["entry"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_entry(k, c, seq, p)
            self.h[(c["exit"], REQ)] = lambda seq, p, k=kind, c=c: self.piece_exit(k, c, seq, p)

    def lobby(self):
        return REG.where.get(self, {}).get(1)

    def room(self, i):
        rs = rooms_of(self.lobby())
        return rs[i - 1] if 1 <= i <= len(rs) else None

    def piece_count(self, k, c, seq, p):
        n = 8 if k == 2 else len(self.TABLE[k])
        self.send(ANS, c["count"], struct.pack(">H", n), seq=seq)

    def piece_name(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        name = self.room(i).name if k == 2 else self.TABLE[k][i - 1][0]
        self.send(ANS, c["name"], struct.pack(">H", i) + str16(name), seq=seq)

    def piece_status(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        st = self.room(i).status() if k == 2 else self.TABLE[k][i - 1][1]
        self.send(ANS, c["status"], struct.pack(">HB", i, st), seq=seq)

    def piece_join(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        if k == 2:
            self.send(ANS, c["join"], struct.pack(">HHH", i, len(self.room(i).members), self.room(i).max), seq=seq)
        else:
            self.send(ANS, c["join"], struct.pack(">HHH", i, REG.count(k, i), 100), seq=seq)

    def piece_explain(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        text = self.room(i).explain if k == 2 else "private test " + ("plaza", "lobby")[k]
        self.send(ANS, c["explain"], struct.pack(">H", i) + str16(text), seq=seq)

    def piece_entry(self, k, c, seq, p):
        i = struct.unpack(">H", p[:2])[0]
        if k == 2:      # RoomEntry: u16 room, obfuscated password
            r = self.room(i)
            pw = b""
            if len(p) > 2:
                pw, _ = self.enc_string(p, 2, seq)
                pw = pw.split(b"\0")[0]
            if r is None or not r.members:
                self.send(ANS, c["entry"], html_msg("この部屋には参加できません。"), seq=seq, res=1)
                return
            if len(r.members) >= r.max:
                log("%s cannot join room %d: full (%d players)" % (self.user_handle, i, r.max))
                self.send(ANS, c["entry"], html_msg("満員のため参加できません。"), seq=seq, res=1)   # "full: cannot join"
                return
            if r.pw and pw != r.pw:
                log("%s cannot join room %d: wrong password" % (self.user_handle, i))
                self.send(ANS, c["entry"], html_msg("パスワードが違います。"), seq=seq, res=1)   # "wrong password"
                return
            REG.enter(self, 2, i)
            others = list(r.members)
            r.members.append(self)
            log("%s joins room %d of lobby %s (%d members)" % (self.user_handle, i, self.lobby(), len(r.members)))
            self.send(ANS, c["entry"], b"", seq=seq)
            for o in others:    # NoticeRoomCommer (6503): str id, str handle, str mini
                o.safe_send(NOTE, 0x6503, str16(self.user_id) + str16(self.user_handle[:16]) + str16(self.mini))
            self.room_changed(r)
            return
        REG.enter(self, k, i)
        self.send(ANS, c["entry"], b"", seq=seq)
        if k == 1 and ADMIN_MESSAGE and not getattr(self, "admin_sent", False):
            # 6706 request: str title, str html; the client shows it in the town (once its screens allow it) and answers
            # 6706 when closed (circle). Sent at the first lobby entry: at the login the client is still behind the
            # login screens' fade-out and the dialog would sit under it
            self.admin_sent = True
            self.send(REQ, 0x6706, str16("MH1 test server") + html_msg(ADMIN_MESSAGE))
        if k == 1:      # tell the others in the lobby (NoticeLobbyCommer)
            for o in REG.others(self, 1, i):
                o.safe_send(NOTE, 0x6411, str16(self.user_id) + str16(self.user_handle[:16]) + str16(self.mini))

    def leave_room(self):
        i = REG.leave(self, 2)
        if i is None:
            return
        r = self.room(i)
        if r is None or self not in r.members:
            return
        r.members.remove(self)
        for o in r.members:     # NoticeRoomLeaver (6502): str id
            o.safe_send(NOTE, 0x6502, str16(self.user_id))
        if not r.members:
            r.reset()
        self.room_changed(r)

    def room_changed(self, r):
        """the lobby's other hunters see the room's status and member count change (notices 6404 / 6403)"""
        for o in REG.members(1, self.lobby()):
            o.safe_send(NOTE, 0x6404, struct.pack(">HB", r.id, r.status()))
            o.safe_send(NOTE, 0x6403, struct.pack(">HHH", r.id, len(r.members), r.max))

    def piece_exit(self, k, c, seq, p):
        if k == 2:
            self.leave_room()
            self.send(ANS, c["exit"], b"", seq=seq)
            return
        i = REG.leave(self, k)
        self.send(ANS, c["exit"], b"", seq=seq)
        if k == 1 and i is not None:
            for o in REG.others(self, 1, i):
                o.safe_send(NOTE, 0x6410, str16(self.user_id))

    # rooms: made at the guild counter (lb_guild_make_room: read the rooms, reserve the first free one, its rules,
    # set name / password / rules / explanation, finish, property), joined from the quest board (Lb_join)
    def on_6407_1(self, seq, p):            # RoomCreate: u16 room
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        if r is None or r.members:
            self.send(ANS, 0x6407, html_msg("この部屋は使用中です。"), seq=seq, res=1)
            return
        r.reset()
        r.members = [self]
        REG.enter(self, 2, i)
        log("%s creates room %d in lobby %s" % (self.user_handle, i, self.lobby()))
        self.send(ANS, 0x6407, b"", seq=seq)
        self.room_changed(r)

    # room rules (6603-6608, 660E; docs/network.md 5.6): the client reads them for the room it makes (Lbc_GetRoomRule)
    # and for the room it joins (Lbc_GuestReadRoom); every request carries u16 room, rule k (0-based), choice c, and
    # every answer echoes k (and c): the client files them by those numbers. A missing answer hangs its read.
    def on_6603_1(self, seq, p):            # number of rules: u8 n
        self.send(ANS, 0x6603, bytes([len(ROOM_RULES)]), seq=seq)

    def rule_args(self, p):
        i, k = struct.unpack(">HB", p[:3])
        return self.room(i), k, (p[3] if len(p) > 3 else 0)

    def on_6607_1(self, seq, p):            # number of choices: u8 k, u8 n
        r, k, _ = self.rule_args(p)
        self.send(ANS, 0x6607, bytes([k, len(ROOM_RULES[k][1]) if k < len(ROOM_RULES) else 0]), seq=seq)

    def on_6604_1(self, seq, p):            # the rule's name: u8 k, str (<= 0x40 bytes)
        r, k, _ = self.rule_args(p)
        self.send(ANS, 0x6604, bytes([k]) + str16(ROOM_RULES[k][0][:0x40] if k < len(ROOM_RULES) else b""), seq=seq)

    def on_6606_1(self, seq, p):            # its current choice: u8 k, u8 c (the room's, else the default)
        r, k, _ = self.rule_args(p)
        now = r.rules.get(k, ROOM_RULES[k][2]) if r is not None and k < len(ROOM_RULES) else 0
        self.send(ANS, 0x6606, bytes([k, now]), seq=seq)

    def on_6605_1(self, seq, p):            # may the creator set it: u8 k, u8 1
        r, k, _ = self.rule_args(p)
        self.send(ANS, 0x6605, bytes([k, 1]), seq=seq)

    def on_6608_1(self, seq, p):            # a choice's name: u8 k, u8 c, str (<= 0x40 bytes)
        r, k, c = self.rule_args(p)
        names = ROOM_RULES[k][1] if k < len(ROOM_RULES) else []
        self.send(ANS, 0x6608, bytes([k, c]) + str16(names[c][:0x40] if c < len(names) else b""), seq=seq)

    def on_660E_1(self, seq, p):            # a choice's triples (meaning not known): u8 k, u8 c, u8 0
        r, k, c = self.rule_args(p)
        self.send(ANS, 0x660E, bytes([k, c, 0]), seq=seq)

    def on_6601_1(self, seq, p):            # may the room have a name: u8 1
        self.send(ANS, 0x6601, bytes([1]), seq=seq)

    def on_6602_1(self, seq, p):            # may it have a password
        self.send(ANS, 0x6602, bytes([1]), seq=seq)

    def on_660F_1(self, seq, p):            # may it have an explanation
        self.send(ANS, 0x660F, bytes([1]), seq=seq)

    def my_room(self):
        i = REG.where.get(self, {}).get(2)
        return self.room(i) if i else None

    def on_6609_1(self, seq, p):            # room name (obfuscated string)
        r = self.my_room()
        name, _ = self.enc_string(p, 0, seq)
        if r:
            r.name = name
        self.send(ANS, 0x6609, b"", seq=seq)

    def on_660A_1(self, seq, p):            # room password
        r = self.my_room()
        pw, _ = self.enc_string(p, 0, seq)
        if r:
            r.pw = pw.split(b"\0")[0]
            log("room %d has a password" % r.id)
        self.send(ANS, 0x660A, b"", seq=seq)

    def on_660B_1(self, seq, p):            # one rule: u8 rule, u8 choice
        r = self.my_room()
        if r and len(p) >= 2 and p[0] < len(ROOM_RULES) and p[1] < len(ROOM_RULES[p[0]][1]):
            r.rules[p[0]] = p[1]
            if p[0] == 0:           # players
                r.max = p[1] + 1
            log("room %d rule %d = %d (%s)" % (r.id, p[0], p[1], ROOM_RULES[p[0]][1][p[1]].decode("cp932", "replace")))
        self.send(ANS, 0x660B, b"", seq=seq)

    def on_6610_1(self, seq, p):            # explanation (the recruiting message)
        r = self.my_room()
        text, _ = self.enc_string(p, 0, seq)
        if r:
            r.explain = text
        self.send(ANS, 0x6610, b"", seq=seq)

    def on_660C_1(self, seq, p):            # rules finished
        self.send(ANS, 0x660C, b"", seq=seq)
        r = self.my_room()
        if r:
            self.room_changed(r)

    def on_6509_1(self, seq, p):            # SetRoomProperty: u32 (lb_guild_make_room: quest << 1, leader's HR << 9)
        r = self.my_room()
        if r:
            r.prop = struct.unpack(">I", p[:4])[0]
            log("room %d property %08X (quest %d)" % (r.id, r.prop, (r.prop >> 1) & 0xFF))
            for o in REG.members(1, self.lobby()):
                o.safe_send(NOTE, 0x650A, struct.pack(">HI", r.id, r.prop))
        self.send(ANS, 0x6509, b"", seq=seq)

    def on_650A_1(self, seq, p):            # RoomProperty: u16 room -> u16 room, u32
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        self.send(ANS, 0x650A, struct.pack(">HI", i, r.prop if r else 0), seq=seq)

    def on_6405_1(self, seq, p):            # RoomPasswordInfo: u16 room -> u16 room, u8 (1: has a password)
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        self.send(ANS, 0x6405, struct.pack(">HB", i, 1 if r and r.pw else 0), seq=seq)

    def on_640B_1(self, seq, p):            # RoomJoinInfo: u16 room -> u16 room, 5 x u16 (meaning not known: members, 4, 0, 0, 0)
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        n = len(r.members) if r else 0
        self.send(ANS, 0x640B, struct.pack(">HHHHHH", i, n, r.max if r else 4, 0, 0, 0), seq=seq)

    # matching: every member says ready (MatchEntry 6504, u8 1 / 0), the leader starts (6508 notice); the server tells
    # the room 6910, then each client reads the match (lbc_game_ready_00 -> cnLBS_Read_MatchInfomation: 6911 member
    # count, 6912 its own player number, 6913 / 6917 per player, 6915 battle code, 6914 game rule, 6916 the game
    # server address), logs out of the lobby server (lbc_game_ready_04) and goes to the game server (Game_task step 3).
    # Here the "game server" is the room leader's own game (the PC's co-op host, docs/network.md 3.4): 6916 gives the
    # leader's address and a port of this room.
    def room_ready(self, r):
        return sum(1 for m in r.members if getattr(m, "ready", 0))

    def on_6504_1(self, seq, p):            # MatchEntry: u8 1 ready / 0 cancel
        self.ready = p[0] if p else 0
        self.send(ANS, 0x6504, b"", seq=seq)
        r = self.my_room()
        if r:
            for o in r.members:     # NoticeMatchEntryUser (6506): u16 ready, u16 members
                o.safe_send(NOTE, 0x6506, struct.pack(">HH", self.room_ready(r), len(r.members)))

    def on_6412_1(self, seq, p):            # MatchEntryUser: u16 room -> u16 room, u16 ready, u16 members
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        self.send(ANS, 0x6412, struct.pack(">HHH", i, self.room_ready(r) if r else 0, len(r.members) if r else 0), seq=seq)

    def on_6508_16(self, seq, p):           # MatchStart (the leader): to every member, notice 6910
        r = self.my_room()
        if not r:
            return
        r.match = list(r.members)
        log("match start in room %d: %s" % (r.id, ", ".join(m.user_handle for m in r.match)))
        for o in r.match:
            o.safe_send(NOTE, 0x6910, b"")

    def match_room(self):
        for lobby, rs in list(ROOMS.items()):
            for r in rs:
                if self in getattr(r, "match", []):
                    return r
        return None

    def on_6911_1(self, seq, p):            # MatchJoin: u8 number of players
        r = self.match_room()
        self.send(ANS, 0x6911, bytes([len(r.match) if r else 0]), seq=seq)

    def on_6912_1(self, seq, p):            # MatchPlSide: u8 this player's number, 1-based (the client keeps it - 1 as
        r = self.match_room()               # USER_PL_ID, the slot; the leader is 1 = slot 0, the co-op host)
        self.send(ANS, 0x6912, bytes([r.match.index(self) + 1 if r else 0]), seq=seq)

    def on_6913_1(self, seq, p):            # MatchOpponentInfo(u8 n): u8 n, u8, str id, str handle, str mini, str, u8
        r = self.match_room()
        n = p[0] if p else 1
        m = r.match[n - 1] if r and 1 <= n <= len(r.match) else self
        self.send(ANS, 0x6913, bytes([n, 0]) + str16(m.user_id) + str16(m.user_handle[:16]) + str16(m.mini) + str16("") +
                  bytes([0]), seq=seq)

    def on_6917_1(self, seq, p):            # MatchOpponentStatus(u8 n): u8 n, u16 side (1), 5 x u32 (meaning not known: 0)
        n = p[0] if p else 1
        self.send(ANS, 0x6917, bytes([n]) + struct.pack(">HIIIII", 1, 0, 0, 0, 0, 0), seq=seq)

    def on_6915_1(self, seq, p):            # MatchBattleCode: str (16 characters kept at cw+0x35E0)
        r = self.match_room()
        self.send(ANS, 0x6915, str16("ROOM%02d%010d" % (r.id if r else 0, int(time.time()) % 10 ** 10)), seq=seq)

    def on_6914_1(self, seq, p):            # MatchGameRule: str
        self.send(ANS, 0x6914, str16(""), seq=seq)

    def on_6916_1(self, seq, p):            # MatchMcsIpAddr: str 4 address bytes, str 2 port bytes (big endian)
        r = self.match_room()
        leader = r.match[0] if r else self
        ip = leader.client_address[0]
        port = COOP_PORT + (r.id if r else 0)
        self.send(ANS, 0x6916, str16(bytes(int(x) for x in ip.split("."))) + str16(struct.pack(">H", port)), seq=seq)

    def on_6918_16(self, seq, p):           # MatchRejection: the client gave up on the match
        log("%s rejected the match" % self.user_handle)

    def on_640A_1(self, seq, p):            # room member list: as the lobby's (630A)
        i = struct.unpack(">H", p[:2])[0]
        r = self.room(i)
        members = r.members if r else []
        out = struct.pack(">HBB", 0, 3, len(members))
        for m in members:
            out += str16(m.user_id) + str16(m.user_handle[:16]) + str16(m.mini)
        self.send(ANS, 0x640A, out, seq=seq)

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
        # to everyone in the lobby, the sender too: Lb_send_chat (lb_ad.c) does not log its own line when it
        # chats to the whole lobby (cw+0x32BE == 0), it only shows what comes back
        for o in REG.members(1, lobby):
            o.safe_send(NOTE, 0x6702, out)

    def on_6708_16(self, seq, p):           # chat binary: the town's game data (lb_send_data: type byte + payload)
        # client: one obfuscated string (__cnet_SendSet_ChatBinary). To every other member of the same lobby as
        # notice 6708: str sender id, str data (_cnet_RecvFromLbs_NoticeChatBinary -> CallBack_Event_LbsBinary ->
        # Lb_check_receipt(id, data)). Positions, actions, chairs, status, stage changes all travel this way.
        data, _ = self.enc_string(p, 0, seq)
        lobby = REG.where.get(self, {}).get(1)
        if VERBOSE:
            log("binary type %d (%d bytes) in lobby %s" % (data[0] if data else -1, len(data), lobby))
        if lobby is None:
            return
        out = str16(self.user_id) + str16(data)
        for o in REG.others(self, 1, lobby):
            o.safe_send(NOTE, 0x6708, out)

    def tell_target(self, ident):
        for o in REG.members(1, REG.where.get(self, {}).get(1)):
            if o.user_id.encode()[:6] == ident[:6]:
                return o
        return None

    def on_670B_1(self, seq, p):            # chat to one hunter: obfuscated id (6), obfuscated text, u8
        ident, off = self.enc_string(p, 0, seq)
        text, off = self.enc_string(p, off, seq)
        self.send(ANS, 0x670B, b"", seq=seq)
        o = self.tell_target(ident)
        if o is not None:
            o.safe_send(NOTE, 0x670C, str16(self.user_id) + str16(self.user_handle[:16]) + str16(text) + bytes([0, 0, 0, 0]))

    def on_670D_1(self, seq, p):            # chat binary to one hunter (trades, item requests, comments)
        ident, off = self.enc_string(p, 0, seq)
        data, off = self.enc_string(p, off, seq)
        self.send(ANS, 0x670D, b"", seq=seq)
        o = self.tell_target(ident)
        if o is not None:
            o.safe_send(NOTE, 0x670E, str16(self.user_id) + str16(data))

    # user search, mail, admin message (docs/network.md 5.7) ---------------------------------------------------------
    def find_user(self, ident):
        """a logged-in hunter by id (the client sends 6 characters)"""
        ident = ident.split(b"\0")[0][:6]
        with REG.lock:
            for c in list(REG.where):
                if c.user_id.encode()[:6] == ident:
                    return c
        for c in list(CLIENTS):
            if c.user_id.encode()[:6] == ident and c.user_handle:
                return c
        return None

    def on_6703_1(self, seq, p):            # SearchUser: obfuscated id -> where that hunter is
        ident, _ = self.enc_string(p, 0, seq)
        o = self.find_user(ident)
        if o is None:
            self.send(ANS, 0x6703, html_msg("そのハンターはログインしていません。"), seq=seq, res=1)
            return
        w = REG.where.get(o, {})
        # str id, u16 plaza, u16 lobby, u16 room (1-based, 0 = none), u8, u8 (not shown by the client: 0), str message
        self.send(ANS, 0x6703, str16(o.user_id) + struct.pack(">HHHBB", w.get(0) or 0, w.get(1) or 0, w.get(2) or 0, 0, 0) +
                  str16(""), seq=seq)

    SEARCH_PAGE = 10        # records per answer; the client asks for the rest with the 6709 notice ("certify")

    def search_match(self, o, conds):
        m = o.mini
        for t, v in conds:
            if t == 1 and o.user_id.encode()[:6] != v.split(b"\0")[0][:6]:
                return False
            if t == 2 and v.split(b"\0")[0] not in o.user_handle.encode("cp932", "replace"):
                return False
            if t == 3 and m[0] != v[0]:                     # weapon kind (mini +0)
                return False
            if t == 6 and not (v[0] * 4 + 1 <= m[1] <= v[1] * 4 + 4):     # rank range (mini +1), guess
                return False
        return True                 # (types 4 and 5: never sent by the game's screens, not checked)

    def on_6709_1(self, seq, p):            # ConditionSearchUser: u8 max, u8 n, n x (u8 type, value)
        mx, n = p[0], p[1]
        off, conds = 2, []
        for _ in range(n):
            t = p[off]
            off += 1
            if t in (1, 2):
                v, off = self.enc_string(p, off, seq)
            elif t == 6:
                v, off = p[off:off + 2], off + 2
            else:
                v, off = p[off:off + 1], off + 1
            conds.append((t, v))
        with REG.lock:
            users = [c for c in REG.where if c.user_handle]
        self.search = [o for o in users if self.search_match(o, conds)][:mx or 80]
        log("%s searches %s: %d found" % (self.user_handle, conds, len(self.search)))
        self.search_page(0x6709, seq, 0)

    def search_page(self, code, seq, start):
        rs = self.search[start:start + self.SEARCH_PAGE]
        last = 1 if start + len(rs) >= len(self.search) else 0
        out = bytes([len(self.search), start, len(rs), last])
        for o in rs:
            out += str16(o.user_id[:8]) + str16(o.user_handle.encode("cp932", "replace")[:16]) + str16(o.mini[:0x40])
        self.send(ANS, code, out, seq=seq)

    def on_6709_16(self, seq, p):           # the client got a page with "more to come": u8 next record; the next page
        # goes back as an answer with this notice's sequence number (the client's slot now waits on it). Code 670A (its
        # answer row has the same handler as 6709's; which code the real server used is a guess)
        self.search_page(0x670A, seq, p[0] if p else 0)

    def on_6704_1(self, seq, p):            # SendMail: obfuscated id (6), obfuscated text
        ident, off = self.enc_string(p, 0, seq)
        text, _ = self.enc_string(p, off, seq)
        ident = ident.split(b"\0")[0][:6].decode("ascii", "replace")
        o = self.find_user(ident.encode())
        mail = str16(self.user_id) + str16(self.user_handle.encode("cp932", "replace")[:16]) + str16(text[:0x7E])
        if o is not None:
            o.safe_send(NOTE, 0x6705, mail)
        elif self.mail_keep(ident, mail):   # kept until that hunter's next login (guess: what the real one did)
            pass
        else:
            self.send(ANS, 0x6704, html_msg("そのIDのハンターはいません。"), seq=seq, res=1)
            return
        log("mail from %s to %s%s" % (self.user_handle, ident, "" if o else " (kept until the next login)"))
        self.send(ANS, 0x6704, b"", seq=seq)

    # storage hooks: memory here; mh1-server (tools/server/lobby_stub.py) keeps them in its SQLite store
    def place_save(self, plaza, lobby):     # the connection ended inside a lobby
        PLACES[self.user_id] = (plaza, lobby)

    def place_take(self):                   # the place to go back to at the next login (6891), once
        return PLACES.pop(self.user_id, None)

    def hunter_seen(self):                  # login finished
        KNOWN_IDS.add(self.user_id[:6])

    def mail_keep(self, ident, payload):    # a mail for a hunter not logged in: True = kept
        if ident not in KNOWN_IDS:
            return False
        MAILBOX.setdefault(ident, []).append(payload)
        return True

    def mail_pending(self):
        return MAILBOX.pop(self.user_id[:6], [])

    def deliver_mail(self):
        for mail in self.mail_pending():
            self.safe_send(NOTE, 0x6705, mail)

    def on_6706_2(self, seq, p):            # the client closed the administrator message
        log("%s read the administrator message" % self.user_handle)

    def on_630A_1(self, seq, p):            # lobby member list: u16 ?, u8 fields per entry, u8 count, entries
        i = struct.unpack(">H", p[:2])[0]
        members = REG.members(1, i)
        out = struct.pack(">HBB", 0, 3, len(members))
        for m in members:
            out += str16(m.user_id) + str16(m.user_handle[:16]) + str16(m.mini)
        self.send(ANS, 0x630A, out, seq=seq)

    # event quests (file download, docs/network.md 5.8): asked at every lobby entry (Lbc_DownloadQuest)
    def on_6881_1(self, seq, p):            # FileDownloadHeader -> u8 n, n x u32 size (0 files: no event quest)
        self.send(ANS, 0x6881, bytes([len(EVENT_FILES)]) + b"".join(struct.pack(">I", len(f)) for f in EVENT_FILES),
                  seq=seq)

    def on_6882_1(self, seq, p):            # FileDownloadData: u8 file, u32 offset, u32 size (0x200) -> the chunk
        i, ofs, n = struct.unpack(">BII", p[:9])
        if i >= len(EVENT_FILES):
            self.send(ANS, 0x6882, html_msg("ファイルがありません。"), seq=seq, res=1)
            return
        chunk = EVENT_FILES[i][ofs:ofs + n]
        self.send(ANS, 0x6882, struct.pack(">BII", i, ofs, len(chunk)) + struct.pack(">H", len(chunk)) + chunk, seq=seq)

    # personal data (docs/network.md 5.8): the PS2 registers name, zip, address, telephone, age and mail address after
    # the portal's registration page (cnLBS_RegistPersonalData). The test server accepts and stores NOTHING (privacy:
    # docs/server.md section 9); it only logs which fields came.
    def on_6181_1(self, seq, p):            # PersonalDataChange: may the data be sent -> result 0
        self.send(ANS, 0x6181, b"", seq=seq)

    def pd_field(name):
        def h(self, seq, p):
            log("%s sent personal data field %s (%d bytes, not stored)" % (self.user_handle, name, len(p)))
        return h
    on_6182_16 = pd_field("name")
    on_6183_16 = pd_field("zip")
    on_6184_16 = pd_field("address")
    on_6185_16 = pd_field("telephone")
    on_6186_16 = pd_field("age")
    on_6187_16 = pd_field("mail address")

    def on_6188_1(self, seq, p):            # PersonalDataRegisted: done -> result 0
        self.send(ANS, 0x6188, b"", seq=seq)

    def send_patch(self, data):
        """6121 start (str name: 4-char id + 10-char version, u16, u32 byte count, u32 byte sum), 6122 data (u16 block,
        u16 n, n bytes), 6123 footer, all notices; then 6125 finish (request): the client checks the sum and goes to its
        patch step (lbc_login_patch). Only for testing the client's handling: a real patch is PS2 code encrypted with DNAS
        keys, and the PC refuses every patch."""
        log("sending a patch of %d bytes" % len(data))
        self.send(NOTE, 0x6121, str16(b"TEST0000000001") + struct.pack(">HII", 0, len(data), sum(data) & 0xFFFFFFFF))
        for k in range(0, len(data), 0x200):
            chunk = data[k:k + 0x200]
            self.send(NOTE, 0x6122, struct.pack(">HH", k // 0x200, len(chunk)) + chunk)
        self.send(NOTE, 0x6123, b"")
        self.send(REQ, 0x6125, b"")

    def on_6125_2(self, seq, p):            # the client applied the patch (a PS2 would; the PC never answers this)
        log("%s applied the patch" % (self.user_handle or "a client"))

    def on_6124_2(self, seq, p):            # patch line check answer
        pass

    def on_6001_2(self, seq, p):            # the line check's answer
        pass

    def on_6002_1(self, seq, p):            # logout
        self.send(ANS, C_LOGOUT, b"", seq=seq)


class Server(socketserver.ThreadingTCPServer):
    allow_reuse_address = True
    daemon_threads = True


def main():
    global VERBOSE, COOP_PORT, ADMIN_MESSAGE, EVENT_FILES, PATCH
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=10200)
    ap.add_argument("--allow-public", action="store_true", help="bind a non-private address")
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument("--coop-port", type=int, default=10300, help="the matched room's leader hosts on this + room number")
    ap.add_argument("--admin-message", default="", help="an administrator message (6706) every hunter gets after the login")
    ap.add_argument("--event-quest", action="append", default=[], metavar="FILE",
                    help="a downloadable event quest file (a mission file in the disc's format, quest number >= 0xC8; "
                         "it comes from your own disc: never commit or share one)")
    ap.add_argument("--patch", metavar="FILE", help="send FILE as a patch during the login (to test the client's refusal)")
    a = ap.parse_args()
    PATCH = open(a.patch, "rb").read() if a.patch else None
    EVENT_FILES = [open(f, "rb").read() for f in a.event_quest]
    ADMIN_MESSAGE = a.admin_message
    VERBOSE = a.verbose
    COOP_PORT = a.coop_port
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
