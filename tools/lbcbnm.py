#!/usr/bin/env python3
"""lbcbnm.py FILE... : in-place conversion of src/lobby/b/nm/CallBack_Result_*.c drafts: `long long spNN; spNN = arg0;` ->
`CNET_RES res` parameter (see lbcb.py). Prints check.py's status line."""
import sys, re, subprocess
for p in sys.argv[1:]:
    s = open(p).read()
    m = re.search(r'long long (sp\w+);', s)
    if not m: print('skip', p); continue
    v = m.group(1)
    s = s.replace('#include "lobby_a.h"', '#include "lobby_b.h"').replace('#include "lobby_f.h"', '#include "lobby_b.h"')
    s = re.sub(r'^extern char (temp_\w+|sp\w+)\[\];\n', '', s, flags=re.M)
    s = re.sub(r'\(int arg0\) \{', '(CNET_RES res) {', s)
    s = re.sub(r'^\s*long long %s;\n' % v, '', s, flags=re.M)
    s = re.sub(r'^\s*%s = arg0;\n' % v, '', s, flags=re.M)
    s = s.replace('(s8) %s' % v, 'res.val').replace(v, 'res.val')
    open(p, 'w').write(s)
    out = subprocess.run(['python3', 'tools/check.py', p], capture_output=True, text=True).stdout
    print(*[l for l in out.split('\n') if l.startswith(('--', 'OK')) and 'CallBack' in l])
    if 'rror' in out: print(' ERROR', p)
