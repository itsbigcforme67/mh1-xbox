#!/usr/bin/env python3
"""flipcmp.py FILE FUNC: try swapping the operands of each relational operator (a < b -> b > a) and of each
`+` / `==` / `!=` between simple operands in FUNC, one site at a time, then pairs; keep the best by
tools/check.py. Writes the best variant back only if it is strictly better. Cheap operand-order search for the
slt at/v1 and addu operand-order differences. Usage: tools/flipcmp.py src/main/x/y_nm.c Func"""
import re, subprocess, sys, itertools

path, func = sys.argv[1], sys.argv[2]
src = open(path).read()
m = re.search(r'\n(?:static )?[\w \*]+\b%s\([^;{]*\)\s*\{' % re.escape(func), src)
if not m:
    m = re.search(r'\n(?:static )?[\w \*]+\b%s\([^;{]*\)\s*\n\s*\{' % re.escape(func), src)
start = m.start()
# end of function: first "\n}\n" after start
end = src.index("\n}\n", start) + 2
body = src[start:end]
FLIP = {'<': '>', '>': '<', '<=': '>=', '>=': '<=', '==': '==', '!=': '!=', '+': '+'}
STOPL = set('(,;{}=?:')
def operands(b, i, oplen):
    # left operand
    j = i - 1
    depth = 0
    while j >= 0:
        c = b[j]
        if c in ')]': depth += 1
        elif c in '([':
            if depth == 0: break
            depth -= 1
        elif depth == 0 and (c in ',;{}?:' or b[j-1:j+1] in ('&&', '||') or (c == '=' and b[j-1] not in '<>=!+-*/&|%^' and b[j+1] != '=') or c == '!' and False):
            break
        j -= 1
    l0 = j + 1
    k = i + oplen
    depth = 0
    while k < len(b):
        c = b[k]
        if c in '([': depth += 1
        elif c in ')]':
            if depth == 0: break
            depth -= 1
        elif depth == 0 and (c in ',;{}?:' or b[k:k+2] in ('&&', '||')):
            break
        k += 1
    return l0, k
sites = []
for mm in re.finditer(r' (<=|>=|==|!=|<|>|\+) ', body):
    op = mm.group(1)
    i = mm.start() + 1
    l0, r1 = operands(body, i, len(op))
    L = body[l0:i - 1].strip(); R = body[i + len(op) + 1:r1].strip()
    if not L or not R or '\n' in L or '\n' in R: continue
    if re.search(r'\b(if|while|for|return)\b', L): 
        L2 = re.split(r'\b(?:if|while|for|return)\b\s*\(?', L)[-1].strip()
        if not L2: continue
    sites.append((l0, i, len(op), r1, op, L, R))
def apply(sel):
    b = body
    for (l0, i, ol, r1, op, L, R) in sorted(sel, reverse=True):
        left = b[l0:i - 1]; right = b[i + ol + 1:r1]
        lead = left[:len(left) - len(left.lstrip())]
        trail = right[len(right.rstrip()):]
        new = lead + right.strip() + ' ' + FLIP[op] + ' ' + left.strip() + trail
        b = b[:l0] + new + b[r1:]
    return src[:start] + b + src[end:]
def score(text):
    open(path, 'w').write(text)
    out = subprocess.run(['python3', 'tools/check.py', path], capture_output=True, text=True).stdout
    for l in out.splitlines():
        if (' %s ' % func) in l:
            if l.startswith('OK'): return 0
            mm = re.search(r'\((\d+)/', l)
            return int(mm.group(1))
    return 99999
base = score(src)
print('base', base, 'sites', len(sites)); sys.stdout.flush()
best = (base, src)
try:
    for n in ((1, 2) if len(sites) <= 14 else (1,)):
        for sel in itertools.combinations(range(len(sites)), n):
            s = score(apply([sites[k] for k in sel]))
            if s < best[0]:
                best = (s, apply([sites[k] for k in sel]))
                print('better', s, [sites[k][5] + ' ' + sites[k][4] + ' ' + sites[k][6] for k in sel]); sys.stdout.flush()
            if best[0] == 0: break
        if best[0] == 0: break
finally:
    open(path, 'w').write(best[1])
print('final', best[0])
