#!/usr/bin/env python3
"""lbset.py FILE : stdin holds one or more complete C function definitions; each replaces the function of the same
name in FILE (or is appended when absent)."""
import re, sys
f = sys.argv[1]
src = open(f).read()
new = sys.stdin.read()
pat = re.compile(r'^((?:static )?[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n)', re.M)
for m in list(pat.finditer(new)):
    name = m.group(2)
    end = new.index('\n}\n', m.end()) + 3
    text = new[m.start():end]
    old = re.search(r'^(?:static )?[A-Za-z_][\w \*]*?\b%s\([^;{]*\)\s*\{\n.*?\n\}\n' % re.escape(name), src, re.M | re.S)
    if old:
        src = src[:old.start()] + text + src[old.end():]
    else:
        src = src.rstrip('\n') + '\n\n' + text
open(f, 'w').write(src)
