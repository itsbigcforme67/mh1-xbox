"""vt.py helper: from vt import try_variants
try_variants(file, funcmark, funcname, [ [(old,new),...], ... ], apply_first=False)
Each variant is a list of replacements applied to the function text. Prints the check.py diff
count and the number of lines in align.py output. With apply=i, writes that variant into the file."""
import subprocess,os,sys
def fn_span(src,mark):
    a=src.index(mark); b=src.index("\n}\n",a)+3
    return a,b
def score(path,fn):
    out=subprocess.run(['./tools/cnt.sh',path,fn],capture_output=True,text=True).stdout.strip()
    al=subprocess.run(['python3','tools/align.py',path,fn],capture_output=True,text=True).stdout.split('\n')
    n=sum(1 for l in al if l.startswith('   - ') or l.startswith('   + '))
    return out,n
def try_variants(file,mark,fn,variants,apply=None):
    src=open(file).read(); a,b=fn_span(src,mark); base=src[a:b]
    z=os.path.join(os.path.dirname(file),'zzv.c')
    res=[]
    for i,v in enumerate(variants):
        t=base
        ok=True
        for old,new in v:
            if old not in t: print(i,'MISSING',repr(old[:40])); ok=False; break
            t=t.replace(old,new,1)
        if not ok: continue
        open(z,'w').write(src[:a]+t+src[b:])
        out,n=score(z,fn)
        print(i,out[-40:] if out else 'COMPILE ERROR',n)
        res.append((n,i,t))
    if os.path.exists(z): os.remove(z)
    if apply is not None:
        t=[r for r in res if r[1]==apply][0][2]
        open(file,'w').write(src[:a]+t+src[b:])
    return res
