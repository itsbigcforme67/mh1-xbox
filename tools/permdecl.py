#!/usr/bin/env python3
"""permdecl.py FILE FUNC DECLMARK_START DECLMARK_END [INITSTART INITEND]: brute-force the order of local declaration lines (between two
marker comment lines) and optionally of the statement lines between a second marker pair. Keeps the best (fewest diffs)."""
import itertools, subprocess, re, sys
p, func = sys.argv[1:3]
marks = sys.argv[3:]
s0 = open(p).read()
def block(m1, m2, s):
    a = s.index(m1) + len(m1); b = s.index(m2, a)
    return a, b
sections = []
for i in range(0, len(marks), 2):
    a, b = block(marks[i], marks[i + 1], s0)
    sections.append((a, b, [l for l in s0[a:b].split('\n') if l.strip()]))
def score(s):
    open(p, 'w').write(s)
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True).stdout
    m = re.search(r'^(--|OK)\s+%s\s.*?(?:\((\d+)/)?$' % re.escape(func), out, re.M)
    if not m: return 10**9
    if m.group(1) == 'OK': return 0
    return int(re.search(r'^--\s+%s\s.*\((\d+)/' % re.escape(func), out, re.M).group(1))
best = (10**9, None)
perms = [list(itertools.permutations(range(len(x[2])))) for x in sections]
for combo in itertools.product(*perms):
    s = s0
    for (a, b, lines), perm in sorted(zip(sections, combo), key=lambda t: -t[0][0]):
        s = s[:a] + '\n' + '\n'.join(lines[i] for i in perm) + '\n' + s[b:]
    n = score(s)
    if n < best[0]:
        best = (n, s); print(n, combo, flush=True)
    if n == 0: break
open(p, 'w').write(best[1])
print('best', best[0])
