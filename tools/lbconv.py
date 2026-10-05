#!/usr/bin/env python3
"""lbconv.py MODULE FUNC... | --file F: m2c draft converted to something closer to compilable C
(K&R externs, int types, M2C_FIELD -> casts, lobby structs by field name). Hand-edit after."""
import re, subprocess, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
mod = sys.argv[1]
args = sys.argv[2:]
t = subprocess.run([sys.executable, os.path.join(ROOT, "tools/draft.py"), mod] + args, capture_output=True, text=True).stdout

SHOP={0x14:'step',0x15:'x15',0x16:'x16',0x17:'x17',0x18:'x18',0x19:'mode',0x1A:'x1A',0x1B:'x1B',0x1C:'x1C',
0x20:'f20',0x24:'f24',0x28:'f28',0x2C:'f2C',0x30:'f30',0x34:'f34',0x38:'f38',0x3C:'f3C',0x40:'f40',0x44:'f44',0x48:'tag',0x4C:'help',0x50:'list',0x64:'tbl',0x6C:'x6C',0x6D:'x6D',0x6E:'x6E',0x70:'x70',0x74:'cur',0x78:'x78',0x7C:'qty',0x80:'count',0x84:'x84',0x8C:'key',0x8E:'x8E',0x8F:'x8F'}
SYS={0x7:'step',0x68:'x68',0x6C:'x6C',0x87:'x87'}
PIT={0:'x0',4:'pos',8:'x08',9:'x09',0xA:'step'}
NAMED={'lbShop':SHOP,'lb_sys':SYS,'lb_pit':PIT}
for n, tab in NAMED.items():
    t = re.sub(r'M2C_FIELD\(&%s, [^,]*, (0x[0-9A-Fa-f]+)\)' % n,
               lambda m, n=n, tab=tab: ('%s.%s' % (n, tab[int(m.group(1), 16)])) if int(m.group(1), 16) in tab else m.group(0), t)
# CnetSys_w
t = re.sub(r'\*\(&CnetSys_w \+ 0x2E \+ \(([\w ]+) \* 0x1C\)\)', r'CnetSys_w.bg[\1].cmd', t)
t = re.sub(r'M2C_FIELD\(&(\w+), ([^,]*)\*, (0x[0-9A-Fa-f]+)\)', lambda m: 'CNW(%s, %s)' % (m.group(2).strip(), m.group(3)) if m.group(1) == 'CnetSys_w' else '*(%s*)((u8 *)&%s + %s)' % (m.group(2), m.group(1), m.group(3)), t)
# generic M2C_FIELD(x, T *, off) -> *(T *)((u8 *)x + off)
t = re.sub(r'M2C_FIELD\(([^,()]+(?:\([^()]*\))?[^,()]*), ([^,]*)\*, (0x[0-9A-Fa-f]+|[0-9]+)\)', lambda m: '*(%s*)((u8 *)%s + %s)' % (m.group(2), m.group(1), m.group(3)), t)
t = t.replace('M2C_UNK', 'int').replace('s64', 'long long')
# prototypes with /* extern */: K&R
t = re.sub(r'^(\S[^\n(]*?\b(\w+))\((.*)\);\s*/\* extern \*/', lambda m: re.sub(r'^s32|^s16|^s8|^u\d+|^long long|^int', 'int', m.group(1).split()[0]) + ' ' + m.group(2) + '();' if False else m.group(1) + '();', t, flags=re.M)
t = re.sub(r'^extern int (\w+);', r'extern u8 \1[];', t, flags=re.M)
sys.stdout.write(t)
