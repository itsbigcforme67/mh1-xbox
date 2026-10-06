#!/usr/bin/env python3
"""lbglob.py FILE... : in an int-mode near-match draft, access the struct pointer globals cw and pNet through per-file overlay structs
(`F(T, v, off)` with v = (int)cw / cw / (u8 *)cw -> `((CWS *)cw)->xOFF`; same for pNet), as the original code does. Keeps the rewrite only
if the file still compiles and check.py's total diff does not get worse."""
import re, sys, subprocess
SZ = {'u8': 1, 's8': 1, 'char': 1, 'u16': 2, 's16': 2, 's32': 4, 'u32': 4, 'int': 4}
def cnt(p):
    r = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = r.stdout + r.stderr
    if 'Error' in out: return 10**9
    return sum(int(x) for x in re.findall(r'^--\s+\S+\s+lobby\s+0x\w+\s+\d+ bytes\s+\((\d+)/', out, re.M))
def convert(s, B, tag):
    aliases = set(re.findall(r'\b(\w+) = (?:\(u8 \*\)|\(int\)|\(void \*\))?%s;' % B, s))
    bases = {B, '(u8 *)' + B, '(int)' + B} | aliases
    used = {}
    def rep(m):
        T, base, off = m.group(1), m.group(2).strip(), int(m.group(3), 0)
        if base in bases and T in SZ and off not in used or (base in bases and used.get(off) == T):
            used[off] = T
            return '((%s *)%s)->x%04X' % (tag, B, off)
        return m.group(0)
    t = re.sub(r'F\((\w+), ([^,()]+(?: \*\))?[^,()]*), (0x[0-9A-Fa-f]+|\d+)\)', rep, s)
    if not used: return s
    offs = sorted(used); fields = []; pos = 0
    for o in offs:
        T = used[o]; z = SZ[T]
        if o < pos or o % z: return s
        if o > pos: fields.append('u8 pad%04X[0x%X];' % (pos, o - pos))
        fields.append('%s x%04X;' % (T, o)); pos = o + z
    for v in aliases:
        t = re.sub(r'^\s*%s = (?:\(u8 \*\)|\(int\)|\(void \*\))?%s;\n' % (v, B), '', t, flags=re.M)
        t = re.sub(r'^\s*(?:int|s32|void \*|u8 \*) \*?%s;\n' % v, '', t, flags=re.M)
        t = re.sub(r'\b%s\b' % v, '(int)%s' % B, t)
    td = 'typedef struct { %s } %s;\n' % (' '.join(fields), tag)
    i = t.index('\n', t.index('#include')) + 1
    return t[:i] + td + t[i:]
for p in sys.argv[1:]:
    s = open(p).read(); b = cnt(p); t = s
    for B, tag in (('cw', 'GS_CW'), ('pNet', 'GS_PNET')):
        t2 = convert(t, B, tag)
        if t2 != t:
            open(p, 'w').write(t2)
            n = cnt(p)
            if n <= b: t, b = t2, n
            else: open(p, 'w').write(t)
    open(p, 'w').write(t)
    print(p, cnt(p))
