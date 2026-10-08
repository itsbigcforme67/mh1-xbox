#!/usr/bin/env python3
"""alignall.py NMOUT : for every unmatched main function the PC links (tools/targets/nm.py output) that lives in an
*_nm.c / matching C file of src/main, run align.py and print the number of real differing lines (small = nearly there)."""
import sys,subprocess,glob,os,re,collections
res=[]
byfile=collections.defaultdict(list)
for l in open(sys.argv[1]):
    p=l.split()
    if len(p)>=4 and p[1].startswith('0x'):
        for o in p[3:]:
            if o.startswith('rt_') or o=='trans_stage.o': continue
            c=glob.glob('src/**/%s.c'%o[:-2],recursive=True)
            if c: byfile[c[0]].append(p[0])
for f,fns in byfile.items():
    out=subprocess.run(['python3','tools/check.py',f],capture_output=True,text=True).stdout
    st={}
    for ln in out.splitlines():
        m=re.match(r'^(OK|--)  (\S+).*?(?:\((\d+)/(\d+) instructions differ\))?$',ln)
        if m: st[m.group(2)]=(m.group(1),m.group(3),m.group(4))
    for fn in fns:
        s=st.get(fn)
        if not s: continue
        res.append((int(s[1] or 0),int(s[2] or 0),fn,f))
res.sort()
for r in res: print(*r)
