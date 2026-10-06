#!/usr/bin/env python3
"""lbtu_put.py TU.c SRC.c FUNC...: replace the C definition of FUNC in TU.c (also an asm stub) by the one in SRC.c
(keeps a leading `static` of the TU version)."""
import re, sys
tu, src = sys.argv[1:3]
s = open(tu).read(); w = open(src).read()
def span(text, name):
    m = re.search(r'^(?:static )?(?:[A-Za-z_][\w \*]*?\b)%s\([^;{]*\)\s*\{\n' % name, text, re.M)
    if not m:
        return None
    return m.start(), text.index('\n}\n', m.end()) + 3
for n in sys.argv[3:]:
    a = span(s, n); b = span(w, n)
    if not b:
        sys.exit('%s not in %s' % (n, src))
    new = w[b[0]:b[1]]
    if a:
        old = s[a[0]:a[1]]
        if old.startswith('static ') and not new.startswith('static '):
            new = 'static ' + new
        s = s[:a[0]] + new + s[a[1]:]
    else:
        m = re.search(r'(/\* original bytes[^\n]*\*/\n)?asm [^\n]*\b%s\([^\n]*\)\n\{\n#include "%s.inc"\n\}\n' % (n, n), s)
        if not m:
            sys.exit('no definition or stub of %s in %s' % (n, tu))
        s = s[:m.start()] + new + s[m.end():]
open(tu, 'w').write(s)
print('put', sys.argv[3:])
