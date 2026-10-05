#!/usr/bin/env python3
"""align.py FILE FUNC: compare one function with the original, ignoring
relocation targets and branch-address shifts, so only real differences show.
Runs tools/check.py -v. Standard library only."""
import sys,subprocess,re,difflib
f,func=sys.argv[1],sys.argv[2]
import os
out=subprocess.run(['python3','tools/check.py',f,'-v']+os.environ.get('CHK_ARGS','').split(),capture_output=True,text=True).stdout
lines=out.split('\n'); i=[k for k,l in enumerate(lines) if (' %s '%func) in l and l[:2] in('--','OK')][0]
L=[];R=[]
for l in lines[i+1:]:
    if re.match(r'^(OK|--)  ',l): break
    if '|' not in l: continue
    a,b=l.split('|',1)
    m=re.match(r'\s*(>>)?\s*([0-9A-F]{8})\s+(.*)',a)
    if m and m.group(3).strip(): L.append((m.group(2),m.group(3).strip()))
    if b.strip(): R.append(b.strip())
def key(s):
    s=re.sub(r'\s+',' ',s.replace('(reloc)','')).strip()
    op=s.split(' ')[0]
    if op in('jal','j') : return op
    if op=='lui' or '(at)' in s or '(gp)' in s or ', gp,' in s: return re.sub(r'-?0x[0-9A-Fa-f]+|-?\d+(?=\(|$)','N',s)
    if op.startswith('b'): return re.sub(r'0x[0-9A-F]{8}','ADDR',s)
    if op=='addiu' and re.search(r', (-?\d+)$',s) and abs(int(re.search(r', (-?\d+)$',s).group(1)))>=4096: return re.sub(r'-?\d+$','N',s)
    return s
a=[key(x[1]) for x in L]; b=[key(x) for x in R]
# right side reloc addiu with 0
b=[re.sub(r'(addiu \w+, \w+), 0$',r'\1, N',x) for x in b]
sm=difflib.SequenceMatcher(None,a,b,autojunk=False)
for op,i1,i2,j1,j2 in sm.get_opcodes():
    if op=='equal': continue
    print(op, L[i1][0] if i1<len(L) else 'end')
    for k in range(i1,i2): print('   -',L[k][1])
    for k in range(j1,j2): print('   +',R[k])
