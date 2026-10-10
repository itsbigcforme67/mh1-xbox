#!/usr/bin/env python3
"""mk_event_quest.py - turn a mission file from your own disc into a downloadable "event quest" for the test servers
(docs/network.md 5.8). Event quests are quest numbers >= 0xC8: the lobby downloads the file into mission_area at every
lobby entry (6881 / 6882), the guild counter offers it, Quest_start uses it as it is. The quest number lives in the
mission's info record (its offset is the file's first u32, little endian) at +0x1D.
Get a mission file with the PC build's test aid: RT_MISSION_DUMP=out.bin build/pc/mhview disc/mh1 --quest N --time 1
The result is Capcom data from your disc: keep it in build/ (gitignored), never commit or share it.
usage: mk_event_quest.py MISSION.bin OUT.bin [NUMBER (default 0xC8)]"""
import struct
import sys

if len(sys.argv) < 3:
    sys.exit(__doc__)
d = bytearray(open(sys.argv[1], "rb").read())
no = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0xC8
if not 0xC8 <= no <= 0xFF:
    sys.exit("event quest numbers are 0xC8..0xFF")
info = struct.unpack_from("<I", d, 0)[0]
if not 0 < info < len(d) - 0x20:
    sys.exit("%s does not look like a mission file (info record offset %#x)" % (sys.argv[1], info))
print("quest %d -> event quest %d (%#x)" % (d[info + 0x1D], no, no))
d[info + 0x1D] = no
open(sys.argv[2], "wb").write(bytes(d))
