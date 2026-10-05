/* eft18 - game.bin 0x00551830-0x00552CF8: eft18_move to eft18_m00.
 * eft18_set_com is still assembly (near-match in
 * eft18_nm.c, which also says what the effect does); the rest is in
 * eft18b.c and eft18c.c. */
#include "eft.h"
#include "pl.h"
#include "shell.h"
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

/* One sprite (0x2C bytes), after the effect's matrix in the work area. */
typedef struct EFT18_PIECE {
    f32 scale[3];       /* 0x00 from the keyframes */
    f32 alpha;          /* 0x0C */
    f32 pos[3];         /* 0x10 */
    PRIM *prim;         /* 0x1C */
    s16 prim_no;        /* 0x20 */
    s16 no;             /* 0x22 */
    s16 lag;            /* 0x24 */
    s16 uv;             /* 0x26 */
    u16 rot;            /* 0x28 */
    s16 drot;           /* 0x2A */
} EFT18_PIECE;

typedef struct EFT18_WORK {
    FLMAT mat;              /* 0x00 */
    EFT18_PIECE piece[1];   /* 0x40 */
} EFT18_WORK;

/* One rock (0x28 bytes) of type 4. */
typedef struct EFT18_ROCK {
    u8 alive;           /* 0x00 */
    u8 x01;             /* 0x01 */
    s16 prim_no;        /* 0x02 */
    PRIM *prim;         /* 0x04 */
    f32 vel[3];         /* 0x08 */
    f32 pos[3];         /* 0x14 */
    u16 rx;             /* 0x20 */
    u16 ry;             /* 0x22 */
    u16 rz;             /* 0x24 */
    u8 _pad26[2];
} EFT18_ROCK;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern f32 D_3F2090[3];
extern void *eft18_data[];
extern s16 eft18_num[12];
extern s16 eft18_all_time[12];
extern s16 eft18_index[12];
extern f32 eft18_type1_scale[];
extern f32 eft18_type1_rot[];
extern s16 eft18_type1_time_tbl[];
extern s16 eft18_type5_time_tbl[];
extern s16 eft18_type8_time[];
extern s16 eft18_type8_lag[];
extern void **eft18_type5_fade_data[];
extern void *fade74_data[];
extern void *fade_type7_data[];
extern void *fade_type8_data[];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
FLMAT *get_joint_wmat(PLW *, s16);
void flvecCopy(f32 *, f32 *);
void flvecRotX(f32 *, f32);
void flvecRotY(f32 *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flvecNormalize(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
void RotateY(FLMAT *, f32);
void RotateZ(FLMAT *, f32);
void PointToPoint(f32 *, f32 *, f32 *);
f32 GetGroundHit(f32 *);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
void SetTrnslMode(int, int);
void se_req2(int, int, int, f32 *, int, int);
void Eft13_set_pos(f32, f32 *, int);

void eft18_move(EFTW *ew);
void eft18_i(EFTW *ew);
void eft18_m(EFTW *ew);
void eft18_t(PRIM *pr);
void eft18_i00(EFTW *ew);
void eft18_m00(EFTW *ew);
void eft18_t00(PRIM *pr);
void eft18_i01(EFTW *ew);
void eft18_m01(EFTW *ew);
void eft18_t01(PRIM *pr);
void eft18_i02(EFTW *ew);
void eft18_m02(EFTW *ew);
void eft18_t02(PRIM *pr);
void eft18_d(EFTW *ew);
void eft18_e(EFTW *ew);
void eft18_z_adj(FLMAT *m, f32 *pos);
void eft18_se_req(EFTW *ew, f32 *pos);
EFTW *eft18_set_com(s16 arg);

void eft18_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft18_i(ew);
        break;
    case 1:
        eft18_m(ew);
        break;
    case 2:
        eft18_d(ew);
        break;
    case 3:
        eft18_e(ew);
        break;
    }
}

void eft18_i(EFTW *ew) {
    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    switch (ew->arg) {
    default:
        eft18_i00(ew);
        break;
    case 3:
        eft18_i01(ew);
        break;
    case 4:
        eft18_i02(ew);
        break;
    }
}

void eft18_m(EFTW *ew) {
    switch (ew->arg) {
    default:
        eft18_m00(ew);
        break;
    case 3:
        eft18_m01(ew);
        break;
    case 4:
        eft18_m02(ew);
        break;
    }
}

void eft18_t(PRIM *pr) {
    switch (((EFTW *)pr->owner)->arg) {
    default:
        eft18_t00(pr);
        break;
    case 3:
        eft18_t01(pr);
        break;
    case 4:
        eft18_t02(pr);
        break;
    }
}

void eft18_i00(EFTW *ew) {
    f32 v[3];
    FLMAT rm;
    FLMAT jm;
    s32 n;
    EFT18_WORK *w = ew->work;
    PLW *pl = (PLW *)ew->owner;
    EFT18_PIECE *p = w->piece;
    s16 num = eft18_num[ew->arg];
    s16 i;
    s16 k;
    u16 rot;
    f32 *sp;

    switch (ew->arg) {
    case 0:
    case 1:
    case 5:
    case 6:
        v[0] = -10.0f;
        v[1] = -15.0f;
        v[2] = 87.0f;
        flmatCopy(&jm, get_joint_wmat(pl, ew->u0A.joint));
        flmatGetTrans(ew->pos, &jm);
        flmatCopy(&rm, &jm);
        RotateY(&rm, 3.1415927f);
        flvecApplyMat33_2(v, &rm);
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        flmatCopy(&w->mat, &rm);
        break;
    case 0xA:
    case 0xB:
        flmatInit(&w->mat);
        flmatRotX33(&w->mat, DEG2RAD(ANG2DEG(ew->timer)));
        flmatRotY33(&w->mat, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        break;
    }
    n = num;
    for (i = 0; i < n; i++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft18_t;
            switch (ew->arg) {
            case 0:
            case 10:
                p->no = i;
                p->lag = 0;
                break;
            case 1:
            case 11:
                k = i / 2;
                p->no = i & 1;
                if (p->no == 0) {
                    p->rot = ran_suu(1);
                    rot = p->rot;
                    sp = &eft18_type1_scale[k];
                    p->scale[0] = *sp;
                    p->scale[1] = *sp;
                    p->scale[2] = *sp;
                } else {
                    p->rot = rot;
                }
                p->lag = -4;
                p->alpha = 1.0f;
                if (i == 2) {
                    p->uv = 1;
                } else if (i == 4) {
                    p->uv = 3;
                } else {
                    p->uv = -1;
                }
                break;
            case 2:
                p->no = (30.0f * ran_suu(1) / 65535.0f - 15.0f) / 20.0f;
                p->rot = ran_suu(1);
                p->lag = -4 - i * 3;
                break;
            case 5:
                p->no = i;
                switch (i) {
                case 2:
                    p->lag = 0;
                    p->rot = ran_suu(1);
                    break;
                case 4:
                    p->lag = -1;
                    p->rot = ran_suu(1);
                    break;
                default:
                    p->lag = 0;
                    p->rot = 0;
                    break;
                }
                break;
            case 6:
                p->no = i;
                p->rot = ran_suu(1);
                p->lag = -5 - i * 2;
                break;
            case 7:
                if (i == 0) {
                    p->no = 0;
                    p->rot = 0;
                    p->lag = 0;
                    flvecCopy(p->pos, ew->pos);
                } else {
                    p->no = i;
                    p->rot = ran_suu(1);
                    p->drot = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                    p->lag = -6 - (i - 1) * 5;
                    p->pos[0] = ew->pos[0] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                    p->pos[1] = ew->pos[1] + 0.0050000004f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                    p->pos[2] = ew->pos[2] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                }
                break;
            case 8:
                p->lag = eft18_type8_lag[i];
                flvecCopy(p->pos, ew->pos);
                switch (i) {
                case 0:
                case 1:
                    p->no = i;
                    p->rot = 0;
                    break;
                case 2:
                case 3:
                case 4:
                default:
                    p->no = 2;
                    p->rot = ran_suu(1);
                    break;
                }
                break;
            case 9:
                p->no = i + 1;
                p->rot = ran_suu(1);
                p->drot = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                p->lag = -6 - (i - 1) * 5;
                p->pos[0] = ew->pos[0] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                p->pos[1] = ew->pos[1] + 0.0050000004f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                p->pos[2] = ew->pos[2] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                break;
            }
        } else {
            p->prim = 0;
        }
        p++;
    }
    eft18_m(ew);
}

void eft18_m00(EFTW *ew) {
    f32 v[3];
    f32 w[3];
    FLMAT jm;
    EFT18_WORK *wk;
    EFT18_PIECE *p;
    s16 i;
    s16 num;
    s16 all;
    s16 idx;
    s16 time;
    s16 step;
    void *d;

    wk = ew->work;
    p = wk->piece;
    num = eft18_num[ew->arg];
    all = eft18_all_time[ew->arg];
    idx = eft18_index[ew->arg];
    switch (ew->arg) {
    case 0:
    case 10:
        time = 7;
        step = 1;
        break;
    case 2:
        time = 20;
        step = 3;
        break;
    case 5:
        step = 1;
        break;
    case 6:
        time = 17;
        step = 1;
        break;
    case 7:
        if (ew->timer == 6) {
            eft18_se_req(ew, ew->pos);
        }
        step = 1;
        break;
    case 8:
        step = 1;
        if (ew->timer == all - 1) {
            Eft13_set_pos(1.0f, ew->pos, 0x1D);
        }
        break;
    case 9:
        time = 30;
        step = 1;
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < num; i++) {
        switch (ew->arg) {
        case 1:
        case 0xB:
            if (p->no == 0) {
                step = 1;
            } else {
                step = 3;
            }
            time = eft18_type1_time_tbl[i];
            break;
        case 2:
            idx = 14;
            break;
        case 5:
            time = eft18_type5_time_tbl[i];
            break;
        case 6:
            idx = 24;
            break;
        case 7:
            if (p->no == 0) {
                time = 4;
            } else {
                time = 30;
            }
            break;
        case 8:
            time = eft18_type8_time[i];
            break;
        }
        if (++p->lag <= 0) {
            p++;
            idx += step;
            continue;
        }
        if (p->lag == 1) {
            if ((ew->arg == 2 || ew->arg == 6) && p->prim != 0) {
                v[0] = -10.0f;
                v[1] = -15.0f;
                v[2] = 87.0f;
                flmatCopy(&jm, get_joint_wmat((PLW *)ew->owner, ew->u0A.joint));
                flmatGetTrans(p->prim->pos, &jm);
                RotateY(&jm, 3.1415927f);
                flvecApplyMat33_2(v, &jm);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
            }
        } else if (p->lag > time) {
            p++;
            idx += step;
            continue;
        }
        switch (ew->arg) {
        case 0:
        case 5:
        case 10:
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (ew->arg == 5 && p->no == 1) {
                p->scale[0] *= 0.75f;
                p->scale[1] *= 0.75f;
                p->scale[2] *= 0.75f;
            }
            if (p->prim != 0) {
                p->prim->pos[0] = ew->pos[0];
                p->prim->pos[1] = ew->pos[1];
                p->prim->pos[2] = ew->pos[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        case 1:
        case 11:
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, v);
            flvecApplyMat33_2(v, &wk->mat);
            p->rot += (u16)(s32)(0.5f + 65536.0f * eft18_type1_rot[i / 2] / 360.0f);
            if (p->no == 0) {
                p->uv++;
            } else {
                w[0] = 0.0f;
                w[1] = 0.0f;
                w[2] = (f32)(-5 * i);
                flvecApplyMat33_2(w, &rview_mat);
                v[0] += w[0];
                v[1] += w[1];
                v[2] += w[2];
                d = eft18_data[idx++];
                eft_vec_linear(p->lag, d, p->scale);
                d = eft18_data[idx++];
                eft_alpha_linear(p->lag, d, &p->alpha);
            }
            if (p->prim != 0) {
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        case 2:
            p->rot += p->no;
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, v);
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            d = eft18_data[idx++];
            eft_alpha_linear(p->lag, d, &p->alpha);
            if (p->prim != 0) {
                p->prim->pos[1] += 0.5f;
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        case 6:
            p->rot += 0xA1;
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            p->scale[0] *= 1.4f - 0.2f * (f32)i;
            p->scale[1] *= 1.4f - 0.2f * (f32)i;
            p->scale[2] *= 1.4f - 0.2f * (f32)i;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = (100.0f - 20.0f * (f32)i) / (f32)time;
            if (p->prim != 0) {
                flvecApplyMat33_2(v, &wk->mat);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        case 7:
        case 9:
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (p->prim != 0) {
                if (p->no == 0) {
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 30.0f;
                    flvecApplyMat33_2(v, &rview_mat);
                    p->prim->pos[0] = p->pos[0] + v[0];
                    p->prim->pos[1] = p->pos[1] + v[1];
                    p->prim->pos[2] = p->pos[2] + v[2];
                } else {
                    p->rot += p->drot;
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 5.0f * (f32)i;
                    p->pos[1] += 2.0f;
                    p->prim->pos[0] = p->pos[0] + v[0];
                    p->prim->pos[1] = p->pos[1] + v[1];
                    p->prim->pos[2] = p->pos[2] + v[2];
                }
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        case 8:
            d = eft18_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            if (p->prim != 0) {
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
            break;
        }
        p++;
    }
}
