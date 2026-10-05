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

def conv(s):
    s = re.sub(r'M2C_FIELD\(saved_reg_gp, (\w+) \*\*?, (-0x[0-9A-Fa-f]+)\)', lambda m: ('(%s)cw' % ('u8 *' if m.group(1) in ('void', 'u8', 's8') else m.group(1)) if m.group(2) in GP else 'GPBAD'), s)
    s = re.sub(r'M2C_FIELD\(([^,]+?), ([\w ]+?) \*, (0x[0-9A-Fa-f]+|\d+)\)', r'F(\2, \1, \3)', s)
    s = re.sub(r'M2C_FIELD\(([^,]+?), ([\w ]+?) \*\*, (0x[0-9A-Fa-f]+|\d+)\)', r'F(\2 *, \1, \3)', s)
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

def attempt(fn):
    d = draft(fn)
    if d is None: return fn, {'status': 'nodraft'}
    hdr, body = d
    if 'M2C_ERROR' in body or 'GPBAD' in conv(body) or 'M2C_BITWISE' in body or 'saved_reg' in body.replace('saved_reg_gp, ', 'X'):
        pass
    body = conv(body)
    if 'M2C_ERROR' in body or 'GPBAD' in body or 'M2C_' in body:
        return fn, {'status': 'unsupported'}
    decl = set()
    # externs from the draft header: functions
    lines = []
    for l in hdr.split('\n'):
        l = l.strip()
        if not l or l.startswith('/*') or l.startswith('*'): continue
        l = conv(l)
        l = re.sub(r'\s*/\*.*?\*/', '', l)
        lines.append(l)
    path = os.path.join(ROOT, 'src/lobby/zz_auto_%s.c' % re.sub(r'\W', '_', fn))
    extra = []
    res = {'status': 'error'}
    try:
        for it in range(6):
            src = '#include "lobby.h"\n' + '\n'.join(extra) + '\n' + body
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
        for fn, r in ex.map(attempt, names):
            results[fn] = r
            print(fn, r.get('status'), r.get('d', ''), r.get('n', ''), flush=True)
            json.dump(results, open(outf, 'w'))
