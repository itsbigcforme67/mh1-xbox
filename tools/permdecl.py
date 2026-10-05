#!/usr/bin/env python3
"""permdecl.py NM.c FUNC [maxperms] [helper,helper...]: brute-force the order of the local declarations of FUNC (Allman or K&R)
using a mini file (decls + that function + the named helper functions defined earlier, e.g. static leaf callees) compiled with tools/check.py. Writes the best order back into NM.c."""
import re,subprocess,sys,os,itertools,random,tempfile
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
nm,func=sys.argv[1],sys.argv[2]
maxp=int(sys.argv[3]) if len(sys.argv)>3 else 120
ns={}
exec(open('tools/split_runs.py').read().split('def main')[0],ns)
parse=ns['parse']
def mini(src):
    lines,funcs=parse(src)
    inbody=set()
    for f in funcs: inbody.update(range(f[1],f[2]+1))
    decl='\n'.join(lines[k] for k in range(len(lines)) if k not in inbody)
    f=[f for f in funcs if f[0]==func][0]
    body='\n\n'.join('\n'.join(lines[g[1]:g[2]+1]) for g in funcs if g[1]<=f[1] and (g[0]==func or g[0] in HELPERS))
    return decl+'\n'+body+'\n'
HELPERS=set(sys.argv[4].split(',')) if len(sys.argv)>4 else set()
src=open(nm).read()
lines,funcs=parse(src)
f=[f for f in funcs if f[0]==func][0]
# declaration block: lines after the '{' line
start=f[1]
while lines[start]!='{': start+=1
k=start+1
decl=[]
while True:
    l=lines[k]
    if re.match(r'^    (?:static )?(?:struct )?[A-Za-z_][\w ]*[\s\*]+[\w\[\]\*, ]+(?:\s*=\s*[^;]+)?;\s*$',l) and '(' not in l.split('=')[0] and not l.strip().startswith(('return','if','for','while','switch','goto')):
        decl.append(l); k+=1
    else: break
if len(decl)<2: print('nothing to permute',len(decl)); sys.exit()
tmp=tempfile.mktemp(suffix='.c',dir=os.path.join(os.getcwd(),'build'))
def score(order):
    nl=lines[:start+1]+list(order)+lines[start+1+len(decl):]
    text='\n'.join(nl)
    open(tmp,'w').write(mini(text))
    out=subprocess.run(['python3','tools/check.py',tmp],capture_output=True,text=True).stdout
    l=[x for x in out.split('\n') if (' %s '%func) in x]
    if not l: return 99999
    if l[0].startswith('OK'): return 0
    m=re.search(r'\((\d+)/',l[0]); return int(m.group(1))
import math
best=None
if len(decl)<=7 or maxp<=0:
    perms=list(itertools.permutations(decl)) if len(decl)<=7 else []
    if len(perms)>maxp:
        random.seed(1); orig=tuple(decl); rest=random.sample(perms,maxp-1); perms=[orig]+rest
    for p in perms:
        sc=score(p)
        if best is None or sc<best[0]:
            best=(sc,p); print(sc,[x.strip() for x in p]); sys.stdout.flush()
        if sc==0: break
else:
    # hill climbing with pair swaps (too many permutations)
    cur=list(decl); cs=score(cur); best=(cs,tuple(cur)); print('start',cs); sys.stdout.flush()
    improved=True
    while improved and best[0]>0:
        improved=False
        for i in range(len(cur)):
            for j in range(i+1,len(cur)):
                t=list(cur); t[i],t[j]=t[j],t[i]
                sc=score(t)
                if sc<best[0]:
                    best=(sc,tuple(t)); cur=t; improved=True; print(sc,[x.strip() for x in t]); sys.stdout.flush()
                    if sc==0: break
            if best[0]==0: break
os.remove(tmp)
nl=lines[:start+1]+list(best[1])+lines[start+1+len(decl):]
open(nm,'w').write('\n'.join(nl))
print('best',best[0])
