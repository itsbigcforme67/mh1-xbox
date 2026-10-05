/* em12 (part 3) - game.bin 0x005B40D0-0x005B5248: em12_main_sub (mode dispatch), the per-animation
 * sound/effect script ef_move_sub, em12_effect_move, sound_call, em12_local_init and the dummy
 * program. em12_main (0x005B3A50) and em_mov01 (0x005B06E0) are near-matches in em12_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM12W {
    u8 eff;             /* 0x00 em12_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[2];
    u8 x0A;             /* 0x0A */
    u8 _pad0B[5];
    f32 tgt[3];         /* 0x10 target position (flvecCopy source in act 2 / act_set) */
    u8 _pad1C[4];
    f32 dist;           /* 0x20 distance to the target */
    f32 yobi[3];        /* 0x24 position handed to push_em_yobi */
    f32 x30;            /* 0x30 range */
    u8 _pad34[2];
    u8 x36;             /* 0x36 */
    u8 _pad37;
    u8 x38;             /* 0x38 stage */
    u8 _pad39[3];
    u8 x3C;             /* 0x3C */
    u8 x3D;             /* 0x3D */
    u16 x3E;            /* 0x3E */
    u8 _pad40[0x90 - 0x40];
    s8 x90;             /* 0x90 */
} EM12W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern f32 em12_scale_tbl[];
extern EMW em_work[];
extern u8 em_boss_tbl[];
void em12_act_set(EMW *em, int kind, u16 no, int unused);
int em_mode_timer_sub(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
int Em_Yobi_Ck(EMW *, f32 *);
void push_em_yobi(f32 *);
void SetVector(f32 *, f32, f32, f32);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
u8 Em_Smoke_Ck(EMW *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
void Shell11_set(EMW *, int);
void Eft13_set_em(EMW *, int, int);
void Eft20_set(f32, EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void eft01_set(PLW *, int);
int Code_Make(int, int, int, int);
void em12_main_sub(EMW *em);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void Eft02_set4(f32, int, int, int, f32 *);
int Event_flag_ck(int);
extern f32 st45_pos_tbl[][6];
extern f32 st49_pos_tbl_0065BBC0[][6];
extern f32 st51_pos_tbl_0065BC20[][6];
extern f32 st53_pos_tbl_0065BC80[][6];
extern f32 st56_pos_tbl[][6];
void Em_Mode_Chg(EMW *, int, int);
void Tuto_flag_set(int);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void flvecCopy(f32 *, f32 *);
f32 CalcDistanceXZ(f32 *, f32 *);
int calc_vec_ang(f32, f32, f32, f32);
void flvecRotY(f32 *, f32);
void em_cmd_reset(EMW *);
void Eft16_set_ex(EMW *, int, int);
void em_escape_mind_set(EMW *, u8, u8);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em12_init(EMW *);
int em_frame_check2(EMW *, int, f32);
int em_frame_check(EMW *, f32, int);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void Quest_enemy_escape(EMW *);
void pull_em_yobi(f32 *);
void em12_next_act_set(EMW *em);
int em_frame_check3(EMW *, int, f32, f32);
void mot_miration_ret(EMW *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
s8 em_search_set(EMW *, int, f32, f32);

void em_move00_005B0460(EMW *);
void em_mov00_005B0590(EMW *);
void em_mov01_005B06E0(EMW *);
void em_move01_005B1400(EMW *);
void em_move02_005B0580(EMW *);
void em_move03_005B17E0(EMW *);
void em_move04_005B2440(EMW *);
void em_move05_005B2F10(EMW *);
void em_move06_005B3790(EMW *);
void em_move07_005B39E0(EMW *);

void em12_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em_move00_005B0460(em); break;
    case 1: em_move01_005B1400(em); break;
    case 2: em_move02_005B0580(em); break;
    case 3: em_move03_005B17E0(em); break;
    case 4: em_move04_005B2440(em); break;
    case 5: em_move05_005B2F10(em); break;
    case 6: em_move06_005B3790(em); break;
    case 7: em_move07_005B39E0(em); break;
    }
}

/* Sound and effect script per animation (sound_call(em, frame, se, joint)
 * plays a sound at the joint once the animation reaches the frame). */
static void sound_call_005B5150(EMW *em, int frame, int se, int joint);

static void ef_move_sub_005B4190(EMW *em, EM12W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005B5150(em, 0x100, 9, 0x10);
        break;
    case 0x3EA:
        sound_call_005B5150(em, 0x20, 0xA, 0x10);
        sound_call_005B5150(em, 0x86, 0xB, 0x10);
        sound_call_005B5150(em, 0xA0, 8, 0x10);
        sound_call_005B5150(em, 0x26, 0x14, 0);
        sound_call_005B5150(em, 0x52, 5, 0x14);
        sound_call_005B5150(em, 0xCE, 4, 0x14);
        break;
    case 0x3EC:
        sound_call_005B5150(em, 0x60, 0xB, 0x10);
        sound_call_005B5150(em, 0xCC, 0xB, 0x10);
        break;
    case 0x3ED:
        sound_call_005B5150(em, 0x46, 7, 0x10);
        sound_call_005B5150(em, 0x6A, 8, 0x10);
        break;
    case 0x3EE:
        sound_call_005B5150(em, 0x20, 8, 0x10);
        break;
    case 0x3EF:
        sound_call_005B5150(em, 0x2C, 7, 0x10);
        sound_call_005B5150(em, 0x28, 3, 6);
        sound_call_005B5150(em, 0x26, 2, 0xA);
        if (em_frame_check(em, 40.0f, 0) != 0) {
            Shell11_set(em, 0xC);
            break;
        }
        break;
    case 0x3F3:
        sound_call_005B5150(em, 0x20, 1, 6);
        sound_call_005B5150(em, 0x6A, 1, 0xA);
        sound_call_005B5150(em, 0xBA, 1, 6);
        sound_call_005B5150(em, 0x2E, 4, 0x14);
        sound_call_005B5150(em, 0x84, 4, 0x17);
        sound_call_005B5150(em, 0xD0, 4, 0x14);
        break;
    case 0x3F4:
        sound_call_005B5150(em, 0x26, 3, 6);
        sound_call_005B5150(em, 0x30, 3, 0xA);
        sound_call_005B5150(em, 0xC, 6, 0x17);
        sound_call_005B5150(em, 0x12, 6, 0x14);
        break;
    case 0x3F5:
        sound_call_005B5150(em, 0x14, 3, 0xA);
        sound_call_005B5150(em, 0x4E, 3, 0xA);
        sound_call_005B5150(em, 0x34, 3, 6);
        sound_call_005B5150(em, 0x1A, 6, 0x14);
        sound_call_005B5150(em, 0x3A, 6, 0x17);
        if (em_frame_check(em, 24.0f, 0) != 0) {
            Eft13_set_em(em, 0x17, 0x14);
        }
        if (em_frame_check(em, 52.0f, 0) != 0) {
            Eft13_set_em(em, 0x14, 0x14);
            break;
        }
        break;
    case 0x3FD:
        sound_call_005B5150(em, 0x40, 1, 6);
        sound_call_005B5150(em, 0x60, 1, 0xA);
        sound_call_005B5150(em, 0x8A, 4, 0x17);
        sound_call_005B5150(em, 0xAE, 4, 0x14);
        break;
    case 0x3FE:
        sound_call_005B5150(em, 0x40, 1, 0xA);
        sound_call_005B5150(em, 0x60, 1, 6);
        sound_call_005B5150(em, 0x8A, 4, 0x14);
        sound_call_005B5150(em, 0xAE, 4, 0x17);
        break;
    case 0x406:
        sound_call_005B5150(em, 0x46, 7, 0x10);
        sound_call_005B5150(em, 0x46, 4, 0x14);
        sound_call_005B5150(em, 0x20, 4, 0x17);
        sound_call_005B5150(em, 0x36, 2, 6);
        sound_call_005B5150(em, 0x18, 2, 0xA);
        break;
    case 0x407:
        sound_call_005B5150(em, 4, 0xE, 0);
        sound_call_005B5150(em, 0x2E, 0x14, 0x1B);
        sound_call_005B5150(em, 0x3C, 7, 0x10);
        sound_call_005B5150(em, 0x34, 4, 0x14);
        sound_call_005B5150(em, 0x7A, 1, 0x17);
        sound_call_005B5150(em, 0xA6, 5, 0x14);
        sound_call_005B5150(em, 0xD2, 2, 0xA);
        if (em_frame_check(em, 30.0f, 0) != 0) {
            if (em->kind == 12) {
                Shell11_set(em, 0);
                break;
            }
            Shell11_set(em, 8);
            break;
        }
        break;
    case 0x408:
        sound_call_005B5150(em, 0x46, 0x13, 0x10);
        sound_call_005B5150(em, 0xE6, 0x13, 0x10);
        sound_call_005B5150(em, 0xCE, 4, 0x14);
        sound_call_005B5150(em, 0x144, 4, 0x14);
        sound_call_005B5150(em, 0x38, 4, 0x17);
        sound_call_005B5150(em, 0xA2, 4, 0x17);
        sound_call_005B5150(em, 0x4C, 2, 6);
        sound_call_005B5150(em, 0x7A, 2, 6);
        sound_call_005B5150(em, 0xFC, 2, 6);
        sound_call_005B5150(em, 0x128, 2, 6);
        sound_call_005B5150(em, 0x62, 2, 0xA);
        sound_call_005B5150(em, 0x90, 2, 0xA);
        sound_call_005B5150(em, 0xE6, 2, 0xA);
        sound_call_005B5150(em, 0x118, 2, 0xA);
        break;
    case 0x409:
    case 0x40A:
        sound_call_005B5150(em, 0x2C, 4, 0x14);
        sound_call_005B5150(em, 0x70, 4, 0x14);
        sound_call_005B5150(em, 0x1C, 4, 0x17);
        sound_call_005B5150(em, 0x5C, 4, 0x17);
        sound_call_005B5150(em, 0x3A, 2, 6);
        sound_call_005B5150(em, 0x7C, 2, 6);
        sound_call_005B5150(em, 0x26, 2, 0xA);
        sound_call_005B5150(em, 0x4A, 2, 0xA);
        break;
    case 0x40B:
        sound_call_005B5150(em, 0x2A, 4, 0x14);
        sound_call_005B5150(em, 0x38, 1, 6);
        sound_call_005B5150(em, 0x5A, 4, 0x17);
        sound_call_005B5150(em, 0x6A, 1, 0xA);
        sound_call_005B5150(em, 0x5A, 0xA, 0x10);
        break;
    case 0x40C:
        if (em_frame_check(em, 20.0f, 0) != 0) {
            Shell11_set(em, 1);
        }
        sound_call_005B5150(em, 0x1A, 0xE, 0x14);
        sound_call_005B5150(em, 0x1A, 0x14, 0x10);
        sound_call_005B5150(em, 0x14, 0x13, 0x10);
        sound_call_005B5150(em, 0x14, 3, 6);
        sound_call_005B5150(em, 0x18, 6, 0x17);
        sound_call_005B5150(em, 0x32, 3, 6);
        sound_call_005B5150(em, 0x44, 4, 0x14);
        break;
    case 0x40D:
        if (em_frame_check(em, 24.0f, 0) != 0) {
            if (em->kind == 12) {
                Shell11_set(em, 2);
            } else {
                Shell11_set(em, 9);
            }
        }
        sound_call_005B5150(em, 0x14, 7, 0x10);
        sound_call_005B5150(em, 0x5C, 7, 0x10);
        sound_call_005B5150(em, 0x1E, 0x14, 0x1B);
        sound_call_005B5150(em, 0x5C, 0x14, 0x1B);
        sound_call_005B5150(em, 0xA, 4, 0x14);
        sound_call_005B5150(em, 0x50, 4, 0x14);
        sound_call_005B5150(em, 0x1E, 4, 0x17);
        sound_call_005B5150(em, 0x5E, 4, 0x17);
        sound_call_005B5150(em, 0x2C, 1, 0xA);
        sound_call_005B5150(em, 0x74, 1, 0xA);
        sound_call_005B5150(em, 0x18, 1, 6);
        sound_call_005B5150(em, 0x42, 1, 6);
        break;
    case 0x424:
        if (em_frame_check(em, 2.0f, 0) != 0) {
            Eft16_set_ex(em, 3, 0xD);
            Eft16_set_ex(em, 4, 0xE);
            break;
        }
        break;
    case 0x425:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x10, 1, 6);
        sound_call_005B5150(em, 0x1A, 1, 0xA);
        sound_call_005B5150(em, 0x68, 0xE, 0x1B);
        sound_call_005B5150(em, 0x6E, 1, 0xA);
        sound_call_005B5150(em, 0x8C, 1, 0x17);
        break;
    case 0x426:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x10, 1, 0xA);
        sound_call_005B5150(em, 0x1A, 1, 6);
        sound_call_005B5150(em, 0x68, 0xE, 0x1B);
        sound_call_005B5150(em, 0x6E, 1, 6);
        sound_call_005B5150(em, 0x8C, 1, 0x14);
        break;
    case 0x427:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x1C, 0x12, 0);
        sound_call_005B5150(em, 0x22, 0xF, 0);
        sound_call_005B5150(em, 0x44, 3, 0xA);
        sound_call_005B5150(em, 0x50, 4, 0x17);
        if (em_frame_check(em, 28.0f, 0) != 0) {
            Eft20_set(0.4f * em->scale[0], em, 0, 0);
            break;
        }
        break;
    case 0x428:
        sound_call_005B5150(em, 0x14, 0x10, 0x10);
        sound_call_005B5150(em, 4, 0x16, 0x10);
        sound_call_005B5150(em, 0x64, 0x15, 0);
        sound_call_005B5150(em, 0xC2, 0x15, 0);
        sound_call_005B5150(em, 0x32, 0xE, 0x14);
        sound_call_005B5150(em, 0x96, 0xE, 0x14);
        if (em_frame_check(em, 46.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
        }
        if (em_frame_check(em, 88.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
        }
        if (em_frame_check(em, 190.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
            break;
        }
        break;
    case 0x429:
        sound_call_005B5150(em, 6, 8, 0x10);
        sound_call_005B5150(em, 0x42, 8, 0x10);
        break;
    case 0x42A:
        sound_call_005B5150(em, 0xA, 0xE, 6);
        sound_call_005B5150(em, 0x14, 0xA, 0x10);
        sound_call_005B5150(em, 0x30, 3, 6);
        sound_call_005B5150(em, 0x4E, 5, 0x14);
        sound_call_005B5150(em, 0x64, 4, 0x17);
        sound_call_005B5150(em, 0x82, 1, 6);
        break;
    case 0x42B:
        sound_call_005B5150(em, 0x1A, 1, 6);
        sound_call_005B5150(em, 0x1A, 0xE, 6);
        sound_call_005B5150(em, 0x18, 0xE, 0x17);
        sound_call_005B5150(em, 0x54, 0x12, 0);
        sound_call_005B5150(em, 0x60, 0xF, 0);
        sound_call_005B5150(em, 0x7E, 3, 6);
        sound_call_005B5150(em, 0x8C, 4, 0x14);
        break;
    case 0x42C:
        sound_call_005B5150(em, 2, 9, 0x10);
        sound_call_005B5150(em, 0xB4, 9, 0x10);
        v[2] = 20.0f;
        v[0] = 0.0f;
        v[1] = 0.0f;
        em_sleep_eff_set(em, 15, v, 1.2f);
        break;
    case 0x42D:
        sound_call_005B5150(em, 6, 0x10, 0x10);
        break;
    }
}


void em12_effect_move(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_005B4190(em, w);
        break;
    }
}

static void sound_call_005B5150(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        if (em->scale[0] == 0.5f) {
            Em_se_req2(em, se + 0x20, 0, pos, 5, 0);
        } else {
            Em_se_req2(em, se, 0, pos, 5, 0);
        }
    }
}

void em12_local_init(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void dummy_em_prog_005B5240(void) {
}
