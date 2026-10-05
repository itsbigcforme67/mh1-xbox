#!/usr/bin/env python3
"""appendfn.py RUN.c NM.c func [func...]: append the named function definitions of the whole-file
near-match NM.c (with the comment block directly above) to the matching run file RUN.c, so a function
that matches in NM.c after a fix can be linked by extending the run's end address in config/c_files.txt.
Does NOT regenerate the run (run files carry hand edits). Prints the address/size line from check.py."""
import re, subprocess, sys
run, nm = sys.argv[1:3]
s = open(nm).read()
out = open(run).read().rstrip('\n') + '\n'
chk = subprocess.run(['python3', 'tools/check.py', nm], capture_output=True, text=True).stdout
for fn in sys.argv[3:]:
    m = None
    for m in re.finditer(r'^(?:static )?[A-Za-z_][\w \*]*?\b%s\([^;{]*\)\s*\{\n' % re.escape(fn), s, re.M):
        pass
    if not m:
        sys.exit('not found: ' + fn)
    start = m.start()
    end = s.index('\n}\n', m.end()) + 3
    pre = s[:start].rstrip('\n')
    if pre.endswith('*/'):
        c = pre.rfind('/*')
        if c >= 0 and '\n\n' not in pre[c:]:
            start = c
    out += '\n' + s[start:end]
    for l in chk.split('\n'):
        if re.match(r'^(OK|--)\s+%s\s' % re.escape(fn), l):
            print(l)
open(run, 'w').write(out)
