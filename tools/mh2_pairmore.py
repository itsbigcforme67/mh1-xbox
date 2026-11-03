#!/usr/bin/env python3
"""mh2_pairmore.py - more MH2 function pairs for the data map: reference functions tools/mh2_map.py found no twin for.

Most tables the PC still lacks have readers with no pair in docs/survey/mh2_map.csv: mh2_map.py asks for 70% of the opcode sequence
(or 50% and a call site). Two kinds of evidence it does not use, both from the data names tools/mh2_datamap.py has placed:

  table    a placed object holds a function pointer in the reference and an unpaired function at the same offset in MH2.
  items    a reference function and an unpaired MH2 function touch the same placed objects and call the same paired functions.
           Rare items count more; the opcode sequence must still agree (ratio >= 0.4) and the winner must stand clear.

Run in rounds with tools/mh2_datamap.py: pairs bring tables, tables bring pairs. --eval hides every 8th edited pair, tries to find it
again and prints the result.

Writes config/mh2/func_pairs.txt: `module 0xADDR name src ref_module 0xREF_ADDR how score` (names and addresses only).
Usage: python3 tools/mh2_pairmore.py [--eval]
"""
import collections
import csv
import difflib
import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mh2_datamap as D  # noqa: E402
from mhg_map import P, token  # noqa: E402


def calls_of(words, base):
    return [((base + 4 * i) & 0xF0000000) | ((w & 0x3FFFFFF) << 2) for i, w in enumerate(words) if (w >> 26) == 3]


class Pairs:
    def __init__(self, mp, hide=()):
        self.mp = mp
        self.hide = set(hide)
        self.rows = {}
        for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")):
            if r["mh2_module"] in D.SEES:
                self.rows[(r["mh2_module"], int(r["mh2_addr"], 16))] = r
        self.m2of = {}
        self.ref_done = set()
        self.m2_done = set()
        for mod, a2, _s, cls, s, src, rm, a1 in mp.pairs:
            k = (mod, a2)
            if k in self.hide or cls == "ambiguous":
                continue
            self.ref_done.add((src, rm, a1))
            self.m2_done.add(k)
            self.m2of[(src, rm, a1)] = a2
        self.new = {}
        mp.known_index()
        self.m2items = {}
        for k, r in self.rows.items():
            if k in self.m2_done:
                continue
            sz = int(r["size"])
            if sz < 16 or sz > 0x8000:
                continue
            self.m2items[k] = self.m2_items(k, sz)

    def m2_items(self, k, sz):
        m, a = k
        mp = self.mp
        w = mp.M2[m].words(a, sz // 4)
        out = set()
        for t, _op in D.accesses(w, None).values():
            g = D.mh2_mod(mp.M2, m, t)
            n = mp.known_at(g, t) if g else None
            if n:
                out.add(("d", n))
        for t in calls_of(w, a):
            out.add(("f", t))
        return out, w

    def ref_items(self, src, rm, a1):
        ref = self.mp.R[src]
        w = ref.mods[rm].words(a1, ref.fsize[(rm, a1)] // 4)
        out = set()
        sees = D.RSEES[src].get(rm, (rm,))
        for t, _op in D.accesses(w, ref.gp).values():
            n, _o = ref.obj_at(sees, t)
            if n and n in self.mp.known:
                out.add(("d", n))
        for t in calls_of(w, a1):
            for y in sees:
                g = self.m2of.get((src, y, t))
                if g:
                    out.add(("f", g))
                    break
        return out, w

    def tables(self):
        """pointer words of placed objects: reference function here, unpaired MH2 function there"""
        got = {}
        mp = self.mp
        for src, ref in mp.R.items():
            for rm, lst in ref.ptrs.items():
                x = ref.mods.get(rm)
                if x is None or rm not in D.RSEES[src]:
                    continue
                for p in lst:
                    if not x.has(p):
                        continue
                    cn, off = ref.obj_at((rm,), p)
                    if not cn or cn not in mp.known:
                        continue
                    t = x.word(p)
                    fk = next(((src, y, t) for y in D.RSEES[src][rm] if (y, t) in ref.fsize), None)
                    if fk is None or fk in self.ref_done:
                        continue
                    m2, c2 = mp.known[cn]
                    v = mp.init_word(m2, c2 + off)
                    if not v:
                        continue
                    for y in D.SEES[m2]:
                        if (y, v) in self.rows and (y, v) not in self.m2_done:
                            got.setdefault(fk, collections.Counter())[(y, v)] += 1
                            break
        n = 0
        for fk, c in got.items():
            if len(c) == 1:
                k = next(iter(c))
                self.add(fk, k, "table", 1.0)
                n += 1
        return n

    def add(self, fk, k, how, score):
        self.new[k] = (fk, how, score)
        self.ref_done.add(fk)
        self.m2_done.add(k)
        self.m2of[fk] = k[1]
        self.m2items.pop(k, None)

    def items(self):
        inv = collections.defaultdict(list)
        for k, (s, _w) in self.m2items.items():
            for it in s:
                inv[it].append(k)
        cands = []
        for src, ref in self.mp.R.items():
            for (rm, a1), sz in ref.fsize.items():
                fk = (src, rm, a1)
                if fk in self.ref_done or sz < 16 or sz > 0x8000:
                    continue
                if src == "mh1" and rm == "main" and False:
                    continue
                items, w = self.ref_items(src, rm, a1)
                if not items:
                    continue
                sc = collections.Counter()
                for it in items:
                    l = inv.get(it, ())
                    if 0 < len(l) <= 40:
                        wt = 1 + math.log(40 / len(l))
                        for k in l:
                            if k[0] in D.DATA_TO[rm]:
                                sc[k] += wt
                if not sc:
                    continue
                ta = [token(v) for v in w]
                best = []
                for k, s in sc.most_common(6):
                    wb = self.m2items[k][1]
                    if not 0.4 <= len(w) / max(len(wb), 1) <= 2.5:
                        continue
                    r = difflib.SequenceMatcher(None, ta, [token(v) for v in wb], autojunk=False).ratio()
                    best.append((s * (0.3 + r), r, s, k))
                best.sort(reverse=True)
                if not best:
                    continue
                t, r, s, k = best[0]
                nshare = len(items & self.m2items[k][0])
                if r < 0.4 or nshare < 2 and r < 0.6:
                    continue
                if len(best) > 1 and best[1][0] * 1.3 > t:
                    continue
                cands.append((t, fk, k, r))
        cands.sort(key=lambda e: -e[0])
        n = 0
        for t, fk, k, r in cands:
            if k in self.m2_done or fk in self.ref_done:
                continue
            self.add(fk, k, "items", r)
            n += 1
        return n


def run(mp, hide=()):
    pr = Pairs(mp, hide)
    for rnd in range(3):
        a = pr.tables()
        b = pr.items()
        print("round %d: %d from tables, %d from shared items" % (rnd + 1, a, b))
        if not a + b:
            break
    return pr


def load_known(mp):
    """names tools/mh2_datamap.py placed at high or medium become anchors"""
    for l in open(P("config/mh2/data_map.txt")):
        f = l.split()
        if l[0] != "#" and len(f) >= 4 and f[3] in ("high", "medium") and f[1] in mp.names:
            mp.known.setdefault(f[1], (f[0], int(f[2], 16)))


def main():
    mp = D.Mapper()
    mp.pairs = D.read_pairs(False)
    load_known(mp)
    if "--eval" in sys.argv:
        ed = [p for p in mp.pairs if p[3] in ("edited",) and p[4] >= 0.75 and p[2] >= 64 and p[0] in D.SEES]
        held = ed[::8]
        hide = set((p[0], p[1]) for p in held)
        truth = {(p[0], p[1]): (p[5], p[6], p[7]) for p in held}
        # the reference function goes back to being unpaired too: drop the row from the list the class reads
        mp.pairs = [p for p in mp.pairs if (p[0], p[1]) not in hide]
        pr = run(mp, ())
        ok = bad = 0
        by = collections.Counter()
        for k, t in truth.items():
            r = pr.new.get(k)
            if r is None:
                continue
            good = r[0] == t or pr.mp.R[r[0][0]].func.get(r[0][1:]) == pr.mp.R[t[0]].func.get(t[1:])
            ok += good
            bad += not good
            by[(r[1], good)] += 1
        print("held out %d edited pairs: found again %d, wrong %d, not found %d; %s" % (len(truth), ok, bad, len(truth) - ok - bad, dict(by)))
        return
    pr = run(mp)
    with open(P("config/mh2/func_pairs.txt"), "w") as o:
        o.write("# Generated by tools/mh2_pairmore.py: more MH2 function pairs. module address name src ref_module ref_address how score\n")
        for (m, a), ((src, rm, a1), how, sc) in sorted(pr.new.items()):
            nm = mp.R[src].func.get((rm, a1), "-")
            o.write("%s 0x%08X %s %s %s 0x%08X %s %.2f\n" % (m, a, nm, src, rm, a1, how, sc))
    print("wrote config/mh2/func_pairs.txt: %d pairs" % len(pr.new))


if __name__ == "__main__":
    main()
