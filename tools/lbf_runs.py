#!/usr/bin/env python3
"""lbruns.py NM.c PREFIX "comment": split the fully matching, not yet registered functions of a lobby working file
(NM.c, whole file in address order or not) into address-contiguous runs PREFIXNN.c in src/lobby/ and append their
'lobby START END NAME' lines to config/c_files.txt. Run tools/rebuild.sh afterwards."""
import os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lbf_jt, glob
nm, prefix, cmt = sys.argv[1:4]
reg = []
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby':
        reg.append((int(p[1], 16), int(p[2], 16)))
out = subprocess.run(['python3', 'tools/check.py', nm, '--module', 'lobby'], capture_output=True, text=True).stdout
force = set(filter(None, os.environ.get('FORCE_OK', '').split(',')))
rows = []
for l in out.split('\n'):
    m = re.match(r'^(?:OK|--)\s+(\S+)\s+(?:lobby|game)\s+0x([0-9A-F]+)\s+(\d+) bytes', l)
    if m and (l.startswith('OK') or m.group(1) in force):
        a = int(m.group(2), 16); sz = int(m.group(3))
        if any(s <= a < e for s, e in reg):
            continue
        rows.append((m.group(1), a, sz))
rows.sort(key=lambda r: r[1])
runs = []; cur = []
for r in rows:
    if cur and 0 <= r[1] - (cur[-1][1] + cur[-1][2]) < 16:
        cur.append(r)
    else:
        if cur: runs.append(cur)
        cur = [r]
if cur: runs.append(cur)
n = 1
while os.path.exists('src/lobby/f/%s%02d.c' % (prefix, n)): n += 1
lines = []
for run in runs:
    name = '%s%02d' % (prefix, n); n += 1
    s, e = run[0][1], run[-1][1] + run[-1][2]
    hdr = '%s - %s 0x%08X-0x%08X: %s. Whole file in %s.' % (name, cmt, s, e, ', '.join(r[0] for r in run), os.path.basename(nm))
    subprocess.run(['python3', 'tools/mkrun2.py', nm, 'src/lobby/f/%s.c' % name, hdr] + [r[0] for r in run], check=True)
    lines.append('lobby 0x%08X 0x%08X f/%s' % (s, e, name))
    jt = []
    for r in run:
        for a, e2 in lbf_jt.ranges(r[0]):
            jt.append((a, e2)); print('  jump table', r[0], hex(a), hex(e2))
    if jt:   # one object has ONE rodata slot: several tables become one range (alignment padding included)
        lines.append('lobby:rodata 0x%08X 0x%08X f/%s' % (min(a for a, b in jt), max(b for a, b in jt), name))
    print(lines[-1], '#', ', '.join(r[0] for r in run))
with open('config/c_files.txt', 'a') as f:
    f.write('\n'.join(lines) + ('\n' if lines else ''))
