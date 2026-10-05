#!/usr/bin/env python3
"""pc_abs.py - absolute PS2 addresses in game C, for the PC build.

Some decompiled files still read main's work areas by address, e.g.
`*(u8 *)0x3F34C1` (game_w.master). For the PC build (tools/build_pc.sh) a
copy of such a file is compiled in which each `(T *)0xADDR` that falls
inside a sized main symbol (config/symbols/main.txt) reads the host's
symbol instead: `(T *)(rt_abs_<sym> + off)`, with
`extern unsigned char rt_abs_<sym>[] __asm__("<sym>");` declared at the
top (a byte view under another C name, so it never clashes with the
file's own typed declaration). Addresses outside main symbols are left
alone.

    python3 tools/pc_abs.py in.c out.c     # exit status 1: nothing to rewrite

Standard library only.
"""
import bisect
import re
import sys

LINE = re.compile(r"^(\w+) = 0x([0-9A-Fa-f]+);(?:\s*//\s*(?:type:(\w+))?\s*(?:size:0x([0-9A-Fa-f]+))?)?")
CAST = re.compile(r"\(\s*(\w+(?:\s*\*)+)\s*\)\s*0x([0-9A-Fa-f]{6,8})\b")


def main():
    src = open(sys.argv[1]).read()
    syms = []
    for line in open("config/symbols/main.txt"):
        m = LINE.match(line)
        if m and m.group(3) != "func" and m.group(4):
            va, size = int(m.group(2), 16), int(m.group(4), 16)
            if 0x100000 <= va < 0x533980 and size:
                syms.append((va, size, m.group(1)))
    syms.sort()
    starts = [s[0] for s in syms]
    used = {}

    def fix(m):
        a = int(m.group(2), 16)
        i = bisect.bisect_right(starts, a) - 1
        while i >= 0:   # the nearest symbol below a that contains it
            va, size, name = syms[i]
            if va <= a < va + size:
                used[name] = 1
                return "(%s)(rt_abs_%s + 0x%X)" % (m.group(1), name, a - va)
            if a - va > 0x10000:
                break
            i -= 1
        return m.group(0)

    out = CAST.sub(fix, src)
    if not used:
        sys.exit(1)
    head = "".join('extern unsigned char rt_abs_%s[] __asm__("%s");\n' % (n, n) for n in sorted(used))
    open(sys.argv[2], "w").write(head + out)


if __name__ == "__main__":
    main()
