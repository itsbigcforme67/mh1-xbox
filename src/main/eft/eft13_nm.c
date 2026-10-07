/* eft13_nm - NOT BUILT. Near-match C for the five eft13 functions still in
 * asm: eft13_i, eft13_m, eft13_set_pos, eft13_set_sub_em, eft13_set_pos_em.
 * Written from m2c drafts checked against the asm; believed equivalent, not
 * register/order-matched (see docs/agents/agent-D.md). */
#include "eft.h"
#include "em.h"
#include "game.h"
#include "prim.h"
#include "fl.h"
#include "clay.h"
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
FLMAT *get_joint_wmat(void *, s16);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
u16 calc_mat_angY(FLMAT *);
int GetWaterHit(f32 *, f32 *);
void se_req2(int, int, int, f32 *, int, int);
void func_544C90(f32 *, int, int, f32);    /* game.bin Eft08_set */
void func_544D20(void *, int, int, f32, f32); /* game.bin Eft08_set2 */

void eft13_move(EFTW *ew);
void eft13_i(EFTW *ew);
void eft13_m(EFTW *ew);
void eft13_d(EFTW *ew);
void eft13_e(EFTW *ew);
s16 eft13_set_pos(f32 *pos, EFT13_CHR *chr, int j, int arg);
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

void eft13_i(EFTW *ew) {
    f32 v[3];
    EFT13_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft13_num[ew->arg];
    switch (ew->arg) {
    case 9:
        if (ew->mode2 == 1) {
            v[0] = 10.0f;
            v[1] = 0.0f;
            v[2] = -100.0f;
        } else {
            v[0] = -10.0f;
            v[1] = 0.0f;
            v[2] = -100.0f;
        }
        flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        break;
    case 19:
    case 20:
    case 21:
    case 26:
    case 29:
        ew->u0A.joint = ran_suu(1);
        break;
    }
    eft13_se_req(ew);
    for (i = 0; i < n; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->size = 1.0f;
            p->time = 0;
            switch (ew->arg) {
            case 0:
                p->size = ew->scale;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.001f);
                break;
            case 3:
            case 18:
            case 30:
            case 32:
            case 33:
                p->size = ew->scale;
                p->rot = RAND_ROT(0.09f);
                break;
            case 5:
            case 10:
                p->size = ew->scale * (1.0f + 0.0005f * ((u16)ran_suu(1) & 0x3FF));
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.001f);
                break;
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 11:
            case 12:
            case 15:
            case 17:
            case 23:
            case 24:
            case 25:
                p->size = ew->scale;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.001f);
                break;
            case 13:
                p->time = i * -2;
                p->size = ew->scale * (1.0f + 0.2f * (0.001f * ((u16)ran_suu(1) & 0x3FF)));
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.001f);
                break;
            case 14:
                p->size = ew->scale;
                p->time = eft13_type14_lag_tbl[i];
                switch (i) {
                case 0:
                    p->no = 0;
                    break;
                case 1:
                case 2:
                    p->no = 1;
                    break;
                }
                p->rot = 0;
                p->drot = 0;
                break;
            case 16:
            case 31:
                p->time = -1;
                p->size = ew->scale;
                p->rot = 0;
                p->drot = 0;
                break;
            case 19:
                p->time = eft13_type19_lag_tbl[i] - 6;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.002f);
                break;
            case 20:
            case 27:
                p->time = 0;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                break;
            case 21:
            case 26:
            case 29:
                p->size = ew->scale;
                if (i == 0) {
                    p->size *= 2.0f;
                    p->time = 0;
                } else if (i == 5) {
                    p->time = -2;
                } else {
                    p->time = 0;
                }
                p->rot = 0;
                p->drot = 0;
                break;
            case 22:
            case 28:
                p->time = 0;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                p->drot = (ran_suu(1) & 0xFF) - 0x80;
                break;
            case 34:
                p->time = i * -3;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                flvecCopy(p->pos, ew->pos);
                break;
            }
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft13_t;
        } else {
            p->prim = 0;
        }
    }
}

void eft13_m(EFTW *ew) {
    FLMAT m;
    f32 v[3];
    EFT13_PIECE *p = ew->work;
    u16 all = eft13_all_time[ew->arg];
    s16 idx = eft13_index[ew->arg];
    s16 step = eft13_param[ew->arg];
    s16 n;
    s16 dang;
    s16 i;
    u16 time;
    int ang;
    void *d;

    switch (ew->arg) {
    case 6:
        dang = 0x2AAA;
        break;
    case 8:
        dang = 0x2AAA;
        break;
    case 7:
    case 11:
    case 12:
    case 15:
    case 23:
    case 24:
    case 25:
        dang = 0x3333;
        break;
    case 19:
        dang = 0x2000;
        break;
    case 21:
    case 26:
    case 29:
        ew->u0A.joint += 0xB6;
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    n = eft13_num[ew->arg];
    ang = 0;
    for (i = 0; i < n; i++, ang += dang) {
        switch (ew->arg) {
        case 6:
        case 7:
        case 8:
        case 11:
        case 12:
        case 15:
        case 17:
        case 19:
        case 23:
        case 24:
        case 25:
            idx = eft13_index[ew->arg];
        default:
            time = all;
            break;
        case 13:
            idx = eft13_index[ew->arg];
            time = 12;
            break;
        case 14:
            time = eft13_type14_time_tbl[i];
            break;
        case 21:
        case 26:
        case 29:
            time = eft13_type21_time_tbl[i];
            break;
        case 34:
            idx = eft13_index[ew->arg];
            time = 14;
            break;
        }
        if (++p->time <= 0) {
            p++;
            idx += step;
            continue;
        }
        if (p->time == 1) {
            if (ew->arg == 13) {
                if (ew->owner->be_flag == 0) {
                    p->no = 0xFF;
                    p++;
                    idx += step;
                    continue;
                }
                flmatCopy(&m, get_joint_wmat(ew->owner, 2));
                flmatGetTrans(p->pos, &m);
                p->pos[1] = ew->owner->x5AC;
                p->ang = calc_mat_angY(&m) + 0x6000;
            }
        } else if (p->time > time) {
            p++;
            idx += step;
            continue;
        }
        if (p->no == 0xFF) {
            return;
        }
        eft_vec_linear(p->time, eft13_data[idx], p->scale);
        switch (ew->arg) {
        case 0:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 22:
        case 23:
        case 24:
        case 25:
        case 28:
        case 30:
        case 31:
        case 32:
        case 33:
            eft_vec_linear(p->time, eft13_data[idx + 1], p->pos);
            p->pos[0] *= ew->scale;
            p->pos[1] *= ew->scale;
            p->pos[2] *= ew->scale;
            d = eft13_data[idx + 2];
            idx += 3;
            break;
        case 13:
            eft_vec_linear(p->time, eft13_data[idx + 1], v);
            v[0] *= ew->scale;
            v[1] *= ew->scale;
            v[2] *= ew->scale;
            d = eft13_data[idx + 2];
            idx += 3;
            break;
        default:
            d = eft13_data[idx + 1];
            idx += 2;
            break;
        }
        eft_alpha_linear(p->time, d, &p->alpha);
        if (p->prim != 0) {
            switch (ew->arg) {
            case 3:
            case 18:
            case 30:
            case 32:
            case 33:
                v[0] = p->pos[0];
                v[1] = p->pos[1];
                v[2] = 20.0f + p->pos[2];
                flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
                p->prim->pos[0] = v[0] + ew->pos[0];
                p->prim->pos[1] = v[1] + ew->pos[1];
                p->prim->pos[2] = v[2] + ew->pos[2];
                break;
            case 0:
            case 4:
            case 5:
            case 9:
            case 10:
            case 14:
            case 16:
            case 17:
            case 22:
            case 28:
            case 31:
                p->rot += p->drot;
                flvecCopy(v, p->pos);
                flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
                p->prim->pos[0] = v[0] + ew->pos[0];
                p->prim->pos[1] = v[1] + ew->pos[1];
                p->prim->pos[2] = v[2] + ew->pos[2];
                break;
            case 6:
            case 7:
            case 8:
            case 11:
            case 12:
            case 15:
            case 19:
            case 23:
            case 24:
            case 25:
                p->rot += p->drot;
                flvecCopy(v, p->pos);
                flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint + ang)));
                p->prim->pos[0] = v[0] + ew->pos[0];
                p->prim->pos[1] = v[1] + ew->pos[1];
                p->prim->pos[2] = v[2] + ew->pos[2];
                break;
            case 13:
                p->rot += p->drot;
                flvecRotY(v, DEG2RAD(ANG2DEG(p->ang)));
                p->prim->pos[0] = v[0] + p->pos[0];
                p->prim->pos[1] = v[1] + p->pos[1];
                p->prim->pos[2] = v[2] + p->pos[2];
                break;
            case 20:
            case 27:
                flvecCopy(p->prim->pos, ew->pos);
                break;
            case 21:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 30.0f;
                if (ew->owner != 0 && ew->owner->be_flag == 0 && ew->owner->kind == 2) {
                    v[2] = 90.0f;
                }
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                break;
            case 26:
            case 29:
                p->prim->pos[0] = ew->pos[0];
                p->prim->pos[1] = ew->pos[1];
                p->prim->pos[2] = ew->pos[2];
                break;
            case 34:
                p->pos[1] += 2.0f;
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
                break;
            }
            add_prim(ot0, p->prim, 0x40, 0);
        }
        p++;
    }
}

s16 eft13_set_pos(f32 *pos, EFT13_CHR *chr, int j, int arg) {
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

void eft13_set_sub_em(EFT13_CHR *chr, s16 j, int arg, EFTW *ew) {
    FLMAT m;

    ew->type = 13;
    ew->move = eft13_move;
    ew->arg = arg;
    ew->u0A.joint = chr->ang[1];
    ew->owner = (EMW *)chr;
    switch (ew->arg) {
    case 0:
        switch (chr->kind) {
        case 12:
        case 25:
            ew->scale = 4.0f * chr->scale[0];
            break;
        case 18:
            ew->scale = 0.5f * chr->scale[0];
            break;
        case 13:
        case 16:
        case 27:
        case 28:
        case 30:
        case 31:
            ew->scale = 2.0f * chr->scale[0];
            break;
        }
        break;
    case 3:
        switch (chr->kind) {
        case 9:
        case 23:
            ew->scale = 2.0f * chr->scale[0];
            break;
        case 3:
        case 12:
        case 14:
        case 15:
        case 25:
        case 26:
        case 33:
            ew->scale = chr->scale[0];
            break;
        case 4:
        case 5:
        case 32:
            break;
        }
        break;
    case 6:
        switch (chr->kind) {
        case 1:
        case 6:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 26:
            ew->scale = 1.3f * chr->scale[0];
            break;
        case 12:
        case 25:
            ew->scale = 2.0f * chr->scale[0];
            break;
        case 18:
            ew->scale = chr->scale[0];
            break;
        }
        break;
    case 7:
        switch (chr->kind) {
        case 1:
        case 11:
        case 14:
        case 15:
        case 17:
        case 20:
        case 22:
        case 26:
            ew->scale = chr->scale[0];
            if (j == 2) {
                func_628690(ew, 0xE);
            }
            break;
        case 18:
            if (j == 0) {
                ew->scale = 2.0f * chr->scale[0];
            } else {
                ew->scale = 0.5f * chr->scale[0];
            }
            break;
        case 9:
        case 23:
            ew->scale = 0.5f * chr->scale[0];
            break;
        case 4:
        case 5:
        case 12:
        case 25:
        case 32:
            ew->scale = chr->scale[0];
            break;
        }
        break;
    case 19:
        switch (chr->kind) {
        case 14:
        case 26:
            ew->scale = chr->scale[0];
            break;
        case 7:
            func_62A2C0(ew, 0x50);
            break;
        }
        /* fall through */
    case 20:
        if (chr->kind == 12 || chr->kind == 25) {
            ew->scale = 5.0f * chr->scale[0];
        }
        break;
    case 28:
        if ((game_w.x1DC != 0 && chr->kind == 1) || (game_w.x1DC == 0 && chr->kind == 10)) {
            flmatCopy(&m, get_joint_wmat_em(chr, j));
            ew->u0A.joint = calc_mat_angY(&m) + 0x4000;
        }
        break;
    case 30:
        flmatCopy(&m, get_joint_wmat_em(chr, j));
        ew->u0A.joint = calc_mat_angY(&m) - 0x4000;
        break;
    }
}

s16 eft13_set_pos_em(f32 *pos, EFT13_CHR *chr, s16 j, int arg) {
    FLMAT m;
    f32 v[3];

    switch ((s16)arg) {
    case 0:
        switch (chr->kind) {
        case 12:
        case 25:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            ROT_ADD(pos, chr, 0.0f, 0.0f, -10.0f);
            break;
        case 18:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            ROT_ADD(pos, chr, 0.0f, 0.0f, -20.0f);
            break;
        case 13:
        case 16:
        case 27:
        case 28:
        case 30:
        case 31:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            break;
        }
        break;
    case 3:
        switch (chr->kind) {
        case 4:
        case 5:
        case 32:
            if (j == 0x11) {
                get_joint_pos_em(chr, j, pos);
                get_joint_pos_em(chr, 0xE, v);
                AddVector(pos, pos, v);
                ScaleVector(pos, pos, 0.5f);
            } else {
                get_joint_pos_em(chr, j, pos);
            }
            pos[1] = chr->x5AC;
            break;
        case 9:
        case 23:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            ROT_ADD(pos, chr, 0.02f * (((u16)ran_suu(1) & 0x3FF) - 0x200), 0.0f, 0.0f);
            break;
        case 2:
        case 3:
        case 6:
        case 8:
        case 12:
        case 14:
        case 15:
        case 17:
        case 20:
        case 21:
        case 22:
        case 25:
        case 26:
        case 33:
        case 34:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            break;
        }
        break;
    case 6:
        switch (chr->kind) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 11:
        case 14:
        case 15:
        case 17:
        case 18:
        case 20:
        case 21:
        case 26:
        case 32:
        case 33:
        case 34:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            break;
        case 12:
        case 25:
            pos[0] = chr->pos[0];
            pos[1] = chr->x5AC;
            pos[2] = chr->pos[2];
            break;
        }
        break;
    case 7:
        switch (chr->kind) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 6:
        case 8:
        case 11:
        case 12:
        case 14:
        case 15:
        case 18:
        case 20:
        case 21:
        case 25:
        case 26:
        case 32:
        case 34:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            break;
        case 9:
        case 23:
            get_joint_pos_em(chr, j, pos);
            break;
        case 17:
        case 22:
            flmatCopy(&m, get_joint_wmat_em(chr, j));
            v[1] = 0.0f;
            v[0] = 100.0f;
            v[2] = 0.0f;
            flvecApplyMat33_2(v, &m);
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            pos[0] += v[0];
            pos[2] += v[2];
            break;
        }
        break;
    case 19:
        if (chr->kind == 7 || chr->kind == 0x16 || chr->kind == 0x1A || chr->kind == 0xE) {
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
        }
        /* fall through */
    case 20:
        switch (chr->kind) {
        case 2:
        case 22:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            break;
        case 12:
        case 25:
            get_joint_pos_em(chr, j, pos);
            pos[1] = chr->x5AC;
            ROT_ADD(pos, chr, 0.0f, 0.0f, -10.0f);
            break;
        }
        break;
    case 28:
        if ((game_w.x1DC != 0 && chr->kind == 1) || (game_w.x1DC == 0 && chr->kind == 10)) {
            flmatCopy(&m, get_joint_wmat_em(chr, j));
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 15.0f;
            flvecApplyMat33_2(v, &m);
            flmatGetTrans(pos, &m);
            pos[0] += v[0];
            pos[1] += v[1];
            pos[2] += v[2];
        }
        break;
    case 30:
        flmatCopy(&m, get_joint_wmat_em(chr, j));
        v[0] = 0.0f;
        v[1] = -14.0f;
        v[2] = 45.0f;
        flvecApplyMat33_2(v, &m);
        flmatGetTrans(pos, &m);
        pos[0] += v[0];
        pos[1] += v[1];
        pos[2] += v[2];
        break;
    case 32:
        flmatCopy(&m, get_joint_wmat_em(chr, j));
        v[1] = 0.0f;
        v[0] = 58.2f;
        v[2] = -474.0f;
        flvecApplyMat33_2(v, &m);
        flmatGetTrans(pos, &m);
        pos[0] += v[0];
        pos[1] = chr->x5AC;
        pos[2] += v[2];
        break;
    }
    return eft13_water_ck(chr, pos, arg);
}
