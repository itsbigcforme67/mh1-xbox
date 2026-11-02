#!/usr/bin/env python3
"""mh2_namecheck.py - an independent check of the names in docs/survey/mh2_map.csv.

For a function named from a reference (MH1 or G) with the same number of calls on both sides, walk
the calls in order: is the MH2 callee the twin of the reference callee? Each pair counts only if both
callees were named by their own code (how = exact, neighbour, norm or ngram), never from a call site,
so a name is not checked against itself. This estimates how often a named pair is right; it does
not prove any one name.

Usage: python3 tools/mh2_namecheck.py
"""
import collections
import csv
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mh2_map as M  # noqa: E402
from mhg_map import P, load_g, load_mh1  # noqa: E402


def main():
    m2, m1, g = M.load_mh2(), load_mh1(), load_g()
    rows = list(csv.DictReader(open(P("docs/survey/mh2_map.csv"), encoding="utf-8")))
    at = {(r["mh2_module"], int(r["mh2_addr"], 16)): r for r in rows}
    ref = {(r["src"], r["ref_module"], int(r["ref_addr"], 16)): r for r in rows if r["src"]}

    def callee(mod, t):
        for mm in (mod, "main", "sub_main", "gm_sub", "yn", "nethttp", "lbguild"):
            if (mm, t) in at:
                return at[(mm, t)]
        return None

    st = collections.defaultdict(lambda: [0, 0])
    for r in rows:
        if not r["src"] or r["how"] == "callsite":
            continue
        src = m1 if r["src"] == "mh1" else g
        if r["ref_module"] not in src:
            continue
        a, a2, n = int(r["ref_addr"], 16), int(r["mh2_addr"], 16), int(r["size"]) // 4
        c1 = M.calls(M.trim(src[r["ref_module"]].words(a, n)), a)
        c2 = M.calls(M.trim(m2[r["mh2_module"]].words(a2, n)), a2)
        if len(c1) != len(c2) or not c1:
            continue
        k = r["class"]
        if k == "edited":
            k += " >= 0.9" if float(r["sim"]) >= 0.9 else " < 0.9"
        for x, y in zip(c1, c2):
            q = callee(r["mh2_module"], y)
            if not q or not q["src"] or q["how"] == "callsite":
                continue
            p = next((ref[(r["src"], mm, x)] for mm in (r["ref_module"], "main", "sub_main")
                      if (r["src"], mm, x) in ref), None)
            if p is None:
                continue
            st[k][0] += p is q
            st[k][1] += 1
    for k, (ok, n) in sorted(st.items()):
        print("%-16s %6d of %6d callee pairs agree (%.1f%%)" % (k, ok, n, 100 * ok / max(n, 1)))


if __name__ == "__main__":
    main()
