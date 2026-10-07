/* em20_fly10 - game.bin 0x005F0730-0x005F0934: em_fly10_005F0730. Whole file in em20_ai_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em20.h"
void em20_xang_set_pl(EMW *em, int mode, f32 h);   /* PC: prototype (the draft passed f12 first as an int) */

typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_thirst_tbl[];
typedef f32 (*EM_POSP)[3];
EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);
extern u16 gero_tbl[4][2];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;
    f32 depth;
    f32 floor_y;
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
void em_pl_pos_set(EMW *, u8, f32 *);
void World_calc2(u8, f32 *, f32 *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
void eft09_set(EMW *, int);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
void em_cmd_ck(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void Em_Mahi_Start(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int Code_Make(int, int, int, int);
void Eft13_set_em_scl(EMW *, int, f32, int);
void em_char_set2(EMW *, int, int, int, int);
void flmatGetTrans(f32 *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void em_mahi_eff_set(EMW *, int);
u16 em_act_search(void *);
void em_action_timer_calc(EMW *, int);
void em_rate_clear_g(EMW *);
int Event_flag_ck();
void em_dur_set(EMW *, int);
int Quest_clear_ck();
void em_ikari_add(EMW *, s16);
void Eft20_set(f32, EMW *, int, int);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
int em_frame_check3(EMW *, int, f32, f32);
void em20_act_set(EMW *em, int kind, u16 no, u16 arg);
void em20_ground_point_search(EMW *em);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_hinshi_ck(EMW *, f32);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void get_joint_pos_em(EMW *, int, f32 *);
void em_range_set(EMW *em, s8 no);
void NextStage_No_Set(EMW *);
void Em_Next_Stage_Pos(EMW *);
void NextStage_Dir_Set(EMW *, f32 *);
void em_area_move_init(EMW *em);
void xang_calc_pl(EMW *em, int *ang, f32 a, f32 b);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_egg_ck(EMW *em);
void em_hungry_ck(EMW *em);
void em_thirst_ck(EMW *em);
void em_search_data_set(EMW *em, u8 no);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_hungry_add(EMW *em, s32 n);
void em_no_floor_ck(EMW *em);
void em_hp_add(EMW *em, s16 n);
int em_target_pl_samestage_ck(EMW *em);
void em_ana_loop_cnt_set(EMW *em);
int em_hokaku_ck(EMW *em, f32 rate);
int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step);
void em_niku_eat_set(EMW *em);
void em_sleep2_dmg_timer_set(EMW *em);
void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl);
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
extern s16 em20_stay_timer_tbl[];
extern s16 em20_runaway_timer_tbl[];

void em20_local_init(EMW *em);
void em20_init(EMW *em);
void em20_to_normal(EMW *em, s16 a, s16 b);
void em20_dmg_to_normal(EMW *em, s16 a, s16 b);
void em20_to_fly(EMW *em, int flag);
void em20_frame_reset(EMW *em, int i);
void em_act34(EMW *em, EM20W *w);
void em20_main(EMW *em);
void em20_main_sub(EMW *em, EM20W *w);
void em20_uvmove(EMW *em);
void Em_set_quake_sub(EMW *, int);
void em20_effect_move(EMW *em);
void em20_atk_end_sel(EMW *em, EM20W *w);
void em20_material_sub(EMW *em, int type, u8 *tbl);
void dummy_em_prog_005FCDB0(void);

void em_fly10_005F0730(EMW *em, EM20W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        NextStage_No_Set(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        w->x18 = 1;
        break;
    case 1:
        em20_senkai_target(em);
        em20_fly_adjy(em, 1);
        em->pos[1] += 100.0f;
        temp_f1 = em->pos[1];
        if (!(temp_f1 < em->tgt_pos[1])) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x05 = temp_a1 + 1;
        em->adj_z = 50.0f;
        em_char_set(em, 0xC, 0, 0);
        /* fallthrough */
    case 3:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
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
        em20_to_fly(em, 1);
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}
