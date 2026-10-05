#!/usr/bin/env python3
"""mkruns.py NM.c OUTDIR PREFIX FIRSTNUM "range comment": split the fully matching, address-contiguous
runs of NM.c out into OUTDIR/PREFIXNN.c (via mkrun2.py) and print the c_files.txt lines for them.
Only prints/writes; edit config/c_files.txt yourself (jump tables need main:rodata lines)."""
import re,subprocess,sys
nm,outdir,prefix,first,cmt=sys.argv[1:6]
first=int(first)
out=subprocess.run(['python3','tools/check.py',nm],capture_output=True,text=True).stdout
rows=[]
for l in out.split('\n'):
    m=re.match(r'^(OK|--)\s+(\S+)\s+main\s+0x([0-9A-F]+)\s+(\d+) bytes',l)
    if m: rows.append((m.group(2),int(m.group(3),16),int(m.group(4)),m.group(1)=='OK'))
rows.sort(key=lambda r:r[1])
runs=[];cur=[]
for r in rows:
    if r[3] and cur and 0<=r[1]-(cur[-1][1]+cur[-1][2])<16: cur.append(r)
    elif r[3]: 
        if cur: runs.append(cur)
        cur=[r]
    else:
        if cur: runs.append(cur)
        cur=[]
if cur: runs.append(cur)
n=first
for run in runs:
    name=f'{prefix}{n:02d}'
    path=f'{outdir}/{name}.c'
    s,e=run[0][1],run[-1][1]+run[-1][2]
    hdr=f'{name} - {cmt} 0x{s:08X}-0x{e:08X}: '+', '.join(r[0] for r in run)+'. Whole file in '+nm.split('/')[-1]+'.'
    subprocess.run(['python3','tools/mkrun2.py',nm,path,hdr]+[r[0] for r in run],check=True)
    print(f'main 0x{s:08X} 0x{e:08X} {outdir.replace("src/main/","")}/{name}   # {len(run)} funcs: {run[0][0]}..{run[-1][0]}')
    n+=1
