#!/usr/bin/env python3
"""semdiff.py FILE.c [FUNC ...]: screen the near-match functions of FILE.c for REAL differences from the original.
Runs tools/check.py -v and compares the instructions of the original and of our compile as multisets after
erasing register names, relocation targets, branch targets and nops. A function whose multisets are equal differs
only by register allocation / scheduling / block order ("equivalent modulo allocation"); anything else lists the
instructions only one side has (a different constant, field offset, compare, extra or missing operation).
This is a screen for behaviour bugs in near-match copies, not a proof. Standard library only."""
import sys, subprocess, re, collections
f = sys.argv[1]
want = set(sys.argv[2:])
out = subprocess.run(['python3', 'tools/check.py', f, '-v'], capture_output=True, text=True).stdout.split('\n')

def norm(s):
    s = re.sub(r'\s+', ' ', s.replace('(reloc)', '')).strip()
    if not s or s == 'nop':
        return None
    op = s.split(' ')[0]
    if op in ('jal', 'j'):
        return op
    if op == 'jr' or op == 'jalr':
        return s
    if op == 'lui' or '(at)' in s or '(gp)' in s or ', gp,' in s:
        s = re.sub(r'-?0x[0-9A-Fa-f]+|-?\d+(?=\(|$)', 'N', s)
    elif op.startswith('b') or op in ('beq', 'bne'):
        s = re.sub(r'0x[0-9A-F]{8}', 'ADDR', s)
    elif op == 'addiu' and re.search(r', (-?\d+)$', s) and abs(int(re.search(r', (-?\d+)$', s).group(1))) >= 4096:
        s = re.sub(r'-?\d+$', 'N', s)
    s = re.sub(r'(addiu \w+, \w+), 0$', r'\1, N', s)
    # all pure moves / sign extensions are scheduling-sensitive: keep opcode only for daddu x,y,zero
    s = re.sub(r'\$f\d+', 'F', s)
    s = re.sub(r'\b(zero|at|v[01]|a[0-3]|t[0-9]|s[0-7]|gp|sp|fp|ra|k[01])\b', 'R', s)
    # branch direction is block order; a conditional branch is kept as op with its operand registers erased
    s = s.replace('beq R, R, ADDR', 'beq R, R, ADDR')
    return s

cur = None
res = {}
for l in out:
    m = re.match(r'^(OK|--)\s+(\S+)\s', l)
    if m:
        cur = m.group(2); res[cur] = ([], []); continue
    if cur and '|' in l:
        a, b = l.split('|', 1)
        mm = re.match(r'\s*(>>)?\s*([0-9A-F]{8})\s+(.*)', a)
        if mm:
            x = norm(mm.group(3))
            if x: res[cur][0].append(x)
        y = norm(b)
        if y: res[cur][1].append(y)
for fn, (a, b) in res.items():
    if want and fn not in want: continue
    ca, cb = collections.Counter(a), collections.Counter(b)
    miss = ca - cb      # original has, we do not
    extra = cb - ca     # we have, original does not
    # relocated address pairs (lui/addiu of a symbol): ours shows N, the original its literal; cancel them
    nN = extra.get('addiu R, R, N', 0)
    if nN:
        lits = [k for k in miss if re.match(r'addiu R, R, -?\d+$', k)]
        for k in sorted(lits, key=lambda k: -abs(int(k.split(', ')[-1]))):
            while nN and miss[k] > 0:
                miss[k] -= 1; nN -= 1
        extra['addiu R, R, N'] = nN
        miss = +miss; extra = +extra
    if not miss and not extra:
        print('EQUIV    %s (%d instr)' % (fn, len(a)))
    else:
        print('DIFFERS  %s: original-only %d, ours-only %d' % (fn, sum(miss.values()), sum(extra.values())))
        for k, v in sorted(miss.items()): print('    orig  %dx %s' % (v, k))
        for k, v in sorted(extra.items()): print('    ours  %dx %s' % (v, k))
