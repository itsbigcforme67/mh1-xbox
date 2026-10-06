#!/usr/bin/env python3
"""mkruns3.py MODULE NM.c OUTDIR PREFIX FIRSTNUM "comment" [--only a,b] [--skip a,b]
Splits the matching, address-contiguous runs of a near-match file NM.c into OUTDIR/PREFIXNN.c and
prints the config/c_files.txt lines (text and jump-table rodata ranges).
"Matching" = zero real differences by tools/alignall.py (relocation noise ignored) and every call
target name equal to the original's up to the address suffix. Static functions that are called from
another run, or from a function that is not in any run (it stays asm), lose their `static`
(otherwise the link fails). rebuild.sh stays the final judge. (agent D)"""
import re, subprocess, sys, os, glob
mod, nm, outdir, prefix, first, cmt = sys.argv[1:7]
VERIFY = '--verify' in sys.argv
first = int(first)
only = skip = None
for i, a in enumerate(sys.argv):
    if a == '--only': only = set(sys.argv[i + 1].split(','))
    if a == '--skip': skip = set(sys.argv[i + 1].split(','))
chk = subprocess.run(['python3', 'tools/check.py', nm, '-v'], capture_output=True, text=True).stdout
al = subprocess.run(['python3', 'tools/alignall.py', nm], capture_output=True, text=True).stdout
real = {}
for l in al.split('\n'):
    m = re.match(r'^(OK|--)\s+(\S+)\s+(\d+)', l)
    if m: real[m.group(2)] = int(m.group(3))
rows = []
cur = None
def strip(n): return re.sub(r'_(?:[0-9A-F]{6,8}|i|s)$', '', n)   # _i = alias with a different prototype (config/main_aliases.txt)
for l in chk.split('\n'):
    m = re.match(r'^(OK|--)  (\S+)\s+%s\s+0x([0-9A-F]+)\s+(\d+) bytes' % mod, l)
    if m:
        cur = [m.group(2), int(m.group(3), 16), int(m.group(4)), True]
        rows.append(cur); continue
    m = re.search(r'\(calls (\S+), original calls (\S+)\)', l)
    if m and cur:
        a, b = m.group(1), m.group(2)
        ok = b == '?' or any(strip(a) == strip(x) or a == x for x in b.split('/'))   # '?' = target without a symbol (overlay by address): the build links it
        if not ok: cur[3] = False
rows = [r for r in rows if real.get(r[0], 1) == 0 and r[3] and (only is None or r[0] in only) and (skip is None or r[0] not in skip)]
rows.sort(key=lambda r: r[1])
runs = []; cur = []
for r in rows:
    if cur and 0 <= r[1] - (cur[-1][1] + cur[-1][2]) < 16: cur.append(r)
    else:
        if cur: runs.append(cur)
        cur = [r]
if cur: runs.append(cur)
src = open(nm).read()
# function bodies and static info
pat = re.compile(r'^((?:static )?(?:[A-Za-z_][\w \*]*?\b))(\w+)\([^;{]*\)\s*\{\n', re.M)
defs = {}
for m in pat.finditer(src):
    end = (m.end() + 2) if src[m.end():m.end() + 2] == '}\n' else src.index('\n}\n', m.end()) + 3
    defs[m.group(2)] = (m.group(1).startswith('static'), src[m.end():end])
runof = {}
for i, run in enumerate(runs):
    for r in run: runof[r[0]] = i
glob_names = set()
for name, (st, body) in defs.items():
    for callee in defs:
        if callee == name: continue
        if re.search(r'\b%s\b' % re.escape(callee), body):
            if defs[callee][0] and runof.get(callee, -1) != runof.get(name, -2):
                glob_names.add(callee)
# jump tables: lit_* referenced by a run's functions in the original asm
asmtxt = ''
for f in glob.glob('asm/%s/text/*.s' % mod): asmtxt += open(f).read() + '\n'
datatxt0 = ''
for f in glob.glob('asm/%s/data/data/*.s' % mod): datatxt0 += open(f).read() + '\n'
# a static function whose symbol is still referenced by asm (a function that stays asm, or a data
# table) must be global; the asm of functions that are in a run is gone after a rebuild, but the
# original text of the file (if still present) lists them as glabels: ignore those definitions.
for g, (st, body) in defs.items():
    if st and g in runof:
        refs = re.findall(r'\b%s\b' % re.escape(g), asmtxt + datatxt0)
        own = len(re.findall(r'(?m)^(?:glabel|endlabel|nonmatching) %s\b' % re.escape(g), asmtxt))
        if len(refs) > own * 2 + (1 if own else 0) and len(refs) - own * 3 > 0:
            glob_names.add(g)
datatxt = ''
for f in glob.glob('asm/%s/data/data/*.s' % mod): datatxt += open(f).read() + '\n'
import struct
_img = open('disc/mh1/split/%s.bin' % mod, 'rb').read()
_vram = struct.unpack_from('<I', _img, 8)[0] if mod != 'main' else 0x100000
def _w(a): return struct.unpack_from('<I', _img, a - _vram)[0]
def func_tables(fa, fsz):
    """jump tables of the function at fa: lui+addiu/ori pairs that point at words which all fall inside the function"""
    found = []
    ins = [_w(fa + 4 * k) for k in range(fsz // 4)]
    for k, w in enumerate(ins):
        if w >> 26 == 0x0F:                      # lui rt, hi
            rt = (w >> 16) & 31; hi = w & 0xFFFF
            for d in range(1, 8):
                if k + d >= len(ins): break
                v = ins[k + d]
                if (v >> 26) in (0x09, 0x0D) and ((v >> 21) & 31) == rt:   # addiu / ori rt, rt, lo
                    lo = v & 0xFFFF
                    if (v >> 26) == 0x09 and lo & 0x8000: lo -= 0x10000
                    a = (hi << 16) + lo
                    if (0x340000 <= a < 0x3C0000 if mod == 'main' else 0x680000 <= a < 0x6C0000) and a % 4 == 0:
                        n = 0
                        while n < 600 and a - _vram + 4 * n + 4 <= len(_img):
                            t = _w(a + 4 * n)
                            if fa <= t < fa + fsz and t % 4 == 0: n += 1
                            else: break
                        if n >= 2: found.append((a, a + 4 * n))
                    break
    return found
def lit_range(x): return x
n = first
for run in runs:
    name = '%s%02d' % (prefix, n)
    path = '%s/%s.c' % (outdir, name)
    s, e = run[0][1], run[-1][1] + run[-1][2]
    hdr = '%s - %s 0x%08X-0x%08X: %s. Whole file in %s.' % (name, cmt, s, e, ', '.join(r[0] for r in run), os.path.basename(nm))
    subprocess.run(['python3', 'tools/mkrun2.py', nm, path, hdr] + [r[0] for r in run], check=True)
    txt = open(path).read()
    for g in glob_names:
        txt = re.sub(r'(?m)^static ((?:[A-Za-z_][\w \*]*?\b)%s\()' % re.escape(g), r'\1', txt)
    open(path, 'w').write(txt)
    print('%s 0x%08X 0x%08X %s   # %d funcs: %s..%s' % (mod, s, e, path[len('src/%s/' % mod):-2] if path.startswith('src/%s/' % mod) else name, len(run), run[0][0], run[-1][0]))
    rng = []
    for r in run:
        rng += func_tables(r[1], r[2])
    rng.sort(); merged = []
    for a, b in rng:
        if merged and a <= merged[-1][1] + 12 and all(_w(x) == 0 for x in range(merged[-1][1], a, 4)): merged[-1][1] = max(merged[-1][1], b)
        else: merged.append([a, b])
    for a, b in merged:
        print('%s:rodata 0x%08X 0x%08X %s' % (mod, a, b, path[len('src/%s/' % mod):-2] if path.startswith('src/%s/' % mod) else name))
    n += 1
print('# global (non-static) names:', ', '.join(sorted(glob_names)))

if VERIFY:
    bad = set()
    for fpath in sorted(glob.glob('%s/%s[0-9][0-9].c' % (outdir, prefix))):
        o = subprocess.run(['python3', 'tools/alignall.py', fpath], capture_output=True, text=True).stdout
        for l in o.split('\n'):
            m = re.match(r'^--\s+(\S+)\s+(\d+)', l)
            if m and int(m.group(2)) > 0: bad.add(m.group(1))
    print('# verify: functions that do not match inside their run:', ', '.join(sorted(bad)) or 'none')
