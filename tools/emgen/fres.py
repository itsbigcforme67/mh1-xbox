#!/usr/bin/env python3
"""fres.py ASMFILE DRAFT.c OUT.c : resolve M2C_ERROR float args in m2c draft by reading asm."""
import re,struct,sys
asmf,draft,out=sys.argv[1:4]
ctx=open('/tmp/claude-1000/w/ctx.c').read()
def proto(name):
    m=re.search(r'^[\w \*]+?\b%s\s*\(([^;{]*)\);'%name,ctx,re.M)
    if not m: return None
    ps=[p.strip() for p in m.group(1).split(',')]
    return ps
asm=open(asmf).read()
funcs={}
for m in re.finditer(r'^glabel (\S+)\n(.*?)^endlabel \1\n',asm,re.M|re.S):
    ins=[]
    for l in m.group(2).split('\n'):
        mm=re.match(r'\s*/\* \S+ \S+ \S+ \*/\s+(\S+)\s*(.*)$',l)
        if mm: ins.append((mm.group(1),mm.group(2).strip()))
    funcs[m.group(1)]=ins
def fl(h): return struct.unpack('<f',struct.pack('<I',h))[0]
def fmt(f):
    s=repr(float(f)) 
    s='%.9g'%f
    if '.' not in s and 'e' not in s and 'n' not in s: s+='.0'
    return s+'f'
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
            return 'REG%d_%s'%(reg,op)
    return 'REG%d'%reg
def resolve(ins,jidx,freg):
    # scan back
    regs={}
    for k in range(jidx-1,max(jidx-40,-1),-1):
        op,args=ins[k]
        if op in('jal','jr') : 
            if k<jidx-1: break
            continue
        a=[x.strip() for x in args.split(',')]
        if op=='mtc1' and a[1]=='$f%d'%freg:
            r=a[0]
            if r=='$0': return '0.0f'
            for j in range(k-1,max(k-12,-1),-1):
                o2,a2=ins[j]
                m=re.match(r'(\$\d+), \(0x([0-9A-F]+) >> 16\)',a2)
                if o2=='lui' and m and m.group(1)==r:
                    return fmt(fl(int(m.group(2),16)))
            return 'MTC1(%s)'%r
        if op=='lwc1' and a[0]=='$f%d'%freg:
            return 'LWC1(%s)'%a[1]
        if op=='mov.s' and a[0]=='$f%d'%freg:
            return 'MOVS(%s)'%a[1]
        if a and a[0]=='$f%d'%freg and op not in ('swc1','c.lt.s','c.le.s','c.eq.s'):
            return 'F%d_%s(%s)'%(freg,op,','.join(a[1:]))
    return 'F%d_?'%freg
text=open(draft).read()
# split into functions
parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',text)
res=[]
unres=0
for p in parts:
    m=re.match(r'\S[^\n]*?\b(\w+)\([^\n]*\)\s*\{$',p,re.M)
    if not m or m.group(1) not in funcs or ('M2C_ERROR(/* Read from unset register $' not in p and 'Unable to find stack arg' not in p):
        res.append(p); continue
    ins=funcs[m.group(1)]
    jals={}
    for i,(op,a) in enumerate(ins):
        if op=='jal': jals.setdefault(a.split()[0],[]).append(i)
    cnt={}
    def fixcall(mc):
        global unres
        name=mc.group(1)
        k=cnt.get(name,0); cnt[name]=k+1
        args=mc.group(2)
        if 'Unable to find stack arg' in args and name in jals and k<len(jals[name]):
            def rs(mm):
                return resolve_int(ins,jals[name][k],8 if mm.group(1)=='0x10' else 9)
            args=re.sub(r'M2C_ERROR\(/\* Unable to find stack arg (0x1[04]) in block \*/\)',rs,args)
            if 'M2C_ERROR(/* Read from unset register' not in args: return name+'('+args+')'
        if 'M2C_ERROR(/* Read from unset register' not in args: return mc.group(0)
        pr=proto(name); 
        if pr is None or name not in jals or k>=len(jals[name]):
            unres+=1; return mc.group(0)
        # float param -> freg
        fregs=[];n=0
        for q in pr:
            if q.startswith(('f32','float')) and '*' not in q: fregs.append(12+n); n+=1
            else: fregs.append(None)
        # split top-level args
        depth=0;cur='';al=[]
        for ch in args:
            if ch in '([': depth+=1
            if ch in ')]': depth-=1
            if ch==',' and depth==0: al.append(cur.strip());cur=''
            else: cur+=ch
        al.append(cur.strip())
        def bits(t):
            mm=re.match(r'^(-?[\d\.]+(?:e-?\d+)?)f$',t)
            if mm:
                b=struct.unpack('<I',struct.pack('<f',float(mm.group(1))))[0]
                return str(b) if b<10 else '0x%X'%b
            return t
        nfl=sum(1 for q in fregs if q is not None)
        shifted=any(q is None for q in fregs[:0]) 
        # position of first float
        first=next((i for i,q in enumerate(fregs) if q is not None),None)
        if first is not None and len(al)==len(fregs) and any(q is None for q in fregs[first+1:]):
            newal=[None]*len(fregs); n=0
            for i,q in enumerate(fregs):
                if q is not None: newal[i]=resolve(ins,jals[name][k],q)
                else:
                    if n==0 and fregs[0] is not None: newal[i]='em'
                    else: newal[i]=bits(al[n]) if n<len(al) else '0'
                    n+=1
            return name+'('+', '.join(newal)+')'
        for i,a in enumerate(al):
            if 'M2C_ERROR' in a and i<len(fregs) and fregs[i] is not None:
                al[i]=resolve(ins,jals[name][k],fregs[i])
        return name+'('+', '.join(al)+')'
    # find calls with balanced parens containing M2C_ERROR: handle by manual scan
    outp='';i=0
    pat=re.compile(r'\b(\w+)\(')
    while True:
        mm=pat.search(p,i)
        if not mm: outp+=p[i:];break
        name=mm.group(1)
        # find matching paren
        j=mm.end();d=1
        while d and j<len(p):
            if p[j]=='(':d+=1
            elif p[j]==')':d-=1
            j+=1
        inner=p[mm.end():j-1]
        if name in jals:
            fake=re.match(r'(\w+)\((.*)\)$',name+'('+inner+')',re.S)
            outp+=p[i:mm.start()]+fixcall(fake)
            i=j
        else:
            outp+=p[i:mm.end()]; i=mm.end()
    res.append(outp)
open(out,'w').write(''.join(res))
print('unresolved calls',unres, 'remaining ERR',''.join(res).count('M2C_ERROR(/* Read from unset'))
