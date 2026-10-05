#!/usr/bin/env python3
"""plmove.py - append the functions of pl_wip.c to pl_nm.c and reset pl_wip.c"""
import os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
w = open(os.path.join(ROOT, "src/main/pl/pl_wip.c")).read()
hdr = '#include "plf.h"\n'
body = w[w.index(hdr) + len(hdr):].strip("\n")
if body:
    with open(os.path.join(ROOT, "src/main/pl/pl_nm.c"), "a") as f:
        f.write("\n" + body + "\n")
open(os.path.join(ROOT, "src/main/pl/pl_wip.c"), "w").write(w[:w.index(hdr) + len(hdr)])
