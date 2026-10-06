#!/usr/bin/env python3
"""permcases.py FILE FUNC [INDENT=4]: brute-force the order of the case blocks of the (first) switch at the given indentation in FUNC
(blocks start at `<indent>case X:` / `default:` lines; consecutive labels with an empty body stay together). Keeps the best order."""
import itertools, re, subprocess, sys
p, func = sys.argv[1:3]
ind = ' ' * (int(sys.argv[3]) if len(sys.argv) > 3 else 4)
occ = int(sys.argv[4]) if len(sys.argv) > 4 else 0
s0 = open(p).read()
a = s0.index(func + '(')
sws = list(re.compile(r'^%sswitch .*\{.*\n' % ind, re.M).finditer(s0, a))
sw = sws[occ]
body_start = sw.end()
end = re.compile(r'^%s\}\n' % ind, re.M).search(s0, body_start).start()
body = s0[body_start:end]
lines = body.split('\n')
blocks = []; cur = []
lab = re.compile(r'^%s(case .*|default):\s*(/\*.*\*/)?$' % ind)
prev_label = False
for l in lines[:-1]:
    if lab.match(l):
        if cur and not prev_label: blocks.append(cur); cur = []
        prev_label = True
    elif l.strip(): prev_label = False
    cur.append(l)
if cur: blocks.append(cur)
print(len(blocks), 'blocks')
if len(blocks) > 7 or len(blocks) < 2: sys.exit()
def score(s):
    open(p, 'w').write(s)
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True).stdout
    mm = re.search(r'^(--|OK)\s+%s\s.*$' % re.escape(func), out, re.M)
    if not mm: return 10**9
    if mm.group(1) == 'OK': return 0
    return int(re.search(r'\((\d+)/', mm.group(0)).group(1))
best = (score(s0), s0); base = best[0]
for perm in itertools.permutations(range(len(blocks))):
    nb = '\n'.join('\n'.join(blocks[i]) for i in perm) + '\n'
    s = s0[:body_start] + nb + s0[end:]
    n = score(s)
    if n < best[0]: best = (n, s)
    if n == 0: break
open(p, 'w').write(best[1]); print(p, func, base, '->', best[0])
