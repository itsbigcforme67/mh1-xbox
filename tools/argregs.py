#!/usr/bin/env python3
"""argregs.py - which argument registers each function really reads.

m2c drafts drop arguments a caller leaves in a register unchanged ("a0 left
over"), and K&R declarations hide it; on the PC port such a call reads
garbage. This tool computes, from the original code, the argument registers
each function reads before writing them (a0-a3, t0-t3 = args 1-8, f12-f19
= float args), following calls (a jal reads what its callee reads), and can
check the calls in a C file against it.

Disassemble first (binutils from tools/binutils):
    OD=tools/binutils/bin/mips64r5900el-ps2-elf-objdump
    $OD -D -b binary -m mips:5900 -EL --adjust-vma=0x100000 disc/mh1/split/main.bin > build/main.dis
    $OD -D -b binary -m mips:5900 -EL --adjust-vma=0x533980 disc/mh1/split/lobby.bin > build/lobby.dis
Then:
    python3 tools/argregs.py lobby build/main.dis build/lobby.dis FUNC...      # print arg registers
    python3 tools/argregs.py lobby build/main.dis build/lobby.dis --check FILE.c...
--check lists every call in FILE.c whose argument count is lower than the
callee reads (or higher, marked '+'), with the callee's registers.
The analysis is a plain liveness pass; jalr (function pointers) and jr to
other code count as reading nothing. Standard library only.
"""
import bisect
import re
import sys

INT_ARGS = ["a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3"]
FLT_ARGS = ["$f12", "$f13", "$f14", "$f15", "$f16", "$f17", "$f18", "$f19"]
ARGS = set(INT_ARGS) | set(FLT_ARGS)
CLOBBER = set(INT_ARGS) | {"v0", "v1", "t4", "t5", "t6", "t7", "t8", "t9", "at", "ra"} | \
    {"$f%d" % i for i in range(20)}
STORES = {"sb", "sh", "sw", "sd", "sq", "swc1", "sdc1", "swl", "swr", "sdl", "sdr", "sqc2"}
LINE = re.compile(r"^\s*([0-9a-f]+):\s+[0-9a-f]{8}\s+(\S+)\s*(.*)$")
SYM = re.compile(r"^(\w+) = 0x([0-9A-Fa-f]+);(?:\s*//\s*(?:type:(\w+))?\s*(?:size:0x([0-9A-Fa-f]+))?)?")


def load_syms(paths):
    funcs = {}
    for p in paths:
        for l in open(p):
            m = SYM.match(l)
            if m and m.group(3) == "func":
                funcs.setdefault(int(m.group(2), 16), (m.group(1), int(m.group(4) or "0", 16)))
    return funcs


def regs_of(ops):
    out = []
    for tok in re.split(r"[,()\s]+", ops):
        if not tok:
            continue
        if re.match(r"^(\$f\d+|zero|at|v[01]|a[0-3]|t\d|s[0-8]|k[01]|gp|sp|fp|ra)$", tok):
            out.append(tok)
    return out


REG = r"(\$f\d+|zero|at|v[01]|a[0-3]|t\d|s[0-8]|k[01]|gp|sp|fp|ra)"


def decode(ins, ops):
    """(reads, writes, kind, target) of one instruction"""
    parts = [x.strip() for x in ops.split(",")] if ops else []
    base = []
    regs = []
    for x in parts:
        m = re.match(r"^-?\w*\(" + REG + r"\)$", x)
        if m:
            base.append(m.group(1))
        elif re.match("^" + REG + "$", x):
            regs.append(x)
    if base:            # loads / stores: the base is read; a load writes its first register
        if ins in STORES or not regs:
            return regs + base, [], "", None
        return regs[1:] + base, regs[:1], "", None
    r = regs_of(ops)
    tgt = None
    m = re.search(r"\b0x([0-9a-f]+)$", ops)
    if m:
        tgt = int(m.group(1), 16)
    if ins == "jal":
        return [], [], "call", tgt
    if ins == "jalr":
        return r[-1:], [], "callr", None
    if ins == "jr":
        return r, [], "ret", None
    if ins in ("j", "b"):
        return [], [], "jump", tgt
    if ins.startswith("b") and ins not in ("break",) and tgt is not None:
        return r, [], "branch", tgt
    if ins in STORES:
        return r, [], "", None
    if ins in ("qmtc2", "ctc2"):
        return r, [], "", None
    if ins in ("qmfc2", "cfc2"):
        return [], r[:1], "", None
    if ins in ("mtc1", "dmtc1", "ctc1", "mthi", "mtlo", "mtsa", "mult", "multu", "div", "divu", "dmult",
               "ddiv", "ddivu", "madd", "maddu", "mult1", "multu1", "div1", "divu1", "madd1", "maddu1"):
        if ins in ("mtc1", "dmtc1", "ctc1", "qmtc2", "ctc2"):
            return r[:1], r[1:], "", None
        if ins in ("mult", "multu", "madd", "maddu", "mult1", "multu1", "madd1", "maddu1") and len(r) == 3:
            return r[1:], r[:1], "", None
        return r, [], "", None
    if ins.startswith("c.") or ins in ("syscall", "nop", "sync", "sync.p", "break", "cache", "ei", "di", "eret"):
        return r, [], "", None
    if ins in ("mfc1", "dmfc1", "cfc1"):
        return r[1:], r[:1], "", None
    if not r:
        return [], [], "", None
    return r[1:], r[:1], "", None


def load_dis(path, funcs, code):
    starts = sorted(funcs)
    for l in open(path):
        m = LINE.match(l)
        if not m:
            continue
        a = int(m.group(1), 16)
        i = bisect.bisect_right(starts, a) - 1
        if i < 0:
            continue
        f = starts[i]
        size = funcs[f][1]
        if size and a >= f + size:
            continue
        code.setdefault(f, []).append((a, m.group(2), m.group(3)))


def analyse(funcs, code):
    need = {f: set() for f in code}
    changed = True
    rounds = 0
    while changed and rounds < 30:
        changed = False
        rounds += 1
        for f, ins in code.items():
            live = liveness(ins, need)
            if live != need[f]:
                need[f] = live
                changed = True
    return need


def liveness(ins, need):
    """registers live at entry: a branch/jump/call and its delay slot form
    one node (branch operands are read first, then the slot runs; a call
    runs after its slot)"""
    n = len(ins)
    idx = {a: k for k, (a, _, _) in enumerate(ins)}
    dec = [decode(i, o) for _, i, o in ins]
    nodes = []          # (gen, kill_order) evaluated by fn, successors
    node_of = {}
    k = 0
    while k < n:
        reads, writes, kind, tgt = dec[k]
        node_of[k] = len(nodes)
        if kind in ("branch", "jump", "ret", "call", "callr") and k + 1 < n:
            pass
        if kind in ("branch", "jump", "ret", "call", "callr") and k + 1 < n:
            slot = dec[k + 1]
            node_of[k + 1] = len(nodes)
            succ = []
            if kind == "branch":
                succ = [k + 2] + ([idx[tgt]] if tgt in idx else [])
            elif kind == "jump":
                succ = [idx[tgt]] if tgt in idx else []
            elif kind in ("call", "callr"):
                succ = [k + 2]
            nodes.append((k, kind, set(reads), set(writes), set(slot[0]), set(slot[1]), tgt, succ))
            k += 2
        else:
            nodes.append((k, "", set(reads), set(writes), set(), set(), None, [k + 1]))
            k += 1
    live = [set() for _ in nodes]
    changed = True
    while changed:
        changed = False
        for ni in range(len(nodes) - 1, -1, -1):
            k, kind, rd, wr, srd, swr, tgt, succ = nodes[ni]
            out = set()
            for s in succ:
                if s < n:
                    out |= live[node_of[s]]
            if kind in ("call", "callr"):
                g = set(rd) | (need.get(tgt, set()) if kind == "call" else set())
                x = (out - CLOBBER) | g
                new = (x - swr) | srd
            elif kind == "jump" and tgt not in idx:      # tail call
                new = ((need.get(tgt, set()) - swr) | srd) | rd
            elif kind in ("branch", "jump", "ret"):
                new = ((out - swr) | srd) | rd
            else:
                new = (out - wr) | rd
            if new != live[ni]:
                live[ni] = new
                changed = True
    return (live[0] & ARGS) if nodes else set()


def fmt(s):
    ints = [r for r in INT_ARGS if r in s]
    flts = [r for r in FLT_ARGS if r in s]
    return "%d int (%s) %d float (%s)" % (len(ints) and INT_ARGS.index(ints[-1]) + 1, " ".join(ints),
                                          len(flts) and FLT_ARGS.index(flts[-1]) + 1, " ".join(flts))


def count_args(s):
    s = s.strip()
    if not s or s == "void":
        return 0
    d = 0
    n = 1
    for ch in s:
        if ch in "([{":
            d += 1
        elif ch in ")]}":
            d -= 1
        elif ch == "," and d == 0:
            n += 1
    return n


def main():
    module, dis_main, dis_ovl = sys.argv[1], sys.argv[2], sys.argv[3]
    rest = sys.argv[4:]
    funcs_main = load_syms(["config/symbols/main.txt"])
    funcs_ovl = load_syms(["config/symbols/%s.txt" % module])
    funcs = dict(funcs_main)
    funcs.update({a: v for a, v in funcs_ovl.items() if a >= 0x533980})
    code = {}
    load_dis(dis_main, {a: v for a, v in funcs.items() if a < 0x533980}, code)
    load_dis(dis_ovl, {a: v for a, v in funcs.items() if a >= 0x533980}, code)
    need = analyse(funcs, code)
    byname = {}
    for a, (n, _) in funcs.items():
        if a in need:
            byname[n] = need[a]
    if rest and rest[0] == "--check":
        for path in rest[1:]:
            check_file(path, byname)
        return
    for n in rest:
        print("%-28s %s" % (n, fmt(byname[n]) if n in byname else "?"))


def check_file(path, byname):
    if True:
        src = open(path).read()
        defs = set(re.findall(r"^[A-Za-z_][\w \*]*?\b(\w+)\([^;]*\)\s*\{", src, re.M))
        seen = set()
        for m in re.finditer(r"\b(\w+)\(", src):
            name = m.group(1)
            if name not in byname:
                continue
            line_start = src.rfind("\n", 0, m.start()) + 1
            line = src[line_start:src.find("\n", m.start())]
            if re.match(r"^[A-Za-z_][\w \*]*\b%s\(" % name, line):
                continue     # a definition or declaration
            d, j = 0, m.end() - 1
            for j in range(m.end() - 1, len(src)):
                if src[j] == "(":
                    d += 1
                elif src[j] == ")":
                    d -= 1
                    if d == 0:
                        break
            got = count_args(src[m.end():j])
            s = byname[name]
            ints = [r for r in INT_ARGS if r in s]
            flts = [r for r in FLT_ARGS if r in s]
            want = (INT_ARGS.index(ints[-1]) + 1 if ints else 0) + len(flts)
            if got != want:
                lno = src.count("\n", 0, m.start()) + 1
                key = (lno, name)
                if key in seen:
                    continue
                seen.add(key)
                print("%s:%d: %s%s(%d args) callee reads %s" % (path, lno, "+ " if got > want else "",
                                                              name, got, fmt(s)))


if __name__ == "__main__":
    main()
