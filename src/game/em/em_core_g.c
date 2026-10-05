/* em_core_g - game.bin 0x00536C80-0x0053948C: target selection, kehai, area/escape/need checks, net_act_set, smoke/senko checks. Matching run of the f_em file
 * (whole file in em_core_nm.c). Meanings of fields are guesses. */
#include "em_sys.h"
#include "game.h"
#include "pl.h"
#include "fl.h"

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest */
    u8 _pad0A[0x14E - 0xA];
    s8 x14E;            /* 0x14E */
} QUEST_W;

extern QUEST_W quest_w;
extern s8 senko_cnt;
extern s8 smoke_cnt;
extern s8 smell_cnt;
extern EM_SPOT *senko_stack[32];
extern EM_SPOT *smoke_stack[32];
extern EM_SEARCH *em_search_tbl[];
extern f32 D_3E4C9C[3];

f32 flvecCalcDistance(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void hit_line2_pk(f32 *, f32 *, void *);
int hit_line_sphr2(void *, EM_SPOT *, f32);
void cpRotMatrix(s32 *, void *);
void frame_init(EMW *, u16, s16, int);
int em_pl_pos_set(EMW *, u8, f32 *);
void em_neck_move_sub(EMW *em, f32 *tgt, int on);

FLMAT *get_joint_wmat_em(EMW *, int);
u16 calc_mat_angY(FLMAT *);
void get_joint_pos_em(EMW *, int, f32 *);
int GetEyeHitLine(EMW *, f32 *, f32 *, f32 *, int);
void SetVector(f32 *, f32, f32, f32);
void senko_ck(EMW *em, int pl, EM_EYE *e);
int smoke_ck(EMW *em, EM_EYE *e);
void Em_Hate_Add(EMW *em, s32 add, s32 max, u8 pl);
s32 *em_hate_suu_set(EMW *em, u8 type, u8 pl);
int Pl_stg_ck_tw(EMW *, PLW *);

s8 em_eye_search_set(EMW *em);

void senko_ck(EMW *em, int pl, EM_EYE *e);

int smoke_ck(EMW *em, EM_EYE *e);

void em_search_data_set(EMW *em, u8 no);

/* mot_data_tbl[kind][no] (12 bytes): one animation request. */
typedef struct EM_MOT {
    u8 no;              /* 0x0 animation number (+1000 + layer * 200) */
    s8 layer;           /* 0x1 0-2: that layer only, 16-18: all but one, else all */
    u8 _pad2;
    u8 ex;              /* 0x3 exmot_data_tbl entry used when already playing */
    s16 tm;             /* 0x4 */
    s8 blend;           /* 0x6 */
    s8 next_blend;      /* 0x7 stored to EMW.x416 */
    u8 can;             /* 0x8 canmot_data_tbl entry when EMW.x6FE is set */
    u8 _pad9[3];
} EM_MOT;

extern EM_MOT *mot_data_tbl[];
extern EM_MOT *canmot_data_tbl[];
extern EM_MOT *exmot_data_tbl[];
void cpRotMatrixYXZ2(s32 *, FLMAT *);

void em_char_set(EMW *em, int no, int blend, s16 tm);

void em_char_set2(EMW *em, int ch, int blend, u16 tm, int n);

/* em_neck_tbl[kind]: neck turning data. */
typedef struct EM_NECK {
    f32 fwd;            /* 0x00 head offset ahead of the body (em_neck_move_sub) */
    u8 _pad04[6];
    u16 spd;            /* 0x0A base turn speed */
    u16 spd_max;        /* 0x0C */
    u16 spd_add;        /* 0x0E */
    u16 lim[4];         /* 0x10 per-joint limits */
    u16 range;          /* 0x18 */
} EM_NECK;

extern EM_NECK *em_neck_tbl[];

void em_neck_move(EMW *em);

void neck_ang_set(EMW *em, u16 spd, u16 ang);


typedef struct EM_MOTW {
    u8 _pad00[0xD0];
    s32 xD0;            /* 0xD0 */
} EM_MOTW;

typedef struct EM_HUNGRY {
    u8 _pad00[8];
    s32 smell;          /* 0x8 hunger level below which smells are followed */
    s32 dec;            /* 0xC drop per frame when idle */
    s32 dec2;           /* 0x10 drop per frame when x888 is set */
} EM_HUNGRY;

typedef struct EM_ACTRATE {
    u16 rate;           /* 0x0 weight, 0xFFFF ends the list */
    u16 act;            /* 0x2 */
} EM_ACTRATE;

extern EM_SMELL *smell_stack[32];
extern EM_HUNGRY *em_hungry_tbl[];

void get_joint_pos_em(EMW *, int, f32 *);
int Pl_stg_ck_tw(EMW *, PLW *);
int GetWallHitLine(f32 *, f32 *, f32 *, u16);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void act_set(EMW *, int, u16);
u16 ran_suu(int);
int Online_ck(void);
void net_act_set(EMW *, int, u16, int);
void calc_ofs_velocity(f32, f32, f32 *, s32);
void calc_velocity(f32 *, f32 *, f32 *, f32, f32);
f32 plFCVFcurveInterpolateHermite(f32, f32, f32, f32, f32, f32, f32);

s8 smell_search(EMW *em, int joint, f32 *out);

int smell_ck(EMW *em, int joint);

EM_SMELL *smell_ptr_ret(EMW *em);

void speed_add(EMW *em, s32 *ang);

void speed_add_g(EMW *em, s32 *ang);

void em01_act_set(EMW *em, int kind, u16 no, u16 arg);
void em02_act_set(EMW *em, int kind, u16 no, u16 arg);
void em03_act_set(EMW *em, int kind, u16 no, u16 arg);
void em04_act_set(EMW *em, int kind, u16 no, u16 arg);
void em20_act_set(EMW *em, int kind, u16 no, u16 arg);
void em07_act_set(EMW *em, int kind, u16 no, u16 arg);
void em08_act_set(EMW *em, int kind, u16 no, u16 arg);
void em09_act_set(EMW *em, int kind, u16 no, u16 arg);
void em12_act_set(EMW *em, int kind, u16 no, u16 arg);
void em16_act_set(EMW *em, int kind, u16 no, u16 arg);
void em14_act_set(EMW *em, int kind, u16 no, u16 arg);
void em15_act_set(EMW *em, int kind, u16 no, u16 arg);
void em17_act_set(EMW *em, int kind, u16 no, u16 arg);
void em21_act_set(EMW *em, int kind, u16 no, u16 arg);
void em27_act_set(EMW *em, int kind, u16 no, u16 arg);
void em29_act_set(EMW *em, int kind, u16 no, u16 arg);
void em19_act_set(EMW *em, int kind, u16 no, u16 arg);
void em33_act_set(EMW *em, int kind, u16 no, u16 arg);
void cmd_target_kind_set(EMW *em, f32 *pos);
void target_kind_set(EMW *em, f32 *pos);
void em_act_set(EMW *em, int kind, u16 no);

void em_type_act_set(EMW *em, int kind, u16 no, u16 arg);

void em_act_set_sub(EMW *em, int kind, u16 no);

void em_act_set(EMW *em, int kind, u16 no);

void em_act_set2(EMW *em, int kind, u16 no, u8 mode);

void mot_miration_ret(EMW *em, f32 *v);

u16 em_act_search(EM_ACTRATE *tbl);

typedef struct EM_IKARI_DATA {
    s16 max;            /* 0x00 anger needed */
    s16 time;           /* 0x02 length of the angry state */
    u8 _pad04[4];
    f32 atk;            /* 0x08 */
    f32 def;            /* 0x0C */
    f32 rate[11];       /* 0x10 anger gain by health band (100%, 90%...) */
} EM_IKARI_DATA;

typedef struct EM_HATE_SUB {
    s32 x0;             /* 0x0 hate lost per frame for the target in sight */
    s32 x4;             /* 0x4 ... for others in sight */
    s32 x8;             /* 0x8 target out of sight */
    s32 xC;             /* 0xC others out of sight */
} EM_HATE_SUB;

extern EM_IKARI_DATA *em_ikari_data_tbl[];
extern s16 em_ninshiki_timer_tbl[];
extern EM_HATE_SUB *em_hate_sub_tbl[];

f32 em_def_attack_set(EMW *);
f32 em_def_defence_set(EMW *);
s32 *em_hate_suu_set(EMW *em, u8 type, u8 pl);
void Em_Hate_Add(EMW *em, s32 add, s32 max, u8 pl);
int pl_flag_ck(PLW *, u32);

void em_ikari_add(EMW *em, s16 n);

void ikari_flag_set(EMW *em);

void Em_Damage_Hate_Set(EMW *em);

void Em_Hate_Add(EMW *em, s32 add, s32 max, u8 pl);

void Em_Hate_Ck(EMW *em);

u8 pl_status_ret(int n);

/* em_kehai_add_tbl[kind]: base add per sound kind, then per player status. */
typedef struct EM_KEHAI_ADD {
    s32 *base;          /* 0x00 [kind] */
    s32 (*st[4])[4];    /* 0x04 [status][kind] for levels 0..3 */
} EM_KEHAI_ADD;

extern EM_KEHAI_ADD *em_kehai_add_tbl[];
extern s32 *em_max_kehai_hate_tbl[];
int Pl_Skill_ck(PLW *, int);
int em_cancel_act_ck(EMW *, u8);

void Em_Kehai_Hate_Add(EMW *em, int pl0, u8 kind, s8 lv);

extern f32 (*em_range_data_tbl[])[2];
extern u16 (*em_range_ang_data_tbl[])[2];

void em_range_set(EMW *em, s8 no);

typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 w;              /* 0x10 stage size x */
    f32 d;              /* 0x14 stage size z */
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;

/* em->area->x18 lists, by stage: routes of points. */
typedef struct EM_ROUTE_PT {
    f32 pos[3];         /* 0x00 */
    u8 _pad0C[8];
} EM_ROUTE_PT;

typedef struct EM_ROUTE {
    u8 _pad00[2];
    s16 n;              /* 0x02 point count (read from the first route only) */
    EM_ROUTE_PT *pt;    /* 0x04 */
    u8 _pad08[0x10];
} EM_ROUTE;

extern f32 (*em_cmd_pos_tbl[])[3];
STAGE_DATA *Stage_data_get(u8);
void NextStage_No_Set(void);
EM_STG_POS *gp_ck(EMW *em, EM_STG_POS *p, s16 stg);
typedef f32 (*EM_POSP)[3];
EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);

/* Target position for a commanded monster (x827 = target kind, x828/x829
 * its arguments). */
void cmd_target_kind_set(EMW *em, f32 *pos) {
    EM_POSP p;
    STAGE_DATA *sd;
    EM_STG_POS *g;
    EM_ROUTE *r;
    EM_ROUTE_PT *pt;
    f32 *q;
    s8 i;
    EM_SMELL *sm;
    u8 found;

    em->x881 = em->x827;
    em->x882 = em->x828;
    em->x883 = em->x829;
    switch (em->x827) {
    case 1:
        switch (em->x882) {
        case 0:
        if (em->x883 == -1) {
            em->x617 = -1;
            em->x3B0 = 0;
        } else {
        if (em->x888 == 1 && em->x844 != -1) {
            em->x829 = em->x844 & 0xF;
            em->x883 = em->x829;
        }
        em->x617 = em->x829;
        em->x3B0 = &player_work[em->x617];
        em_pl_pos_set(em, em->x617, pos);
        }
        }
        break;
    case 2:
        p = gp_ptr_ck(em, (&em->area->x0)[em->x828]);
        switch (em->x828) {
        case 0:
        def0:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = em->pos[1];
                pos[2] = 5000.0f;
            } else {
                pos[0] = p[0][0];
                pos[1] = p[0][1];
                pos[2] = p[0][2];
            }
            break;
        case 1:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = 0.0f;
                pos[2] = 5000.0f;
            } else {
                q = p[em->x829];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            em->x883 = em->x829;
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (em->x829) {
            case 0:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            case 1:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 2:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 3:
            default:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            }
            } else {
                q = p[em->x829];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 3:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x883 = em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 4:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 5:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        em->x73A = em->x829;
        NextStage_No_Set();
        em->x828 = em->x92F;
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[em->x883][0];
        pos[1] = p[em->x883][1];
        pos[2] = p[em->x883][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (em->x882) {
        case 0:
            sc = 1.0f;
            ang = 0;
            break;
        case 1:
            sc = 1.0f;
            ang = 0xC000;
            break;
        case 2:
            sc = 1.0f;
            ang = 0x4000;
            break;
        case 3:
            sc = 1.0f;
            ang = 0x8000;
            break;
        case 8:
            sc = 2.0f;
            ang = 0;
            break;
        case 9:
            sc = 2.0f;
            ang = 0xC000;
            break;
        case 10:
            sc = 2.0f;
            ang = 0x4000;
            break;
        case 11:
            sc = 2.0f;
            ang = 0x8000;
            break;
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 500.0f * sc;
        rot[0] = 0;
        rot[1] = (u16)(em->ang[1] + ang);
        rot[2] = 0;
        cpRotMatrix(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = em->pos[0] + out[0];
        pos[1] = em->pos[1] + out[1];
        pos[2] = em->pos[2] + out[2];
        break;
    }
    case 7:
        found = 0;
        if (smell_cnt != 0) {
            for (i = 0; i < 32; i++) {
                sm = smell_stack[i];
                if (sm != 0 && sm->x10 == em->x882 && sm->x11 == em->x883) {
                    found = 1;
                    pos[0] = sm->pos[0];
                    pos[1] = sm->pos[1];
                    pos[2] = sm->pos[2];
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        p = gp_ptr_ck(em, em->area->x0);
        goto def0;
    case 8:
        pos[0] = em->x9C8[0];
        pos[1] = em->x9C8[1];
        pos[2] = em->x9C8[2];
        break;
    case 9:
        g = gp_ck(em, em->area->x18, em->stg);
        if (g == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        r = (EM_ROUTE *)g->pos;
        if (r == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pt = r[em->x882].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pos[0] = pt[em->x883].pos[0];
        pos[1] = pt[em->x883].pos[1];
        pos[2] = pt[em->x883].pos[2];
        em->x86D = em->x882;
        em->x86E = em->x883;
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = em->x882;
        em->x3B0 = &player_work[em->x617];
        {
            u16 n = em->x3B0->x70E;

            base = gp_ptr_ck(em, em->area->x0)[n];
        }
        {
            u16 a = Em_Calc_angY(base, em->pos);

            rot[0] = 0;
            rot[1] = a;
            rot[2] = 0;
        }
        switch (em->x883) {
        case 0:
            v[0] = 0.0f;
            v[1] = 500.0f;
            v[2] = 1000.0f;
            break;
        case 1:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 800.0f;
            break;
        case 2:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 1500.0f;
            break;
        case 3:
            v[0] = 0.0f;
            v[1] = 600.0f;
            v[2] = 600.0f;
            break;
        }
        cpRotMatrixYXZ2(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = base[0] + out[0];
        pos[1] = base[1] + out[1];
        pos[2] = base[2] + out[2];
        break;
    }
    case 12:
        break;
    }
    em->x881 = em->x827;
    em->x882 = em->x828;
    em->x883 = em->x829;
}

void target_kind_set(EMW *em, f32 *pos) {
    EM_POSP p;
    STAGE_DATA *sd;
    EM_STG_POS *g;
    EM_ROUTE *r;
    EM_ROUTE_PT *pt;
    f32 *q;
    s8 i;
    EM_SMELL *sm;
    u8 found;

    em->x827 = em->x881;
    em->x828 = em->x882;
    em->x829 = em->x883;
    switch (em->x881) {
    case 1:
        switch (em->x882) {
        case 0:
            if (em->x883 == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                em->x617 = em->x883;
                em->x3B0 = &player_work[em->x883];
                em_pl_pos_set(em, em->x617, pos);
                em->x844 = em->x883;
            }
        }
        break;
    case 2:
        p = gp_ptr_ck(em, (&em->area->x0)[em->x882]);
        switch (em->x882) {
        case 0:
        def0:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = em->pos[1];
                pos[2] = 5000.0f;
            } else {
                pos[0] = p[0][0];
                pos[1] = p[0][1];
                pos[2] = p[0][2];
            }
            break;
        case 1:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = 0.0f;
                pos[2] = 5000.0f;
            } else {
                q = p[em->x883];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (em->x883) {
            case 0:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            case 1:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 2:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 3:
            default:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            }
            } else {
                q = p[em->x883];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 3:
        case 4:
        case 5:
            if (p == 0) {
                goto fb;
            }
            q = p[em->x883];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        em->x92F = em->x882;
        em->x73A = em->x883;
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[em->x883][0];
        pos[1] = p[em->x883][1];
        pos[2] = p[em->x883][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (em->x882) {
        case 0:
            sc = 1.0f;
            ang = 0;
            break;
        case 1:
            sc = 1.0f;
            ang = 0xC000;
            break;
        case 2:
            sc = 1.0f;
            ang = 0x4000;
            break;
        case 3:
            sc = 1.0f;
            ang = 0x8000;
            break;
        case 8:
            sc = 2.0f;
            ang = 0;
            break;
        case 9:
            sc = 2.0f;
            ang = 0xC000;
            break;
        case 10:
            sc = 2.0f;
            ang = 0x4000;
            break;
        case 11:
            sc = 2.0f;
            ang = 0x8000;
            break;
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 500.0f * sc;
        rot[0] = 0;
        rot[1] = (u16)(em->ang[1] + ang);
        rot[2] = 0;
        cpRotMatrix(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = em->pos[0] + out[0];
        pos[1] = em->pos[1] + out[1];
        pos[2] = em->pos[2] + out[2];
        break;
    }
    case 7:
        found = 0;
        if (smell_cnt != 0) {
            em->x951 = em->x882;
            em->x952 = em->x883;
            for (i = 0; i < 32; i++) {
                sm = smell_stack[i];
                if (sm != 0 && sm->x10 == em->x882 && sm->x11 == em->x883) {
                    found = 1;
                    pos[0] = sm->pos[0];
                    pos[1] = sm->pos[1];
                    pos[2] = sm->pos[2];
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    fb:
        p = gp_ptr_ck(em, em->area->x0);
        goto def0;
    case 8:
        pos[0] = em->x9C8[0];
        pos[1] = em->x9C8[1];
        pos[2] = em->x9C8[2];
        break;
    case 9:
        g = gp_ck(em, em->area->x18, em->stg);
        if (g == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        r = (EM_ROUTE *)g->pos;
        if (r == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        if (em->x882 >= g->num) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pt = r[em->x882].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        if (em->x883 >= r->n) {
            em->x883 = r->n - 1;
        }
        pos[0] = pt[em->x883].pos[0];
        pos[1] = pt[em->x883].pos[1];
        pos[2] = pt[em->x883].pos[2];
        em->x86D = em->x882;
        em->x86E = em->x883;
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = em->x882;
        em->x3B0 = &player_work[em->x617];
        {
            u16 n = em->x3B0->x70E;

            base = gp_ptr_ck(em, em->area->x0)[n];
        }
        {
            u16 a = Em_Calc_angY(base, em->pos);

            rot[0] = 0;
            rot[1] = a;
            rot[2] = 0;
        }
        switch (em->x883) {
        case 0:
            v[0] = 0.0f;
            v[1] = 500.0f;
            v[2] = 1000.0f;
            break;
        case 1:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 800.0f;
            break;
        case 2:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 1500.0f;
            break;
        case 3:
            v[0] = 0.0f;
            v[1] = 600.0f;
            v[2] = 600.0f;
            break;
        }
        cpRotMatrixYXZ2(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = base[0] + out[0];
        pos[1] = base[1] + out[1];
        pos[2] = base[2] + out[2];
        break;
    }
    case 12:
        if (em->x882 == 0) {
            em->x95A = em->x883;
        }
        break;
    }
    em->x827 = em->x881;
    em->x828 = em->x882;
    em->x829 = em->x883;
}

u8 pl_ninshiki_ck(EMW *em) {
    int i;

    for (i = 0; i < game_w.pl_num; i++) {
        if (em->x88C & (1 << i)) {
            em->x88F |= 1 << i;
            em->x890[i] = em_ninshiki_timer_tbl[em->kind];
        } else if (em->x88F & (1 << i)) {
            if (--em->x890[i] <= 0) {
                em->x88F &= ~(1 << i);
            }
        }
    }
    return em->x88F;
}

extern f32 *em_absolute_ninshiki_len_tbl[];
void SetVector(f32 *, f32, f32, f32);
s8 direction_no_ret(EMW *, int);

void kehai_set(EMW *em) {
    int i;
    PLW *pl;
    f32 *len;
    s8 dir;
    VEC3 ppos;
    VEC3 v[2];

    for (i = 0; i < 4; i++) {
        pl = &player_work[i];
        if (pl->be_flag != 0) {
            if (i < game_w.pl_num) {
                if (Pl_stg_ck_tw(em, pl) && *(u8 *)&pl->flag14 != 3) {
                    len = em_absolute_ninshiki_len_tbl[em->kind];
                    em_pl_pos_set(em, i, &ppos.x);
                    em->x8C4[i] = flvecCalcDistance(em->pos, &ppos.x);
                    SetVector(&v[0].x, em->pos[0], 0.0f, em->pos[2]);
                    SetVector(&v[1].x, ppos.x, 0.0f, ppos.z);
                    em->x8D4[i] = flvecCalcDistance(&v[0].x, &v[1].x);
                    em->x904[i] = Em_Calc_angY(&v[0].x, &v[1].x);
                    if (em->x8C4[i] <= em->search->kehai) {
                        em->x914 |= 1 << i;
                        dir = direction_no_ret(em, i);
                        if (em->x8C4[i] <= len[dir]) {
                            em->x88F |= 1 << i;
                            em->x890[i] = em_ninshiki_timer_tbl[em->kind];
                        }
                        if (em->x8E4[0] >= em->x8C4[i] && em->x8E4[0] >= 0.0f) {
                            s32 *h = em_hate_suu_set(em, 3, i);
                            Em_Hate_Add(em, h[0], h[1], i);
                            Em_Kehai_Hate_Add(em, (u8)i, 0, dir);
                        } else if (em->x8E4[1] >= em->x8C4[i] && em->x8E4[1] >= 0.0f) {
                            s32 *h = em_hate_suu_set(em, 4, i);
                            Em_Hate_Add(em, h[0], h[1], i);
                            Em_Kehai_Hate_Add(em, (u8)i, 1, dir);
                        } else if (em->x8E4[2] >= em->x8C4[i] && em->x8E4[2] >= 0.0f) {
                            s32 *h = em_hate_suu_set(em, 5, i);
                            Em_Hate_Add(em, h[0], h[1], i);
                            Em_Kehai_Hate_Add(em, (u8)i, 2, dir);
                        } else if (em->x8E4[3] >= em->x8C4[i] && em->x8E4[3] >= 0.0f) {
                            s32 *h = em_hate_suu_set(em, 6, i);
                            Em_Hate_Add(em, h[0], h[1], i);
                            Em_Kehai_Hate_Add(em, (u8)i, 3, dir);
                        }
                    } else {
                        em->x914 &= ~(1 << i);
                        em->x8F4[i] = 0;
                    }
                } else {
                    em->x914 &= ~(1 << i);
                    em->x8F4[i] = 0;
                }
            } else {
                em->x914 &= ~(1 << i);
                em->x8F4[i] = 0;
            }
        }
    }
}

void kehai_ck(EMW *em) {
    int i;
    s32 max = em_max_kehai_hate_tbl[em->kind][em->x916];

    for (i = 0; i < 4; i++) {
        if (em_cancel_act_ck(em, 0x10) == 0 && em->x888 != 1 && em->x8BD == 0 &&
            em->x8F4[i] >= max && (em->x914 & (1 << i))) {
            em->x839 = 1;
            em->x917 |= 0x10;
            em->x915 |= 1 << i;
            em->x8F4[i] = 0;
        }
    }
}

void em_escape_mind_set(EMW *em, u8 kind, u8 lv) {
    if (em->x9E3 <= lv) {
        em->x9E3 = lv;
        em->x889 = 1;
        em->x88A = kind;
        em->x84E = 1;
    }
}

int em_cancel_act_ck(EMW *em, u8 flag) {
    int i;
    u8 mask;

    for (i = 0; i < 8; i++) {
        if (flag & (1 << i)) {
            break;
        }
    }
    if (i > 7) {
        return 0;
    }
    for (mask = 0; i < 8; i++) {
        mask |= 1 << i;
    }
    return (em->x83B & mask) != 0;
}

void em01_local_area_move_init(EMW *);
void em07_local_area_move_init(EMW *);
void em08_local_area_move_init(EMW *);
void em14_local_area_move_init(EMW *);
void em15_local_area_move_init(EMW *);
void em17_local_area_move_init(EMW *);
void em20_local_area_move_init(EMW *);
void em21_local_area_move_init(EMW *);
void em27_local_area_move_init(EMW *);

void em_area_move_init(EMW *em) {
    em->x928 = -1;
    em->x929 = -1;
    switch (em->kind) {
    case 1:
    case 11:
        em01_local_area_move_init(em);
        break;
    case 7:
        em07_local_area_move_init(em);
        break;
    case 8:
    case 34:
        em08_local_area_move_init(em);
        break;
    case 14:
    case 26:
        em14_local_area_move_init(em);
        break;
    case 15:
        em15_local_area_move_init(em);
        break;
    case 17:
    case 22:
        em17_local_area_move_init(em);
        break;
    case 6:
    case 20:
        em20_local_area_move_init(em);
        break;
    case 21:
        em21_local_area_move_init(em);
        break;
    case 27:
    case 28:
    case 31:
        em27_local_area_move_init(em);
        break;
    }
}

u16 calc_vec_ang(f32, f32, f32, f32);
f32 CalcDistanceXZ(f32 *, f32 *);
f32 flAbs(f32);

u16 Em_Calc_angY(f32 *a, f32 *b) {
    return calc_vec_ang(a[0], a[2], b[0], b[2]) - 0x4000;
}

void xang_calc_pl(EMW *em, int *ang, f32 a, f32 b) {
    f32 ppos[3];

    if (em->x617 != -1) {
        em_pl_pos_set(em, em->x617, ppos);
        *ang = (u16)(0x10000 - calc_vec_ang(flAbs(CalcDistanceXZ(em->pos, ppos) - -b), em->pos[1], 0.0f,
                                            ppos[1] + a));
    }
}

void xang_calc_target(EMW *em, int *ang, f32 a, f32 b) {
    *ang = (u16)(0x10000 - calc_vec_ang(flAbs(CalcDistanceXZ(em->pos, em->tgt_pos) - -b), em->pos[1], 0.0f,
                                        em->tgt_pos[1] + a));
}

typedef struct STAGE_HATE {
    f32 pos[3];         /* 0x00 */
    f32 range[4];       /* 0x0C */
    s32 hate[4];        /* 0x1C */
    s32 num;            /* 0x2C */
} STAGE_HATE;

extern STAGE_HATE *stage_hate_add_tbl[];

void Stage_Hate_Add(EMW *em) {
    PLW *pl;
    int i;
    int j;
    STAGE_HATE *t;
    f32 d;
    f32 ppos[3];

    for (pl = player_work, i = 0; i < 4; i++, pl++) {
        if (pl->be_flag != 0) {
            t = stage_hate_add_tbl[pl->stg];
            if (t != 0) {
                em_pl_pos_set(em, i, ppos);
                d = flvecCalcDistance(ppos, t->pos);
                for (j = 0; j < t->num; j++) {
                    if (d <= t->range[j]) {
                        Em_Hate_Add(em, t->hate[j], 30000, i);
                        break;
                    }
                }
            }
        }
    }
}

int em_pl_pos_set(EMW *em, u8 pl, f32 *pos) {
    PLW *p;

    if (Online_ck() == 1) {
        if (pl == -1) {
            return 0;
        }
        p = &player_work[pl];
        if (em->x9E2 != 0) {
            pos[0] = p->net_pos[0];
            pos[1] = p->net_pos[1];
            pos[2] = p->net_pos[2];
        } else {
            pos[0] = p->pos[0];
            pos[1] = p->pos[1];
            pos[2] = p->pos[2];
        }
    } else {
        if (pl == -1) {
            return 0;
        }
        p = &player_work[pl];
        pos[0] = p->pos[0];
        pos[1] = p->pos[1];
        pos[2] = p->pos[2];
    }
    return 1;
}

extern u8 em_action_priority_tbl[][8];
extern s16 *em_nest_tbl[];
void em_escape_mind_set(EMW *em, u8 kind, u8 lv);

void em_escape_action_ck(EMW *em) {
    s16 *nest = em_nest_tbl[em->kind];

    if (em->x9E3 < 0x80 && em_cancel_act_ck(em, 0x80) == 0) {
        if (nest != 0 && em->x888 == 1) {
            while (*nest != -1) {
                if (*nest++ == em->stg) {
                    return;
                }
            }
        }
        if (em->x9E4 != 0) {
            em->x9E3 = 0x80;
            em_escape_mind_set(em, em_action_priority_tbl[em->kind][0], 0x80);
        } else if (em->x9E3 < 0x70) {
            if (em->x9E5 != 0) {
                em->x9E5 = 0;
                em->x9E3 = 0x70;
                em_escape_mind_set(em, em_action_priority_tbl[em->kind][4], 0x70);
            } else if (em->x9E3 < 0x60) {
                if (em->x9E6 != 0) {
                    em->x9E3 = 0x60;
                    em_escape_mind_set(em, em_action_priority_tbl[em->kind][1], 0x60);
                } else if (em->x9E3 < 0x50) {
                    if (em->x9E7 != 0) {
                        em->x9E3 = 0x50;
                        em_escape_mind_set(em, em_action_priority_tbl[em->kind][2], 0x50);
                    } else if (em->x9E3 < 0x40 && em->x9E8 != 0 && em->x888 == 0) {
                        em->x9E3 = 0x40;
                        em_escape_mind_set(em, em_action_priority_tbl[em->kind][3], 0x40);
                    }
                }
            }
        }
    }
}

void em_hinshi_ck(EMW *em, f32 rate) {
    s16 *nest = em_nest_tbl[em->kind];

    if (em->mode >= 5) {
        em->x9E4 = 0;
        return;
    }
    if (em->x302 < (s16)(em->x792 * rate)) {
        if (em->x889 == 1 && em->x88A == em_action_priority_tbl[em->kind][0]) {
            return;
        }
        if (em_cancel_act_ck(em, 0x80) != 0) {
            return;
        }
        if (nest != 0 && em->x888 == 1) {
            while (*nest != -1) {
                if (*nest++ == em->stg) {
                    return;
                }
            }
        }
        em->x9E4 = 1;
    }
}

extern u8 em_egg_action_flag_tbl[];

void em_egg_ck(EMW *em) {
    s8 i;

    if (em_egg_action_flag_tbl[em->kind] == 0) {
        return;
    }
    if (em->x889 == 1 && em->x88A == em_action_priority_tbl[em->kind][4]) {
        for (i = 0; i < game_w.pl_num; i++) {
            if (*(u8 *)&player_work[i].work56B & 0xF) {
                return;
            }
        }
        em->x889 = 0;
        em->x88A = 0;
        em->x9E3 = 0;
    } else {
        for (i = 0; i < game_w.pl_num; i++) {
            if (*(u8 *)&player_work[i].work56B & 0xF) {
                em->x9E5 = 1;
                return;
            }
        }
    }
}

typedef struct EM_NEED {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
} EM_NEED;

extern EM_NEED *em_thirst_tbl[];
extern EM_NEED *em_suimin_tbl[];

void em_hungry_ck(EMW *em) {
    EM_HUNGRY *t = em_hungry_tbl[em->kind];
    s32 d;

    if (em->x888 == 0) {
        d = t->dec;
    } else {
        d = t->dec2;
    }
    em->hungry -= d;
    if (em->hungry < 0) {
        em->hungry = 0;
    }
    switch (*(u8 *)&em->x8C1) {
    case 0:
        if (em->hungry == 0) {
            em->x8C1 = 1;
            em->x9E6 = 1;
        }
    }
}

void em_thirst_ck(EMW *em) {
    EM_NEED *t = em_thirst_tbl[em->kind];
    s32 d;

    if (em->x888 == 0) {
        d = t->xC;
    } else {
        d = t->x10;
    }
    em->thirst -= d;
    if (em->thirst < 0) {
        em->thirst = 0;
    }
    switch (*(u8 *)&em->x8C0) {
    case 0:
        if (em->thirst == 0) {
            em->x9E7 = 1;
            em->x8C0 = 1;
        }
    }
}

void em_sleep_ck(EMW *em) {
    EM_NEED *t = em_suimin_tbl[em->kind];
    s32 d;

    if (em->x888 == 0) {
        d = t->x8;
    } else {
        d = t->xC;
    }
    em->x8A0 -= d;
    if (em->x8A0 < 0) {
        em->x8A0 = 0;
    }
    switch (em->x8BF) {
    case 0:
        if (em->x8A0 == 0) {
            em->x8BF = 1;
            em->x9E8 = 1;
        }
    }
}

s8 direction_no_ret(EMW *em, int pl) {
    int i;
    u16 a = *(u16 *)((u8 *)em + pl * 2 + 0x904);

    for (i = 0; i < 4; i++) {
        if ((u16)(a + em->x90C[i] - em->ang[1]) <= em->x90C[i] * 2) {
            return i;
        }
    }
    return i;
}

void net_send_em(EMW *, int, int);

void net_act_set(EMW *em, int kind, u16 no, int x) {
    em->x794 = kind;
    em->x795 = no;
    em->x797 = em->x881;
    em->x7A8 = em->x882;
    em->x7A9 = em->x883;
    em->x954 = ran_suu(0);
    action_timer_calc(em, 0);
    em->x390 &= 0x100100;
    em->x394 = 0;
    if (em->x8C3 == 0) {
        if (em->x9E2 != 0) {
            if ((u16)kind == 4 || (u16)kind == 5) {
                net_send_em(em, 3, x);
            } else {
                net_send_em(em, 1, x);
            }
        } else {
            em->x39A = em->x954;
            em_act_set_sub(em, kind, no);
        }
    }
}

int Em_Smoke_Ck(EMW *em) {
    int i;
    EM_SPOT *s;

    for (i = 0; i < 32; i++) {
        s = smoke_stack[i];
        if (s != 0 && s->stg == em->stg && (s->flag & 1) && flvecCalcDistance(s->pos, em->pos) <= s->range) {
            return 1;
        }
    }
    return 0;
}

int Em_Unko_Smoke_Ck(EMW *em) {
    int i;
    EM_SPOT *s;

    for (i = 0; i < 32; i++) {
        s = smoke_stack[i];
        if (s != 0 && s->stg == em->stg && (s->flag & 2) && (em->x388 == 0 || em->x388 == 1) &&
            em->x9E3 < 0x80 && flvecCalcDistance(s->pos, em->pos) <= s->range) {
            return 1;
        }
    }
    return 0;
}

u8 Em_Senko_Ck(EM_SPOT *s) {
    int i;
    int hit = 0;
    EMW *em;
    PLW *pl;
    s32 *h;
    f32 ppos[3];

    for (i = 0; i < 20; i++) {
        em = &em_work[i];
        if (em->be_flag != 0) {
            pl = &player_work[s->pl];
            if (Pl_stg_ck_tw(em, pl) && *(u8 *)&pl->flag14 != 3) {
                em->x88F |= 1 << s->pl;
                em->x890[s->pl] = em_ninshiki_timer_tbl[em->kind];
                h = em_hate_suu_set(em, 7, s->pl);
                Em_Hate_Add(em, h[0], h[1], s->pl);
                em_pl_pos_set(em, s->pl, ppos);
                if (flvecCalcDistance(s->pos, ppos) <= s->range) {
                    hit = 1;
                }
            }
        }
    }
    return hit;
}

extern s32 (*em_hate_add_tbl[])[2];
int Pl_silencer_ck(PLW *);

s32 *em_hate_suu_set(EMW *em, u8 type, u8 n);

