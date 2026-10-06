#!/usr/bin/env python3
"""lbdeclrand.py FILE FUNC "decl1|decl2|..." [N] : like lbdbf.py but samples N random orders (default 300) of the given contiguous
declaration lines when there are too many permutations. Writes the best order back to FILE. 3 threads."""
import sys, random, re, subprocess, os, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f, fn, spec = sys.argv[1:4]
N = int(sys.argv[4]) if len(sys.argv) > 4 else 300
decls = spec.split('|')
src = open(f).read()
block = ''.join('    %s\n' % d for d in decls)
assert block in src, 'declaration block not found contiguous in that order'
def run(perm):
    s = src.replace(block, ''.join('    %s\n' % d for d in perm), 1)
    tmp = f.replace('.c', '_zzr%d.c' % (abs(hash(perm)) % 1000000))
    open(tmp, 'w').write(s)
    try: out = subprocess.run(['python3', 'tools/check.py', tmp, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally: os.remove(tmp)
    l = [x for x in out.split('\n') if ' %s ' % fn in x]
    if not l: return perm, 99999
    if l[0].startswith('OK'): return perm, 0
    m = re.search(r'\((\d+)/', l[0]); return perm, int(m.group(1)) if m else 99999
random.seed(1)
perms = {tuple(decls)}
while len(perms) < N: 
    p = decls[:]; random.shuffle(p); perms.add(tuple(p))
best = None
with cf.ThreadPoolExecutor(3) as ex:
    for perm, sc in ex.map(run, list(perms)):
        if best is None or sc < best[1]:
            best = (perm, sc); print(sc, flush=True)
            if sc == 0: break
if best[1] < 99999:
    open(f, 'w').write(src.replace(block, ''.join('    %s\n' % d for d in best[0]), 1))
print('best', best)
