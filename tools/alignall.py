#!/usr/bin/env python3
"""alignall.py FILE [-v] : like align.py for every function of FILE with one check.py run.
Prints "OK/--  name  real-diff-count" (relocation / branch-address noise ignored); -v also
prints each differing hunk."""
import sys,subprocess,re,difflib
f=sys.argv[1]; verbose='-v' in sys.argv
out=subprocess.run(['python3','tools/check.py',f,'-v'],capture_output=True,text=True).stdout
lines=out.split('\n')
heads=[k for k,l in enumerate(lines) if re.match(r'^(OK|--)  ',l)]
def key(s):
    s=re.sub(r'\s+',' ',s.replace('(reloc)','')).strip()
    op=s.split(' ')[0]
    if op in('jal','j'): return op
    if op=='lui' or '(at)' in s or '(gp)' in s or ', gp,' in s: return re.sub(r'-?0x[0-9A-Fa-f]+|-?\d+(?=\(|$)','N',s)
    if op.startswith('b'): return re.sub(r'0x[0-9A-F]{8}','ADDR',s)
    if op=='addiu' and re.search(r', (-?\d+)$',s) and abs(int(re.search(r', (-?\d+)$',s).group(1)))>=4096: return re.sub(r'-?\d+$','N',s)
    return s
for n,i in enumerate(heads):
    end=heads[n+1] if n+1<len(heads) else len(lines)
    name=lines[i].split()[1]
    if lines[i].startswith('OK'): print('OK  %-34s 0'%name); continue
    L=[];R=[]
    for l in lines[i+1:end]:
        if '|' not in l: continue
        a,b=l.split('|',1)
        m=re.match(r'\s*(>>)?\s*([0-9A-F]{8})\s+(.*)',a)
        if m and m.group(3).strip(): L.append((m.group(2),m.group(3).strip()))
        if b.strip(): R.append(b.strip())
    a=[key(x[1]) for x in L]; b=[key(x) for x in R]
    b=[re.sub(r'(addiu \w+, \w+), 0$',r'\1, N',x) for x in b]
    sm=difflib.SequenceMatcher(None,a,b,autojunk=False)
    cnt=0; hunks=[]
    for op,i1,i2,j1,j2 in sm.get_opcodes():
        if op=='equal':
            # lui immediates are masked by key() (they may be relocations); compare them exactly when mine has no reloc
            for k in range(i2-i1):
                lt=L[i1+k][1]; rt=R[j1+k]
                if lt.startswith('lui ') and '(reloc)' not in rt and re.sub(r'\s+',' ',lt)!=re.sub(r'\s+',' ',rt):
                    cnt+=1; hunks.append(('replace',L[i1+k][0],[lt],[rt]))
            continue
        if op=='replace' and i2-i1==j2-j1 and all('(reloc)' in R[j1+k] and L[i1+k][1].split()[:1]==R[j1+k].split()[:1] and re.sub(r',[^,]*$','',L[i1+k][1])==re.sub(r',[^,]*$','',R[j1+k].replace('(reloc)','').rstrip()) for k in range(i2-i1)):
            continue
        cnt+=max(i2-i1,j2-j1)
        hunks.append((op,L[i1][0] if i1<len(L) else 'end',[L[k][1] for k in range(i1,i2)],R[j1:j2]))
    print('--  %-34s %d   (%d instr)'%(name,cnt,len(L)))
    if verbose:
        for op,ad,x,y in hunks:
            print('    %s %s'%(op,ad))
            for t in x: print('       -',t)
            for t in y: print('       +',t)
