#!/usr/bin/env python3
"""lbmerge.py PREFIX "comment" NAME... : for functions whose standalone source is build/lbauto/NAME.c (tools/lbauto.py) or
src/lobby/_one/NAME.c, build contiguous runs PREFIXNN.c in src/lobby/ (merging the declarations), verify each run with
tools/check.py --module lobby, split a run that fails to compile or match into single-function files, register all with
'lobby START END NAME' lines in config/c_files.txt."""
import sys, os, re, subprocess
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__))))
import lbf_jt
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
S = os.environ.get('LBFL', '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/fl.txt')
LD = os.environ.get('LBDIR', 'f')
prefix, cmt = sys.argv[1:3]; names = sys.argv[3:]
info = {}
for l in open(S):
    a, nm, sz = l.split(); info[nm] = (int(a, 16), int(sz))
def src(nm):
    for d in ('src/lobby/_one', 'build/lbauto'):
        p = os.path.join(d, nm + '.c')
        if os.path.exists(p): return open(p).read()
    raise SystemExit('no source for ' + nm)
def split(nm):
    s = src(nm)
    s = s.replace('#include "lobby.h"\n', '')
    s = re.sub(r'#include "lobby_[a-z]\.h"\n', '', s)
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
while os.path.exists('src/lobby/%s/%s%02d.c' % (LD, prefix, num)): num += 1
def hdr_of(nm):
    m = re.search(r'#include "(lobby_[a-z]\.h)"', src(nm))
    return m.group(1) if m else 'lobby_f.h'
def build(group, path):
    decls = []; bodies = []
    hs = set(hdr_of(n) for n in group)
    if hs == {'lobby_a.h', 'lobby_b.h'}: hs = {'lobby_b.h'}
    if len(hs) > 1:
        return False
    H = hs.pop()
    for n in group:
        d, b = split(n)
        for l in d:
            if l not in decls: decls.append(l)
        bodies.append(b)
    hdr = '/* %s%02d - %s 0x%08X-0x%08X: %s (first drafted by tools/lbauto.py). */\n' % (prefix, num, cmt, info[group[0]][0], info[group[-1]][0] + info[group[-1]][1], ', '.join(group))
    open(path, 'w').write(hdr + '#include "%s"\n' % H + '\n'.join(decls) + ('\n' if decls else '') + '\n' + '\n'.join(bodies))
    return True
def ok(path, group):
    out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    force = set(filter(None, os.environ.get('FORCE_OK', '').split(',')))
    got = [l for l in out.split('\n') if l.startswith('OK') or (len(l.split()) > 1 and l.split()[1] in force)]
    return len(got) == len(group)
lines = []
def emit(group):
    global num
    path = 'src/lobby/%s/%s%02d.c' % (LD, prefix, num)
    if os.environ.get("DBG"): build(group, path); os.system("cp %s /tmp/dbg.c" % path)
    if build(group, path) and ok(path, group):
        lines.append('lobby 0x%08X 0x%08X %s/%s%02d' % (info[group[0]][0], info[group[-1]][0] + info[group[-1]][1], LD, prefix, num))
        tabs = sorted(set(r for n in group for r in lbf_jt.ranges(n)))
        merged = []
        for a, e in tabs:   # one rodata slot per object: tables only separated by alignment padding are one range
            if merged and a - merged[-1][1] < 16: merged[-1][1] = max(merged[-1][1], e)
            else: merged.append([a, e])
        for a, e in merged:
            lines.append('lobby:rodata 0x%08X 0x%08X %s/%s%02d' % (a, e, LD, prefix, num)); print('  jump table', lines[-1])
        print(lines[-1], '#', ', '.join(group)); num += 1
    else:
        if os.path.exists(path): os.remove(path)
        if len(group) == 1: print('FAILED', group[0]); return
        for n in group: emit([n])
for g in runs: emit(g)
with open('config/c_files.txt', 'a') as f:
    f.write('\n'.join(lines) + ('\n' if lines else ''))
