#!/usr/bin/env python3
"""lbbsc1.py NAME... : rewrite F(T, (F(U, B, OFF) + B), D) (byte-array index + base) as BSC1(T, B, F(U, B, OFF), D); keep when diff drops."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def score(n, src):
    q = 'src/lobby/zz_b1_%s.c' % n
    open(q, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(n), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
pat = re.compile(r'F\((\w+), \((F\(\w+, (\w+), 0x[0-9A-F]+\)) \+ \3\), (0x[0-9A-F]+)\)')
for n in sys.argv[1:]:
    p = 'build/lbauto/%s.c' % n
    s = open(p).read()
    t = pat.sub(lambda m: 'BSC1(%s, %s, %s, %s)' % (m.group(1), m.group(3), m.group(2), m.group(4)), s)
    if t == s: print(n, 'nochange'); continue
    a = score(n, s); b = score(n, t)
    print(n, a, '->', b)
    if b is not None and (a is None or b < a): open(p, 'w').write(t)
