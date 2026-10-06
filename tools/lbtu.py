#!/usr/bin/env python3
"""lbtu.py NAME START END [--dry]: merge every registered lobby run inside START..END (hex, text addresses) plus
the not-yet-written functions in between into ONE C translation unit src/lobby/f/NAME.c, so that file-static
helpers can be shared by their callers (MWCC only uses a callee's register usage when it is a static function
defined earlier in the same file).  Functions that are not C yet (unwritten, or written but not matching) stay
original bytes: `asm` functions fed by build/raw/NAME.inc (config/c_rawfuncs.txt, see tools/build.py gen_raw).

What it does:
  * reads the 'lobby START END f/RUN' lines in config/c_files.txt that fall inside the range,
  * takes their function bodies (and the declaration lines around them, de-duplicated) from src/lobby/f/RUN.c,
  * adds an `asm` stub for every other function symbol in the range (single-nop padding symbols are skipped),
  * orders everything by address, writes NAME.c, rewrites config/c_files.txt (old run lines removed, one new line
    START..END of the last function; old runs' rodata lines are retargeted to NAME) and appends c_rawfuncs.txt lines.
The old RUN.c files are removed with `git rm` by the caller.  Afterwards: edit NAME.c (make helpers static, ...)
and run tools/rebuild.sh lobby; it must print OK.
"""
import glob, os, re, struct, subprocess, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
name, S, E = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
dry = '--dry' in sys.argv
BASE = 0x533980
bindata = open('disc/mh1/split/lobby.bin', 'rb').read()

# function symbols (address -> (size, name)) from the symbol table (works whatever the current asm/ split looks like)
syms = {}
for l in open('config/symbols/lobby.txt'):
    m = re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+);.*type:func size:0x([0-9A-Fa-f]+)', l)
    if m:
        syms[int(m.group(2), 16)] = (int(m.group(3), 16), m.group(1))

lines = open('config/c_files.txt').read().split('\n')
runs = []        # (addr, end, runname)
keep = []
retarget = {}
for l in lines:
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby':
        a, b = int(p[1], 16), int(p[2], 16)
        if S <= a < E:
            runs.append((a, b, p[3]))
            continue
    keep.append(l)
runs.sort()
runnames = set(r[2] for r in runs)

pat = re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n', re.M)
def split_chunks(s):
    chunks = []; pos = 0
    for m in pat.finditer(s):
        if m.start() < pos:
            continue
        end = (m.end() + 2) if s[m.end():m.end() + 2] == '}\n' else s.index('\n}\n', m.end()) + 3
        start = m.start()
        pre = s[pos:start]
        t = pre.rstrip('\n'); cs = start
        if t.endswith('*/'):
            c = t.rfind('/*')
            if c >= 0 and '\n\n' not in t[c:]:
                cs = pos + c; pre = s[pos:cs]
        chunks.append((None, pre)); chunks.append((m.group(1), s[cs:end])); pos = end
    chunks.append((None, s[pos:]))
    return chunks

funcs = {}   # addr -> (name, text)
decls = []
seen = set()
for a, b, r in runs:
    path = 'src/lobby/f/%s.c' % r.split('/')[-1]
    s = open(path).read()
    s = re.sub(r'^/\*.*?\*/\n', '', s, count=1, flags=re.S)
    cks = split_chunks(s)
    names = [n for n, t in cks if n]
    for n, t in cks:
        if n is None:
            for ln in t.split('\n'):
                k = ln.strip()
                if not k or k in seen:
                    continue
                seen.add(k); decls.append(ln)
        else:
            funcs[None] = None
            # address of the function: the symbol table maps names; use run start for the first function
            funcs.setdefault('list', []).append((n, t, a))
# addresses of C functions: parse from config/symbols/lobby.txt
symaddr = {}
for l in open('config/symbols/lobby.txt'):
    m = re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+)', l)
    if m:
        symaddr[m.group(1)] = int(m.group(2), 16)
for l in open('config/symbols/main.txt'):
    m = re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+)', l)
    if m:
        symaddr.setdefault(m.group(1), int(m.group(2), 16))
items = []   # (addr, name, text or None, size)
cnames = set()
for n, t, a in funcs.get('list', []):
    ad = symaddr.get(n)
    if ad is None:
        sys.exit('no address for %s' % n)
    items.append((ad, n, t))
    cnames.add(n)
caddrs = set(i[0] for i in items)
# ranges covered by C functions: find sizes via the run ranges (a function ends where the next item starts)
raw = []
for ad, (sz, n) in sorted(syms.items()):
    if not (S <= ad < E) or ad in caddrs:
        continue
    w = struct.unpack_from('<I', bindata, ad - BASE)[0]
    if sz == 4 and w == 0:
        continue
    # skip symbols that lie inside a registered run (they should not be listed as asm then)
    raw.append((ad, n, sz))
for ad, n, sz in raw:
    items.append((ad, n, None))
items.sort()
sizes = {ad: sz for ad, (sz, n) in syms.items()}
out = ['/* %s - one translation unit 0x%08X-0x%08X: %s. Built by tools/lbtu.py from the per-run files; functions that are\n   not C yet stay original bytes (asm stubs, build/raw/*.inc). */' % (name, S, E, ', '.join(i[1] for i in items))]
body = '\n'.join(decls)
out.append(body)
rawlines = []
for ad, n, t in items:
    if t is None:
        sz = sizes[ad]
        out.append('/* original bytes: build/raw/%s.inc (config/c_rawfuncs.txt) */\nasm int %s()\n{\n#include "%s.inc"\n}\n' % (n, n, n))
        rawlines.append('lobby 0x%08X 0x%X %s' % (ad, sz, n))
    else:
        out.append(t)
last = items[-1]
# end of the last item
if last[2] is None:
    endaddr = last[0] + sizes[last[0]]
else:
    endaddr = max(b for a, b, r in runs)
text = '\n'.join(out) + '\n'
print('items', len(items), 'raw', len(rawlines), 'range', hex(items[0][0]), hex(endaddr))
if dry:
    sys.exit(0)
open('src/lobby/f/%s.c' % name, 'w').write(text)
# rodata lines retargeted
new = []
for l in keep:
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby:rodata' and p[3] in runnames:
        l = '%s %s %s f/%s' % (p[0], p[1], p[2], name)
    new.append(l)
while new and new[-1] == '':
    new.pop()
new.append('lobby 0x%08X 0x%08X f/%s' % (items[0][0], endaddr, name))
open('config/c_files.txt', 'w').write('\n'.join(new) + '\n')
with open('config/c_rawfuncs.txt', 'a') as f:
    for l in rawlines:
        f.write(l + '\n')
for a, b, r in runs:
    p = 'src/lobby/f/%s.c' % r.split('/')[-1]
    subprocess.run(['git', 'rm', '-q', '-f', p])
print('wrote', name)
