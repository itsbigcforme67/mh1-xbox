#!/usr/bin/env python3
"""pc_abs.py - absolute PS2 addresses in game C, for the PC build.

Some decompiled files still read main's work areas by address, e.g.
`*(u8 *)0x3F34C1` (game_w.master). For the PC build (tools/build_pc.sh) a
copy of such a file is compiled in which each `(T *)0xADDR` that falls
inside a sized main symbol (config/symbols/main.txt) reads the host's
symbol instead: `(T *)(rt_abs_<sym> + off)`, with
`extern unsigned char rt_abs_<sym>[] __asm__("<sym>");` declared at the
top (a byte view under another C name, so it never clashes with the
file's own typed declaration). An address in a lobby.bin data symbol
(config/symbols/lobby.txt, e.g. lb_sys 0x6EAE50 from main's eft26.c) reads
rt_lb_mem, the host copy of the whole overlay (src/pc/rt/rt_data.c).
Other addresses are left alone.

    python3 tools/pc_abs.py in.c out.c     # exit status 1: nothing to rewrite

Standard library only.
"""
import bisect
import os
import re
import sys

LINE = re.compile(r"^(\w+) = 0x([0-9A-Fa-f]+);(?:\s*//\s*(?:type:(\w+))?\s*(?:size:0x([0-9A-Fa-f]+))?)?")
CAST = re.compile(r"\(\s*(\w+(?:\s*\*)+)\s*\)\s*0x([0-9A-Fa-f]{6,8})\b")
# MH2V(mh1, mh2): the PC build is never MH2, so the MH1 address stands (done first, or CAST would not see it)
MH2V_ADDR = re.compile(r"\bMH2V\(\s*(0x[0-9A-Fa-f]{6,8})\s*,\s*0x[0-9A-Fa-f]+\s*\)")


def main_mhg(src):
    """VERSION=mhg: the literal is an MH1 address. config/mhg/pc_addr.txt (tools/mhg_addrmap.py --emit) gives its G
    address (the nearest earlier entry's delta for neighbours); the G symbol below it (config/mhg/symbols) is the host view."""
    tr = {}
    for fn in ("config/mhg/pc_addr.txt", "config/mhg/pc_addr_hand.txt"):
        for l in open(fn):
            f = l.split()
            if len(f) >= 2 and not l.startswith("#"):
                tr[int(f[0], 16)] = int(f[1], 16)
    tk = sorted(tr)
    syms = []
    for fn in ("main", "lobby"):
        for line in open("config/mhg/symbols/%s.txt" % fn):
            m = re.match(r"^(\w+) = 0x([0-9A-Fa-f]+);(?:\s*//\s*(?:type:(\w+))?)?", line)
            if m and m.group(3) != "func":
                va = int(m.group(2), 16)
                if (fn == "main" and 0x100000 <= va < 0x4A0C80) or (fn == "lobby" and va >= 0x542600):
                    syms.append((va, m.group(1), fn))
    syms.sort()
    starts = [x[0] for x in syms]
    used = {}

    def fix(m):
        a = int(m.group(2), 16)
        g = tr.get(a)
        if g is None:
            i = bisect.bisect_right(tk, a) - 1
            if i < 0 or a - tk[i] > 0x40:
                return m.group(0)
            g = tr[tk[i]] + (a - tk[i])
        i = bisect.bisect_right(starts, g) - 1
        if i < 0 or g - syms[i][0] > 0x2000:
            return m.group(0)
        va, name, fn = syms[i]
        if fn == "lobby":
            used["rt_lb_mem"] = 1
            return "(%s)(rt_lb_mem + 0x%X)" % (m.group(1), g - 0x542600)
        used[name] = 1
        return "(%s)(rt_abs_%s + 0x%X)" % (m.group(1), name, g - va)

    return CAST.sub(fix, src), used


def main():
    src = MH2V_ADDR.sub(r"\1", open(sys.argv[1]).read())
    if os.environ.get("VERSION") == "mhg":
        out, used = main_mhg(src)
        if not used:
            sys.exit(1)
        head = ('#define RTAB_S_(x) #x\n#define RTAB_S(x) RTAB_S_(x)\n'
                '#define RTAB_P RTAB_S(__USER_LABEL_PREFIX__)\n')
        head += "".join('extern unsigned char rt_abs_%s[] __asm__(RTAB_P "%s");\n' % (n, n) for n in sorted(used) if n != "rt_lb_mem")
        if "rt_lb_mem" in used:
            head += "extern unsigned char rt_lb_mem[];\n"
        open(sys.argv[2], "w").write(head + out)
        return
    syms = []
    for line in open("config/symbols/main.txt"):
        m = LINE.match(line)
        if m and m.group(3) != "func" and m.group(4):
            va, size = int(m.group(2), 16), int(m.group(4), 16)
            if 0x100000 <= va < 0x533980 and size:
                syms.append((va, size, m.group(1)))
    for line in open("config/symbols/lobby.txt"):
        m = LINE.match(line)
        if m and m.group(3) != "func" and m.group(4):
            va, size = int(m.group(2), 16), int(m.group(4), 16)
            if va >= 0x533980 and size:
                syms.append((va, size, "@lb"))
    syms.sort()
    starts = [s[0] for s in syms]
    used = {}

    def fix(m):
        a = int(m.group(2), 16)
        i = bisect.bisect_right(starts, a) - 1
        while i >= 0:   # the nearest symbol below a that contains it
            va, size, name = syms[i]
            if va <= a < va + size:
                if name == "@lb":
                    used["rt_lb_mem"] = 1
                    return "(%s)(rt_lb_mem + 0x%X)" % (m.group(1), a - 0x533980)
                used[name] = 1
                return "(%s)(rt_abs_%s + 0x%X)" % (m.group(1), name, a - va)
            if a - va > 0x10000:
                break
            i -= 1
        return m.group(0)

    out = CAST.sub(fix, src)
    if not used:
        sys.exit(1)
    # asm labels carry the target's symbol prefix ("_" on win32 / the Xbox)
    head = ('#define RTAB_S_(x) #x\n#define RTAB_S(x) RTAB_S_(x)\n'
            '#define RTAB_P RTAB_S(__USER_LABEL_PREFIX__)\n')
    head += "".join('extern unsigned char rt_abs_%s[] __asm__(RTAB_P "%s");\n' % (n, n) for n in sorted(used) if n != "rt_lb_mem")
    if "rt_lb_mem" in used:
        head += "extern unsigned char rt_lb_mem[];\n"
    open(sys.argv[2], "w").write(head + out)


if __name__ == "__main__":
    main()
