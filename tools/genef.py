#!/usr/bin/env python3
"""genef.py ASMFILE FUNC TABLE_LABEL END DEFAULT OUT.c : translate a monster's ef_move_sub (sound/effect script per animation,
switch on w->anim through a jump table) from asm into C. Run it BEFORE the file is registered in c_files.txt (it reads
asm/game/text/ASMFILE.s and the table from asm/game/data/data/*.s). Example (em01):
  python3 tools/genef.py f_em_566630 ef_move_sub_00574EE0 lit_4394_00685EF0 0x57A768 0x57A760 /tmp/ef_gen.c
The output is the function body (env WTYPE = name of the monster work struct, default EM01W); fix up by hand what it
flags with #error."""
import re, struct, sys
import os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASMFILE, FUNC, TABLE, ENDS, DEFS, OUTC = sys.argv[1:7]
_asm = open(os.path.join(ROOT, 'asm/game/text/%s.s' % ASMFILE)).read()
_asm_func = re.search(r'^glabel %s\n(.*?)^endlabel %s\n' % (re.escape(FUNC), re.escape(FUNC)), _asm, re.M | re.S).group(0)
RAW = re.search(r'^glabel %s\n(.*?)^endlabel %s\n' % (re.escape(FUNC), re.escape(FUNC)), _asm, re.M | re.S).group(0).split('\n')
tbl_words = []
import glob
for f in glob.glob(os.path.join(ROOT, 'asm/game/data/data/*.s')):
    t = open(f).read()
    m = re.search(r'dlabel %s\n(.*?)enddlabel' % TABLE, t, re.S)
    if m:
        tbl_words = [int(x, 16) for x in re.findall(r'\.word (0x[0-9A-Fa-f]+)', m.group(1))]
END = int(ENDS, 16)
DEFAULT = int(DEFS, 16)
GPR = {'$0': 0}
def f32(h): return struct.unpack('>f', struct.pack('>I', h))[0]
def fl(v):
    s = repr(round(v, 7))
    if 'e' in s: s = '%.9g' % v
    if '.' not in s and 'e' not in s: s += '.0'
    return s + 'f'
# parse instructions
ins = []   # (addr, text)
labels = {}  # addr -> True
pending = []
for l in RAW:
    m = re.match(r'\s*/\* \w+ ([0-9A-F]{8}) \w+ \*/\s+(.*)$', l)
    if m:
        ins.append((int(m.group(1), 16), re.sub(r'\s+', ' ', m.group(2).strip())))
        continue
    m = re.match(r'\s*\.L([0-9A-F]{8}):', l)
    if m:
        labels[int(m.group(1), 16)] = True
byaddr = {a: i for i, (a, t) in enumerate(ins)}
def fieldname(off):
    return None
EMFIELDS = {}
# parse em.h for names
emh = open(os.path.join(ROOT, 'include/em.h')).read()
for m in re.finditer(r'^\s+(?:u8|s8|u16|s16|u32|s32|f32|struct \w+ \*)\s+(\w+)(\[[^\]]*\])?;\s*/\* 0x([0-9A-F]+)', emh, re.M):
    EMFIELDS[int(m.group(3), 16)] = (m.group(1), m.group(2))

class Ctx:
    def __init__(self):
        self.r = {}
        self.f = {}
        self.cond = None
        self.orpend = []
        self.hi = {}

def parse_test(i, stopi):
    """return (cond, J, tgt, next_index) for a test starting at ins[i], else None.
    J = jump taken when cond is true."""
    j = i
    # skip leading filler (lui $1 / addiu $4,1 / nop) used by delay slots
    def t(k): return ins[k][1] if k < len(ins) else ''
    def tg(x):
        mm = re.search(r'\.L([0-9A-F]{8})', x)
        return int(mm.group(1), 16) if mm else None
    # a) frame check
    k = j
    f12 = f13 = None; a1 = 0
    consts = {}
    while k < stopi:
        x = t(k)
        mm = re.match(r'lui (\$\d+), \((0x[0-9A-Fa-f]+) >> 16\)', x)
        if mm:
            consts[mm.group(1)] = f32(int(mm.group(2), 16)); k += 1; continue
        mm = re.match(r'mtc1 (\$\d+), (\$f\d+)', x)
        if mm:
            if mm.group(2) == '$f12': f12 = consts.get(mm.group(1))
            if mm.group(2) == '$f13': f13 = consts.get(mm.group(1))
            k += 1; continue
        if re.match(r'mtc1 \$0, (\$f\d+)', x):
            mm2 = re.match(r'mtc1 \$0, (\$f\d+)', x)
            if mm2.group(1) == '$f12': f12 = 0.0
            k += 1; continue
        if re.match(r'daddu \$4, \$16, \$0', x) or re.match(r'daddu \$5, \$0, \$0', x) or re.match(r'addiu \$5, \$0, (0x[0-9A-Fa-f]+|\d+)', x):
            mm = re.match(r'addiu \$5, \$0, (0x[0-9A-Fa-f]+|\d+)', x)
            if mm: a1 = int(mm.group(1), 0)
            k += 1; continue
        break
    x = t(k)
    mm = re.match(r'jal (em_frame_check\w*)', x)
    if mm and f12 is not None:
        fn = mm.group(1)
        # delay slot of jal may set a1
        d = t(k + 1)
        m5 = re.match(r'(?:addiu|daddu) \$5, \$0, (0x[0-9A-Fa-f]+|\d+|\$0)', d)
        if m5 and m5.group(1) != '$0': a1 = int(m5.group(1), 0)
        elif re.match(r'daddu \$5, \$0, \$0', d): a1 = 0
        if fn == 'em_frame_check': c = 'em_frame_check(em, %s, %s)' % (fl(f12), a1)
        elif fn == 'em_frame_check2': c = 'em_frame_check2(em, %s, %s)' % (a1, fl(f12))
        else: c = 'em_frame_check3(em, %s, %s, %s)' % (a1, fl(f12), fl(f13))
        b = t(k + 2)
        mb = re.match(r'(beqz|bnez) \$2, ', b)
        if mb:
            delay2 = t(k + 3)
            nxt = k + 3
            if delay2 == 'nop' or True:
                return (c, mb.group(1) == 'bnez', tg(b), k + 4 if delay2 == 'nop' else k + 3)
    k = j
    x = t(k)
    # b) lbu $3, off($16); beqz/bnez $3
    mm = re.match(r'lbu (\$\d), (0x[0-9A-Fa-f]+)\(\$16\)', x)
    if mm and re.match(r'(beqz|bnez) \%s, ' % mm.group(1), t(k + 1)):
        off = int(mm.group(2), 16)
        nm = EMFIELDS.get(off)
        c = 'em->' + nm[0] if nm and not nm[1] else 'EMF(em, u8, 0x%X)' % off
        b = t(k + 1)
        if off == 2 and False: pass
        return (c, b.startswith('bnez'), tg(b), k + 3 if t(k + 2) == 'nop' else k + 2)
    # c) kind == N
    mm = re.match(r'lbu \$3, 0x2\(\$16\)', x)
    if mm:
        m2 = re.match(r'addiu \$2, \$0, (0x[0-9A-Fa-f]+|\d+)', t(k + 1))
        if m2 and re.match(r'bne \$3, \$2, ', t(k + 2)):
            return ('em->kind == %s' % (int(m2.group(1), 0) if int(m2.group(1), 0) < 10 else hex(int(m2.group(1), 0))), False, tg(t(k + 2)), k + 3)
    # d) game_w.x1E & mask / % N
    if re.match(r'lui \$1, \(0x3F340E >> 16\)', x):
        m2 = re.match(r'lhu (\$\d), \(0x3F340E & 0xFFFF\)\(\$1\)', t(k + 1))
        if m2:
            m3 = re.match(r'andi (\$\d), \$\d, (0x[0-9A-Fa-f]+)', t(k + 2))
            if m3 and re.match(r'bnez ', t(k + 3)):
                return ('(*(u16 *)&game_w.x1E & %s)' % m3.group(2), True, tg(t(k + 3)), k + 4)
    mm = re.match(r'addiu \$3, \$0, (0x[0-9A-Fa-f]+|\d+)', x)
    if mm and re.match(r'lui \$1', t(k + 1)) is None and False: pass
    if re.match(r'lui \$1, \(0x3F340E >> 16\)', x) is None:
        pass
    # d2) modulus: [lui $1] addiu R,$0,N ; lhu ..($1) ; div ; mfhi ; bnez
    kk = k
    if re.match(r'lui \$1, \(0x3F340E >> 16\)', t(kk)): kk += 1
    m2 = re.match(r'addiu (\$\d), \$0, (0x[0-9A-Fa-f]+|\d+)', t(kk))
    if m2 and re.match(r'lhu (\$\d), \(0x3F340E', t(kk + 1)) and t(kk + 2).startswith('div'):
        for q in range(kk + 3, kk + 7):
            if t(q).startswith('mfhi') and re.match(r'bnez ', t(q + 1)):
                return ('(*(u16 *)&game_w.x1E %% %s)' % (int(m2.group(2), 0)), True, tg(t(q + 1)), q + 3 if t(q + 2) == 'nop' else q + 2)
    # e) pos[1] <= K + x5AC (instruction order varies)
    if x.startswith('lui') or x.startswith('lwc1 $f1, 0x5AC'):
        K = None
        found5ac = False
        for q in range(k, k + 8):
            tq = t(q)
            mm = re.match(r'lui (\$\d), \((0x[0-9A-Fa-f]+) >> 16\)', tq)
            if mm and q < k + 6: K = f32(int(mm.group(2), 16))
            if re.match(r'lwc1 \$f1, 0x5AC\(\$16\)', tq): found5ac = True
            if tq.startswith('c.le.s') and t(q + 1).startswith('bc1f') and K is not None and found5ac:
                return ('em->pos[1] <= %s + em->x5AC' % fl(K), False, tg(t(q + 1)), q + 3 if t(q + 2) == 'nop' else q + 2)
    return None

def neg(c):
    if c.startswith('!'): return c[1:] if not c.startswith('!(') else c[2:-1]
    if c.startswith('em->kind =='): return c.replace('==', '!=')
    if c.startswith('(*(u16'): return '!' + c
    return '!(%s)' % c if (' ' in c or '(' in c) else '!%s' % c


def build_expr(tests, body, end):
    n = len(tests)
    starts = [x[4] for x in tests]
    def run(assign):
        i = 0
        while i < n:
            c, J, tgt, nx, st = tests[i]
            v = (assign >> i) & 1
            if v == (1 if J else 0):
                if tgt == body: return True
                if tgt in starts: i = starts.index(tgt); continue
                return False
            i += 1
        return True
    on = [a for a in range(1 << n) if run(a)]
    if len(on) == (1 << n): return 'T'
    if not on: return 'F'
    # Quine-McCluskey: cubes as (value, mask) mask=1 means don't care
    cubes = set((a, 0) for a in on)
    primes = set()
    while cubes:
        nxt = set(); used = set()
        cl = sorted(cubes)
        for x in range(len(cl)):
            for y in range(x + 1, len(cl)):
                (v1, m1), (v2, m2) = cl[x], cl[y]
                if m1 == m2 and bin(v1 ^ v2).count('1') == 1:
                    nxt.add((v1 & ~(v1 ^ v2), m1 | (v1 ^ v2)))
                    used.add(cl[x]); used.add(cl[y])
        for c in cl:
            if c not in used: primes.add(c)
        cubes = nxt
    def covers(p, a): return (a & ~p[1]) == (p[0] & ~p[1])
    # greedy cover, prefer cubes with most don't-cares and low index
    left = set(on); chosen = []
    pl = sorted(primes, key=lambda p: (-bin(p[1]).count('1'), p))
    # essential first
    while left:
        best = max(pl, key=lambda p: (len([a for a in left if covers(p, a)]), -bin(p[1]).count('1') * -1, -min([k for k in range(n) if not (p[1] >> k) & 1] or [0])))
        chosen.append(best)
        left -= set(a for a in left if covers(best, a))
    def term(p):
        lits = []
        for k in range(n):
            if (p[1] >> k) & 1: continue
            c = tests[k][0]
            lits.append(c if (p[0] >> k) & 1 else neg(c))
        return ' && '.join(lits)
    chosen.sort(key=lambda p: min(k for k in range(n) if not (p[1] >> k) & 1))
    terms = [term(p) for p in chosen]
    if len(terms) == 1: return terms[0]
    return ' || '.join('(%s)' % t if '&&' in t else t for t in terms)

def parse_chain(i, stopi):
    tests = []
    k = i
    while k < stopi:
        while k < stopi and ins[k][1] == 'nop' and tests:
            k += 1
        p = parse_test(k, stopi)
        if not p: break
        c, J, tgt, nx = p
        tests.append((c, J, tgt, nx, ins[k][0]))
        k = nx
        # skip pure delay fillers that belong to next test start (lui $1, addiu $4,1)
    if not tests: return None
    # X = first non-test instruction after the whole run
    q = k
    while q < len(ins) and ins[q][1] == 'nop': q += 1
    Xaddr = ins[q][0] if q < len(ins) else 0
    for m in range(1, len(tests) + 1):
        sub = tests[:m]
        nxt_i = sub[-1][3]
        q2 = nxt_i
        while q2 < len(ins) and ins[q2][1] == 'nop': q2 += 1
        bodyaddr = ins[q2][0] if q2 < len(ins) else ins[nxt_i][0]
        bodies = {ins[nxt_i][0], bodyaddr}
        starts = [x[4] for x in sub]
        ends = set()
        for (c, J, tgt, nx, st) in sub:
            if tgt in bodies or tgt in starts: continue
            ends.add(tgt)
        if len(ends) == 1:
            end = list(ends)[0]
            if end <= Xaddr: continue
            sub2 = [(c, J, bodyaddr if tgt in bodies else tgt, nx, st) for (c, J, tgt, nx, st) in sub]
            return sub2, bodyaddr, end, nxt_i
    return None

def run_case(start, stop):
    out = []
    ctx = Ctx()
    r = ctx.r
    f = ctx.f
    stack = []   # end labels of open ifs
    i = byaddr[start]
    stopi = byaddr.get(stop, len(ins))
    ind = 2
    def emit(s):
        out.append('    ' * (ind + len(stack)) + s)
    def regval(x):
        v = r.get(x)
        return v
    def argstr(v):
        if v is None: return '0 /*?*/'
        if isinstance(v, int): return hex(v) if v > 9 else str(v)
        return v
    def step(t):
        m = re.match(r'(\w+)\s*(.*)', t)
        op = m.group(1); a = [x.strip() for x in m.group(2).split(',')] if m.group(2) else []
        if op in ('nop',): return
        if op == 'addiu' and a[1] == '$0': r[a[0]] = int(a[2], 0); return
        if op == 'addiu' and a[1] == '$16': r[a[0]] = 'em+%d' % int(a[2], 0); return
        if op == 'addiu' and a[1] == '$29': r[a[0]] = 'sp+%d' % int(a[2], 0); return
        if op == 'addiu' and a[1] == a[0]:
            v = r.get(a[0])
            if isinstance(v, int): r[a[0]] = v + int(a[2], 0); return
            if v: r[a[0]] = '(%s + %d)' % (v, int(a[2], 0)); return
        if op == 'ori' and a[1] == '$0': r[a[0]] = int(a[2], 0); return
        if op == 'ori' and a[1] == a[0] and '&' in a[2]: return
        if op == 'daddu':
            if a[1] == '$0' and a[2] == '$0': r[a[0]] = 0; return
            if a[2] == '$0':
                if a[1] == '$16': r[a[0]] = 'em'; return
                r[a[0]] = r.get(a[1]); return
        if op == 'lui':
            mm = re.match(r'\((0x[0-9A-Fa-f]+) >> 16\)', a[1])
            if mm: r[a[0]] = ('hi', int(mm.group(1), 16)); return
        if op == 'mtc1':
            v = r.get(a[0])
            if a[0] == '$0': f[a[1]] = 0.0; return
            if isinstance(v, tuple): f[a[1]] = f32(v[1]); return
            f[a[1]] = None; return
        if op == 'andi' and a[1] != '$0':
            v = r.get(a[1]); k = int(a[2], 0)
            if isinstance(v, str) and v.startswith('(u16)ran_suu'):
                r[a[0]] = '(' + v + ' & ' + hex(k) + ')' if k != 0xFFFF else v; return
            r[a[0]] = None; return
        if op == 'dsll32': r[a[0]] = ('shl', r.get(a[1])); return
        if op == 'dsra32':
            v = r.get(a[1])
            if isinstance(v, tuple) and v[0] == 'shl': r[a[0]] = '(s16)' + str(v[1]) if not str(v[1]).startswith('(s16)') else v[1]; return
        if op == 'div':
            ctx.modn = r.get(a[2]); return
        if op == 'mfhi': return
        if op == 'mov.s':
            f[a[0]] = f.get(a[1]); return
        r[a[0]] = None if a else None
    def delay(k):
        if k + 1 < len(ins): step(ins[k + 1][1])
    while i < stopi:
        addr, t = ins[i]
        # close blocks
        while stack and stack[-1] == addr:
            stack.pop()
            emit('}')
        # (emit after pop uses len(stack))
        pc = parse_chain(i, stopi)
        if pc:
            sub, bodyaddr, end, nxt_i = pc
            # peel leading necessary conditions (jump to END when false) as separate nested ifs
            expr = build_expr(sub, bodyaddr, end)
            if expr in ('T', 'F'): emit('#error const chain')
            else:
                emit('if (%s) {' % expr)
                stack.append(end)
            # execute filler instrs between nxt_i and body for register state
            i = nxt_i
            continue
        m = re.match(r'(\w+)\s*(.*)', t)
        op = m.group(1); args = m.group(2)
        if op == 'jal':
            fn = args.strip()
            dl = ins[i + 1][1] if i + 1 < len(ins) else ''
            mm_d = re.match(r'(sw|swc1) (\$\w+), (0x[0-9A-Fa-f]+)\(\$29\)', dl)
            if mm_d:
                off = int(mm_d.group(3), 16)
                v = r.get(mm_d.group(2))
                val = '0.0f' if mm_d.group(2) == '$0' else (fl(f32(v[1])) if isinstance(v, tuple) else '?')
                arr = 'va' if off >= 0x30 else 'vb'
                emit('%s[%s] = %s;' % (arr, (off - (0x30 if off >= 0x30 else 0x20)) // 4, val))
            delay(i)
            a1, a2, a3, a4 = r.get('$5'), r.get('$6'), r.get('$7'), r.get('$8')
            F12, F13 = f.get('$f12'), f.get('$f13')
            if fn.startswith('sound_call') and os.environ.get('SOUND5'):
                if fn.endswith('mov') or fn.endswith('mov2'):
                    emit('%s(em, %s, %s, %s, %s, %s);' % (fn, argstr(a1), argstr(a2), argstr(a3), argstr(a4), argstr(r.get('$9'))))
                else:
                    emit('%s(em, %s, %s, %s, %s);' % (fn, argstr(a1), argstr(a2), argstr(a3), argstr(a4)))
            elif fn.startswith('sound_call'):
                emit('%s(em, %s, %s, %s);' % (fn, argstr(a1), argstr(a2), argstr(a3)))
            elif fn.startswith('quake_call'):
                emit('%s(em, %s, %s);' % (fn, argstr(a1), argstr(a2)))
            elif fn in ('em_frame_check', 'em_frame_check2', 'em_frame_check3'):
                if fn == 'em_frame_check':
                    c = 'em_frame_check(em, %s, %s)' % (fl(F12), argstr(a1))
                elif fn == 'em_frame_check2':
                    c = 'em_frame_check2(em, %s, %s)' % (argstr(a1), fl(F12))
                else:
                    c = 'em_frame_check3(em, %s, %s, %s)' % (argstr(a1), fl(F12), fl(F13))
                ctx.cond = c
            elif fn == 'ran_suu':
                r['$2'] = '(u16)ran_suu(1)' if r.get('$4') == 1 else '(u16)ran_suu()'
            elif fn == 'shell05_set4':
                emit('shell05_set4(em, %s, %s);' % (argstr(a1), argstr(a2)))
            elif fn.startswith('shell0') and fn.endswith('_set'):
                emit('%s(em, %s);' % (fn, argstr(a1)))
            elif fn == 'Shell22_set3':
                emit('Shell22_set3(em, %s, w->x1A);' % argstr(a1))
                emit('w->x1A++;')
                emit('w->x1A &= 3;')
            elif fn == 'Eft10_set':
                emit('Eft10_set(%s, em, %s, %s);' % (fl(F12), argstr(a1), argstr(a2)))
            elif fn == 'em_uvset':
                emit('em_uvset(em, %s, %s, %s);' % (argstr(a1), argstr(a2), argstr(a3)))
            elif fn == 'Eft20_set':
                emit('Eft20_set(%s, em, %s, %s);' % (fl(F12), argstr(a1), argstr(a2)))
            elif fn == 'Eft13_set_em_scl':
                emit('Eft13_set_em_scl(em, %s, %s, %s);' % (argstr(a1), fl(F12), argstr(a2)))
            elif fn == 'Eft15_set3':
                emit('Eft15_set3(em, %s, %s, %s);' % (argstr(a1), fl(F12), argstr(a2)))
            elif fn == 'em_sleep_eff_set':
                emit('em_sleep_eff_set(em, %s, %s, %s);' % (argstr(a1), 'va' if a2 == 'sp+48' else 'vb', fl(F12)))
            elif fn == 'em_mahi_eff_set':
                emit('em_mahi_eff_set(em, %s);' % argstr(a1))
            elif fn.startswith('ground_land'):
                emit('%s(em);' % fn)
            elif fn.startswith('move_default'):
                emit('%s(em);' % fn)
            else:
                emit('#error unhandled call %s' % fn)
            for k in ('$2', ):
                pass
            # clobber temporaries (but keep a0 etc. conservative)
            for k in (('$5',) if fn == 'em_uvset' else ('$4', '$5', '$6', '$7', '$8', '$9')):   # em_uvset (static leaf) keeps a2/a3
                if k in r: del r[k]
            f.clear()
            i += 2
            continue
        if op in ('beqz', 'bnez') and args.startswith('$2') and ctx.cond:
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            delay(i)
            if op == 'bnez':
                # OR if the body label follows the next frame-check branch, else negated if
                is_or = False
                for j in range(i + 2, min(i + 40, len(ins))):
                    tj = ins[j][1]
                    if tj.startswith('beqz $2') and j + 2 < len(ins) and tgt in [ins[k][0] for k in range(j + 2, min(j + 5, len(ins))) if all(ins[q][1] == 'nop' for q in range(j + 2, k))]:
                        is_or = True; break
                    if tj.startswith('bnez $2') and ('%08X' % tgt) in tj:
                        continue
                    if tj.startswith('b') and not tj.startswith('beqz $2'):
                        break
                if is_or:
                    ctx.orpend.append(ctx.cond); ctx.cond = None
                    ctx.orbody = tgt
                else:
                    c = ' || '.join(ctx.orpend + [ctx.cond]); ctx.orpend = []; ctx.cond = None
                    emit('if (!(%s)) {' % c)
                    stack.append(tgt)
            else:
                c = ' || '.join(ctx.orpend + [ctx.cond]); ctx.orpend = []; ctx.cond = None
                emit('if (%s) {' % c)
                stack.append(tgt)
            i += 2
            continue
        if op == 'beqz' and args.startswith('$3'):
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            delay(i)
            # previous lbu $3, off($16)
            prev = ins[i - 1][1]
            mm = re.match(r'lbu\s+\$3, (0x[0-9A-Fa-f]+)\(\$16\)', prev)
            if not mm: mm = re.match(r'lbu\s+\$3, (0x[0-9A-Fa-f]+)\(\$16\)', ins[i - 2][1])
            off = int(mm.group(1), 16) if mm else -1
            nm = EMFIELDS.get(off)
            emit('if (%s != 0) {' % ('em->' + nm[0] if nm and not nm[1] else 'EMF(em, u8, 0x%X)' % off))
            stack.append(tgt)
            i += 2
            continue
        if op == 'bne' and args.startswith('$3, $2'):
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            delay(i)
            emit('if (em->kind == %s) {' % argstr(r.get('$2')))
            stack.append(tgt)
            i += 2
            continue
        if op == 'bnez' and args.startswith('$3'):
            # game_w.x1E & mask test
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            delay(i)
            prev = ins[i - 1][1]
            mm = re.match(r'andi\s+\$3, \$3, (0x[0-9A-Fa-f]+)', prev)
            if prev.startswith('mfhi'):
                emit('if (*(u16 *)&game_w.x1E %% %s == 0) {' % argstr(ctx.modn))
            else:
                emit('if (!(*(u16 *)&game_w.x1E & %s)) {' % (mm.group(1) if mm else '?'))
            stack.append(tgt)
            i += 2
            continue
        if op == 'bc1f':
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            emit('if (em->pos[1] <= %s + em->x5AC) {' % fl(ctx.fc if hasattr(ctx, 'fc') else 1000.0))
            stack.append(tgt)
            i += 2
            continue
        if op == 'lwc1' or op == 'lui' and False:
            pass
        if op == 'b':
            tgt = int(re.search(r'\.L([0-9A-F]{8})', args).group(1), 16)
            if stack and stack[-1] < tgt:
                stack.pop()
                emit('} else {')
                stack.append(tgt)
            elif tgt == END:
                while stack and stack[-1] == END:
                    stack.pop(); emit('}')
                if stack:
                    emit('#error open blocks at break %s' % [hex(x) for x in stack])
                emit('break;')
            else:
                emit('#error branch to %X' % tgt)
            i += 2
            continue
        if op == 'lui':
            mm = re.match(r'(\$\d+), \((0x[0-9A-Fa-f]+) >> 16\)', args)
            if mm:
                r[mm.group(1)] = ('hi', int(mm.group(2), 16))
                ctx.lastlui = int(mm.group(2), 16)
                if mm.group(1) == '$3' and i + 1 < len(ins) and ins[i + 1][1].startswith('mtc1'):
                    pass
            i += 1
            continue
        if op == 'mtc1':
            step(t)
            ctx.fc = f.get(a_ := re.match(r'\$\d+, (\$f\d+)', args).group(1)) if False else ctx.fc if hasattr(ctx, 'fc') else None
            mm = re.match(r'(\$\d+), (\$f\d+)', args)
            if mm.group(2) == '$f0' and isinstance(r.get(mm.group(1)), tuple):
                ctx.fc = f32(r[mm.group(1)][1])
            i += 1
            continue
        if op == 'sw' or op == 'swc1':
            # local vector store for em_sleep_eff_set
            mm = re.match(r'(\$\d+), (0x[0-9A-Fa-f]+)\(\$29\)', args)
            if mm:
                off = int(mm.group(2), 16)
                v = r.get(mm.group(1))
                if mm.group(1) == '$0': val = '0.0f'
                elif isinstance(v, tuple): val = fl(f32(v[1]))
                else: val = '?'
                arr = 'va' if off >= 0x30 else 'vb'
                idx = (off - (0x30 if off >= 0x30 else 0x20)) // 4
                emit('%s[%s] = %s;' % (arr, idx, val))
            i += 1
            continue
        if re.match(r'(beq|bne|beqz|bnez|bgez|bltz|blez|bgtz|bc1t|bc1f)\b', t):
            emit('#error unhandled branch: %s' % t)
        step(t)
        i += 1
    if stack and not all(x == END or x == stop for x in stack):
        out.append('#error unclosed blocks %s' % [hex(x) for x in stack])
    while stack and stack[-1] == stop or (stack and stack[-1] == END):
        stack.pop(); emit('}')
    return out

# case map
cases = {}
for idx, tgt in enumerate(tbl_words):
    cases.setdefault(tgt, []).append(0x3E9 + idx)
if TABLE == '-':
    # sparse switch compiled as a compare ladder: addiu $4,$0,CASE ... beq $3,$4,.Lxxxx
    _cur = None
    for _l in RAW:
        _m = re.search(r'addiu\s+\$4, \$0, (0x[0-9A-Fa-f]+)', _l)
        if _m:
            _cur = int(_m.group(1), 16)
            continue
        _m = re.search(r'beq\s+\$3, \$4, \.L([0-9A-Fa-f]{8})', _l)
        if _m and _cur is not None:
            cases.setdefault(int(_m.group(1), 16), []).append(_cur)
        if re.search(r'\bb\s+\.L%08X' % DEFAULT, _l):
            break
starts = sorted(a for a in cases if a not in (END, DEFAULT))
lines = []
lines.append('static void %s(EMW *em, %s *w) {' % (FUNC, os.environ.get('WTYPE', 'EM01W')))
if 'em_sleep_eff_set' in _asm_func:
    lines.append('    f32 va[4];')
    lines.append('    f32 vb[4];')
lines.append('')
lines.append('    if (em->char0 != w->anim) {')
lines.append('        w->anim = em->char0;')
lines.append('    }')
lines.append('    switch (w->anim) {')
_empt = sorted(cases.get(END, []))
if _empt:
    for _c in _empt:
        lines.append('    case 0x%X:' % _c)
    lines.append('        break;')
# empty cases (table -> END)
empties = sorted(cases.get(END, []))
allc = sorted([(c[0], a) for a, c in cases.items() if a not in (END, DEFAULT)] , key=lambda x: x[1])
for k, a in enumerate(starts):
    stop = starts[k + 1] if k + 1 < len(starts) else DEFAULT
    # ends at first of: next case start or DEFAULT
    for c in sorted(cases[a]):
        lines.append('    case 0x%X:' % c)
    body = run_case(a, stop)
    lines.extend(body)
lines.append('    default:')
lines.append('        %s(em);' % [x for x in re.findall(r'jal\s+(move_default\w*)', _asm)][0])
lines.append('        break;')
lines.append('    }')
lines.append('}')
open(OUTC, 'w').write('\n'.join(lines) + '\n')
print('empty cases:', [hex(x) for x in empties])
print(len(lines), 'lines')
