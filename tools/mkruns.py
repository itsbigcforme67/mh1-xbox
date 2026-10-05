#!/usr/bin/env python3
"""mkruns.py NM.c PREFIX [env MKRUNS_KEEP=name1,name2]: split the near-match file into matching runs (files PREFIX, PREFIXb, ...) and
print c_files.txt lines (text only; rodata lines separately)."""
import re, sys, os, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
nm, prefix = sys.argv[1:3]
exec(open(os.path.join(ROOT, 'tools/status.py')).read().split("if __name__")[0].replace('f=sys.argv[1] if len(sys.argv) > 1 else "x"', 'f=nm'))
# parse chunks
lines = open(nm).read().split('\n')
chunks = []   # (kind, name, text) kind: 'top' or 'fn'
i = 0
top = []
def flush():
    global top
    if top:
        chunks.append(('top', None, '\n'.join(top))); top = []
hdr = re.compile(r'^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\(.*\)\s*(?:[A-Za-z_][^{;]*;\s*)*\{\s*$')
macro = re.compile(r'^(DMG_SIMPLE)\((\w+),')
while i < len(lines):
    l = lines[i]
    m = macro.match(l)
    if m:
        flush(); chunks.append(('fn', m.group(2), l)); i += 1; continue
    m = hdr.match(l)
    if m and not l.startswith(('extern', 'typedef', '#', ' ')) and '=' not in l.split('(')[0] and not l.rstrip().endswith(';'):
        # find closing brace line
        j = i
        while lines[j] != '}':
            j += 1
        # attach preceding comment lines that directly precede (no blank)
        k = len(top)
        cm = []
        while top and top[-1].strip() and (top[-1].lstrip().startswith(('/*', '*', '*/')) or top[-1].rstrip().endswith('*/')):
            cm.insert(0, top.pop())
        flush()
        chunks.append(('fn', m.group(1), '\n'.join(cm + lines[i:j + 1])))
        i = j + 1
        continue
    top.append(l); i += 1
flush()
fn = {n: t for k, n, t in chunks if k == 'fn'}
print('functions:', len(fn), file=sys.stderr)
missing = [n for n in fn if n not in res]
print('no status for', missing, file=sys.stderr)
def statusof(n): return st.get(n, '?')
tops = '\n'.join(t for k, n, t in chunks if k == 'top')
tops = re.sub(r'\n{3,}', '\n\n', tops)
order = sorted([n for n in fn if n in res], key=lambda n: res[n]['addr'])
runs = []; cur = []
for n in order:
    if statusof(n) in ('OK', 'NOISE'): cur.append(n)
    else:
        if cur: runs.append(cur)
        cur = []
if cur: runs.append(cur)
letters = ['', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n']
outl = []
for k, run in enumerate(runs):
    path = prefix + letters[k] + '.c'
    KEEP = set(os.environ.get('MKRUNS_KEEP', '').split(','))  # statics that must stay static (callees only used in their own run)
    body = '\n\n'.join(re.sub(r'^static ', '', fn[n], flags=re.M) if n not in KEEP else fn[n] for n in run)
    D = set(n for n in run if n in KEEP)
    def fixproto(m):
        return m.group(0) if m.group(2) in D else m.group(1) + m.group(3)
    t_ = re.sub(r'^(static )((?:[A-Za-z_][\w \*]*?\b)?(\w+))\(', lambda m: m.group(0) if m.group(3) in D else m.group(2) + '(', tops, flags=re.M)
    first, last = run[0], run[-1]
    head = '/* em01 AI, run %d: %s .. %s (game.bin 0x%08X-0x%08X). Matching functions of em01_ai_nm.c (that file holds the\n * whole AI including the near-matches). See em01_ai_nm.c for the description. */\n' % (k + 1, first, last, res[first]['addr'], res[last]['addr'] + res[last]['size'])
    # strip leading top comment of the nm file
    t = re.sub(r'\A/\*.*?\*/\n', '', t_, count=1, flags=re.S)
    open(path, 'w').write(head + t.rstrip('\n') + '\n\n' + body + '\n')
    outl.append('game 0x%08X 0x%08X %s' % (res[first]['addr'], res[last]['addr'] + res[last]['size'], os.path.relpath(path[:-2], 'src/game')))
    print('%s: %d functions %s..%s' % (path, len(run), first, last), file=sys.stderr)
print('\n'.join(outl))
