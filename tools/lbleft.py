#!/usr/bin/env python3
"""lbleft.py [MIN [MAX]] : list lobby functions (0x5C4E60-end) that are neither registered nor defined in any src/lobby/f/*.c file."""
import re, glob, os, sys
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
S = '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/fl.txt'
info = []
for l in open(S):
    if l.strip():
        a, nm, sz = l.split(); info.append((int(a, 16), nm, int(sz)))
reg = []
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby': reg.append((int(p[1], 16), int(p[2], 16)))
defined = set()
for f in glob.glob('src/lobby/f/*.c'):
    t = open(f).read()
    for m in re.finditer(r'^[\w\*\s]*?\b(\w+)\([^;{)]*\)(?:\n[\w \*;\[\]]+)*\s*\{', t, re.M):
        defined.add(m.group(1))
lo = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0
hi = int(sys.argv[2], 16) if len(sys.argv) > 2 else 1 << 32
tot = 0; n = 0
for a, nm, sz in info:
    if any(x <= a < y for x, y in reg): continue
    if nm in defined or re.sub(r'_[0-9A-F]{6,8}$', '', nm) in defined: continue
    if lo <= a < hi:
        print('%X %s %d' % (a, nm, sz)); tot += sz; n += 1
print('left', n, tot, file=sys.stderr)
