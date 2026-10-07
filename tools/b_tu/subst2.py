# subst2.py NMFILE OUT LO HI  : nm file with matched run bodies (c_files runs in [LO,HI)) substituted
import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import re,sys,os
os.chdir(ROOT)
S=SCR+'/'
exec(open(os.path.join(ROOT,'tools/b_tu/subst.py')).read().split("nm = open")[0])
nmf,outf,lo,hi=sys.argv[1],sys.argv[2],int(sys.argv[3],16),int(sys.argv[4],16)
nm=open(nmf).read(); cks=split_chunks(nm)
runs={};files=[]
for l in open('config/c_files.txt'):
    p=l.split()
    if len(p)>=4 and p[0]=='main' and lo<=int(p[1],16)<hi:
        s=open('src/main/%s.c'%p[3]).read(); files.append(p[3])
        s=re.sub(r'^/\*.*?\*/\n','',s,count=1,flags=re.S)
        for n,t in split_chunks(s):
            if n: runs[n]=t
out=[];rep=[]
for n,t in cks:
    if n and n in runs: out.append(runs[n]); rep.append(n)
    else: out.append(t)
open(outf,'w').write(''.join(out))
print('matched bodies',len(runs),'replaced',len(rep),'files',len(files)); print('not in nm:',[n for n in runs if n not in rep])
print('nm functions not matched:',[n for n,t in cks if n and n not in runs])
