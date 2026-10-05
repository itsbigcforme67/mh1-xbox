#!/usr/bin/env python3
"""pipe.py NM.c : generic clean-ups after d2c2 on an m2c draft (agent D pipeline)"""
import re,sys
sys.path.insert(0,'/tmp/claude-1000/w')
p=sys.argv[1]
s=open(p).read()
s=s.replace('M2C_BITWISE(s32, ','(s32)(')
s=re.sub(r'M2C_BITWISE\(s16, \((.*?)\)\)',r'(s16)(\1)',s)
s=re.sub(r'xang_calc_target\(em, ([^;]*?), 0\.0f, (?:F13_\?|MOVS\(\$f12\))\)',r'xang_calc_target(em, \1, 0.0f, 0.0f)',s)
s,nn=re.subn(r'( *)if \(!\(temp_f1 >= 2\.1474836e9f\)\) \{\n *(var_\w+) = temp_f1;\n *\} else \{\n *\2 = M2C_BITWISE\(f32, .*?\);\n *\}\n',r'\1\2 = (u32)temp_f1;\n',s)
s=re.sub(r'f32 (var_\w+);',lambda m:'u32 %s;'%m.group(1) if re.search(r'\b%s = \(u32\)temp_f1'%m.group(1),s) else m.group(0),s)
# mot_miration_ret vector
def fixmot(f):
    if 'mot_miration_ret(em, &sp30)' in f:
        f=f.replace('    f32 sp30;\n','    f32 sp30[3];\n').replace('&sp30','sp30').replace('sp38','sp30[2]')
    return f
parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s)
s=''.join(fixmot(x) for x in parts)
s=re.sub(r'\bEft19_set\(0x22, 0\)',r'Eft19_set(em, 0x22, 0)',s)
s=re.sub(r'(_fly_adjy2)\(\(EMW \*\) \w+\)',r'\1(em)',s)
# return -> break normalisation
for _ in range(3):
    s=re.sub(r'\n( +)return;\n( +)\}\n( +)return;\n',r'\n\2}\n\3break;\n',s)
    s=re.sub(r'return;\n( +case |\s*default)',r'break;\n\1',s)
s=re.sub(r'return;\n    \}\n\}\n','break;\n    }\n}\n',s)
# float vectors
import vecfix
s=vecfix.fix(s)
# fields
import emfields
s=emfields.conv(s)
s=s.replace('M2C_FIELD(em, s32 *, 0xA4)','em->ang[1]')
# game_w x1E as u16
s=s.replace('game_w.x1E','GAME_X1E16')
if 'GAME_X1E16' in s and '#define GAME_X1E16' not in s:
    s=s.replace('#define M2C_FIELD','#define GAME_X1E16 (*(u16 *)((u8 *)&game_w + 0x1E))\n#define M2C_FIELD',1)
# layer busy macro
s,n=re.subn(r'M2C_FIELD\(\(\((.+?) \* 0x50\) \+ em\), s32 \*, 0x194\)',r'EM_LYR(em, \1)',s)
if 'EM_LYR(' in s and 'define EM_LYR' not in s:
    s=s.replace('#define M2C_FIELD','typedef struct EML {\n    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */\n    u8 _pad[0x4C];\n} EML;\n#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)\n#define M2C_FIELD',1)
# swim/ran
s=s.replace('(ran_suu(1) & 0xFFFF & 1)','((u16)ran_suu(1) & 1)')
# static for suffix names
def mk(m):
    if m.group(2).startswith('dummy_em_prog'): return m.group(0)
    return 'static '+m.group(1)+m.group(2)+'('
s=re.sub(r'(?m)^(?!static)((?:void|s32|u8|int) )(\w+_00[56][0-9A-F]{5})\(',mk,s)

# turn-toward-target block (as em03: at most 0x40 per frame)
tp=re.compile(r'( +)(\w+) = em->ang\[1\];\n +(\w+) = \(\(Em_Calc_angY\(em->pos, em->tgt_pos\) & 0xFFFF\) - \2\) & 0xFFFF;\n +if \(\3 < 0x8001\) \{\n +if \(\3 < 0x40\) \{\n +(\w+) = \2 \+ \3;\n +\} else \{\n +\4 = \2 \+ 0x40;\n +\}\n +\} else if \(\3 >= 0xFFC1\) \{\n +\4 = \2 \+ \3;\n +\} else \{\n +\4 = \2 - 0x40;\n +\}\n +em->ang\[1\] = \4;\n')
def trep(m):
    i=m.group(1)
    return (i+'d = (u16)((u16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);\n'+i+'if (d <= 0x8000) {\n'+i+'    if (d <= 0x3F) {\n'+i+'        em->ang[1] += d;\n'+i+'    } else {\n'+i+'        em->ang[1] += 0x40;\n'+i+'    }\n'+i+'} else if (d > 0xFFC0) {\n'+i+'    em->ang[1] += d;\n'+i+'} else {\n'+i+'    em->ang[1] -= 0x40;\n'+i+'}\n')
s,nt=tp.subn(trep,s)
parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s)
out=[]
for f in parts:
    if re.search(r'\n +d = \(u16\)\(\(u16\)Em_Calc_angY',f) and not re.search(r'\n    int d;',f):
        f=re.sub(r'\{\n',r'{\n    int d;\n',f,count=1)
    out.append(f)
s=''.join(out)

# flag turn: goto block_7 form
gp=re.compile(r'( +)if \((\w+) >= 0xE39\) \{\n +if \(\2 >= 0xF1C8\) \{\n +goto block_7;\n +\}\n +if \(\2 >= 0x8000\) \{\n +em_char_set\(em, (\d+), 0, 0\);\n +(?:return|break);\n +\}\n +em_char_set\(em, (\d+), 0, 0\);\n +(?:return|break);\n +\}\nblock_7:\n +pl_flag_set\(\(PLW \*\) em, 0x20000\);\n +em_char_set\(em, (\d+), 0, 0\);\n')
def grep_(m):
    i=m.group(1); v=m.group(2)
    return (i+'if (%s < 0xE39 || %s >= 0xF1C8) {\n'%(v,v)+i+'    pl_flag_set((PLW *)em, 0x20000);\n'+i+'    em_char_set(em, %s, 0, 0);\n'%m.group(5)+i+'} else if (%s >= 0x8000) {\n'%v+i+'    em_char_set(em, %s, 0, 0);\n'%m.group(3)+i+'} else {\n'+i+'    em_char_set(em, %s, 0, 0);\n'%m.group(4)+i+'}\n')
s,ng=gp.subn(grep_,s)
s=re.sub(r'\(s32\)\((var_\w+)\)',r'\1',s)

vp=re.compile(r'( +)var_v1 = w->tgt_ang;\n( +)\} else if \((\w+) < 0x8000\) \{\n +var_v1 = \(em->ang\[1\] \+ (\w+)\) & 0xFFFF;\n +\} else \{\n +var_v1 = \(em->ang\[1\] - \4\) & 0xFFFF;\n +\}\n +em->ang\[1\] = \(s32\) var_v1;\n')
def vrep(m):
    i1,i2,t,v=m.groups()
    return (i1+'em->ang[1] = w->tgt_ang;\n'+i2+'} else if (%s < 0x8000) {\n'%t+i1+'em->ang[1] = (em->ang[1] + %s) & 0xFFFF;\n'%v+i2+'} else {\n'+i1+'em->ang[1] = (em->ang[1] - %s) & 0xFFFF;\n'%v+i2+'}\n')
s,nv=vp.subn(vrep,s)
s=s.replace('    u16 var_v1;\n','') if nv else s

s=re.sub(r'em_char_set2\(em, (0x3E9|0x4B1|0x579), (\w+), (\w+), M2C_ERROR\(/\* Unable to find stack arg 0x10 in block \*/\)\)',lambda x:'em_char_set2(em, %s, %s, %s, %s)'%(x.group(1),x.group(2),x.group(3),{'0x3E9':'0','0x4B1':'1','0x579':'2'}[x.group(1)]),s)
s=re.sub(r'(\w+_to_\w+)\((?:temp_\w+|arg\d)\);',r'\1(em);',s)
s=re.sub(r'(\w+_to_\w+)\((?:\d+|0x[0-9A-F]+), temp_\w+\);',r'\1(em);',s)
# ternary store pattern x95A etc
def tern(s):
    pat=re.compile(r'( +)(var_\w+) = (\d+);\n +if \(([^\n]*?)\) \{\n\n +\} else \{\n +\2 = (\d+);\n +\}\n +(em->\w+) = \2;\n')
    def r(m):
        i,v,a,c,b,dst=m.groups()
        return '%sif (%s) {\n%s    %s = %s;\n%s} else {\n%s    %s = %s;\n%s}\n'%(i,c,i,dst,a,i,i,dst,b,i)
    return pat.sub(r,s)
s=tern(s)
s=re.sub(r'    s8 var_v0;\n','',s)

s=re.sub(r'(_fly_adjy)\((?:\d+|temp_\w+)\)',r'\1(em, 1)',s)
def demofix(f):
    m=re.match(r'\S[^\n]*?\b(em_demo\w+)\(',f)
    if m and '    case 2:\n        break;\n    case 0:' in f:
        f=f.replace('    case 2:\n        break;\n    case 0:','    case 0:',1)
        k=f.index('\n    }\n',f.index('    case 0:'))
        f=f[:k]+'\n    case 2:\n        break;'+f[k:]
    return f
s=''.join(demofix(x) for x in re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s))

s=re.sub(r'(_senkai_\w+)\((?:temp_\w+|arg\d)\)',r'\1(em)',s)

s=s.replace('NextStage_No_Set();','NextStage_No_Set(em);')
s=re.sub(r'(?<!void )Em_Next_Stage_Pos\([^;]*\);','Em_Next_Stage_Pos(em);',s)

def fixmot2(f):
    for m in re.finditer(r'mot_miration_ret\(em, &sp([0-9A-F]+)\)',f):
        lo=m.group(1); hi='%X'%(int(lo,16)+8)
        f=f.replace('&sp'+lo,'sp'+lo)
        f=re.sub(r'f32 sp%s;'%lo,'f32 sp%s[3];'%lo,f)
        f=re.sub(r'\bsp%s\b'%hi,'sp%s[2]'%lo,f)
        f=re.sub(r'    (?:f32|s32) sp%s\[2\];\n'%lo,'',f)
    return f
s=''.join(fixmot2(x) for x in re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s))

s=re.sub(r'em->hagi\[([\w ()&]+)\]\[2\]',r'em->hagi[\1].cnt',s)
s=s.replace('em + 0xAC','em->pos').replace('em + 0x934','em->tgt_pos')
pat3=re.compile(r'( +)var_v0 = 0x10;\n +if \((\w+) == 0\) \{\n\n +\} else \{\n +var_v0 = 0xA;\n +if \(em->x8B6 == 0\) \{\n\n +\} else \{\n +var_v0 = 6;\n +\}\n +\}\n +em->x95A = var_v0;\n')
def r3(m):
    i,v=m.groups()
    return i+'if (%s == 0) {\n'%v+i+'    em->x95A = 0x10;\n'+i+'} else if (em->x8B6 == 0) {\n'+i+'    em->x95A = 0xA;\n'+i+'} else {\n'+i+'    em->x95A = 6;\n'+i+'}\n'
s=pat3.sub(r3,s)
s=re.sub(r'    s8 var_v0;\n','',s)
s=re.sub(r'(em\d+_atk_end_sel)\((?:temp_\w+)\);',r'\1(em);',s)
s=re.sub(r'(em\d+_fly_adjy2)\(&jtbl_\w+, \w+\)',r'\1(em)',s)

def fixaddr(f):
    for m in re.finditer(r'&sp([0-9A-F]+)\b',f):
        lo=m.group(1)
        v=int(lo,16)
        names=[('%X'%(v+4),1),('%X'%(v+8),2)]
        if any(re.search(r'\bsp%s\b'%n,f) for n,_ in names):
            f=re.sub(r'(?:f32|s32) sp%s;\n'%lo,'f32 sp%s[3];\n'%lo,f)
            f=f.replace('&sp'+lo,'sp'+lo)
            for n,k in names:
                f=re.sub(r'\bsp%s\b'%n,'sp%s[%d]'%(lo,k),f)
                f=re.sub(r'    (?:f32|s32) sp%s\[%d\];\n'%(lo,k),'',f)
    return f
s=''.join(fixaddr(x) for x in re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s))
s=re.sub(r'Event_flag_ck\((0x\w+), &jtbl_\w+, \w+\)',r'Event_flag_ck(\1)',s)
open(p,'w').write(s)
print('ok')
