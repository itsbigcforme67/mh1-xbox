#!/usr/bin/env python3
"""f12.py FUNC: per jal, print float consts in f12-f14 and int consts a1..a4 (incl. delay slot)."""
import sys,re,subprocess,struct
fn=sys.argv[1]
txt=subprocess.run(['tools/fa.sh',fn],capture_output=True,text=True).stdout.split('\n')
txt=[l.strip() for l in txt]
def f(h): return struct.unpack('>f',struct.pack('>I',h))[0]
regs={};fl={};ints={}
def step(l):
    m=re.match(r'lui\s+\$(\d+), \((0x[0-9A-F]+) >> 16\)',l)
    if m: regs[m.group(1)]=int(m.group(2),16); ints[m.group(1)]=None; return
    m=re.match(r'mtc1\s+\$(\d+), \$f(\d+)',l)
    if m:
        r=m.group(1); fl['f'+m.group(2)]=f(regs[r]) if r in regs else '?'; return
    m=re.match(r'lwc1\s+\$f(\d+), (.*)',l)
    if m: fl['f'+m.group(1)]='mem '+m.group(2); return
    m=re.match(r'(addiu|ori)\s+\$(\d+), \$0, (0x[0-9A-F]+|-?\d+)',l)
    if m: ints[m.group(2)]=m.group(3); return
    m=re.match(r'daddu\s+\$(\d+), \$0, \$0',l)
    if m: ints[m.group(1)]='0'; return
    m=re.match(r'(\w+)\s+\$(\d+),',l)
    if m and m.group(1) not in('sb','sh','sw','sd','sq','swc1','beq','bne','beqz','bnez','mtc1') :
        ints[m.group(2)]=None
i=0
while i<len(txt):
    l=txt[i]
    m=re.match(r'jal\s+(\S+)',l)
    if m:
        if i+1<len(txt): step(txt[i+1])
        a=' '.join('a%d=%s'%(int(r)-4,ints[r]) for r in ('5','6','7','8','9') if ints.get(r))
        print(m.group(1),{k:v for k,v in fl.items() if k in('f12','f13','f14')},a)
        fl={};ints={}
    else: step(l)
    i+=1
