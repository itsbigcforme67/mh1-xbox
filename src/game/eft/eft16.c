/* eft16 - game.bin 0x0054D660-0x00550F68, one translation unit, every function
 * matches. eft16_m needed explicit induction variables (x5 = i * 5 spilled,
 * y15 = i * 5 + 15 in a register) to get the original's spill slot order.
 * Hit effects: blood and sparks where an attack lands, with fifteen types
 * (arg). Each spawns up to eft16_num sprites that grow and fade along
 * keyframe tables (eft16_data through eft16_index/param). Type 0 sprays
 * drops along a timing table (eft16_time_tbl1..3, picked by hit kind in
 * mode2 bits 0x30) and also starts Eft02 splashes; types 3 and 4 spray from
 * a monster's neck (eft16_neck_tbl) through Eft02_set4. The colour comes
 * from Eft_blood_rgb (mode2 bits 0xC0 pick the row). Started by Eft16_set
 * (on a hit character, position kept relative to a joint), Eft16_set_ex
 * (neck) and Eft16_set_ex3 / Eft16_set_impact (at a point). Names of the
 * work fields are guesses from use. */
#include "eft.h"
#include "em.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT16_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 lag;            /* 0x02 frame counter, starts at 0 or below */
    f32 pos[3];         /* 0x04 position (type 0: velocity) */
    f32 scale[3];       /* 0x10 from the keyframes */
    s16 no;             /* 0x1C sub-kind; 0xFF = off */
    u16 rot;            /* 0x1E */
    u16 alpha;          /* 0x20 */
    u8 _pad22[2];
    f32 size;           /* 0x24 */
    f32 grav;           /* 0x28 type 0: fall speed */
    PRIM *prim;         /* 0x2C */
    s16 uv;             /* 0x30 texture frame */
} EFT16_PIECE;

/* Neck table entry: offsets for Eft16_set_ex and directions for eft16_m. */
typedef struct EFT16_NECK {
    f32 *ofs;           /* 0x00 two vectors: type 3, type 4 */
    f32 *dir;           /* 0x04 two vectors: type 3, type 4 */
} EFT16_NECK;

/* Character fields not in the shared headers yet. */
#define CHR_ANG3EC(c) (*(u16 *)((u8 *)(c) + 0x3EC))
#define CHR_X4D8(c) (*(u8 **)((u8 *)(c) + 0x4D8))

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft16_data[];
extern s16 eft16_num[];
extern s16 eft16_all_time[];
extern s16 eft16_param[];
extern s16 eft16_index[];
extern s16 eft16_time[];
extern f32 eft16_scale_tbl[16];
extern EFT16_NECK *eft16_neck_tbl[];
extern s16 eft16_time_tbl1[][2];
extern s16 eft16_time_tbl2[][2];
extern s16 eft16_time_tbl3[2][2];
extern s16 uv78_00647180[];
extern u8 Eft_blood_rgb[][4];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatInvert(FLMAT *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
FLMAT *get_joint_wmat(void *, int);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetTrnslMode(int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft02_set4(u16, u16, int, f32 *, f32);
void se_req2(int, int, int, f32 *, int, int);

static void eft16_move(EFTW *ew);
static void eft16_i(EFTW *ew);
static void eft16_m(EFTW *ew);
static void eft16_d(EFTW *ew);
static void eft16_e(EFTW *ew);
static void eft16_t(PRIM *pr);
static s16 eft16_rot(s16 no);
static s16 eft16_col_type_sel(u8 flag);
static void eft16_se_req(EFTW *ew, f32 *pos);

static void eft16_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft16_i(ew);
        break;
    case 1:
        eft16_m(ew);
        break;
    case 2:
        eft16_d(ew);
        break;
    case 3:
        eft16_e(ew);
        break;
    }
}

static void eft16_i(EFTW *ew) {
    s16 n;
    EFT16_PIECE *p = ew->work;
    s16 i;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft16_num[ew->arg];
    switch (ew->arg) {
    case 2:
    case 5:
    case 6:
        eft16_se_req(ew, ew->pos);
        break;
    case 3:
    case 4:
        return;
    }
    for (i = 0; i < n; p++, i++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            switch (ew->arg) {
            case 0:
                p->no = i;
                if (!(ew->mode2 & 0x20)) {
                    p->lag = -10 - ((u16)ran_suu(1) & 7);
                } else {
                    p->lag = 0;
                }
                p->size = ew->scale;
                p->grav = -0.8f;
                break;
            case 2:
                p->lag = -i;
                p->no = 0;
                p->alpha = 0xFF;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                break;
            case 5:
                if (i < 3) {
                    p->no = 0;
                    p->lag = -i;
                    p->size = ew->scale;
                    p->rot = ew->u0A.joint + (i * 0x2AAA + (u16)ran_suu(1) / 3);
                } else {
                    p->no = 1;
                    p->lag = 0;
                    p->size = ew->scale;
                    p->rot = ran_suu(1);
                }
                p->alpha = 0xFF;
                break;
            case 6:
                p->lag = 0;
                p->no = i;
                if (i == 0) {
                    p->alpha = 0xFF;
                }
                p->size = ew->scale;
                break;
            case 8:
                p->no = i;
                p->lag = -4 * i - 10;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                p->uv = 0;
                break;
            case 9:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -12 * i - 10;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 12:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -10 * i;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 13:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -ew->stg * i;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 14:
                p->no = i;
                p->uv = (u16)ran_suu(1) & 3;
                p->rot = ran_suu(1);
                p->lag = -ew->stg;
                p->size = ew->scale;
                p->alpha = 0xFF;
                break;
            }
            p->pos[0] = ew->pos[0];
            p->pos[1] = ew->pos[1];
            p->pos[2] = ew->pos[2];
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft16_t;
        } else {
            p->prim = 0;
        }
    }
}

static void eft16_m(EFTW *ew) {
    int y15;
    int x5;
    FLMAT m;
    f32 v[3];
    f32 o[3];
    f32 a;
    f32 f;
    s32 n;
    s32 k;
    s16 num;
    s16 all;
    s16 idx;
    s16 time;
    s16 step;
    s16 i;
    s16 j;
    s16 hit;
    s16 tm;
    s16 found;
    s16 cnt;
    s16 lim;
    u16 rx;
    u16 ry;
    s16 *tbl;
    s32 u;
    f32 *dir;
    EFT16_NECK *nk;
    void *d;
    EFT16_PIECE *p = ew->work;

    num = eft16_num[ew->arg];
    all = eft16_all_time[ew->arg];
    idx = eft16_index[ew->arg];
    time = eft16_time[ew->arg];
    step = eft16_param[ew->arg];
    switch (ew->arg) {
    case 13:
        all = ew->u0A.joint;
        break;
    case 14:
        all = ew->u0A.joint;
        if (all < 0) {
            all = 1000;
            ew->timer = 0;
            time = all;
        } else {
            time = all;
        }
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    if (ew->x07 != 0xFF && (ew->owner->x04 == 3 || ew->owner->be_flag == 0)) {
        switch (ew->arg) {
        case 0:
        case 8:
        case 9:
            for (i = 0; i < num; i++) {
                p[i].no = 0xFF;
            }
            break;
        case 3:
        case 4:
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
    }
    if (ew->arg == 3 || ew->arg == 4) {
        nk = eft16_neck_tbl[ew->owner->kind];
        if (nk == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        dir = nk->dir;
        switch (ew->arg) {
        case 3:
            cnt = 3;
            lim = 0x55;
            break;
        case 4:
            cnt = 3;
            lim = 0x32;
            break;
        }
        tm = ew->timer;
        if (tm % (cnt + 1) == 1) {
            if (ew->x07 != 0xFF) {
                flmatCopy(&m, get_joint_wmat(ew->owner, ew->x07));
                flvecApplyMat33(v, ew->pos, &m);
                flmatGetTrans(o, &m);
                o[0] += v[0];
                o[1] += v[1];
                o[2] += v[2];
                if (ew->arg == 3) {
                    v[0] = *dir++;
                    v[1] = *dir++;
                    v[2] = *dir++;
                } else if (ew->arg == 4) {
                    dir += 3;
                    v[0] = *dir++;
                    v[1] = *dir++;
                    v[2] = *dir++;
                }
                flvecApplyMat33_2(v, &m);
                rx = -(u16)(s32)(0.5f + 65536.0f * flArcTan2(v[1], flSqrt(v[0] * v[0] + v[2] * v[2])) / 6.2831855f);
                ry = (s32)(0.5f + 65536.0f * flArcTan2(v[0], v[2]) / 6.2831855f);
            } else {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            if (ew->arg == 4) {
                f = 0.4f;
            } else if (tm <= lim) {
                f = 1.0f;
            } else {
                f = 1.0f - 0.06666667f * (f32)(tm - lim);
            }
            Eft02_set4(rx, ry, 3, o, ew->scale * f);
        }
        return;
    }
    if (ew->arg == 0 && p->no != 0xFF) {
        if ((ew->mode2 & 0x20) == 0) {
            hit = ew->timer - 10;
        } else {
            hit = ew->timer;
        }
        switch (ew->mode2 & 0x30) {
        default:
            tbl = eft16_time_tbl1[0];
            break;
        case 0x10:
            tbl = eft16_time_tbl2[0];
            break;
        case 0x20:
            tbl = eft16_time_tbl3[0];
            break;
        }
        j = 0;
        found = 0;
        while (tbl[j * 2] != -1) {
            if (hit == tbl[j * 2]) {
                found = 1;
                break;
            }
            j++;
        }
        if (found == 1) {
            if (ew->x07 != 0xFF) {
                flmatCopy(&m, get_joint_wmat(ew->owner, ew->x07));
                flvecApplyMat33(v, ew->pos, &m);
                flmatGetTrans(o, &m);
                o[0] += v[0];
                o[1] += v[1];
                o[2] += v[2];
                if (j == 0 && ew->owner->x10 == 1 && (ew->mode2 & 0x30) != 0x20) {
                    eft16_se_req(ew, o);
                }
                f = 0.1f * (f32)tbl[j * 2 + 1];
                f *= ew->scale * eft16_scale_tbl[ew->mode2 & 0xF];
                switch (ew->mode2 & 0x30) {
                case 0:
                    Eft02_set3(ew->owner, (u16)ew->u0A.joint, 3, 2, o, f);
                    break;
                case 0x20:
                    Eft02_set3(ew->owner, (u16)ew->u0A.joint, 3, 0, o, f);
                    break;
                case 0x10:
                    Eft02_set3(ew->owner, (u16)ew->u0A.joint, 3, 3, o, f);
                    break;
                }
            } else {
                flvecCopy(o, ew->pos);
            }
        }
    }
    n = num;
    i = 0;
    if (i < n) {
    y15 = 15;
    x5 = 0;
    do {
        switch (ew->arg) {
        case 0:
        case 8:
        case 9:
        case 12:
        case 13:
            idx = eft16_index[ew->arg];
            break;
        case 2:
            idx = eft16_index[ew->arg];
            if (i == 2) {
                time = 4;
                idx += step;
            }
            break;
        case 5:
            if (p->no == 0) {
                time = 5;
                idx = eft16_index[ew->arg];
            } else {
                time = 5;
                idx = step + eft16_index[ew->arg];
            }
            break;
        case 6:
            if (i != 0) {
                step = 2;
                time = 5;
            }
            break;
        case 14:
            if (ew->u0A.joint < 0) {
                p->lag = 0;
            }
            break;
        }
        if (++p->lag <= 0) {
            idx += step;
            p++;
            continue;
        }
        if (p->lag == 1 && ew->arg != 2 && ew->arg != 5 && ew->arg != 6 && ew->arg != 14) {
            if (ew->x07 != 0xFF) {
                if (p->no == 0xFF) {
                    p++;
                    continue;
                }
                flmatCopy(&m, get_joint_wmat(ew->owner, ew->x07));
                flvecApplyMat33(v, ew->pos, &m);
                flmatGetTrans(o, &m);
                if (ew->arg == 0) {
                    if (p->prim != 0) {
                        p->prim->pos[0] = o[0] + v[0];
                        p->prim->pos[1] = o[1] + v[1];
                        p->prim->pos[2] = o[2] + v[2];
                    }
                    v[0] = 0.02f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                    v[1] = 0.02f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                    v[2] = 30.0f + 0.005f * (f32)((u16)ran_suu(1) & 0x3FF);
                    p->uv = (u16)ran_suu(1) & 3;
                    p->rot = ran_suu(1);
                    flvecCopy(p->pos, v);
                    flvecRotY(p->pos, DEG2RAD(ANG2DEG(ew->u0A.joint)));
                } else if (ew->arg == 8 || ew->arg == 9) {
                    p->pos[0] = o[0] + v[0];
                    p->pos[1] = o[1] + v[1];
                    p->pos[2] = o[2] + v[2];
                    if (ew->arg == 8) {
                        v[0] = 0.04f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                        v[1] = 0.02f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                        v[2] = 3.0f;
                        p->pos[0] += v[0];
                        p->pos[1] += v[1];
                        p->pos[2] += v[2];
                    }
                } else {
                    p->pos[0] = o[0] + v[0] + 0.004f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                    p->pos[1] = 10.0f + (o[1] + v[1] + 0.004f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200));
                    p->pos[2] = o[2] + v[2] + 0.004f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                }
            }
        } else if (p->lag > time) {
            if (ew->arg == 0xD) {
                p->lag = -ew->stg * 9 + 0x3C;
                flvecCopy(p->pos, ew->pos);
            }
            idx += step;
            p++;
            continue;
        }
        switch (ew->arg) {
        case 0:
        case 9:
        case 12:
        case 13:
            d = eft16_data[k = idx];
            eft_vec_linear(p->lag, d, p->scale);
            idx += 2;
            eft_alpha_linear(p->lag, eft16_data[(s16)(k + 1)], &a);
            p->alpha = 255.0f * a;
            break;
        case 2:
            d = eft16_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (i == 2) {
                p->rot = ran_suu(1);
                p->no = (u16)ran_suu(1) % 3;
            }
            break;
        case 5:
            d = eft16_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (i >= 3) {
                p->no = (u16)ran_suu(1) % 3 + 1;
                p->rot = ran_suu(1);
            }
            break;
        case 6:
            d = eft16_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (i == 0) {
                p->rot = ran_suu(1);
                p->no = (u16)ran_suu(1) % 3;
            } else {
                d = eft16_data[idx++];
                eft_alpha_linear(p->lag, d, &a);
                p->alpha = 255.0f * a;
            }
            break;
        default:
            d = eft16_data[k = idx];
            eft_vec_linear(p->lag, d, p->scale);
            idx += 2;
            eft_alpha_linear(p->lag, eft16_data[(s16)(k + 1)], &a);
            p->alpha = 255.0f * a;
            u = 0;
            while (uv78_00647180[u] != -1) {
                if (p->lag == uv78_00647180[u]) {
                    p->uv++;
                    break;
                }
                u++;
            }
            break;
        case 14:
            break;
        }
        if (p->prim != 0) {
            switch (ew->arg) {
            case 0:
                p->pos[1] += p->grav;
                p->prim->pos[0] += p->pos[0];
                p->prim->pos[1] += p->pos[1];
                p->prim->pos[2] += p->pos[2];
                break;
            case 2:
            case 5:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = y15;
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] = p->pos[0] + v[0];
                p->prim->pos[1] = p->pos[1] + v[1];
                p->prim->pos[2] = p->pos[2] + v[2];
                break;
            case 6:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = y15;
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] = p->pos[0] + v[0];
                p->prim->pos[1] = p->pos[1] + v[1];
                p->prim->pos[2] = p->pos[2] + v[2];
                break;
            case 8:
                switch (i) {
                case 6:
                    p->scale[0] *= 0.8f;
                    p->scale[1] *= 0.8f;
                    p->scale[2] *= 0.8f;
                    p->alpha = 0.8f * p->alpha;
                    break;
                case 7:
                    p->scale[0] *= 0.6f;
                    p->scale[1] *= 0.6f;
                    p->scale[2] *= 0.6f;
                    p->alpha = 0.6f * p->alpha;
                    break;
                case 8:
                    p->scale[0] *= 0.4f;
                    p->scale[1] *= 0.4f;
                    p->scale[2] *= 0.4f;
                    p->alpha = 0.4f * p->alpha;
                    break;
                }
                p->pos[1] += 3.5f + 0.003f * (f32)((u16)ran_suu(1) & 0x3FF);
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
                break;
            case 9:
                switch (i) {
                case 4:
                    p->alpha = 0.8f * p->alpha;
                    break;
                case 5:
                    p->alpha = 0.6f * p->alpha;
                    break;
                case 6:
                    p->alpha = 0.4f * p->alpha;
                    break;
                }
                p->rot += eft16_rot(p->no);
                p->pos[1] += 2.0f + 0.001f * (f32)((u16)ran_suu(1) & 0x3FF);
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
                break;
            case 12:
            case 13:
                p->rot += eft16_rot(p->no);
                if (ew->arg == 0xC) {
                    p->alpha = 0.1f * p->alpha;
                }
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = -x5;
                flvecApplyMat33_2(v, &rview_mat);
                p->pos[1] += 3.0f + 0.001f * (f32)((u16)ran_suu(1) & 0x3FF);
                p->prim->pos[0] = p->pos[0] + v[0];
                p->prim->pos[1] = p->pos[1] + v[1];
                p->prim->pos[2] = p->pos[2] + v[2];
                break;
            case 14:
                flvecCopy(p->prim->pos, ew->pos);
                break;
            }
            add_prim(ot0, p->prim, 0x40, 0);
        }
        p++;
    } while (x5 += 5, y15 += 5, i++, i < n);
    }
}

static void eft16_d(EFTW *ew) {
    s16 n;
    s16 i;
    EFT16_PIECE *p = ew->work;

    ew->mode++;
    n = eft16_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft16_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft16_t(PRIM *pr) {
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT16_PIECE *p = &((EFT16_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;
    u16 flag = 0;
    s16 c;
    f32 t;
    u8 r;
    u8 g;
    u8 b;

    if (mw != 0 && mw->flag != 0) {
        r = 0xFF;
        g = 0xFF;
        mats = mw->mat;
        b = 0xFF;
        switch (ew->arg) {
        case 0:
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            sc[0] = p->scale[0] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            sc[1] = p->scale[1] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            sc[2] = p->scale[2] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            cl = &mw->clay[97];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMakeTrans(&uv, 0.125f * (f32)(p->uv & 1), 0.125f * (f32)(p->uv >> 1), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            c = eft16_col_type_sel(ew->mode2);
            r = Eft_blood_rgb[c][0];
            g = Eft_blood_rgb[c][1];
            b = Eft_blood_rgb[c][2];
            if (p->lag >= 8) {
                t = (f32)(p->lag - 8) / 8.0f;
                r = (u8)(r - (u8)((s32)((u32)r >> 1) * t));
                g = (u8)(g - (u8)((s32)((u32)g >> 1) * t));
                b = (u8)(b - (u8)((s32)((u32)b >> 1) * t));
            }
            break;
        case 2:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            if (pr->no == 0) {
                if ((ew->mode2 & 0x30) == 0x20) {
                    return;
                }
                rot[2] = DEG2RAD(ANG2DEG(ew->u0A.joint));
                cl = mw->clay;
            } else {
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                cl = &mw->clay[p->no] + 32;
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            if (ew->stg == 1) {
                r = 0xFF;
                g = 0xFF;
                flag |= 2;
                b = 0xFF;
                if (pr->no == 0) {
                    flmatMakeTrans(&uv, 0.125f, 0.0f, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                }
            } else {
                c = eft16_col_type_sel(ew->mode2);
                r = Eft_blood_rgb[c][0];
                g = Eft_blood_rgb[c][1];
                b = Eft_blood_rgb[c][2];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 5:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            if (p->no == 0) {
                cl = mw->clay;
            } else {
                cl = &mw->clay[p->no] + 31;
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            if (ew->stg == 1) {
                r = 0xFF;
                flag |= 2;
                g = 0xFF;
                b = 0xFF;
                if (p->no == 0) {
                    flmatMakeTrans(&uv, 0.125f, 0.0f, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                }
            } else {
                c = eft16_col_type_sel(ew->mode2);
                r = Eft_blood_rgb[c][0];
                g = Eft_blood_rgb[c][1];
                b = Eft_blood_rgb[c][2];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 6:
            flag |= 2;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            if (pr->no == 0) {
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                cl = &mw->clay[p->no] + 32;
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
            } else {
                cl = &mw->clay[21];
                make_mat_srt(sc, rot, pr->pos, 0, &m);
            }
            break;
        case 8:
            flag |= 2;
            sc[0] = 0.5f * p->size * p->scale[0];
            sc[1] = 0.5f * p->size * p->scale[1];
            sc[2] = 0.5f * p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            cl = &mw->clay[78];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv >> 2), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 9:
        case 12:
        case 13:
            flSetRenderState(0x6C, 0);
            if (ew->arg == 9) {
                sc[0] = 0.5f * p->size * p->scale[0];
                sc[1] = 0.5f * p->size * p->scale[1];
                sc[2] = 0.5f * p->size * p->scale[2];
            } else if (ew->arg == 10 || ew->arg == 13) {
                sc[0] = 0.2f * p->size * p->scale[0];
                sc[1] = 0.2f * p->size * p->scale[1];
                sc[2] = 0.2f * p->size * p->scale[2];
            } else if (ew->arg == 12) {
                sc[0] = 2.0f * p->size * p->scale[0];
                sc[1] = 2.0f * p->size * p->scale[1];
                sc[2] = 2.0f * p->size * p->scale[2];
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            if (ew->arg == 0xC) {
                cl = &mw->clay[74];
            } else {
                if (!(p->no & 1)) {
                    cl = &mw->clay[77];
                    flmatMakeTrans(&uv, 0.0f, 0.25f, 0.0f);
                } else {
                    cl = &mw->clay[79];
                    flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                }
                flSetRenderState(0x19, (u32)&uv);
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            break;
        case 14:
            sc[0] = p->size;
            sc[1] = p->size;
            sc[2] = p->size;
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            cl = &mw->clay[97];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatRotX33(&m, 4.712389f);
            flmatMakeTrans(&uv, 0.125f * (f32)(p->uv & 1), 0.125f * (f32)(p->uv >> 1), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            r = Eft_blood_rgb[0][0];
            g = Eft_blood_rgb[0][1];
            b = Eft_blood_rgb[0][2];
            break;
        }
        if (ew->arg != 0xE) {
            flmatMul33_2(&m, &rview_mat);
        }
        eft_trans_sub_col(cl, &m, (p->alpha << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF), flag, mats);
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}

static s16 eft16_rot(s16 no) {
    switch (no & 6) {
    case 0:
        return 0x111;
    case 2:
        return -0x110;
    case 4:
        return 0x89;
    case 6:
        return -0x88;
    }
    return 0;
}

static s16 eft16_col_type_sel(u8 flag) {
    s16 c;

    switch (flag & 0xC0) {
    case 0x80:
        c = 2;
        break;
    default:
        c = 0;
        break;
    }
    return c;
}

static void eft16_se_req(EFTW *ew, f32 *pos) {
    switch (ew->arg) {
    case 0:
        if (eft16_scale_tbl[ew->mode2 & 0xF] <= 0.5f) {
            se_req2(1, 0x6E, 0, pos, 1, 0);
            break;
        }
        switch (ew->mode2 & 0x30) {
        case 0:
            se_req2(1, 0x6E, 0, pos, 1, 0);
            break;
        case 0x10:
            se_req2(1, 0x6F, 0, pos, 1, 0);
            break;
        }
        break;
    case 2:
    case 5:
        switch (ew->stg) {
        case 1:
            switch (ew->mode2 & 0x30) {
            case 0x10:
                se_req2(1, 0x65, 0, pos, 1, 0);
                break;
            case 0:
                se_req2(1, 0x64, 0, pos, 1, 0);
                break;
            }
            break;
        }
        break;
    case 6:
        se_req2(1, 0x63, 0, pos, 1, 0);
        break;
    }
}

void Eft16_set(PLW *pl, int arg, s16 hit, f32 *pos, f32 scale) {
    FLMAT m;
    FLMAT inv;
    f32 t[3];
    EFTW *ew;
    s16 *jl;

    if (pl != 0 && Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0x10;
            ew->move = eft16_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            switch (hit) {
            default:
            case 2:
                ew->mode2 = 0;
                break;
            case 3:
                ew->mode2 = 0x10;
                break;
            case 0:
                ew->mode2 = 0x20;
                break;
            }
            ew->scale = scale;
            if (ew->arg == 6) {
                ew->pos[0] = pos[0];
                ew->pos[1] = pos[1];
                ew->pos[2] = pos[2];
                ew->u0A.ang = ran_suu(1);
                ew->x07 = 0xFF;
                return;
            }
            ew->u0A.ang = CHR_ANG3EC(pl) + 0x8000;
            ew->mode2 &= 0x30;
            if (pl->x10 != 0) {
                switch (pl->kind) {
                case 9:
                case 0x17:
                    ew->mode2 |= 2;
                    break;
                case 0x13:
                case 0x18:
                    ew->mode2 |= 0x82;
                    break;
                }
                if (CHR_X4D8(pl) != 0 && (jl = *(s16 **)(CHR_X4D8(pl) + 0xA0)) != 0) {
                    ew->x07 = *jl;
                    flmatCopy(&m, get_joint_wmat(pl, ew->x07));
                    flmatGetTrans(t, &m);
                    ew->pos[0] = pos[0] - t[0];
                    ew->pos[1] = pos[1] - t[1];
                    ew->pos[2] = pos[2] - t[2];
                    flmatInvert(&inv, &m);
                    flvecApplyMat33_2(ew->pos, &inv);
                    return;
                }
            } else {
                ew->mode2 |= 1;
                ew->x07 = 0xA;
                if (ew->arg == 8 || ew->arg == 9) {
                    ew->pos[0] = 0.0f;
                    ew->pos[1] = 0.0f;
                    ew->pos[2] = 0.0f;
                } else {
                    flmatCopy(&m, get_joint_wmat(pl, ew->x07));
                    flmatGetTrans(t, &m);
                    ew->pos[0] = pos[0] - t[0];
                    ew->pos[1] = pos[1] - t[1];
                    ew->pos[2] = pos[2] - t[2];
                    flmatInvert(&inv, &m);
                    flvecApplyMat33_2(ew->pos, &inv);
                }
                return;
            }
            ew->x07 = 0xFF;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
        }
    }
}

void Eft16_set_ex(EMW *em, int arg, int joint) {
    EFTW *ew;
    EFT16_NECK *nk;
    f32 *ofs;

    if (Em_stg_ck(em) != 0 && (nk = eft16_neck_tbl[em->kind]) != 0 && (ew = pull_eft_work(0)) != 0) {
        ew->type = 0x10;
        ew->move = eft16_move;
        ew->arg = arg;
        ew->x07 = joint;
        ew->owner = em;
        ofs = nk->ofs;
        if (ew->arg == 3) {
            ew->pos[0] = *ofs++ * em->scale[0];
            ew->pos[1] = *ofs++ * em->scale[1];
            ew->pos[2] = *ofs++ * em->scale[2];
        } else {
            ofs += 3;
            ew->pos[0] = *ofs++ * em->scale[0];
            ew->pos[1] = *ofs++ * em->scale[1];
            ew->pos[2] = *ofs++ * em->scale[2];
        }
    }
}

void Eft16_set_ex3(f32 *pos, int arg, s16 ang, int cnt, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0x10;
        ew->move = eft16_move;
        ew->arg = arg;
        ew->u0A.joint = ang;
        ew->scale = scale;
        ew->stg = cnt;
        ew->x07 = 0xFF;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
    }
}

void Eft16_set_impact(PLW *pl, f32 *pos, int arg, s16 hit, s16 wpn, f32 scale) {
    EFTW *ew;

    if (pl != 0 && Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0x10;
            ew->move = eft16_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            switch (hit) {
            default:
            case 2:
                ew->mode2 = 0;
                break;
            case 3:
                ew->mode2 = 0x10;
                break;
            case 0:
                ew->mode2 = 0x20;
                break;
            }
            ew->mode2 &= 0x3F;
            switch (wpn) {
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                ew->stg = 1;
                break;
            default:
                if (pl->x10 != 0 && (pl->kind == 0x13 || pl->kind == 0x18)) {
                    ew->mode2 |= 0x80;
                }
                break;
            }
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->u0A.ang = ran_suu(1);
            ew->x07 = 0xFF;
        }
    }
}
