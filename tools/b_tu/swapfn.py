import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import re,sys,os
os.chdir(ROOT)
S=SCR+'/'
exec(open(os.path.join(ROOT,'tools/b_tu/subst.py')).read().split("nm = open")[0])
f,fn,srcf=sys.argv[1],sys.argv[2],sys.argv[3]
a=open(f).read(); b=open(srcf).read()
new=[t for n,t in split_chunks(b) if n==fn][0]
out=[]; done=False
for n,t in split_chunks(a):
    if n==fn: out.append(new); done=True
    else: out.append(t)
assert done
open(f,'w').write(''.join(out)); print('swapped',fn)
