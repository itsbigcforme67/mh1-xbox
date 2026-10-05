#!/usr/bin/env python3
"""mkrun.py NM.c OUT.c "header comment" func1 func2 ...: copy the preamble of NM.c
(everything before its first function definition) and the named functions into OUT.c."""
import re,sys
nm,out,hdr=sys.argv[1:4]; funcs=sys.argv[4:]
s=open(nm).read()
# split into top-level function chunks
pat=re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)\s*\{\n',re.M)
chunks={}
first=None
for m in pat.finditer(s):
    name=m.group(1)
    end=s.index('\n}\n',m.end())+3
    # include preceding comment block
    start=m.start()
    pre=s[:start].rstrip('\n')
    if pre.endswith('*/'):
        c=pre.rfind('/*')
        start=c
    if first is None: first=start
    chunks[name]=s[start:end]
pre=s[:first]
pre=re.sub(r'^/\*.*?\*/\n','',pre,count=1,flags=re.S)
body='\n'.join(chunks[f] for f in funcs)
open(out,'w').write('/* '+hdr+' */\n'+pre+body)
