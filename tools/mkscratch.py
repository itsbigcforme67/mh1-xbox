#!/usr/bin/env python3
"""mkscratch.py NM.c OUT.c FUNC [FUNC...] : scratch TU = include line + K&R decls for every other function
in NM.c + the named functions (copied verbatim, any 'typedef'/'extern' lines before the first function are
kept). Lets you test a function as if its callees lived in other source files."""
import re,sys
nm,out=sys.argv[1:3]; funcs=sys.argv[3:]
s=open(nm).read()
pat=re.compile(r'^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\(([^;{]*)\)\s*\{\n',re.M)
chunks={}; order=[]
for m in pat.finditer(s):
    end=s.index('\n}\n',m.end())+3
    start=m.start()
    pre=s[:start].rstrip('\n')
    if pre.endswith('*/'):
        c=pre.rfind('/*')
        if c>=0 and '\n}\n' not in pre[c:]: start=c
    chunks[m.group(1)]=(start,end,m.group(0)); order.append(m.group(1))
first=min(v[0] for v in chunks.values())
pre=s[:first]
# typedefs/externs in between functions: keep any line starting with typedef/extern/#
extra=[]
for m in re.finditer(r'^(typedef [^\n]*|extern [^\n]*|#[^\n]*)$',s[first:],re.M):
    extra.append(m.group(0))
decls=[]
for n in order:
    if n in funcs: continue
    hdr=chunks[n][2]
    stat=hdr.startswith('static')
    ret=re.match(r'^(?:static )?([A-Za-z_][\w \*]*?)\b'+n+r'\(',hdr).group(1).strip()
    if stat: continue
    decls.append("%s %s();"%(ret,n))
import os
ov=[x.strip()+';' for x in os.environ.get('DECLS','').split(';') if x.strip()]
for o in ov:
    nm_=re.search(r'(\w+)\(',o).group(1)
    decls=[d for d in decls if not re.search(r'\b'+nm_+r'\(',d)]
decls+=ov
body='\n'.join(s[chunks[f][0]:chunks[f][1]] for f in funcs)
import subprocess
for _ in range(40):
    open(out,'w').write(pre+'\n'.join(extra)+'\n'+'\n'.join(decls)+'\n\n'+body)
    r=subprocess.run(['python3','tools/check.py',out],capture_output=True,text=True)
    m=re.search(r"identifier '(\w+)\(.*?redeclared",r.stdout+r.stderr)
    if not m: break
    decls=[d for d in decls if not re.search(r'\b'+m.group(1)+r'\(',d)]
