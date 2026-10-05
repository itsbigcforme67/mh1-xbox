/* em17_r05 - monster 17 AI 0x005DB5D0-0x005DC550: em_act15_005DB5D0, em_act16_005DB720, em_act17_005DB850, em_act19_005DB8D0, em_act18_005DB9A0, em_act20_005DBB00, em_act21_005DBC10, em_act22_005DBD70, em_act23_005DBDF0, em_act24_005DBEC0, em_act25_005DBF90, em_act26_005DC010, em_act27_005DC0C0, em_act28_005DC1D0, em_act29_005DC2A0, em_act31_005DC3E0, em_act33_005DC4C0. Whole file in em17_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em17.h"

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
void em_char_set(EMW *, int, int, int);
void SetVector(f32 *, f32, f32, f32);
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
void em_dur_set(EMW *, int);
void em_ikari_add(EMW *, s16);
void Eft20_set(f32, EMW *, int, int);
void Eft13_set_em(EMW *, int, int);
int em_frame_check3(EMW *, int, f32, f32);
u8 Em_stg_ck(EMW *);
void em17_horm_init(EMW *);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_no_battle_area_ck(EMW *, int, int);
void get_joint_pos_em(EMW *, int, f32 *);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void em17_act_set(EMW *em, int kind, u16 no, u16 arg);
void em_range_set(EMW *em, s8 no);
void em_area_move_init(EMW *em);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_search_data_set(EMW *em, u8 no);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_no_floor_ck(EMW *em);
void em_hp_add(EMW *em, s16 n);
void em_ana_loop_cnt_set(EMW *em);
void em_tail_off_sub(EMW *em);
void em_sleep2_dmg_timer_set(EMW *em);
void Eft17_set_ex(f32 *, int, int, f32);
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
extern s16 em17_stay_timer_tbl[];
extern s16 em17_runaway_timer_tbl[];

void em17_local_init(EMW *em);
void em17_init(EMW *em);
u16 *em_act_search2_005DA580(EMW *em, u16 *tbl);
void act_dist_select_005DA5C0(EMW *em);
void em17_to_normal(EMW *em, s16 a, s16 b);
void em17_to_fly(EMW *em, int flag);
void em17_frame_reset(EMW *em, int i);
void em_act00_005DA9F0(EMW *em, EM17W *w);
void em_act01_005DAAD0(EMW *em, EM17W *w);
void em_act02_005DABF0(EMW *em, EM17W *w);
void em_act03_005DAC80(EMW *em, EM17W *w);
void em_act04_005DAD40(EMW *em, EM17W *w);
void em_act05_005DADD0(EMW *em, EM17W *w);
void em_act06_005DAEC0(EMW *em, EM17W *w);
void em_act07_005DAF90(EMW *em, EM17W *w);
void em_act08_005DB050(EMW *em, EM17W *w);
void em_act09_005DB120(EMW *em, EM17W *w);
void em_act10_005DB1C0(EMW *em, EM17W *w);
void em_act11_005DB2D0(EMW *em, EM17W *w);
void em_act12_005DB3A0(EMW *em, EM17W *w);
void em_act13_005DB440(EMW *em, EM17W *w);
void em_act14_005DB510(EMW *em, EM17W *w);
void em_act15_005DB5D0(EMW *em, EM17W *w);
void em_act16_005DB720(EMW *em, EM17W *w);
void em_act17_005DB850(EMW *em, EM17W *w);
void em_act19_005DB8D0(EMW *em, EM17W *w);
void em_act18_005DB9A0(EMW *em, EM17W *w);
void em_act20_005DBB00(EMW *em, EM17W *w);
void em_act21_005DBC10(EMW *em, EM17W *w);
void em_act22_005DBD70(EMW *em, EM17W *w);
void em_act23_005DBDF0(EMW *em, EM17W *w);
void em_act24_005DBEC0(EMW *em, EM17W *w);
void em_act25_005DBF90(EMW *em, EM17W *w);
void em_act26_005DC010(EMW *em, EM17W *w);
void em_act27_005DC0C0(EMW *em, EM17W *w);
void em_act28_005DC1D0(EMW *em, EM17W *w);
void em_act29_005DC2A0(EMW *em, EM17W *w);
void em_act31_005DC3E0(EMW *em, EM17W *w);
void em_act33_005DC4C0(EMW *em, EM17W *w);
void em_mv00_005DC550(EMW *em, EM17W *w);
void em_mv01_005DC6B0(EMW *em, EM17W *w);
void em_mv02_005DC6C0(EMW *em, EM17W *w);
void em_mv03_005DC740(EMW *em, EM17W *w);
void em_mv04_005DCA10(EMW *em, EM17W *w);
void em_mv05_005DCBD0(EMW *em, EM17W *w);
void em_mv06_005DCD30(EMW *em, EM17W *w);
void em_mv07_005DCE90(EMW *em, EM17W *w);
void em_fly00_005DCFD0(EMW *em, EM17W *w);
void em_fly01_005DD0D0(EMW *em, EM17W *w);
void em_fly02_005DD310(EMW *em, EM17W *w);
void em_fly03_005DD420(EMW *em, EM17W *w);
void em_fly04_005DD6D0(EMW *em, EM17W *w);
void em_fly05_005DD840(EMW *em, EM17W *w);
void em_fly07_005DD9B0(EMW *em, EM17W *w);
void em_fly09_005DDAF0(EMW *em, EM17W *w);
void em_fly14_005DDC60(EMW *em, EM17W *w);
void em_fly16_005DDD20(EMW *em, EM17W *w);
void em_fly20_005DDE20(EMW *em, EM17W *w);
void em_fly21_005DDF40(EMW *em, EM17W *w);
void em_fly23_005DE080(EMW *em, EM17W *w);
void em_atk00_005DE2C0(EMW *em, EM17W *w);
void em_atk02_005DE360(EMW *em, EM17W *w);
void em_atk03_005DE370(EMW *em, EM17W *w);
void em_atk04_005DE3F0(EMW *em, EM17W *w);
void em_atk05_005DE530(EMW *em, EM17W *w);
void em_atk06_005DE710(EMW *em, EM17W *w);
void em_atk07_005DE800(EMW *em, EM17W *w);
void em_atk08_005DE9E0(EMW *em, EM17W *w);
void em_atk09_005DEAA0(EMW *em, EM17W *w);
void em_dmg00_005DEB30(EMW *em, EM17W *w);
void em_dmg01_005DEBD0(EMW *em, EM17W *w);
void em_dmg02_005DEC60(EMW *em, EM17W *w);
void em_dmg03_005DECF0(EMW *em, EM17W *w);
void em_dmg04_005DED80(EMW *em, EM17W *w);
void em_dmg05_005DEF50(EMW *em, EM17W *w);
void em_dmg06_005DEFF0(EMW *em, EM17W *w);
void em_dmg07_005DF090(EMW *em, EM17W *w);
void em_dmg08_005DF180(EMW *em, EM17W *w);
void em_dmg09_005DF2A0(EMW *em, EM17W *w);
void em_dmg10_005DF3D0(EMW *em, EM17W *w);
void em_dmg11_005DF460(EMW *em, EM17W *w);
void em_dmg12_005DF540(EMW *em, EM17W *w);
void em_dmg13_005DF670(EMW *em, EM17W *w);
void em_dmg14_005DF7A0(EMW *em, EM17W *w);
void em_dmg15_005DF8C0(EMW *em, EM17W *w);
void em_dmg16_005DF950(EMW *em, EM17W *w);
void em_dmg17_005DFA40(EMW *em, EM17W *w);
void em_dmg18_005DFAC0(EMW *em, EM17W *w);
void em_dmg19_005DFBF0(EMW *em, EM17W *w);
void em_dmg20_005DFCF0(EMW *em, EM17W *w);
void em_demo00_005DFDF0(EMW *em, EM17W *w);
void em_demo04_005E06F0(EMW *em, EM17W *w);
void em_die00_005E07A0(EMW *em, EM17W *w);
void em_die01_005E08A0(EMW *em, EM17W *w);
void em_die02_005E09C0(EMW *em, EM17W *w);
void em_die03_005E0B90(EMW *em, EM17W *w);
void em17_soukou_dm_sel_set(EMW *em);
void em_move00_005E0D20(EMW *em, EM17W *w);
void em_move01_005E0F60(EMW *em, EM17W *w);
void em_move02_005E1030(EMW *em, EM17W *w);
void em_move03_005E11A0(EMW *em, EM17W *w);
void em_move04_005E1270(EMW *em, EM17W *w);
void em_move05_005E1400(EMW *em, EM17W *w);
void em_move06_005E1490(EMW *em, EM17W *w);
void em17_main(EMW *em);
void em17_main_sub(EMW *em, EM17W *w);
void em17_uvmove(EMW *em);
void sound_call_sub_005E1F80(EMW *em, int se, int joint);
void sound_call_005E1FF0(EMW *em, int frame, int se, int joint);
void sound_call_parts_005E2090(EMW *em, int frame, int se, int joint, u8 layer);
void Em_set_quake_sub(EMW *, int);
void quake_call_005E2130(EMW *em, int frame, int arg);
void move_default_005E2180(EMW *em);
void ef_move_sub_005E21D0(EMW *em, EM17W *w);
void em17_effect_move(EMW *em);
void ground_land_eff_set_005E63F0(EMW *em);
s32 kyusyu_char_set2_005E64A0(EMW *em);
void em17_atk_end_sel(EMW *em);
void dummy_em_prog_005E65C0(void);





extern u8 *em17_act_add[3];
extern u16 *em17_rail_add[2];
extern u16 *em17_rail_half_add[1];






extern u8 *em17_act_add[3];

















































































































void em_act15_005DB5D0(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 0x51, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05 = temp_v1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    sound_call_parts_005E2090(em, 0x20, 0x57, 0x23, 1);
    sound_call_parts_005E2090(em, 0x11A, 0x20, 0x23, 1);
}

void em_act16_005DB720(EMW *em, EM17W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x51, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x50, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
            return;
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act17_005DB850(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6A, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act19_005DB8D0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x68, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act18_005DB9A0(EMW *em, EM17W *w) {
    s16 temp_a1_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        Em_Suimin_Start(em);
        em->work08 = 0x708;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        temp_a1_2 = em->x792;
        if (em->x302 < temp_a1_2) {
            em_hp_add(em, 1);
        } else {
            em_hp_add(em, temp_a1_2);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hp_add(em, em->x792);
            em_hinshi_end(em);
            em17_act_set(em, 0, 0x17, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

void em_act20_005DBB00(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

void em_act21_005DBC10(EMW *em, EM17W *w) {
    s16 temp_a1_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x27, 0, 0);
        em->work08 = 0x258;
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_a1_2 = em->x792;
        if (em->x302 < temp_a1_2) {
            em_hp_add(em, 1);
        } else {
            em_hp_add(em, temp_a1_2);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hp_add(em, em->x792);
            em_hinshi_end(em);
            em17_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

void em_act22_005DBD70(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x95C = 2;
    em->x7D6 = 2;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        em->x762 = 3;
        /* fallthrough */
    case 1:
        break;
    }
}

void em_act23_005DBDF0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x21, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        em_suimin_end(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act24_005DBEC0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act25_005DBF90(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act26_005DC010(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x95C = 2;
    em->x7D6 = 2;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        em->x762 = 3;
        em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        /* fallthrough */
    case 1:
        break;
    }
}

void em_act27_005DC0C0(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 0, 0x1C, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

void em_act28_005DC1D0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em->x88B = 1;
        em_range_set(em, 0);
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act29_005DC2A0(EMW *em, EM17W *w) {
    f32 v[3];
    s32 temp_v0;
    u8 temp_v1;

    em->x9EA = 5;
    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 4, 0x13, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x13, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005E1F80(em, 0x57, 0x23);
    }
}

void em_act31_005DC3E0(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 4, 0x14, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x14, 3);
        }
        break;
    }
}

void em_act33_005DC4C0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}
