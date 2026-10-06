#!/usr/bin/env python3
"""lbvar.py NAME... : mechanical variants of the near-matching auto drafts in build/lbauto/NAME.c (x >= C -> x > C-1,
x <= C -> x < C+1). Keeps a variant when it lowers the instruction difference; prints OK / remaining diffs."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def run(fn, src):
    path = 'src/lobby/zz_var_%s.c' % fn
    open(path, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby', '--at', ''] if False else ['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
def ge(m):
    return '> %s' % (hex(int(m.group(1), 0) - 1) if m.group(1).startswith('0x') else str(int(m.group(1)) - 1))
def le(m):
    return '< %s' % (hex(int(m.group(1), 0) + 1) if m.group(1).startswith('0x') else str(int(m.group(1)) + 1))
for fn in sys.argv[1:]:
    p = 'build/lbauto/%s.c' % fn
    src = open(p).read()
    best = run(fn, src)
    if best is None: print(fn, 'err'); continue
    bs = src
    for name, pat, fx in (('ge', r'>= (0x[0-9A-Fa-f]+|\d+)\b', ge), ('le', r'<= (0x[0-9A-Fa-f]+|\d+)\b', le)):
        s2 = re.sub(pat, fx, bs)
        if s2 != bs:
            r = run(fn, s2)
            if r is not None and r < best: best, bs = r, s2
    if bs != src: open(p, 'w').write(bs)
    print(fn, 'OK' if best == 0 else 'd%d' % best, flush=True)
