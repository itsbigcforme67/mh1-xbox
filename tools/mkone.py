#!/usr/bin/env python3
"""mkone.py NM.c OUT.c FUNC[,FUNC2..] HEADER : write OUT.c holding NM.c's top part (typedefs, externs, prototypes) plus only the
named functions (static dropped). Column-0 functions are cut by brace matching; whatever is left at file level stays."""
import re,sys
nm,out,names,head=sys.argv[1:5]
names=names.split(',')
lines=open(nm).read().split('\n')
res=[];i=0;keep=[]
hdr=re.compile(r'^(?:static )?[A-Za-z_][\w \*]*?\b(\w+)\([^;]*$')
while i<len(lines):
    l=lines[i]
    m=hdr.match(l)
    if m and not l.startswith(('extern','typedef','#',' ','}')) and '=' not in l.split('(')[0]:
        # function (K&R or ANSI): find opening brace line then closing '}' at col 0
        j=i
        while not lines[j].endswith('{') and lines[j]!='{': j+=1
        while lines[j]!='}': j+=1
        if m.group(1) in names:
            keep.append('\n'.join(lines[i:j+1]).replace('static ','',1) if lines[i].startswith('static ') else '\n'.join(lines[i:j+1]))
        i=j+1; continue
    res.append(l); i+=1
top='\n'.join(res)
top=re.sub(r'\A/\*.*?\*/\n','',top,count=1,flags=re.S)
top=re.sub(r'\n{3,}','\n\n',top)
open(out,'w').write('/* %s */\n'%head+top.rstrip('\n')+'\n\n'+'\n\n'.join(keep)+'\n')
