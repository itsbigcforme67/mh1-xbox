#!/usr/bin/env python3
"""dbf.py FILE FUNC "decl1|decl2|..." : brute-force the order of the given local declaration lines (must be contiguous top-of-function lines, text w/o indent); parallel 3; writes best."""
import itertools,subprocess,re,sys,concurrent.futures as cf,os,shutil
f,func,spec=sys.argv[1:4]
decls=spec.split('|')
src=open(f).read()
block=''.join('    %s\n'%d for d in decls)
assert block in src, 'declaration block not found contiguous in that order'
def run(perm):
    s=src.replace(block,''.join('    %s\n'%d for d in perm),1)
    tmp=f.replace('.c','_zz%d.c'%abs(hash(perm)%100000))
    open(tmp,'w').write(s)
    try:
        out=subprocess.run(['python3','tools/check.py',tmp,'--module','lobby'],capture_output=True,text=True).stdout
    finally:
        os.remove(tmp)
    l=[x for x in out.split('\n') if (' %s '%func) in x]
    if not l: return perm,99999
    if l[0].startswith('OK'): return perm,0
    m=re.search(r'\((\d+)/',l[0]); return perm,int(m.group(1)) if m else 99999
best=None
with cf.ThreadPoolExecutor(3) as ex:
    for perm,sc in ex.map(run,list(itertools.permutations(decls))):
        if best is None or sc<best[1]: best=(perm,sc); print(sc,perm,flush=True)
if best[1]<99999:
    open(f,'w').write(src.replace(block,''.join('    %s\n'%d for d in best[0]),1))
print('best',best)
