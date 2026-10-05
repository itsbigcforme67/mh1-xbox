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
