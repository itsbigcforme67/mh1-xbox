#!/usr/bin/env python3
"""nm_scan.py [GLOB_DIR]: run check.py on every *_nm.c under src/main and list the functions that do not
match (name, address, size, differing/total instructions), sorted by difference count.
Only functions in the E ranges unless --all is given."""
import subprocess, glob, re, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
allr = '--all' in sys.argv
RANGES = [(0x160000, 0x1C4000), (0x216000, 0x24A240), (0x2814E0, 0x293B68)]
res = []
for f in sorted(glob.glob('src/main/**/*_nm.c', recursive=True)):
    out = subprocess.run(['python3', 'tools/check.py', f], capture_output=True, text=True).stdout
    for l in out.split('\n'):
        m = re.match(r'--\s+(\S+)\s+main\s+0x([0-9A-F]+)\s+(\d+) bytes\s+\((\d+)/(\d+) instr', l)
        if m:
            a = int(m.group(2), 16)
            if allr or any(x <= a < y for x, y in RANGES):
                res.append((int(m.group(4)), int(m.group(5)), m.group(1), a, int(m.group(3)), f))
for r in sorted(res):
    print('%4d/%-5d %-34s %08X %5d  %s' % (r[0], r[1], r[2], r[3], r[4], r[5]))
