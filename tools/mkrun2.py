#!/usr/bin/env python3
"""mkrun2.py NM.c OUT.c "header comment" func1 func2 ...: like mkrun.py, but keeps ALL
top-level text that is not a function definition (declarations scattered between the
functions) and only the named function bodies."""
import re,sys
nm,out,hdr=sys.argv[1:4]; funcs=sys.argv[4:]
s=open(nm).read()
pat=re.compile(r'^(?:[A-Za-z_][\w \*]*?\b)(\w+)\([^;{]*\)\s*\{\n',re.M)
chunks=[]  # (name or None, text)
pos=0
for m in pat.finditer(s):
    name=m.group(1)
    if m.start()<pos: continue
    end=s.index('\n}\n',m.end())+3
    start=m.start()
    pre=s[pos:start]
    t=pre.rstrip('\n')
    cs=start
    if t.endswith('*/'):
        c=t.rfind('/*')
        if c>=0 and '\n\n' not in t[c:]:
            cs=pos+c
            pre=s[pos:cs]
    chunks.append((None,pre))
    chunks.append((name,s[cs:end]))
    pos=end
chunks.append((None,s[pos:]))
body=''
fd={n:t for n,t in chunks if n}
for n,t in chunks:
    if n is None: body+=t
body=re.sub(r'^/\*.*?\*/\n','',body,count=1,flags=re.S)
body+='\n'+'\n'.join(fd[f] for f in funcs)
open(out,'w').write('/* '+hdr+' */\n'+body)
