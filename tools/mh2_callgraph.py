#!/usr/bin/env python3
"""mh2_callgraph.py - what each stage of MH2 reaches, with pointers followed (docs/mh2_port.md section 4 and item 8).

From a start set, in the overlays that stage has loaded:
  call     jal, and j to another function (tail call)
  code     a function takes a function's address (lui + addiu: a task or callback handed on)
  table    a function references data, and the data holds function addresses. An object runs from the referenced address to the
           next address any code references or the next named symbol; pointers to data inside it are followed the same way.

A word counts as a function pointer only if it is exactly the start of a function of docs/survey/mh2_map.csv in a loaded module, so
a number that looks like an address inside code is not followed. Jump tables inside a function are not followed (their targets are
inside the function). An over-reach is possible: a table reached this way may hold entries that never run at this stage (a table of
all monsters, say), so "table" numbers are an upper bound on what a given quest runs, and jal-only numbers a lower bound.

Which overlays load together is read from the code here, not known: see `crossrefs`, printed first. A module pair counts when code or
data of one holds an exact function start of the other.

Writes docs/survey/mh2_reach.csv: function, module, address, size, class, name, then one column per stage (1 = reached with pointers).
Also a list of the tables a stage's code reads with no address for the PC: --missing [stage].
Usage: python3 tools/mh2_callgraph.py [--missing quest]
"""
import bisect
import collections
import csv
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mhg_map import P  # noqa: E402
from mh2_map import load_mh2  # noqa: E402
from mh2_datamap import LOADS  # noqa: E402

RES = ("main", "sub_main")
QUEST = RES + ("game", "gm_sub", "plsel")
TOWN = RES + ("lobby", "lbguild", "nethttp", "plsel", "gm_sub")
# the quest row with the other monsters' named functions cut off: an estimate of one hunt (their unnamed helpers fall away with them,
# unless something else reaches them). em01 is the Rathian in MH1's code; em00 and shared code stay.
ONE = re.compile(r"^(game_)?em(?!00|01)\d\d[a-z]?_")
# name, loaded overlays, start functions (name or (module, address))
STAGES = [("title", RES + ("select",), [("select", 0x6B5CF0), ("select", 0x6B5230)]),
          ("mode menu", RES + ("gm_sub", "game"), [("gm_sub", 0x8FAFD0)]),
          ("char select", RES + ("plsel", "gm_sub", "game"), [("plsel", 0xD60AB0)]),
          ("game task", QUEST, [("sub_main", 0x3260B0)]),
          ("quest", QUEST, ["enemy_mv", "Quest_start", "hit_check", "CameraMove", ("sub_main", 0x3260B0)]),
          ("quest, one monster", QUEST, ["enemy_mv", "Quest_start", "hit_check", "CameraMove", ("sub_main", 0x3260B0)]),
          ("town", TOWN, [("lobby", 0x7B97B0), ("lobby", 0x7C90C0), ("lobby", 0x7CA500)])]


def simm(x):
    return ((x & 0xFFFF) ^ 0x8000) - 0x8000


def scan(words, base):
    """(calls, refs): jal/j targets, and absolute addresses formed by lui + lo in one function"""
    calls, refs = [], []
    hi = {}
    for i, w in enumerate(words):
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        if op in (2, 3):
            calls.append(((base + 4 * i) & 0xF0000000) | ((w & 0x3FFFFFF) << 2))
        elif op == 0xF:
            hi[rt] = (w & 0xFFFF) << 16
        elif op == 0x09 or op in LOADS:
            if rs in hi and rs:
                refs.append((hi[rs] + simm(w)) & 0xFFFFFFFF)
            if op == 0x09 or op < 0x28:
                hi.pop(rt, None)
        elif op == 0xD:
            hi.pop(rt, None)
        elif op == 0 and (w & 0x3F) in (0x21, 0x2D, 0x25):
            rd = (w >> 11) & 31
            v = hi.get(rs, hi.get(rt)) if rs and rt else None
            if v is None:
                hi.pop(rd, None)
            else:
                hi[rd] = v
    return calls, refs


class Graph:
    def __init__(self):
        self.M = load_mh2()
        self.fn = {}                # (mod, addr) -> row
        self.byname = {}
        self.cov = collections.defaultdict(list)
        for l in open(P("config/mh2/c_files.txt")):
            f = l.split("#", 1)[0].split()
            if len(f) == 4 and ":" not in f[0]:
                self.cov[f[0]].append((int(f[1], 16), int(f[2], 16)))
        for r in csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")):
            k = (r["mh2_module"], int(r["mh2_addr"], 16))
            if r["mh2_module"] in self.M:
                self.fn[k] = r
                self.byname.setdefault(r["name"], k)
        self.st = collections.defaultdict(set)
        for m, a in self.fn:
            self.st[m].add(a)
        self.calls, self.refs = {}, {}
        for (m, a), r in self.fn.items():
            sz = int(r["size"])
            self.calls[(m, a)], self.refs[(m, a)] = scan(self.M[m].words(a, sz // 4), a)
        # object boundaries: every address code references, plus named data
        self.labels = collections.defaultdict(set)
        for (m, a), rf in self.refs.items():
            for t in rf:
                self.labels[self.home(m, t) or m].add(t)
        self.named = collections.defaultdict(dict)
        for mm in self.M:
            p = P("config/mh2/symbols/%s.txt" % mm)
            if os.path.exists(p):
                for l in open(p):
                    r = re.match(r"^(\S+) = 0x([0-9A-Fa-f]+);(.*)$", l.strip())
                    if r and "type:func" not in r.group(3):
                        self.named[mm][int(r.group(2), 16)] = r.group(1)
        self.placed = {}
        for l in open(P("config/mh2/data_map.txt")):
            f = l.split()
            if l[0] != "#" and len(f) >= 4 and f[3] != "low":
                self.named[f[0]][int(f[2], 16)] = f[1]
        self.sorted = {m: sorted(self.labels[m] | set(self.named[m])) for m in self.M}

    def home(self, m, a):
        if self.M[m].vram <= a < self.M[m].end:
            return m
        for x in self.M:
            if self.M[x].vram <= a < self.M[x].end:
                return x
        return None

    def resolve(self, frm, a, loaded):
        """the module holding address a, seen from module frm with these overlays loaded"""
        for m in (frm,) + loaded:
            if m in loaded and self.M[m].vram <= a < self.M[m].end:
                return m
        return None

    def data_ptrs(self, m, a, loaded, seen, out):
        """function starts held by the object at a (and objects it points at)"""
        todo = [(m, a)]
        while todo:
            m, a = todo.pop()
            x = self.M[m]
            if (m, a) in seen or a & 3 or x.text_lo <= a < x.text_hi or not x.has(a):
                continue
            seen.add((m, a))
            s = self.sorted[m]
            k = bisect.bisect_right(s, a)
            end = min(s[k] if k < len(s) else a + 0x4000, a + 0x4000, x.vram + len(x.data))
            p = a
            while p + 4 <= end:
                v = x.word(p)
                p += 4
                if v < 0x100000 or v >= 0x2000000:
                    continue
                t = self.resolve(m, v, loaded)
                if not t:
                    continue
                if v in self.st[t]:
                    out.add((t, v))
                elif not (self.M[t].text_lo <= v < self.M[t].text_hi):
                    todo.append((t, v))

    def reach(self, starts, loaded, ptrs, block=None):
        roots = [self.byname[s] if isinstance(s, str) else s for s in starts]
        todo, seen, dseen = list(roots), set(roots), set()
        via = {}
        while todo:
            k = todo.pop()
            m, _a = k
            nxt = set()
            for t in self.calls[k]:
                r = self.resolve(m, t, loaded)
                if r and t in self.st[r]:
                    nxt.add((r, t))
            if ptrs:
                for t in self.refs[k]:
                    r = self.resolve(m, t, loaded)
                    if not r:
                        continue
                    if t in self.st[r]:
                        nxt.add((r, t))
                    else:
                        self.data_ptrs(r, t, loaded, dseen, nxt)
            for n in nxt:
                if block and block.match(self.fn[n]["name"]):
                    continue
                if n not in seen:
                    seen.add(n)
                    todo.append(n)
        return seen

    def row(self, seen):
        c = collections.Counter()
        nf = collections.Counter()
        for k in seen:
            r = self.fn[k]
            sz = int(r["size"])
            c["bytes"] += sz
            cl = r["class"]
            g = "same" if cl in ("identical", "absolute", "shape") else "edited" if cl in ("edited", "rewritten") else "none"
            c[g] += sz
            nf[g] += 1
            if any(lo <= k[1] < hi for lo, hi in self.cov[k[0]]):
                c["C"] += sz
        return len(seen), c, nf

    def crossrefs(self):
        """how many exact function starts of module B appear in code and data of module A"""
        c = collections.Counter()
        for (m, a), rf in self.refs.items():
            for t in rf + self.calls[(m, a)]:
                for x in self.M:
                    if x != m and self.M[x].vram <= t < self.M[x].end and t in self.st[x]:
                        if not (self.M[m].vram <= t < self.M[m].end):
                            c[(m, x)] += 1
        for m, x in self.M.items():
            for a in range((x.text_hi + 3) & ~3, x.vram + len(x.data) - 3, 4):
                v = x.word(a)
                if x.vram <= v < x.end:
                    continue
                for y in self.M:
                    if y != m and self.M[y].vram <= v < self.M[y].end and v in self.st[y]:
                        c[(m, y)] += 1
        return c


def main():
    G = Graph()
    cr = G.crossrefs()
    print("exact function starts of one module held by another (which overlays load together; an address can collide):")
    for (a, b), n in sorted(cr.items()):
        if a not in RES and b not in RES:
            print("  %-9s -> %-9s %d" % (a, b, n))
    print("\n| from | loaded | jal only: functions, bytes | with pointers: functions | bytes | C | identical/absolute/shape | edited | no counterpart |")
    print("|---|---|---|---|---|---|---|---|---|")
    res = {}
    for nm, loaded, st in STAGES:
        blk = ONE if nm == "quest, one monster" else None
        lo = G.reach(st, loaded, False, blk)
        hi = G.reach(st, loaded, True, blk)
        res[nm] = hi
        n0, c0, _f = G.row(lo)
        n1, c1, _f = G.row(hi)
        print("| %s | %s | %d, %d | %d | %d | %d | %d | %d | %d |" % (nm, "+".join(loaded[2:]), n0, c0["bytes"], n1, c1["bytes"], c1["C"], c1["same"], c1["edited"], c1["none"]))
    with open(P("docs/survey/mh2_reach.csv"), "w", newline="") as o:
        w = csv.writer(o)
        w.writerow(["mh2_module", "mh2_addr", "size", "name", "class"] + [s[0] for s in STAGES])
        for k in sorted(G.fn):
            r = G.fn[k]
            w.writerow([k[0], "0x%08X" % k[1], r["size"], r["name"], r["class"]] + [1 if k in res[s[0]] else 0 for s in STAGES])
    if "--missing" in sys.argv:
        missing_report(G, res)


def missing_report(G, res):
    """the references in reached code to data with no name yet, most-read first: what is unnamed on this path"""
    stage = sys.argv[sys.argv.index("--missing") + 1]
    c = collections.Counter()
    for k in res[stage]:
        for t in set(G.refs[k]):
            m = G.resolve(k[0], t, dict((s[0], s[1]) for s in STAGES)[stage])
            if m and t not in G.st[m] and not (G.M[m].text_lo <= t < G.M[m].text_hi):
                c[(m, t)] += 1
    print("data addresses read by %s code: %d" % (stage, len(c)))


if __name__ == "__main__":
    main()
