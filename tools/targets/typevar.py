#!/usr/bin/env python3
"""typevar.py FILE FUNC [--apply] : greedy search over the integer types of FUNC's local variables and parameters
(int/long/u8/s8/u16/s16/u32/s32/u64/s64): each single change that lowers check.py's difference count is kept.
Found menu_data_monster_sub (`long m`: the 64-bit register gives the original's extension). Prints the best source."""
import sys,re,subprocess,os
f,func=sys.argv[1:3]
TYPES=['int','long','u8','s8','u16','s16','u32','s32','u64','s64','unsigned int','unsigned long']
src=open(f).read()
pat=re.compile(r'^(?:static )?[A-Za-z_][\w \*]*?[ \*]'+re.escape(func)+r'\(.*\)\s*\{?\s*$',re.M)
m=pat.search(src)
a=m.start(); b=src.index('\n}\n',a)+3
body=src[a:b]
def score(text):
    open(f,'w').write(src[:a]+text+src[b:])
    out=subprocess.run(['python3','tools/check.py',f],capture_output=True,text=True).stdout
    mm=re.search(r'(OK|--)  '+re.escape(func)+r' .*?(?:\((\d+)/\d+ instructions differ\))?$',out,re.M)
    if not mm: return 9999
    return 0 if mm.group(1)=='OK' else int(mm.group(2) or 9997)
cur=body; best=score(cur); print('start',best)
INT=r'(?:int|long|u8|s8|u16|s16|u32|s32|u64|s64|unsigned int|unsigned long|unsigned char|signed char|short|unsigned short)'
decl=re.compile(r'^(\s*)(%s)(\s+)(\w+)(\s*(?:=[^;]*)?;)$'%INT,re.M)
improved=True
while improved and best>0:
    improved=False
    for mm in list(decl.finditer(cur)):
        for t in TYPES:
            if t==mm.group(2): continue
            cand=cur[:mm.start()]+mm.group(1)+t+mm.group(3)+mm.group(4)+mm.group(5)+cur[mm.end():]
            sc=score(cand)
            if sc<best:
                best=sc; cur=cand; improved=True; print('improved',mm.group(4),'->',t,best)
                break
        if improved: break
open(f,'w').write(src[:a]+cur+src[b:] if '--apply' in sys.argv else src)
print('final',best); print(cur)
