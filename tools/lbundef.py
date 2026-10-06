#!/usr/bin/env python3
"""lbundef.py FILE... : add `extern char X[];` for every identifier the compiler reports undefined (repeats until it compiles or stops)."""
import re, subprocess, sys
for p in sys.argv[1:]:
    for it in range(12):
        r = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = r.stdout + r.stderr
        names = re.findall(r"undefined identifier '(\w+)'", out)
        if not names: break
        s = open(p).read()
        add = ''.join('extern char %s[];\n' % n for n in sorted(set(names)) if 'extern char %s[]' % n not in s)
        if not add: break
        i = s.index('\n', s.index('#include')) + 1
        open(p, 'w').write(s[:i] + add + s[i:])
    r = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True); out = r.stdout + r.stderr
    print(p, 'ERR' if 'Error' in out else 'ok')
