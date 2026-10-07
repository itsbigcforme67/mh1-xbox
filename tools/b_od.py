#!/usr/bin/env python3
"""b_od.py FUNC: disassemble one function of the main module straight from disc/mh1/split/main.bin (original bytes), with callee names.
Use it to read what the original did when the function is already registered as C or raw (asm/ no longer has it)."""
import sys,re,struct
import os
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0,os.path.join(ROOT,'tools'))
from mips_dis import dis,listing
import os
os.chdir(ROOT)
name=sys.argv[1]
for l in open('config/symbols/main.txt'):
    m=re.match(r'%s\s*=\s*0x([0-9A-Fa-f]+);.*size:0x([0-9A-Fa-f]+)'%re.escape(name),l)
    if m:
        a=int(m.group(1),16);s=int(m.group(2),16);break
d=open('disc/mh1/split/main.bin','rb').read()
# symbol names for calls
syms={}
for l in open('config/symbols/main.txt'):
    m=re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+)',l)
    if m: syms[int(m.group(2),16)]=m.group(1)
for pc in range(a,a+s,4):
    w=struct.unpack_from('<I',d,pc-0x100000)[0]
    t=dis(w,pc)
    mm=re.search(r'0x([0-9a-fA-F]{8})$',t)
    if mm and t.startswith('jal') and int(mm.group(1),16) in syms: t+='  ; '+syms[int(mm.group(1),16)]
    print('%08X  %s'%(pc,t))
