#!/usr/bin/env python3
"""declrand.py FILE FUNCMARK FUNC [N] [seed]: try N random permutations of the leading declaration lines
of FUNC (lines from the line after the signature up to the first line that is not a declaration), report best."""
import sys,random,re,subprocess,os,itertools
sys.path.insert(0,os.path.join(os.path.dirname(__file__)))
from vt import fn_span, score
f,mark,fn=sys.argv[1:4]
N=int(sys.argv[4]) if len(sys.argv)>4 else 100
random.seed(int(sys.argv[5]) if len(sys.argv)>5 else 1)
src=open(f).read(); a,b=fn_span(src,mark)
t=src[a:b]
lines=t.split('\n')
i=1; decl=[]
while re.match(r'^    [A-Za-z_][\w \*]*?[\w\]]+( = [^;]+)?;$',lines[i]) and '(' not in lines[i].split('=')[0]:
    decl.append(lines[i]); i+=1
rest=lines[i:]
print(len(decl),'decl lines')
z=os.path.join(os.path.dirname(f),'zzr.c')
best=None
perms=set()
tries=0
while tries<N:
    p=decl[:]; random.shuffle(p)
    key=tuple(p)
    if key in perms: continue
    perms.add(key); tries+=1
    nt='\n'.join([lines[0]]+p+rest)
    open(z,'w').write(src[:a]+nt+src[b:])
    out,n=score(z,fn)
    m=re.search(r'\((\d+)/',out)
    d=int(m.group(1)) if m else (0 if out.startswith('OK') else 9999)
    if best is None or (d,n)<best[0]:
        best=((d,n),nt); print(d,n,[x.strip() for x in p]); sys.stdout.flush()
    if d==0: break
os.remove(z)
open('/tmp/declrand_best.txt','w').write(best[1])
