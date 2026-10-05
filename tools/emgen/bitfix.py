#!/usr/bin/env python3
"""bitfix.py NM.c ASM.s... : replace M2C_BITWISE(f32, tempX) float call args using the asm (mtc1 constants)"""
import re,struct,sys
p=sys.argv[1]; asms=sys.argv[2:]
s=open(p).read()
funcs={}
for a in asms:
    for m in re.finditer(r'^glabel (\S+)\n(.*?)^endlabel \1\n',open(a).read(),re.M|re.S):
        ins=[]
        for l in m.group(2).split('\n'):
            mm=re.match(r'\s*/\* \S+ \S+ \S+ \*/\s+(\S+)\s*(.*)$',l)
            if mm: ins.append((mm.group(1),mm.group(2).strip()))
        funcs[m.group(1)]=ins
def fl(h): return struct.unpack('<f',struct.pack('<I',h))[0]
def fmt(f):
    x='%.9g'%f
    if '.' not in x and 'e' not in x: x+='.0'
    return x+'f'
def resolve(ins,j,freg):
    for k in range(j-1,max(j-40,-1),-1):
        op,args=ins[k]
        if op in('jal','jr') and k<j-1: break
        a=[x.strip() for x in args.split(',')]
        if op=='mtc1' and a[1]=='$f%d'%freg:
            if a[0]=='$0': return '0.0f'
            for q in range(k-1,max(k-12,-1),-1):
                o2,a2=ins[q]
                m=re.match(r'(\$\d+), \(0x([0-9A-F]+) >> 16\)',a2)
                if o2=='lui' and m and m.group(1)==a[0]: return fmt(fl(int(m.group(2),16)))
            return None
        if a and a[0]=='$f%d'%freg and op not in ('swc1','c.lt.s','c.le.s','c.eq.s'): return None
    return None
parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s)
out=[];n=0;miss=0
for f in parts:
    m=re.match(r'\S[^\n]*?\b(\w+)\([^\n]*\)\s*\{$',f,re.M)
    if not m or m.group(1) not in funcs or 'M2C_BITWISE(f32' not in f: out.append(f); continue
    ins=funcs[m.group(1)]
    jal={}
    for i,(op,a) in enumerate(ins):
        if op=='jal': jal.setdefault(a.split()[0],[]).append(i)
    cnt={}
    def rep(mm):
        global n,miss
        name=mm.group(1)
        k=cnt.get(name,0); cnt[name]=k+1
        if name not in jal or k>=len(jal[name]): miss+=1; return mm.group(0)
        v=resolve(ins,jal[name][k],12)
        if v is None: miss+=1; return mm.group(0)
        n+=1
        return mm.group(0).replace(mm.group(2),v)
    f=re.sub(r'\b(\w+)\([^;()]*?(M2C_BITWISE\(f32, \w+\))[^;()]*?\)',rep,f)
    out.append(f)
open(p,'w').write(''.join(out)); print('fixed',n,'missed',miss)
