#!/usr/bin/env python3
"""lbdecl.py FILE FUNC 'decl1;decl2;...' : try all orderings of the named single-line local declarations
(given as exact lines without indentation, separated by '|') at their current position; keeps the best (--module lobby)."""
import itertools, subprocess, re, sys
path, func, spec = sys.argv[1], sys.argv[2], sys.argv[3]
decls = spec.split('|')
src = open(path).read()
idx = [src.index('    ' + d + '\n') for d in decls]
first = min(idx)
for d in decls: src = src.replace('    ' + d + '\n', '', 1)
def score(s):
    open(path, 'w').write(s)
    out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    l = [x for x in out.split('\n') if (' %s ' % func) in x]
    if not l: return 99999
    l = l[0]
    return 0 if l.startswith('OK') else int(re.search(r'\((\d+)/', l).group(1))
best = None
for perm in itertools.permutations(decls):
    s = src[:first] + ''.join('    ' + d + '\n' for d in perm) + src[first:]
    sc = score(s)
    print(sc, perm, flush=True)
    if best is None or sc < best[0]: best = (sc, s)
open(path, 'w').write(best[1])
print('best', best[0])
