#!/usr/bin/env python3
"""lbbsc.py NAME... : rewrite `(idx * 0x5C) + base` address arithmetic in build/lbauto/NAME.c as struct-array
indexing (BSC macro / &((BSCELL *)base)[idx]) which gives the original `addu idx*size, base` operand order; checks each."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def bal(s, i):
    d = 0
    for j in range(i, len(s)):
        if s[j] == '(': d += 1
        elif s[j] == ')':
            d -= 1
            if d == 0: return j
def conv(s):
    # F(T, ((IDX * 0x5C) + BASE), D)
    out = ''; i = 0
    while True:
        j = s.find('F(', i)
        if j < 0: out += s[i:]; break
        e = bal(s, j + 1)
        inner = s[j + 2:e]
        m = re.match(r'(\w+), \(\((.*) \* (0x5C|2|4|8)\) \+ (\w+)\), (0x[0-9A-Fa-f]+|\d+)$', inner, re.S)
        if m:
            out += s[i:j] + 'BSC%s(%s, %s, %s, %s)' % ({'0x5C':'','2':'2','4':'4','8':'8'}[m.group(3)], m.group(1), m.group(4), m.group(2), m.group(5)); i = e + 1
        else:
            out += s[i:j + 2]; i = j + 2
    # temp = (IDX * 0x5C) + BASE;
    for m in list(re.finditer(r'\n\s*(\w+) = \((.*?) \* 0x5C\) \+ (\w+);', out)):
        x, idx, base = m.group(1), m.group(2), m.group(3)
        uses = re.findall(r'\b%s\b' % x, out)
        fu = re.findall(r'F\(\w+, %s, ' % x, out)
        if len(uses) - 1 == len(fu) + len(re.findall(r'(?:int|s32|u32) %s;' % x, out)):
            out = out.replace(m.group(0), '')
            out = re.sub(r'F\((\w+), %s, (0x[0-9A-Fa-f]+|\d+)\)' % x, lambda q: 'BSC(%s, %s, %s, %s)' % (q.group(1), base, idx, q.group(2)), out)
    return out
for n in sys.argv[1:]:
    p = 'build/lbauto/%s.c' % n
    s = open(p).read()
    t = conv(s)
    for _ in range(3): t = conv(t)
    if t == s: print(n, 'unchanged'); continue
    q = 'src/lobby/zz_bsc_%s.c' % n
    open(q, 'w').write(t)
    out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(n), out, re.M)
    print(n, m.group(1) if m else 'ERR: ' + out[-200:], m.group(2) if m else '')
    if m and (m.group(1) == 'OK' or True): open(p, 'w').write(t) if m.group(1) == 'OK' else open('build/lbauto/%s.bsc.c' % n, 'w').write(t)
