import re,subprocess,sys,os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
"""genruns.py NM.c PREFIX FIRSTLETTER MINADDR [extra_ok_names...]  (agent E helper)
Reorders the functions of a near-match file (the whole file's C, brace on its own line) into address order, then writes the
matching runs (consecutive functions that check.py reports OK, plus the named extras) into PREFIX<letter>.c with
tools/split_runs.py and prints the 'main START END path' lines for config/c_files.txt. Example:
python3 tools/genruns.py src/main/stage/f_stage_nm.c src/main/stage/f_stage b 15C6B0"""
nm,prefix,first,minaddr=sys.argv[1:5]
minaddr=int(minaddr,16)
extra=set(sys.argv[5:])
skip=set(x for x in os.environ.get('GENRUNS_SKIP','').split(',') if x)
ns={}
exec(open('tools/split_runs.py').read().split('def main')[0],ns)
parse=ns['parse']
src=open(nm).read()
lines,funcs=parse(src)
out=subprocess.run(['python3','tools/check.py',nm],capture_output=True,text=True).stdout
addr={};ok={};size={}
for l in out.split('\n'):
    m=re.match(r'(OK|--)\s+(\S+)\s+main\s+0x([0-9A-F]+)\s+(\d+) bytes',l)
    if m:
        addr[m.group(2)]=int(m.group(3),16); ok[m.group(2)]=((m.group(1)=='OK') or m.group(2) in extra) and m.group(2) not in skip; size[m.group(2)]=int(m.group(4))
# reorder functions by address
inbody=set()
for f in funcs: inbody.update(range(f[1],f[2]+1))
decl=[lines[k] for k in range(len(lines)) if k not in inbody]
decl=re.sub(r'\n{3,}','\n\n','\n'.join(decl)).rstrip()+'\n'
fs=sorted(funcs,key=lambda f:addr.get(f[0],1<<30))
open(nm,'w').write(decl+'\n'+'\n\n'.join('\n'.join(lines[f[1]:f[2]+1]) for f in fs)+'\n')
names=[f[0] for f in fs]
rows=sorted([(addr[n],size[n],n) for n in names if n in addr])
runs=[];cur=[]
for a,s,n in rows:
    if a>=minaddr and ok[n]: cur.append((a,s,n))
    else:
        if cur: runs.append(cur)
        cur=[]
if cur: runs.append(cur)
letters=list('bcdefghijklmnopqrstuvwxyz')+[a+b for a in 'abcdefghijklmnopqrstuvwxyz' for b in 'abcdefghijklmnopqrstuvwxyz']
if first!='-': letters=letters[letters.index(first):]
args=[];cfg=[]
for k,run in enumerate(runs):
    f=run[0];l=run[-1]
    args.append('%s:%s-%s'%(letters[k],f[2],l[2]))
    cfg.append('main 0x%08X 0x%08X %s%s'%(f[0],l[0]+l[1],os.path.relpath(prefix+letters[k],'src/main'),''))
subprocess.run(['python3','tools/split_runs.py',nm,prefix]+args,check=True,capture_output=True)
# strip static: the near-match file needs `static` helpers (callers keep values in temp registers), the linked run files must export them
keep_=set(x for x in os.environ.get('GENRUNS_KEEP_STATIC','').split(',') if x)   # LOCAL functions whose callers are in the same run
def fix_static(m):
    mm=re.search(r'\b(\w+)\(',m.group(0))
    return 'static ' if mm and mm.group(1) in keep_ else ''
for a_ in args:
    fn_=prefix+a_.split(':')[0]+'.c'
    t_=open(fn_).read()
    t_=re.sub(r'^static [^\n]*',lambda m: fix_static(m)+m.group(0)[len('static '):],t_,flags=re.M)
    open(fn_,'w').write(t_)
print('\n'.join(args)); print('\n'.join(cfg))
