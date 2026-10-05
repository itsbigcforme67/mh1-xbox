#!/usr/bin/env python3
"""lbe2.py NAME... : second-chance transforms for auto drafts that did not compile (build/lbauto_err/NAME.c ->
build/lbauto_e2/NAME.c): void * -> u8 *, then retry. Reports the first compiler error or the diff."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def jtcalls(s):
    # *((int)&TBL + REST): table read. REST = (IDX * 4) -> ((int *)&TBL)[IDX] / call ((int (**)())&TBL)[IDX]();
    # otherwise a byte (when compared) or word read at (u8 *)&TBL + REST
    out = ''; i = 0
    while True:
        j = s.find('*((int)&', i)
        if j < 0: out += s[i:]; break
        out += s[i:j]
        m = re.match(r'\*\(\(int\)&(\w+) \+ ', s[j:])
        if not m: out += s[j]; i = j + 1; continue
        k = j + 1; d = 0
        while k < len(s):
            if s[k] == '(': d += 1
            elif s[k] == ')':
                d -= 1
                if d == 0: break
            k += 1
        rest = s[j + m.end():k]
        k += 1
        nxt = s[k:k + 1]
        mm = re.match(r'^\((.*) \* 4\)$', rest)
        if mm and mm.group(1).count('(') == mm.group(1).count(')'):
            ty = 'int (**)()' if nxt == '(' else 'int *'
            out += '((%s)&%s)[%s]' % (ty, m.group(1), mm.group(1)); i = k
        else:
            ty = 's8' if re.match(r' [!=]= ', s[k:k + 4]) else 'int'
            out += '(*(%s *)((u8 *)&%s + %s))' % (ty, m.group(1), rest); i = k
    return out
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
    s2 = jtcalls(s2)
    s2 = re.sub(r'^extern [^\n]*\b(?:s64|u64|int32|s128)\b[^\n]*\n', '', s2, flags=re.M)
    def _fix(l):
        if l.startswith('extern '): return l
        l = re.sub(r'\bs64\b', 'long long', l)
        l = re.sub(r'\bu64\b', 'unsigned long long', l)
        l = re.sub(r'(?<![\w(])SearchResult\b', '(int)SearchResult', l); l = l.replace('*(int)SearchResult', '*(u8 *)SearchResult')
        l = re.sub(r'\bint32\b', 'int', l)
        return l
    s2 = '\n'.join(_fix(l) for l in s2.split('\n'))
    for it in range(8):
        st, out = chk(fn, s2)
        if st != 'err': break
        ch = False
        for m in re.finditer(r"identifier '(\w+)(?:\([^)]*\))?' redeclared", out):
            n = m.group(1)
            t = re.sub(r'^extern [^\n(]*\b%s\b[^\n(]*;\n' % n, '', s2, flags=re.M)
            t = re.sub(r'^(?:int|void|s32|u8) %s\(\);\n' % n, '', t, flags=re.M)
            if t != s2: s2 = t; ch = True
        for m in re.finditer(r"#\s+(\d+): (\w+) = [^\n]*\n#\s+Error:[^\n]*\n#\s+illegal operands 'char\[\]' = 'int'", out):
            ln, nm_ = int(m.group(1)), m.group(2)
            L = s2.split('\n')
            # line numbers are those of the written file (same as s2)
            if 0 < ln <= len(L) and re.match(r'\s*%s = ' % nm_, L[ln-1]):
                L[ln-1] = L[ln-1].replace(nm_ + ' = ', nm_ + '[0] = ', 1); s2 = '\n'.join(L); ch = True
        if not ch: break
    if st == 'err':
        e = [l for l in out.split('\n') if l.startswith('#') and not l.startswith('# ---') and 'File:' not in l and 'mwcc' not in l][:3]
        print(fn, 'err', ' | '.join(x.strip('# ') for x in e)); 
        open('build/lbauto_e2/%s.c' % fn, 'w').write(s2)
    else:
        open('build/lbauto_e2/%s.c' % fn, 'w').write(s2)
        print(fn, st)
