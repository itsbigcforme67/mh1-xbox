#!/usr/bin/env python3
"""fz.py FILE FUNC [rounds]: greedy single-site source mutation search for one near-match function (agent E).
Mutations (applied one at a time, kept when the check.py difference count of FUNC drops):
  narrow local declarations (u8/s8/u16/s16) -> int and int -> u16/s16 ; compare flips (>= N <-> > N-1, <= N <-> < N+1, < N <-> <= N-1, > N <-> >= N+1);
  swap the operands of a simple `a op b` (op in + | & == !=) ; `x += y` <-> `x = x + y`; enlarge the first array/struct local by 8 bytes.
Compiles a temporary copy of FILE (zzfz<pid>.c next to it), so FILE is only rewritten when something improved. Prints each kept change."""
import re, subprocess, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f, fn = sys.argv[1:3]
rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 3
src = open(f).read()
m = re.search(r'^[A-Za-z_][^;{}\n]*\b%s\([^;{]*\)[^;{]*\{\n' % re.escape(fn), src, re.M)
if not m: sys.exit('function not found')
a = m.start(); b = src.index('\n}\n', a) + 3
z = os.path.join(os.path.dirname(f), 'zzfz%d.c' % os.getpid())
def score(body):
    open(z, 'w').write(src[:a] + body + src[b:])
    try:
        o = subprocess.run(['python3', 'tools/check.py', z], capture_output=True, text=True, timeout=300).stdout
    except Exception:
        return 999
    for l in o.splitlines():
        if re.match(r'(OK|--)\s+%s\s' % re.escape(fn), l):
            if l.startswith('OK'): return 0
            mm = re.search(r'\((\d+)/', l)
            return int(mm.group(1)) if mm else 999
    return 999
def mutations(t):
    out = []
    for mm in re.finditer(r'^(    )(u8|s8|u16|s16|int|u32|s32) (\w+)(;| = [^;]+;)$', t, re.M):
        ty = mm.group(2)
        for nt in (['int'] if ty != 'int' else ['u16', 's16', 'u8', 's8', 'u32']):
            out.append((mm.start(), mm.end(), mm.group(1) + nt + ' ' + mm.group(3) + mm.group(4)))
    for mm in re.finditer(r'(?<=[\w\)\]]) (>=|<=|<|>) (0x[0-9A-Fa-f]+|\d+)(?=[\) ;&|])', t):
        op, n = mm.group(1), mm.group(2)
        v = int(n, 0); hexf = n.startswith('0x')
        def fmt(x): return ('0x%X' % x) if hexf else str(x)
        rep = {'>=': ('>', v - 1), '<=': ('<', v + 1), '<': ('<=', v - 1), '>': ('>=', v + 1)}[op]
        if rep[1] >= 0: out.append((mm.start(), mm.end(), ' %s %s' % (rep[0], fmt(rep[1]))))
    for mm in re.finditer(r'\b(\w+(?:->\w+|\.\w+|\[\w+\])*) (\+|\||&|==|!=) (\w+(?:->\w+|\.\w+|\[\w+\])*)\b', t):
        out.append((mm.start(), mm.end(), '%s %s %s' % (mm.group(3), mm.group(2), mm.group(1))))
    for mm in re.finditer(r'\b(\w+) \+= ([^;]+);', t):
        out.append((mm.start(), mm.end(), '%s = %s + %s;' % (mm.group(1), mm.group(1), mm.group(2))))
    for mm in re.finditer(r'\b(\w+) = \1 \+ ([^;]+);', t):
        out.append((mm.start(), mm.end(), '%s += %s;' % (mm.group(1), mm.group(2))))
    for mm in re.finditer(r'^(    )(u8|s8|s32|u32|s16|u16|int) (\w+)\[(0x[0-9A-Fa-f]+|\d+)\];$', t, re.M):
        n = int(mm.group(4), 0)
        out.append((mm.start(), mm.end(), '%s%s %s[0x%X];' % (mm.group(1), mm.group(2), mm.group(3), n + 8)))
    return out
cur = src[a:b]
best = score(cur); print('start', best, flush=True)
changed = False
try:
    for r in range(rounds):
        improved = False
        i = 0
        muts = mutations(cur)
        # apply from the end so earlier offsets stay valid within one pass of candidates
        for (s, e, new) in muts:
            cand = cur[:s] + new + cur[e:]
            if cand == cur: continue
            sc = score(cand)
            if sc < best:
                print('kept', repr(cur[s:e][:50]), '->', repr(new[:50]), sc, flush=True)
                best = sc; cur = cand; improved = changed = True
                break
        if not improved or best == 0: break
finally:
    if os.path.exists(z): os.remove(z)
if changed:
    open(f, 'w').write(src[:a] + cur + src[b:])
    print('written', f, 'score', best)
else:
    print('no improvement')
