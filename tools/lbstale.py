#!/usr/bin/env python3
"""lbstale.py FILE... : drop trailing stale `temp_xx` arguments (leftover argument registers that m2c passes along) from calls in
near-match drafts. Per file: try removing them one call at a time (greedy), keep a removal when the total diff count of check.py
does not get worse and the file still compiles. Prints before/after."""
import sys, re, subprocess
def cnt(p):
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True).stdout
    if 'Error' in out: return 10**9
    return sum(int(x) for x in re.findall(r'^--\s+\S+\s+lobby\s+0x\w+\s+\d+ bytes\s+\((\d+)/', out, re.M))
pat = re.compile(r'\b([A-Za-z_]\w*)\(((?:[^()]|\([^()]*\))*?(?:,\s*temp_\w+)+)\)')
for p in sys.argv[1:]:
    s = open(p).read(); base = cnt(p); cur = base
    for m in list(pat.finditer(s))[::-1]:
        name, args = m.group(1), m.group(2)
        if name in ('if', 'while', 'switch', 'F', 'sizeof'): continue
        newargs = re.sub(r'(?:,\s*temp_\w+)+$', '', args)
        if newargs == args: continue
        t = s[:m.start()] + '%s(%s)' % (name, newargs) + s[m.end():]
        open(p, 'w').write(t)
        n = cnt(p)
        if n <= cur: s, cur = t, n
        else: open(p, 'w').write(s)
    open(p, 'w').write(s)
    print(p, base, '->', cur)
