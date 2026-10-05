#!/usr/bin/env python3
"""plclean.py FILE - post-process an m2c pl draft: PLSW raw accessors, switch comments, kind, Pl_master_ck(pl)."""
import re, sys
p = sys.argv[1]
s = open(p).read()
for off, n in [("0x364", "pl->sw.now"), ("0x366", "pl->sw.old"), ("0x368", "pl->sw.trg"), ("0x36A", "pl->sw.trg_old"),
               ("0x378", "pl->sw.an_now"), ("0x37A", "pl->sw.an_old"), ("0x37C", "pl->sw.an_trg"), ("0x37E", "pl->sw.an_trg_old"),
               ("0x380", "pl->sw.ang[0]"), ("0x382", "pl->sw.ang[1]")]:
    s = s.replace("(*(u16 *)((u8 *)pl + %s))" % off, n)
s = re.sub(r' *\/\* switch \d.*?\*\/', '', s)
s = re.sub(r'\(\(int\) \((act_ck\(pl, 0, 0x[0-9A-F]+\)) << 0x30\) >> 0x30\)', r'\1', s)
s = s.replace("PU8(pl, 2)", "pl->kind").replace("Pl_master_ck()", "Pl_master_ck(pl)")
open(p, 'w').write(s)

# clean the junk prototypes pl_draft appended to include/plf.h: drop M2C_UNK/s64/TODO lines and later duplicates of a name
import os
h = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "include", "plf.h")
lines = open(h).read().split("\n")
seen, out = set(), []
for l in lines:
    if "M2C_UNK" in l or "TODO type" in l or re.match(r"^s64 ", l) or re.match(r"^void pl_(mv|at|dm|egg|chat)\d+\(int\);", l):
        continue
    m = re.match(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\(", l)
    if m and l.rstrip().endswith(");") and not l.startswith("typedef"):
        if m.group(1) in seen:
            print("plclean: dropped duplicate prototype:", l)
            continue
        seen.add(m.group(1))
    out.append(l)
open(h, "w").write("\n".join(out))
