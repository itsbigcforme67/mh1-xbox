#!/usr/bin/env python3
"""mkruns_mod.py MODULE NM.c OUTDIR PREFIX FIRSTNUM "comment": like mkruns.py but for any module
(select, yn, ...). Splits the fully matching address-contiguous runs of NM.c into
OUTDIR/PREFIXNN.c and prints the c_files.txt lines (name = file name without src/MODULE/)."""
import os, re, subprocess, sys
FORCE_OK = set(filter(None, os.environ.get('FORCE_OK', '').split(',')))  # functions check.py cannot verify (calls into another module)
mod, nm, outdir, prefix, first, cmt = sys.argv[1:7]
first = int(first)
out = subprocess.run(['python3', 'tools/check.py', nm], capture_output=True, text=True).stdout
rows = []
for l in out.split('\n'):
    m = re.match(r'^(OK|--)\s+(\S+)\s+%s\s+0x([0-9A-F]+)\s+(\d+) bytes' % mod, l)
    if m:
        rows.append((m.group(2), int(m.group(3), 16), int(m.group(4)), m.group(1) == 'OK' or m.group(2) in FORCE_OK))
rows.sort(key=lambda r: r[1])
runs = []; cur = []
for r in rows:
    if r[3] and cur and 0 <= r[1] - (cur[-1][1] + cur[-1][2]) < 16:
        cur.append(r)
    elif r[3]:
        if cur: runs.append(cur)
        cur = [r]
    else:
        if cur: runs.append(cur)
        cur = []
if cur: runs.append(cur)
n = first
for run in runs:
    name = '%s%02d' % (prefix, n)
    path = '%s/%s.c' % (outdir, name)
    s, e = run[0][1], run[-1][1] + run[-1][2]
    hdr = '%s - %s 0x%08X-0x%08X: %s. Whole file in %s.' % (name, cmt, s, e, ', '.join(r[0] for r in run), nm.split('/')[-1])
    subprocess.run(['python3', 'tools/mkrun2.py', nm, path, hdr] + [r[0] for r in run], check=True)
    print('%s 0x%08X 0x%08X %s   # %d funcs: %s..%s' % (mod, s, e, name, len(run), run[0][0], run[-1][0]))
    n += 1
