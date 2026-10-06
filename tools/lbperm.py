#!/usr/bin/env python3
"""lbperm.py FILE FUNC START END : try every ordering of the statement lines [START,END) (1-based) of FILE and score FUNC with check.py
(3 threads). Stops at the first OK. Prints the best ordering. Keep START..END to <= 6 independent statements (720 compiles)."""
import sys, itertools, subprocess, os, re, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f, fn, s, e = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
L = open(f).read().split('\n')
mid = L[s - 1:e - 1]
def run(p):
    tmp = f.replace('.c', '_zz%d.c' % (abs(hash(p)) % 1000000))
    open(tmp, 'w').write('\n'.join(L[:s - 1] + list(p) + L[e - 1:]))
    try: out = subprocess.run(['python3', 'tools/check.py', tmp, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally: os.remove(tmp)
    l = [x for x in out.split('\n') if ' %s ' % fn in x]
    if not l: return p, 9999
    if l[0].startswith('OK'): return p, 0
    m = re.search(r'\((\d+)/', l[0]); return p, int(m.group(1)) if m else 9999
best = None
with cf.ThreadPoolExecutor(3) as ex:
    for p, sc in ex.map(run, list(itertools.permutations(mid))):
        if best is None or sc < best[1]:
            best = (p, sc); print(sc, flush=True)
            if sc == 0: break
print(best[1]); print('\n'.join(best[0]))
