#!/usr/bin/env python3
"""pc_shift64.py - rewrite m2c's 64-bit sign extensions `(EXPR << 0x30) >> 0x30` as `((s16)(EXPR))` in C drafts the
PC build links. On the EE these come from dsll32 / dsra32 pairs (a 16-bit sign extension in a 64-bit register); in
32-bit C a shift by 48 is undefined and gcc folds it to 0, so every screen coordinate computed that way was 0
(agent B, 10 Oct 2026: the plaza *Trans drafts in src/lobby/b/nm). Edits the files in place, prints the count.
usage: pc_shift64.py FILE.c [...]"""
import sys
PAT = " << 0x30) >> 0x30"
for fn in sys.argv[1:]:
    s = open(fn).read()
    n = 0
    while PAT in s:
        i = s.index(PAT)
        d, j = 0, i - 1
        while True:
            c = s[j]
            if c == ')': d += 1
            elif c == '(':
                if d == 0: break
                d -= 1
            j -= 1
        expr = s[j + 1:i]
        s = s[:j] + "((s16)(" + expr + "))" + s[i + len(PAT):]
        n += 1
    open(fn, "w").write(s)
    print(fn, n)
