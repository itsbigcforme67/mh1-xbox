#!/usr/bin/env python3
"""mh2_pairmore.py - more MH2 function pairs for the data map: reference functions tools/mh2_map.py found no twin for.

Most tables the PC still lacks have readers with no pair in docs/survey/mh2_map.csv: mh2_map.py asks for 70% of the opcode sequence
(or 50% and a call site). Two kinds of evidence it does not use, both from the data names tools/mh2_datamap.py has placed:

  table    a placed object holds a function pointer in the reference and an unpaired function at the same offset in MH2.
  order    functions keep their file's order. Between two sure pairs, the reference's functions and MH2's are aligned in order by
           opcode ratio: unnamed ones get pairs, and a weak name out of place is listed (build/mh2/order_suspects.txt; --fix writes
           the clear ones into config/mh2/func_fix.txt).
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

if os.environ.get("PYTHONHASHSEED") != "0" and __name__ == "__main__":      # same output on every run: set order feeds ties
    os.environ["PYTHONHASHSEED"] = "0"
    os.execv(sys.executable, [sys.executable] + sys.argv)

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

    def order(self):
        """see the module's text: sure pairs in order (longest increasing run), gaps between them aligned"""
        mp = self.mp
        rows = self.rows
        n = 0
        self.suspects = []
        name_at = {}
        for k, r in rows.items():
            name_at[k] = r["name"]
        for src, ref in mp.R.items():
            byname = {}
            for (rm, a), nm in ref.func.items():
                if (rm, a) in ref.fsize and ref.fsize[(rm, a)] >= 8:
                    byname.setdefault(nm, (rm, a))
            for rm in ref.mods:
                flist = sorted(a for (m, a) in ref.fsize if m == rm and ref.fsize[(m, a)] >= 8)
                if not flist:
                    continue
                for m2 in D.DATA_TO.get(rm, ()):
                    mf = sorted(a for (m, a) in rows if m == m2)
                    if not mf:
                        continue
                    anchors = []
                    for a in mf:
                        r = rows[(m2, a)]
                        if r["class"] in ("identical", "absolute", "shape") or (r["class"] == "edited" and float(r["sim"] or 0) >= 0.9):
                            h = byname.get(r["name"])
                            if h and h[0] == rm:
                                anchors.append((h[1], a))
                    if len(anchors) < 3:
                        continue
                    anchors.sort()
                    # longest increasing subsequence of the MH2 addresses
                    tails, prev, idx = [], [-1] * len(anchors), []
                    import bisect
                    for i, (_r, a) in enumerate(anchors):
                        j = bisect.bisect_left(tails, a)
                        if j == len(tails):
                            tails.append(a)
                            idx.append(i)
                        else:
                            tails[j] = a
                            idx[j] = i
                        prev[i] = idx[j - 1] if j else -1
                    i = idx[-1] if idx else -1
                    run = []
                    while i >= 0:
                        run.append(anchors[i])
                        i = prev[i]
                    run.reverse()
                    for (r0, m0), (r1, m1) in zip(run, run[1:]):
                        F = [a for a in flist if r0 < a < r1]
                        X = [a for a in mf if m0 < a < m1 and 16 <= int(rows[(m2, a)]["size"]) <= 0x8000]
                        F = [a for a in F if 16 <= ref.fsize[(rm, a)] <= 0x8000]
                        if not F or not X or len(F) * len(X) > 900:
                            continue
                        n += self.align_gap(src, ref, rm, m2, F, X)
        return n

    def align_gap(self, src, ref, rm, m2, F, X):
        mp = self.mp
        tf = [[token(v) for v in ref.mods[rm].words(a, ref.fsize[(rm, a)] // 4)] for a in F]
        tx = [[token(v) for v in mp.M2[m2].words(a, int(self.rows[(m2, a)]["size"]) // 4)] for a in X]
        R = [[0.0] * len(X) for _ in F]
        for i, p in enumerate(tf):
            for j, q in enumerate(tx):
                if 0.4 <= len(p) / len(q) <= 2.5:
                    R[i][j] = difflib.SequenceMatcher(None, p, q, autojunk=False).ratio()
        S = [[0.0] * (len(X) + 1) for _ in range(len(F) + 1)]
        for i in range(len(F) - 1, -1, -1):
            for j in range(len(X) - 1, -1, -1):
                S[i][j] = max(S[i + 1][j], S[i][j + 1], S[i + 1][j + 1] + max(R[i][j] - 0.4, -0.2 if R[i][j] else -9))
        i = j = n = 0
        while i < len(F) and j < len(X):
            if S[i][j] == S[i + 1][j]:
                i += 1
            elif S[i][j] == S[i][j + 1]:
                j += 1
            else:
                fk, k, r = (src, rm, F[i]), (m2, X[j]), R[i][j]
                row = self.rows[k]
                i += 1
                j += 1
                if r < 0.5:
                    continue
                unnamed = k not in self.m2_done
                if unnamed and fk not in self.ref_done:
                    self.add(fk, k, "order", r)
                    n += 1
                elif not unnamed and row["class"] in ("edited", "rewritten") and ref.func.get(fk[1:]) not in (None, row["name"]):
                    self.suspects.append((k, row["name"], row["sim"], ref.func[fk[1:]], r))
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
        c = pr.order()
        b = pr.items()
        print("round %d: %d from tables, %d by order, %d from shared items" % (rnd + 1, a, c, b))
        if not a + b + c:
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
    print("wrote config/mh2/func_pairs.txt: %d pairs; %s" % (len(pr.new), collections.Counter(v[1] for v in pr.new.values())))
    with open(P("build/mh2/order_suspects.txt"), "w") as o:
        seen = set()
        for k, nm, sim, other, r in pr.suspects:
            if k not in seen:
                seen.add(k)
                o.write("%s 0x%08X map=%s sim=%s order=%s ratio=%.2f\n" % (k[0], k[1], nm, sim, other, r))
    print("order suspects: %d (build/mh2/order_suspects.txt)" % len(seen))


if __name__ == "__main__":
    main()
