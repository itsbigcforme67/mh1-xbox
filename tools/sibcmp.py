#!/usr/bin/env python3
"""sibcmp.py LO_A HI_A PFX_A LO_B HI_B PFX_B : compare same-role functions of two sibling monsters in game.bin
(e.g. 0x566630 0x57AB60 em01 0x5EBA10 0x5FCDC0 em20). Functions are paired by name without the monster prefix and the
address suffix. Prints per pair: SAME (identical opcode sequence, only immediates/offsets/call targets differ), or
the number of instructions whose opcode differs, and the sizes."""
import sys, os, re, struct, csv
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from mips_dis import dis
lo_a, hi_a, pa, lo_b, hi_b, pb = sys.argv[1:7]
lo_a, hi_a, lo_b, hi_b = [int(x, 16) for x in (lo_a, hi_a, lo_b, hi_b)]
data = open(os.path.join(ROOT, 'disc/mh1/split/game.bin'), 'rb').read()
vram = struct.unpack_from('<I', data, 8)[0]
rows = [r for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mh1_symbols.csv'), encoding='utf-8'))
        if r['type'] == 'FUNC' and r['section'] == 'game.bin']
def funcs(lo, hi, pre):
    d = {}
    for r in rows:
        a = int(r['addr'], 16)
        if lo <= a < hi:
            k = re.sub(r'_[0-9A-F]{6,8}$', '', r['name'])
            if k.startswith(pre + '_'): k = k[len(pre) + 1:]
            d[k] = (a, int(r['size']), r['name'])
    return d
A = funcs(lo_a, hi_a, pa); B = funcs(lo_b, hi_b, pb)
def ops(a, n):
    out = []
    for i in range(n // 4):
        w = struct.unpack_from('<I', data, a - vram + 4 * i)[0]
        t = dis(w, a + 4 * i)
        out.append(t.split()[0] if t.strip() else '?')
    return out
same = 0; tot = 0
for k in sorted(A, key=lambda x: A[x][0]):
    if k not in B: continue
    oa, ob = ops(A[k][0], A[k][1]), ops(B[k][0], B[k][1])
    tot += 1
    if oa == ob:
        same += 1; print('SAME %-28s %4d instrs' % (k, len(oa)))
    else:
        d = sum(1 for x, y in zip(oa, ob) if x != y)
        print('diff %-28s %4d vs %4d instrs, %d opcodes differ' % (k, len(oa), len(ob), d))
print(same, 'of', tot, 'pairs identical in opcodes')
