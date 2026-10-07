import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
# gen2.py SRC LO HI RUNNAME STUBS...  : write src/main/RUNNAME.c from scratch SRC (all-C TU) with listed functions as raw asm,
#   register run LO..(end of last fn), remove old runs in range, fix c_rawfuncs.
import re,sys,os,subprocess
os.chdir(ROOT)
S=SCR+'/'
exec(open(os.path.join(ROOT,'tools/b_tu/subst.py')).read().split("nm = open")[0])
srcf,lo,hi,run=sys.argv[1],int(sys.argv[2],16),int(sys.argv[3],16),sys.argv[4]
stubs=sys.argv[5:]
src=open(S+srcf).read()
syms={}
for l in open('config/symbols/main.txt'):
    m=re.match(r'(\S+)\s*=\s*0x([0-9A-Fa-f]+);.*type:func.*size:0x([0-9A-Fa-f]+)',l)
    if m: syms[m.group(1)]=(int(m.group(2),16),int(m.group(3),16))
cks=split_chunks(src)
defined=set(n for n,t in cks if n)
words=open('disc/mh1/split/main.bin','rb').read()
import struct
out=[];raw=[]
def ansi(hdr):
    import re as _re
    if '\n' not in hdr: return hdr
    lines=hdr.split('\n'); first=lines[0]
    m=_re.match(r'(.*\()([^)]*)(\))',first)
    names=[x.strip() for x in m.group(2).split(',') if x.strip()]
    decl={}
    for l in lines[1:]:
        l=l.strip().rstrip(';')
        if not l: continue
        nm=_re.search(r'(\w+)$',l).group(1)
        decl[nm]=l
    ps=', '.join(decl.get(n,'int '+n) for n in names) if names else 'void'
    return m.group(1)+ps+m.group(3)
def stub(n,hdr):
    hdr=ansi(hdr)
    inc=n
    a,s=syms[n]; raw.append('main 0x%08X 0x%X %s'%(a,s,inc))
    return "/* original bytes: build/raw/%s.inc (config/c_rawfuncs.txt) */\n#ifdef __MWERKS__\nasm %s\n{\n#include \"%s.inc\"\n}\n#endif\n\n"%(inc,hdr,inc)
for n,t in cks:
    if n in stubs:
        m=re.match(r'((?:/\*.*?\*/\n)?)([^{]*?)\{',t,re.S)
        out.append(stub(n,m.group(2).strip()))
    else: out.append(t)
text=''.join(out)
# functions of the range not in the text at all: stub with unknown signature, ordered by address at the end (caller must place if needed)
missing=sorted((a,n,s) for n,(a,s) in ((n,(a,s)) for n,(a,s) in syms.items()) if lo<=a<hi and n not in defined)
for a,n,s in [(x[0],x[1],x[2]) for x in missing]:
    w=struct.unpack_from('<I',words,a-0x100000)[0]
    if s==4 and w==0: continue
    print('MISSING (not in source):',n,hex(a),s)
open('src/main/%s.c'%run,'w').write(text)
# last function
last=max((a+s) for n,(a,s) in syms.items() if lo<=a<hi and n in defined)
first=min(a for n,(a,s) in syms.items() if lo<=a<hi and n in defined)
L=open('config/c_files.txt').read().split('\n'); new=[]; gone=set()
for l in L:
    p=l.split()
    if len(p)>=4 and p[0]=='main' and lo<=int(p[1],16)<hi: gone.add(p[3]); continue
    new.append(l)
new=[ (' '.join([p for p in l.split()[:3]]+[run]) if (l.split()[:1]==['main:rodata'] and len(l.split())>=4 and l.split()[3] in gone) else l) for l in new]
while new and new[-1]=='': new.pop()
new.append('main 0x%08X 0x%08X %s'%(first,last,run))
open('config/c_files.txt','w').write('\n'.join(new)+'\n')
R=[l for l in open('config/c_rawfuncs.txt').read().split('\n') if l.strip()]
R=[l for l in R if not (l.split()[0]=='main' and lo<=int(l.split()[1],16)<hi)]
R+=raw
open('config/c_rawfuncs.txt','w').write('\n'.join(R)+'\n')
for g in gone:
    p='src/main/%s.c'%g
    if os.path.exists(p) and g!=run: subprocess.run(['git','rm','-q','-f',p])
print('run',hex(first),hex(last),'removed',len(gone),'stubs',len(raw))
