#!/usr/bin/env python3
"""lbfld.py NAME... : rewrite F(T, &lb_sys, 0xNN) / F(T, &lb_pit, N) / F(T, pNet, N) in build/lbauto*/NAME.c to the typed
member accesses of include/lobby_b.h (lb_sys.xNN, lb_pit.x08, pNet->x0C ...) and switch the include to lobby_b.h.
Writes build/lbauto_e2/NAME.c; prints the compile status."""
import sys, os, re, subprocess
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
PIT = {0: 'x0', 4: 'pos', 8: 'x08', 9: 'x09', 0xA: 'step'}
NET = {0: 'idx', 2: 'depth', 3: 'step', 4: 'x04', 5: 'x05', 6: 'x06', 7: 'sel', 8: 'menu', 9: 'cur', 0xA: 'x0A', 0xC: 'x0C', 0xD: 'x0D', 0xF: 'yesno', 0x10: 'x10', 0x12: 'x12', 0x24: 'x24', 0x26: 'x26', 0x28: 'x28'}
def conv(s):
    def sysf(m):
        off = int(m.group(2), 0)
        return 'lb_sys.x%02X' % off
    s = re.sub(r'F\((\w+), &lb_sys, (0x[0-9A-Fa-f]+|\d+)\)', sysf, s)
    s = re.sub(r'F\((\w+), &lb_pit, (0x[0-9A-Fa-f]+|\d+)\)', lambda m: 'lb_pit.%s' % PIT[int(m.group(2), 0)] if int(m.group(2), 0) in PIT else m.group(0), s)
    s = re.sub(r'F\((\w+), pNet, (0x[0-9A-Fa-f]+|\d+)\)', lambda m: 'pNet->%s' % NET[int(m.group(2), 0)] if int(m.group(2), 0) in NET else m.group(0), s)
    s = re.sub(r'^extern (?:char|int|u8)\s*\**\s*(?:lb_pit|pNet)(?:\[\])?;\n', '', s, flags=re.M)
    s = re.sub(r'^extern char lb_pit\[\];\n', '', s, flags=re.M)
    s = s.replace('#include "lobby_a.h"', '#include "lobby_b.h"').replace('#include "lobby_f.h"', '#include "lobby_b.h"')
    return s
for fn in sys.argv[1:]:
    p = 'build/lbauto_e2/%s.c' % fn
    if not os.path.exists(p): p = 'build/lbauto_err/%s.c' % fn
    if not os.path.exists(p): p = 'build/lbauto/%s.c' % fn
    s = conv(open(p).read())
    open('build/lbauto_e2/%s.c' % fn, 'w').write(s)
    print(fn, 'written')
