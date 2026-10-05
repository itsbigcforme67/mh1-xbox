/* eft22 - game.bin 0x00556FE0-0x00557474, after eft22_end_init (still
 * assembly). See eft22.c. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "uki.h"
#include "fl.h"
#include "clay.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern EFT_MDLW *eft_mdlw[5];
extern u32 eft22_pl_rgb[];      /* float colour per player */
extern f32 sao_top_ofs[][3];    /* rod tip offset per bend */
extern u8 ot1[];

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

u32 ran_suu(int);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void get_joint_pos(PLW *, int, f32 *);

void flvecNormalize(f32 *);
void release_prim(s16);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void flExecuteClay(s32, int);
void Material_set_sub(void *, CLAY *);
f32 flvecCalcLength(f32 *);
FLMAT *get_joint_wmat(PLW *, int);
int frame_check2(PLW *, int, f32);
void se_req2(int, int, int, f32 *, int, int);
void eft22_sao_pos(f32 *out, PLW *pl);
void eft22_v0_calc(f32 *vel, f32 from, f32 to, f32 grav, f32 time);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
int pl_flag_ck(PLW *, int);
int frame_check(PLW *, int, f32);
u8 Pl_stg_ck(PLW *);
void Eft20_set2(f32, f32 *, int, int);
void Eft08_set(f32 *, int, int, f32);
void eft22_end_init(EFTW *ew, UKI *w);
void eft22_line_sub(EFTW *ew, UKI *w);
void eft22_rate_add(EFTW *ew, UKI *w);
void eft22_se_req(EFTW *ew, f32 *pos, s16 kind);

void eft22_line_sub(EFTW *ew, UKI *w) {
    switch (ew->mode2) {
    case 3:
        if (w->line_on == 0) {
            if (((u16)ran_suu(1) & 7) == 0) {
                w->line_on = 1;
                w->line_cnt = 0;
            }
        } else if (++w->line_cnt >= 4) {
            w->line_on = 0;
        }
        break;
    default:
        w->line_on = 0;
        break;
    }
}

void eft22_sao_pos(f32 *out, PLW *pl) {
    f32 v0[3];
    f32 v1[3];
    FLMAT m1;
    FLMAT m2;
    s16 n;

    switch (pl->char0) {
    case 0x327:
        if (frame_check2(pl, 0, 20.0f) == 0) {
            n = 0;
        } else if (frame_check2(pl, 0, 30.0f) == 0) {
            n = 1;
        } else {
            n = 2;
        }
        break;
    case 0x328:
        if (frame_check2(pl, 0, 20.0f) == 0) {
            n = 1;
        } else if (frame_check2(pl, 0, 26.0f) == 0) {
            n = 2;
        } else if (frame_check2(pl, 0, 28.0f) == 0) {
            n = 1;
        } else {
            n = 0;
        }
        break;
    case 0x329:
        if (frame_check2(pl, 0, 36.0f) == 0) {
            n = 1;
        } else if (frame_check2(pl, 0, 40.0f) == 0) {
            n = 2;
        } else if (frame_check2(pl, 0, 42.0f) == 0) {
            n = 1;
        } else {
            n = 0;
        }
        break;
    case 0x32A:
        if (frame_check2(pl, 0, 20.0f) == 0) {
            n = 1;
        } else if (frame_check2(pl, 0, 22.0f) != 0) {
            n = 2;
        } else if (frame_check2(pl, 0, 24.0f) != 0) {
            n = 1;
        } else {
            n = 0;
        }
        break;
    default:
        n = 0;
        break;
    }
    flmatCopy(&m1, get_joint_wmat(pl, 0x12));
    v0[0] = -9.2f;
    v0[1] = -1.7f;
    v0[2] = 12.0f;
    flvecApplyMat33_2(v0, &m1);
    flmatInit(&m2);
    flmatRotZ33(&m2, -1.5707964f);
    flmatMul33_2(&m2, &m1);
    v1[0] = sao_top_ofs[n][0];
    v1[1] = sao_top_ofs[n][1];
    v1[2] = sao_top_ofs[n][2];
    flvecApplyMat33_2(v1, &m2);
    out[0] = v1[0] + (m1[3][0] + v0[0]);
    out[1] = v1[1] + (m1[3][1] + v0[1]);
    out[2] = v1[2] + (m1[3][2] + v0[2]);
}

void eft22_se_req(EFTW *ew, f32 *pos, s16 kind) {
    switch (kind) {
    case 0:
        se_req2(1, 0x2E, 0, pos, 1, 0);
        break;
    case 1:
        se_req2(1, 0x70, 0, pos, 1, 0);
        break;
    }
}
