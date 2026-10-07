#!/usr/bin/env python3
"""declhill2.py FILE MARK FUNC [--apply]: hill climb over the ORDER of the local declaration lines of FUNC
(works for K&R headers and brace-on-next-line too, unlike declbf.py). MARK is a unique text that starts the
function (e.g. 'void Quest_next_em_set(n)'). The declaration block is the run of lines after the opening
brace that look like `type name;` / `type *name;` (no initialiser needed to be special). Score = number of
real differing lines from tools/align.py (relocation noise ignored). Tries every single-line move
(remove + insert elsewhere) per round until no improvement. Prints the best order; --apply writes it."""
import sys, os, re, subprocess
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from vt import score
f, mark, fn = sys.argv[1:4]
apply_ = '--apply' in sys.argv
src = open(f).read()
a = src.index(mark)
b = src.index('\n}\n', a) + 3
func = src[a:b]
lines = func.split('\n')
k = next(i for i, l in enumerate(lines) if l.strip() == '{') + 1
decl = []
while re.match(r'^\s+[A-Za-z_][\w\s\*\[\]]*[\w\]];\s*$', lines[k + len(decl)]) and '(' not in lines[k + len(decl)]:
    decl.append(lines[k + len(decl)])
head, rest = lines[:k], lines[k + len(decl):]
z = os.path.join(os.path.dirname(f), 'zzh%d.c' % os.getpid())
cache = {}
def sc(order):
    key = tuple(order)
    if key in cache:
        return cache[key]
    open(z, 'w').write(src[:a] + '\n'.join(head + list(order) + rest) + src[b:])
    out, n = score(z, fn)
    r = (n if out else 10**6)
    m = re.search(r'\((\d+)/', out or '')
    cache[key] = (r, int(m.group(1)) if m else 10**6)
    return cache[key]
cur = list(decl); best = sc(cur)
print('start', best, flush=True)
improved = True
while improved and best[0] > 0:
    improved = False
    for i in range(len(cur)):
        for j in range(len(cur)):
            if i == j:
                continue
            t = cur[:]; x = t.pop(i); t.insert(j, x)
            s = sc(t)
            if s < best:
                best, cur, improved = s, t, True
                print('better', best, flush=True)
                break
        if improved:
            break
os.path.exists(z) and os.remove(z)
print('final', best)
print('\n'.join(cur))
if apply_ and cur != decl:
    open(f, 'w').write(src[:a] + '\n'.join(head + cur + rest) + src[b:])
    print('applied')
