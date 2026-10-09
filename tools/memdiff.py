#!/usr/bin/env python3
"""memdiff.py FILE.c [FUNC ...]: multiset diff of the exact load/store instructions (op + offset, non-sp bases, relocated
operands excluded) of the original and of our compile. Unlike sigdiff.py it keeps the width AND the sign (lh vs lhu, lb vs lbu),
so a field read with the wrong signedness or width, or at a different offset, shows up. Sign-only pairs are listed
separately ("sign"): they matter only when the value can reach the top bit. Standard library only."""
import sys, subprocess, re, collections
f = sys.argv[1]; want = set(sys.argv[2:])
out = subprocess.run(['python3', 'tools/check.py', f, '-v'], capture_output=True, text=True).stdout.split('\n')
rx = re.compile(r'^(l|s)(wc1|wl|wr|w|hu|h|bu|b|d|q|dc1|qc1)\s+\S+, (-?\d+)\((\w+)\)(\s*\(reloc\))?')
def tok(s):
    reloc = '(reloc)' in s
    s = re.sub(r'\s+', ' ', s).strip()
    m = rx.match(s)
    if not m or m.group(4) in ('sp', 'gp'): return None
    if reloc: RELOC[cur] = RELOC.get(cur, 0) + 1; return None
    w = {'wc1': 'w'}.get(m.group(2), m.group(2))
    return (m.group(1) + w, int(m.group(3)))
res = {}; cur = None; RELOC = {}
for l in out:
    m = re.match(r'^(OK|--)\s+(\S+)\s', l)
    if m: cur = m.group(2); res[cur] = (collections.Counter(), collections.Counter()); continue
    if cur and '|' in l:
        a, b = l.split('|', 1)
        mm = re.match(r'\s*(>>)?\s*([0-9A-F]{8})\s+(.*)', a)
        if mm:
            t = tok(mm.group(3))
            if t: res[cur][0][t] += 1
        t = tok(b)
        if t: res[cur][1][t] += 1
SIGN = {'lh': 'lhu', 'lhu': 'lh', 'lb': 'lbu', 'lbu': 'lb', 'lw': 'lwu'}
for fn, (a, b) in res.items():
    if want and fn not in want: continue
    mo, mu = a - b, b - a
    # symbol accesses: ours is `op 0(reg)` (+ reloc) or dropped, the original shows the low half of the address as an offset
    big = lambda off: off >= 0x1000 or off < -0x400
    for (op, off), n in list(mo.items()):
        if big(off):
            k = min(n, mu.get((op, 0), 0)) if (op, 0) in mu else 0
            if k: mu[(op, 0)] -= k; mo[(op, off)] -= k
    nrel = RELOC.get(fn, 0)
    for (op, off), n in sorted(list(mo.items()), key=lambda kv: -abs(kv[0][1])):
        if big(off) and nrel > 0 and mo[(op, off)] > 0:
            k = min(mo[(op, off)], nrel); mo[(op, off)] -= k; nrel -= k
    mo, mu = +mo, +mu
    # cancel sign-only pairs
    sign = []
    for (op, off), n in list(mo.items()):
        alt = SIGN.get(op)
        if alt and mu.get((alt, off), 0) > 0:
            k = min(n, mu[(alt, off)]); sign.append('%s<->%s %d x%d' % (op, alt, off, k))
            mo[(op, off)] -= k; mu[(alt, off)] -= k
    mo, mu = +mo, +mu
    if not mo and not mu and not sign: print('SAME   ', fn); continue
    print('DIFFERS', fn, 'REAL' if (mo or mu) else 'sign-only')
    if sign: print('    sign:', '; '.join(sign))
    if mo: print('    orig-only:', ', '.join('%s %d x%d' % (k[0], k[1], v) for k, v in sorted(mo.items(), key=str)))
    if mu: print('    ours-only:', ', '.join('%s %d x%d' % (k[0], k[1], v) for k, v in sorted(mu.items(), key=str)))
