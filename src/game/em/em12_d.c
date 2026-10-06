/* em12_d - game.bin 0x005B06E0-0x005B0AE0: em_mov01_005B06E0 (em12 move 1 state machine). Whole file in em12_nm.c. */
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

#define EM12_WALK(em, v, k) \
    do { \
        v[0] = (k) * em->scale[0]; \
        v[1] = 0.0f; \
        v[2] = 0.0f; \
        flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1]))); \
        em->pos[0] += v[0]; \
        em->pos[2] += v[2]; \
    } while (0)

#define EM12_REV_POS(em, tbl, v, act) \
    do { \
        flvecCopy(em->pos, tbl[em->type]); \
        flvecCopy(em->x5A0, em->pos); \
        flvecCopy(v, &tbl[em->type][3]); \
        em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF; \
        em_act_set(em, 7, act); \
        em->x798 = 0.0f; \
    } while (0)

/* Sound and effect script per animation (sound_call(em, frame, se, joint)
 * plays a sound at the joint once the animation reaches the frame). */
static void sound_call_005B5150(EMW *em, int frame, int se, int joint);

void em_mov01_005B06E0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x406) {
            *(f32 *)&em->x3AC = 400.0f;
        }
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            if (em_search_set(em, 0x78E4, 700.0f, 300.0f) != -1) {
                if ((u16)(em->horm_ang - em->ang[1]) < 0x3000) {
                    em->x05 = 4;
                    em_char_set(em, 0x22, 0, 0);
                    em->x07 = 0;
                    w->x0A = 0;
                } else if ((u16)(em->horm_ang - em->ang[1]) > 0xD000) {
                    em->x05 = 4;
                    em_char_set(em, 0x21, 0, 0);
                    em->x07 = 1;
                    w->x0A = 0;
                } else if ((u16)(em->horm_ang - em->ang[1]) > 0x5400 && (u16)(em->horm_ang - em->ang[1]) < 0xA600) {
                    em_act_set(em, 3, 0);
                } else {
                    em->x05++;
                    em_char_set(em, 0x20, 0, 0);
                }
            } else {
                em12_next_act_set(em);
            }
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05++;
            em->work08 = (em->x39A & 3) * 30 + 0xD2;
        }
        break;
    case 3:
        if (em_search_set(em, 0x8000, 500.0f, 200.0f) != -1) {
            int d = (u16)(em->horm_ang - em->ang[1]);

            if (d > 0x5400 && d < 0xA600) {
                em_act_set(em, 3, 0);
            } else {
                em->work08--;
                if (em_frame_check2(em, 0, 14.0f) != 0) {
                    if (d < 0x8000) {
                        if (em->work08 <= 0) {
                            em->x05++;
                            em_char_set(em, 0x22, 0, 0);
                            em->x07 = 0;
                            w->x0A = 0;
                        }
                        em->ang[1] -= 0x42;
                    } else {
                        if (em->work08 <= 0) {
                            em->x05++;
                            em_char_set(em, 0x21, 0, 0);
                            em->x07 = 1;
                            w->x0A = 0;
                        }
                        em->ang[1] += 0x42;
                    }
                }
            }
        } else {
            em->x05 = 5;
            em_char_set(em, 0x23, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05 = 2;
            em_char_set(em, 0x20, 0, 0);
        } else {
            if (em_frame_check(em, 6.0f, 0) != 0) {
                w->x0A++;
            } else if (em_frame_check(em, 122.0f, 0) != 0) {
                w->x0A = 0;
            }
            if (w->x0A != 0) {
                if (em->x07 == 0) {
                    em->ang[1] -= 0x1D7;
                } else {
                    em->ang[1] += 0x1D7;
                }
            }
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}
