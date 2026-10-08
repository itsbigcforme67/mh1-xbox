#!/usr/bin/env python3
"""stmtmove.py FILE FUNC FIRST LAST : hill-climb over the order of the single-line statements FIRST..LAST (line numbers of
FILE, inclusive) of FUNC: every move of one line to another place is scored with check.py; improvements are kept.
Statements must be independent single lines (no braces). Writes the best order into FILE."""
import sys,re,subprocess
f,func=sys.argv[1],sys.argv[2]; a,b=int(sys.argv[3]),int(sys.argv[4])
L=open(f).read().split('\n')
head=L[:a-1]; blk=L[a-1:b]; tail=L[b:]
def score(bl):
    open(f,'w').write('\n'.join(head+bl+tail))
    out=subprocess.run(['python3','tools/check.py',f],capture_output=True,text=True).stdout
    m=re.search(r'(OK|--)  '+re.escape(func)+r' .*?(?:\((\d+)/\d+ instructions differ\))?$',out,re.M)
    if not m: return 9999
    return 0 if m.group(1)=='OK' else int(m.group(2) or 9997)
best=score(blk); print('start',best)
imp=True
while imp and best>0:
    imp=False
    n=len(blk)
    for i in range(n):
        for j in range(n):
            if i==j: continue
            c=blk[:]; x=c.pop(i); c.insert(j,x)
            s=score(c)
            if s<best:
                best=s; blk=c; imp=True; print('improved',best); break
        if imp: break
open(f,'w').write('\n'.join(head+blk+tail))
print('final',best); print('\n'.join(blk))
