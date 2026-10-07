# mergetu.py OUT FILE...  : merge scratch all-C files (de-dup declaration lines, functions ordered by address)
import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import re,sys,os
os.chdir(ROOT)
S=SCR+'/'
exec(open(os.path.join(ROOT,'tools/b_tu/subst.py')).read().split("nm = open")[0])
out=sys.argv[1]; files=sys.argv[2:]
addr={}
for l in open('config/symbols/main.txt'):
    m=re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+)',l)
    if m: addr.setdefault(m.group(1),int(m.group(2),16))
decls=[];seen=set();funcs={}
for f in files:
    s=open(S+f).read()
    for n,t in split_chunks(s):
        if n is None:
            for ln in t.split('\n'):
                k=ln.strip()
                if not k: continue
                if k in seen and not k.startswith(('/*',' *','*/')) : continue
                seen.add(k); decls.append(ln)
        else:
            bare=n
            if bare not in addr: print('no addr',n); continue
            funcs[addr[bare]]=t
text='\n'.join(decls)+'\n\n'+'\n'.join(funcs[a] for a in sorted(funcs))+'\n'
open(S+out,'w').write(text)
print('funcs',len(funcs))
