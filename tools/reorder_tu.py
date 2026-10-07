#!/usr/bin/env python3
"""reorder_tu.py FILE [--write]: put the function definitions of a near-match C file into original address order
(from docs/survey/mh1_symbols.csv). Everything before the first function definition (types, externs, prototypes)
stays on top. Definitions that are LOCAL in the symbol table are made `static` only if they already are.
A forward declaration list is NOT generated: add `static` prototypes by hand if a function calls a later one.
Prints the new order; with --write rewrites FILE. Chunks keep the comment lines directly above them."""
import csv, re, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
f = sys.argv[1]
src = open(f).read()
addr = {}
for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mh1_symbols.csv'), encoding='utf-8')):
    if r['type'] == 'FUNC':
        addr.setdefault(r['name'], int(r['addr'], 16))
# split at top-level function definitions
lines = src.split('\n')
chunks = []  # (start_line, end_line, name)
i = 0
depth = 0
start_of_item = None
while i < len(lines):
    l = lines[i]
    if depth == 0 and re.match(r'^[A-Za-z_][\w \*]*\b(\w+)\s*\(.*', l) and not l.rstrip().endswith(';') and not l.startswith('typedef'):
        m = re.match(r'^[A-Za-z_][\w \*]*?\b(\w+)\s*\(', l)
        name = m.group(1)
        # walk to the opening brace and its matching close
        j = i
        d = 0
        seen = False
        while j < len(lines):
            d += lines[j].count('{') - lines[j].count('}')
            if '{' in lines[j]:
                seen = True
            if seen and d == 0:
                break
            j += 1
        # include comment lines directly above
        s = i
        while s > 0 and (lines[s - 1].startswith('/*') or lines[s - 1].startswith(' *') or lines[s - 1].startswith('//') or lines[s - 1].startswith(' */') or lines[s-1].startswith('#if') or lines[s-1].startswith('#else') or lines[s-1].startswith('#endif')) and False:
            s -= 1
        chunks.append((s, j, name))
        i = j + 1
        continue
    i += 1
if not chunks:
    sys.exit('no functions found')
def with_comment(s):
    while s > 0 and (lines[s - 1].strip().startswith(('/*', '*', '//')) or lines[s - 1].strip().endswith('*/')):
        s -= 1
    return s
chs = [(with_comment(s), e, n) for s, e, n in chunks]
# text between functions (types, externs, prototypes) is moved to the top, in order
head = lines[:chs[0][0]]
for (s1, e1, n1), (s2, e2, n2) in zip(chs, chs[1:]):
    gap = lines[e1 + 1:s2]
    if any(g.strip() for g in gap):
        head += gap
head += lines[chs[-1][1] + 1:]
body = [('\n'.join(lines[s:e + 1]), n) for s, e, n in chs]
body.sort(key=lambda b: addr.get(b[1], 1 << 40))
print('order:', ' '.join('%s@%s' % (n, hex(addr.get(n, 0))) for _, n in body))
if '--write' in sys.argv:
    open(f, 'w').write('\n'.join(head).rstrip('\n') + '\n\n' + '\n\n'.join(b for b, _ in body) + '\n')
