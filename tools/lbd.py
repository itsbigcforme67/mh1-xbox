#!/usr/bin/env python3
"""lbd.py FUNC... : print m2c draft (from $LBDRAFTS dir) and compact asm of lobby functions."""
import sys, os, re, glob, subprocess
D = os.environ.get('LBDRAFTS', '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/drafts')
txt = ''.join(open(f).read() for f in sorted(glob.glob(D + '/d*.c')))
for fn in sys.argv[1:]:
    m = re.search(r'((?:^[^\n]*\n)*?)^[\w\*\s]+\b%s\([^;{]*\) \{\n.*?\n\}\n' % re.escape(fn), txt, re.M | re.S)
    # find the function definition and trailing extern block just before it
    i = None
    for mm in re.finditer(r'^[\w\*\s]+\b%s\([^;{]*\) \{\n' % re.escape(fn), txt, re.M):
        i = mm.start(); break
    if i is None:
        print('/* no draft for', fn, '*/'); continue
    j = txt.index('\n}\n', i) + 3
    k = txt.rfind('\n\n\n', 0, i); k2 = txt.rfind('\n\n', 0, i)
    print(txt[max(k2, 0):j])
    if os.environ.get("ASM"):
        print("--- asm")
        print(subprocess.run(['python3', 'tools/lbasm.py', fn], capture_output=True, text=True).stdout)
