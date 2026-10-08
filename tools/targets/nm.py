import csv,subprocess,glob,os,collections
rows=[r for r in csv.DictReader(open('docs/survey/mh1_symbols.csv')) if r['type']=='FUNC' and r['section']=='main']
by={r['name']:r for r in rows}
rng=[]
for l in open('config/c_files.txt'):
    f=l.split('#')[0].split()
    if f and f[0]=='main': rng.append((int(f[1],0),int(f[2],0)))
def m(a): return any(lo<=a<hi for lo,hi in rng)
src=collections.defaultdict(list)
for o in glob.glob('build/pc/*.o'):
    out=subprocess.run(['nm','--defined-only',o],capture_output=True,text=True).stdout
    for ln in out.splitlines():
        p=ln.split()
        if len(p)==3 and p[1] in 'TWt' and p[2] in by:
            src[p[2]].append((os.path.basename(o),p[1]))
res=[]
for n,l in src.items():
    r=by[n]; a=int(r['addr'],16)
    if not m(a): res.append((n,r['addr'],int(r['size']),l))
res.sort(key=lambda x:x[1])
tot=0
for n,a,s,l in res:
    tot+=s; print(n,a,s,' '.join(x[0] for x in l))
print(len(res),tot)
