import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
# stperm.py FUNC 'block text (exact, contiguous independent statements, one per line)' : try all orders, report best
import sys,os,re,itertools,subprocess
os.chdir(ROOT)
S=SCR+'/'
func=sys.argv[1]; block=sys.argv[2].strip('\n')
fs=os.environ.get('FS','full_static.c')
src=open(S+fs).read()
assert block in src, 'block not found'
lines=block.split('\n')
best=None
for p in itertools.permutations(range(len(lines))):
    t=src.replace(block,'\n'.join(lines[i] for i in p))
    open(S+'stp_tmp.c','w').write(t)
    out=subprocess.run(['python3','tools/align.py',S+'stp_tmp.c',func],capture_output=True,text=True).stdout
    sc=len(re.findall(r'^(replace|insert|delete)',out,re.M)) if out.strip() else 9999
    if best is None or sc<best[0]:
        best=(sc,p); print(sc,p); sys.stdout.flush()
    if sc==0: break
open(S+'stp_best.c','w').write(src.replace(block,'\n'.join(lines[i] for i in best[1])))
print('best',best)
