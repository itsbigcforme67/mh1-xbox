#!/usr/bin/env python3
"""lbsweep.py [-j N] : check every build/lbauto/NAME.c that is not registered yet; print the OK ones and write build/lbsweep.json."""
import re, subprocess, sys, os, json, glob, concurrent.futures as cf
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
S = '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/fl.txt'
info = {}
for l in open(S):
    if l.strip():
        a, nm, sz = l.split(); info[nm] = (int(a, 16), int(sz))
reg = []
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby': reg.append((int(p[1], 16), int(p[2], 16)))
names = []
for f in glob.glob('build/lbauto/*.c'):
    n = os.path.basename(f)[:-2]
    if n.endswith('.err') or n.endswith('.bsc') or n not in info: continue
    if any(x <= info[n][0] < y for x, y in reg): continue
    names.append(n)
jobs = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[1] == '-j' else 2
def one(n):
    q = 'src/lobby/zz_sw_%s.c' % n
    open(q, 'w').write(open('build/lbauto/%s.c' % n).read())
    try:
        out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/(\d+))?' % re.escape(n), out, re.M)
    if not m: return n, ('err',)
    return n, ('OK',) if m.group(1) == 'OK' else ('diff', int(m.group(2)), int(m.group(3)))
res = {}
with cf.ThreadPoolExecutor(jobs) as ex:
    for n, r in ex.map(one, names):
        res[n] = r
        if r[0] == 'OK': print('OK', n, flush=True)
json.dump(res, open('build/lbsweep.json', 'w'))
