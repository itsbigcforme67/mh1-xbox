#!/usr/bin/env python3
"""reg.py SRC.c FUNC NEWNAME [comment] : if FUNC matches OK in SRC.c, install it as src/lobby/b/NEWNAME.c, register in c_files.txt, delete nm draft src/lobby/b/nm/FUNC.c (if exists)"""
import sys,subprocess,re,os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
src,fn,new=sys.argv[1:4]
cm=sys.argv[4] if len(sys.argv)>4 else fn
out=subprocess.run(['python3','tools/check.py',src,'--module','lobby'],capture_output=True,text=True).stdout
m=[l for l in out.split('\n') if l.startswith('OK  %s '%fn)]
if not m: sys.exit('NOT OK: '+fn+'\n'+out[-500:])
parts=m[0].split(); addr=int(parts[3],16); size=int(parts[4])
cf=open('config/c_files.txt').read()
line='lobby 0x%08X 0x%08X b/%s\n'%(addr,addr+size,new)
# overlap check
for l in cf.split('\n'):
    q=l.split()
    if len(q)==4 and q[0]=='lobby' and not ':' in q[0]:
        a,e=int(q[1],16),int(q[2],16)
        if a<addr+size and addr<e: sys.exit('OVERLAP with '+l)
dst='src/lobby/b/%s.c'%new
assert not os.path.exists(dst), dst+' exists'
body=open(src).read()
hdr='/* %s - agent C 0x%08X-0x%08X: %s. */\n'%(new,addr,addr+size,cm)
open(dst,'w').write(hdr+body)
open('config/c_files.txt','a').write(line)
nm='src/lobby/b/nm/%s.c'%fn
if os.path.exists(nm): os.remove(nm)
print('registered',line.strip())
