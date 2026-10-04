#!/usr/bin/env python3
"""
mips_dis.py - minimal disassembler for PS2 EE (R5900) code, for reading
small functions while identifying the compiler. Standard library only.

Covers the common integer, branch, load/store and COP1 (float) instructions.
Anything it does not know is printed as ".word 0x........" - it never guesses.
It is a reading aid, not the Phase 1 disassembler (that will be splat or
similar).

Usage:
    python3 mips_dis.py SLPM_654.95 --sym symbols.csv FUNC_NAME [FUNC_NAME ...]
    python3 mips_dis.py SLPM_654.95 --addr 0x00100000 --size 64
    python3 mips_dis.py some.o --obj FUNC_NAME   # function in a compiled .o
"""
import argparse
import csv
import struct
import sys

R = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
     "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
     "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
     "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]

OPS = {  # opcode -> (mnemonic, form)
    0x02: ("j", "J"), 0x03: ("jal", "J"),
    0x04: ("beq", "B2"), 0x05: ("bne", "B2"), 0x06: ("blez", "B1"),
    0x07: ("bgtz", "B1"), 0x14: ("beql", "B2"), 0x15: ("bnel", "B2"),
    0x16: ("blezl", "B1"), 0x17: ("bgtzl", "B1"),
    0x08: ("addi", "I"), 0x09: ("addiu", "I"), 0x0A: ("slti", "I"),
    0x0B: ("sltiu", "I"), 0x0C: ("andi", "IU"), 0x0D: ("ori", "IU"),
    0x0E: ("xori", "IU"), 0x0F: ("lui", "LUI"), 0x18: ("daddi", "I"),
    0x19: ("daddiu", "I"),
    0x20: ("lb", "M"), 0x21: ("lh", "M"), 0x23: ("lw", "M"), 0x24: ("lbu", "M"),
    0x25: ("lhu", "M"), 0x27: ("lwu", "M"), 0x37: ("ld", "M"), 0x1E: ("lq", "M"),
    0x28: ("sb", "M"), 0x29: ("sh", "M"), 0x2B: ("sw", "M"), 0x3F: ("sd", "M"),
    0x1F: ("sq", "M"), 0x31: ("lwc1", "MF"), 0x39: ("swc1", "MF"),
    0x22: ("lwl", "M"), 0x26: ("lwr", "M"), 0x2A: ("swl", "M"), 0x2E: ("swr", "M"),
    0x1A: ("ldl", "M"), 0x1B: ("ldr", "M"), 0x2C: ("sdl", "M"), 0x2D: ("sdr", "M"),
    0x2F: ("cache", "M"), 0x33: ("pref", "M"),
}
SPECIAL = {
    0x00: "sll", 0x02: "srl", 0x03: "sra", 0x04: "sllv", 0x06: "srlv",
    0x07: "srav", 0x08: "jr", 0x09: "jalr", 0x0A: "movz", 0x0B: "movn",
    0x0C: "syscall", 0x0D: "break", 0x0F: "sync", 0x10: "mfhi", 0x11: "mthi",
    0x12: "mflo", 0x13: "mtlo", 0x14: "dsllv", 0x16: "dsrlv", 0x17: "dsrav",
    0x18: "mult", 0x19: "multu", 0x1A: "div", 0x1B: "divu",
    0x20: "add", 0x21: "addu", 0x22: "sub", 0x23: "subu", 0x24: "and",
    0x25: "or", 0x26: "xor", 0x27: "nor", 0x2A: "slt", 0x2B: "sltu",
    0x2C: "dadd", 0x2D: "daddu", 0x2E: "dsub", 0x2F: "dsubu",
    0x38: "dsll", 0x3A: "dsrl", 0x3B: "dsra", 0x3C: "dsll32", 0x3E: "dsrl32",
    0x3F: "dsra32",
}
REGIMM = {0x00: "bltz", 0x01: "bgez", 0x02: "bltzl", 0x03: "bgezl",
          0x10: "bltzal", 0x11: "bgezal"}
COP1_S = {0x00: "add.s", 0x01: "sub.s", 0x02: "mul.s", 0x03: "div.s",
          0x04: "sqrt.s", 0x05: "abs.s", 0x06: "mov.s", 0x07: "neg.s",
          0x16: "rsqrt.s", 0x18: "adda.s", 0x1A: "mula.s", 0x1C: "madd.s",
          0x1D: "msub.s", 0x24: "cvt.w.s", 0x28: "max.s", 0x29: "min.s",
          0x30: "c.f.s", 0x32: "c.eq.s", 0x34: "c.lt.s", 0x36: "c.le.s"}


def s16(v):
    return v - 0x10000 if v & 0x8000 else v


def dis(word, pc):
    op = word >> 26
    rs, rt, rd = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
    sa, fn, imm = (word >> 6) & 31, word & 63, word & 0xFFFF
    if word == 0:
        return "nop"
    if op == 0:
        m = SPECIAL.get(fn)
        if m is None:
            return None
        if fn in (0x00, 0x02, 0x03, 0x38, 0x3A, 0x3B, 0x3C, 0x3E, 0x3F):
            return "%s %s, %s, %d" % (m, R[rd], R[rt], sa)
        if fn in (0x04, 0x06, 0x07, 0x14, 0x16, 0x17):
            return "%s %s, %s, %s" % (m, R[rd], R[rt], R[rs])
        if fn == 0x08:
            return "jr %s" % R[rs]
        if fn == 0x09:
            return "jalr %s, %s" % (R[rd], R[rs])
        if fn in (0x0C, 0x0D, 0x0F):
            return m
        if fn in (0x10, 0x12):
            return "%s %s" % (m, R[rd])
        if fn in (0x11, 0x13):
            return "%s %s" % (m, R[rs])
        if fn in (0x18, 0x19, 0x1A, 0x1B):
            return "%s %s, %s" % (m, R[rs], R[rt]) if rd == 0 else \
                "%s %s, %s, %s" % (m, R[rd], R[rs], R[rt])
        return "%s %s, %s, %s" % (m, R[rd], R[rs], R[rt])
    if op == 1:
        m = REGIMM.get(rt)
        return m and "%s %s, 0x%08X" % (m, R[rs], pc + 4 + s16(imm) * 4)
    if op == 0x11:                                   # COP1
        if rs == 0x00:
            return "mfc1 %s, $f%d" % (R[rt], rd)
        if rs == 0x04:
            return "mtc1 %s, $f%d" % (R[rt], rd)
        if rs == 0x08:
            names = {0: "bc1f", 1: "bc1t", 2: "bc1fl", 3: "bc1tl"}
            return "%s 0x%08X" % (names.get(rt & 3), pc + 4 + s16(imm) * 4)
        if rs == 0x10:
            m = COP1_S.get(fn)
            if m is None:
                return None
            ft, fs, fd = rt, rd, sa
            if m.startswith("c."):
                return "%s $f%d, $f%d" % (m, fs, ft)
            if m in ("sqrt.s", "abs.s", "mov.s", "neg.s", "cvt.w.s"):
                return "%s $f%d, $f%d" % (m, fd, fs)
            return "%s $f%d, $f%d, $f%d" % (m, fd, fs, ft)
        if rs == 0x14 and fn == 0x20:
            return "cvt.s.w $f%d, $f%d" % (sa, rd)
        return None
    if op == 0x1C and fn in (0x18, 0x19):            # R5900 mult/multu 3-op
        return "%s %s, %s, %s" % ("mult" if fn == 0x18 else "multu", R[rd], R[rs], R[rt])
    ent = OPS.get(op)
    if ent is None:
        return None
    m, form = ent
    if form == "J":
        return "%s 0x%08X" % (m, ((pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2))
    if form == "B2":
        return "%s %s, %s, 0x%08X" % (m, R[rs], R[rt], pc + 4 + s16(imm) * 4)
    if form == "B1":
        return "%s %s, 0x%08X" % (m, R[rs], pc + 4 + s16(imm) * 4)
    if form == "I":
        return "%s %s, %s, %d" % (m, R[rt], R[rs], s16(imm))
    if form == "IU":
        return "%s %s, %s, 0x%X" % (m, R[rt], R[rs], imm)
    if form == "LUI":
        return "lui %s, 0x%X" % (R[rt], imm)
    if form == "M":
        return "%s %s, %d(%s)" % (m, R[rt], s16(imm), R[rs])
    if form == "MF":
        return "%s $f%d, %d(%s)" % (m, rt, s16(imm), R[rs])
    return None


def listing(code, base, names=None):
    out = []
    for i in range(0, len(code) - 3, 4):
        word = struct.unpack_from("<I", code, i)[0]
        pc = base + i
        text = dis(word, pc) or ".word 0x%08X" % word
        if names and pc in names:
            text += "    ; " + names[pc]
        out.append("  %08X: %08X  %s" % (pc, word, text))
    return "\n".join(out)


def elf_sections(data):
    shoff, = struct.unpack_from("<I", data, 32)
    shnum, shstrndx = struct.unpack_from("<HH", data, 48)
    secs = [struct.unpack_from("<10I", data, shoff + i * 40) for i in range(shnum)]
    st = secs[shstrndx]
    strtab = data[st[4]:st[4] + st[5]]
    named = []
    for s in secs:
        end = strtab.find(b"\0", s[0])
        named.append((strtab[s[0]:end].decode(), s))
    return named


def read_vaddr(data, addr, size):
    for name, s in elf_sections(data):
        sh_addr, sh_off, sh_size = s[3], s[4], s[5]
        if sh_size and sh_addr <= addr and addr + size <= sh_addr + sh_size \
                and s[1] != 8:
            return data[sh_off + addr - sh_addr: sh_off + addr - sh_addr + size]
    return None


def obj_function(data, func):
    """Bytes of a named function in a relocatable .o (text section)."""
    secs = elf_sections(data)
    symtab = next(s for n, s in secs if s[1] == 2)
    strsec = secs[symtab[6]][1]
    strs = data[strsec[4]:strsec[4] + strsec[5]]
    for i in range(symtab[5] // 16):
        no, val, size, info, _o, shndx = struct.unpack_from("<IIIBBH", data, symtab[4] + i * 16)
        name = strs[no:strs.find(b"\0", no)].decode()
        if name == func and info & 0xF == 2:
            sec = secs[shndx][1]
            return data[sec[4] + val: sec[4] + val + size]
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("elf")
    ap.add_argument("funcs", nargs="*")
    ap.add_argument("--sym", help="symbol CSV from elf_survey.py")
    ap.add_argument("--addr", type=lambda v: int(v, 0))
    ap.add_argument("--size", type=lambda v: int(v, 0), default=64)
    ap.add_argument("--obj", action="store_true", help="input is a relocatable .o")
    args = ap.parse_intermixed_args()
    data = open(args.elf, "rb").read()

    if args.obj:
        for f in args.funcs:
            code = obj_function(data, f)
            print("%s:" % f)
            print(listing(code, 0) if code else "  not found")
        return
    if args.addr is not None:
        print(listing(read_vaddr(data, args.addr, args.size), args.addr))
        return
    rows = list(csv.DictReader(open(args.sym)))
    for f in args.funcs:
        for r in rows:
            if r["name"] == f and r["type"] == "FUNC":
                a, n = int(r["addr"], 16), int(r["size"])
                code = read_vaddr(data, a, n)
                print("%s @ 0x%08X (%d bytes, %s):" % (f, a, n, r["section"]))
                print(listing(code, a) if code else "  bytes not in this file "
                      "(overlay? extract it with mwo_unpack.py)")


if __name__ == "__main__":
    sys.exit(main())
