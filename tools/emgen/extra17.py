import re
p='/home/james/claude projects/MH XBOX/mh1-wt/D/wip/em17_nm.c'
s=open(p).read()
def rep_fn(pattern,new,proto=None):
    global s
    m=re.search(pattern,s,re.M)
    j=s.index('\n}\n',m.end())+3
    s=s[:m.start()]+new+s[j:]
rep_fn(r'^u16 \*em_act_search2_005DA580\(void \*em, u16 \*arg1\) \{$','''static u16 *em_act_search2_005DA580(EMW *em, u16 *tbl) {
    EM17W *w = (EM17W *)em->ex;
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
s=re.sub(r'(?m)^u16 \*em_act_search2_005DA580\([^\n]*\);\n','static u16 *em_act_search2_005DA580(EMW *em, u16 *tbl);\n',s)
s=s.replace('extern int em17_act_add;','extern u8 *em17_act_add[3];').replace('extern int em17_rail_add;','extern u16 *em17_rail_add[2];').replace('extern int em17_rail_half_add;','extern u16 *em17_rail_half_add[1];')
rep_fn(r'^static void act_dist_select_005DA5C0\(EMW \*em\) \{$','''static void act_dist_select_005DA5C0(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    u16 *p;
    u8 temp_a1;
    u8 temp_a2;

    temp_a1 = em->x734;
    temp_a2 = M2C_FIELD(em, u8 *, 0x735);
    switch (temp_a1) {
    case 0:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, em_act_search(em17_act_add[temp_a2]) & 0xFFFF, 1);
        }
        break;
    case 1:
        if (em->x8C3 == 0) {
            p = em_act_search2_005DA580(em, em17_rail_add[temp_a2]);
            if (p[0] == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em17_act_set(em, p[0], p[1], 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            p = em_act_search2_005DA580(em, em17_rail_half_add[temp_a2]);
            if (p[0] == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em17_act_set(em, p[0], p[1], 1);
        }
        break;
    case 3:
        em->x839 = 1;
        if (em->x388 == 0) {
            em17_act_set(em, 0, 1, 0);
        } else {
            em17_act_set(em, 2, 2, 0);
        }
        break;
    }
}
''')
rep_fn(r'^void em17_to_normal\(EMW \*em\) \{$','''void em17_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 != 0) {
        act_dist_select_005DA5C0(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)(0.3f * (f32)em->x792)) {
            em17_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em17_act_set(em, 0, 1, 0);
        } else {
            em17_act_set(em, 0, 0x11, 0);
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
    em17_act_set(em, 0, 1, 0);
}
''')
s=s.replace('void em17_to_normal(EMW *em);','void em17_to_normal(EMW *em, s16 a, s16 b);')
rep_fn(r'^void em17_to_fly\(EMW \*em\) \{$','''void em17_to_fly(EMW *em, int flag) {
    if (em->x734 != 3) {
        act_dist_select_005DA5C0(em);
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
s=s.replace('void em17_to_fly(EMW *em);','void em17_to_fly(EMW *em, int flag);')
s=re.sub(r'act_dist_select_005DA5C0\([^;]*\);',lambda m:m.group(0) if m.group(0)=='act_dist_select_005DA5C0(em);' else 'act_dist_select_005DA5C0(em);',s)
s=s.replace('static void act_dist_select_005DA5C0(em);','static void act_dist_select_005DA5C0(EMW *em);')
open(p,'w').write(s)

s=open(p).read()
s=s.replace('M2C_FIELD(arg1, s8 *, 0x1B) = GetYouganHit(em->pos);','w->x1B = GetYouganHit(em->pos);')
s=re.sub(r'(kyusyu_char_set2_005E64A0)\(em, temp_a1\)',r'\1(em)',s)
m=re.search(r'(?m)^static void em_demo00_005DFDF0\(EMW \*em, EM17W \*w\) \{$',s)
j=s.index('\n}\n',m.end())+3
s=s[:m.start()]+'''static void em_demo00_005DFDF0(EMW *em, EM17W *w) {
    f32 dst[3];
    f32 a120[3];
    f32 out110[3];
    s32 ang[3];
    f32 vF0[3];
    f32 outE0[3];
    FLMAT m;
    EMW *temp_s1;
    STAGE_DATA *temp_s0;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    s32 temp_v1_2;
    s32 temp_v1_3;
    u16 temp_v1;
    u8 temp_a1;
    u8 temp_v1_4;

    temp_s1 = em->x944;
    temp_s0 = Stage_data_get(em->stg);
    if (temp_s1 == 0) {
        SetVector(dst, 5000.0f, 0.0f, 5000.0f);
    } else {
        SetVector(dst, temp_s1->pos[0], temp_s1->pos[1], temp_s1->pos[2]);
    }
    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->tgt_pos[0] = 3000.0f;
        em->tgt_pos[1] = 2000.0f;
        em->tgt_pos[2] = 9000.0f;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16)(w->dang - em->ang[1]);
        em17_senkai_sub(em, 3, 1);
        temp_f1 = em->adj_z;
        if (temp_f1 > 100.0f) {
            em->adj_z = temp_f1 - 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05 += 1;
            em->work08 = 0x384;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    case 2:
        temp_f1_2 = em->adj_z;
        if (temp_f1_2 > 80.0f) {
            em->adj_z = temp_f1_2 - 1.0f;
        }
        w->dang = Em_Calc_angY(em->pos, dst);
        w->dang = (u16)(w->dang - em->ang[1]);
        em17_senkai_sub(em, 3, 1);
        temp_v1 = w->dang;
        if (temp_v1 >= 0x801 && temp_v1 < 0xF800) {
        } else if (CalcDistanceXZ(em->pos, dst) > 4000.0f) {
            em->x05 += 1;
            w->turn = 0x100;
            em_char_set(em, 0x2C, 0, 0);
            em->work08 = 0x708;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0 && em->x05 == 2) {
            em->x05 = 0;
        }
        break;
    case 3:
        temp_v1_3 = em->ang[2];
        if (temp_v1_3 != 0) {
            if (temp_v1_3 < 0x8001) {
                em->ang[2] = temp_v1_3 - w->bank_spd;
            } else {
                em->ang[2] = temp_v1_3 + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f) != 0) {
            temp_f1_3 = em->adj_z;
            if (temp_f1_3 > 50.0f) {
                em->adj_z = temp_f1_3 - 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x708;
        }
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 1.0f;
        break;
    case 4:
        if (em->pos[1] <= 1000.0f + temp_s0->floor_y) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0x708;
        }
        if (kyusyu_char_set2_005E64A0(em) != 0) {
            temp_v1_4 = em->x05;
            if (temp_v1_4 == 4) {
                em->x05 = temp_v1_4 + 1;
                em->work08 = 0x708;
            }
        }
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, -3000.0f, 0.0f);
        speed_add(em, w->spd);
        break;
    case 5:
        kyusyu_char_set2_005E64A0(em);
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        if (!(200.0f + em->tgt_pos[1] < em->pos[1])) {
            xang_calc_target(em, w->spd, 0.0f, 0.0f);
        } else {
            xang_calc_target(em, w->spd, -2000.0f, 0.0f);
        }
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, dst) <= 600.0f || (em->char0 == 0x415 && (em_frame_check2(em, 0, 52.0f) != 0 || em->x194 == 0))) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 4, 0);
            if (temp_s1 != 0) {
                ang[0] = 0;
                ang[1] = em->ang[1];
                ang[2] = 0;
                SetVector(a120, 0.0f, 0.0f, 600.0f);
                cpRotMatrixYXZ2(ang, &m);
                flvecApplyMat33(out110, a120, &m[0][0]);
                temp_s1->pos[0] = em->pos[0] + out110[0];
                temp_s1->pos[2] = em->pos[2] + out110[2];
                if (temp_s1->mode != 6 || temp_s1->x15 != 1) {
                    em_act_set(temp_s1, 6, 1);
                    temp_s1->ang[1] = (em->ang[1] - 0x4000) & 0xFFFF;
                }
            }
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x23, 0, 0);
            if (temp_s1 != 0) {
                em_act_set(temp_s1, 6, 2);
            }
        }
        if (temp_s1 != 0) {
            ang[0] = 0;
            ang[1] = em->ang[1];
            ang[2] = 0;
            SetVector(vF0, 0.0f, 0.0f, 600.0f);
            cpRotMatrixYXZ2(ang, &m);
            flvecApplyMat33(outE0, vF0, &m[0][0]);
            temp_s1->pos[0] = em->pos[0] + outE0[0];
            temp_s1->pos[2] = em->pos[2] + outE0[2];
            temp_s1->x40E = 5;
        }
        break;
    case 7:
        if (em_frame_check(em, 0, 60.0f) != 0 || em_frame_check(em, 0, 202.0f) != 0) {
            Eft13_set_em(em, 0x1C, 6);
        }
        if (em_frame_check(em, 0, 52.0f) != 0 || em_frame_check(em, 0, 208.0f) != 0) {
            Eft13_set_em(em, 0x16, 6);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x4F, 0, 0);
            if (temp_s1 != 0) {
                em_act_set(temp_s1, 6, 3);
            }
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x3B, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 1, 0, 0);
            if (temp_s1 != 0) {
                temp_s1->act_spd = 0.0f;
            }
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x14, 0, 0);
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_4 = em->x5AC;
    if (em->pos[1] < temp_f1_4) {
        em->pos[1] = temp_f1_4;
    }
}
'''+s[j:]
open(p,'w').write(s)

s=open(p).read()
m=re.search(r'(?m)^static void sound_call_005E1FF0\([^\n]*\) \{$',s)
j=s.index('\n}\n',m.end())+3
s=s[:m.start()]+'''static void sound_call_005E1FF0(EMW *em, int frame, int se, int joint) {
    EM17W *w = (EM17W *)em->ex;
    int add;

    add = 0;
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        if (w->x1B == 1) {
            if (joint != 0x14) {
                if (joint == 0x1A) {
                    add = 0x35;
                }
            } else {
                add = 0x35;
            }
        }
        sound_call_sub_005E1F80(em, se + add, joint);
    }
}
'''+s[j:]
s=re.sub(r'(?m)^static void sound_call_005E1FF0\([^\n]*\);$','static void sound_call_005E1FF0(EMW *em, int frame, int se, int joint);',s,count=1)
open(p,'w').write(s)

s=open(p).read()
m=re.search(r'(?m)^void em17_init\(EMW \*em\) \{$',s)
j=s.index('\n}\n',m.end())+3
cases=[(0,8000,0,6000,None),(15,7900,1050,13900,None),(18,10000,200,8000,None),(22,9900,300,10350,None),(24,10000,0,5700,'em->ang[1] = 0x4000;'),(27,14300,0,7100,None),(37,9750,300,7750,None),(40,10500,0,9500,None)]
body=''
for st,x,y,z,extra in cases:
    body+='        case %d:\n            em->pos[0] = %d.0f + 1500.0f * (f32)(u32)em->x13;\n            em->pos[1] = %d.0f;\n            em->pos[2] = %d.0f;\n'%(st,x,y,z)
    if extra: body+='            %s\n'%extra
    body+='            break;\n'
s=s[:m.start()]+'''void em17_init(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    s16 temp_v0;
    u8 temp_a0;
    u8 temp_a1;

    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
'''+body+'''        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em->mode = 0;
    em->x15 = 0;
    if (em->kind == 0x11) {
        em_char_set(em, 1, 0, 0);
        em->x388 = 0;
        em17_act_set(em, 0, 1, 0);
        temp_v0 = em_hp_vital_set2(em, 0x4EC, 0x4D8);
        em->x302 = temp_v0;
        em->x792 = temp_v0;
        em->x839 = 1;
    } else {
        em_char_set(em, 0x6B, 0, 0);
        em->x388 = 0;
        em17_act_set(em, 0, 0x16, 0);
        temp_v0 = em_hp_vital_set2(em, 0x2A8, 0x334);
        em->x302 = temp_v0;
        em->x792 = temp_v0;
        em->x839 = 0;
    }
    w->x06 = 0;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em17_stay_timer_tbl[em->stg];
    em->runaway_tm = em17_runaway_timer_tbl[em->stg];
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
'''+s[j:]
if 'void eft09_set(EMW *, int);' not in s: s=s.replace('void em_act_set(EMW *, int, u16);','void em_act_set(EMW *, int, u16);\nvoid eft09_set(EMW *, int);',1)
s=re.sub(r'(?m)^[^\n]*void eft09_set\(EMW \*, [^\n]*\n','',s) if False else s
open(p,'w').write(s)
