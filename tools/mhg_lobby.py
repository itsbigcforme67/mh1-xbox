"""mhg_lobby.py - Monster Hunter G's side of the lobby server (tools/mh1_testserver.py, mh1-server): the commands G
added or changed (docs/network.md "G online"). mh1_testserver's Client.dispatch asks this module first for a G client
(Client.game == "mhg", set by G's 61A0 at the login); everything not here is MH1's handler. The module is read again
when the file changes, so a running server picks up an edit without dropping a PS2 that is logged in.

Handlers are plain functions on_CODE_CAT(client, seq, payload). Formats are read from G's client (lobby.bin at
0x542600); where a value's meaning is not known it says so and sends the value that makes the client go on.
"""
import struct
import sys
import zlib

import g_info

# the running lobby module: mh1_testserver itself when it was started as a script, else the imported one (mh1-server)
_m = sys.modules.get("__main__")
ts = _m if hasattr(_m, "Client") and hasattr(_m, "REG") else __import__("mh1_testserver")
ANS, REQ, NOTE, html_msg, str16 = ts.ANS, ts.REQ, ts.NOTE, ts.html_msg, ts.str16


def log(*a):
    ts.log(*a)

G_EVENT_FILES = []      # event quest files (none yet): files 1-3 after the info file
G_NEWS = []             # news texts sent after the login (68A0 / 68A1), at most 3


def g_files():
    """G's download files: file 0 is G's online information file, read by the plaza and the guild (lobby 0x569610,
    0x56A850, 0x56AB70, Lb_guild ...); G refuses to go on without it (0 files = logout, lobby 0x581210). Files 1-3
    would be event quests (a 512-byte header each, the whole file on demand). Our file 0: tools/g_info.py."""
    return [g_info.info_file()] + list(G_EVENT_FILES)



def on_61A1_1(self, seq, p):            # G only, after 6131: no payload -> u32, u32
    # G's answer handler (lobby 0x5B0300): a non-zero first value / 60 * 30 becomes MCSLS_SWIN_LIMIT (the in-hunt
    # session's send window), a non-zero second value / 60 a float next to it (gp-0x6C18). Zero leaves both at the
    # client's defaults. The original server's values are not known.
    self.send(ANS, 0x61A1, struct.pack(">II", 0, 0), seq=seq)


def on_6883_1(self, seq, p):            # G: one file's record: u8 file -> u8 file, u32 size, u32 version
    # G keeps the 8 bytes per file with its download (mission_area+0x23F4 + 8 * file) and downloads a file again
    # only when they differ (lobby 0x581210). The version is ours: a checksum of the file.
    i = p[0]
    fs = self.files()
    f = fs[i] if i < len(fs) else b""
    self.send(ANS, 0x6883, struct.pack(">BII", i, len(f), zlib.crc32(f) & 0x7FFFFFFF), seq=seq)


# G's town (lobby) attributes, asked for each lobby with name / status / join users (lobby 0x5AC620 / 0x5AC7A0;
# kept in G's lobby records of 356 bytes, flags 0x80 / 0x10). What they mean is not known yet: 0.
def on_630D_1(self, seq, p):            # G: u16 lobby -> u16 lobby, u32
    self.send(ANS, 0x630D, p[:2] + struct.pack(">I", 0), seq=seq)


def on_630B_1(self, seq, p):            # G: u16 lobby -> u16 lobby, u8
    self.send(ANS, 0x630B, p[:2] + b"\0", seq=seq)


def on_6148_1(self, seq, p):            # G: u8 login kind -> u8: 0 = nothing wanted; 1-3 / 4 = the server asks
    # for one of the hunter's records (sub_main 0x4C8F50 / 0x4C8F90: 336 bytes from User_data for 1-3, 44 for
    # 4), which the client then sends with 6149 (u8 kind, u8 what, obfuscated record) and asks again. Rankings on
    # Capcom's side, we guess; this server keeps nothing and asks for nothing.
    self.send(ANS, 0x6148, b"\0", seq=seq)


def on_6149_1(self, seq, p):            # G: a record the server asked for (never, here): -> u8 0 (done)
    self.send(ANS, 0x6149, b"\0", seq=seq)


def on_68A0_1(self, seq, p):            # G: news after the login: -> u8 n (<= 3), 4 bytes (sent back with 68A1)
    n = len(G_NEWS)
    self.send(ANS, 0x68A0, bytes([n, 0, 0, 0, 0]), seq=seq)


def on_68A1_1(self, seq, p):            # G: u8 k, 3 bytes -> str (text of news k, up to 1280 bytes, the game's HTML)
    k = p[0]
    t = G_NEWS[k] if k < len(G_NEWS) else ""
    self.send(ANS, 0x68A1, html_msg(t), seq=seq)
