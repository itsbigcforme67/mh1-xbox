#!/usr/bin/env python3
"""d2c2.py DRAFT.c OUT.c NN "range/comment" : m2c draft -> near-compilable C with EMNNW struct."""
import re,sys
src=open(sys.argv[1]).read(); out=sys.argv[2]; NN=sys.argv[3]; hdr=sys.argv[4]
ctxlines=open('/tmp/claude-1000/w/ctx.c').read().split('\n')[1095:]
lines=[l for l in src.split('\n') if not l.startswith('/* Warning')]
body=[]
for l in lines:
    if re.match(r'^[\w ]*[\w\*]+ \**\w+\(.*\);\s*/\* extern \*/$',l): continue
    body.append(l)
text='\n'.join(body)
text=re.sub(r'\b(0x[0-9A-Fa-f]+|\d+)U\b',r'\1',text)
text=re.sub(r'\barg0\b','em',text)
text=text.replace('M2C_UNK','int')
text=re.sub(r'\*\((?:u8|void|s8) \*\)0x3F3404','game_w.stage',text)
text=re.sub(r'\*\((?:s16|void|u16) \*\)0x3C7448','quest_w.no',text)
text=re.sub(r'\*\((?:u8|void|s8) \*\)0x3F341E','game_w.x2E',text)
def sub_ex(mm):
    off=int(mm.group(2),0)
    if 0x444<=off<0x50C:
        rec(off-0x444,mm.group(1)); return 'w->x%02X'%(off-0x444)
    return mm.group(0)
text=re.sub(r'\(\(s64\) \(\(s64\) ([\w\.\->\[\]]+) << 0x(30|38)\) >> 0x\2\)',lambda m:'(%s)%s'%('s16' if m.group(2)=='30' else 's8',m.group(1)),text)
text=re.sub(r'\(\(s64\) \(([^()]*(?:\([^()]*\))?[^()]*) << 0x(30|38)\) >> 0x\2\)',lambda m:'(%s)(%s)'%('s16' if m.group(2)=='30' else 's8',m.group(1)),text)
text=re.sub(r'\*\((?:void|u8) \*\)0x3F340E','game_w.x1E',text)
text=text.replace('NULL','0')
text=re.sub(r'\(EMW \*\)(0x[0-9A-Fa-f]+|\d+)',r'\1',text)
text=re.sub(r'\(s32\) \(s16\)',r'(s16)',text)
text=text.replace('M2C_BITWISE(s32, ','(s32)(')
# s16 conversion idioms
text=re.sub(r'\(s32\) \(\(s64\) \(\(([^;]*?)\) << 0x30\) >> 0x30\)',r'(s16)(\1)',text)
text=re.sub(r'\(\(s64\) \(\(([^;]*?)\) << 0x30\) >> 0x30\)',r'(s16)(\1)',text)
text=re.sub(r'\(s64\) \(M2C_BITWISE\(s64, \((.*?)\)\) << 0x30\) >> 0x30',r'(s16)(\1)',text)
# per-function work var
W='EM%sW'%NN
offs={}
def tsize(t):
    t=t.replace('*','').strip()
    return {'s8':1,'u8':1,'s16':2,'u16':2,'s32':4,'u32':4,'f32':4,'int':4}.get(t,4)
def rec(off,t):
    t=t.replace('*','').strip()
    if off not in offs: offs[off]=t
    elif offs[off]!=t and tsize(t)>=tsize(offs[off]) and False: pass
funcs=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',text)
newf=[]
for f in funcs:
    m=re.search(r'\n\s*(?:void \*|u8 \*|s8 \*)?(temp_\w+|var_\w+) = em \+ 0x444;',f)
    f2=re.sub(r'M2C_FIELD\(em, (\w+ \*), (0x[0-9A-Fa-f]+)\)',sub_ex,f)
    changed=(f2!=f); f=f2
    ex_idx=re.findall(r'em->ex\[(0x[0-9A-Fa-f]+|\d+)\]',f)
    v=m.group(1) if m else None
    hm=re.match(r'([^\n]*)\(EMW \*em, u8 \*arg1\)',f)
    if hm:
        f=f.replace('(EMW *em, u8 *arg1)','(EMW *em, %s *w)'%W,1)
        def sub3(mm):
            off=int(mm.group(2),0); rec(off,mm.group(1)); return 'w->x%02X'%off
        f=re.sub(r'M2C_FIELD\(arg1, (\w+ \*), (0x[0-9A-Fa-f]+|\d+)\)',sub3,f)
        f=re.sub(r'\barg1 \+ (0x[0-9A-Fa-f]+|\d+)',lambda mm:'(s32 *)((u8 *)w + %s)'%mm.group(1),f)
        f=re.sub(r'\barg1\b','w',f)
        f=re.sub(r'\btemp_s0 = w;','',f)
        if False: pass

    if v:
        f=re.sub(r'\n\s*(?:void \*|u8 \*|s8 \*)?%s = em \+ 0x444;'%v,'\n',f,count=1)
        f=re.sub(r'\n\s*(?:void|u8|s8) \*%s;'%v,'',f,count=1)
        def sub(mm):
            off=int(mm.group(2),0); rec(off,mm.group(1)); return 'w->x%02X'%off
        f=re.sub(r'M2C_FIELD\(%s, (\w+ \*), (0x[0-9A-Fa-f]+|\d+)\)'%v,sub,f)
        f=re.sub(r'\b%s\b'%v,'((u8 *)w)',f)
    if (ex_idx or v or changed) and not hm:
        def sub2(mm):
            off=int(mm.group(1),0); rec(off,'u8'); return 'w->x%02X'%off
        f=re.sub(r'em->ex\[(0x[0-9A-Fa-f]+|\d+)\]',sub2,f)
        # declare w after the first '{' line
        f=re.sub(r'\{\n',r'{\n    %s *w = (%s *)em->ex;\n'%(W,W),f,count=1)
    newf.append(f)
text=''.join(newf)
text=re.sub(r'\n{3,}','\n\n',text)
defs=re.findall(r'^(\w[\w \*]*?\b\w+\([^;{]*\))\s*\{$',text,re.M)
locals_=set(re.search(r'(\w+)\(',d).group(1) for d in defs)
used=set(re.findall(r'\b(\w+)\s*\(',text))
need=[];seen=set()
for l in ctxlines:
    m=re.match(r'^[\w \*]+?\b(\w+)\s*\(',l)
    if m and l.rstrip().endswith(';') and m.group(1) in used and m.group(1) not in locals_ and m.group(1) not in seen:
        need.append(l); seen.add(m.group(1))
    else:
        m=re.match(r'^extern\s+[\w \*]+?\b(\w+)(\[[^\]]*\])*;',l)
        if m and re.search(r'\b%s\b'%m.group(1),text) and m.group(1) not in seen:
            need.append(l); seen.add(m.group(1))
protos='\n'.join(d+';' for d in defs)
# struct
st='typedef struct %s {\n'%W
pos=0
for off in sorted(offs):
    t=offs[off]; sz=tsize(t)
    if off<pos: continue
    if off>pos: st+='    u8 _pad%02X[0x%X];\n'%(pos,off-pos)
    st+='    %s x%02X;\n'%(t,off); pos=off+sz
st+='} %s;\n'%W
head='''/* %s */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))

'''%hdr
open(out,'w').write(head+st+'\n'+'\n'.join(need)+'\n\n'+protos+'\n\n'+text+'\n')
print(len(need),'externs',len(defs),'functions',len(offs),'work fields')
