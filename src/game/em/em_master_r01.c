/* em_master_r01 - near-match fixes: em_hagitori_lv_up. Whole file in em_master_nm.c. 0x0053B6D0-0x0053B8A0: em_hagitori_lv_up. Whole file in em_master_nm.c. */
#include "em_sys.h"
#include "game.h"
#include "pl.h"
#include "fl.h"

int Pl_stg_ck_tw(EMW *, PLW *);
void net_send_em(EMW *, int, int, u8);


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
void Em_Mode_Chg();
void Em_Sleep_End(EMW *em);
void Em_Sleep2_End(EMW *em);
void Em_Mahi_End(EMW *em);












typedef f32 (*EM_POSP)[3];




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
















void em_hagitori_lv_up(EMW *em, u8 bits) {
    s16 i;
    s16 j;
    u8 m = 1;
    FLMAT mat;
    f32 p[3];
    f32 v[3];
    EM_HAGI_EFT *h;

    if ((u8)Em_stg_ck(em)) {
        for (i = 0; i < 8; m <<= 1, i++) {
            if (!(bits & m)) {
                continue;
            }
            for (j = 0; j < 15; j++) {
                if (em->kind == hagitori_eft_tbl[j].kind && i == hagitori_eft_tbl[j].part &&
                    em->hagi[i].cnt == hagitori_eft_tbl[j].lv) {
                    h = &hagitori_eft_tbl[j];
                    flmatCopy(&mat, get_joint_wmat_em(em, hagitori_eft_tbl[j].joint));
                    flvecCopy(v, h->ofs);
                    flvecApplyMat33_2(v, &mat);
                    p[0] = mat[3][0] + v[0];
                    p[1] = mat[3][1] + v[1];
                    p[2] = mat[3][2] + v[2];
                    Eft02_set3(1.0f, em, 0, 10, 0, p);
                    break;
                }
            }
        }
    }
}
