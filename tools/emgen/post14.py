import re
p='/home/james/claude projects/MH XBOX/mh1-wt/D/wip/em14_nm.c'
s=open(p).read()
s=re.sub(r'typedef struct EM14W \{.*?\} EM14W;\n','',s,flags=re.S)
s=s.replace('#include "quest.h"\n','#include "quest.h"\n#include "em14.h"\n',1)
for a,b in (('w->x30','w->spd[0]'),('w->x34','w->spd[1]'),('w->x38','w->spd[2]'),('w->x14','w->tgt_ang'),('w->x10','w->dist'),('w->x17','w->has_tgt'),('w->x00','w->eff'),('w->x40 ','w->turn ')):
    s=s.replace(a,b)
s=s.replace('(s32 *) ((s32 *)((u8 *)w + 0x30))','w->spd').replace('(s32 *)((u8 *)w + 0x30)','w->spd')
s=re.sub(r'act_dist_select_005B5650\([^;]*\);','act_dist_select_005B5650(em);',s)
s=s.replace('static void act_dist_select_005B5650(em);','static void act_dist_select_005B5650(EMW *em);')
i=s.index('void em14_to_normal(EMW *em) {'); j=s.index('\n}\n',i)+3
s=s[:i]+'''void em14_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 != 0) {
        act_dist_select_005B5650(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        em14_act_set(em, 0, 1, 0);
        return;
    }
    if (em->char0 != 0x3E9) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2DE != 0x44D) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2E0 != 0x4B1) {
        em_char_set(em, 1, a, b);
    }
    em->x388 = 0;
    em->x3F4 = 0;
    em14_act_set(em, 0, 1, 0);
}
'''+s[j:]
s=s.replace('void em14_to_normal(EMW *em);','void em14_to_normal(EMW *em, s16 a, s16 b);')
i=s.index('void em14_frame_reset(EMW *em) {'); j=s.index('\n}\n',i)+3
s=s[:i]+'''void em14_frame_reset(EMW *em, int i) {
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
'''+s[j:]
s=s.replace('void em14_frame_reset(EMW *em);','void em14_frame_reset(EMW *em, int i);')
s=s.replace('extern int em14_act_add;','extern u8 *em14_act_add[3];')
s=s.replace('em_act_search(*(&em14_act_add + (em->_pad735[0] * 4)))','em_act_search(em14_act_add[M2C_FIELD(em, u8 *, 0x735)])')
s=s.replace('Event_flag_ck(0x12, &jtbl_1871, temp_a2)','Event_flag_ck(0x12)')
s=s.replace('em14_atk_end_sel(temp_a2);','em14_atk_end_sel(em);')
s=re.sub(r'(em_move0\d_\w+)\(em, M2C_ERROR\(/\* Read from unset register \$a1 \*/\)\)',r'\1(em, w)',s)
s=s.replace('void em14_main_sub(EMW *em);','void em14_main_sub(EMW *em, EM14W *w);')
s=s.replace('void em14_main_sub(EMW *em) {','void em14_main_sub(EMW *em, EM14W *w) {')
s=s.replace('(em, w);\n    }\n    if','(em, w);\n    }\n    if')
open(p,'w').write(s)

# ---- part b
s=open(p).read()
i=s.index('static void ground_land_eff_set_005C1120(EMW *em, s32 arg3) {'); j=s.index('\n}\n',i)+3
s=s[:i]+'''static void ground_land_eff_set_005C1120(EMW *em) {
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
'''+s[j:]
s=s.replace('static void ground_land_eff_set_005C1120(EMW *em, s32 arg3);','static void ground_land_eff_set_005C1120(EMW *em);')
s=s.replace('em14_main_sub(em);','em14_main_sub(em, w);')
s=s.replace('static void ef_move_sub_005BCD40(EMW *em, void *arg1, f32 arg2, f32 arg3);','static void ef_move_sub_005BCD40(EMW *em, EM14W *w);')
s=s.replace('static void ef_move_sub_005BCD40(EMW *em, void *arg1, f32 arg2, f32 arg3) {','static void ef_move_sub_005BCD40(EMW *em, EM14W *w) {\n    f32 sp50[3];\n    FLMAT m50;')
s=s.replace('M2C_FIELD(arg1, s16 *, 2)','w->anim')
s=s.replace('ef_move_sub_005BCD40(em->ex, temp_a2);','ef_move_sub_005BCD40(em, w);')
s=re.sub(r'flmatCopy\(\(f32 \(\*\)\[4\]\[4\]\) &sp50\[0\]\[0\], get_joint_wmat_em\(em, 0x27\)\);\n( +)flvecApplyMat33_2\(v, \(f32 \(\*\)\[4\]\[4\]\) &sp50\[0\]\[0\]\);\n +(v\d?)\[0\] = sp50\[3\]\[0\] \+ sp90;\n +\2\[1\] = sp50\[3\]\[1\] \+ sp94;\n +\2\[2\] = sp50\[3\]\[2\] \+ sp98;\n +Eft02_set3\(em, 0, 0xA, 0, REG8_addiu, REG9\);',
 lambda m:'flmatCopy(&m50, get_joint_wmat_em(em, 0x27));\n%sflvecApplyMat33_2(v, &m50);\n%s%s[0] = m50[3][0] + v[0];\n%s%s[1] = m50[3][1] + v[1];\n%s%s[2] = m50[3][2] + v[2];\n%sEft02_set3(em, 0, 0xA, 0, %s, 1.0f);'%(m.group(1),m.group(1),m.group(2),m.group(1),m.group(2),m.group(1),m.group(2),m.group(1),m.group(2)),s)
s=s.replace('Eft02_set3(em, 0, 9, 0, REG8_addiu, REG9);','Eft02_set3(em, 0, 9, 0, sp50, 1.0f);').replace('Eft02_set3(em, 0, 9, 1, REG8_addiu, REG9);','Eft02_set3(em, 0, 9, 1, sp50, 1.0f);')
s=s.replace('Eft13_set_pos2(10.0f, em, (EMW *) &sp50, (f32 *)0x22);','Eft13_set_pos2(10.0f, em, sp50, 0x22);')
s=s.replace('get_joint_pos_em(em, 0x27, &sp50);','get_joint_pos_em(em, 0x27, sp50);').replace('AddVector(&sp50, &sp50, v3);','AddVector(sp50, sp50, v3);')
s=s.replace('SetVector(&sp50, 6e-45f, arg2, arg3);\n                Eft13_set_pos(20.0f, em, (f32 *)0x14);','SetVector(sp50, em->pos[0], em->x7E4, em->pos[2]);\n                Eft13_set_pos(20.0f, sp50, 0x14);')
open(p,'w').write(s)

# ---- part c
s=open(p).read()
s=s.replace('    f32 sp54;\n    f32 sp50;\n','')
i=s.index('static void ef_move_sub_005BCD40(EMW *em, EM14W *w) {'); j=s.index('\n}\n',i)
seg=s[i:j].replace('&sp50','sp50')
s=s[:i]+seg+s[j:]
i=s.index('static void em_dmg07_005B9ED0(EMW *em, EM14W *w) {')
s=s[:i]+s[i:].replace('static void em_dmg07_005B9ED0(EMW *em, EM14W *w) {','static void em_dmg07_005B9ED0(EMW *em, EM14W *w) {\n    FLMAT m50;',1)
open(p,'w').write(s)

# ---- part d
s=open(p).read()
s=s.replace('sp54 = em->x5AC;','sp50[1] = em->x5AC;')
s=s.replace('Eft17_set_ex(sp50, (s32) M2C_FIELD(em, u16 *, 0xA4), 8, 1.0f);','Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);')
open(p,'w').write(s)

# ---- part e: helpers
s=open(p).read()
def sub_def(name,new):
    global s
    m=re.search(r'(?m)^static void %s\([^\n]*\) \{$'%name,s)
    j=s.index('\n}\n',m.end())+3
    s=s[:m.start()]+new+s[j:]
    s=re.sub(r'(?m)^static void %s\([^\n]*\);$'%name,new.split('{')[0].strip()+';',s,count=1)
sub_def('sound_call_sub_005BCB30','''static void sound_call_sub_005BCB30(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}
''')
sub_def('sound_call_005BCBA0','''static void sound_call_005BCBA0(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005BCB30(em, se, joint);
    }
}
''')
sub_def('sound_call_parts_005BCC00','''static void sound_call_parts_005BCC00(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}
''')
sub_def('quake_call_005BCCA0','''static void quake_call_005BCCA0(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}
''')
s=s.replace('static void move_default_005BCCF0(void *em)','static void move_default_005BCCF0(EMW *em)')
s=re.sub(r'(M2C_FIELD\(em, )s16( \*, 0x5F[0246]\))',r'\1u16\2',s)
s=re.sub(r'(M2C_FIELD\(em, )s8( \*, 0x5F[89AB]\))',r'\1u8\2',s)
if 'void Em_set_quake_sub' not in s: s=s.replace('static void quake_call_005BCCA0(EMW *em, int frame, int arg);','static void quake_call_005BCCA0(EMW *em, int frame, int arg);\nvoid Em_set_quake_sub(EMW *, int);',1)
s=re.sub(r'(sound_call_parts_005BCC00\(em, [^;]*?)\);',lambda m:m.group(1)+', 1);' if not m.group(0).count(',')>=5 else m.group(0),s)
open(p,'w').write(s)

# ---- part f
s=open(p).read()
if 'define EM_LYR' not in s:
    s=s.replace('#define M2C_FIELD','typedef struct EML {\n    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */\n    u8 _pad[0x4C];\n} EML;\n#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)\n#define M2C_FIELD',1)
def rep_fn(name,new,proto_old=None,proto_new=None):
    global s
    m=re.search(r'(?m)^(?:static )?void %s\([^\n]*\) \{$'%name,s)
    j=s.index('\n}\n',m.end())+3
    s=s[:m.start()]+new+s[j:]
rep_fn('em14_to_swim','''void em14_to_swim(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 4;
    em->x3F4 = 0;
    em->x839 = 1;
    em14_act_set(em, 2, 1, 0);
}
''')
rep_fn('em14_to_fly','''void em14_to_fly(EMW *em, int flag) {
    em->x839 = 1;
    em->act_spd = 1.0f;
    if (flag & 0xFF) {
        return;
    }
    em_act_set(em, 2, 0xE);
}
''')
s=s.replace('void em14_to_fly(EMW *em);','void em14_to_fly(EMW *em, int flag);')
rep_fn('act_dist_select_005B5650','''static void act_dist_select_005B5650(EMW *em) {
    if (em->x734 == 3) {
        em->x839 = 1;
        em14_act_set(em, 0, 1, 0);
    }
}
''')
open(p,'w').write(s)

# ---- part g: init
s=open(p).read()
m=re.search(r'(?m)^void em14_init\(EMW \*em\) \{$',s)
j=s.index('\n}\n',m.end())+3
s=s[:m.start()]+'''void em14_init(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
    int hp;
    u8 temp_a0;
    u8 temp_v1_2;

    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
        case 0x33:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 11000.0f;
            break;
        case 0x34:
            em->pos[0] = 12000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        case 0x35:
            em->pos[0] = 12000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 4;
    em14_act_set(em, 2, 1, 0);
    w->x06 = 0;
    if (em->kind == 0xE) {
        hp = em_hp_vital_set2(em, 0x7D0, 0x5DC);
        em->x302 = hp;
    } else {
        hp = em_hp_vital_set2(em, 0x640, 0x578);
        em->x302 = hp;
    }
    em->x792 = hp;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em14_stay_timer_tbl[em->stg];
    em->runaway_tm = em14_runaway_timer_tbl[em->stg];
    w->tgt_ang = 0x4000;
    w->x3C = 0x100;
    w->turn = 0x200;
    w->x44 = 0x100;
    w->x20 = 0x2000;
    w->x1C = 0;
    w->x1A = 0;
    w->x1B = 0;
    em->x734 = 3;
    M2C_FIELD(em, u8 *, 0x735) = 0;
    em->x7E0 = 1000.0f;
    temp_v1_2 = em->x948 & 1;
    em->x948 = temp_v1_2;
    if (temp_v1_2 == 0) {
        temp_a0 = em->kind;
        switch (temp_a0) {
        case 0xE:
        case 0x1A:
            em->ex[0xA3] = 0;
            eft09_set(em, 0x2000, 0x200, 0x100);
            break;
        }
    }
}
'''+s[j:]
open(p,'w').write(s)
