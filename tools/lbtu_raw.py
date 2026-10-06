#!/usr/bin/env python3
"""lbtu_raw.py TU.c FUNC...: turn the C definition of FUNC in TU.c back into an `asm` stub (original bytes, config/c_rawfuncs.txt must
list FUNC). The return type and parameters come from the existing definition."""
import re, sys
tu = sys.argv[1]
s = open(tu).read()
for n in sys.argv[2:]:
    m = re.search(r'^(?:static )?((?:[A-Za-z_][\w \*]*?)\b)%s\(([^;{]*)\)\s*\{\n' % n, s, re.M)
    if not m:
        sys.exit('%s not found' % n)
    en = s.index('\n}\n', m.end()) + 3
    rt = m.group(1).strip()
    params = m.group(2)
    stub = '/* original bytes: build/raw/%s.inc (config/c_rawfuncs.txt) */\nasm %s %s(%s)\n{\n#include "%s.inc"\n}\n' % (n, rt, n, params, n)
    s = s[:m.start()] + stub + s[en:]
open(tu, 'w').write(s)
