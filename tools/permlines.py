#!/usr/bin/env python3
"""permlines.py FILE FUNC START_MARK END_MARK : brute-force order of the statement lines between two marker lines (exclusive) inside FILE."""
import itertools,subprocess,re,sys
p,func,start,end=sys.argv[1:5]
s=open(p).read()
a=s.index(start)+len(start)
b=s.index(end,a)
lines=[l for l in s[a:b].split('\n') if l.strip()]
print(len(lines),'lines')
best=None
for perm in itertools.permutations(range(len(lines))):
    body='\n'+'\n'.join(lines[i] for i in perm)+'\n'
    open(p,'w').write(s[:a]+body+s[b:])
    out=subprocess.run(['python3','tools/check.py',p],capture_output=True,text=True).stdout
    m=re.search(r'^(--|OK)\s+%s\s.*?(?:\((\d+)/)?'%re.escape(func),out,re.M)
    if not m: continue
    n=0 if m.group(1)=='OK' else int(re.search(r'^--\s+%s\s.*\((\d+)/'%re.escape(func),out,re.M).group(1))
    if best is None or n<best[0]:
        best=(n,perm); print(n,perm,flush=True)
    if n==0: break
perm=best[1]
open(p,'w').write(s[:a]+'\n'+'\n'.join(lines[i] for i in perm)+'\n'+s[b:])
print('best',best)
