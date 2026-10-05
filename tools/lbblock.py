#!/usr/bin/env python3
"""lbblock.py NAME... : second batch of mechanical fixes for the m2c-derived drafts in src/lobby/b/nm/NAME.c, each kept only
when the number of differing instructions drops:
  T2: `block_N: default: return X;` + `goto block_N;` -> `break;` and one `return X;` after the switch."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def chk(fn, s):
    path = 'src/lobby/zz_f3_%s.c' % fn
    open(path, 'w').write(s)
    try:
        r = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True); out = r.stdout + r.stderr
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    return (0 if m.group(1) == 'OK' else int(m.group(2))) if m else None
def t2(s):
    m = re.search(r'^block_(\d+):\n +default:[^\n]*\n +return ([^;]+);\n', s, re.M)
    if not m: return s
    n, val = m.group(1), m.group(2)
    s = s[:m.start()] + s[m.end():]
    s = re.sub(r'goto block_%s;' % n, 'break;', s)
    s = re.sub(r'\n\}\n$', '\n    return %s;\n}\n' % val, s)
    return s
for fn in sys.argv[1:]:
    p = 'src/lobby/b/nm/%s.c' % fn
    s = open(p).read()
    base = chk(fn, s)
    if base is None: print(fn, 'err'); continue
    s2 = t2(s)
    if s2 == s: print(fn, 'same'); continue
    d = chk(fn, s2)
    if d is not None and d < base:
        open(p, 'w').write(s2); print(fn, base, '->', d)
    else:
        print(fn, 'no gain', base, d)
