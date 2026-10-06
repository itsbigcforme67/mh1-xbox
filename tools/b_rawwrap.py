#!/usr/bin/env python3
"""b_rawwrap.py FILE FUNC ADDR SIZE: wrap FUNC's C definition in FILE as
#ifdef __MWERKS__ asm stub (original bytes) #else <C> #endif and add the c_rawfuncs.txt line."""
import re,sys
f,fn,addr,size=sys.argv[1:5]
s=open(f).read()
m=re.search(r'^(\w[\w \*]*?)\b%s\(([^;{]*)\)\s*\{\n'%fn,s,re.M)
en=s.index('\n}\n',m.end())+3
rt=m.group(1).strip(); par=m.group(2)
body=s[m.start():en]
stub='/* original bytes: build/raw/%s.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */\n#ifdef __MWERKS__\nasm %s %s(%s)\n{\n#include "%s.inc"\n}\n#else\n%s#endif\n'%(fn,rt,fn,par,fn,body)
s=s[:m.start()]+stub+s[en:]
open(f,'w').write(s)
open('config/c_rawfuncs.txt','a').write('lobby %s %s %s\n'%(addr,size,fn))
