#!/usr/bin/env python3
"""cmpnm.py : for functions that exist both in an *_nm.c near-match and in a matched file, rank the pairs by
how different their C text is (identifiers/numbers kept, whitespace and casts dropped). Low ratio = look at it."""
import re,glob,difflib,sys,collections
def funcs(path):
    L=open(path,errors='replace').read().split('\n'); out={}; i=0
    pat=re.compile(r'^(?:static\s+)?[A-Za-z_][\w \*]*?[ \*](\w+)\s*\(.*(?:\)\s*\{\s*|)$')
    while i<len(L):
        m=re.match(r'^(?:static |inline )*[A-Za-z_][\w \*]*?[ \*](\w+)\s*\(([^;]*)$',L[i])
        if m and ('{' in L[i] or (i+1<len(L) and L[i+1].startswith('{')) or not L[i].rstrip().endswith(';')):
            j=i
            while j<len(L) and L[j]!='}': j+=1
            body='\n'.join(L[i:j+1])
            if '{' in body: out[m.group(1)]=body
            i=j+1
        else: i+=1
    return out
def norm(b):
    b=re.sub(r'/\*.*?\*/','',b,flags=re.S); b=re.sub(r'//.*','',b)
    b=re.sub(r'\((?:u|s)(?:8|16|32|64)\)|\((?:int|void|long|char|short)(?: ?\*)?\)','',b)
    b=re.sub(r'\bfunc_[0-9A-F]{6}\b','FN',b)
    return re.findall(r'\w+|[^\s\w]',b)
nm={}
for f in glob.glob('src/main/**/*_nm.c',recursive=True):
    for k,v in funcs(f).items(): nm.setdefault(k,[]).append((f,v))
reg=set('src/main/%s.c'%l.split()[3] for l in open('config/c_files.txt') if l.split('#')[0].split() and l.split()[0]=='main')
res=[]
for f in glob.glob('src/main/**/*.c',recursive=True):
    if f.endswith('_nm.c') or '/tu/' in f or f not in reg: continue
    for k,v in funcs(f).items():
        for g,w in nm.get(k,[]):
            a,b=norm(v),norm(w)
            if len(a)<30: continue
            r=difflib.SequenceMatcher(None,a,b,autojunk=False).ratio()
            res.append((r,k,f,g,len(a),len(b)))
res.sort()
for r in res[:int(sys.argv[1]) if len(sys.argv)>1 else 60]: print('%.2f %s %s vs %s (%d/%d tokens)'%r)
