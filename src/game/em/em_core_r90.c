/* em_core_r90 - near-match fixes: smell_ck. Whole file in em_core_nm.c. 0x005355A0-0x00535824: smell_ck. Whole file in em_core_nm.c. */
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
u16 calc_vec_ang(f32, f32, f32, f32);
void neck_ang_set(EMW *em, u16 spd, u16 ang);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);

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







/* em_kehai_add_tbl[kind]: base add per sound kind, then per player status. */
typedef struct EM_KEHAI_ADD {
    s32 *base;          /* 0x00 [kind] */
    s32 (*st[4])[4];    /* 0x04 [status][kind] for levels 0..3 */
} EM_KEHAI_ADD;

extern EM_KEHAI_ADD *em_kehai_add_tbl[];
extern s32 *em_max_kehai_hate_tbl[];
int Pl_Skill_ck(PLW *, int);
int em_cancel_act_ck(EMW *, u8);


extern f32 (*em_range_data_tbl[])[2];
extern u16 (*em_range_ang_data_tbl[])[2];


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




extern f32 *em_absolute_ninshiki_len_tbl[];
void SetVector(f32 *, f32, f32, f32);
s8 direction_no_ret(EMW *, int);





void em01_local_area_move_init(EMW *);
void em07_local_area_move_init(EMW *);
void em08_local_area_move_init(EMW *);
void em14_local_area_move_init(EMW *);
void em15_local_area_move_init(EMW *);
void em17_local_area_move_init(EMW *);
void em20_local_area_move_init(EMW *);
void em21_local_area_move_init(EMW *);
void em27_local_area_move_init(EMW *);


u16 calc_vec_ang(f32, f32, f32, f32);
f32 CalcDistanceXZ(f32 *, f32 *);
f32 flAbs(f32);




typedef struct STAGE_HATE {
    f32 pos[3];         /* 0x00 */
    f32 range[4];       /* 0x0C */
    s32 hate[4];        /* 0x1C */
    s32 num;            /* 0x2C */
} STAGE_HATE;

extern STAGE_HATE *stage_hate_add_tbl[];



extern u8 em_action_priority_tbl[][8];
extern s16 *em_nest_tbl[];
void em_escape_mind_set(EMW *em, u8 kind, u8 lv);



extern u8 em_egg_action_flag_tbl[];


typedef struct EM_NEED {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
} EM_NEED;

extern EM_NEED *em_thirst_tbl[];
extern EM_NEED *em_suimin_tbl[];





void net_send_em(EMW *, int, int);





extern s32 (*em_hate_add_tbl[])[2];
int Pl_silencer_ck(PLW *);


int smell_ck(EMW *em, int joint) {
    f32 min = -1.0f;
    u8 i;
    PLW *pl;
    EM_SMELL *s;
    u8 found = 0;
    u8 best;
    f32 d;
    f32 p[3];
    f32 hit[3];
    u8 r;

    if (em->hungry <= em_hungry_tbl[em->kind]->smell && em->x388 == 0) {
        for (i = 0; i < game_w.pl_num; i++) {
            pl = &player_work[i];
            if (pl->be_flag == 0 || Pl_stg_ck_tw(em, pl) == 0) {
                found++;
            }
        }
        if (found == game_w.pl_num) {
            found = 0;
        } else {
            i = 0;
            found = 0;
            for (; i < 32; i++) {
                s = smell_stack[i];
                if (s != 0 && s->stg == em->stg) {
                    d = flvecCalcDistance(em->pos, s->pos);
                    if (d <= s->range) {
                        found = 1;
                        if (min == -1.0f) {
                            min = d;
                            best = i;
                        } else if (min > d) {
                            min = d;
                            best = i;
                        }
                    }
                }
            }
        }
    }
    if (found == 0) {
        em->x951 = 0xFF;
        em->x952 = 0xFF;
        em->x950 = 0xFF;
        return 0;
    }
    s = smell_stack[best];
    get_joint_pos_em(em, joint, p);
    if (game_w.gate_open != 0) {
        r = GetWallHitLine(p, s->pos, hit, em->x95E | 0x4000);
    } else {
        r = GetWallHitLine(p, s->pos, hit, em->x95E);
    }
    if (!(u8)r) {
        em->x951 = s->x10;
        em->x952 = s->x11;
        em->x950 = s->type;
        return 1;
    }
    em->x951 = 0xFF;
    em->x952 = 0xFF;
    em->x950 = 0xFF;
    return 0;
}
