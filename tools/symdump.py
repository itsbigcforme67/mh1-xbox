"""symdump.py LISTING.s: tiny symbolic executor for a stripped function listing (address comments removed with
sed). Prints calls with the argument registers resolved (float constants decoded), stores, branches as
conditions. Easier to read than m2c for functions that pass floats in f12-f14 or have jump tables."""
import re,sys,struct
src=open(sys.argv[1]).read().split('\n')
def f2s(bits):
    v=struct.unpack('<f',struct.pack('<I',bits&0xffffffff))[0]
    if v==int(v) and abs(v)<1e9: return '%.1ff'%v
    return repr(v)+'f'
# parse lines
ins=[]
for l in src:
    l=l.rstrip()
    if not l.strip(): continue
    if l.startswith('glabel') or l.startswith('endlabel'): continue
    m=re.match(r'^\s*(\.L\w+):',l)
    if m: ins.append(('label',m.group(1),'')); continue
    m=re.match(r'^\s*(?:\.word\s+.*)$',l)
    if m: continue
    m=re.match(r'^\s*(\w[\w.]*)\s*(.*)$',l)
    if not m: continue
    op=m.group(1); args=m.group(2).split('#')[0].strip()
    ins.append((op,args,l))
CALLEE=set('$%d'%i for i in (16,17,18,19,20,21,22,23,30))
regs={}   # value strings
fregs={}
def R(r):
    r=r.strip()
    if r in ('$0','$zero'): return '0'
    return regs.get(r, ('s'+r[1:]) if r in CALLEE else r.replace('$','r'))
def F(r):
    return fregs.get(r.strip(),r.strip().replace('$','F'))
def memexpr(o):
    m=re.match(r'(.+)\((\$\d+)\)$',o)
    off=m.group(1); base=m.group(2)
    m2=re.match(r'%lo\((.*)\)$',off)
    if m2: return m2.group(1)
    m3=re.match(r'%gp_rel\((.*)\)$',off)
    if m3: return m3.group(1)
    if base=='$29': return 'sp[%s]'%off
    return '(%s+%s)'%(R(base),off)
out=[]
pending=[]
def emit(s): out.append(s)
def is_branch(op): return op in ('b','beq','bne','beqz','bnez','blez','bgtz','bltz','bgez','bc1t','bc1f','jal','j','jr','beql','bnel')
def parse_mem(a):
    m=re.match(r'(\$\w+),\s*(.*)',a); return m.group(1),m.group(2)
def execute(op,a,line):
    p=[x.strip() for x in a.split(',')] if a else []
    def setr(d,v):
        if d in CALLEE:
            emit('  s%s = %s;'%(d[1:],v)); regs[d]='s%s'%d[1:]
        else: regs[d]=v
    if op=='lui':
        m=re.match(r'(\$\d+),\s*\((0x[0-9A-Fa-f]+)\s*>>\s*16\)',a)
        if m: setr(m.group(1),'0x%X'%(int(m.group(2),16)&0xffff0000)); return
        m=re.match(r'(\$\d+),\s*%hi\((.*)\)',a)
        if m: setr(m.group(1),'&'+m.group(2)); return
        m=re.match(r'(\$\d+),\s*(\w+)',a); setr(m.group(1),'0x%X'%(int(m.group(2),0)<<16)); return
    if op=='ori':
        m=re.match(r'(\$\d+),\s*(\$\d+),\s*\((0x[0-9A-Fa-f]+)\s*&\s*0xFFFF\)',a)
        if m:
            base=regs.get(m.group(2),'0')
            try: v=int(base,16)|int(m.group(3),16)&0xffff
            except: v=None
            if v is not None: setr(m.group(1),'0x%X'%v); return
        m=re.match(r'(\$\d+),\s*(\$\d+),\s*(0x[0-9A-Fa-f]+)',a)
        if m:
            b=R(m.group(2))
            try: setr(m.group(1),'0x%X'%(int(b,16)|int(m.group(3),16))); return
            except: setr(m.group(1),'(%s|%s)'%(b,m.group(3))); return
    if op in ('addiu','daddiu'):
        m=re.match(r'(\$\d+),\s*(\$\d+),\s*(.*)',a)
        d,s,i=m.groups()
        if '%lo(' in i:
            sym=re.match(r'%lo\((.*)\)',i).group(1); setr(d,'&'+sym); return
        i=i.strip()
        if s=='$0': setr(d,str(int(i,0))); return
        if s=='$29': setr(d,'&sp[%s]'%i); return
        setr(d,'(%s+%s)'%(R(s),i)); return
    if op in ('daddu','addu'):
        d,s,t=[x.strip() for x in p]
        if t=='$0': setr(d,R(s)); return
        if s=='$0': setr(d,R(t)); return
        setr(d,'(%s+%s)'%(R(s),R(t))); return
    if op in ('subu','dsubu'):
        d,s,t=p; setr(d,'(%s-%s)'%(R(s),R(t))); return
    if op in ('andi',):
        d,s,i=p; setr(d,'(%s&%s)'%(R(s),i)); return
    if op in ('and',):
        d,s,t=p; setr(d,'(%s&%s)'%(R(s),R(t))); return
    if op in ('or',):
        d,s,t=p; setr(d,'(%s|%s)'%(R(s),R(t))); return
    if op in ('sll','dsll32'):
        d,s,i=p; setr(d,'(%s<<%s)'%(R(s),i)); return
    if op in ('srl','sra','dsra32'):
        d,s,i=p; setr(d,'(%s>>%s)'%(R(s),i)); return
    if op in ('lw','lh','lhu','lb','lbu','ld'):
        d,mm=parse_mem(a); t={'lw':'s32','lh':'s16','lhu':'u16','lb':'s8','lbu':'u8','ld':'s64'}[op]
        setr(d,'*(%s*)%s'%(t,memexpr(mm))); return
    if op in ('sw','sh','sb','sd'):
        d,mm=parse_mem(a); t={'sw':'s32','sh':'s16','sb':'s8','sd':'s64'}[op]
        emit('  *(%s*)%s = %s;'%(t,memexpr(mm),R(d))); return
    if op=='lwc1':
        d,mm=parse_mem(a)
        fregs[d]='*(f32*)%s'%memexpr(mm); return
    if op=='swc1':
        d,mm=parse_mem(a)
        emit('  *(f32*)%s = %s;'%(memexpr(mm),F(d))); return
    if op=='mtc1':
        d,s=p
        v=R(d)
        try:
            fregs[s]=f2s(int(v,16)) if v.startswith('0x') else ('0.0f' if v=='0' else '(f32)bits(%s)'%v)
        except: fregs[s]='(f32)bits(%s)'%v
        return
    if op=='mfc1':
        d,s=p; setr(d,'bits(%s)'%F(s)); return
    if op=='mov.s':
        d,s=p; fregs[d]=F(s); return
    if op in ('add.s','sub.s','mul.s','div.s'):
        d,s,t=p; o={'add.s':'+','sub.s':'-','mul.s':'*','div.s':'/'}[op]
        fregs[d]='(%s %s %s)'%(F(s),o,F(t)); return
    if op=='neg.s':
        d,s=p; fregs[d]='-%s'%F(s); return
    if op=='cvt.s.w':
        d,s=p; fregs[d]='(f32)%s'%F(s); return
    if op=='nop' or op=='.word': return
    if op=='slt' or op=='sltu' or op=='slti' or op=='sltiu':
        d,s,t=p; setr(d,'(%s <%s %s)'%(R(s),'u' if 'u' in op else '',R(t) if op in('slt','sltu') else t)); return
    if op in ('div',):
        emit('  /* div %s */'%a); return
    if op=='mfhi':
        setr(p[0],'HI'); return
    if op in ('movn','movz'):
        emit('  /* %s %s */'%(op,a)); return
    emit('  /* ?? %s %s */'%(op,a))
def condstr(op,a):
    p=[x.strip() for x in a.split(',')]
    if op in ('beq','bne'):
        o='==' if op=='beq' else '!='
        return '%s %s %s'%(R(p[0]),o,R(p[1])),p[2]
    if op in ('beqz','bnez'):
        o='==' if op=='beqz' else '!='
        return '%s %s 0'%(R(p[0]),o),p[1]
    if op in ('blez','bgtz','bltz','bgez'):
        o={'blez':'<=','bgtz':'>','bltz':'<','bgez':'>='}[op]
        return '%s %s 0'%(R(p[0]),o),p[1]
    if op in ('bc1t','bc1f'):
        return 'FPCOND%s'%('' if op=='bc1t' else ' (inverted)'),p[0]
i=0
lastcmp=''
while i<len(ins):
    op,a,line=ins[i]
    if op=='label':
        emit(a+':'); 
        for r in list(regs):
            regs.pop(r,None)
        i+=1; continue
    if op.startswith('c.'):
        p=[x.strip() for x in a.split(',')]
        lastcmp='%s %s %s'%(F(p[0]),{'c.lt.s':'<','c.le.s':'<=','c.eq.s':'=='}[op],F(p[1]))
        i+=1; continue
    if is_branch(op):
        # delay slot
        d=ins[i+1] if i+1<len(ins) else None
        if op=='jal':
            if d and d[0] not in ('label',): execute(d[0],d[1],d[2])
            args=[]
            fn=a
            argl=[]
            for r in ('$4','$5','$6','$7','$8','$9'):
                if r in regs: argl.append('%s=%s'%(r,regs[r]))
            fl=[]
            for r in ('$f12','$f13','$f14'):
                if r in fregs: fl.append('%s=%s'%(r,fregs[r]))
            emit('  %s(%s ; %s);'%(fn,', '.join(argl),', '.join(fl)))
            # clear volatile
            for r in ['$2','$3','$4','$5','$6','$7','$8','$9','$10','$11','$12','$13','$14','$15','$24','$25']: regs.pop(r,None)
            for r in list(fregs):
                if r in ('$f0','$f1','$f2','$f3','$f4','$f12','$f13','$f14','$f5','$f6','$f7','$f8'): fregs.pop(r,None)
            regs['$2']='RET'; fregs['$f0']='RETF'
            i+=2; continue
        if op in ('b','j'):
            if d and d[0]!='label': execute(d[0],d[1],d[2])
            emit('  goto %s;'%a); i+=2; continue
        if op=='jr':
            emit('  jr %s'%a); i+=2; continue
        cs,target=condstr(op,a)
        if 'FPCOND' in cs: cs=cs.replace('FPCOND',lastcmp)
        if d and d[0]!='label': execute(d[0],d[1],d[2])
        emit('  if (%s) goto %s;'%(cs,target))
        i+=2; continue
    execute(op,a,line)
    i+=1
print('\n'.join(out))
