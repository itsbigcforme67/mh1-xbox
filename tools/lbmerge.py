#!/usr/bin/env python3
"""lbmerge.py PREFIX "comment" NAME... : for functions whose standalone source is build/lbauto/NAME.c (tools/lbauto.py) or
src/lobby/_one/NAME.c, build contiguous runs PREFIXNN.c in src/lobby/ (merging the declarations), verify each run with
tools/check.py --module lobby, split a run that fails to compile or match into single-function files, register all with
'lobby START END NAME' lines in config/c_files.txt."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
S = '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/fl.txt'
prefix, cmt = sys.argv[1:3]; names = sys.argv[3:]
info = {}
for l in open(S):
    a, nm, sz = l.split(); info[nm] = (int(a, 16), int(sz))
def src(nm):
    for d in ('build/lbauto', 'src/lobby/_one'):
        p = os.path.join(d, nm + '.c')
        if os.path.exists(p): return open(p).read()
    raise SystemExit('no source for ' + nm)
def split(nm):
    s = src(nm).replace('#include "lobby.h"\n', '')
    m = re.search(r'^[\w\*\s]+\b%s\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n' % re.escape(nm), s, re.M)
    return [l.strip() for l in s[:m.start()].split('\n') if l.strip()], s[m.start():].strip() + '\n'
reg = []
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby': reg.append((int(p[1], 16), int(p[2], 16)))
names = [n for n in names if not any(x <= info[n][0] < y for x, y in reg)]
names.sort(key=lambda n: info[n][0])
runs = []; cur = []
for n in names:
    a, sz = info[n]
    if cur and 0 <= a - (info[cur[-1]][0] + info[cur[-1]][1]) < 16: cur.append(n)
    else:
        if cur: runs.append(cur)
        cur = [n]
if cur: runs.append(cur)
num = 1
while os.path.exists('src/lobby/%s%02d.c' % (prefix, num)): num += 1
def build(group, path):
    decls = []; bodies = []
    for n in group:
        d, b = split(n)
        for l in d:
            if l not in decls: decls.append(l)
        bodies.append(b)
    hdr = '/* %s%02d - %s 0x%08X-0x%08X: %s (first drafted by tools/lbauto.py). */\n' % (prefix, num, cmt, info[group[0]][0], info[group[-1]][0] + info[group[-1]][1], ', '.join(group))
    open(path, 'w').write(hdr + '#include "lobby.h"\n' + '\n'.join(decls) + ('\n' if decls else '') + '\n' + '\n'.join(bodies))
def ok(path, group):
    out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    got = [l for l in out.split('\n') if l.startswith('OK')]
    return len(got) == len(group)
lines = []
def emit(group):
    global num
    path = 'src/lobby/%s%02d.c' % (prefix, num)
    build(group, path)
    if ok(path, group):
        lines.append('lobby 0x%08X 0x%08X %s%02d' % (info[group[0]][0], info[group[-1]][0] + info[group[-1]][1], prefix, num))
        print(lines[-1], '#', ', '.join(group)); num += 1
    else:
        os.remove(path)
        if len(group) == 1: print('FAILED', group[0]); return
        for n in group: emit([n])
for g in runs: emit(g)
with open('config/c_files.txt', 'a') as f:
    f.write('\n'.join(lines) + ('\n' if lines else ''))
