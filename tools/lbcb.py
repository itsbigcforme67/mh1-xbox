#!/usr/bin/env python3
"""lbcb.py NAME... : turn an auto draft of a CallBack_Result_* style function (by-value CNET_RES argument spilled to the
stack as `long long sp18; sp18 = arg0; (s8) sp18`) into compilable C: `CNET_RES res` parameter, stale temp_* arguments
dropped from calls, cw accessed through a struct (tools/lbcws.py). Reads build/lbauto_e2 or build/lbauto, writes
build/lbauto/NAME.c when the result compiles."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
def chk(fn, src):
    path = 'src/lobby/zz_cb_%s.c' % fn
    open(path, 'w').write(src)
    try:
        r = subprocess.run(['python3', 'tools/check.py', path, '--module', 'lobby'], capture_output=True, text=True); out = r.stdout + r.stderr
    finally:
        os.remove(path)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(fn), out, re.M)
    return (0 if m.group(1) == 'OK' else int(m.group(2))) if m else None
for fn in sys.argv[1:]:
    p = 'build/lbauto_e2/%s.c' % fn
    if not os.path.exists(p): p = 'build/lbauto/%s.c' % fn
    s = open(p).read()
    s = s.replace('#include "lobby_a.h"', '#include "lobby_b.h"').replace('#include "lobby_f.h"', '#include "lobby_b.h"')
    s = re.sub(r'^extern char (temp_\w+|sp\w+)\[\];\n', '', s, flags=re.M)
    s = re.sub(r'\(int arg0\) \{', '(CNET_RES res) {', s)
    s = re.sub(r'^\s*long long sp18;\n', '', s, flags=re.M)
    s = re.sub(r'^\s*sp18 = arg0;\n', '', s, flags=re.M)
    s = s.replace('(s8) sp18', 'res.val').replace('sp18', 'res.val')
    # stale arguments
    def fixcall(m):
        name, args = m.group(1), m.group(2)
        if name in ('if', 'while', 'switch', 'return', 'F', 'sizeof'): return m.group(0)
        if not re.search(r'\btemp_\w+', args): return m.group(0)
        parts = [a.strip() for a in args.split(',')]
        keep = [a for a in parts if not re.match(r'^temp_\w+( \+ 0x[0-9A-Fa-f]+)?$', a)]
        return '%s(%s)' % (name, ', '.join(keep))
    s = re.sub(r'\b(\w+)\(([^()]*)\)', fixcall, s)
    s = re.sub(r'\(F\(u8, (temp_\w+), (0x[0-9A-F]+)\)\)', r'F(u8, \1, \2)', s)
    s = re.sub(r'\s*\(temp_a0 = F\(u8, temp_a1, 0x2C45\), \(temp_a0 == (\w+)\)\)', r' (F(u8, temp_a1, 0x2C45) == \1)', s)
    s = re.sub(r'int (temp_\w+);', r'u8 *\1;', s)
    s = re.sub(r'(temp_\w+) = \(int\)cw;', r'\1 = (u8 *)cw;', s)
    s = re.sub(r'^\s*u8 temp_a0;\n', '', s, flags=re.M) if 'temp_a0' not in re.sub(r'u8 temp_a0;', '', s) else s
    open('build/lbauto/%s.c' % fn, 'w').write(s)
    d = chk(fn, s)
    if d is None: print(fn, 'err'); continue
    # struct view of cw
    r = subprocess.run(['python3', 'tools/lbcws.py', fn], capture_output=True, text=True).stdout.strip()
    d2 = chk(fn, open('build/lbauto/%s.c' % fn).read())
    print(fn, 'd%s' % d, '->', 'OK' if d2 == 0 else 'd%s' % d2)
