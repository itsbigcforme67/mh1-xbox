/* em_master - game.bin 0x00539AC0-0x00539C90: Em_Yobi_Ck (monsters that
 * answer a call for help) and Ikari_Data_Set. First matching run of the
 * shared monster housekeeping file that starts with Em_Master_Change
 * (0x5395F0, still asm); the whole file, near-matches included, is in
 * em_master_nm.c. Meanings are guesses. */
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

int Em_Yobi_Ck(EMW *em, EM_YOBI *self) {
    s8 i;
    EM_YOBI *y;

    if (em_yobi_cnt == 0) {
        return 0;
    }
    if (em->x9E1 != 0) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        y = em_yobi_stack[i];
        if (y == 0 || y == self || y->stg != em->stg) {
            continue;
        }
        switch (y->type) {
        case 0:
            if (em->kind != 16 && em->kind != 27) {
                continue;
            }
            break;
        case 1:
            if (em->kind != 9) {
                continue;
            }
            break;
        case 2:
            if (em->kind != 12 && em->kind != 25) {
                continue;
            }
            break;
        }
        if (!(y->range < flvecCalcDistance(y->pos, em->pos))) {
            SetVector(em->x9C8, y->pos[0], y->pos[1], y->pos[2]);
            return 1;
        }
    }
    return 0;
}

void Ikari_Data_Set(EMW *em) {
    s16 *p = em_ikari_data_tbl[em->kind];

    if (p != 0) {
        em->x8B0 = *p;
    } else {
        em->x8B0 = -1;
    }
    em->x8B2 = 0;
    em->x8B4 = 0;
    em->x8B6 = 0;
    em->x8B7 = 0;
    em->x8B8 = 0;
}

void Em_Taisei_Set(EMW *em);

void Em_Sleep_Start(EMW *em);

void Em_Sleep2_Start(EMW *em);

void Em_Sleep_End(EMW *em);

void Em_Sleep2_End(EMW *em);

void Em_Mahi_Start(EMW *em);

void Em_Mahi_End(EMW *em);

void Em_Sleep_Flag_Ck(EMW *em);

void Em_Sleep_Flag_Ck2(EMW *em);

typedef f32 (*EM_POSP)[3];

EM_STG_POS *gp_ck(EMW *em, EM_STG_POS *p, s16 stg);

EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);

void em_wall_bit_set(EMW *em);

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

void em_boss_work_set(EMW *em);

void smell_dmg_set(EMW *em, EM_SMELL *s);

void em_tsuushin_set(EMW *em);

void em_hinshi_end(EMW *em);

void em_hinshi_end2(EMW *em);

void em_hungry_end(EMW *em);

void em_thirst_end(EMW *em);

void em_suimin_end(EMW *em);

void Em_Suimin_Start(EMW *em);

void em_sleep_eff_set(EMW *em, int a, int b);

void em_mahi_eff_set(EMW *em, int a);

s32 st_mv_ptr_ck(EMW *em);

void em_hungry_add(EMW *em, s32 n);

void em_thirst_add(EMW *em, s32 n);

void em_suimin_add(EMW *em, s32 n);

void em_no_floor_ck(EMW *em);

void em_no_floor_ck2(EMW *em);

void em_no_battle_area_ck(EMW *em, u16 mode, u16 sub);

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

int em_mode_timer_sub(EMW *em);

void em_hp_add(EMW *em, s16 n);

int em_target_pl_samestage_ck(EMW *em);

s16 em_hp_vital_set(EMW *em, s16 v);

s16 em_hp_vital_set2(EMW *em, s16 a, s16 b);

f32 em_def_attack_set(EMW *em);

f32 em_def_defence_set(EMW *em);

void em_ana_loop_cnt_set(EMW *em);

int em_hokaku_ck(EMW *em, f32 rate);

int Bdora_hp_ck(void);

int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step);

void em_hagitori_lv_up(EMW *em, u8 bits);

void em_tail_off_sub(EMW *em);

void em_g_init_flag_set(EMW *em, u8 f);

void em_niku_eat_set(EMW *em);

