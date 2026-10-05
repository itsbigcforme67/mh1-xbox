#!/usr/bin/env python3
import re,sys
NN=sys.argv[1]; p=sys.argv[2]
s=open(p).read()
W='EM%sW'%NN
def rep_fn(pattern,newfn):
    global s
    m=re.search(pattern,s,re.M)
    if not m: return None
    name=m.group(1)
    j=s.index('\n}\n',m.end())+3
    new=newfn(name)
    s=s[:m.start()]+new+s[j:]
    return name
def protofix(name,sig):
    global s
    s=re.sub(r'(?m)^static void %s\([^\n]*\);$'%re.escape(name),sig+';',s,count=1)
def sub_hdr(fn_re,tmpl):
    n=rep_fn(fn_re,lambda nm:tmpl.replace('NAME',nm))
    if n: protofix(n,tmpl.split('{')[0].replace('NAME',n).strip())
sub_hdr(r'^static void (sound_call_sub_\w+)\(EMW \*em, s32 arg1, s32 arg2\) \{$','''static void NAME(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}
''')
sn=re.search(r'static void (sound_call_sub_\w+)\(EMW',s).group(1) if re.search(r'static void (sound_call_sub_\w+)\(EMW',s) else None
if sn:
    sub_hdr(r'^static void (sound_call_[0-9A-F]{8})\(EMW \*em, s32 arg1, f32 arg2, int arg3\) \{$','''static void NAME(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        %s(em, se, joint);
    }
}
'''%sn)
    sub_hdr(r'^static void (sound_call_parts_\w+)\(EMW \*em, s32 arg1, f32 arg2, s32 arg3\) \{$','''static void NAME(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}
''')
    s=re.sub(r'(sound_call_parts_\w+\(em, [^;]*?)\);',lambda m:m.group(0) if m.group(1).count(',')>=4 else m.group(1)+', 1);',s)
sub_hdr(r'^static void (quake_call_\w+)\(EMW \*em, s32 arg1, f32 arg2\) \{$','''static void NAME(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}
''')
if 'void Em_set_quake_sub' not in s and 'Em_set_quake_sub(' in s:
    s=s.replace('static void quake_call','void Em_set_quake_sub(EMW *, int);\nstatic void quake_call',1)
s=re.sub(r'static void (move_default_\w+)\(void \*em\)',r'static void \1(EMW *em)',s)
s=re.sub(r'(M2C_FIELD\(em, )s16( \*, 0x5F[0246]\))',r'\1u16\2',s)
s=re.sub(r'(M2C_FIELD\(em, )s8( \*, 0x5F[89AB]\))',r'\1u8\2',s)
sub_hdr(r'^static void (ground_land_eff_set_\w+)\(EMW \*em, s32 arg3\) \{$','''static void NAME(EMW *em) {
    f32 pos[3];

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0x14, pos);
        pos[1] = em->x5AC;
        if (pos[1] <= 46.0f) {
            eft11_set(em, pos, 1);
            get_joint_pos_em(em, 0x1A, pos);
            eft11_set(em, pos, 1);
        }
    } else {
        Eft20_set(1.0f, em, 0xB, 0);
    }
}
''')
# frame_reset
n=rep_fn(r'^void (em%s_frame_reset)\(EMW \*em\) \{$'%NN,lambda nm:'''void %s(EMW *em, int i) {
    if (EM_LYR(em, i) == 0) {
        switch (i) {
        case 0:
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
            break;
        case 1:
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
            break;
        case 2:
            em_char_set2(em, 0x579, 0xA, 0, 2);
            break;
        }
    }
}
'''%nm)
if n: s=s.replace('void %s(EMW *em);'%n,'void %s(EMW *em, int i);'%n)
if 'EM_LYR(' in s and 'define EM_LYR' not in s:
    s=s.replace('#define M2C_FIELD','typedef struct EML {\n    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */\n    u8 _pad[0x4C];\n} EML;\n#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)\n#define M2C_FIELD',1)
# ef_move_sub
m=re.search(r'^static void (ef_move_sub_\w+)\(EMW \*em, void \*arg1[^)]*\) \{$',s,re.M)
if m:
    nm=m.group(1)
    s=s.replace(m.group(0),'static void %s(EMW *em, %s *w) {'%(nm,W))
    s=re.sub(r'static void %s\(EMW \*em, void \*arg1[^)]*\);'%nm,'static void %s(EMW *em, %s *w);'%(nm,W),s)
    an=re.search(r'M2C_FIELD\(arg1, s16 \*, (\d+|0x[0-9A-F]+)\)',s)
    if an: s=s.replace(an.group(0),'w->anim')
    s=re.sub(r'%s\(em->ex, \w+\);'%nm,'%s(em, w);'%nm,s)
    s=re.sub(r'%s\(em, [^;]*?\);'%nm,'%s(em, w);'%nm,s)
    s=re.sub(r'%s\(em->ex[^;]*\);'%nm,'%s(em, w);'%nm,s)
# main_sub
s=re.sub(r'void (em%s_main_sub)\(EMW \*em\)'%NN,r'void \1(EMW *em, %s *w)'%W,s)
s=re.sub(r'(em%s_main_sub)\(em\);'%NN,r'\1(em, w);',s)
s=re.sub(r'(em_move0\d_\w+)\(em, M2C_ERROR\(/\* Read from unset register \$a1 \*/\)\)',r'\1(em, w)',s)
# generic NextStage etc
s=s.replace('NextStage_No_Set();','NextStage_No_Set(em);')
s=re.sub(r'(?<!void )Em_Next_Stage_Pos\([^;]*\);','Em_Next_Stage_Pos(em);',s)
s=s.replace('void NextStage_No_Set(void);','void NextStage_No_Set(EMW *);\nvoid Em_Next_Stage_Pos(EMW *);\nvoid NextStage_Dir_Set(EMW *, f32 *);')
s=re.sub(r'(\w+_fly_adjy)\((?:\d+|temp_\w+)\)',r'\1(em, 1)',s)
if 'STAGE_DATA' in s and 'typedef struct STAGE_DATA' not in s:
    s=s.replace('#define GAME_X1E16','typedef struct STAGE_DATA {\n    u8 _pad00[0x10];\n    f32 width;          /* 0x10 */\n    f32 depth;          /* 0x14 */\n    f32 floor_y;        /* 0x18 */\n} STAGE_DATA;\nSTAGE_DATA *Stage_data_get(u8);\n#define GAME_X1E16',1)
if 'FLYNEED' in s and 'typedef struct FLYNEED' not in s:
    s=s.replace('#define GAME_X1E16','typedef struct FLYNEED {\n    u8 _pad00[0x14];\n    s32 x14;\n} FLYNEED;\n#define GAME_X1E16',1)
open(p,'w').write(s)
print('postgen2 done')
