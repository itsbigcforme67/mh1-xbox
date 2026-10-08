#!/usr/bin/env python3
"""declhill.py FILE FUNCMARK FUNC : hill-climb over the order of the leading declaration lines of FUNC
(pairwise swaps and moves), keeping the best (check.py diff count, then align lines). Writes the result back."""
import sys,re,os,itertools
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from vt import fn_span, score
f,mark,fn=sys.argv[1:4]
src=open(f).read(); a,b=fn_span(src,mark)
lines=src[a:b].split('\n')
i=1; decl=[]
while re.match(r'^    [A-Za-z_][\w \*]*?[\w\]\[]+( = [^;]+)?;$',lines[i]) and '(' not in lines[i].split('=')[0]:
    decl.append(lines[i]); i+=1
rest=lines[i:]
z=os.path.join(os.path.dirname(f),'zzh.c')
cache={}
def ev(p):
    k=tuple(p)
    if k in cache: return cache[k]
    nt='\n'.join([lines[0]]+list(p)+rest)
    open(z,'w').write(src[:a]+nt+src[b:])
    out,n=score(z,fn)
    m=re.search(r'\((\d+)/',out)
    d=int(m.group(1)) if m else (0 if out.startswith('OK') else 9999)
    cache[k]=(d,n); return cache[k]
cur=decl[:]; best=ev(cur); print('start',best); sys.stdout.flush()
improved=True
while improved and best[0]>0:
    improved=False
    n=len(cur)
    cands=[]
    for x in range(n):
        for y in range(n):
            if x==y: continue
            p=cur[:]; item=p.pop(x); p.insert(y,item); cands.append(p)
    for x in range(n):
        for y in range(x+1,n):
            p=cur[:]; p[x],p[y]=p[y],p[x]; cands.append(p)
    for p in cands:
        s=ev(p)
        if s<best:
            best=s; cur=p; improved=True; print(best,[l.strip() for l in cur]); sys.stdout.flush(); break
os.path.exists(z) and os.remove(z)
nt='\n'.join([lines[0]]+cur+rest)
open(f,'w').write(src[:a]+nt+src[b:])
print('final',best)
