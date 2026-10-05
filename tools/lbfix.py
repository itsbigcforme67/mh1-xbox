#!/usr/bin/env python3
"""lbfix.py NAME... : try to repair near-matching auto drafts (build/lbauto/NAME.c). m2c drops arguments that are just
passed through (a0 untouched), so for each call site we try inserting `arg0,` / `arg1,` / `arg2,` as extra leading args
(and a leading `int arg0` parameter when missing). Writes the best variant back; prints OK / remaining diff count."""
import sys, os, re, itertools, subprocess, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
KW = {'if', 'while', 'switch', 'for', 'return', 'sizeof', 'F'}
def run(fn, src, tag):
    path = 'src/lobby/zz_fix_%s_%s.c' % (re.sub(r'\W', '_', fn), tag)
    open(path, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
def fix(fn):
    p = 'build/lbauto/%s.c' % fn
    if not os.path.exists(p): return fn, 'nosrc'
    src = open(p).read()
    m = re.search(r'^([\w\*\s]+\b%s\()([^\n]*)(\) \{\n)' % re.escape(fn), src, re.M)
    body_start = m.end()
    body = src[body_start:]
    sites = [x for x in re.finditer(r'\b([A-Za-z_]\w*)\(', body) if x.group(1) not in KW and not re.match(r'(s8|s16|s32|u8|u16|u32|int|f32|char)$', x.group(1))]
    sites = sites[:5]
    best = run(fn, src, 'b')
    if best is None: return fn, 'err'
    if best == 0: return fn, 'OK'
    bestsrc = src
    params = [x.strip() for x in m.group(2).split(',') if x.strip() and x.strip() != 'void']
    have = {int(re.search(r'arg(\d)$', x).group(1)) for x in params if re.search(r'arg(\d)$', x)}
    opts = [None] + ['arg0', 'arg1', 'arg2', 'arg3']
    tried = 0
    for combo in itertools.product(range(len(opts)), repeat=len(sites)):
        if not any(combo): continue
        if tried > 40: break
        tried += 1
        b2 = body; shift = 0; used = set()
        for site, c in zip(sites, combo):
            if c == 0: continue
            a = opts[c]; used.add(int(a[3:]))
            pos = site.end() + shift
            ins = a + (', ' if b2[pos] != ')' else '')
            b2 = b2[:pos] + ins + b2[pos:]; shift += len(ins)
        need = sorted(set(range(max(used | have) + 1)) - have) if (used | have) else []
        allp = sorted(have | set(need))
        keep = {int(re.search(r'arg(\d)$', x).group(1)): x for x in params if re.search(r'arg(\d)$', x)}
        newp = ', '.join(keep.get(k, 'int arg%d' % k) for k in range(max(used | have) + 1))
        s2 = src[:m.start()] + m.group(1) + newp + m.group(3) + b2
        r = run(fn, s2, 'v%d' % tried)
        if r is not None and r < best:
            best = r; bestsrc = s2
            if r == 0: break
    if bestsrc is not src: open(p, 'w').write(bestsrc)
    return fn, 'OK' if best == 0 else 'd%d' % best
if __name__ == '__main__':
    names = sys.argv[1:]
    with cf.ThreadPoolExecutor(3) as ex:
        for fn, st in ex.map(fix, names):
            print(fn, st, flush=True)
