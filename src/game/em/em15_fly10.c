/* em15_fly10 - game.bin 0x005C6430-0x005C6624: em_fly10_005C6430. Whole file in em15_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em15.h"

typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_hungry_tbl[];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;          /* 0x10 */
    f32 depth;          /* 0x14 */
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;
STAGE_DATA *Stage_data_get(u8);
#define GAME_X1E16 (*(u16 *)((u8 *)&game_w + 0x1E))
typedef struct EML {
    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */
    u8 _pad[0x4C];
} EML;
#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))


f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
void eft09_set(EMW *, int);
void em_cmd_reset(EMW *);
void Em_Sleep_End(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
void Eft04_set_time(EMW *, int, int, f32);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void Em_Mahi_Start(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Eft13_set_em_scl(EMW *, int, f32, int);
void em_char_set2(EMW *, int, int, int, int);
void flmatGetTrans(f32 *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void em_mahi_eff_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void Eft15_set3(EMW *, int, f32, int);
void Eft13_set_em(EMW *, int, int);
int em_frame_check3(EMW *, int, f32, f32);
void em15_act_set(EMW *em, int kind, u16 no, u16 arg);
s16 em_hp_vital_set2(EMW *, s16, s16);
void get_joint_pos_em(EMW *, int, f32 *);
void em_range_set(EMW *em, s8 no);
void NextStage_No_Set(EMW *);
void Em_Next_Stage_Pos(EMW *);
void NextStage_Dir_Set(EMW *, f32 *);
void em_area_move_init(EMW *em);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_search_data_set(EMW *em, u8 no);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_hungry_end(EMW *em);
void em_thirst_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_hungry_add(EMW *em, s32 n);
void em_thirst_add(EMW *em, s32 n);
void em_hp_add(EMW *em, s16 n);
void em_ana_loop_cnt_set(EMW *em);
int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step);
void em_niku_eat_set(EMW *em);
void Quest_enemy_capture();
extern s16 em15_stay_timer_tbl[];
extern s16 em15_runaway_timer_tbl[];

void em15_local_init(EMW *em);
void em15_init(EMW *em);
void em15_to_normal(EMW *em);
void em15_to_tenjo(EMW *em);
void em15_to_fly(EMW *em);
void em15_frame_reset(EMW *em, int i);
void em_fly26(EMW *em, EM15W *w);
void em_fly27(EMW *em, EM15W *w);
void em_fly28(EMW *em, EM15W *w);
void em_fly29(EMW *em, EM15W *w);
void em_fly30(EMW *em, EM15W *w);
void em_fly31(EMW *em, EM15W *w);
void em_fly32(EMW *em, EM15W *w);
void em_fly33(EMW *em, EM15W *w);
void em_fly34(EMW *em, EM15W *w);
void em15_uvmove(EMW *em);
void Em_set_quake_sub(EMW *, int);
void em15_effect_move(EMW *em);
void dummy_em_prog_005CF0E0(void);

void em_fly10_005C6430(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        NextStage_No_Set(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        break;
    case 1:
        em15_senkai_target(em);
        em15_fly_adjy(em, 1);
        em->pos[1] += 100.0f;
        temp_f1 = em->pos[1];
        if (!(temp_f1 < em->tgt_pos[1])) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x05 = temp_a1 + 1;
        em->adj_z = 40.0f;
        em_char_set(em, 0xF, 0, 0);
        /* fallthrough */
    case 3:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0) {
            em->x05 += 1;
        }
        break;
    case 4:
        em->x05 = temp_a1 + 1;
        Em_Next_Stage_Pos(em);
        WyvernAreaMove(em);
        if (em->stg == 0xF) {
            em->stg = 0x13;
        } else {
            em->stg = 0xF;
        }
        em15_to_fly(em);
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}
