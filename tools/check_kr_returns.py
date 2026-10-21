#!/usr/bin/env python3
"""check_kr_returns.py: list declarations `int f();` (or s32 / u32 / long) of functions whose definition returns a
char / short type (s8 u8 s16 u16 ...). gcc (the PC build) happens to leave the upper bits of the return register
clear in most of these; nxdk's clang (the Xbox build) returns the whole register as the callee left it, so the caller
reads garbage: the G Elder's quest list crashed on the Xbox that way (Lb_get_quest_type, 21 Oct 2026). Fix a hit by
declaring the real type (guard with #ifndef __MWERKS__ in a byte-matching file). Standard library only.
usage: tools/check_kr_returns.py [DIR...]   (default src)"""
import os, re, sys
dirs = sys.argv[1:] or ['src']
files = [os.path.join(r, f) for d in dirs for r, _, fs in os.walk(d) for f in fs if f.endswith('.c')]
txt = {f: open(f, errors='replace').read() for f in files}
defs = {}
for f, t in txt.items():
    for m in re.finditer(r'^(?:static\s+)?(s8|u8|s16|u16|char|short|unsigned char|unsigned short|signed char)\s+(\w+)\s*\([^;{]*\)\s*\{', t, re.M):
        defs.setdefault(m.group(2), set()).add((m.group(1), f))
n = 0
for f, t in sorted(txt.items()):
    for m in re.finditer(r'^\s*(?:extern\s+)?(int|s32|u32|long)\s+(\w+)\s*\(([^)]*)\)\s*;', t, re.M):
        if m.group(2) in defs:
            ty, df = sorted(defs[m.group(2)])[0]
            print('%s: %s %s(%s) -- defined %s in %s' % (f, m.group(1), m.group(2), m.group(3), ty, df))
            n += 1
print('%d declaration(s)' % n, file=sys.stderr)
