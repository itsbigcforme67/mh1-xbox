#!/usr/bin/env python3
"""lbtail.py NAME... : in src/lobby/b/nm/NAME.c drop the `return;` that m2c puts at the end of the last switch case
(the original has no jump there); keep the change when the number of differing instructions does not grow."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def chk(fn, s):
    path = 'src/lobby/zz_tl_%s.c' % fn
    open(path, 'w').write(s)
    try:
        r = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True); out = r.stdout + r.stderr
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    return (0 if m.group(1) == 'OK' else int(m.group(2))) if m else None
for fn in sys.argv[1:]:
    p = 'src/lobby/b/nm/%s.c' % fn
    s = open(p).read()
    base = chk(fn, s)
    s2 = re.sub(r'\n( +)return;\n(    \}\n\}\n)$', r'\n\2', s)
    s2 = re.sub(r'\n +return;\n( {4,8}\}\n {4}\}\n\}\n)$', r'\n\1', s2) if s2 == s else s2
    if s2 == s: print(fn, 'same'); continue
    d = chk(fn, s2)
    if d is not None and base is not None and d < base:
        open(p, 'w').write(s2); print(fn, base, '->', d)
    else:
        print(fn, 'no gain', base, d)
