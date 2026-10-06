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
OTHERC = set()
import glob
for f in glob.glob('src/lobby/*/*.c'):
    if not f.endswith('_nm.c'):
        OTHERC |= set(re.findall(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)\s*\{', open(f).read(), re.M))
heads = list(re.finditer(r'^([A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{)\n', t, re.M))
chunks = []
for m in heads:
    e = t.index('\n}\n', m.end() - 1) + 3
    chunks.append((m.group(2), m.start(), e))
header = t[:chunks[0][1]]
# declarations between the functions are visible to every run (gaps in the near-match file)
for (n1, s1, e1), (n2, s2, e2) in zip(chunks, chunks[1:]):
    gap = t[e1:s2]
    if gap.strip():
        header += gap
text = {n: t[s:e] for n, s, e in chunks}
out = subprocess.run([sys.executable, 'tools/check.py', nm] + sys.argv[4:], capture_output=True, text=True).stdout
st = {}
for l in out.split('\n'):
    m = re.match(r'(OK|--)\s+(\S+)\s+lobby\s+0x([0-9A-F]+)\s+(\d+) bytes', l)
    if m:
        st[m.group(2)] = (m.group(1) == 'OK', int(m.group(3), 16), int(m.group(4)))
order = sorted((n for n in text if n in st), key=lambda n: st[n][1])
def build_runs():
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
    return runs
letters = ['', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
def rpath(k):
    return prefix + (letters[k] if k < len(letters) else str(k)) + '.c'
def keep_static(n, run):
    # a static helper stays static (MWCC then does inter-procedural register allocation for its callers)
    # when every reference to it is inside this run and no linked asm mentions the symbol
    if not text[n].startswith('static '):
        return False
    for o, tx in text.items():
        if o != n and o not in run and re.search(r'\b%s\b' % re.escape(n), tx):
            return False
    hits = subprocess.run(['grep', '-rlw', '--include=*.s', n, 'asm/lobby'], capture_output=True, text=True).stdout.split()
    # asm of functions that are compiled from C (here or elsewhere) is not linked
    return all(os.path.basename(h)[:-2] in text or os.path.basename(h)[:-2] in OTHERC for h in hits if '/text/' in h) and not any('/text/' not in h for h in hits)
def write_runs(runs):
    lines = []
    for k, run in enumerate(runs):
        a0 = st[run[0]][1]; a1 = st[run[-1]][1] + st[run[-1]][2]
        h = re.sub(r'\A/\*.*?\*/\n', '', header, count=1, flags=re.S)
        cm = '/* %s, run %d: %s .. %s (lobby.bin 0x%08X-0x%08X): the matching functions of %s. */\n' % (os.path.basename(prefix), k + 1, run[0], run[-1], a0, a1, os.path.basename(nm))
        open(rpath(k), 'w').write(cm + h.rstrip('\n') + '\n\n' + '\n'.join(text[n] if keep_static(n, run) else re.sub(r'^static ', '', text[n]) for n in run))
        lines.append('lobby 0x%08X 0x%08X %s' % (a0, a1, os.path.relpath(rpath(k)[:-2], 'src/lobby')))
    return lines
def verify(runs):
    """functions that no longer match when compiled inside their run file (e.g. they lose a static callee)"""
    bad = set()
    for k, run in enumerate(runs):
        o = subprocess.run([sys.executable, 'tools/check.py', rpath(k)], capture_output=True, text=True).stdout
        bad |= {m.group(1) for m in re.finditer(r'^--\s+(\S+)\s+lobby', o, re.M)}
        if 'Error' in o:
            bad |= set(run)
    return bad
for it in range(6):
    runs = build_runs()
    lines = write_runs(runs)
    bad = verify(runs) & set(n for n in st if st[n][0])
    if not bad:
        break
    print('demoted (not matching inside their run): ' + ' '.join(sorted(bad)), file=sys.stderr)
    for n in bad:
        st[n] = (False,) + st[n][1:]
for k, run in enumerate(runs):
    print('%s: %d functions' % (rpath(k), len(run)), file=sys.stderr)
print('\n'.join(lines))
