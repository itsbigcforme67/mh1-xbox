#!/usr/bin/env python3
"""lbruns.py NM.c OUTPREFIX MODULE_DIR: split a near-match file of the lobby overlay into runs of
contiguous fully matching functions (per tools/check.py), write OUTPREFIX{,b,c,...}.c and print
c_files.txt lines. The NM file is: a header (everything before the first function definition)
followed by function definitions in address order."""
import re, subprocess, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
nm, prefix, regdir = sys.argv[1:4]
t = open(nm).read()
sym = {}
for l in open('config/symbols/lobby.txt'):
    m = re.match(r'(\w+) = 0x([0-9A-F]+); // type:func size:0x([0-9A-F]+)', l)
    if m:
        sym[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
heads = list(re.finditer(r'^([A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{)\n', t, re.M))
chunks = []
for m in heads:
    e = t.index('\n}\n', m.end()) + 3
    chunks.append((m.group(2), m.start(), e))
header = t[:chunks[0][1]]
text = {n: t[s:e] for n, s, e in chunks}
out = subprocess.run([sys.executable, 'tools/check.py', nm] + sys.argv[4:], capture_output=True, text=True).stdout
st = {}
for l in out.split('\n'):
    m = re.match(r'(OK|--)\s+(\S+)\s+lobby\s+0x([0-9A-F]+)\s+(\d+) bytes', l)
    if m:
        st[m.group(2)] = (m.group(1) == 'OK', int(m.group(3), 16), int(m.group(4)))
order = sorted((n for n in text if n in st), key=lambda n: st[n][1])
runs = []; cur = []
prev_end = None
for n in order:
    ok, a, sz = st[n]
    if ok and cur and a - ((prev_end + 15) & ~15) == 0 or ok and cur and a == prev_end:
        cur.append(n)
    elif ok:
        if cur: runs.append(cur)
        cur = [n]
    else:
        if cur: runs.append(cur)
        cur = []
    prev_end = a + sz
if cur: runs.append(cur)
letters = ['', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
lines = []
for k, run in enumerate(runs):
    path = prefix + (letters[k] if k < len(letters) else str(k)) + '.c'
    a0 = st[run[0]][1]; a1 = st[run[-1]][1] + st[run[-1]][2]
    h = re.sub(r'\A/\*.*?\*/\n', '', header, count=1, flags=re.S)
    cm = '/* %s, run %d: %s .. %s (lobby.bin 0x%08X-0x%08X): the matching functions of %s. */\n' % (os.path.basename(prefix), k + 1, run[0], run[-1], a0, a1, os.path.basename(nm))
    open(path, 'w').write(cm + h.rstrip('\n') + '\n\n' + '\n'.join(text[n] for n in run))
    lines.append('lobby 0x%08X 0x%08X %s' % (a0, a1, os.path.relpath(path[:-2], 'src/lobby')))
    print('%s: %d functions' % (path, len(run)), file=sys.stderr)
print('\n'.join(lines))
