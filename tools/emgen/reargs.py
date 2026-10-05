#!/usr/bin/env python3
"""reargs.py NM.c ASM.s NAME NARGS : rewrite calls NAME(em, ...) with NARGS args using the asm (a1.. from registers)"""
import re,sys
p,asmf,name,n=sys.argv[1],sys.argv[2],sys.argv[3],int(sys.argv[4])
s=open(p).read()
funcs={}
for m in re.finditer(r'^glabel (\S+)\n(.*?)^endlabel \1\n',open(asmf).read(),re.M|re.S):
    ins=[]
    for l in m.group(2).split('\n'):
        mm=re.match(r'\s*/\* \S+ \S+ \S+ \*/\s+(\S+)\s*(.*)$',l)
        if mm: ins.append((mm.group(1),mm.group(2).strip()))
    funcs[m.group(1)]=ins
def resolve_int(ins,jidx,reg):
    for k in [jidx+1]+list(range(jidx-1,max(jidx-40,-1),-1)):
        if k>=len(ins): continue
        op,args=ins[k]
        if op in('jal','jr') and k<jidx-1: break
        a=[x.strip() for x in args.split(',')]
        if a and a[0]=='$%d'%reg and op not in ('sw','sb','sh','sd','swc1','sq'):
            if op=='addiu' and a[1]=='$0': return str(int(a[2],0))
            if op in('daddu','addu','or') and a[1]=='$0' and a[2]=='$0': return '0'
            if op=='ori' and a[1]=='$0': return a[2]
            return 'REG%d_%s_%s'%(reg,op,'_'.join(a[1:]).replace('$','r').replace(' ','').replace('(','').replace(')','').replace('-','m'))
    return 'REG%d'%reg
parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s)
out=[];cnt=0
for f in parts:
    m=re.match(r'\S[^\n]*?\b(\w+)\([^\n]*\)\s*\{$',f,re.M)
    if not m or m.group(1) not in funcs or (name+'(') not in f: out.append(f); continue
    ins=funcs[m.group(1)]
    jals=[i for i,(op,a) in enumerate(ins) if op=='jal' and a.split()[0]==name]
    # tail call 'j name'
    jals+= [i for i,(op,a) in enumerate(ins) if op=='j' and a.split()[0]==name]
    jals.sort()
    k=[0]
    def rep(mm):
        i=k[0]; k[0]+=1
        if i>=len(jals): return mm.group(0)
        vals=['em']+[resolve_int(ins,jals[i],4+j) for j in range(1,n)]
        return name+'('+', '.join(vals)+')'
    f2=re.sub(r'\b%s\((?:em|[^;()]*?)\)(?=;|\s*[!=<&|)]|\))'%re.escape(name),rep,f)
    out.append(f2)
open(p,'w').write(''.join(out))
