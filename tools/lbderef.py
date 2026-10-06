#!/usr/bin/env python3
"""lbderef.py FILE... : int-mode m2c derefs `*((int)&SYM + off...)` -> `*(s32 *)((int)&SYM + off...)` (a compiling start; the access
width must then be fixed from the asm), `extern int pNet;`-style redeclarations of known globals dropped."""
import re, sys
for p in sys.argv[1:]:
    s = open(p).read(); t = s
    t = re.sub(r'(?<![\w)\]])\*\(\(int\)&(\w+)((?: \+ (?:[^()+]|\((?:[^()]|\([^()]*\))*\))+?)*)\)', lambda m: '*(s32 *)((int)&%s%s)' % (m.group(1), m.group(2)), t)
    t = re.sub(r'^extern int pNet;\n', '', t, flags=re.M)
    open(p, 'w').write(t)
    print(p, 'changed' if t != s else 'same')
