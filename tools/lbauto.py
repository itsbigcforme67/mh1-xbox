#!/usr/bin/env python3
"""lbauto.py [-j N] [--out FILE] NAME...|--all : automatic first-draft attempt for lobby functions.
For each function: take the m2c draft (see tools/lbd.py), convert it (F() field macros, K&R externs), compile it alone
with tools/check.py --module lobby, add missing declarations from the compiler's 'undefined identifier' errors, and
report OK / N-of-M instructions differ / error. Results go to a JSON file; matching C is saved under
build/lbauto/NAME.c (not committed). Standard library only."""
import sys, os, re, json, glob, subprocess, concurrent.futures as cf
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
D = os.environ.get('LBDRAFTS', '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/drafts')
txt = ''.join(open(f).read() for f in sorted(glob.glob(D + '/d*.c')))
OUT = os.path.join(ROOT, 'build/lbauto'); os.makedirs(OUT, exist_ok=True)
GP = {'-0x432C': 'cw'}
GPV = 0x38EB70
SYMS = []
for l in open(os.path.join(ROOT, 'config/symbols/main.txt')).readlines() + open(os.path.join(ROOT, 'config/symbols/lobby.txt')).readlines():
    m = re.match(r'(\S+) = 0x([0-9A-F]+); // (?:type:\w+ )?size:0x([0-9A-F]+)', l)
    if m: SYMS.append((int(m[2], 16), m[1], int(m[3], 16)))
SYMS.sort()
TS = {'u8': 1, 's8': 1, 'u16': 2, 's16': 2, 's32': 4, 'u32': 4, 'f32': 4, 'void': 4, 'int': 4, 's64': 8, 'u64': 8, 'f64': 8}
def gpsym(T, off):
    addr = GPV + int(off, 16) if not off.startswith('-') else GPV - int(off[1:], 16)
    for a, n, sz in SYMS:
        if a <= addr < a + max(sz, 1):
            return a, n, sz, addr
    return None

def _balanced(s, i):
    d = 0
    for j in range(i, len(s)):
        if s[j] == '(': d += 1
        elif s[j] == ')':
            d -= 1
            if d == 0: return j + 1
    raise ValueError

def conv_fields(s, decls):
    out = []; i = 0; key = 'M2C_FIELD('
    while True:
        j = s.find(key, i)
        if j < 0:
            out.append(s[i:]); break
        out.append(s[i:j])
        e = _balanced(s, j + len(key) - 1)
        inner = conv_fields(s[j + len(key):e - 1], decls)
        parts, d, cur = [], 0, ''
        for ch in inner:
            if ch in '([': d += 1
            elif ch in ')]': d -= 1
            if ch == ',' and d == 0: parts.append(cur.strip()); cur = ''
            else: cur += ch
        parts.append(cur.strip())
        if len(parts) == 3:
            base, ty, off = parts
            ty = re.sub(r'\s*\*$', '', ty).strip()
            if base == 'saved_reg_gp':
                m = re.match(r'(-?0x[0-9A-Fa-f]+)$', off)
                out.append(gprep2(ty, m.group(1) if m else off, decls) if m else 'GPBAD')
            else:
                out.append('F(%s, %s, %s)' % (ty, base, off))
        else:
            out.append(s[j:e])
        i = e
    return ''.join(out)

def gprep2(T, off, decls):
    ptr = '*' in T or T.startswith('void')
    T = T.replace('*', '').strip()
    if off in GP: return '(%s)cw' % ('u8 *' if T in ('void', 'u8', 's8') else T)
    r = gpsym(T, off)
    if not r: return 'GPBAD'
    a, n, sz, addr = r
    if n == 'cw': return 'cw'
    TT = 'u8 *' if ptr else T
    size = 4 if ptr else TS.get(T, 4)
    if addr == a and size <= sz:
        decls.append('extern %s %s;' % (TT, n)); return n
    decls.append('extern u8 %s[%d];' % (n, max(sz, 1)))
    return '(*(%s *)(%s + %d))' % (TT, n, addr - a)

def make_ah():
    src = open(os.path.join(ROOT, 'include/lobby_f.h')).read()
    out = []
    for l in src.split('\n'):
        m = re.match(r'^((?:[\w\*]+\s+)+\**)(\w+)\((.*)\);\s*$', l)
        if m and not l.startswith(('#', 'typedef', 'extern')):
            out.append('%s%s();' % (m.group(1), m.group(2)))
        else: out.append(l)
    t = '\n'.join(out).replace('#ifndef LOBBY_F_H\n#define LOBBY_F_H', '#ifndef LOBBY_A_H\n#define LOBBY_A_H\n/* generated from lobby.h by tools/lbauto.py: function prototypes replaced by K&R declarations */')
    open(os.path.join(ROOT, 'include/lobby_a.h'), 'w').write(t)
make_ah()

def to_int_mode(body):
    # pointer-typed identifiers become int; derefs/indexing get explicit casts
    ptrs = {}
    def decl(m):
        T = m.group(1).strip(); name = m.group(3)
        T = re.sub(r'\s*\*+$', '', T).strip()
        T = {'void': 'u8', 'int': 'u8', 'M2C_UNK': 'u8'}.get(T, T)
        ptrs[name] = T
        return 'int ' + name
    body = re.sub(r'\b((?:const )?(?:void|u8|s8|u16|s16|s32|u32|f32|int|char|M2C_UNK)(?:\s*\*)+)\s*(\*?)(arg\d|var_\w+|temp_\w+)\b', lambda m: decl(m), body)
    for name, T in ptrs.items():
        body = re.sub(r'(?<![\w\)\]])\*%s\b' % name, '(*(%s *)%s)' % (T, name), body)
        body = re.sub(r'\b%s\[' % name, '((%s *)%s)[' % (T, name), body)
    body = re.sub(r'\bF\(\s*(?:\w+\s+)*\*+\s*,', 'F(int,', body)
    body = re.sub(r'^(?:s32|void|u8|s8|u16|s16|u32)\s*\*+\s*(\w+\()', r'int \1', body, flags=re.M)
    body = re.sub(r'\(int \*\)(\d+)', r'\1', body)
    body = re.sub(r'= \(u8 \*\)&(\w+);', r'= (int)&\1;', body)
    body = re.sub(r'= &(\w+);', r'= (int)&\1;', body)
    body = re.sub(r'\(u8 \*\)&(\w+) \+ ', r'(int)&\1 + ', body)
    return body

def conv(s, decls):
    s = conv_fields(s, decls)
    def gprep(m):
        T = m.group(1); ptr = m.group(0).count('**') > 0 or 'void' in T
        off = m.group(2)
        if off in GP: return '(%s)cw' % ('u8 *' if T in ('void', 'u8', 's8') else T)
        r = gpsym(T, off)
        if not r: return 'GPBAD'
        a, n, sz, addr = r
        if n == 'cw': return 'cw'
        TT = 'u8 *' if ptr else T
        size = 4 if ptr else TS.get(T, 4)
        if addr == a and size <= sz:
            decls.append('extern %s %s;' % (TT, n))
            return n
        decls.append('extern u8 %s[%d];' % (n, max(sz, 1)))
        return '(*(%s *)(%s + %d))' % (TT, n, addr - a)
    s = re.sub(r'M2C_FIELD\(saved_reg_gp, ([\w ]+?) \*\*?, (-?0x[0-9A-Fa-f]+)\)', gprep, s)
    s = re.sub(r'M2C_FIELD\(([^,]+?), ([\w ]+?) \*, (0x[0-9A-Fa-f]+|\d+)\)', r'F(\2, \1, \3)', s)
    s = re.sub(r'M2C_FIELD\(([^,]+?), ([\w ]+?) \*\*, (0x[0-9A-Fa-f]+|\d+)\)', r'F(\2 *, \1, \3)', s)
    s = re.sub(r'&(\w+) \+ ', r'(u8 *)&\1 + ', s)
    s = re.sub(r'= &(\w+);', r'= (u8 *)&\1;', s)
    s = re.sub(r'\bs64 (arg\d|var_\w+|temp_\w+)', r'int \1', s)
    for reg, n in (('$t0', 4), ('$t1', 5), ('$t2', 6), ('$t3', 7)):
        s = re.sub(r'\*?M2C_ERROR\(/\* Read from unset register \%s \*/\)' % reg.replace('$', '$'), 'arg%d' % n, s)
    s = re.sub(r',\s*\*?M2C_ERROR\(/\* Read from unset register \$a[0-3] \*/\)', '', s)
    s = s.replace('M2C_UNK', 'int')
    s = re.sub(r'\(s64\) \(\(s64\) (\w+) << 0x30\) >> 0x30', r'(s16)\1', s)
    s = re.sub(r'\(s64\) \((\w+) << 0x38\) >> 0x38', r'(s8)\1', s)
    s = re.sub(r'\(s64\) \(\(s64\) (\w+) << 0x38\) >> 0x38', r'(s8)\1', s)
    s = re.sub(r'\(s64\) \((\w+\([^()]*\)) << 0x30\) >> 0x30', r'(s16)\1', s)
    s = s.replace('(s64)', '').replace('(s32) ', '')
    s = s.replace('*(u8 *)0x3F34C1', 'game_w.master').replace('*(u8 *)0x3F3404', 'game_w.stage')
    s = s.replace('*(void *)0x3F34C1', 'game_w.master').replace('*(void *)0x3F3404', 'game_w.stage')
    s = s.replace('NULL', '0').replace('void *arg', 'u8 *arg').replace('(void *)', '(u8 *)')
    return s

def draft(fn):
    m = None
    for mm in re.finditer(r'^[\w\*\s]+\b%s\([^;{]*\) \{\n' % re.escape(fn), txt, re.M):
        m = mm; break
    if not m: return None
    i = m.start(); j = txt.index('\n}\n', i) + 3
    k = txt.rfind('\n}\n', 0, i)
    hdr = txt[k + 3:i]
    return hdr, txt[i:j]

def attempt(fn, mode=''):
    d = draft(fn)
    if d is None: return fn, {'status': 'nodraft'}
    hdr, body = d
    gd = []
    body = conv(body, gd).lstrip('\n')
    if mode == 'int': body = to_int_mode(body)
    for nm, sz in set(re.findall(r'&(sp[0-9A-Fa-f]+)(?:[^;\n]*?), (0x[0-9A-Fa-f]{2,})\)', body)):
        if int(sz, 16) >= 0x10 and re.search(r'\bint %s;' % nm, body):
            body = re.sub(r'\bint %s;' % nm, 'char %s[%s];' % (nm, sz), body)
            body = re.sub(r'&%s\b' % nm, nm, body)
    mm = re.match(r'([^\n]*?\b%s\()([^\n]*)(\) \{\n)' % re.escape(fn), body)
    if mm:
        params = [x.strip() for x in mm.group(2).split(',') if x.strip() and x.strip() != 'void']
        idx = {}
        for x in params:
            q = re.search(r'arg(\d)$', x)
            if q: idx[int(q.group(1))] = x
        used = {int(x) for x in re.findall(r'\barg(\d)\b', body[mm.end():])}
        top = max(set(idx) | used) if (idx or used) else -1
        if top >= 0 and (top + 1 != len(params) or len(idx) != len(params)):
            new = [idx.get(k, 'int arg%d' % k) for k in range(top + 1)]
            body = mm.group(1) + ', '.join(new) + mm.group(3) + body[mm.end():]
    if 'M2C_ERROR' in body or 'GPBAD' in body or 'M2C_' in body:
        return fn, {'status': 'unsupported'}
    decl = set()
    # externs from the draft header: functions
    lines = []
    for l in hdr.split('\n'):
        l = l.strip()
        if not l or l.startswith('/*') or l.startswith('*'): continue
        l = conv(l, gd)
        l = re.sub(r'\s*/\*.*?\*/', '', l)
        lines.append(l)
    path = os.path.join(ROOT, 'src/lobby/zz_auto_%s.c' % re.sub(r'\W', '_', fn))
    extra = sorted(set(gd))
    res = {'status': 'error'}
    try:
        for it in range(6):
            ex2 = extra
            if mode == 'int':
                ex2 = [re.sub(r'^extern (?:\w+\s+)*\*+\s*(\w+);', r'extern int \1;', e) for e in extra]
            src = ('#include "lobby_a.h"\n' if mode == 'int' else '#include "lobby_f.h"\n') + '\n'.join(ex2) + '\n' + body
            open(path, 'w').write(src)
            p = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True, cwd=ROOT)
            out = p.stdout + p.stderr
            m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/(\d+))?' % re.escape(fn), out, re.M)
            if m:
                if m.group(1) == 'OK':
                    res = {'status': 'OK'}
                    open(os.path.join(OUT, fn + '.c'), 'w').write(src)
                else:
                    res = {'status': 'diff', 'd': int(m.group(2)), 'n': int(m.group(3))}
                    open(os.path.join(OUT, fn + '.c'), 'w').write(src)
                break
            if '??' in out and fn in out:
                res = {'status': 'nosym'}; break
            added = False
            for mm in re.finditer(r"undefined identifier '(\w+)'", out):
                name = mm.group(1)
                if name in ('arg0', 'arg1', 'arg2', 'arg3'): continue
                if any(('%s(' % name) in l or ('%s;' % name) in l for l in extra): continue
                # find how it is used in the body
                if re.search(r'\b%s\s*\(' % name, body): extra.append('int %s();' % name)
                else: extra.append('extern char %s[];' % name)
                added = True
            for mm in re.finditer(r"'?(\w+)\(\.\.\.\)'? redeclared|identifier '(\w+)\(\.\.\.\)' redeclared", out):
                pass
            if not added:
                res = {'status': 'error', 'msg': out.strip().split('\n')[3:6]}
                break
    finally:
        if os.path.exists(path): os.remove(path)
    return fn, res

if __name__ == '__main__':
    args = sys.argv[1:]
    jobs = 4; outf = os.path.join(ROOT, 'build/lbauto.json')
    names = []
    i = 0
    while i < len(args):
        if args[i] == '-j': jobs = int(args[i + 1]); i += 2
        elif args[i] == '--out': outf = args[i + 1]; i += 2
        elif args[i] == '--all':
            S = os.path.dirname(D)
            reg = []
            for l in open(os.path.join(ROOT, 'config/c_files.txt')):
                q = l.split()
                if len(q) >= 4 and q[0] == 'lobby': reg.append((int(q[1], 16), int(q[2], 16)))
            for l in open(os.path.join(S, 'fl.txt')):
                if not l.strip(): continue
                a, nm, sz = l.split()
                a = int(a, 16)
                if any(x <= a < y for x, y in reg) or re.search(r'_[0-9A-F]{6,8}$', nm): continue
                names.append(nm)
            i += 1
        else: names.append(args[i]); i += 1
    results = {}
    with cf.ThreadPoolExecutor(jobs) as ex:
        for fn, r in ex.map((lambda f: attempt(f, 'int')) if os.environ.get('INTONLY') else attempt, names):
            if r.get('status') in ('error', 'diff') and not os.environ.get('NOINT') and not os.environ.get('INTONLY'):
                fn2, r2 = attempt(fn, 'int')
                if r2.get('status') == 'OK' or (r2.get('status') == 'diff' and (r.get('status') == 'error' or r2['d'] < r['d'])):
                    r = r2; r['int'] = True
            results[fn] = r
            print(fn, r.get('status'), r.get('d', ''), r.get('n', ''), flush=True)
            json.dump(results, open(outf, 'w'))
