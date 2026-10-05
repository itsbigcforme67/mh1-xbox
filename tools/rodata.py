import re, glob, sys
sys.path.insert(0,'/tmp/w')
import os
base=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))+"/"
# tables: name -> (addr, nwords)
tabs={}
for f in glob.glob(base+'asm/game/data/data/*.s'):
    t=open(f).read()
    for m in re.finditer(r'dlabel (lit_\w+)\n(.*?)enddlabel \1', t, re.S):
        ws=re.findall(r'/\* \w+ ([0-9A-F]{8}) \w+ \*/\s+\.word', m.group(2))
        if ws: tabs[m.group(1)]=(int(ws[0],16), len(ws))
asm=open(base+'asm/game/text/'+sys.argv[2]+'.s').read()
funcs={}
for m in re.finditer(r'^glabel (\S+)\n(.*?)^endlabel \1\n', asm, re.M|re.S):
    funcs[m.group(1)]=re.findall(r'%hi\((lit_\w+)\)', m.group(2))
import subprocess
runs=eval(sys.argv[1])  # list of lists of function names; argv[2] = asm file name (without .s) holding the functions (before registering)
for k,run in enumerate(runs):
    ts=[]
    for n in run:
        for lt in funcs.get(n,[]):
            if lt not in ts: ts.append(lt)
    if not ts: print(k,'no rodata'); continue
    a=[tabs[t] for t in ts]
    lo=min(x[0] for x in a); hi=max(x[0]+x[1]*4 for x in a)
    tot=sum(x[1]*4 for x in a)
    print(k, [ (t,hex(tabs[t][0]),tabs[t][1]) for t in ts], hex(lo), hex(hi), 'contiguous' if hi-lo==tot else 'GAP %d vs %d'%(hi-lo,tot))
