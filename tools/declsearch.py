#!/usr/bin/env python3
"""declsearch.py SRC MARK FUNC SCRATCH [seconds]: random move search over the order of the leading declaration lines of FUNC
(arrays allowed; declhill.py/declrand.py before 8 Oct 2026 stopped at the first array declaration and silently tried nothing).
SRC is only read; SCRATCH is a throw-away copy path in the same directory (compiled by check.py); the best source is written to
/tmp/declsearch_best_FUNC.txt. MARK is the exact signature line."""
import sys,re,subprocess,random,time,os
src_path,mark,fn,scratch=sys.argv[1:5]
secs=int(sys.argv[5]) if len(sys.argv)>5 else 1200
src=open(src_path).read()
a=src.index(mark)
b=src.index("\n}\n",a)+3
lines=src[a:b].split('\n')
i=1;decl=[]
while re.match(r'^    [A-Za-z_][\w \*]*?[\w\]\[]+( = [^;]+)?;$',lines[i]) and '(' not in lines[i].split('=')[0]:
    decl.append(lines[i]); i+=1
rest=lines[i:]
print(len(decl),'decls',flush=True)
cache={}
def ev(p):
    k=tuple(p)
    if k in cache: return cache[k]
    open(scratch,'w').write(src[:a]+'\n'.join([lines[0]]+list(p)+rest)+src[b:])
    out=subprocess.run(['python3','tools/check.py',scratch],capture_output=True,text=True).stdout
    d=9999
    for l in out.split('\n'):
        if (' %s '%fn) in l:
            m=re.search(r'\((\d+)/',l); d=int(m.group(1)) if m else 0
    cache[k]=d; return d
t0=time.time()
cur=decl[:]; best=ev(cur); bestp=cur[:]
print('start',best,flush=True)
random.seed(7)
while time.time()-t0<secs and best>0:
    # local search from a random perturbation of the best
    p=bestp[:]
    for _ in range(random.randint(1,3)):
        x=random.randrange(len(p)); y=random.randrange(len(p))
        it=p.pop(x); p.insert(y,it)
    s=ev(p)
    if s<=best:
        if s<best: print(s,[l.strip() for l in p],flush=True)
        best=s; bestp=p
open('/tmp/declsearch_best_%s.txt'%fn,'w').write('\n'.join([lines[0]]+bestp+rest))
print('final',best)
if os.path.exists(scratch): os.remove(scratch)
