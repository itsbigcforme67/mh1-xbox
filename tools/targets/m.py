import csv,sys
rows=[r for r in csv.DictReader(open('docs/survey/mh1_symbols.csv')) if r['type']=='FUNC' and r['section']=='main']
rng=[]
for l in open('config/c_files.txt'):
    f=l.split('#')[0].split()
    if f and f[0]=='main': rng.append((int(f[1],0),int(f[2],0),f[3] if len(f)>3 else ''))
def m(a): 
    for lo,hi,p in rng:
        if lo<=a<hi: return p
    return None
for n in sys.argv[1:]:
    for r in rows:
        if r['name']==n:
            print(n,r['addr'],r['size'],m(int(r['addr'],16)))
