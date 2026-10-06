#!/usr/bin/env python3
"""b_covered.py: list lobby functions in agent B's range with no linked run (raw holdouts included as covered=no)."""
import re
syms=[]
for l in open('config/symbols/lobby.txt'):
    m=re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+);.*type:func size:0x([0-9A-Fa-f]+)',l)
    if m: syms.append((int(m.group(2),16),int(m.group(3),16),m.group(1)))
runs=[]
for l in open('config/c_files.txt'):
    p=l.split()
    if len(p)>=4 and p[0]=='lobby': runs.append((int(p[1],16),int(p[2],16)))
for a,s,n in sorted(syms,key=lambda x:-x[1]):
    if 0x533980<=a<0x5C4E60 and not(0x53E848<=a<0x590D40) and not any(r0<=a<r1 for r0,r1 in runs): print(s,n,hex(a))
