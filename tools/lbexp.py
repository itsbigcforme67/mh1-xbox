#!/usr/bin/env python3
"""lbexp.py FILE.c : compile one scratch C file with the project compiler and print each function's disassembly
(for quick experiments with source forms; no comparison)."""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check
from mips_dis import dis
f = sys.argv[1]
o = tempfile.mktemp(suffix='.o')
r = subprocess.run([check.WIBO, check.MWCC] + check.CFLAGS + ['-o', o, f], capture_output=True, text=True, cwd=check.ROOT)
if not os.path.exists(o):
    print(r.stdout, r.stderr); sys.exit(1)
for name, (code, masks, calls) in check.read_obj(o).items():
    print('==', name)
    for i in range(0, len(code), 4):
        w = int.from_bytes(code[i:i+4], 'little')
        print('%04X %s%s' % (i, dis(w, i), '  (reloc)' if i in masks else ''))
