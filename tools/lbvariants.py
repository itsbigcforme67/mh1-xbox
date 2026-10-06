#!/usr/bin/env python3
"""lbvariants.py FILE FUNC < variants : each variant (separated by a line '=====') replaces the whole function FUNC in a temp copy of FILE;
prints the check.py score per variant (OK or '(n/m instructions differ)'). K&R headers are fine."""
import sys, re, subprocess, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f, fn = sys.argv[1:3]
src = open(f).read()
m = re.search(r'^[A-Za-z_][^\n;]*\b%s\([^{]*\{' % re.escape(fn), src, re.M)
a = m.start(); i = src.index('{', m.start()); d = 0
for j in range(i, len(src)):
    if src[j] == '{': d += 1
    elif src[j] == '}':
        d -= 1
        if d == 0: b = j + 1; break
for k, v in enumerate(sys.stdin.read().split('\n=====\n')):
    tmp = f.replace('.c', '_zzt.c')
    open(tmp, 'w').write(src[:a] + v.strip() + src[b:])
    try: out = subprocess.run(['python3', 'tools/check.py', tmp, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally: os.remove(tmp)
    l = [x for x in out.split('\n') if ' %s ' % fn in x]
    print(k, (l[0][:2] + ' ' + (re.search(r'\(.*\)', l[0]) or [''])[0]) if l else 'ERR ' + out[-300:])
