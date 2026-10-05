#!/usr/bin/env python3
"""lbfix3.py NAME... : tag-handler specific repairs of build/lbauto/NAME.c (bsw as u8 *, spNN buffers as char[0x100],
get_input_type stores as s8); keeps the result only when the diff count drops (or it becomes OK)."""
import re, subprocess, sys, os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def score(n, src):
    q = 'src/lobby/zz_f3_%s.c' % n
    open(q, 'w').write(src)
    try:
        out = subprocess.run(['python3', 'tools/check.py', q, '--module', 'lobby'], capture_output=True, text=True).stdout
    finally:
        os.remove(q)
    m = re.search(r'^(OK|--)\s+%s\s+\S+\s+0x[0-9A-F]+\s+\d+ bytes(?:\s+\((\d+)/)?' % re.escape(n), out, re.M)
    if not m: return None
    return 0 if m.group(1) == 'OK' else int(m.group(2))
def fix(s):
    s = re.sub(r'extern (?:int|s32) bsw;', 'extern u8 *bsw;', s)
    m = re.search(r'\n    (?:u8|int|s32) (sp\w+);\n', s)
    if m and re.search(r'get_tag_in_parameter\w*\(\w+, &%s' % m.group(1), s):
        nm = m.group(1)
        s = s.replace(m.group(0), '\n    u8 %s[0x108];\n' % nm)
        s = s.replace('&' + nm, nm)
        s = re.sub(r'\b%s (==|!=)' % nm, nm + r'[0] \1', s)
    s = re.sub(r'F\(s32, bsw, (0x[0-9A-F]+)\) = get_input_type', r'F(s8, bsw, \1) = get_input_type', s)
    s = re.sub(r'(temp_\w+) = bsw;', r'\1 = bsw;', s)
    s = re.sub(r'\bint (temp_\w+);', lambda m: 'int %s;' % m.group(1), s)
    return s
for n in sys.argv[1:]:
    p = 'build/lbauto/%s.c' % n
    s = open(p).read(); t = fix(s)
    # locals assigned from bsw must be pointers
    for nm in set(re.findall(r'\b(\w+) = bsw;', t)):
        t = re.sub(r'\bint %s;' % nm, 'u8 *%s;' % nm, t)
    a = score(n, s); b = score(n, t)
    print(n, a, '->', b)
    if b is not None and (a is None or b < a):
        open(p, 'w').write(t)
