#!/usr/bin/env python3
"""status.py FILE: run check.py -v; print per function: OK / NOISE (only static-call-name noise) / DIFF n.
Importable: exec the part before __main__ with f set to the file."""
import subprocess,re,sys,os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
f=sys.argv[1] if len(sys.argv) > 1 else "x"
out=subprocess.run(['python3','tools/check.py',f,'-v'],capture_output=True,text=True).stdout
cur=None; res={}
for l in out.split('\n'):
    m=re.match(r'(OK|--)\s+(\S+)\s+game\s+0x([0-9A-F]+)\s+(\d+) bytes(?:\s+\((\d+)/(\d+))?',l)
    if m:
        cur=m.group(2); res[cur]=dict(ok=m.group(1)=='OK',addr=int(m.group(3),16),size=int(m.group(4)),diff=0,real=0); continue
    if cur and '>>' in l:
        res[cur]['diff']+=1
        if '(calls ' not in l: res[cur]['real']+=1
import json
st={}
for n,v in res.items():
    st[n]='OK' if v['ok'] else ('NOISE' if v['real']==0 else 'DIFF %d'%v['real'])
if __name__=='__main__':
    for n,v in sorted(res.items(), key=lambda x:x[1]['addr']):
        if st[n]!='OK' and st[n]!='NOISE': print(hex(v['addr']),n,st[n])
    print(sum(1 for x in st.values() if x in('OK','NOISE')),'of',len(st),'true OK')
