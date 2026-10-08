#!/usr/bin/env python3
"""tryrep.py FILE FUNC OLD VARIANTS.txt : replace the exact text OLD (once) in FILE by each variant (separated by
'//----' lines) and report check.py's difference count for FUNC; the best variant stays in FILE."""
import sys,re,subprocess
f,func,oldf,vf=sys.argv[1:5]
old=open(oldf).read().rstrip('\n')
src=open(f).read()
assert src.count(old)==1,'OLD must occur once (%d)'%src.count(old)
vs=[v.strip('\n') for v in open(vf).read().split('//----') if v.strip()]
def cnt():
    out=subprocess.run(['python3','tools/check.py',f],capture_output=True,text=True).stdout
    m=re.search(r'(OK|--)  '+re.escape(func)+r' .*?(?:\((\d+)/\d+ instructions differ\))?$',out,re.M)
    if not m: return 9999
    return 0 if m.group(1)=='OK' else int(m.group(2) or 9997)
best=None
for n,v in enumerate(vs):
    open(f,'w').write(src.replace(old,v))
    c=cnt(); print(n,c)
    if best is None or c<best[0]: best=(c,n,v)
    if c==0: break
open(f,'w').write(src.replace(old,best[2]))
print('best',best[1],best[0])
