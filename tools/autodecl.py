#!/usr/bin/env python3
"""autodecl.py FILE FUNC [MAXPERMS]: brute-force the order of the local declaration lines at the top of FUNC (declbf.py that also
handles K&R definitions and keeps the best result). FILE is left unchanged if nothing improves."""
import itertools, subprocess, re, sys, math
p, func = sys.argv[1:3]
maxp = int(sys.argv[3]) if len(sys.argv) > 3 else 120
src = open(p).read()
m = re.search(r'^[\w\*\s]*\b%s\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{\n' % re.escape(func), src, re.M)
if not m: sys.exit('no function')
pos = m.end(); lines = []
while True:
    nl = src.index('\n', pos); line = src[pos:nl + 1]
    if not re.match(r'\s+(?:struct \w+|[A-Za-z_]\w*)[\s\*]+\w+(\[\w*\])?( = [^;(]+)?;\s*$', line) or line.strip().startswith(('return', 'goto')): break
    lines.append(line); pos = nl + 1
if len(lines) < 2 or math.factorial(len(lines)) > maxp: sys.exit('decls: %d' % len(lines))
start = m.end()
def score(s):
    open(p, 'w').write(s)
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = out.stdout + out.stderr
    mm = re.search(r'^(--|OK)\s+%s\s.*$' % re.escape(func), out, re.M)
    if not mm: return 10**9
    if mm.group(1) == 'OK': return 0
    return int(re.search(r'\((\d+)/', mm.group(0)).group(1))
base = score(src); best = (base, src)
for perm in itertools.permutations(range(len(lines))):
    s = src[:start] + ''.join(lines[i] for i in perm) + src[pos:]
    n = score(s)
    if n < best[0]: best = (n, s)
    if n == 0: break
open(p, 'w').write(best[1])
print('%s %s %d -> %d' % (p, func, base, best[0]))
