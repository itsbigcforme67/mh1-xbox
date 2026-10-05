#!/usr/bin/env python3
"""lbv.py VARIANT NAME... : try a mechanical source variant on build/lbauto/NAME.c; keep it when the diff count drops.
Variants: bswp  (extern int bsw -> extern u8 *bsw, locals assigned from bsw become u8 *)."""
import re, subprocess, sys, os, concurrent.futures as cf
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def score(n, src):
    q = 'src/lobby/zz_v_%s.c' % n
    open(q, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(n), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
def v_bswp(s):
    if not re.search(r'extern (?:int|s32) bsw;', s): return None
    s = re.sub(r'extern (?:int|s32) bsw;', 'extern u8 *bsw;', s)
    for nm in set(re.findall(r'\b(\w+) = bsw;', s)):
        s = re.sub(r'\b(?:int|s32) %s;' % nm, 'u8 *%s;' % nm, s)
    return s
V = {'bswp': v_bswp}
def run(n):
    p = 'build/lbauto/%s.c' % n
    if not os.path.exists(p): return n, 'nosrc'
    s = open(p).read(); t = V[sys.argv[1]](s)
    if t is None: return n, 'na'
    a = score(n, s); b = score(n, t)
    if b is None: return n, 'err'
    if a is None or b < a:
        open(p, 'w').write(t); return n, '%s -> %s' % (a, b)
    return n, 'same/worse %s %s' % (a, b)
with cf.ThreadPoolExecutor(2) as ex:
    for n, r in ex.map(run, sys.argv[2:]): print(n, r, flush=True)
