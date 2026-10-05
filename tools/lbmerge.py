#!/usr/bin/env python3
"""lbmerge.py NM.c NEW.c [PROTO.h]: insert the function definitions of NEW.c into NM.c (replacing same-named
ones), keep them sorted by lobby symbol address, and refresh the unprototyped forward declarations in the
proto header (include/lbnet_proto.h): extern/data lines are kept, new ones can be put in NEW.c as lines
starting with 'extern '; functions never need a manual declaration."""
import re, sys, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
nm, new = sys.argv[1:3]
proto = sys.argv[3] if len(sys.argv) > 3 else 'include/lbnet_proto.h'
sym = {}
for l in open('config/symbols/lobby.txt'):
    m = re.match(r'(\w+) = 0x([0-9A-F]+); // type:func', l)
    if m:
        sym[m.group(1)] = int(m.group(2), 16)
HEAD = re.compile(r'^([A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)(?:\n[^;{\n]*;)*\s*\{)\n', re.M)
def defs(t):
    out = {}
    for m in HEAD.finditer(t):
        e = t.index('\n}\n', m.end()) + 3
        out[m.group(2)] = t[m.start():e]
    return out
t = open(nm).read()
cur = defs(t)
n = open(new).read()
upd = defs(n)
cur.update(upd)
hdr = t[:HEAD.search(t).start()]
miss = [k for k in cur if k not in sym]
if miss:
    print('no symbol address for', miss, file=sys.stderr)
order = sorted(cur, key=lambda k: sym.get(k, 1 << 40))
open(nm, 'w').write(hdr + '\n'.join(cur[k] for k in order))
# proto header
p = open(proto).read()
ext = [l for l in p.split('\n') if l.startswith('extern ')]
for l in n.split('\n'):
    if l.startswith('extern ') and l not in ext:
        ext.append(l)
def sig(k):
    h = cur[k].split('\n')[0]
    m = re.match(r'^([\w \*]+?)\s*(\*?)\b%s\(' % re.escape(k), h)
    rt = m.group(1).strip()
    return '%s %s%s();' % (rt, m.group(2), k)
fwd = [sig(k) for k in order if not cur[k].startswith('static ')]
# keep forward declarations of functions defined elsewhere (other files / not yet written)
old = [l for l in p.split('\n') if re.match(r'^[\w \*]+\(\);$', l) and not l.startswith('extern ')]
oldnames = {re.match(r'^[\w \*]*?(\w+)\(\);$', l).group(1): l for l in old}
for k in cur:
    oldnames.pop(k, None)
fwd_all = fwd + sorted(oldnames.values())
body = '/* lbnet_proto.h - extern data and unprototyped declarations for the lobby network layer (cnlbs). */\n#ifndef LBNET_PROTO_H\n#define LBNET_PROTO_H\n#include "lbnet.h"\n\n' + '\n'.join(ext) + '\n\n' + '\n'.join(fwd_all) + '\n\n#endif\n'
open(proto, 'w').write(body)
print(len(cur), 'functions in', nm)
