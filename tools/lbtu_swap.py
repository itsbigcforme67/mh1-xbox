#!/usr/bin/env python3
"""lbtu_swap.py TU.c SRC.c FUNC...: replace the `asm` stub of FUNC in the translation unit TU.c by the C body of FUNC from the
working file SRC.c (near-match source). Declaration lines of SRC.c that TU.c does not have yet are added before the first function.
Run `python3 tools/check.py TU.c --module lobby` afterwards."""
import re, sys
tu, src = sys.argv[1:3]
names = sys.argv[3:]
s = open(tu).read()
w = open(src).read()
pat = re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n', re.M)
def body(name):
    for m in pat.finditer(w):
        if m.group(1) == name:
            end = (m.end() + 2) if w[m.end():m.end() + 2] == '}\n' else w.index('\n}\n', m.end()) + 3
            st = m.start()
            pre = w[:st].rstrip('\n')
            if pre.endswith('*/'):
                c = pre.rfind('/*')
                if c >= 0 and '\n\n' not in pre[c:]:
                    st = c
            return w[st:end]
    sys.exit('%s not in %s' % (name, src))
# declaration lines of src (outside function bodies)
decl = []
pos = 0
for m in pat.finditer(w):
    if m.start() < pos:
        continue
    decl.append(w[pos:m.start()])
    end = (m.end() + 2) if w[m.end():m.end() + 2] == '}\n' else w.index('\n}\n', m.end()) + 3
    pos = end
decl.append(w[pos:])
decl = [re.sub(r'/\*.*?\*/', '', c, flags=re.S) for c in decl]
have = set(l.strip() for l in s.split('\n'))
add = []
for chunk in decl:
    for ln in chunk.split('\n'):
        k = ln.strip()
        if k and k not in have and not k.startswith('#include') and not k.startswith('/*'):
            add.append(ln); have.add(k)
first = pat.search(s)
# insert declarations before the first stub/function that follows the declaration block: just before first function-like text
idx = s.index('\n/* original bytes') if '\n/* original bytes' in s else first.start()
fm = pat.search(s)
ins = min(i for i in (fm.start() if fm else len(s), s.find('\n/* original bytes')) if i >= 0)
s = s[:ins] + '\n'.join(add) + '\n' + s[ins:]
for n in names:
    m = re.search(r'(/\* original bytes[^\n]*\*/\n)?asm [^\n]*\b%s\([^\n]*\)\n\{\n#include "%s.inc"\n\}\n' % (n, n), s)
    if not m:
        sys.exit('stub for %s not found' % n)
    s = s[:m.start()] + body(n) + '\n' + s[m.end():]
open(tu, 'w').write(s)
print('swapped', names, 'decl lines added', len(add))
