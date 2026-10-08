#!/usr/bin/env python3
"""weaken.py LOG : for every 'FILE:LINE: multiple definition of `X'' in the link log whose FILE is a
src/pc source, put __attribute__((weak)) on the definition so the matched game C wins. Prints what it changed."""
import re,sys
done=set()
for l in open(sys.argv[1]):
    m=re.match(r"(.*?\.c):(\d+): multiple definition of `(\w+)'; (\S+?\.o):(.*?):(\d+): first defined here",l)
    if not m: continue
    f,ln,name=m.group(1),int(m.group(2)),m.group(3)
    if '/src/pc/' not in f: continue
    f=f[f.index('src/pc/'):]
    if (f,name) in done: continue
    L=open(f).read().split('\n')
    # definition may begin a few lines above the reported line (the line of the function name is reported)
    for k in range(ln-1,max(ln-6,-1),-1):
        if re.search(r'\b'+name+r'\s*\(',L[k]) :
            if 'weak' not in L[k]:
                L[k]='__attribute__((weak)) '+L[k]
                open(f,'w').write('\n'.join(L)); print('weak',f,k+1,name)
            done.add((f,name)); break
    else: print('NOT FOUND',f,ln,name)
