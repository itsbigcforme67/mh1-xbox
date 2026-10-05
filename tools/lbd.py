#!/usr/bin/env python3
"""lbd.py FUNC... : print m2c draft (from $LBDRAFTS dir) and compact asm of lobby functions."""
import sys, os, re, glob, subprocess
D = os.environ.get('LBDRAFTS', '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/drafts')
txt = ''.join(open(f).read() for f in sorted(glob.glob(D + '/d*.c')))
for fn in sys.argv[1:]:
    i = None
    for mm in re.finditer(r'^[\w\*\s]+\b%s\([^;{]*\) \{\n' % re.escape(fn), txt, re.M):
        i = mm.start(); break
    if i is None:
        print('/* no draft for', fn, '*/'); continue
    j = txt.index('\n}\n', i) + 3
    k2 = txt.rfind('\n}\n', 0, i)
    hdr = txt[k2 + 3:i]
    hdr = '\n'.join(l for l in hdr.split('\n') if l.strip() and not l.startswith('M2C_UNK') or l.startswith('extern'))
    print(hdr + '\n' + txt[i:j])
    if os.environ.get("ASM"):
        print("--- asm")
        print(subprocess.run(['python3', 'tools/lbasm.py', fn], capture_output=True, text=True).stdout)
