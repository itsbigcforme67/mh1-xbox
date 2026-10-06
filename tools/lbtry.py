#!/usr/bin/env python3
"""lbtry.py [-j N] NAME... : greedy source-form search on the near-matching auto drafts build/lbauto/NAME.c.
Tries, one at a time and keeping a change only when the number of differing instructions drops:
  - passing the function's own parameters through at the start of each call (K&R stale argument registers: `f(a)` -> `f(arg0, a)`, `f(arg0, arg1, a)`),
  - flipping each comparison (`a >= b` <-> `b <= a`, `a > b` <-> `b < a`, ...),
  - `x ? 1 : 0` wrappers on `return a != b;`-style returns,
  - every order of up to 5 local declarations (declbf).
Standard library only; scratch copies live in build/scr (never in src/)."""
import itertools, os, re, subprocess, sys, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
KW = {'if', 'while', 'switch', 'for', 'return', 'sizeof', 'F', 'case'}

def score(fn, src, tag):
    os.makedirs('build/scr', exist_ok=True)
    path = 'build/scr/try_%s_%s.c' % (re.sub(r'\W', '_', fn), tag)
    open(path, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    if not m:
        return 99999
    return 0 if m.group(1) == 'OK' else int(m.group(2))

def variants(fn, src):
    m = re.search(r'^([\w\*\s]+\b%s\()([^\n]*)(\) \{\n)' % re.escape(fn), src, re.M)
    if not m:
        return
    params = [p.strip().split()[-1].strip('*') for p in m.group(2).split(',') if p.strip() and p.strip() != 'void']
    start = m.end()
    body = src[start:]
    # 1. pass-through arguments
    for cm in re.finditer(r'\b([A-Za-z_]\w*)\(', body):
        if cm.group(1) in KW or cm.group(1) == fn:
            continue
        for k in range(1, min(3, len(params)) + 1):
            ins = ', '.join(params[:k]) + (', ' if body[cm.end()] != ')' else '')
            if body[cm.end():].startswith(params[0] + ','):
                continue
            yield src[:start] + body[:cm.end()] + ins + body[cm.end():]
    # 2. comparison flips
    for cm in re.finditer(r'(\(|\|\| |&& )([\w\.\*\[\]]+) (>=|<=|>|<) ([\w\.\*\[\]]+)(\)| \|\||\s&&)', body):
        a, op, b = cm.group(2), cm.group(3), cm.group(4)
        flip = {'>=': '<=', '<=': '>=', '>': '<', '<': '>'}[op]
        yield src[:start] + body[:cm.start(2)] + '%s %s %s' % (b, flip, a) + body[cm.end(4):]
    # 3. return a != b / == -> ? 1 : 0
    for cm in re.finditer(r'return (\(?[^;\n]*?(?:!=|==)[^;\n]*?\)?);', body):
        if '?' in cm.group(1):
            continue
        yield src[:start] + body[:cm.start()] + 'return %s ? 1 : 0;' % cm.group(1) + body[cm.end():]
    # 4. declaration order
    dm = re.match(r'((?:    [^\n;(]+;\n)+)', body)
    if dm:
        decls = [l for l in dm.group(1).split('\n') if l.strip()]
        if 2 <= len(decls) <= 5:
            rest = body[dm.end():]
            for perm in itertools.permutations(decls):
                if list(perm) != decls:
                    yield src[:start] + '\n'.join(perm) + '\n' + rest

def work(fn):
    p = 'build/lbauto/%s.c' % fn
    if not os.path.exists(p):
        return fn, 'nosrc', 0
    src = open(p).read()
    best = score(fn, src, 'base')
    start = best
    improved = True
    rounds = 0
    while improved and best > 0 and rounds < 4:
        improved = False
        rounds += 1
        for i, v in enumerate(variants(fn, src)):
            s = score(fn, v, str(i % 50))
            if s < best:
                best, src, improved = s, v, True
                if best == 0:
                    break
    if best < start:
        open(p, 'w').write(src)
    return fn, best, start

if __name__ == '__main__':
    args = sys.argv[1:]
    jobs = 3
    if args and args[0] == '-j':
        jobs = int(args[1]); args = args[2:]
    with cf.ThreadPoolExecutor(jobs) as ex:
        for fn, best, start in ex.map(work, args):
            print('%s %s -> %s' % (fn, start, 'OK' if best == 0 else best), flush=True)
