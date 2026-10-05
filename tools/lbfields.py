#!/usr/bin/env python3
"""lbfields.py: rewrite the CNET_SYS struct in include/lbnet.h from config/lbnet_fields.txt.
Each line: OFFSET TYPE NAME [COMMENT]   (OFFSET hex; TYPE is a C type, arrays as TYPE[N] in NAME)."""
import re, os, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SIZE = {'s8': 1, 'u8': 1, 's16': 2, 'u16': 2, 's32': 4, 'u32': 4, 'f32': 4, 'ptr': 4}
rows = []
for l in open(os.path.join(ROOT, 'config/lbnet_fields.txt')):
    l = l.rstrip('\n')
    if not l.strip() or l.startswith('#'):
        continue
    m = re.match(r'(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)\s*(.*)', l)
    off = int(m.group(1), 16)
    ty, name, cm = m.group(2), m.group(3), m.group(4)
    if ty.endswith('*') or ty == 'ptr':
        sz = 4
    elif ty in SIZE:
        sz = SIZE[ty]
    else:
        mm = re.match(r'(\w+)\[(\w+)\]', ty)
        sz = None
    am = re.match(r'(\w+?)\[(0x[0-9A-Fa-f]+|\d+)\]$', name)
    if am:
        n = int(am.group(2), 0)
        esz = SIZE.get(ty, None)
        if esz is None:
            esz = int(sys.argv[1]) if False else None
        sz = n * (esz if esz else 1)
    ms = re.search(r'size=(0x[0-9A-Fa-f]+)', cm)
    if ms:
        sz = int(ms.group(1), 16)
    rows.append((off, ty, name, cm, sz))
rows.sort()
out = ['typedef struct CNET_SYS {']
pos = 0
for off, ty, name, cm, sz in rows:
    if off < pos:
        sys.exit('overlap at 0x%X (%s)' % (off, name))
    if off > pos:
        gap = off - pos
        out.append('    u8 _pad%03X[0x%X];' % (pos, gap))
    if sz is None:
        # struct-array types given as TYPE with explicit size column via comment "size=0x.."
        ms = re.search(r'size=(0x[0-9A-Fa-f]+)', cm)
        sz = int(ms.group(1), 16)
    out.append('    %s %s;%s' % (ty, name, ('  /* 0x%03X %s */' % (off, re.sub(r'size=\S+\s*', '', cm))) if True else ''))
    pos = off + sz
out.append('} CNET_SYS;')
hdr = os.path.join(ROOT, 'include/lbnet.h')
t = open(hdr).read()
t = re.sub(r'typedef struct CNET_SYS \{.*?\} CNET_SYS;', '\n'.join(out), t, flags=re.S)
open(hdr, 'w').write(t)
print('CNET_SYS: %d fields, ends at 0x%X' % (len(rows), pos))
