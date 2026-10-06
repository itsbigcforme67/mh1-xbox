#!/usr/bin/env python3
"""lbblk2.py FILE... : `block_N:\\n default:\\n return X;` in the middle of a switch (m2c output) -> `break;` there, `goto block_N;`
-> `break;`, and a single `return X;` after the switch. Keeps the change only if check.py's diff count does not get worse."""
import sys, re, subprocess
def cnt(p):
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = out.stdout + out.stderr
    if 'Error' in out: return 10**9
    m = re.findall(r'^--\s+\S+\s+lobby\s+0x\w+\s+\d+ bytes\s+\((\d+)/', out, re.M)
    return sum(map(int, m))
for p in sys.argv[1:]:
    s = open(p).read()
    m = re.search(r'^block_(\d+):\n\s+default:\n\s+return ([^;]+);\n', s, re.M) or re.search(r'^\s+default:\n^block_(\d+):\n\s+return ([^;]+);\n', s, re.M)
    if not m: continue
    n, val = m.group(1), m.group(2)
    t = s[:m.start()] + '        break;\n' + s[m.end():]
    t = re.sub(r'goto block_%s;' % n, 'break;', t)
    # return after the switch: the last "    }\n}\n" of the function
    i = t.rindex('    }\n}\n')
    t = t[:i] + '    }\n    return %s;\n}\n' % val + t[i + len('    }\n}\n'):]
    base = cnt(p)
    open(p, 'w').write(t)
    new = cnt(p)
    if new <= base: print('kept', p, base, '->', new)
    else:
        open(p, 'w').write(s); print('reverted', p, base, '->', new)
