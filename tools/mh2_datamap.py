#!/usr/bin/env python3
"""mh2_datamap.py - carry MH1's and G's DATA names to MH2 through the code of named function pairs (docs/mh2_port.md).

tools/mh2_map.py names 7,285 functions but only 659 data objects: it reads operands of identical and shape twins and keeps a
name only when every vote agrees. The PC port needs far more data names (src/pc/rt/tables.txt alone has about 1,070). This tool
reads every pair in docs/survey/mh2_map.csv (edited ones too, score >= 0.75), aligns reference and MH2 instructions by opcode
(difflib), and at each aligned global access takes "MH1/G symbol + offset here = MH2 address there". A name is kept when its top
address has at least 2 votes and 2/3 of them, or it is the only address voted for (one vote from an edited pair is the weakest case).
Run on names mh2_map.py already placed, this is a check of the method (printed).

An access is found by a linear scan: `lui r` then a load, store or addiu with base r (the reference also: base gp). So a register
reused across a branch can mislead one vote; the vote rule is there for that.

Writes config/mh2/data_map.txt: `module name 0xADDR votes total` (names and addresses only).
Usage: python3 tools/mh2_datamap.py
"""
import bisect
import collections
import csv
import difflib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mhg_map import P, token, load_mh1, load_g, read_split_names, MH1_GP  # noqa: E402
from mh2_map import load_mh2  # noqa: E402

G_GP = 0x2F2B70
LOADS = set(range(0x20, 0x28)) | {0x28, 0x29, 0x2B, 0x31, 0x39, 0x37, 0x3F, 0x1E, 0x1F, 0x2C, 0x2D}


def simm(x):
    return ((x & 0xFFFF) ^ 0x8000) - 0x8000


def accesses(words, base, gp):
    """{instruction index: absolute target} of lui+lo (and gp) accesses in one function"""
    hi, out = {}, {}
    for i, w in enumerate(words):
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        if op == 0xF:
            hi[rt] = (w & 0xFFFF) << 16
            continue
        if op == 0x09 or op in LOADS:           # addiu or a load/store
            if gp is not None and rs == 28:
                out[i] = (gp + simm(w)) & 0xFFFFFFFF
            elif rs in hi and (rs != 0):
                out[i] = (hi[rs] + simm(w)) & 0xFFFFFFFF
            if op == 0x09 or (op in LOADS and op < 0x28):
                hi.pop(rt, None)
        elif op == 0xD and rs in hi:            # ori: a number, not an address
            hi.pop(rt, None)
        elif op == 0 and (w & 0x3F) in (0x21, 0x2D, 0x25):      # addu/daddu/or: base + index, keep lui's value out
            hi.pop((w >> 11) & 31, None)
    return out


def load_syms(path, out):
    pat = re.compile(r"^(\S+) = 0x([0-9A-Fa-f]+);(?: // (.*))?$")
    try:
        lines = open(path)
    except OSError:
        return
    for l in lines:
        m = pat.match(l.strip())
        if m and "type:func" not in (m.group(3) or ""):
            sz = re.search(r"size:0x([0-9A-Fa-f]+)", m.group(3) or "")
            out.append((int(m.group(2), 16), int(sz.group(1), 16) if sz else 0, m.group(1)))


def main():
    M1, G, M2 = load_mh1(), load_g(), load_mh2()
    # reference data symbols, per reference address space
    sy = {"mh1": collections.defaultdict(list), "g": collections.defaultdict(list)}
    for m in ("main", "game", "lobby", "select", "yn"):
        load_syms(P("config/symbols/%s.txt" % m), sy["mh1"][m])
    for m in ("main", "sub_main", "game", "lobby", "select", "yn", "nethttp"):
        load_syms(P("config/mhg/symbols/%s.txt" % m), sy["g"][m])
    for m in ("main", "game", "lobby", "select", "yn"):
        load_syms(P("config/mhg/pc_symbols/%s.txt" % m), sy["g"][m])    # the PC view: sized, with G's hand-found names
    for src in sy:
        for m in sy[src]:
            sy[src][m].sort()
    keys = {src: {m: [e[0] for e in sy[src][m]] for m in sy[src]} for src in sy}

    def name_at(src, mods, t):
        for m in mods:
            if m not in sy[src]:
                continue
            k = bisect.bisect_right(keys[src][m], t) - 1
            if k < 0:
                continue
            a, sz, nm = sy[src][m][k]
            if t < a + max(sz, 1):
                return nm, t - a
        return None, 0

    def mod2_of(mod, a):
        for mm in (mod, "main", "sub_main", "gm_sub", "lbguild", "nethttp"):
            x = M2.get(mm)
            if x and x.vram <= a < x.end and not (x.text_lo <= a < x.text_hi):
                return mm
        return None

    votes = collections.defaultdict(collections.Counter)
    strong = set()
    n = 0
    for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"))):
        if not r["ref_addr"]:
            continue
        cls, s = r["class"], float(r["sim"] or 0)
        if cls not in ("identical", "absolute", "shape") and not (cls == "edited" and s >= 0.75):
            continue
        mod, a2, size = r["mh2_module"], int(r["mh2_addr"], 16), int(r["size"])
        src, rm, a1 = r["src"], r["ref_module"], int(r["ref_addr"], 16)
        if size > 0x8000 or size < 8:
            continue
        ref = (M1 if src == "mh1" else G).get(rm)
        if ref is None:
            continue
        b = M2[mod].words(a2, size // 4)
        # the reference's own size: its symbol's, else as long as MH2's plus a bit
        w = ref.words(a1, min(len(b) + len(b) // 4 + 8, (ref.text_hi - a1) // 4))
        gp = MH1_GP if src == "mh1" else G_GP
        ra = accesses(w, a1, gp)
        ma = accesses(b, a2, None)
        if not ra or not ma:
            continue
        ta, tb = [token(x) for x in w], [token(x) for x in b]
        if len(ta) * len(tb) > 2500000:
            continue
        sm = difflib.SequenceMatcher(None, ta, tb, autojunk=False)
        mods = (rm, "main") if rm != "main" else ("main", "sub_main", "game")
        if src == "mh1":
            mods = (rm, "main", "game")
        else:
            mods = (rm, "main", "sub_main", "game")
        pairs = []
        for i0, j0, sz in sm.get_matching_blocks():
            for k in range(sz):
                if i0 + k in ra and j0 + k in ma:
                    pairs.append((ra[i0 + k], ma[j0 + k]))
        for t1, t2 in pairs:
            nm, off = name_at(src, mods, t1)
            if not nm or nm.startswith("D_") or nm.startswith("func_"):
                continue
            t = (t2 - off) & 0xFFFFFFFF
            mm = mod2_of(mod, t)
            if mm:
                votes[nm][(mm, t)] += 1
                if cls in ("identical", "absolute", "shape"):
                    strong.add((nm, mm, t))
        n += 1
    print("pairs read: %d; names voted: %d" % (n, len(votes)))
    known = {}
    for m in M2:
        p = P("config/mh2/symbols/%s.txt" % m)
        lst = []
        load_syms(p, lst)
        for a, sz, nm in lst:
            known[re.sub(r"_mh2_[0-9A-F]{8}$", "", nm)] = (m, a)
    out, agree, wrong = [], 0, []
    for nm, c in sorted(votes.items()):
        (k, v), tot = c.most_common(1)[0], sum(c.values())
        ok = (v >= 2 and v * 3 >= tot * 2) or (v == tot and (nm, k[0], k[1]) in strong) or (v == 1 and tot == 1)
        if nm in known:
            if ok:
                if (k[0], k[1]) == known[nm]:
                    agree += 1
                else:
                    wrong.append((nm, known[nm][1], k[1]))
            continue
        if ok:
            out.append((k[0], nm, k[1], v, tot))
    print("check on names mh2_map.py placed: %d agree, %d differ %s" % (agree, len(wrong), ["%s %X/%X" % w for w in wrong[:12]]))
    with open(P("config/mh2/data_map.txt"), "w") as o:
        o.write("# Generated by tools/mh2_datamap.py: MH1/G data names at MH2 addresses, from aligned code. module name address votes total\n")
        for m, nm, a, v, t in sorted(out):
            o.write("%s %s 0x%08X %d %d\n" % (m, nm, a, v, t))
    print("new data names: %d (config/mh2/data_map.txt)" % len(out))


if __name__ == "__main__":
    main()
