#!/usr/bin/env python3
"""sigdiff.py FILE.c [FUNC ...]: coarser cousin of semdiff.py for functions whose code is restructured (so semdiff is noisy).
Compares multisets of (a) float arithmetic/compare opcodes, (b) immediates of addi/slti/andi/ori/lui (not sp-relative),
(c) memory offsets of loads and stores (width classes merged: lw/lwc1 = W, lh/lhu = H, lb/lbu = B) off non-sp bases.
Prints what only the original / only our compile has. Equal sets do not prove equivalence; a difference points at a
constant, offset or float operation that exists on only one side (a likely real behaviour difference)."""
import sys, subprocess, re, collections
f = sys.argv[1]; want = set(sys.argv[2:])
out = subprocess.run(['python3', 'tools/check.py', f, '-v'], capture_output=True, text=True).stdout.split('\n')
def toks(s):
    s = re.sub(r'\s+', ' ', s.replace('(reloc)', '')).strip()
    if not s or s == 'nop': return []
    op = s.split(' ')[0]; r = []
    if re.match(r'(add|sub|mul|div|sqrt|abs|neg|c\.\w+|madd|msub|mula|cvt|trunc|min|max|rsqrt)\.s', op) or op.startswith(('cvt', 'trunc')):
        return ['F ' + op]
    m = re.match(r'(l|s)(wc1|w|hu|h|bu|b|d|q|wl|wr)\s+\S+, (-?\d+)\((\w+)\)', s)
    if m:
        if m.group(4) == 'sp': return []
        w = {'wc1': 'W', 'w': 'W', 'hu': 'H', 'h': 'H', 'bu': 'B', 'b': 'B'}.get(m.group(2), m.group(2))
        return ['%s %s %s' % ('LD' if m.group(1) == 'l' else 'ST', w, m.group(3))]
    m = re.match(r'(addiu|daddiu|slti|sltiu|andi|ori|xori) (\w+), (\w+), (-?\w+)$', s)
    if m:
        if m.group(3) == 'sp' or m.group(2) == 'sp': return []
        v = m.group(4)
        try: v = int(v, 0)
        except: pass
        if op in ('addiu', 'daddiu') and v in (0,): return []
        if op in ('slti', 'sltiu'): op = 'slti'
        if op == 'daddiu': op = 'addiu'
        return ['IMM %s %s' % (op, v)]
    m = re.match(r'lui \w+, (0x[0-9A-Fa-f]+)$', s)
    if m and int(m.group(1), 16) >= 0x100: return ['LUI ' + m.group(1)]
    if op in ('jal', 'j') or op.startswith('b'): return []
    return []
sides = {}
cur = None
for l in out:
    m = re.match(r'^(OK|--)  (\S+)', l)
    if m:
        cur = m.group(2); sides[cur] = (collections.Counter(), collections.Counter()); continue
    if cur and '|' in l:
        a, b = l.split('|', 1)
        a = re.sub(r'^\s*(>>)?\s*[0-9A-F]{8}\s+', '', a)
        for t in toks(a): sides[cur][0][t] += 1
        for t in toks(b): sides[cur][1][t] += 1
def split(c):
    fl = collections.Counter(); nums = set()
    for k, v in c.items():
        if k.startswith('F '): fl[k] = v
        else:
            m = re.match(r'(?:LD|ST) \w (-?\d+)$', k)
            if m:
                if int(m.group(1)) != 0: nums.add(int(m.group(1)))
                continue
            m = re.match(r'IMM (\w+) (-?\d+)$', k)
            if m: nums.add(int(m.group(2))); continue
            nums.add(k.split(' ', 1)[1])
    return fl, nums
for n, (o, u) in sides.items():
    if want and n not in want: continue
    fo, no = split(o); fu, nu = split(u)
    do = fo - fu; du = fu - fo
    xo = sorted(no - nu, key=str); xu = sorted(nu - no, key=str)
    if not do and not du and not xo and not xu: print('SAME    ', n); continue
    print('DIFFERS ', n)
    if do or du: print('    float ops orig-only: %s  ours-only: %s' % (dict(do), dict(du)))
    if xo or xu: print('    numbers orig-only: %s\n    numbers ours-only: %s' % (xo, xu))
