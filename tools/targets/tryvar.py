#!/usr/bin/env python3
"""tryvar.py FILE FUNC VARIANTS.txt : replace FUNC's definition in FILE by each variant (separated by lines
'//----') and report check.py's instruction-difference count; keeps the best variant in FILE (prints it)."""
import sys,re,subprocess
f,func,vf=sys.argv[1:4]
src=open(f).read()
L=src.split('\n')
pat=re.compile(r'^(?:static )?[A-Za-z_][\w \*]*?[ \*]'+re.escape(func)+r'\(.*\{\s*$')
i=[k for k,l in enumerate(L) if pat.match(l)][0]
j=i
while L[j]!='}': j+=1
head,tail='\n'.join(L[:i]),'\n'.join(L[j+1:])
vs=[v.strip('\n') for v in open(vf).read().split('//----') if v.strip()]
best=None
for n,v in enumerate(vs):
    open(f,'w').write(head+'\n'+v+'\n'+tail)
    out=subprocess.run(['python3','tools/check.py',f],capture_output=True,text=True).stdout
    m=re.search(r'(OK|--)  '+re.escape(func)+r' .*?(?:\((\d+)/\d+ instructions differ\))?$',out,re.M)
    if 'rror' in out and not m: cnt=9999
    elif not m: cnt=9998
    else: cnt=0 if m.group(1)=='OK' else int(m.group(2) or 9997)
    print(n,cnt)
    if best is None or cnt<best[0]: best=(cnt,n,v)
    if cnt==0: break
open(f,'w').write(head+'\n'+best[2]+'\n'+tail)
print('best',best[1],best[0])
