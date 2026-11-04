#!/usr/bin/env python3
"""cov_report_g.py COVFILE [BIN] : what the Monster Hunter G PC build ran that is still MH1 code where G changed it.
cov_report_g.py --names LIST [BIN] : the same test on a list of names (an earlier report such as
docs/survey/mhg_cov_g.txt, whose G-CHANGED rows are read, or one name per line) instead of a coverage dump: re-checks
an old coverage run against today's code without a COV=1 rebuild.

Run a COV=1 G build (`COV=1 tools/build_pc_mhg.sh`) with RT_COV=COVFILE (src/pc/rt/rt_cov.c; several runs may append
to one file). For every function the runs entered, this looks up its MH1 name in docs/survey/mhg_map.csv (G twin
class: identical / shape / edited / rewritten / missing) and reads the function's own source lines (DWARF: nm -l, up to
the next function of the same file or the function's closing `}` in column 0) for a G marker (VERSION_MHG, rt_ver_mhg,
RTVA, UDO, MHG), whether the function sits in the G branch of an #if on VERSION_MHG, and whether its body uses a macro
#defined inside an #if on VERSION_MHG in its own file (G ports often switch through such a macro defined above the
function: em_area_move_init's G_AM, enemy_mv's EMU_KIND_27, Pl_damage_sub's PB). gcc's local labels (.L1234, one per
switch case) are not functions and are skipped (4 Nov 2026: they used to cut a body at its first case label).
Prints:
  - counts per class of the functions entered;
  - G-CHANGED: entered functions whose G twin is missing / rewritten / edited / shape (shape: same code, other constants or
    struct offsets), with no G marker, no G-branch #if and no local G macro (the PC runs MH1's logic where G's differs:
    candidates for a G port). Column `hdr` is `H` when the body uses a macro defined under VERSION_MHG in a header
    (CW_OFS, LBSHOP_OFS, IBO ...): usually G's layout only, counted separately in the heading;
  - the zero-filled G tables (build/mhg/pctree/build/pc/mhg_tables_missing.txt) that an entered function refers to.
Names and addresses only. Standard library + binutils (nm, objdump)."""
import csv, os, re, subprocess, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
args = sys.argv[1:]
names_mode = bool(args) and args[0] == '--names'
if names_mode: args = args[1:]
cov = args[0]
binp = args[1] if len(args) > 1 else os.path.join(ROOT, 'build/mhg/pc/mhview_g')
PCT = os.path.join(ROOT, 'build/mhg/pctree')
PCB = os.path.join(PCT, 'build/pc')
MARK = re.compile(r'VERSION_MHG|rt_ver_mhg|\bRTVA|\bUDO\b|MHG')
CLASSES = ('missing', 'rewritten', 'edited', 'shape')

def is_label(name):
    return name.startswith('.L')      # gcc local labels (switch cases, loop heads): not functions

syms, where = {}, {}
for l in subprocess.run(['nm', '-n', '-l', binp], capture_output=True, text=True).stdout.split('\n'):
    p = l.split(None, 3)
    if len(p) >= 3 and p[1] in 'tTwW':
        a = int(p[0], 16)
        name = p[2].split('\t')[0]
        if is_label(name): continue
        syms[a] = name
        m = re.search(r'\t(/.*):(\d+)$', l)
        if m:
            where[name] = (m.group(1), int(m.group(2)))

src_hint = {}      # names mode: source path from the report (for static functions gcc inlined away)
if names_mode:
    covered = set()
    for l in open(cov, errors='replace'):
        p = l.split()
        if len(p) >= 5 and p[0] in CLASSES and re.match(r'^\d+\.\d+$', p[1]):
            covered.add(p[3]); src_hint[p[3]] = p[-1]
        elif len(p) == 1 and re.match(r'^[A-Za-z_]\w*$', p[0]):
            covered.add(p[0])
else:
    base_sym = [a for a, n in syms.items() if n == 'rt_cov_dump'][0]
    heads, addrs = set(), set()
    for l in open(cov, errors='replace'):
        l = l.strip()
        m = re.match(r'^# (\d+)$', l)
        if m:
            heads.add(int(m.group(1)) - base_sym)
        elif re.match(r'^[0-9a-f]{5,8}$', l):
            addrs.add(int(l, 16))
    covered = set()
    for a in addrs:
        for b in heads:
            if a - b in syms:
                covered.add(syms[a - b]); break

srccache = {}
def lines(f):
    if f not in srccache:
        try: srccache[f] = open(f, errors='replace').read().split('\n')
        except OSError: srccache[f] = []
    return srccache[f]

# names absent from the binary (static functions gcc inlined): find the definition in the reported source file
for n in covered:
    if n in where or n not in src_hint or src_hint[n] == '?': continue
    f = os.path.join(PCT, src_hint[n])
    for i, x in enumerate(lines(f)):
        if re.match(r'^[A-Za-z_].*\b' + re.escape(n) + r'\s*\([^;]*$', x):
            where[n] = (f, i + 1); break

# function line ranges per file: up to the next function, or the closing brace in column 0
starts = {}
for n, (f, ln) in where.items():
    starts.setdefault(f, []).append((ln, n))
end_of = {}
for f, lst in starts.items():
    lst.sort()
    ls = lines(f)
    for i, (ln, n) in enumerate(lst):
        end = lst[i + 1][0] - 1 if i + 1 < len(lst) else ln + 1500
        for k in range(ln - 1, min(end, len(ls))):
            if ls[k].startswith('}'):
                end = k + 1; break
        end_of[n] = max(end, ln)
def body(n):
    if n not in where: return ''
    f, ln = where[n]
    return '\n'.join(lines(f)[ln - 1:end_of[n]])
def version_branch(n):
    """'G' when the function's first line lies in the G branch of an #if on VERSION_MHG (#ifdef/#if ... VERSION_MHG:
    the first branch; #ifndef VERSION_MHG / #if !VERSION_MHG: the #else branch; #elif defined(VERSION_MHG) after an
    #if VERSION_MHG && __MWERKS__ branch: the #elif branch, which is the one the PC compiles), 'MH1' when it lies in
    an MH1-only branch (#ifndef VERSION_MHG, #if ... && !defined(VERSION_MHG), the #else of an #ifdef VERSION_MHG:
    not built for G), else None"""
    if n not in where: return None
    f, ln = where[n]
    st = []     # per open #if: [branch is G, branch is MH1 only, an earlier branch tested G positively, ... negatively]
    def pos(d): return 'MHG' in d and not re.search(r'!\s*(defined\s*\(?\s*)?VERSION_MHG', d)
    def neg(d): return d.startswith('ifndef') and 'MHG' in d or bool(re.search(r'!\s*(defined\s*\(?\s*)?VERSION_MHG', d))
    for l in lines(f)[:ln - 1]:
        t = l.strip()
        if not t.startswith('#'): continue
        d = t[1:].strip()
        if d.startswith('if'):
            st.append([pos(d), neg(d), pos(d), neg(d)])
        elif d.startswith('elif'):
            if st:
                e = st[-1]
                e[0], e[1] = pos(d), neg(d)
                e[2], e[3] = e[2] or pos(d), e[3] or neg(d)
        elif d.startswith('else'):
            if st:
                e = st[-1]
                e[0], e[1] = e[3] and not e[2], e[2] and not e[3]
        elif d.startswith('endif'):
            if st: st.pop()
    if any(e[0] for e in st): return 'G'
    if any(e[1] for e in st): return 'MH1'
    return None
def in_g_block(n):
    return version_branch(n) == 'G'

def g_macros(ls):
    """names #defined inside an #if / #ifdef / #elif on VERSION_MHG (either branch: a macro with a G and an MH1 form
    switches behaviour by version as much as one defined for G only)"""
    out, st = set(), []
    for l in ls:
        t = l.strip()
        if not t.startswith('#'): continue
        d = t[1:].strip()
        if d.startswith('if'): st.append('MHG' in d)
        elif d.startswith('elif'):
            if st: st[-1] = st[-1] or 'MHG' in d
        elif d.startswith('endif'):
            if st: st.pop()
        elif d.startswith('define') and any(st):
            m = re.match(r'define\s+(\w+)', d)
            if m and 'MHG' not in m.group(1): out.add(m.group(1))
    return out
fmac = {}
def local_macros(f):
    if f not in fmac: fmac[f] = g_macros(lines(f))
    return fmac[f]
hmac = set()
for base in ('src', 'include'):
    for dp, dn, fn in os.walk(os.path.join(PCT, base)):
        for x in fn:
            if x.endswith('.h'): hmac |= g_macros(lines(os.path.join(dp, x)))
def idents(b):
    return set(re.findall(r'\b[A-Za-z_]\w*\b', b))

gmap = {}
for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mhg_map.csv'))):
    if r['class'] != 'g-only' and r['name']:
        gmap[r['name']] = r
cls = {}
changed, nlocal, nlinked = [], 0, 0
binnames = set(syms.values())
for n in covered:
    r = gmap.get(n)
    c = r['class'] if r else 'pc-only'
    cls[c] = cls.get(c, 0) + 1
    if c not in CLASSES: continue
    b = body(n)
    if names_mode and (n not in where or (n not in binnames and version_branch(n) == 'MH1')):
        nlinked += 1; continue      # not in today's binary, and its source is gone or MH1-only: reported apart
    if MARK.search(b) or in_g_block(n): continue
    ids = idents(b)
    if n in where and ids & local_macros(where[n][0]):
        nlocal += 1; continue
    hdr = 'H' if ids & hmac else ''
    sim = float(r['sim'] or 0)
    changed.append((c, sim, int(r['size']), n, r['g_module'], r['g_addr'], hdr,
                    os.path.relpath(where[n][0], PCT) if n in where else '?'))
print('functions %s:' % ('listed' if names_mode else 'entered'), len(covered))
for c in sorted(cls): print('  %-10s %d' % (c, cls[c]))
print('G through a macro defined under VERSION_MHG in the function\'s own file: %d' % nlocal)
if names_mode: print('not in this binary, source gone or MH1-only (removed in G, renamed): %d' % nlinked)
order = {'missing': 0, 'rewritten': 1, 'edited': 2, 'shape': 3}
changed.sort(key=lambda x: (order[x[0]], x[1], -x[2]))
nh = sum(1 for x in changed if x[6])
print('\nG-CHANGED: entered, G twin %s, no G marker in the PC body (%d; %d of them use G header macros, hdr H: '
      'usually layout only)' % ('missing/rewritten/edited/shape', len(changed), nh))
print('class      sim  mh1size name                              G twin              hdr source')
for c, sim, sz, n, gm, ga, hdr, src in changed:
    print('%-9s %4.2f %7d %-33s %-8s %-10s %-3s %s' % (c, sim, sz, n, gm, ga, hdr, src))

# zero-filled tables referenced by entered functions
miss = [l.strip() for l in open(os.path.join(PCB, 'mhg_tables_missing.txt')) if l.strip()] if os.path.exists(os.path.join(PCB, 'mhg_tables_missing.txt')) else []
if miss:
    ms = set(miss)
    users = {}
    for o in sorted(os.listdir(PCB)):
        if not o.endswith('.o'): continue
        und = subprocess.run(['nm', '-u', os.path.join(PCB, o)], capture_output=True, text=True).stdout.split()
        if not ms.intersection(und): continue
        fn = None
        for l in subprocess.run(['objdump', '-dr', os.path.join(PCB, o)], capture_output=True, text=True).stdout.split('\n'):
            m = re.match(r'^[0-9a-f]+ <([^>]+)>:', l)
            if m:
                if not is_label(m.group(1)): fn = m.group(1)
                continue
            m = re.search(r'R_386_\w+\s+(\S+)', l)
            if m and fn:
                s = m.group(1).split('+')[0].split('-')[0]
                if s in ms: users.setdefault(s, set()).add(fn)
    print('\nZERO-FILLED G tables (%d) and the entered functions that read them' % len(miss))
    for t in miss:
        u = sorted(users.get(t, ()))
        ran = [f for f in u if f in covered]
        print('%-28s %s%s' % (t, 'RAN: ' + ' '.join(ran) if ran else 'not reached', ('  (also ' + ' '.join(sorted(set(u) - set(ran))) + ')') if set(u) - set(ran) else ''))
