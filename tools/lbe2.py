#!/usr/bin/env python3
"""lbe2.py NAME... : second-chance transforms for auto drafts that did not compile (build/lbauto_err/NAME.c ->
build/lbauto_e2/NAME.c): void * -> u8 *, then retry. Reports the first compiler error or the diff."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def chk(fn, src):
    path = 'src/lobby/zz_e2_%s.c' % fn
    open(path, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True); out = out.stdout + out.stderr
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    if m: return ('OK' if m.group(1) == 'OK' else 'd%s' % m.group(2)), out
    return 'err', out
for fn in sys.argv[1:]:
    src = open('build/lbauto_err/%s.c' % fn).read()
    s2 = re.sub(r'\bvoid \*', 'u8 *', src)
    s2 = re.sub(r'\bs64 (\w+);', r'long long \1;', s2)
    s2 = re.sub(r'\bs128 (\w+);', r'unsigned __int128 \1;', s2)
    s2 = re.sub(r'(\w+) = \(u8 \*\)cw;', r'\1 = (int)cw;', s2)
    s2 = re.sub(r'(\w+) = pNet;', r'\1 = (int)pNet;', s2)
    for it in range(8):
        st, out = chk(fn, s2)
        if st != 'err': break
        ch = False
        for m in re.finditer(r"identifier '(\w+)(?:\([^)]*\))?' redeclared", out):
            n = m.group(1)
            t = re.sub(r'^extern [^\n(]*\b%s\b[^\n(]*;\n' % n, '', s2, flags=re.M)
            t = re.sub(r'^(?:int|void|s32|u8) %s\(\);\n' % n, '', t, flags=re.M)
            if t != s2: s2 = t; ch = True
        if not ch: break
    if st == 'err':
        e = [l for l in out.split('\n') if l.startswith('#') and not l.startswith('# ---') and 'File:' not in l and 'mwcc' not in l][:3]
        print(fn, 'err', ' | '.join(x.strip('# ') for x in e)); 
        open('build/lbauto_e2/%s.c' % fn, 'w').write(s2)
    else:
        open('build/lbauto_e2/%s.c' % fn, 'w').write(s2)
        print(fn, st)
