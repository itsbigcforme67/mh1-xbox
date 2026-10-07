#!/usr/bin/env python3
"""lbtu3.py NAME START END OUTFILE: like lbtu2.py but resolves conflicting declarations between the merged runs:
a declaration (extern object, typedef, #define) whose identifier is already declared differently by an earlier run is
renamed inside its own run (name_cN); renamed externs get a linker alias line printed to OUTFILE.alias
(config/lobby_aliases.txt format). Writes only OUTFILE (+ .alias); config is not touched."""
import re, struct, sys, os, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__))); os.chdir(ROOT)
name, S, E, outf = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4]
BASE = 0x533980
bindata = open('disc/mh1/split/lobby.bin', 'rb').read()
syms = {}; symaddr = {}
for l in open('config/symbols/lobby.txt'):
    m = re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+);(.*)', l)
    if m:
        symaddr[m.group(1)] = int(m.group(2), 16)
        m2 = re.search(r'type:func size:0x([0-9A-Fa-f]+)', m.group(3))
        if m2: syms[int(m.group(2), 16)] = (int(m2.group(1), 16), m.group(1))
for l in open('config/symbols/main.txt'):
    m = re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+)', l)
    if m: symaddr.setdefault(m.group(1), int(m.group(2), 16))
runs = []
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p) >= 4 and p[0] == 'lobby' and S <= int(p[1], 16) < E:
        runs.append((int(p[1], 16), int(p[2], 16), p[3]))
runs.sort()
pat = re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n', re.M)
def split_chunks(s):
    chunks = []; pos = 0
    for m in pat.finditer(s):
        if m.start() < pos: continue
        end = (m.end() + 2) if s[m.end():m.end() + 2] == '}\n' else s.index('\n}\n', m.end()) + 3
        start = m.start(); pre = s[pos:start]
        t = pre.rstrip('\n'); cs = start
        if t.endswith('*/'):
            c = t.rfind('/*')
            if c >= 0 and '\n\n' not in t[c:]: cs = pos + c; pre = s[pos:cs]
        chunks.append((None, pre)); chunks.append((m.group(1), s[cs:end])); pos = end
    chunks.append((None, s[pos:]))
    return chunks
def units(text):
    """split declaration text into units (statements / #lines / comments)"""
    out = []; cur = []; depth = 0
    for ln in text.split('\n'):
        if not cur and (not ln.strip() or ln.lstrip().startswith('#') or ln.strip().startswith('/*') and ln.strip().endswith('*/')):
            out.append(ln); continue
        cur.append(ln); depth += ln.count('{') - ln.count('}')
        if depth <= 0 and ln.rstrip().endswith(';'):
            out.append('\n'.join(cur)); cur = []; depth = 0
    if cur: out.append('\n'.join(cur))
    return out
def ident(u):
    s = u.strip()
    m = re.match(r'#define\s+(\w+)', s)
    if m: return ('def', m.group(1))
    if s.startswith('typedef'):
        m = re.search(r'(\w+)\s*(\[[^\]]*\])?\s*;\s*$', s)
        return ('type', m.group(1)) if m else None
    if s.startswith('extern'):
        m = re.search(r'(\w+)\s*(\[[^\]]*\])*\s*;\s*$', s)
        if m and '(' not in s: return ('obj', m.group(1))
        m = re.search(r'(\w+)\s*\(', s)
        return ('fn', m.group(1)) if m else None
    m = re.match(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^{}]*\);$', s)
    if m: return ('fn', m.group(1))
    return None
def norm(u): return re.sub(r'\s+', ' ', u.strip())
sfx = {}  # base name -> address-suffixed symbol name (functions defined in the range whose symbol carries _ADDR)
for ad, (sz, n) in syms.items():
    m = re.match(r'(.*)_[0-9A-F]{6,8}$', n)
    if m and S <= ad < E and m.group(1) not in symaddr: sfx[m.group(1)] = n
ansi = set(); allnames = set(n for ad, (sz, n) in syms.items() if S <= ad < E)
for a, b, r in runs:
    s0 = open('src/lobby/%s.c' % r).read()
    for mm in pat.finditer(s0):
        first = mm.group(0).split('{')[0]
        par = re.search(r'\w\(([^)]*)\)', first)
        if par and par.group(1).strip() not in ('', 'void') and not re.search(r'\)\s*\n\s*\w', first.strip()):
            ansi.add(mm.group(1))
print('ansi', len(ansi), file=sys.stderr)
knr = []
seen = {}  # (kind,name) -> normalized text
decls = []; aliases = []; items = []; renamed = {}
def seed(path, done=set()):
    if path in done: return
    done.add(path)
    tx = open(path).read()
    for m in re.finditer(r'^#include "(\w+\.h)"', tx, re.M):
        if os.path.exists('include/' + m.group(1)): seed('include/' + m.group(1))
    for u in units(tx):
        k = ident(u)
        if k is not None: seen[k] = norm(u)
INC = re.compile(r'^#include "(lobby_f|lobby_b|lobby_a|lobby_s|lbui_proto|lbnet)\.h"$')
NOB = bool(os.environ.get('LBTU_NOB'))  # village TUs: runs include lobby.h, not the lobby_b.h family
HDR = [h for h in os.environ.get('LBTU_HDR', '').split(',') if h]  # headers every run includes first (seeded + emitted at the top)
if not NOB: seed('include/lobby_b.h'); seed('include/lbnet.h')
for h in HDR: seed('include/' + h)
for ri, (a, b, r) in enumerate(runs):
    s = open('src/lobby/%s.c' % r).read()
    s = re.sub(r'^/\*.*?\*/\n', '', s, count=1, flags=re.S)
    cks = split_chunks(s)
    ren = {}
    # first pass: decide renames for this file
    pre_units = []
    for n, t in cks:
        if n is None:
            for u in units(t):
                pre_units.append(u)
    for u in pre_units:
        k = ident(u)
        if k is None or INC.match(u.strip()): continue
        if k in seen and seen[k] != norm(u) and k[0] in ('obj', 'type', 'def'):
            nn = '%s_c%d' % (k[1], ri); ren[k[1]] = nn
            if k[0] == 'obj':
                if k[1] in symaddr: aliases.append('%s = 0x%08X;' % (nn, symaddr[k[1]]))
                else: print('NO ADDRESS for', k[1], file=sys.stderr)
    defhere = set(n for n, tx in cks if n)
    for u in pre_units:  # ANSI prototype in this run that differs from an earlier run's declaration: private alias name
        k = ident(u)
        if k is None or k[0] != 'fn' or k[1] in ren or k[1] in defhere or INC.match(u.strip()): continue
        if u.lstrip().startswith(('extern', 'asm', 'static')): continue
        par = re.search(r'\w\(([^)]*)\)\s*;', u)
        if not par or par.group(1).strip() in ('', 'void') or not re.search(r'\w\s+\**\w+\s*(,|$)|\*', par.group(1)): continue
        if ((k in seen and seen[k] != norm(u)) or k[1] in ansi) and k[1] in symaddr:
            ren[k[1]] = '%s_a%d' % (k[1], ri); aliases.append('%s = 0x%08X;' % (ren[k[1]], symaddr[k[1]]))
    bodytxt = '\n'.join(tx for n, tx in cks if n)
    for u in pre_units:
        k = ident(u)
        if k and k[0] == 'obj' and k[1] in allnames and k[1] not in defhere and k[1] not in ren:
            ren[k[1]] = k[1] + '_o'; aliases.append('%s_o = 0x%08X;' % (k[1], symaddr[k[1]]))
    for n0 in sorted(ansi):
        if n0 not in defhere and n0 not in ren and re.search(r'\b%s\s*\(' % re.escape(n0), bodytxt):
            ren[n0] = n0 + '_k'; aliases.append('%s_k = 0x%08X;' % (n0, symaddr[n0])); knr.append('int %s_k();' % n0)
    changed = True
    while changed:
        changed = False
        for u in pre_units:
            k = ident(u)
            if k is None or k[0] != 'obj' or k[1] in ren: continue
            if any(re.search(r'\b%s\b' % re.escape(o), u) for o in ren):
                nn = '%s_c%d' % (k[1], ri); ren[k[1]] = nn; changed = True
                if k[1] in symaddr: aliases.append('%s = 0x%08X;' % (nn, symaddr[k[1]]))
    def rn(x):
        for o, nw in ren.items(): x = re.sub(r'\b%s\b' % re.escape(o), nw, x)
        return x
    for n, t in cks:
        if n is None:
            for u in units(t):
                u2 = rn(u); k = ident(u2)
                if INC.match(u.strip()): continue
                if k is not None and k[0] == 'fn' and k[1].endswith('_k') and k[1][:-2] in ansi: continue
                if k is not None:
                    if k in seen and seen[k] == norm(u2): continue
                    if k in seen and k[0] == 'fn': continue
                    seen[k] = norm(u2)
                elif not u2.strip(): continue
                if re.match(r'extern\s+\w+\s+(s64|u64|s32|u32|u8|s8|u16|s16|int|char|f32)\[', u2.strip()): continue
                decls.append(u2)
        else:
            items.append((symaddr[sfx.get(n, n)], n, re.sub(r'\b%s\(void\)(\s*\{)' % re.escape(n), r'%s()\1' % n, rn(t))))
cn = set(i[1] for i in items); caddrs = set(i[0] for i in items)
sizes = {ad: sz for ad, (sz, n) in syms.items()}
for ad, (sz, n) in sorted(syms.items()):
    if not (S <= ad < E) or ad in caddrs: continue
    w = struct.unpack_from('<I', bindata, ad - BASE)[0]
    if sz == 4 and w == 0: continue
    items.append((ad, n, None))
items.sort()
cdefs = set(i[1] for i in items if i[2] is not None) | set(n for ad, (sz, n) in syms.items() if S <= ad < E)
def keep(u):
    k = ident(u)
    return not (k is not None and k[0] in ('fn', 'obj') and k[1] in cdefs and not k[1].endswith(('_o', '_k')))
protos = {}
for u in decls:
    k = ident(u)
    if k and k[0] == 'fn' and not u.lstrip().startswith(('extern', 'asm', 'static')) and re.search(r'\w\s+\**\w+[,)]|\*', u[u.index('('):] if '(' in u else '') and not re.search(r'\(\s*(void)?\s*\)', u): protos.setdefault(k[1], u)
decls = [u for u in decls if keep(u)]
fwd = []
for ad, n, tx in items:
    if tx is None: continue
    mm = re.search(r'^((?:static\s+)?[A-Za-z_][\w \*]*?)\b%s\(([^)]*)\)([^{]*)\{' % re.escape(n), tx, re.M)
    if not mm: continue
    ret = mm.group(1).strip(); params = mm.group(2).strip(); kr = mm.group(3).strip()
    if ret.startswith('static'): ret = ret[6:].strip()
    if kr and n in protos: fwd.append(protos[n]); continue
    if kr or params in ('', 'void') or not re.search(r'\w\s+\**\w+$|\*', params.split(',')[0]): fwd.append('%s %s();' % (ret, n))
    else: fwd.append('%s %s(%s);' % (ret, n, params))
decls = decls + fwd + sorted(set(knr))
body = '\n'.join(decls)
out = ['/* %s - one translation unit 0x%08X-0x%08X (lbtu3). */' % (name, S, E) + (''.join('\n#include "%s"' % h for h in HDR) if NOB else '\n#define Lbs_MatchStart Lbs_MatchStart_hdr\n#include "lobby_b.h"\n#undef Lbs_MatchStart\ntypedef struct CNET_W5D4 { s32 w[0x175]; } CNET_W5D4;'), body]
raw = []
for ad, n, t in items:
    if t is None:
        mt = re.search(r'^([A-Za-z_][\w \*]*?)\b%s\(([^)]*)\);' % re.escape(n), body, re.M)
        sig = ('%s %s(%s)' % (mt.group(1).strip(), n, mt.group(2))) if mt and not mt.group(1).strip().startswith(('asm', 'static', 'extern', 'typedef')) else 'int %s()' % n
        out.append('#ifdef __MWERKS__\nasm %s\n{\n#include "%s.inc"\n}\n#endif\n' % (sig, n)); raw.append('lobby 0x%08X 0x%X %s' % (ad, sizes[ad], n))
    else: out.append(t)
open(outf, 'w').write('\n'.join(out) + '\n')
open(outf + '.alias', 'w').write('\n'.join(aliases) + '\n')
open(outf + '.raw', 'w').write('\n'.join(raw) + '\n')
print('items', len(items), 'raw', len(raw), 'aliases', len(aliases))
