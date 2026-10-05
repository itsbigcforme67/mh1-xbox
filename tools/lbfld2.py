#!/usr/bin/env python3
"""lbfld2.py NAME... : apply tools/lbfld.py conversions to build/lbauto_e2/NAME.c in memory, keep the result when it
compiles with no more differing instructions."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, 'tools')
import importlib.util
spec = importlib.util.spec_from_file_location('lbfld_mod', 'tools/lbfld.py')
src = open('tools/lbfld.py').read().split('for fn in sys.argv[1:]:')[0]
ns = {'__file__': os.path.abspath('tools/lbfld.py')}
exec(src, ns)
conv = ns['conv']
def chk(fn, s):
    path = 'src/lobby/zz_f2_%s.c' % fn
    open(path, 'w').write(s)
    try:
        r = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True); out = r.stdout + r.stderr
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    return (0 if m.group(1) == 'OK' else int(m.group(2))) if m else None
for fn in sys.argv[1:]:
    p = 'build/lbauto_e2/%s.c' % fn
    if not os.path.exists(p): print(fn, 'nofile'); continue
    s = open(p).read()
    base = chk(fn, s)
    s2 = conv(s)
    if s2 == s: print(fn, 'same'); continue
    d = chk(fn, s2)
    if d is not None and (base is None or d <= base):
        open(p, 'w').write(s2); print(fn, base, '->', d)
    else:
        print(fn, 'worse', base, d)
