#!/usr/bin/env python3
"""mh2_diff.py - side-by-side of one C file against its MH2 twin, for near-miss work (docs/mh2.md section 6).

Compiles src/PATH.c with MH2's flags (always fresh, so header edits count) and compares each function with the MH2
function the name map gives it, relocated fields masked. Left column is MH2, right column is ours. The output goes to the
terminal only: never commit it.

  mh2_diff.py [-g] [-q] [-a MODULE:ADDR] [-o NAME,NAME] PATH ...   PATH relative to src/, no .c; -g: VERSION_MHG on; -q: summary only
"""
import difflib
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mh2_carry as C  # noqa: E402
from mips_dis import dis  # noqa: E402


def build(path, gform):
    obj = "build/mh2/diff_%s.o" % re.sub(r"\W", "_", path)
    p = subprocess.run([C.WIBO, C.P("tools/compilers", C.CC, "mwccps2.exe")] + C.OPT + C.BASE +
                       (["-DVERSION_MHG"] if gform else []) + ["src/%s.c" % path, "-o", obj],
                       cwd=C.ROOT, capture_output=True, text=True, timeout=120)
    if p.returncode:
        print((p.stdout + p.stderr)[:3000])
        return None
    return C.Obj(C.P(obj))


_rev = {}


def rev(m):
    if m not in _rev:
        _rev[m] = {a: n for n, a in C.visible(m).items() if not n.startswith("D_")}
    return _rev[m]


def refs(ob, sh, at, m):
    """symbol -> {(addend, address the original instruction holds)} for one text section"""
    load, data = C.image(m)[:2]
    out = {}
    hi = {}
    raw = ob.S[sh][4]
    for off, t, y in sorted(ob.rel[sh]):
        w = struct.unpack_from("<I", data, at + off - load)[0]
        v = struct.unpack_from("<I", ob.d, raw + off)[0]
        if t == C.HI:
            hi[y] = (w & 0xFFFF, v & 0xFFFF)
        elif t == C.LO and y in hi:
            h, ah = hi[y]
            add = (ah << 16) + C.sext(v & 0xFFFF)
            out.setdefault(ob.syms[y][0], set()).add((add & 0xFFFFFFFF, ((h << 16) + C.sext(w & 0xFFFF)) & 0xFFFFFFFF))
    return {k: v for k, v in out.items() if len(set(b - a for a, b in v)) > 1}


def one(ob, name, sh, val, sz, site, quiet):
    m, a = site
    load, data = C.image(m)[:2]
    mine = list(struct.unpack_from("<%dI" % (sz // 4), ob.d, ob.S[sh][4] + val))
    n = C.func_sizes().get((m, a), sz) // 4 or sz // 4
    theirs = list(struct.unpack_from("<%dI" % n, data, a - load))
    rel = {o - val: (t, y) for o, t, y in ob.rel[sh] if val <= o < val + sz}
    a1 = [x & ~C.MASK.get(rel.get(i * 4, (0,))[0], 0) & 0xFFFFFFFF for i, x in enumerate(mine)]
    b1 = [x & ~C.MASK.get(rel.get(i * 4, (0,))[0], 0) & 0xFFFFFFFF if i * 4 in rel else x for i, x in enumerate(theirs)]
    sm = difflib.SequenceMatcher(None, b1, a1, False)
    # compare masked: words at a site that has a relocation on our side only count their fixed bits
    ops = sm.get_opcodes()
    nd = max(len(a1), len(b1)) - sum(b.size for b in sm.get_matching_blocks())
    print("%s %s %s:0x%08X  ours %d theirs %d  %d differ" % ("OK" if nd == 0 and len(a1) == len(b1) else "--", name, m, a,
                                                             len(a1), len(b1), nd))
    if nd == 0 and len(a1) == len(b1):
        r = C.resolve(ob, m, {sh: a}, set())
        if isinstance(r, str):
            print("   resolve: %s" % r)
            for n, ts in refs(ob, sh, a, m).items():
                print("     %s: %s" % (n, ", ".join("+0x%X at 0x%08X" % t for t in sorted(ts))))
    if quiet or (nd == 0 and len(a1) == len(b1)):
        return nd

    tab = rev(m)

    def left(i):
        if i is None:
            return " " * 56
        t = dis(theirs[i], a + i * 4)
        r = re.search(r"jal 0x([0-9A-F]{8})", t)
        return "%08X %-47s" % (a + i * 4, (t[:4] + tab.get(int(r[1], 16), r[1])) if r else t)

    def right(j):
        if j is None:
            return ""
        s = dis(mine[j], val + j * 4)
        r = rel.get(j * 4)
        return "%-34s %s" % (s, ob.syms[r[1]][0] if r else "")
    last = -1
    for tag, i1, i2, j1, j2 in ops:
        if tag == "equal":
            for k in range(i2 - i1):
                # context, and any relocated line (the name is what the map should agree with)
                near = k < 2 or k >= i2 - i1 - 2
                if near:
                    print("     " + left(i1 + k) + " | " + right(j1 + k))
                elif k == 2:
                    print("     ...")
            continue
        for k in range(max(i2 - i1, j2 - j1)):
            print(">>   " + left(i1 + k if i1 + k < i2 else None) + " | " + right(j1 + k if j1 + k < j2 else None))
    return nd


def main():
    args = sys.argv[1:]
    g = "-g" in args
    q = "-q" in args
    force = {}
    if "-a" in args:
        i = args.index("-a")
        for spec in args[i + 1].split(","):
            mod, ad = spec.split(":")
            force[mod] = int(ad, 16)
        del args[i:i + 2]
    only = None
    if "-o" in args:                  # only these functions, comma separated
        i = args.index("-o")
        only = args[i + 1].split(",")
        del args[i:i + 2]
    hint = C.hints()
    for path in [x for x in args if x not in ("-g", "-q")]:
        ob = build(path, g)
        if ob is None:
            continue
        print("==", path)
        tot = 0
        funcs = sorted(ob.funcs, key=lambda f: (ob.layout.get(f[1], 0), f[2]))
        for name, sh, val, sz in funcs:
            if only and name not in only:
                continue
            if force:
                m = list(force)[0]
                tot += one(ob, name, sh, val, sz, (m, force[m] + ob.layout[sh] + val), q)
                continue
            tw = hint.get(re.sub(r"_[0-9A-F]{6,8}$", "", name)) or hint.get(name)
            if not tw:
                print("?? %s no twin (%d instructions)" % (name, sz // 4))
                continue
            tot += one(ob, name, sh, val, sz, tw[0], q)
        print("== total differ", tot)


if __name__ == "__main__":
    main()
