import csv,subprocess,glob,os,collections
rows=[r for r in csv.DictReader(open('docs/survey/mh1_symbols.csv')) if r['type']=='FUNC' and r['section']=='main']
by={r['name']:r for r in rows}
rng=[]
for l in open('config/c_files.txt'):
    f=l.split('#')[0].split()
    if f and f[0]=='main': rng.append((int(f[1],0),int(f[2],0),f[3]))
def m(a):
    for lo,hi,p in rng:
        if lo<=a<hi: return p
src=collections.defaultdict(list)
for o in glob.glob('build/pc/*.o'):
    out=subprocess.run(['nm','--defined-only',o],capture_output=True,text=True).stdout
    for ln in out.splitlines():
        p=ln.split()
        if len(p)==3 and p[1] in 'TWt' and p[2] in by:
            src[p[2]].append((os.path.basename(o)[:-2],p[1]))
res=[]
for n,l in src.items():
    r=by[n]; a=int(r['addr'],16); mf=m(a)
    if mf:
        base=os.path.basename(mf)
        names=[x[0] for x in l]
        if base not in names: res.append((r['addr'],int(r['size']),n,mf,names))
res.sort()
for x in res: print(*x)
print(len(res),sum(x[1] for x in res))
