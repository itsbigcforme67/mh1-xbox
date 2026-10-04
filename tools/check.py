#!/usr/bin/env python3
"""
check.py - compile a C file with the project compiler and compare every
function in it against the original bytes, in any module.

Relocated fields are masked using the object's own relocation entries
(R_MIPS_26 / HI16 / LO16 / GPREL16 / 32), so a function counts as matching
when every instruction is identical apart from addresses the linker fills
in. The full build (tools/build.py) is the final word; this is the fast
inner loop.

Usage:
    python3 tools/check.py src/main/pl/pl_master_ck.c
    python3 tools/check.py src/game/em15.c -v        # show diffs
    python3 tools/check.py file.c --add main pl/foo  # on full match, append
                                                     # its range to c_files.txt
"""
import argparse
import csv
import os
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mips_dis import dis  # noqa: E402

WIBO = os.path.join(ROOT, "tools/compilers/wibo")
MWCC = os.path.join(ROOT, "tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe")
CFLAGS = ["-c", "-O4,p", "-nostdinc", "-stderr", "-Iinclude"]
SECTIONS = {"main": "main", "select": "select.bin", "game": "game.bin",
            "yn": "yn.bin", "lobby": "lobby.bin"}

R_MIPS_32, R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16 = 2, 4, 5, 6, 7
MASKS = {R_MIPS_26: 0x03FFFFFF, R_MIPS_HI16: 0xFFFF, R_MIPS_LO16: 0xFFFF,
         R_MIPS_GPREL16: 0xFFFF, R_MIPS_32: 0xFFFFFFFF}


def module_image(module):
    """(vram, bytes) of a module as built by setup_split.py."""
    path = os.path.join(ROOT, "disc/mh1/split/%s.bin" % module)
    data = open(path, "rb").read()
    if module == "main":
        return 0x100000, data
    return struct.unpack_from("<I", data, 8)[0], data


def original_functions():
    """name -> list of (module, addr, size); names repeat across files."""
    out = {}
    inv = {v: k for k, v in SECTIONS.items()}
    for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"),
                                 encoding="utf-8")):
        if r["type"] == "FUNC" and r["section"] in inv:
            out.setdefault(r["name"], []).append(
                (inv[r["section"]], int(r["addr"], 16), int(r["size"])))
    return out


def read_obj(path):
    """Functions in a relocatable .o: name -> (bytes, {offset: mask},
    {offset: call target name}). Call targets come from R_MIPS_26 relocs,
    resolved through section symbols when the callee is in the same object."""
    d = open(path, "rb").read()
    shoff, = struct.unpack_from("<I", d, 32)
    shnum, shstr = struct.unpack_from("<HH", d, 48)
    S = [struct.unpack_from("<10I", d, shoff + i * 40) for i in range(shnum)]

    def nm(o, t):
        return t[o:t.index(b"\0", o)].decode()
    sy = next(s for s in S if s[1] == 2)
    st = S[sy[6]]
    strs = d[st[4]:st[4] + st[5]]
    syms = []
    for k in range(sy[5] // 16):
        no, val, size, info, _o, sh = struct.unpack_from("<IIIBBH", d, sy[4] + k * 16)
        syms.append((nm(no, strs), val, size, info & 0xF, sh))
    by_sec = {}
    for name, val, size, typ, sh in syms:
        if typ == 2:
            by_sec.setdefault(sh, []).append((val, name))
    relmask, calls = {}, {}
    for i, s in enumerate(S):
        if s[1] == 9:                           # SHT_REL
            for k in range(s[5] // 8):
                off, info = struct.unpack_from("<II", d, s[4] + k * 8)
                m = MASKS.get(info & 0xFF)
                if m:
                    relmask[(s[7], off)] = m
                if info & 0xFF == R_MIPS_26:
                    name, val, size, typ, sh = syms[info >> 8]
                    if typ == 3:                # section symbol + addend in insn
                        sec = S[s[7]]
                        word = struct.unpack_from("<I", d, sec[4] + off)[0]
                        tgt = (word & 0x3FFFFFF) << 2
                        name = next((n for v, n in by_sec.get(sh, []) if v == tgt), "?")
                    calls[(s[7], off)] = name
    funcs = {}
    for name, val, size, typ, sh in syms:
        if typ != 2 or sh >= len(S):
            continue
        sec = S[sh]
        code = d[sec[4] + val:sec[4] + val + size]
        masks = {o - val: m for (si, o), m in relmask.items()
                 if si == sh and val <= o < val + size}
        fcalls = {o - val: n for (si, o), n in calls.items()
                  if si == sh and val <= o < val + size}
        funcs[name] = (code, masks, fcalls)
    return funcs


def func_names_by_addr():
    out = {}
    inv = {v: k for k, v in SECTIONS.items()}
    for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"),
                                 encoding="utf-8")):
        if r["type"] == "FUNC" and r["section"] in inv:
            out.setdefault((inv[r["section"]], int(r["addr"], 16)), set()).add(r["name"])
    return out


def compare(mine, masks, orig, base, calls=None, names=None, module=None):
    lines, ok = [], len(mine) == len(orig)
    for i in range(0, max(len(mine), len(orig)), 4):
        a = struct.unpack_from("<I", orig, i)[0] if i + 4 <= len(orig) else None
        b = struct.unpack_from("<I", mine, i)[0] if i + 4 <= len(mine) else None
        m = masks.get(i, 0)
        same = a is not None and b is not None and (a & ~m) == (b & ~m)
        note = ""
        if same and calls and i in calls and names is not None:
            tgt = ((base + i + 4) & 0xF0000000) | ((a & 0x3FFFFFF) << 2)
            want = names.get((module, tgt), set()) | names.get(("main", tgt), set())
            if calls[i] not in want:
                same = False
                note = "   (calls %s, original calls %s)" % (calls[i], "/".join(sorted(want)) or "?")
        ok = ok and same
        ta = (dis(a, base + i) or ".word") if a is not None else ""
        tb = (dis(b, base + i) or ".word") if b is not None else ""
        if m:
            tb += "   (reloc)"
        tb += note
        lines.append("%s %08X  %-34s | %s" % ("  " if same else ">>", base + i, ta, tb))
    return ok, lines


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cfile")
    ap.add_argument("-v", action="store_true", help="print diffs for non-matching functions")
    ap.add_argument("--add", nargs=2, metavar=("MODULE", "NAME"),
                    help="if all functions match and are contiguous, add to c_files.txt")
    ap.add_argument("--module", help="prefer this module when a name is ambiguous")
    ap.add_argument("--cc", help="another compiler dir under tools/compilers "
                    "(e.g. bundle/mwcps2-3.0b22-020926)")
    ap.add_argument("--flags", help="replace -O4,p (e.g. '-O3')")
    args = ap.parse_args()

    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build") if
                                     os.path.isdir(os.path.join(ROOT, "build")) else None) as tmp:
        obj = os.path.relpath(os.path.join(tmp, "check.o"), ROOT)
        src = os.path.relpath(os.path.abspath(args.cfile), ROOT)
        cc = os.path.join(ROOT, "tools/compilers", args.cc, "mwccps2.exe") if args.cc else MWCC
        flags = CFLAGS if not args.flags else [f for f in CFLAGS if not f.startswith("-O")] + args.flags.split()
        p = subprocess.run([WIBO, cc] + flags + [src, "-o", obj], cwd=ROOT,
                           capture_output=True, text=True)
        if p.returncode:
            sys.exit(p.stdout + p.stderr)
        funcs = read_obj(os.path.join(ROOT, obj))

    orig = original_functions()
    images = {}
    results, all_ok = [], True
    names = func_names_by_addr()
    for name, (code, masks, calls) in funcs.items():
        cands = orig.get(name, [])
        if args.module:
            cands = [c for c in cands if c[0] == args.module] or cands
        if not cands:
            print("??  %-32s not in the symbol table" % name)
            all_ok = False
            continue
        best = None
        for mod, addr, size in cands:
            if mod not in images:
                images[mod] = module_image(mod)
            base, img = images[mod]
            o = img[addr - base: addr - base + size]
            ok, lines = compare(code, masks, o, addr, calls, names, mod)
            if best is None or ok:
                best = (ok, lines, mod, addr, size)
            if ok:
                break
        ok, lines, mod, addr, size = best
        all_ok &= ok
        results.append((mod, addr, size, name, ok))
        print("%s  %-32s %-6s 0x%08X %5d bytes%s" % (
            "OK" if ok else "--", name, mod, addr, size,
            "" if ok else "  (%d/%d instructions differ)" % (
                sum(1 for l in lines if l.startswith(">>")), len(lines))))
        if not ok and args.v:
            print("\n".join("      " + l for l in lines))

    if args.add and all_ok and results:
        mod = args.add[0]
        rs = sorted(r for r in results if r[0] == mod)
        lo, hi = rs[0][1], rs[-1][1] + rs[-1][2]
        covered = sum(r[2] for r in rs)
        orig_in = sorted({(a, s) for v in orig.values() for (m, a, s) in v
                          if m == mod and lo <= a < hi})
        if sum(s for _a, s in orig_in) != covered:
            sys.exit("not adding: the C file does not cover every function in "
                     "0x%08X-0x%08X" % (lo, hi))
        with open(os.path.join(ROOT, "config/c_files.txt"), "a") as f:
            f.write("%s 0x%08X 0x%08X %s\n" % (mod, lo, hi, args.add[1]))
        print("added %s 0x%08X-0x%08X %s to config/c_files.txt" % (mod, lo, hi, args.add[1]))
    sys.exit(0 if all_ok else 1)


if __name__ == "__main__":
    main()
