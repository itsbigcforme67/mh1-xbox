import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import re,sys,os
os.chdir(ROOT)
exec(open('tools/mtu.py').read().split("funcs = {}")[0].split("pat = re.compile")[0].split("name, S, E")[0]) if False else None
pat = re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)(?:\n(?:[\w \*]+;\n)+)?\s*\{\n', re.M)
def split_chunks(s):
    chunks = []; pos = 0
    for m in pat.finditer(s):
        if m.start() < pos: continue
        end = (m.end() + 2) if s[m.end():m.end() + 2] == '}\n' else s.index('\n}\n', m.end()) + 3
        start = m.start()
        pre = s[pos:start]
        t = pre.rstrip('\n'); cs = start
        if t.endswith('*/'):
            c = t.rfind('/*')
            if c >= 0 and '\n\n' not in t[c:]:
                cs = pos + c; pre = s[pos:cs]
        chunks.append((None, pre)); chunks.append((m.group(1), s[cs:end])); pos = end
    chunks.append((None, s[pos:]))
    return chunks
nm = open('src/main/chat/chat_nm.c').read()
cks = split_chunks(nm)
runs = {}
declsets = {}
import glob
for l in open('config/c_files.txt'):
    p = l.split()
    if len(p)>=4 and p[0]=='main' and p[3].startswith('chat/'):
        a=int(p[1],16)
        if 0x2755D0<=a<0x27BF80:
            s=open('src/main/%s.c'%p[3]).read()
            s=re.sub(r'^/\*.*?\*/\n','',s,count=1,flags=re.S)
            for n,t in split_chunks(s):
                if n: runs[n]=t
print(len(runs),'matched fn bodies')
out=[]
rep=[]
for n,t in cks:
    if n and n in runs:
        out.append(runs[n]); rep.append(n)
    else: out.append(t)
open(sys.argv[1],'w').write(''.join(out))
print('replaced',len(rep))
miss=[n for n in runs if n not in rep]; print('not in nm:',miss)
