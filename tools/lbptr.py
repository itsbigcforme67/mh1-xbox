#!/usr/bin/env python3
"""lbptr.py NAME... : in build/lbauto/NAME.c, for `F(T, X, OFF) = v;` stores to a field of X that is also read earlier via
F(T, X, OFF), declare `T *pN = &F(T, X, OFF);` right after `X = ...;` and store through it (MWCC then keeps the address in a
register: addiu p,x,OFF; lhu OFF(x); ...; sh 0(p)). Verifies; writes OK results back."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
for n in sys.argv[1:]:
    p = 'build/lbauto/%s.c' % n; s = open(p).read()
    m = None
    for m in re.finditer(r'\n    (\w+) = (\w+);\n', s): pass
    st = re.search(r'\n(\s+)F\((\w+), (\w+), (0x[0-9A-Fa-f]+)\) = ([^;]+);', s[s.find('{'):])
    best = None
    for st in re.finditer(r'\n(\s+)F\((\w+), (\w+), (0x[0-9A-Fa-f]+)\) = ([^;]+);', s):
        T, X, off = st.group(2), st.group(3), st.group(4)
        if re.search(r'F\(%s, %s, %s\)' % (T, X, off), s[:st.start()]):
            decl = '\n    %s *pp;' % T
            a = re.search(r'\n    %s = [^;]+;\n' % X, s)
            if not a: continue
            t = s[:st.start()] + '\n%s*pp = %s' % (st.group(1), st.group(5)) if False else None
            t = s[:a.end()] + '    pp = &F(%s, %s, %s);\n' % (T, X, off) + s[a.end():st.start()] + '\n%s*pp = %s;' % (st.group(1), st.group(5)) + s[st.end():]
            t = t.replace('{\n', '{\n    %s *pp;' % T, 1).replace('{\n    %s *pp;' % T, '{\n    %s *pp;\n' % T, 1)
            best = t; break
    if not best: print(n, 'nopattern'); continue
    q = 'src/lobby/zz_ptr_%s.c' % n
    open(q, 'w').write(best)
    out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(n), out, re.M)
    print(n, m.group(1) if m else 'ERR ' + out[-300:], m.group(2) if m else '')
    if m and m.group(1) == 'OK': open(p, 'w').write(best)
