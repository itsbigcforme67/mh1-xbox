import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import sys,subprocess,re,os
os.chdir(ROOT)
S=SCR+'/'
a=sys.argv[1:]
apply='--apply' in a
if apply: a.remove('--apply')
func=a[0]; pairs=a[1:]
s=open(S+os.environ.get('FS','full_static.c')).read()
for i in range(0,len(pairs),2):
    if pairs[i] not in s: print('MISSING:',pairs[i][:60]); sys.exit(1)
    s=s.replace(pairs[i],pairs[i+1])
open(S+'try_tmp.c','w').write(s)
cc=subprocess.run([os.path.join(ROOT,"tools/b_tu/cc1.sh"),S+'try_tmp.c'],capture_output=True,text=True)
err=[l for l in cc.stdout.split('\n') if 'Error' in l or re.match(r'#\s+\d+:',l)]
out=subprocess.run(['python3','tools/align.py',S+'try_tmp.c',func],capture_output=True,text=True).stdout
if not out.strip() and err: print('COMPILE ERR',err[:4]); sys.exit(1)
print('hunks',len(re.findall(r'^(replace|insert|delete)',out,re.M)))
if apply: open(S+os.environ.get('FS','full_static.c'),'w').write(s); print('applied')
