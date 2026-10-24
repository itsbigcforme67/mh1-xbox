#!/usr/bin/env python3
"""g_info.py - Monster Hunter G's online information file ("file 0" of the login's file download), made by our server.

G downloads it at every login (6881 / 6883 / 6882, lobby 0x581210) into mission_area + 0x2414 and refuses to go on
without it. Its layout is read from the G client (docs/network.md "G online"); nothing here comes from Capcom's file,
which we never had. Field meanings marked "guess" are not confirmed.

    python3 tools/g_info.py out.bin      writes the file (for a look with a hex viewer)
"""
import struct
import sys

SIZE = 0x9C8            # 2504: G puts the 68A1 news texts right after it (mission_area + 0x2414 + 2504)

# Layout (offsets in the file; "+0x..." is where the G client reads it):
#   0x000  16 records of 32 bytes: the town types of the plaza's town list. +8: the name (Shift-JIS, drawn by
#          lobby 0x56AB70, and sent to the server when the player picks it, lbc_in_plaza_03 0x57F8F0 -> 6314);
#          +0..7 not read by anything we found.
#   0x208  16 bytes: each town type's text colour in that list (font palette; 10 = the grey of a closed entry)
#   0x22E  5 x 16 bytes: per plaza class (1-5), per town type: non-zero = open (0 = grey, cannot be picked)
#   0x27E  5 x 2 signed bytes: per plaza class, the lowest and the highest value of the hunter's mini data byte 1 that
#          may enter (lobby 0x56AF20; else message 13). [guess] byte 1 is the hunter rank.
#   0x288  5 x 256 bytes: per plaza class, the help text under the class menu (lobby 0x57DDC0)
#   0x788  256 bytes, 0x888 256 bytes: two more texts of the plaza menu (lobby 0x569610: the first when the plaza's
#          class is '1' (ClassInfo -> PlazaInfo+91), else the second) [what the menu entry is: not seen yet]
#   0x988  64 bytes: not read by anything we found
CLASSES = [     # (help text, lowest, highest)
    ("どなたでも入れる街です。", 0, 127),
    ("どなたでも入れる街です。（２）", 0, 127),
    ("どなたでも入れる街です。（３）", 0, 127),
    ("どなたでも入れる街です。（４）", 0, 127),
    ("どなたでも入れる街です。（５）", 0, 127),
]
TOWN_TYPES = ["街%s" % "１２３４５６７８９"[i] if i < 9 else "街１%s" % "０１２３４５６"[i - 9] for i in range(16)]


def put(b, off, data, size):
    data = data[:size - 1]
    b[off:off + len(data)] = data


def info_file():
    b = bytearray(SIZE)
    for t, name in enumerate(TOWN_TYPES):
        put(b, t * 32 + 8, name.encode("cp932"), 24)
        b[0x208 + t] = 0
    for k, (text, lo, hi) in enumerate(CLASSES):
        for t in range(16):
            b[0x22E + 16 * k + t] = 1
        b[0x27E + 2 * k] = lo & 0xFF
        b[0x27E + 2 * k + 1] = hi & 0xFF
        put(b, 0x288 + 256 * k, text.encode("cp932"), 256)
    put(b, 0x788, "ここはテスト用の街です。".encode("cp932"), 256)
    put(b, 0x888, "ここはテスト用の街です。".encode("cp932"), 256)
    return bytes(b)


if __name__ == "__main__":
    open(sys.argv[1], "wb").write(info_file())
