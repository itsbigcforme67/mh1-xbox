/* em_master_b - game.bin 0x00539D00-0x0053B6D0: monster status helpers
 * (sleep/paralysis start and end, hunger/thirst/sleepiness counters,
 * no-floor and battle-area checks, hit points by quest and online mode,
 * attack/defence multipliers). Second matching run of the em_master file
 * (see em_master_nm.c). Meanings are guesses. */
#include "em_sys.h"
#include "game.h"
#include "pl.h"
#include "fl.h"

int Pl_stg_ck_tw(EMW *, PLW *);
void net_send_em(EMW *, int, int, u8);

void Em_Master_Change(EMW *em);

/* Call-for-help points ("yobi"): up to 32 entries; an entry near the
 * monster on its stage that suits its kind sets em->x9C8 to its position. */
typedef struct EM_YOBI {
    f32 pos[3];         /* 0x00 */
    f32 range;          /* 0x0C */
    u8 _pad10[2];
    u8 type;            /* 0x12 0: kinds 16/27, 1: kind 9, 2: kinds 12/25 */
    u8 _pad13;
    u8 stg;             /* 0x14 */
} EM_YOBI;

extern s8 em_yobi_cnt;
extern EM_YOBI *em_yobi_stack[32];
extern s16 *em_ikari_data_tbl[];
extern u16 em_wall_val_tbl[];

f32 flvecCalcDistance(f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
void Em_Mode_Chg(EMW *, int, int, s32);
void Em_Sleep_End(EMW *em);
void Em_Sleep2_End(EMW *em);
void Em_Mahi_End(EMW *em);

int Em_Yobi_Ck(EMW *em, EM_YOBI *self);

void Ikari_Data_Set(EMW *em);

void Em_Taisei_Set(EMW *em);

void Em_Sleep_Start(EMW *em) {
    int i;
    s32 t;

    if (em->taisei & 8) {
        Em_Mahi_End(em);
    }
    if (em->taisei & 1) {
        Em_Sleep2_End(em);
    }
    for (i = 0; i < game_w.pl_num; i++) {
        em->x918[i] = 0;
        em->x8F4[i] = 0;
    }
    em->x9E8 = 0;
    em->x7B2 = 0;
    em->x7B6 = 0;
    t = em->x8AC;
    em->x8A0 = t;
    em->x762 = 2;
    em->x7D2 = 2;
    em->taisei |= 2;
    em->x8BD = 1;
    Em_Mode_Chg(em, 0, 0, t);
}

void Em_Sleep2_Start(EMW *em) {
    int i;
    s32 t;

    if (em->taisei & 8) {
        Em_Mahi_End(em);
    }
    if (em->taisei & 2) {
        Em_Sleep_End(em);
    }
    for (i = 0; i < game_w.pl_num; i++) {
        em->x918[i] = 0;
        em->x8F4[i] = 0;
    }
    em->x9E8 = 0;
    em->x7CC = 0;
    em->x7D0 = 0;
    t = em->x8AC;
    em->x8A0 = t;
    em->x762 = 2;
    em->x7D2 = 1;
    em->taisei |= 1;
    em->x8BD = 1;
    Em_Mode_Chg(em, 0, 0, t);
}

void Em_Sleep_End(EMW *em) {
    EM_TAISEI_DATA *d = em_sleep_data_tbl[em->kind];

    em->x762 = 0;
    em->taisei &= 0xFC;
    em->x7D2 = 0;
    em->x8BD = 0;
    em->x9E8 = 0;
    em->x88B = 1;
    if (em->x7EF < 3) {
        em->x7EF++;
        em->sleep_tol += d->add;
    }
}

void Em_Sleep2_End(EMW *em) {
    EM_TAISEI_DATA *d = em_sleep2_data_tbl[em->kind];

    em->x762 = 0;
    em->taisei &= 0xFC;
    em->x7D2 = 0;
    em->x8BD = 0;
    em->x9E8 = 0;
    em->x88B = 1;
    if (em->x7F2 < 3) {
        em->x7F2++;
        em->sleep2_tol += d->add;
    }
}

void Em_Mahi_Start(EMW *em) {
    em->x762 = 4;
    em->x7D2 = 4;
    em->taisei |= 8;
    em->x8BD = 1;
}

void Em_Mahi_End(EMW *em) {
    EM_TAISEI_DATA *d = em_mahi_data_tbl[em->kind];

    em->x7C4 = 0;
    em->x7C8 = 0;
    em->taisei &= 0xF7;
    em->x7D2 = 0;
    em->x8BD = 0;
    em->x762 = 0;
    if (em->x7F1 < 3) {
        em->x7F1++;
        em->mahi_tol += d->add;
    }
}

void Em_Sleep_Flag_Ck(EMW *em) {
    if (em->x7D2 == 1) {
        Em_Sleep2_End(em);
    }
    if (em->x7D2 == 2) {
        Em_Sleep_End(em);
    }
    if (em->taisei & 8) {
        Em_Mahi_End(em);
    }
}

void Em_Sleep_Flag_Ck2(EMW *em) {
    if (em->x7D2 == 1) {
        Em_Sleep2_End(em);
    }
    if (em->x7D2 == 2) {
        Em_Sleep_End(em);
    }
}

typedef f32 (*EM_POSP)[3];

EM_STG_POS *gp_ck(EMW *em, EM_STG_POS *p, s16 stg) {
    s16 i;

    for (i = 0; i < 88; i++, p++) {
        if (p->stg == -1 || p->stg == stg) {
            return p;
        }
    }
    return 0;
}

EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p) {
    s16 i;

    for (i = 0; i < 88; i++, p++) {
        if (p->stg == -1 || p->stg == em->stg) {
            return p->pos;
        }
    }
    return 0;
}

void em_wall_bit_set(EMW *em) {
    em->x95E = em_wall_val_tbl[em->kind];
}

/* em_stage_move_ok_data[kind][x2E]: rows {s16 quest; ...; rows2 at +4},
 * rows2 {u8 stg; ...; result at +4}. */
typedef struct EM_MVOK2 {
    u8 stg;
    u8 _pad1[3];
    s32 ok;
} EM_MVOK2;

typedef struct EM_MVOK {
    s16 quest;
    u8 _pad2[2];
    EM_MVOK2 *rows;
} EM_MVOK;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest */
    u8 _pad0A[0x14E - 0xA];
    s8 x14E;            /* 0x14E 1: harder settings (more hit points, attack x1.5-1.7) */
} QUEST_W;

extern QUEST_W quest_w;
extern EMW em_work[];
extern u8 em_tsuushin_tbl[];
extern u8 em_boss_tbl[];
extern u8 em_pl_find_action_tbl[];
extern u8 em_action_priority_tbl[][8];
extern s16 *em_nest_tbl[];
extern f32 enemy_mahi_size[];
extern EM_MVOK **em_stage_move_ok_data[];

void poison_stock_set(EMW *, s16);
void sleep_stock_set(EMW *, s16);
void mahi_stock_set(EMW *, s16);
void Eft06_set2(EMW *, int, u8, int);
void Eft06_set(f32, EMW *, int, int, u8);
int em_cancel_act_ck(EMW *, int);

void em_boss_work_set(EMW *em) {
    s8 i;
    EMW *e = em_work;

    em->boss = 0;
    switch (em->kind) {
    case 16:
    case 13:
    case 30:
        for (i = 0; i < 20; i++, e++) {
            if (e->kind == 27 && e->be_flag != 0) {
                em->boss = e;
                return;
            }
        }
        break;
    }
}

void smell_dmg_set(EMW *em, EM_SMELL *s) {
    switch (s->type) {
    case 0:
        return;
    case 1:
        poison_stock_set(em, s->val);
        break;
    case 2:
        sleep_stock_set(em, s->val);
        break;
    case 3:
        mahi_stock_set(em, s->val);
        break;
    }
}

void em_tsuushin_set(EMW *em) {
    em->x9E2 = em_tsuushin_tbl[em->kind];
    em->x9E9 = em_boss_tbl[em->kind];
    em->x9EC = em_pl_find_action_tbl[em->kind];
}

void em_hinshi_end(EMW *em) {
    em->x9E4 = 0;
}

void em_hinshi_end2(EMW *em) {
    s16 *p;

    if (em->x889 == 1 && em->x88A == em_action_priority_tbl[em->kind][0]) {
        p = em_nest_tbl[em->kind];
        if (p != 0) {
            if (em->x888 == 1 && *p != -1) {
                do {
                    if (*p++ == em->stg) {
                        em->x9E4 = 0;
                        em->x889 = 0;
                        em->x88A = 0;
                        em->x9E3 = 0;
                        break;
                    }
                } while (*p != -1);
            }
        } else {
            em->x9E4 = 0;
            em->x889 = 0;
            em->x88A = 0;
            em->x9E3 = 0;
        }
    }
}

void em_hungry_end(EMW *em) {
    em->x9E6 = 0;
    em->x8C1 = 0;
    em->x762 = 0;
}

void em_thirst_end(EMW *em) {
    em->x9E7 = 0;
    em->x8C0 = 0;
    em->x762 = 0;
}

void em_suimin_end(EMW *em) {
    em->x8A0 = em->x8AC;
    em->x9E8 = 0;
    em->x7B2 = 0;
    em->x7B6 = 0;
    em->x7CC = 0;
    em->x7D0 = 0;
    em->x762 = 0;
}

void Em_Suimin_Start(EMW *em) {
    int i;
    s32 t;

    for (i = 0; i < game_w.pl_num; i++) {
        em->x918[i] = 0;
        em->x8F4[i] = 0;
    }
    em->x9E8 = 0;
    em->x7B2 = 0;
    em->x7B6 = 0;
    t = em->x8AC;
    em->x8A0 = t;
    em->x762 = 2;
    em->x8BD = 1;
    Em_Mode_Chg(em, 0, 0, t);
}

void em_sleep_eff_set(EMW *em, int a, int b) {
    u16 t = *(u16 *)&game_w.x1E % 90;

    if (t == 0 || t == 10 || t == 20) {
        Eft06_set2(em, 4, a, b);
    }
}

void em_mahi_eff_set(EMW *em, int a) {
    if (!(*(u16 *)&game_w.x1E & 0x1F)) {
        Eft06_set(em->scale[0] * enemy_mahi_size[em->kind], em, 5, 0, a);
    }
}

s32 st_mv_ptr_ck(EMW *em) {
    EM_MVOK *p = em_stage_move_ok_data[em->kind][game_w.x2E];
    EM_MVOK2 *q;
    s16 i;

    for (i = 0; i < 178; i++, p++) {
        if (p->quest == -1 || p->quest == quest_w.no) {
            q = p->rows;
            break;
        }
    }
    for (i = 0; i < 88; i++, q++) {
        if (q->stg == 0xFF || q->stg == em->stg) {
            return q->ok;
        }
    }
    return 0;
}

void em_hungry_add(EMW *em, s32 n) {
    em->hungry += n;
    if (em->hungry_max < em->hungry) {
        em->hungry = em->hungry_max;
    }
}

void em_thirst_add(EMW *em, s32 n) {
    em->thirst += n;
    if (em->thirst_max < em->thirst) {
        em->thirst = em->thirst_max;
    }
}

void em_suimin_add(EMW *em, s32 n) {
    em->x8A0 += n;
    if (em->x8AC < em->x8A0) {
        em->x8A0 = em->x8AC;
    }
}

void em_no_floor_ck(EMW *em) {
    if (em->x8C3 == 0 && game_w.stage == em->stg) {
        if (em->x8BA > 0) {
            if (--em->x8BA <= 0) {
                em->x8BA = 0;
            }
            return;
        }
        if (em->x388 == 2 || em->x388 == 4) {
            return;
        }
        if (em->mode == 5 || em->mode == 6 || em->mode == 7) {
            return;
        }
        if ((em->x70E == 0 || (em->x70E & 0x8000)) && em_cancel_act_ck(em, 0x80) == 0 && em->x8C2 == 0 && em->x8BD == 0) {
            em->x84E = 1;
            em->x917 |= 0x80;
        }
    }
}

void em_no_floor_ck2(EMW *em) {
    if (em->x8C3 == 0 && game_w.stage == em->stg) {
        if (em->x8BA > 0) {
            if (--em->x8BA <= 0) {
                em->x8BA = 0;
            }
            return;
        }
        if (em->x388 == 2 || em->x388 == 4) {
            return;
        }
        if (em->mode == 5 || em->mode == 6 || em->mode == 7) {
            return;
        }
        if (em->x70E == 0 && em_cancel_act_ck(em, 0x80) == 0 && em->x8C2 == 0 && em->x8BD == 0) {
            em->x84E = 1;
            em->x917 |= 0x80;
        }
    }
}

void em_no_battle_area_ck(EMW *em, u16 mode, u16 sub) {
    s8 i;
    PLW *pl;

    em->x9F1 = 0;
    if (em->x888 != 1) {
        return;
    }
    for (i = 0; i < game_w.pl_num; i++) {
        pl = &player_work[i];
        if (Pl_stg_ck_tw(em, pl) && pl->be_flag) {
            return;
        }
    }
    if (em->mode != mode || em->x15 != sub) {
        em->x9F1 = 1;
        em->pos[0] = em->x5A0[0];
        em->pos[2] = em->x5A0[2];
    }
}

/* hagitori_eft_tbl: effect when a breakable part levels up. */
typedef struct EM_HAGI_EFT {
    u8 kind;            /* 0x0 */
    u8 part;            /* 0x1 */
    u8 lv;              /* 0x2 */
    u8 joint;           /* 0x3 */
    f32 ofs[3];         /* 0x4 */
} EM_HAGI_EFT;

extern s16 em_hp_vital_tbl[];
extern EM_HAGI_EFT hagitori_eft_tbl[15];

int Online_ck();
void Em_Mode_Chg();
void cmd_target_kind_set(EMW *, f32 *);
int Em_stg_ck(EMW *);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void Eft02_set3(f32, EMW *, int, int, int, f32 *);
void em_char_set2(EMW *, int, int, int, int);
void cpRotMatrixYXZ2(s32 *, void *);
void frame_move(EMW *);
void enemy_mk(EMW *);
void tail_off(struct EFTW *);
EM_SMELL *smell_ptr_ret(EMW *);
void em_hp_add(EMW *em, s16 n);

int em_mode_timer_sub(EMW *em) {
    if (em->x888 == 1 && em->x8C3 == 0) {
        if (em->x88F == 0 && em->x886 > 900) {
            em->x886 = 900;
        }
        if (--em->x886 <= 0) {
            if (em->x388 == 2) {
                em->x886 = 0;
            } else {
                Em_Mode_Chg(em, 0, 0);
                return 1;
            }
        }
    }
    return 0;
}

void em_hp_add(EMW *em, s16 n) {
    em->x302 += n;
    if (em->x302 > em->x792 || em->x302 < 0) {
        em->x302 = em->x792;
    }
}

int em_target_pl_samestage_ck(EMW *em) {
    PLW *pl = player_work;
    s8 i;

    if (em->x8C3 != 0) {
        return 1;
    }
    if (em->x617 == -1) {
        return 0;
    }
    for (i = 0; i < game_w.pl_num; i++, pl++) {
        if ((u8)Pl_stg_ck_tw(em, pl) == 0 || pl->be_flag == 0) {
            return 0;
        }
    }
    return 1;
}

s16 em_hp_vital_set(EMW *em, s16 v) {
    s16 hp = v;

    switch (em->kind) {
    case 3:
    case 4:
    case 9:
    case 12:
    case 19:
    case 23:
    case 24:
        break;
    case 5:
    case 13:
    case 16:
    case 25:
    case 30:
    case 34:
        if (Online_ck() == 1) {
            hp = v / 8 * 10;
        }
        if (quest_w.x14E == 1) {
            hp = 1.3f * hp;
        }
        break;
    case 7:
        if (Online_ck() == 1) {
            hp = hp * 2;
        }
        if (quest_w.x14E == 1) {
            hp = 1.2f * hp;
        }
        break;
    default:
        if (Online_ck() == 1) {
            hp = hp * 2;
        }
        if (quest_w.x14E == 1) {
            hp = 1.4f * hp;
        }
        break;
    }
    return hp;
}

s16 em_hp_vital_set2(EMW *em, s16 a, s16 b) {
    s16 v = a + b;
    s16 hp = v;
    s16 min = em_hp_vital_tbl[em->kind];

    switch (em->kind) {
    case 3:
    case 4:
    case 9:
    case 12:
    case 19:
    case 23:
    case 24:
        break;
    case 5:
    case 13:
    case 16:
    case 25:
    case 30:
    case 34:
        if (v != min && v < min) {
            v = min;
            hp = min;
        }
        if (Online_ck() == 1) {
            hp = v / 8 * 10;
        }
        if (quest_w.x14E == 1) {
            hp = 1.3f * hp;
        }
        break;
    case 7:
        if (v != min && v < min) {
            v = min;
            hp = min;
        }
        if (Online_ck() == 1) {
            hp = v * 2;
        }
        if (quest_w.x14E == 1) {
            hp = 1.2f * hp;
        }
        break;
    default:
        if (v != min && v < min) {
            v = min;
            hp = min;
        }
        if (Online_ck() == 1) {
            hp = v * 2;
        }
        if (quest_w.x14E == 1) {
            hp = 1.5f * hp;
        }
        break;
    }
    return hp;
}

f32 em_def_attack_set(EMW *em) {
    f32 f = 1.0f;

    if (quest_w.x14E == 1) {
        switch (em->kind) {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 22:
        case 26:
        case 27:
        case 28:
        case 31:
        case 33:
            f = 1.7f;
            break;
        case 5:
        case 13:
        case 16:
        case 25:
        case 30:
        case 34:
            f = 1.5f;
            break;
        }
    } else if (Online_ck(1) == 1) {
        switch (em->kind) {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 22:
        case 26:
        case 27:
        case 28:
        case 31:
        case 33:
            f = 1.1f;
            break;
        case 5:
        case 13:
        case 16:
        case 25:
        case 30:
        case 34:
            f = 1.05f;
            break;
        }
    }
    return f;
}

f32 em_def_defence_set(EMW *em) {
    f32 f = 1.0f;

    if (quest_w.x14E == 1) {
        switch (em->kind) {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 22:
        case 26:
        case 27:
        case 28:
        case 31:
        case 33:
        case 5:
        case 13:
        case 16:
        case 25:
        case 30:
        case 34:
            f = 0.9f;
            break;
        }
    } else if (Online_ck(1) == 1) {
        switch (em->kind) {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 22:
        case 26:
        case 27:
        case 28:
        case 31:
        case 33:
            f = 0.95f;
            break;
        }
    }
    return f;
}

void em_ana_loop_cnt_set(EMW *em) {
    if (em->x8C3 == 0) {
        em->x827 = 12;
        em->x828 = 0;
        em->x829 = em->x95A;
        cmd_target_kind_set(em, em->tgt_pos);
    }
}

int em_hokaku_ck(EMW *em, f32 rate) {
    return (s16)(em->x792 * rate) >= em->x302;
}

int Bdora_hp_ck(void) {
    EMW *e = em_work;
    s8 i;

    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0 && e->kind == 7) {
            if (e->x302 <= e->x792 / 2) {
                return 1;
            }
        }
    }
    return 0;
}

int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step) {
    if (step != 0 && *(u16 *)&game_w.x1E % step != 0) {
        return 0;
    }
    if (em->x302 >= max) {
        return 0;
    }
    em->x302 += n;
    if (em->x302 >= max) {
        em->x302 = max;
        return 1;
    }
    return 0;
}

void em_hagitori_lv_up(EMW *em, u8 bits);

void em_tail_off_sub(EMW *em);

void em_g_init_flag_set(EMW *em, u8 f);

void em_niku_eat_set(EMW *em);

