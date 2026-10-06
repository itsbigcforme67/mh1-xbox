#!/usr/bin/env python3
"""lbhill.py FILE FUNC START END : hill-climb over statement order. Lines [START,END) (1-based) of FILE are treated as independent
single-line statements; repeatedly tries moving one line to another position (3 threads) and keeps any move that lowers the
check.py instruction-diff count of FUNC. Writes the best order back to FILE. Use it where permutation (lbperm.py) has too many orders."""
import sys, os, re, subprocess, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f, fn, s, e = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
L = open(f).read().split('\n')
head, mid, tail = L[:s - 1], L[s - 1:e - 1], L[e - 1:]
def score(lines, tag=''):
    tmp = f.replace('.c', '_zzh%s.c' % tag)
    open(tmp, 'w').write('\n'.join(head + lines + tail))
    try: out = subprocess.run(['python3', 'tools/check.py', tmp, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally: os.remove(tmp)
    l = [x for x in out.split('\n') if ' %s ' % fn in x]
    if not l: return 9999
    if l[0].startswith('OK'): return 0
    m = re.search(r'\((\d+)/', l[0]); return int(m.group(1)) if m else 9999
best = score(mid, 'b'); print('start', best, flush=True)
improved = True
while improved and best > 0:
    improved = False
    cands = []
    for i in range(len(mid)):
        for j in range(len(mid)):
            if i != j:
                t = mid[:]; x = t.pop(i); t.insert(j, x); cands.append(t)
    with cf.ThreadPoolExecutor(3) as ex:
        res = list(ex.map(lambda ct: (score(ct[1], str(ct[0])), ct[1]), enumerate(cands)))
    sc, t = min(res, key=lambda r: r[0])
    if sc < best:
        best, mid, improved = sc, t, True
        print('improved', best, flush=True)
open(f, 'w').write('\n'.join(head + mid + tail))
print('final', best)
