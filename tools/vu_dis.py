#!/usr/bin/env python3
"""
vu_dis.py - list and disassemble the VU1 microcode programs in MH1's main.

The fl library keeps its VU1 programs inside main as DMA "ret" packets
(symbols Vu1Code_XXXX_YYYY in the ELF symbol table, 0x293B80-0x2E5E00): a
DMA tag, then VIF codes (NOP, MPG ...) carrying the 64-bit VU instructions.
This tool parses those packets and prints PS2 VU assembly. Standard library
only; reads the user's own split main.bin. Never commit its output.

Usage:
    python3 tools/vu_dis.py --list
    python3 tools/vu_dis.py Vu1Code_0001_0002            # one program
    python3 tools/vu_dis.py --all -o build/vu1           # every program, one .vsm each
"""
import argparse
import csv
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAIN = os.path.join(ROOT, "disc/mh1/split/main.bin")
SYMS = os.path.join(ROOT, "docs/survey/mh1_symbols.csv")
BASE = 0x100000

BC = "xyzw"


def dest(w):
    d = (w >> 21) & 0xF
    return "".join(c for c, b in zip("xyzw", (8, 4, 2, 1)) if d & b)


def upper(w):
    ft, fs, fd = (w >> 16) & 31, (w >> 11) & 31, (w >> 6) & 31
    op = w & 0x3F
    dd = dest(w)
    flags = "".join(n for n, b in (("[I]", 31), ("[E]", 30), ("[M]", 29), ("[D]", 28), ("[T]", 27))
                    if w >> b & 1)
    if op < 0x1C:
        names = ["add", "sub", "madd", "msub", "max", "mini", "mul"]
        s = "%s%s.%s vf%02d, vf%02d, vf%02d%s" % (names[op >> 2], BC[op & 3], dd, fd, fs, ft, BC[op & 3])
    elif op < 0x30:
        t = {0x1C: ("mulq", "q"), 0x1D: ("maxi", "i"), 0x1E: ("muli", "i"), 0x1F: ("minii", "i"),
             0x20: ("addq", "q"), 0x21: ("maddq", "q"), 0x22: ("addi", "i"), 0x23: ("maddi", "i"),
             0x24: ("subq", "q"), 0x25: ("msubq", "q"), 0x26: ("subi", "i"), 0x27: ("msubi", "i")}
        if op in t:
            n, r = t[op]
            s = "%s.%s vf%02d, vf%02d, %s" % (n, dd, fd, fs, r)
        else:
            n = {0x28: "add", 0x29: "madd", 0x2A: "mul", 0x2B: "max", 0x2C: "sub", 0x2D: "msub",
                 0x2E: "opmsub", 0x2F: "mini"}[op]
            s = "%s.%s vf%02d, vf%02d, vf%02d" % (n, dd, fd, fs, ft)
    elif op >= 0x3C:
        e = (fd << 2) | (op & 3)
        if e < 0x10:
            n = ["adda", "suba", "madda", "msuba"][e >> 2]
            s = "%s%s.%s ACC, vf%02d, vf%02d%s" % (n, BC[e & 3], dd, fs, ft, BC[e & 3])
        elif e < 0x18:
            n = ["itof0", "itof4", "itof12", "itof15", "ftoi0", "ftoi4", "ftoi12", "ftoi15"][e - 0x10]
            s = "%s.%s vf%02d, vf%02d" % (n, dd, ft, fs)
        elif e < 0x1C:
            s = "mula%s.%s ACC, vf%02d, vf%02d%s" % (BC[e & 3], dd, fs, ft, BC[e & 3])
        elif e == 0x1D:
            s = "abs.%s vf%02d, vf%02d" % (dd, ft, fs)
        elif e == 0x1F:
            s = "clipw.xyz vf%02d, vf%02d" % (fs, ft)
        elif e == 0x2F:
            s = "nop"
        else:
            t = {0x1C: ("mulaq", "q"), 0x1E: ("mulai", "i"), 0x20: ("addaq", "q"), 0x21: ("maddaq", "q"),
                 0x22: ("addai", "i"), 0x23: ("maddai", "i"), 0x24: ("subaq", "q"), 0x25: ("msubaq", "q"),
                 0x26: ("subai", "i"), 0x27: ("msubai", "i")}
            if e in t:
                n, r = t[e]
                s = "%s.%s ACC, vf%02d, %s" % (n, dd, fs, r)
            else:
                n = {0x28: "adda", 0x29: "madda", 0x2A: "mula", 0x2C: "suba", 0x2D: "msuba",
                     0x2E: "opmula"}.get(e)
                s = ("%s.%s ACC, vf%02d, vf%02d" % (n, dd, fs, ft)) if n else "upper?? %08X" % w
    else:
        s = "upper?? %08X" % w
    return s + flags


def sext(v, bits):
    return v - (1 << bits) if v >> (bits - 1) & 1 else v


def lower(w, pc, labels):
    op = w >> 25
    ft, fs, fd = (w >> 16) & 31, (w >> 11) & 31, (w >> 6) & 31
    dd = dest(w)
    imm11 = sext(w & 0x7FF, 11)
    imm15 = ((w >> 10) & 0x7800) | (w & 0x7FF)

    def tgt():
        a = pc + 8 + imm11 * 8
        return labels.get(a, "0x%04X" % a)
    if op == 0x00:
        return "lq.%s vf%02d, %d(vi%02d)" % (dd, ft, imm11, fs)
    if op == 0x01:
        return "sq.%s vf%02d, %d(vi%02d)" % (dd, fs, imm11, ft)
    if op == 0x04:
        return "ilw.%s vi%02d, %d(vi%02d)" % (dd, ft, imm11, fs)
    if op == 0x05:
        return "isw.%s vi%02d, %d(vi%02d)" % (dd, ft, imm11, fs)
    if op == 0x08:
        return "iaddiu vi%02d, vi%02d, 0x%X" % (ft, fs, imm15)
    if op == 0x09:
        return "isubiu vi%02d, vi%02d, 0x%X" % (ft, fs, imm15)
    if 0x10 <= op <= 0x1C:
        n = {0x10: "fceq", 0x11: "fcset", 0x12: "fcand", 0x13: "fcor", 0x14: "fseq", 0x15: "fsset",
             0x16: "fsand", 0x17: "fsor", 0x18: "fmeq", 0x1A: "fmand", 0x1B: "fmor", 0x1C: "fcget"}.get(op)
        if op in (0x10, 0x11, 0x12, 0x13):
            return "%s vi01, 0x%06X" % (n, w & 0xFFFFFF) if op != 0x11 else "fcset 0x%06X" % (w & 0xFFFFFF)
        if op in (0x14, 0x15, 0x16, 0x17):
            return "%s vi%02d, 0x%X" % (n, ft, ((w >> 10) & 0x800) | (w & 0x7FF))
        if op in (0x18, 0x1A, 0x1B):
            return "%s vi%02d, vi%02d" % (n, ft, fs)
        if op == 0x1C:
            return "fcget vi%02d" % ft
    if op == 0x20:
        return "b %s" % tgt()
    if op == 0x21:
        return "bal vi%02d, %s" % (ft, tgt())
    if op == 0x24:
        return "jr vi%02d" % fs
    if op == 0x25:
        return "jalr vi%02d, vi%02d" % (ft, fs)
    if op in (0x28, 0x29):
        return "%s vi%02d, vi%02d, %s" % ("ibeq" if op == 0x28 else "ibne", ft, fs, tgt())
    if op in (0x2C, 0x2D, 0x2E, 0x2F):
        return "%s vi%02d, %s" % ({0x2C: "ibltz", 0x2D: "ibgtz", 0x2E: "iblez", 0x2F: "ibgez"}[op], fs, tgt())
    if op == 0x40:
        f = w & 0x3F
        if f == 0x30:
            return "iadd vi%02d, vi%02d, vi%02d" % (fd, fs, ft)
        if f == 0x31:
            return "isub vi%02d, vi%02d, vi%02d" % (fd, fs, ft)
        if f == 0x32:
            return "iaddi vi%02d, vi%02d, %d" % (ft, fs, sext(fd, 5))
        if f == 0x34:
            return "iand vi%02d, vi%02d, vi%02d" % (fd, fs, ft)
        if f == 0x35:
            return "ior vi%02d, vi%02d, vi%02d" % (fd, fs, ft)
        if f >= 0x3C:
            e = (fd << 2) | (f & 3)
            fsf, ftf = BC[(w >> 21) & 3], BC[(w >> 23) & 3]
            t = {0x30: "move.%s vf%02d, vf%02d" % (dd, ft, fs),
                 0x31: "mr32.%s vf%02d, vf%02d" % (dd, ft, fs),
                 0x34: "lqi.%s vf%02d, (vi%02d++)" % (dd, ft, fs),
                 0x35: "sqi.%s vf%02d, (vi%02d++)" % (dd, fs, ft),
                 0x36: "lqd.%s vf%02d, (--vi%02d)" % (dd, ft, fs),
                 0x37: "sqd.%s vf%02d, (--vi%02d)" % (dd, fs, ft),
                 0x38: "div Q, vf%02d%s, vf%02d%s" % (fs, fsf, ft, ftf),
                 0x39: "sqrt Q, vf%02d%s" % (ft, ftf),
                 0x3A: "rsqrt Q, vf%02d%s, vf%02d%s" % (fs, fsf, ft, ftf),
                 0x3B: "waitq",
                 0x3C: "mtir vi%02d, vf%02d%s" % (ft, fs, fsf),
                 0x3D: "mfir.%s vf%02d, vi%02d" % (dd, ft, fs),
                 0x3E: "ilwr.%s vi%02d, (vi%02d)" % (dd, ft, fs),
                 0x3F: "iswr.%s vi%02d, (vi%02d)" % (dd, ft, fs),
                 0x40: "rnext.%s vf%02d, R" % (dd, ft),
                 0x41: "rget.%s vf%02d, R" % (dd, ft),
                 0x42: "rinit R, vf%02d%s" % (fs, fsf),
                 0x43: "rxor R, vf%02d%s" % (fs, fsf),
                 0x64: "mfp.%s vf%02d, P" % (dd, ft),
                 0x68: "xtop vi%02d" % ft,
                 0x69: "xitop vi%02d" % ft,
                 0x6C: "xgkick vi%02d" % fs,
                 0x70: "esadd P, vf%02d" % fs, 0x71: "ersadd P, vf%02d" % fs,
                 0x72: "eleng P, vf%02d" % fs, 0x73: "erleng P, vf%02d" % fs,
                 0x74: "eatanxy P, vf%02d" % fs, 0x75: "eatanxz P, vf%02d" % fs,
                 0x76: "esum P, vf%02d" % fs,
                 0x78: "esqrt P, vf%02d%s" % (fs, fsf), 0x79: "ersqrt P, vf%02d%s" % (fs, fsf),
                 0x7A: "ercpr P, vf%02d%s" % (fs, fsf), 0x7B: "waitp",
                 0x7C: "esin P, vf%02d%s" % (fs, fsf), 0x7D: "eatan P, vf%02d%s" % (fs, fsf),
                 0x7E: "eexp P, vf%02d%s" % (fs, fsf)}
            if e == 0x30 and not dd and ft == 0 and fs == 0:
                return "nop"
            return t.get(e, "lower?? %08X" % w)
    return "lower?? %08X" % w


def load_symbols():
    progs, labels = {}, {}
    for r in csv.DictReader(open(SYMS, encoding="utf-8")):
        n, a = r["name"], int(r["addr"], 16)
        if not a or r["section"] != "main":
            continue
        if re.fullmatch(r"Vu1Code_[0-9a-fA-F]{4}_[0-9a-fA-F]{4}", n) and r["bind"] == "GLOBAL":
            progs[n] = a
        m = re.fullmatch(r"_\$(Vu1Code_[0-9a-fA-F]{4}_[0-9a-fA-F]{4})_(\w+)", n)
        if m:
            labels.setdefault(m.group(1), {})[m.group(2)] = a
    return progs, labels


def parse_packet(d, addr):
    """DMA tag + VIF stream -> list of (vu_address, [64-bit words])."""
    off = addr - BASE
    tag = struct.unpack_from("<Q", d, off)[0]
    qwc, tid = tag & 0xFFFF, (tag >> 28) & 7
    end = off + 16 + qwc * 16
    p = off + 8                              # VIF codes start in the tag's upper half
    out = []
    while p < end:
        code = struct.unpack_from("<I", d, p)[0]
        cmd = (code >> 24) & 0x7F
        p += 4
        if cmd == 0x00:                      # NOP
            continue
        if cmd == 0x4A:                      # MPG num, loadaddr
            num = (code >> 16) & 0xFF or 256
            vaddr = (code & 0xFFFF) * 8
            if p % 8:
                p += 8 - p % 8
            words = [struct.unpack_from("<Q", d, p + 8 * k)[0] for k in range(num)]
            out.append((vaddr, words, p + BASE))
            p += num * 8
            continue
        if cmd & 0x60 == 0x60:               # UNPACK vn, vl
            vn, vl = (cmd >> 2) & 3, cmd & 3
            num = (code >> 16) & 0xFF or 256
            size = (num * (32 >> vl) * (vn + 1) // 8 + 3) & ~3
            out.append((None, "VIF UNPACK V%d-%d %d qwords -> VU1 mem 0x%03X (constants)"
                        % (vn + 1, 32 >> vl, num, code & 0x3FF), 0))
            p += size
            continue
        if cmd in (0x50, 0x51):              # DIRECT / DIRECTHL: GIF data
            n = (code & 0xFFFF) or 0x10000
            if p % 16:
                p += 16 - p % 16
            out.append((None, "VIF DIRECT %d qwords of GIF data at main 0x%X (GS setup)" % (n, p + BASE), 0))
            p += n * 16
            continue
        names = {0x01: "STCYCL", 0x02: "OFFSET", 0x03: "BASE", 0x04: "ITOP", 0x05: "STMOD",
                 0x06: "MSKPATH3", 0x07: "MARK", 0x10: "FLUSHE", 0x11: "FLUSH", 0x13: "FLUSHA",
                 0x14: "MSCAL", 0x15: "MSCALF", 0x17: "MSCNT"}
        if cmd in names:
            out.append((None, "VIF %s 0x%04X" % (names[cmd], code & 0xFFFF), 0))
            continue
        out.append((None, "VIF code %08X (cmd 0x%02X) at 0x%X, stopping" % (code, cmd, p - 4 + BASE), 0))
        break
    return tid, qwc, out


def disasm(name, addr, labels, d):
    lab = labels.get(name, {})
    tid, qwc, blocks = parse_packet(d, addr)
    lines = ["; %s at main 0x%06X: DMA tag id %d, %d qwords" % (name, addr, tid, qwc)]
    # label symbols are main addresses of instruction words
    by_main = {a: n for n, a in lab.items()}
    vu_labels = {}
    for vaddr, words, maddr in blocks:
        if vaddr is not None:
            for k in range(len(words)):
                if maddr + 8 * k in by_main:
                    vu_labels[vaddr + 8 * k] = by_main[maddr + 8 * k]
    for vaddr, words, maddr in blocks:
        if vaddr is None:
            lines.append("; " + words)
            continue
        lines.append("; MPG %d instructions -> VU1 0x%04X" % (len(words), vaddr))
        for k, w in enumerate(words):
            pc = vaddr + 8 * k
            lo, up = w & 0xFFFFFFFF, w >> 32
            if pc in vu_labels:
                lines.append("%s:" % vu_labels[pc])
            us = upper(up)
            ls = ("loi 0x%08X" % lo) if up >> 31 & 1 else lower(lo, pc, vu_labels)
            lines.append("  %04X  %-40s %s" % (pc, us, ls))
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("prog", nargs="?")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("-o", "--out", default=os.path.join(ROOT, "build/vu1"))
    args = ap.parse_args()
    progs, labels = load_symbols()
    d = open(MAIN, "rb").read()
    if args.list:
        for n, a in sorted(progs.items(), key=lambda x: x[1]):
            tid, qwc, blocks = parse_packet(d, a)
            ninst = sum(len(b[1]) for b in blocks if b[0] is not None)
            print("%-20s 0x%06X  %4d instructions  labels: %s"
                  % (n, a, ninst, " ".join(k for k in labels.get(n, {}) if k not in ("start", "end", "PROG"))))
        return
    if args.all:
        os.makedirs(args.out, exist_ok=True)
        for n, a in progs.items():
            open(os.path.join(args.out, n + ".vsm"), "w").write(disasm(n, a, labels, d))
        print("wrote %d programs to %s" % (len(progs), args.out))
        return
    if args.prog:
        sys.stdout.write(disasm(args.prog, progs[args.prog], labels, d))


if __name__ == "__main__":
    main()
