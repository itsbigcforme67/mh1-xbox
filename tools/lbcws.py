#!/usr/bin/env python3
"""lbcws.py NAME... : rewrite build/lbauto/NAME.c so the client work (cw) is accessed through a per-function struct
(`cw->x2C31`) instead of F(T, cw, 0x2C31). The original code uses a struct pointer there; MWCC then allocates
registers like the original (Match_Logout, GuestRoomMember). Keeps the rewrite only if it does not increase the diff."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def run(fn, src):
    path = 'src/lobby/zz_cws_%s.c' % fn
    open(path, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
SZ = {'u8': 1, 's8': 1, 'char': 1, 'u16': 2, 's16': 2, 's32': 4, 'u32': 4, 'int': 4, 'f32': 4}
def convert(src, fn):
    m = re.search(r'^[\w\*\s]+\b%s\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n' % re.escape(fn), src, re.M)
    if not m: return None
    head, body = src[:m.start()], src[m.start():]
    cwv = set(re.findall(r'\b(\w+) = \(u8 \*\)cw;', body)) | set(re.findall(r'\b(\w+) = \(int\)cw;', body))
    cwv |= set(re.findall(r'\b(\w+) = cw;', body))
    used = {}
    def rep(mm):
        T, base, off = mm.group(1), mm.group(2).strip(), int(mm.group(3), 0)
        if base == '(u8 *)cw' or base == 'cw' or base in cwv:
            if T not in SZ: return mm.group(0)
            used[off] = T
            return 'CWX->x%04X' % off
        return mm.group(0)
    body2 = re.sub(r'F\((\w+), ([^,()]+(?: \*\))?[^,()]*), (0x[0-9A-Fa-f]+|\d+)\)', rep, body)
    if not used: return None
    for v in cwv:
        body2 = re.sub(r'^\s*%s = (?:\(u8 \*\)|\(int\))?cw;\n' % v, '', body2, flags=re.M)
        body2 = re.sub(r'^\s*(?:void|u8) \*%s;\n' % v, '', body2, flags=re.M)
        body2 = re.sub(r'^\s*(?:int|s32) %s;\n' % v, '', body2, flags=re.M)
        body2 = re.sub(r'^\s*(?:void|u8) \*%s = (?:\(u8 \*\))?cw;\n' % v, '', body2, flags=re.M)
        body2 = re.sub(r'\b%s\b(?= [+-] )' % v, '(u8 *)cw', body2)
        body2 = re.sub(r'\b%s\b' % v, '((CWS_%s *)cw)' % fn, body2)
    body2 = body2.replace('CWX->', '((CWS_%s *)cw)->' % fn)
    offs = sorted(used)
    fields = []; pos = 0
    for o in offs:
        T = used[o]; s = SZ[T]
        if o < pos: return None
        if o > pos: fields.append('u8 pad%04X[0x%X];' % (pos, o - pos))
        if o % s: return None
        fields.append('%s x%04X;' % (T, o)); pos = o + s
    td = 'typedef struct { %s } CWS_%s;\n' % (' '.join(fields), fn)
    return head + td + body2
def convert_args(src, fn):
    m = re.search(r'^([\w\*\s]+\b%s\()([^)]*)(\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n)' % re.escape(fn), src, re.M)
    if not m: return None
    head, body = src[:m.start()], src[m.end():]
    params = m.group(2)
    out_params = params; used_all = {}
    for pv in re.findall(r'\b(arg\d)\b', params):
        pt = re.search(r'((?:const )?(?:\w+\s+)*\**)\s*\b%s\b' % pv, params).group(1).strip()
        if not re.match(r'^(int|s32|u32|u8 \*|void \*|u8\*|void\*)$', pt): continue
        used = {}
        def rep(mm):
            T, base, off = mm.group(1), mm.group(2).strip(), int(mm.group(3), 0)
            if base == pv and T in SZ:
                used[off] = T
                return '%s->x%04X' % (pv, off)
            return mm.group(0)
        b2 = re.sub(r'F\((\w+), ([^,()]+), (0x[0-9A-Fa-f]+|\d+)\)', rep, body)
        if not used: continue
        b2 = re.sub(r'\b%s\b(?!->)' % pv, '(u8 *)%s' % pv, b2)
        offs = sorted(used); fields = []; pos = 0; bad = False
        for o in offs:
            T = used[o]; sz = SZ[T]
            if o < pos or o % sz: bad = True; break
            if o > pos: fields.append('u8 pad%04X[0x%X];' % (pos, o - pos))
            fields.append('%s x%04X;' % (T, o)); pos = o + sz
        if bad: continue
        tn = 'ARG_%s_%s' % (fn, pv)
        head += 'typedef struct { %s } %s;\n' % (' '.join(fields), tn)
        out_params = re.sub(r'(?:(?:const )?(?:\w+\s+)*\**)\s*\b%s\b' % pv, '%s *%s' % (tn, pv), out_params, count=1)
        body = b2
    if out_params == params: return None
    return head + m.group(1) + out_params + m.group(3) + body
for fn in sys.argv[1:]:
    p = os.environ.get('NMFILE') or 'build/lbauto/%s.c' % fn
    src = open(p).read()
    base = run(fn, src)
    new = convert(src, fn)
    if new is None or os.environ.get('ARGS'): new = convert_args(new or src, fn)
    if new is None or base is None: print(fn, 'skip'); continue
    r = run(fn, new)
    if r is None and os.environ.get('DBG'): open('/tmp/x_cws.c','w').write(new)
    if r is not None and r <= base and new != src:
        open(p, 'w').write(new)
        print(fn, 'OK' if r == 0 else 'd%d (was %d)' % (r, base), flush=True)
    else:
        print(fn, 'no gain', r, base, flush=True)
