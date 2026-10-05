#!/usr/bin/env python3
"""protos.py NM.c : regenerate the forward prototypes (block between /*PROTOS*/ and /*ENDPROTOS*/) of a near-match file from
the function definitions in it, so a changed signature never conflicts with an old prototype. Statics get static prototypes; a definition preceded by a line `/*KNR*/` gets an unprototyped `();` declaration (callers pass fewer
arguments than the function reads, the original calls it that way)."""
import re, sys
p = sys.argv[1]
s = open(p).read()
a = s.index('/*PROTOS*/')
b = s.index('/*ENDPROTOS*/')
body = s[b + len('/*ENDPROTOS*/'):]
defs = []
for m in re.finditer(r'(?m)^((?:/\*KNR\*/\n)?)((?:static )?(?:void|int|s32|u8|u16|s16|f32|u32|s8) \w+)(\([^;{]*\)) \{$', body):
    defs.append(m.group(2) + ('()' if m.group(1) else m.group(3)) + ';')
s = s[:a] + '/*PROTOS*/\n' + '\n'.join(defs) + '\n' + s[b:]
open(p, 'w').write(s)
print(len(defs), 'prototypes')
