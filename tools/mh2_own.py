#!/usr/bin/env python3
"""mh2_own.py - check Monster Hunter 2 C written by hand (src/mh2/MODULE/*.c) against the original bytes.

Each file holds functions named like plsel_func_00D61710, or by a name in the map. Compiles it with the MH2 compiler
setup and compares every instruction with the disc's module (build/mh2/split), relocated
fields masked. Prints a side-by-side on a difference with -v. The full build is the final word.

Usage: python3 tools/mh2_own.py [-v] src/mh2/plsel/p00D61710.c ...
"""
import csv
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mh2_carry as C  # noqa: E402
from mips_dis import dis  # noqa: E402


def main():
    args = sys.argv[1:]
    v = "-v" in args
    files = [a for a in args if a != "-v"]
    size = {}
    name = {}
    for r in csv.DictReader(open(C.P("docs/survey/mh2_map.csv"), encoding="utf-8")):
        size[(r["mh2_module"], int(r["mh2_addr"], 16))] = int(r["size"])
        name.setdefault(r["name"], int(r["mh2_addr"], 16))
    bad = 0
    os.makedirs(C.P("build/mh2/own"), exist_ok=True)
    for f in files:
        rel = os.path.relpath(os.path.abspath(f), C.ROOT)
        mod = rel.split("/")[2]
        obj = "build/mh2/own/%s.o" % os.path.basename(f)[:-2]
        p = subprocess.run([C.WIBO, C.P("tools/compilers", C.CC, "mwccps2.exe")] + C.OPT + C.BASE + [rel, "-o", obj],
                           cwd=C.ROOT, capture_output=True, text=True)
        if p.returncode:
            print(p.stdout + p.stderr)
            bad += 1
            continue
        ob = C.Obj(C.P(obj))
        load, image = C.image(mod)[:2]
        for n, sh, val, sz in ob.funcs:
            m = re.search(r"([0-9A-F]{8})$", n)
            addr = int(m.group(1), 16) if m else name[n]
            code = ob.d[ob.S[sh][4] + val:ob.S[sh][4] + val + sz]
            o = image[addr - load:addr - load + size.get((mod, addr), sz)]
            mk = {r[0] - val: C.MASK.get(r[1], 0) for r in ob.rel[sh] if val <= r[0] < val + sz}
            nd = abs(len(code) - len(o)) // 4
            lines = []
            for i in range(0, max(len(code), len(o)), 4):
                a = struct.unpack_from("<I", code, i)[0] if i + 4 <= len(code) else None
                b = struct.unpack_from("<I", o, i)[0] if i + 4 <= len(o) else None
                m = mk.get(i, 0)
                same = a is not None and b is not None and (a & ~m) == (b & ~m)
                nd += not same and a is not None and b is not None
                lines.append("%s %-32s | %s" % ("  " if same else ">>", dis(b, addr + i) if b is not None else "",
                                                dis(a, addr + i) if a is not None else ""))
            print("%s %-28s %d/%d bytes  %d differ" % ("OK" if not nd else "--", n, len(code), len(o), nd))
            bad += bool(nd)
            if v and nd:
                print("\n".join("     " + l for l in lines))
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
