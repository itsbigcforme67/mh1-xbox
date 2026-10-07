import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import sys,os,re
os.chdir(ROOT)
S=SCR+'/'
exec(open(os.path.join(ROOT,'tools/b_tu/subst.py')).read().split("nm = open")[0])
src=open(S+sys.argv[1]).read()
fn=sys.argv[2]
cks=split_chunks(src)
out=[]
for n,t in cks:
    if n is None: out.append(t)
    elif n==fn: out.append(t)
open(S+'p_%s.c'%fn,'w').write(''.join(out))
