#!/usr/bin/env python3
"""lbconv.py FUNC... : first-pass C from the m2c drafts: M2C_FIELD(p, T *, off) -> F(T, p, off), M2C_UNK -> int,
pointer-typed args -> u8 *; prints definitions only (no extern block). A starting point to edit by hand."""
import sys, os, re, glob
D = os.environ.get('LBDRAFTS', '/tmp/claude-1000/-home-james-claude-projects/6db1702a-235b-4025-a34e-ca6b5540767b/scratchpad/drafts')
txt = ''.join(open(f).read() for f in sorted(glob.glob(D + '/d*.c')))
def conv(s):
    s = re.sub(r'M2C_FIELD\(([^,]+?), (\w+) \*, (0x[0-9A-Fa-f]+|\d+)\)', r'F(\2, \1, \3)', s)
    s = s.replace('M2C_UNK', 'int').replace('void *arg', 'u8 *arg')
    s = re.sub(r'\(s64\) \(\(s64\) (\w+) << 0x30\) >> 0x30', r'(s16)\1', s)
    s = re.sub(r'\(s64\) \((\w+) << 0x38\) >> 0x38', r'(s8)\1', s)
    s = re.sub(r'\(s64\) \(\(s64\) (\w+) << 0x38\) >> 0x38', r'(s8)\1', s)
    s = s.replace('(s64)', '').replace('(s32) ', '')
    s = re.sub(r'\*\(u8 \*\)0x3F34C1', 'game_w.master', s)
    s = re.sub(r'\*\(u8 \*\)0x3F3404', 'game_w.stage', s)
    return s
seen = set()
for fn in sys.argv[1:]:
    for mm in re.finditer(r'^[\w\*\s]+\b%s\([^;{]*\) \{\n' % re.escape(fn), txt, re.M):
        i = mm.start(); j = txt.index('\n}\n', i) + 3
        print(conv(txt[i:j])); break
    else:
        print('/* no draft for %s */' % fn)
