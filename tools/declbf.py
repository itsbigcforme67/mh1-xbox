#!/usr/bin/env python3
"""declbf.py FILE FUNC: brute-force the order of the contiguous declaration
lines at the top of FUNC (all permutations, up to 7 lines), keep the best."""
import itertools, subprocess, re, sys, shutil
path, func = sys.argv[1], sys.argv[2]
skip = int(sys.argv[3]) if len(sys.argv) > 3 else 0
src = open(path).read()
m = re.search(r'\n((?:static )?[\w\s\*]+\b%s\([^)]*\) \{\n)' % re.escape(func), src)
start = m.end()
lines = []
pos = start
while True:
    nl = src.index('\n', pos)
    line = src[pos:nl+1]
    if line.strip() == '' or not re.match(r'\s+[\w][\w\s\*\[\],]*[\w\]]+( = [^;]+)?;\s*$', line) or '(' in line.split('=')[0]:
        break
    lines.append(line); pos = nl+1
print('decl lines:', len(lines)); sys.stdout.flush()
def score(s):
    open(path,'w').write(s)
    out = subprocess.run(['python3','tools/check.py',path],capture_output=True,text=True).stdout
    l = [x for x in out.split('\n') if (' %s ' % func) in x]
    if not l: return 99999
    l = l[0]
    return 0 if l.startswith('OK') else int(re.search(r'\((\d+)/', l).group(1))
best = None
fixed, rest = lines[:skip], lines[skip:]
try:
    for perm in itertools.permutations(rest):
        s = src[:start] + ''.join(fixed) + ''.join(perm) + src[pos:]
        n = score(s)
        if best is None or n < best[0]:
            best = (n, s); print(n, [p.strip() for p in perm]); sys.stdout.flush()
        if n == 0: break
finally:
    open(path,'w').write(best[1] if best else src)
print('best', best[0])
