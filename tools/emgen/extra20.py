import re
p='/home/james/claude projects/MH XBOX/mh1-wt/D/wip/em20_nm.c'
s=open(p).read()
def rep_fn(pattern,new,proto=None):
    global s
    m=re.search(pattern,s,re.M)
    j=s.index('\n}\n',m.end())+3
    s=s[:m.start()]+new+s[j:]
rep_fn(r'^u16 \*em_act_search2_005EC0A0\(void \*em, u16 \*arg1\) \{$','''static u16 *em_act_search2_005EC0A0(EMW *em, u16 *tbl) {
    EM20W *w = (EM20W *)em->ex;
    u16 *p;
    u8 i;

    i = w->x19;
    w->x19 = i + 1;
    p = tbl + i * 4;
    if (*p == 0xFFFF) {
        p = tbl;
        w->x19 = 1;
    }
    return p;
}
''')
s=re.sub(r'(?m)^u16 \*em_act_search2_005EC0A0\([^\n]*\);\n','static u16 *em_act_search2_005EC0A0(EMW *em, u16 *tbl);\n',s)
s=s.replace('extern int em20_act_add;','extern u8 *em20_act_add[3];').replace('extern int em20_rail_add;','extern u16 *em20_rail_add[2];').replace('extern int em20_rail_half_add;','extern u16 *em20_rail_half_add[1];')
rep_fn(r'^static void act_dist_select_005EC0E0\(EMW \*em\) \{$','''static void act_dist_select_005EC0E0(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    u16 *p;
    u8 temp_a1;
    u8 temp_a2;

    temp_a1 = em->x734;
    temp_a2 = M2C_FIELD(em, u8 *, 0x735);
    switch (temp_a1) {
    case 0:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, em_act_search(em20_act_add[temp_a2]) & 0xFFFF, 1);
        }
        break;
    case 1:
        if (em->x8C3 == 0) {
            p = em_act_search2_005EC0A0(em, em20_rail_add[temp_a2]);
            if (p[0] == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em20_act_set(em, p[0], p[1], 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            p = em_act_search2_005EC0A0(em, em20_rail_half_add[temp_a2]);
            if (p[0] == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em20_act_set(em, p[0], p[1], 1);
        }
        break;
    case 3:
        em->x839 = 1;
        if (em->x388 == 0) {
            em20_act_set(em, 0, 1, 0);
        } else {
            em20_act_set(em, 2, 2, 0);
        }
        break;
    }
}
''')
rep_fn(r'^void em20_to_normal\(EMW \*em\) \{$','''void em20_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 != 0) {
        act_dist_select_005EC0E0(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)(0.3f * (f32)em->x792)) {
            em20_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em20_act_set(em, 0, 1, 0);
        } else {
            em20_act_set(em, 0, 0x11, 0);
        }
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
    em20_act_set(em, 0, 1, 0);
}
''')
s=s.replace('void em20_to_normal(EMW *em);','void em20_to_normal(EMW *em, s16 a, s16 b);')
rep_fn(r'^void em20_to_fly\(EMW \*em\) \{$','''void em20_to_fly(EMW *em, int flag) {
    if (em->x734 != 3) {
        act_dist_select_005EC0E0(em);
        return;
    }
    em->x839 = 1;
    em->act_spd = 1.0f;
    if (flag & 0xFF) {
        return;
    }
    em_act_set(em, 2, 0xE);
}
''')
s=s.replace('void em20_to_fly(EMW *em);','void em20_to_fly(EMW *em, int flag);')
s=re.sub(r'act_dist_select_005EC0E0\([^;]*\);',lambda m:m.group(0) if m.group(0)=='act_dist_select_005EC0E0(em);' else 'act_dist_select_005EC0E0(em);',s)
s=s.replace('static void act_dist_select_005EC0E0(em);','static void act_dist_select_005EC0E0(EMW *em);')
open(p,'w').write(s)

s=open(p).read()

open(p,'w').write(s)

s=open(p).read()
def rep_fn2(pattern,new):
    global s
    m=re.search(pattern,s,re.M)
    if not m: print('NOFN',pattern); return
    j=s.index('\n}\n',m.end())+3
    s=s[:m.start()]+new+s[j:]
rep_fn2(r'^static void item_theft_005EC560\(PLW \*arg1\) \{$','''static void item_theft_005EC560(PLW *pl) {
    int list[20];
    int n;
    int i;
    int sel;
    u16 id;

    if (pl->id == game_w.master && Quest_clear_ck(1) == 0) {
        if (Pl_Skill_ck(pl, 0x2B) != 1) {
            n = 0;
            for (i = 0; i < 20; i++) {
                id = pl->item[i].id;
                if (id != 0) {
                    if (Item_data[id][0] == 0 && Item_data[id][2] < 4) {
                        list[n++] = i;
                    }
                }
            }
            if (n != 0) {
                sel = (s16)pl->item[list[(u16)ran_suu(1) % n]].id;
                Pl_item_stack(pl, sel & 0xFFFF, -1);
                set01_set(1, 6, sel);
            }
        }
    }
}
''')
s=re.sub(r'item_theft_005EC560\(em->x7A0, \w+, \w+\);','item_theft_005EC560(em->x7A0);',s)
rep_fn2(r'^static void hover_eff_set2_005FCA20\(s32 arg3\) \{$','''static void hover_eff_set2_005FCA20(EMW *em) {
    if (GAME_X1E16 % 5 == 0) {
        Eft20_set(1.0f, em, 0xA, 0);
    }
}
''')
s=s.replace('static void hover_eff_set2_005FCA20(s32 arg3);','static void hover_eff_set2_005FCA20(EMW *em);')
s=re.sub(r'(kyusyu_\w+)\(em, (?:temp_\w+|w)\)',r'\1(em)',s)
s=re.sub(r'(em\d+_atk_end_sel)\(em, w\)',r'\1(em)',s)
s=s.replace('FLYNEED','FLYNEED')
s=re.sub(r'void \*temp_a3;\n\n    temp_a2 = em->x05;\n    temp_t0 = em->kind \* 4;\n    temp_a3 = \*\(&em_thirst_tbl \+ temp_t0\);','FLYNEED *temp_a3;\n\n    temp_a2 = em->x05;\n    temp_a3 = em_thirst_tbl[em->kind];',s)
s=s.replace('M2C_FIELD(temp_a3, s32 *, 0x14)','temp_a3->x14')
s=re.sub(r'\n    s32 temp_t0;\n','\n',s,count=0) if False else s
if 'extern FLYNEED *em_thirst_tbl' not in s and 'em_thirst_tbl' in s:
    s=s.replace('#define GAME_X1E16','typedef struct FLYNEED {\n    u8 _pad00[0x14];\n    s32 x14;\n} FLYNEED;\nextern FLYNEED *em_thirst_tbl[];\n#define GAME_X1E16',1)
s=s.replace('extern int em_thirst_tbl;','')
if 'typedef f32 (*EM_POSP)' not in s and 'gp_ptr_ck' in s:
    s=s.replace('#define GAME_X1E16','typedef f32 (*EM_POSP)[3];\nEM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);\n#define GAME_X1E16',1)
open(p,'w').write(s)

s=open(p).read()
rep_fn2(r'^static void em_atk26_005F3CE0\(EMW \*em, EM20W \*w\) \{$','''static void em_atk26_005F3CE0(EMW *em, EM20W *w, int idx) {
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 0;
    case 1:
        if (em20_horm_main(em) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x2F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 2.0f * (f32)(u32)gero_tbl[idx][0]) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, gero_tbl[idx][1], 0);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, gero_tbl[idx][1], 0);
            }
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}
''')
rep_fn2(r'^static void em_atk30_005F4090\(EMW \*em, EM20W \*w\) \{$','''static void em_atk30_005F4090(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 0, 2.0f * (f32)(u32)gero_tbl[0][0]) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, gero_tbl[0][1], 0);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, gero_tbl[0][1], 0);
            }
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}
''')
s=s.replace('static void em_atk26_005F3CE0(EMW *em, EM20W *w);','static void em_atk26_005F3CE0(EMW *em, EM20W *w, int idx);')
i=s.index('static void em_move03_005F6440(EMW *em, EM20W *w) {'); j=s.index('\n}\n',i)
seg=s[i:j]
vals=iter(['1','2','3','0'])
seg=re.sub(r'em_atk26_005F3CE0\(em, w\);',lambda m:'em_atk26_005F3CE0(em, w, %s);'%next(vals),seg)
s=s[:i]+seg+s[j:]
if 'extern u16 gero_tbl' not in s:
    s=s.replace('#define GAME_X1E16','extern u16 gero_tbl[4][2];\n#define GAME_X1E16',1)
s=re.sub(r'(?m)^extern int gero_tbl;\n','',s)
# Stage_data / gp_ptr_ck regions
def stage_fix(f):
    if 'Stage_data_get(' in f:
        f=f.replace('void *temp_v0;','STAGE_DATA *temp_v0;')
        f=f.replace('M2C_FIELD(temp_v0, f32 *, 0x10)','temp_v0->width').replace('M2C_FIELD(temp_v0, f32 *, 0x14)','temp_v0->depth').replace('M2C_FIELD(temp_v0, f32 *, 0x18)','temp_v0->floor_y')
        f=f.replace('void *temp_s0;','STAGE_DATA *temp_s0;')
        f=f.replace('M2C_FIELD(temp_s0, f32 *, 0x18)','temp_s0->floor_y').replace('M2C_FIELD(temp_s0, f32 *, 0x10)','temp_s0->width').replace('M2C_FIELD(temp_s0, f32 *, 0x14)','temp_s0->depth')
    if 'gp_ptr_ck(' in f:
        f=f.replace('void *temp_v0_2;','EM_POSP temp_v0_2;')
        f=f.replace('M2C_FIELD(temp_v0_2, f32 *, 0)','(*temp_v0_2)[0]').replace('M2C_FIELD(temp_v0_2, f32 *, 8)','(*temp_v0_2)[2]')
    return f
s=''.join(stage_fix(x) for x in re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s))
s=re.sub(r'\(s64\) \(\(s64\) (\w+) << 0x38\) >> 0x38',r'(s8)\1',s)
if 'STAGE_DATA' in s and 'typedef struct STAGE_DATA' not in s:
    s=s.replace('#define GAME_X1E16','typedef struct STAGE_DATA {\n    u8 _pad00[0x10];\n    f32 width;\n    f32 depth;\n    f32 floor_y;\n} STAGE_DATA;\nSTAGE_DATA *Stage_data_get(u8);\n#define GAME_X1E16',1)
open(p,'w').write(s)

s=open(p).read()
rep_fn2(r'^void em20_material_sub\(void \*em, s32 arg1, s32 arg2\) \{$','''void em20_material_sub(EMW *em, int type, u8 *tbl) {
    u8 *base = *(u8 **)((u8 *)em->mdl + 0x10);
    int i = 0;
    EM20W *w = (EM20W *)em->ex;
    s32 *p = (s32 *)(tbl + type * 0x8C);

    if (p[1] > 0) {
        s32 *num = &p[1];

        do {
            u8 *m = base + p[2] * 0x4C;

            *(f32 *)(m + 0x10) = em->x798;
            switch (type) {
            case 0:
                switch (i) {
                case 1:
                    *(s32 *)(m + 0x10) = 0;
                    break;
                case 3:
                    if (w->x1B == 0) {
                        *(s32 *)(m + 0x10) = 0;
                    } else {
                        *(f32 *)(m + 0x10) = 1.0f;
                    }
                    break;
                }
                break;
            case 2:
                switch (i) {
                case 5:
                    if (w->x1B != 0) {
                        *(s32 *)(m + 0x10) = 0;
                    }
                    break;
                case 6:
                    *(s32 *)(m + 0x10) = 0;
                    break;
                }
                break;
            case 3:
                switch (i) {
                case 0:
                    if (w->x1B == 0 || w->x08 == 0) {
                        *(s32 *)(m + 0x10) = 0;
                    } else {
                        *(f32 *)(m + 0x10) = 1.0f;
                    }
                    break;
                case 5:
                    if (em->x8B6 != 0 && em->mode != 5) {
                        *(f32 *)(m + 0x10) = 1.0f;
                    } else {
                        *(s32 *)(m + 0x10) = 0;
                    }
                    break;
                }
                break;
            }
            flSetRenderState((i + 0x3A) & 0xFF, (u32)m);
            i++;
            p++;
        } while (i < *num);
    }
}
''')
s=re.sub(r'(?m)^void em20_material_sub\(void \*em, s32 arg1, s32 arg2\);','void em20_material_sub(EMW *em, int type, u8 *tbl);',s)
rep_fn2(r'^static void takeon_eff_set_005FC980\(EMW \*em\) \{$','''static void takeon_eff_set_005FC980(EMW *em) {
    f32 pos[3];
    f32 temp_f2;

    if (game_w.stage == 0 && GAME_X1E16 % 10 == 0) {
        get_joint_pos_em(em, 0, pos);
        temp_f2 = em->x5AC;
        if (pos[1] - temp_f2 < 400.0f && temp_f2 <= 46.0f) {
            eft11_set(em, pos, 1);
        }
    }
}
''')
s=re.sub(r'Event_flag_ck\((0x\w+), &jtbl_\w+, \w+\)',r'Event_flag_ck(\1)',s)
s=re.sub(r'em20_to_fly\(em, REG5\)','em20_to_fly(em, 0)',s)
# ef_move_sub arg1 leftover
i=s.index('static void ef_move_sub_005F7800(EMW *em, EM20W *w) {'); j=s.index('\n}\n',i)
seg=s[i:j]
seg=seg.replace('(arg1)','(w)').replace('M2C_FIELD(arg1, ','M2C_FIELD(w, ')
s=s[:i]+seg+s[j:]
open(p,'w').write(s)

s=open(p).read()
s=s.replace('Eft14_set3(&sp30, 5, 1.0f, 1.0f);','Eft14_set3(sp30, 5, 1.0f, (PLW *)em);')
s=s.replace('''        flmatCopy((f32 (*)[4][4]) &sp40[0][0], get_joint_wmat_em(em, 0x27));
        flvecApplyMat33_2(v, (f32 (*)[4][4]) &sp40[0][0]);
        v2[0] = sp40[3][0] + sp90;
        v2[1] = sp40[3][1] + sp94;
        v2[2] = sp40[3][2] + sp98;
        Eft02_set3(em, 0, 0xA, 0, REG8_addiu, REG9);''','''        flmatCopy(&m40, get_joint_wmat_em(em, 0x27));
        flvecApplyMat33_2(v, &m40);
        v2[0] = m40[3][0] + v[0];
        v2[1] = m40[3][1] + v[1];
        v2[2] = m40[3][2] + v[2];
        Eft02_set3(em, 0, 0xA, 0, v2, 1.0f);''')
s=s.replace('static void em_dmg20_005F5490(EMW *em, EM20W *w) {\n','static void em_dmg20_005F5490(EMW *em, EM20W *w) {\n    FLMAT m40;\n',1)
rep_fn2(r'^void em20_dmg_to_normal\(EMW \*em\) \{$','''void em20_dmg_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x39A % 100 < 0xA) {
        em20_to_normal(em, 0, 0);
        em->x839 = 0;
        if (em->kind == 0x14) {
            em20_act_set(em, 3, 0x1E, 1);
            return;
        }
        if (em->x8B6 != 0) {
            em20_act_set(em, 3, 0x1F, 1);
            return;
        }
        em20_act_set(em, 3, 0x1E, 1);
        return;
    }
    em20_to_normal(em, a, b);
}
''')
s=s.replace('void em20_dmg_to_normal(EMW *em);','void em20_dmg_to_normal(EMW *em, s16 a, s16 b);')
open(p,'w').write(s)

s=open(p).read()
s=s.replace('M2C_FIELD(*(&em_hungry_tbl + temp_t0), s32 *, 0x14)','em_hungry_tbl[em->kind]->x14')
s=s.replace('extern int em_hungry_tbl;','extern FLYNEED *em_hungry_tbl[];')
i=s.index('static void em_fly08_005F02E0(EMW *em, EM20W *w) {'); j=s.index('\n}\n',i)
seg=s[i:j].replace('    s32 temp_t0;\n','')
s=s[:i]+seg+s[j:]
i=s.index('static void ef_move_sub_005F7800(EMW *em, EM20W *w) {'); j=s.index('\n}\n',i)
seg=s[i:j].replace('    f32 sp30;\n','    f32 sp30[3];\n').replace('flmatGetTrans(&sp30,','flmatGetTrans(sp30,')
s=s[:i]+seg+s[j:]
open(p,'w').write(s)

s=open(p).read()
cases=[(0,8000,0,6000,None),(15,7900,1050,13900,None),(18,10000,200,8000,None),(22,9900,300,10350,None),(24,10000,0,5700,'em->ang[1] = 0x4000;'),(27,14300,0,7100,None),(33,None,0,18000,None),(37,9750,300,7750,None),(40,10500,0,9500,None)]
body=''
for st,x,y,z,extra in cases:
    if x is None: body+='        case %d:\n            em->pos[0] = 17300.0f;\n            em->pos[1] = %d.0f;\n            em->pos[2] = %d.0f;\n'%(st,y,z)
    else: body+='        case %d:\n            em->pos[0] = %d.0f + 1500.0f * (f32)(u32)em->x13;\n            em->pos[1] = %d.0f;\n            em->pos[2] = %d.0f;\n'%(st,x,y,z)
    if extra: body+='            %s\n'%extra
    body+='            break;\n'
rep_fn2(r'^void em20_init\(EMW \*em\) \{$','''void em20_init(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    int hp;
    u8 temp_a0;
    u8 temp_a1;

    if (quest_w.no == 0) {
        em->ang[1] = 0;
        switch (game_w.stage) {
'''+body+'''        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em20_act_set(em, 0, 1, 0);
    w->x06 = 0;
    w->x1B = 1;
    w->x08 = 0;
    if (em->kind == 0x14) {
        hp = em_hp_vital_set2(em, 0x1F4, 0x3E8);
        em->x302 = hp;
    } else {
        hp = em_hp_vital_set2(em, 0x12C, 0x2BC);
        em->x302 = hp;
    }
    em->x792 = hp;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em20_stay_timer_tbl[em->stg];
    em->runaway_tm = em20_runaway_timer_tbl[em->stg];
    w->dang = 0x4000;
    w->pitch_spd = 0x100;
    w->turn = 0x200;
    w->bank_spd = 0x100;
    w->bank_max = 0x2000;
    w->turn_left = 0;
    em->x734 = 3;
    M2C_FIELD(em, u8 *, 0x735) = 0;
    w->x48 = 0;
    w->x49 = 0;
    w->x4A = 0;
    temp_a0 = em->x948 & 1;
    em->x948 = temp_a0;
    if (temp_a0 == 0) {
        temp_a1 = em->kind;
        switch (temp_a1) {
        case 1:
        case 6:
        case 8:
        case 0xB:
        case 0xF:
        case 0xE:
        case 0x11:
        case 0x15:
        case 0x16:
        case 0x1A:
            em->ex[0xA3] = 0;
            eft09_set(em, temp_a1);
            break;
        case 0x14:
            break;
        }
    }
}
''')
if 'void eft09_set(EMW *, int);' not in s: s=s.replace('void em_act_set(EMW *, int, u16);','void em_act_set(EMW *, int, u16);\nvoid eft09_set(EMW *, int);',1)
open(p,'w').write(s)
