#!/usr/bin/env python3
"""tubuild.py OUT.c SPEC...  SPEC = file.c:func  (in address order).
Assembles a whole-file translation unit from the per-function chunks of existing
files (preamble lines of every source file are merged, duplicates dropped).
A function may be listed as file.c:func:static to prefix 'static' to its definition.
Standard library only."""
import re,sys
def chunk(path,name):
    L=open(path).read().split('\n')
    pat=re.compile(r'^[A-Za-z_][^;]*[\s\*]'+re.escape(name)+r'\(.*\)\s*\{\s*$')
    for i,l in enumerate(L):
        if pat.match(l): break
    else: raise SystemExit('not found %s %s'%(path,name))
    j=i
    # comment block above
    k=i-1
    while k>=0 and (L[k].startswith('/*') or L[k].startswith(' *') or L[k].startswith('//')): k-=1
    start=k+1
    # inline/static modifiers on previous line? no
    e=i
    while L[e]!='}': e+=1
    return start,e,L
def preamble(path):
    L=open(path).read().split('\n')
    pat=re.compile(r'^[A-Za-z_][^;]*\(.*\)\s*\{\s*$')
    out=[];i=0
    while i<len(L):
        l=L[i]
        if pat.match(l) and not l.startswith('typedef') and not l.startswith('struct'):
            while L[i]!='}': i+=1
            i+=1; continue
        out.append(l); i+=1
    return out
out=sys.argv[1]; specs=sys.argv[2:]
seen=set(); pre=[]; body=[]
files=[]
for s in specs:
    f=s.split(':')[0]
    if f not in files: files.append(f)
inc=[];rest=[]
for f in files:
    for l in preamble(f):
        if l.startswith('#include'):
            if l not in inc: inc.append(l)
        elif l.strip()=='' or l.startswith('/*') or l.startswith(' *'): continue
        elif l not in seen:
            seen.add(l); rest.append(l)
for s in specs:
    p=s.split(':'); st,e,L=chunk(p[0],p[1])
    txt=L[st:e+1]
    if len(p)>2 and p[2]=='static':
        idx=st if False else [x for x in range(len(txt)) if re.match(r'^[A-Za-z_]',txt[x])][0]
        if not txt[idx].startswith('static'): txt[idx]='static '+txt[idx]
    body.append('\n'.join(txt))
open(out,'w').write('\n'.join(inc)+'\n\n'+'\n'.join(rest)+'\n\n'+'\n\n'.join(body)+'\n')
