#!/usr/bin/env python3
"""dalias.py FILE... : D_<addr> names used by matched main C whose address has a real name in
config/symbols/game.txt (or main.txt); prints NAME=real alias pairs for ALIASES in tools/build_pc.sh"""
import re,sys
names={}
for f in ('config/symbols/main.txt','config/symbols/game.txt'):
    for l in open(f):
        m=re.match(r'(\w+) = 0x([0-9A-Fa-f]+);(?:.*size:0x([0-9A-Fa-f]+))?',l)
        if m: names.setdefault(int(m.group(2),16),[]).append((m.group(1),int(m.group(3),16) if m.group(3) else 0))
sh=open('tools/build_pc.sh').read()
seen=set()
for f in sys.argv[1:]:
    for m in re.finditer(r'\b(?:D|func)_([0-9A-F]{6})\b',open(f).read()):
        a=int(m.group(1),16); n=m.group(0)
        if n in seen or n+'=' in sh: continue
        seen.add(n)
        if a in names:
            print('%s=%s'%(n,names[a][0][0]),end=' ')
        else:
            # inside a sized symbol?
            for b,l in names.items():
                for nm,sz in l:
                    if b<a<b+sz: print('%s=%s+0x%X'%(n,nm,a-b),end=' ')
print()
