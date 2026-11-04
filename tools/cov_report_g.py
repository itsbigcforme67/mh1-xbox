#!/usr/bin/env python3
"""cov_report_g.py COVFILE [BIN] : what the Monster Hunter G PC build ran that is still MH1 code where G changed it.

Run a COV=1 G build (`COV=1 tools/build_pc_mhg.sh`) with RT_COV=COVFILE (src/pc/rt/rt_cov.c; several runs may append
to one file). For every function the runs entered, this looks up its MH1 name in docs/survey/mhg_map.csv (G twin
class: identical / shape / edited / rewritten / missing) and reads the function's own source lines (DWARF: nm -l, up to
the next function of the same file) for a G marker (VERSION_MHG, rt_ver_mhg, RTVA, UDO, MHG), and whether the function
sits in the G branch of an #if on VERSION_MHG. Prints:
  - counts per class of the functions entered;
  - G-CHANGED: entered functions whose G twin is missing / rewritten / edited / shape (shape: same code, other constants or
    struct offsets), with no G marker in their body (the PC runs MH1's logic where G's differs: candidates for a G port);
  - the zero-filled G tables (build/mhg/pctree/build/pc/mhg_tables_missing.txt) that an entered function refers to.
Names and addresses only. Standard library + binutils (nm, objdump)."""
import csv, os, re, subprocess, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
cov = sys.argv[1]
binp = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, 'build/mhg/pc/mhview_g')
PCB = os.path.join(ROOT, 'build/mhg/pctree/build/pc')
MARK = re.compile(r'VERSION_MHG|rt_ver_mhg|\bRTVA|\bUDO\b|MHG')

syms, where = {}, {}
for l in subprocess.run(['nm', '-n', '-l', binp], capture_output=True, text=True).stdout.split('\n'):
    p = l.split(None, 3)
    if len(p) >= 3 and p[1] in 'tTwW':
        a = int(p[0], 16)
        name = p[2].split('\t')[0]
        syms[a] = name
        m = re.search(r'\t(/.*):(\d+)$', l)
        if m:
            where[name] = (m.group(1), int(m.group(2)))
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

# function line ranges per file
starts = {}
for n, (f, ln) in where.items():
    if n.startswith('.L'):      # gcc's local labels (one per jump-table case) are not function starts: they cut bodies short
        continue
    starts.setdefault(f, []).append((ln, n))
end_of, srccache = {}, {}
for f, lst in starts.items():
    lst.sort()
    for i, (ln, n) in enumerate(lst):
        end_of[n] = lst[i + 1][0] - 1 if i + 1 < len(lst) else ln + 400
def lines(f):
    if f not in srccache:
        try: srccache[f] = open(f, errors='replace').read().split('\n')
        except OSError: srccache[f] = []
    return srccache[f]
def body(n):
    if n not in where: return ''
    f, ln = where[n]
    return '\n'.join(lines(f)[ln - 1:end_of[n]])
def in_g_block(n):
    """the function's first line lies in the G branch of an #if on VERSION_MHG (#ifdef/#if ... VERSION_MHG: the
    first branch; #ifndef VERSION_MHG / #if !VERSION_MHG: the #else branch)"""
    if n not in where: return False
    f, ln = where[n]
    st = []
    for l in lines(f)[:ln - 1]:
        t = l.strip()
        if not t.startswith('#'): continue
        d = t[1:].strip()
        if d.startswith('if'):
            neg = d.startswith('ifndef') or bool(re.match(r'if\s*!\s*(defined\s*\(?\s*)?VERSION_MHG', d))
            st.append(['MHG' in d, neg])
        elif d.startswith('else') or d.startswith('elif'):
            if st: st[-1][1] = not st[-1][1]
        elif d.startswith('endif'):
            if st: st.pop()
    return any(m and not neg for m, neg in st)

gmap = {}
for r in csv.DictReader(open(os.path.join(ROOT, 'docs/survey/mhg_map.csv'))):
    if r['class'] != 'g-only' and r['name']:
        gmap[r['name']] = r
cls = {}
changed = []
for n in covered:
    r = gmap.get(n)
    c = r['class'] if r else 'pc-only'
    cls[c] = cls.get(c, 0) + 1
    if c in ('edited', 'rewritten', 'missing', 'shape') and not MARK.search(body(n)) and not in_g_block(n):
        sim = float(r['sim'] or 0)
        changed.append((c, sim, int(r['size']), n, r['g_module'], r['g_addr'], os.path.relpath(where[n][0], os.path.join(ROOT, 'build/mhg/pctree')) if n in where else '?'))
print('functions entered:', len(covered))
for c in sorted(cls): print('  %-10s %d' % (c, cls[c]))
order = {'missing': 0, 'rewritten': 1, 'edited': 2, 'shape': 3}
changed.sort(key=lambda x: (order[x[0]], x[1], -x[2]))
print('\nG-CHANGED: entered, G twin %s, no G marker in the PC body (%d)' % ('missing/rewritten/edited/shape', len(changed)))
print('class      sim  mh1size name                              G twin              source')
for c, sim, sz, n, gm, ga, src in changed:
    print('%-9s %4.2f %7d %-33s %-8s %-10s %s' % (c, sim, sz, n, gm, ga, src))

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
                if not m.group(1).startswith('.L'): fn = m.group(1)
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
