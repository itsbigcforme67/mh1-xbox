#!/usr/bin/env python3
"""asm2mwcc.py NAME... : print an MWCC inline-asm C function for each named function found in asm/main/text/*.s (run from the repo root after
tools/rebuild.sh). Meant for VU0 macro-mode routines (lqc2/vmul.../vsqrt...), which the original wrote as inline assembly: registers lose their $,
lui/addiu %hi/%lo pairs become `la reg, sym`, .L labels become Lxxxx: labels, .word-disassembled VU0/FPU opcodes are taken from the comment.
The output compiles with mwccps2 (check.py tells OK / not). Add the extern declarations of the symbols used by `la`/`jal` yourself."""
import re,glob,sys
REG=['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7','s0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','fp','ra']
def reg(m):
    r=m.group(1)
    if r.isdigit(): return REG[int(r)]
    return r
def conv_ops(o):
    o=re.sub(r'\$(\w+)',reg,o)
    return o
def func_text(name):
    for p in glob.glob('asm/main/text/*.s'):
        t=open(p).read()
        m=re.search(r'^glabel %s\n(.*?)^endlabel %s\n'%(name,name),t,re.M|re.S)
        if m: return m.group(1)
def convert(name,rettype,args):
    body=func_text(name)
    out=[]
    lines=body.split('\n')
    ins=[]
    for l in lines:
        s=l.strip()
        if not s: continue
        m=re.match(r'\.L(\w+):',s)
        if m: ins.append(('label',m.group(1))); continue
        m=re.match(r'alabel (\w+)',s)
        if m: ins.append(('label',m.group(1))); continue
        m=re.match(r'/\*.*?\*/\s*(.*)$',s)
        if not m: continue
        t=re.sub(r'\s*/\*.*?\*/\s*$','',m.group(1))
        c=re.search(r'#\s*([a-z0-9_.]+)\s+(.*?)\s*#',t) if t.startswith('.word') else None
        if c: t=c.group(1)+' '+c.group(2)
        ins.append(('ins',t))
    i=0
    res=[]
    while i<len(ins):
        k,t=ins[i]
        if k=='label': res.append(('L%s:'%t) if not t.startswith('func_') else ('Lfunc_%s:'%t[5:])); i+=1; continue
        mn,_,ops=t.partition(' ')
        ops=ops.strip()
        if mn=='lui' and '%hi(' in ops and i+1<len(ins):
            m1=re.match(r'\$(\w+),\s*%hi\((.*)\)',ops)
            n=ins[i+1][1]
            m2=re.match(r'addiu\s+\$(\w+),\s*\$(\w+),\s*%lo\((.*)\)',n)
            if m1 and m2 and m2.group(3)==m1.group(2):
                res.append('la %s, %s'%(REG[int(m1.group(1))] if m1.group(1).isdigit() else m1.group(1),m1.group(2)));i+=2;continue
        ops=conv_ops(ops)
        ops=re.sub(r'0x00([0-9A-F]{6})\b',lambda m:'L'+m.group(1),ops) if re.match(r'(j|b)',mn) and mn!='jr' else ops
        ops=re.sub(r'\.L(\w+)',r'L\1',ops)
        ops=re.sub(r'\bfunc_([0-9A-F]{8})\b',r'Lfunc_\1',ops) if re.match(r'(j|b)',mn) and mn!='jr' else ops
        res.append((mn+' '+ops).strip()); i+=1
    return 'asm %s %s(%s)\n{\n'%(rettype,name,args)+'\n'.join(('    '+x) if not x.endswith(':') else x for x in res)+'\n}\n'
if __name__=='__main__':
    for n in sys.argv[1:]:
        print(convert(n,'void','void'))
