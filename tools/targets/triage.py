import subprocess,re,sys,os,glob,collections
S='.'
objs=collections.defaultdict(list)
fn_of={}
for l in open(S+'/nm_out.txt'):
    p=l.split()
    if len(p)>=4 and p[1].startswith('0x'):
        for o in p[3:]:
            objs[o].append(p[0])
res=[]
for o,fns in sorted(objs.items()):
    base=o[:-2]
    if base.startswith('rt_') or base in('trans_stage',): continue
    c=glob.glob('src/**/%s.c'%base,recursive=True)
    if not c: 
        print('nosrc',o); continue
    out=subprocess.run(['python3','tools/check.py',c[0],'-v'],capture_output=True,text=True).stdout.split('\n')
    cur=None;L=[];R=[]
    def fin():
        if cur and cur in fns:
            nl=[x for x in L if not x.startswith('nop')]; nr=[x for x in R if not x.startswith('nop')]
            jl=sum(1 for x in nl if x.startswith('jal')); jr=sum(1 for x in nr if x.startswith('jal'))
            res.append((cur,c[0],len(nl),len(nr),jl,jr))
    for ln in out:
        m=re.match(r'^(OK|--)  (\S+)',ln)
        if m:
            fin(); cur=m.group(2); L=[];R=[]; continue
        if '|' in ln:
            a,b=ln.split('|',1)
            a=re.sub(r'^\s*(>>)?\s*[0-9A-F]{8}\s+','',a).strip(); b=b.strip()
            if a: L.append(a)
            if b: R.append(b)
    fin()
for r in res:
    flag='' if (r[2]==r[3] and r[4]==r[5]) else 'SIZE/JAL'
    print(*r,flag)
