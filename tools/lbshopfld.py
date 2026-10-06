#!/usr/bin/env python3
"""lbshopfld.py FILE... : F(T, &lbShop, 0xNN) -> lbShop.member (LB_SHOP in include/lobby.h); includes lobby.h instead of lobby_a.h.
Keeps the rewrite only when the file still compiles and check.py's diff count does not get worse."""
import sys, re, subprocess
M = {0x14: 'step', 0x15: 'x15', 0x16: 'x16', 0x17: 'x17', 0x18: 'x18', 0x19: 'mode', 0x1A: 'x1A', 0x1B: 'x1B', 0x1C: 'x1C',
     0x20: 'f20', 0x24: 'f24', 0x28: 'f28', 0x2C: 'f2C', 0x30: 'f30', 0x34: 'f34', 0x38: 'f38', 0x3C: 'f3C', 0x40: 'f40', 0x44: 'f44',
     0x48: 'tag', 0x4C: 'help', 0x50: 'list', 0x60: 'wait', 0x64: 'tbl', 0x6C: 'x6C', 0x6D: 'x6D', 0x6E: 'x6E', 0x70: 'x70',
     0x74: 'cur', 0x78: 'x78', 0x7C: 'qty', 0x80: 'count', 0x84: 'x84', 0x8C: 'key', 0x8E: 'x8E', 0x8F: 'x8F'}
def cnt(p):
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = out.stdout + out.stderr
    if 'Error' in out: return 10**9
    return sum(int(x) for x in re.findall(r'^--\s+\S+\s+lobby\s+0x\w+\s+\d+ bytes\s+\((\d+)/', out, re.M))
for p in sys.argv[1:]:
    s = open(p).read()
    t = re.sub(r'F\(\s*[\w\* ]+?,\s*&lbShop,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)', lambda m: ('lbShop.' + M[int(m.group(1), 0)]) if int(m.group(1), 0) in M else m.group(0), s)
    if t == s: print(p, 'unchanged'); continue
    t = t.replace('#include "lobby_a.h"', '#include "lobby_s.h"', 1)
    t = re.sub(r'^extern char (?:lbShop|shopList)\[\];\n', '', t, flags=re.M)
    t = re.sub(r'(?<![\w.])\(int\)\s*lbShop\.(tbl|list|tag)\b', r'lbShop.\1', t)
    FT = {'f20': 'void (*)()', 'f24': 'void (*)()', 'f28': 'void (*)()', 'f34': 'void (*)()', 'f3C': 'void (*)()', 'f40': 'void (*)()',
          'f2C': 'int (*)()', 'f30': 'int (*)()', 'f38': 'int (*)()'}
    t = re.sub(r'lbShop\.(f[0-9A-F]{2}) = (?:\(int\)\s*)?&?(\w+);', lambda m: 'lbShop.%s = (%s)%s;' % (m.group(1), FT.get(m.group(1), 'void *'), m.group(2)) if m.group(1) in FT else m.group(0), t)
    # dereferences of int-mode pointer arithmetic
    t = re.sub(r'\*\(\(int\)&shopList \+ \(((?:[^()]|\([^()]*\))+?) \* 0x28\)\)', r'shopList[\1].price', t)
    t = re.sub(r'\*\(\(int\)lbShop\.tbl \+ \(((?:[^()]|\([^()]*\))+?) \* 8\)\)', r'lbShop.tbl[(\1) * 2]', t)
    t = re.sub(r'\*\(\(int\)lbShop\.tbl \+ \(((?:[^()]|\([^()]*\))+?) \* 8\) \+ 4\)', r'lbShop.tbl[(\1) * 2 + 1]', t)
    # pointer members used as integers (m2c int mode)
    t = re.sub(r'(?<![\w.&])lbShop\.(tbl|list|tag)\b(?!\s*=[^=])', r'(int)lbShop.\1', t)
    t = re.sub(r'(lbShop\.(?:tbl|list|tag)) = \(int\)&?(\w+);', lambda m: '%s = (void *)%s;' % (m.group(1), ('&' if False else '') + m.group(2)) , t)
    b = cnt(p)
    open(p, 'w').write(t)
    n = cnt(p)
    if n <= b: print(p, b, '->', n)
    else: open(p, 'w').write(s); print(p, 'reverted', b, n)
