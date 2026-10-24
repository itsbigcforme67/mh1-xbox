/* eft13c - SLPM_654.95 0x00107C50-0x00108620: eft13_set_pos, the spawn point of a dust / splash effect (eft13) for a
 * player or monster and effect type arg (a joint position, a point in front of the feet, a fixed spot), then the
 * water check (eft13_water_ck). Part of eft13 (whole file 0x00105B10-0x00109E28, near-match C in eft13_nm.c).
 * Matched by agent B (12 Oct 2026): arg is s16 (the original passes it on to eft13_water_ck unchanged) and the
 * joint number goes to a K&R get_joint_wmat (no sign extension at the call). */
#include "eft.h"
#include "em.h"
#include "game.h"
#include "prim.h"
#include "fl.h"
#include "clay.h"
#ifndef __MWERKS__
unsigned char GetWaterHit(f32 *, f32 *);   /* (declared before its first use) */
#endif
void flvecRotY(f32 *, f32); /* was implicit: the angle went as a double */

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

/* Owner fields used here (player or monster). */
typedef struct EFT13_CHR {
    u8 be_flag;         /* 0x000 */
    u8 x01;             /* 0x001 */
    u8 kind;            /* 0x002 */
    u8 _pad003[0xA0 - 0x03];
    s32 ang[3];         /* 0x0A0 */
    f32 pos[3];         /* 0x0AC */
    f32 scale[3];       /* 0x0B8 */
    u8 _pad0C4[0x2DC - 0xC4];
    u16 char0;          /* 0x2DC */
    u8 _pad2DE[0x5AC - 0x2DE];
    f32 x5AC;           /* 0x5AC ground height */
    u8 _pad5B0[0x909 - 0x5B0];
    u8 x909;            /* 0x909 */
} EFT13_CHR;

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT13_PIECE {
    s16 no;             /* 0x00 0xFF = unused */
    s16 time;           /* 0x02 */
    f32 pos[3];         /* 0x04 */
    f32 scale[3];       /* 0x10 from the keyframes */
    f32 size;           /* 0x1C */
    f32 alpha;          /* 0x20 */
    u16 ang;            /* 0x24 */
    u16 rot;            /* 0x26 */
    s16 drot;           /* 0x28 */
    s16 prim_no;        /* 0x2A */
    PRIM *prim;         /* 0x2C */
} EFT13_PIECE;

extern s16 eft13_num[35];
extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern u8 Eft_kemuri_rgb[11][4];
extern u8 eft13_type26_rgb[8][3];

void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub(CLAY *, FLMAT *, u16, f32, void *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetFilterMode(int);
extern s16 eft13_water_flag[35];
extern s16 Eft_stg_type[];

u8 Pl_stg_ck(void *);
u8 Em_stg_ck(void *);
void release_prim(s16);
FLMAT *get_joint_wmat();
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
u16 calc_mat_angY(FLMAT *);
#ifdef __MWERKS__
int GetWaterHit(f32 *, f32 *);
#else
unsigned char GetWaterHit(f32 *, f32 *);   /* the real return type (tools/check_kr_returns.py) */
#endif
void se_req2(int, int, int, f32 *, int, int);
void func_544C90(f32 *, int, int, f32);    /* game.bin Eft08_set */
void func_544D20(void *, int, int, f32, f32); /* game.bin Eft08_set2 */

void eft13_move(EFTW *ew);
void eft13_i(EFTW *ew);
void eft13_m(EFTW *ew);
void eft13_d(EFTW *ew);
void eft13_e(EFTW *ew);
s16 eft13_set_pos(f32 *pos, EFT13_CHR *chr, int j, s16 arg);
s16 eft13_set_pos_em(f32 *pos, EFT13_CHR *chr, s16 j, int arg);
void eft13_set_sub_em(EFT13_CHR *chr, s16 j, int arg, EFTW *ew);
s8 eft13_water_set(EFT13_CHR *chr, f32 *pos, s16 kind, f32 scale);


u32 ran_suu(int);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void get_joint_pos(void *, int, f32 *);
void get_joint_pos_em(void *, int, f32 *);
FLMAT *get_joint_wmat_em(void *, int);
f32 GetGroundHit(f32 *);
void AddVector(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void eft13_t(PRIM *pr);
void eft13_se_req(EFTW *ew);
void Eft13_set_scl(EFT13_CHR *chr, int j, int arg, f32 scale);
s16 eft13_water_ck(EFT13_CHR *chr, f32 *pos, s16 arg);
void func_53FDF0(f32 *, u16, int, f32);    /* game.bin Eft17_set_ex */
void func_628690(EFTW *, int);             /* game.bin shell01_set2 */
void func_62A2C0(EFTW *, int);             /* game.bin shell05_set3 */
extern void *eft13_data[58];
extern s16 eft13_all_time[35];
extern s16 eft13_index[35];
extern s16 eft13_param[35];
extern s16 eft13_type14_lag_tbl[3];
extern s16 eft13_type14_time_tbl[3];
extern s16 eft13_type19_lag_tbl[8];
extern s16 eft13_type21_time_tbl[6];

#define RAND_ROT(k) (s16)(0.5f + 65536.0f * ((k) * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200)) / 360.0f)
#define ROT_ADD(pos, chr, x, y, z)                         \
    do {                                                   \
        v[0] = (x);                                        \
        v[1] = (y);                                        \
        v[2] = (z);                                        \
        flvecRotY(v, DEG2RAD(ANG2DEG((chr)->ang[1])));     \
        (pos)[0] += v[0];                                  \
        (pos)[1] += v[1];                                  \
        (pos)[2] += v[2];                                  \
    } while (0)
#define MAT_ADD(pos, x, y, z)                              \
    do {                                                   \
        v[0] = (x);                                        \
        v[1] = (y);                                        \
        v[2] = (z);                                        \
        flvecApplyMat33_2(v, &m);                          \
        (pos)[0] = m[3][0] + v[0];                         \
        (pos)[1] = m[3][1] + v[1];                         \
        (pos)[2] = m[3][2] + v[2];                         \
    } while (0)

s16 eft13_set_pos(f32 *pos, EFT13_CHR *chr, int j, s16 arg) {
    FLMAT m;
    f32 v[3];
    f32 g;
    s16 k;

    switch ((s16)arg) {
    case 0:
    case 3:
    case 4:
    case 6:
    case 7:
    case 9:
    case 18:
    case 20:
        get_joint_pos(chr, j, pos);
        break;
    case 5:
        get_joint_pos(chr, j, pos);
        v[1] = -19.0f;
        v[0] = 0.0f;
        v[2] = -13.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(chr->ang[1])));
        AddVector(pos, pos, v);
        break;
    case 8:
#ifdef VERSION_MHG
    case 39:
#endif
        pos[0] = chr->pos[0];
        pos[1] = chr->x5AC;
        pos[2] = chr->pos[2];
        if (chr->char0 == 0x13) {
            ROT_ADD(pos, chr, 0.0f, 0.0f, 50.0f);
        }
        break;
    case 10:
        get_joint_pos(chr, j, pos);
        break;
    case 11:
        get_joint_pos(chr, j, pos);
        pos[1] = chr->x5AC;
        ROT_ADD(pos, chr, 0.0f, 0.0f, 20.0f);
        break;
    case 12:
    case 19:
        flmatCopy(&m, get_joint_wmat(chr, j));
        MAT_ADD(pos, 44.0f, 0.0f, 123.0f);
        g = GetGroundHit(pos);
        if (pos[1] < 50.0f + g) {
            pos[1] = g;
        }
        break;
    case 13:
        get_joint_pos(chr, j, pos);
        pos[1] = chr->x5AC;
        break;
    case 14:
    case 15:
        flmatCopy(&m, get_joint_wmat(chr, j));
        MAT_ADD(pos, 44.0f, 0.0f, 123.0f);
        pos[1] = GetGroundHit(pos);
        break;
    case 16:
        pos[0] = chr->pos[0];
        pos[1] = chr->x5AC;
        pos[2] = chr->pos[2];
        ROT_ADD(pos, chr, 0.0f, 0.0f, -50.0f);
        break;
    case 17:
        get_joint_pos(chr, j, pos);
        pos[1] = chr->x5AC;
        v[2] = -25.0f;
        v[0] = 0.0f;
        v[1] = 0.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(chr->ang[1])));
        AddVector(pos, pos, v);
        break;
    case 21:
        flmatCopy(&m, get_joint_wmat(chr, j));
        switch (chr->kind) {
        case 0:
            v[0] = 10.0f;
            v[1] = -2.0f;
            v[2] = 190.0f;
            break;
        case 4:
#ifdef VERSION_MHG
        case 6:
#endif
            v[0] = -4.9f;
            v[1] = -0.7f;
            v[2] = 90.0f;
            break;
        case 3:
            v[0] = -7.8f;
            v[1] = -10.0f;
            v[2] = 220.0f;
            break;
        case 2:
            v[0] = -5.0f;
            v[1] = -20.0f;
            v[2] = 155.0f;
            break;
        default:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            break;
        }
        flvecApplyMat33_2(v, &m);
        pos[0] = m[3][0] + v[0];
        pos[1] = m[3][1] + v[1];
        pos[2] = m[3][2] + v[2];
        break;
    case 22:
        flmatCopy(&m, get_joint_wmat(chr, j));
        MAT_ADD(pos, 0.0f, -2.0f, 16.0f);
        break;
    case 23:
#ifdef VERSION_MHG
    case 35:
#endif
        get_joint_pos(chr, j, pos);
        break;
    case 24:
        flmatCopy(&m, get_joint_wmat(chr, j));
        v[1] = 0.0f;
        v[0] = -5.0f;
        v[2] = 50.0f;
        flvecApplyMat33_2(v, &m);
        pos[0] = m[3][0] + v[0];
        pos[1] = chr->x5AC;
        pos[2] = m[3][2] + v[2];
        break;
    case 25:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 125.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(chr->ang[1])));
        pos[0] = chr->pos[0] + v[0];
        pos[1] = chr->pos[1] + v[1];
        pos[2] = chr->pos[2] + v[2];
        break;
    case 26:
        flmatCopy(&m, get_joint_wmat(chr, j));
        MAT_ADD(pos, 5.0f, -15.0f, 10.0f);
        break;
    case 31:
        flmatCopy(&m, get_joint_wmat(chr, j));
        MAT_ADD(pos, 44.0f, 0.0f, 123.0f);
        g = GetGroundHit(pos);
        if (pos[1] < 50.0f + g) {
            pos[1] = g;
        }
        break;
    case 33:
        pos[0] = 13446.23f;
        pos[1] = 70.0f;
        pos[2] = 14372.39f;
        break;
#ifdef VERSION_MHG
    case 36:        /* G: Dual Swords demon mode effects */
        get_joint_pos(chr, j, pos);
        pos[1] = 90.0f + chr->x5AC;
        break;
    case 37:
    case 38:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 130.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(chr->ang[1])));
        pos[0] = chr->pos[0] + v[0];
        pos[1] = chr->x5AC;
        pos[2] = chr->pos[2] + v[2];
        break;
#endif
    }
    k = eft13_water_ck(chr, pos, arg);
    if ((s16)arg == 0x13 && k == 0) {
        v[0] = pos[0];
        v[1] = 20.0f + pos[1];
        v[2] = pos[2];
        func_53FDF0(v, chr->ang[1], 10, 0.4f);
        Eft13_set_scl(chr, 14, 14, 1.0f);
    }
    return k;
}
