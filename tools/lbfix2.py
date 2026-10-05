#!/usr/bin/env python3
"""lbfix2.py NAME...|--all : second repair pass over build/lbauto/NAME.err.c (auto drafts that did not compile).
Mechanical fixes: `void *` locals -> `u8 *`, `&SYM` of extern arrays used as pointers -> `(u8 *)SYM`, stray saved_reg_* reads -> 0.
Writes the result to build/lbauto/NAME.fix2.c and reports OK / diff / error (message)."""
import re, subprocess, sys, os, json, glob, concurrent.futures as cf
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def run(n, src):
    q = 'src/lobby/zz_f2_%s.c' % n
    open(q, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/(\d+))?' % re.escape(n), out, re.M)
    if m: return ('OK',) if m.group(1) == 'OK' else ('diff', int(m.group(2)), int(m.group(3)))
    return ('err', out.strip().split('\n')[3:7])
def fix(src):
    syms = re.findall(r'^extern (?:char|u8) (\w+)\[\];', src, re.M)
    src = re.sub(r'^extern char (\w+)\[\];', r'extern u8 \1[];', src, flags=re.M)
    src = re.sub(r'\bvoid \*(\w+);', r'u8 *\1;', src)
    src = re.sub(r'\bvoid \*(\w+) =', r'u8 *\1 =', src)
    for s in set(syms):
        src = re.sub(r'(?<!\(int\))(?<!\(u8 \*\))&%s\b(?!\s*\[)' % s, '(u8 *)%s' % s, src)
        src = re.sub(r'\(int\)&%s\b' % s, '(int)%s' % s, src)
    src = re.sub(r'\b(saved_reg_s\d)\b', '0', src)
    src = re.sub(r'\bextern int (\w+);\n(.*?)\b\1\b', lambda m: m.group(0), src, flags=re.S)
    return src
def one(n):
    p = 'build/lbauto/%s.err.c' % n
    if not os.path.exists(p): return n, ('nosrc',)
    s = fix(open(p).read())
    r = run(n, s)
    open('build/lbauto/%s.fix2.c' % n, 'w').write(s)
    return n, r
names = sys.argv[1:]
if names == ['--all']:
    names = [os.path.basename(f)[:-6] for f in glob.glob('build/lbauto/*.err.c')]
res = {}
with cf.ThreadPoolExecutor(2) as ex:
    for n, r in ex.map(one, names):
        res[n] = r
        print(n, r[0], r[1:] if r[0] != 'err' else ' | '.join(x.strip() for x in r[1])[:140], flush=True)
json.dump(res, open('build/lbfix2.json', 'w'))
