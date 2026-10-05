/* eft17 - game.bin 0x0053E??? (see config/c_files.txt). Monster breath and
 * dust effects with 21 types (arg). Type 8 throws ten tumbling rocks that
 * bounce on the ground (eft17_i08/m08/t08, one model); every other type
 * spawns up to eft17_num sprites that grow and fade along keyframe tables
 * (eft17_data through eft17_index/param, lengths from eft17_time) and are
 * drawn with the monster area's model set (game_w.area_mdlw) or the common
 * effect models (eft_mdlw[0]). Started by Eft17_set / Eft17_set_ang (at a
 * monster joint), Eft17_type3_set, Eft17_set_ex, Eft17_set_pos and
 * Eft17_set_pos_ang (at a point). Work field names are guesses from use. */
#include "eft.h"
#include "em.h"
#include "game.h"
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

/* One sprite or rock (0x30 bytes) of the work area. */
typedef struct EFT17_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 sub-kind (type 8: bounces left) */
    f32 scale[3];       /* 0x04 from the keyframes (type 8: velocity) */
    f32 pos[3];         /* 0x10 */
    f32 alpha;          /* 0x1C (type 8: size) */
    s16 lag;            /* 0x20 frame counter, starts at 0 or below */
    u16 rot[3];         /* 0x22 rot[0] is the spin step for most types */
    union {
        f32 size;       /* 0x28 */
        s16 spin;       /* 0x28 type 8: z spin step */
    } u28;
    PRIM *prim;         /* 0x2C */
} EFT17_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))
#define ANG2RAD(a) DEG2RAD(ANG2DEG(a))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft17_data[];
extern s16 eft17_num[];
extern u16 eft17_all_time[];
extern s16 eft17_param[];
extern s16 eft17_index[];
extern u16 *eft17_time[];
extern s16 eft17_type11_lag_tbl[4];
extern s16 eft17_type15_lag_tbl[4];
extern u8 fade_type15_em03[];
extern u8 fade_type15_em04_em06[];
extern u8 fade_type15_em04_em06_2[];
extern u8 fade_type16_74[];
extern u8 fade_type18_em00[];
extern u8 fade_type18_em00_2[];
extern u8 fade_type3_72[];
extern u8 fade_type3_em02_00643DC0[];
extern u8 fade_type3_em02_2[];
extern u8 fade_type5_74[];

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
s16 Em_area_ck(int);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotX(f32 *, f32);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void RotateX(FLMAT *, f32);
void RotateY(FLMAT *, f32);
FLMAT *get_joint_wmat(void *, s16);
void get_joint_pos_em(EMW *, s16, f32 *);
f32 GetGroundHit(f32 *);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void Material_set_sub(void *, CLAY *);

static f32 eft17_ran_suu_sub(void);
static f32 eft17_ran_suu_sub2(void);
static void eft17_move(EFTW *ew);
static void eft17_i(EFTW *ew);
static void eft17_m(EFTW *ew);
static void eft17_d(EFTW *ew);
static void eft17_t(PRIM *pr);
static void eft17_i00(EFTW *ew);
static void eft17_m00(EFTW *ew);
static void eft17_t00(PRIM *pr);
static void eft17_i08(EFTW *ew);
static void eft17_m08(EFTW *ew);
static void eft17_t08(PRIM *pr);
static void eft17_d00(EFTW *ew);
static void eft17_d08(EFTW *ew);
static void eft17_e(EFTW *ew);
static EFTW *eft17_set_com(int arg, s16 ang);

static f32 eft17_ran_suu_sub(void) {
    return 0.001f * (f32)((u16)ran_suu(1) & 0x3FF);
}

static f32 eft17_ran_suu_sub2(void) {
    return 0.001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
}

static void eft17_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft17_i(ew);
        break;
    case 1:
        eft17_m(ew);
        break;
    case 2:
        eft17_d(ew);
        break;
    case 3:
        eft17_e(ew);
        break;
    }
}

static void eft17_i(EFTW *ew) {
    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    switch (ew->arg) {
    case 8:
        eft17_i08(ew);
        break;
    default:
        eft17_i00(ew);
        break;
    }
}

static void eft17_m(EFTW *ew) {
    switch (ew->arg) {
    case 8:
        eft17_m08(ew);
        break;
    default:
        eft17_m00(ew);
        break;
    }
}

static void eft17_d(EFTW *ew) {
    switch (ew->arg) {
    case 8:
        eft17_d08(ew);
        break;
    default:
        eft17_d00(ew);
        break;
    }
}

static void eft17_t(PRIM *pr) {
    switch (((EFTW *)pr->owner)->arg) {
    case 8:
        eft17_t08(pr);
        break;
    default:
        eft17_t00(pr);
        break;
    }
}

static void eft17_i00(EFTW *ew) {
    FLMAT m;
    f32 v[3];
    s16 n;
    u16 ang;
    EFT17_PIECE *p = ew->work;
    s16 i;

    n = eft17_num[ew->arg];
    switch (ew->arg) {
    case 0:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 50.0f;
        flvecRotY(v, ANG2RAD(ew->u0A.joint));
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        break;
    case 1:
        ew->x07 = ew->owner->kind;
        v[0] = 0.0f;
        if (ew->x07 == 2) {
            flmatCopy(&m, get_joint_wmat(ew->owner, 0x33));
            v[1] = -41.8f;
            v[2] = 50.0f;
        } else {
            flmatCopy(&m, get_joint_wmat(ew->owner, 0x22));
            v[1] = -80.0f;
            v[2] = 80.0f;
        }
        flvecApplyMat33_2(v, &m);
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        ew->u0A.joint = ew->owner->ang[1];
        break;
    case 2:
        ew->x07 = ew->owner->kind;
        v[0] = 0.0f;
        if (ew->x07 == 2) {
            flmatCopy(&m, get_joint_wmat(ew->owner, 0x33));
            v[1] = -91.6f;
            v[2] = 132.8f;
        } else {
            flmatCopy(&m, get_joint_wmat(ew->owner, 0x22));
            v[1] = -80.0f;
            v[2] = 120.0f;
        }
        flvecApplyMat33_2(v, &m);
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        ew->u0A.joint = ew->owner->ang[1];
        break;
    case 3:
        if (ew->mode2 == 0) {
            n = 1;
        } else {
            n = ew->mode2;
        }
        ang = ew->u0A.joint;
        ew->u0A.joint = ew->timer;
        ew->timer = 0;
        break;
    case 6:
        ew->u0A.joint = ran_suu(1);
        break;
    case 7:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 50.0f;
        flvecRotY(v, ANG2RAD(ew->u0A.joint));
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        ew->u0A.joint = ran_suu(1);
        break;
    case 12:
        flmatCopy(&m, get_joint_wmat(ew->owner, ew->u0A.joint));
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 50.0f * ew->owner->scale[2];
        flvecApplyMat33_2(v, &m);
        v[0] += ew->pos[0];
        v[1] += ew->pos[1];
        v[2] += ew->pos[2];
        RotateX(&m, ANG2RAD(ew->mode2 << 8));
        flvecCopy(ew->pos, m[2]);
        ew->pos[0] *= 10.0f;
        ew->pos[1] *= 10.0f;
        ew->pos[2] *= 10.0f;
        ew->x07 = ew->owner->kind;
        break;
    case 14:
        ew->x07 = ew->owner->kind;
        ew->scale *= 2.0f;
        break;
    case 5:
    case 16:
    case 17:
        break;
    case 18:
        ew->stg = (u16)ran_suu(1) & 0x1FF;
        break;
    }
    ew->timer = 0;
    for (i = 0; i < n; p++, i++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->no = i;
            switch (ew->arg) {
            case 0:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                p->u28.size = 1.0f;
                break;
            case 1:
                switch (i) {
                case 0:
                case 1:
                    p->no = 0;
                    p->lag = 0;
                    break;
                case 2:
                case 3:
                    p->no = 1;
                    p->lag = -2;
                    break;
                }
                p->u28.size = ew->scale * (1.0f + 0.5f * eft17_ran_suu_sub());
                p->rot[1] = ran_suu(1);
                p->rot[2] = ran_suu(1);
                p->rot[0] = (ran_suu(1) & 0xFF) - 0x80;
                break;
            case 2:
                if (i == 0) {
                    p->lag = 0;
                } else {
                    p->lag = -1;
                }
                p->rot[2] = ran_suu(1);
                p->u28.size = 0.0f;
                flvecCopy(p->pos, ew->pos);
                break;
            case 3:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                p->rot[1] = ang;
                if (ew->x07 == 2) {
                    p->u28.size = 1.5f;
                } else {
                    p->u28.size = 1.0f;
                }
                if (i == 0) {
                    p->u28.size *= 1.0f + eft17_ran_suu_sub();
                    p->rot[0] = (ran_suu(1) & 0xFF) - 0x80;
                } else {
                    p->u28.size *= 1.0f + 1.5f * eft17_ran_suu_sub();
                    p->rot[0] = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                }
                break;
            case 4:
                p->lag = -5 - i * 2;
                p->rot[2] = ran_suu(1);
                p->rot[0] = (s32)(0.5f + 65536.0f * eft17_ran_suu_sub2() / 360.0f);
                p->u28.size = ew->scale * (1.0f + 0.5f * eft17_ran_suu_sub());
                p->pos[0] = ew->scale * (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[1] = ew->scale * (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[2] = i * -10;
                break;
            case 5:
            case 16:
                p->lag = -4 - i * 5;
                p->no = i;
                p->rot[2] = ran_suu(1);
                p->u28.size = ew->scale * (1.0f + 0.5f * eft17_ran_suu_sub());
                p->rot[0] = (s32)(0.5f + 65536.0f * (2.0f * eft17_ran_suu_sub2()) / 360.0f);
                p->pos[0] = 3.3333333f * (2.0f * eft17_ran_suu_sub2());
                p->pos[1] = 7.5f;
                p->pos[2] = 0.0f;
                break;
            case 6:
                p->lag = 0;
                p->rot[2] = 0;
                p->u28.size = 1.0f;
                p->alpha = 1.0f;
                break;
            case 7:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                p->u28.size = 1.0f;
                p->alpha = 1.0f;
                break;
            case 9:
                p->lag = -i;
                p->rot[2] = ran_suu(1);
                break;
            case 10:
                p->lag = -2;
                p->pos[0] = ew->pos[0];
                p->pos[1] = 2.0f + ew->pos[1];
                p->pos[2] = ew->pos[2];
                break;
            case 11:
                p->lag = eft17_type11_lag_tbl[i];
                p->u28.size = 1.2f * ew->scale;
                break;
            case 12:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                p->u28.size = 0.0f;
                flvecCopy(p->pos, v);
                break;
            case 13:
                p->lag = 0;
                p->no = (u16)ran_suu(1) & 3;
                p->rot[2] = 0;
                p->u28.size = ew->scale * (0.8f + 0.4f * eft17_ran_suu_sub());
                flvecCopy(p->pos, ew->pos);
                break;
            case 14:
                p->lag = 0;
                p->no = (u16)ran_suu(1) & 1;
                p->rot[2] = ran_suu(1);
                p->u28.size = ew->scale * (0.8f + 0.4f * eft17_ran_suu_sub());
                p->pos[0] = ew->pos[0] + 5.0f * eft17_ran_suu_sub2();
                p->pos[1] = ew->pos[1] + 5.0f * eft17_ran_suu_sub2();
                p->pos[2] = ew->pos[2] + 5.0f * eft17_ran_suu_sub2();
                break;
            case 15:
                p->lag = eft17_type15_lag_tbl[i];
                if (i == 0) {
                    p->no = 0;
                } else {
                    p->no = (u16)ran_suu(1) % 6 + 1;
                }
                p->rot[2] = 0;
                flvecCopy(p->pos, ew->pos);
                p->u28.size = ew->scale * (0.8f + 0.4f * eft17_ran_suu_sub());
                break;
            case 17:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                flvecCopy(p->pos, ew->pos);
                break;
            case 18:
                p->lag = 0;
                p->rot[2] = ran_suu(1);
                p->u28.size = 0.8f + 0.2f * eft17_ran_suu_sub();
                break;
            case 19:
                p->lag = i * -3;
                p->rot[2] = ran_suu(1);
                p->u28.size = 0.0f;
                flvecCopy(p->pos, v);
                p->pos[0] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[1] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[2] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                break;
            case 20:
                p->lag = i * -3;
                p->no = (u16)ran_suu(1) & 3;
                p->rot[2] = 0;
                p->u28.size = ew->scale * (0.8f + 0.4f * eft17_ran_suu_sub());
                flvecCopy(p->pos, ew->pos);
                p->pos[0] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[1] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                p->pos[2] += (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                break;
            }
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft17_t;
        } else {
            p->prim = 0;
        }
    }
    eft17_m(ew);
}

static void eft17_m00(EFTW *ew) {
    FLMAT m;
    f32 v[3];
    s32 n;
    s32 nstep;
    s16 num;
    s16 idx;
    s16 step;
    u16 all;
    u16 time;
    u16 *tt;
    s16 i;
    EMW *em;
    void *d;
    EFT17_PIECE *p = ew->work;

    num = eft17_num[ew->arg];
    idx = eft17_index[ew->arg];
    step = eft17_param[ew->arg];
    all = eft17_all_time[ew->arg];
    tt = eft17_time[ew->arg];
    switch (ew->arg) {
    case 1:
        time = 0x12;
        break;
    case 3:
        if (ew->mode2 == 0) {
            num = 1;
        } else {
            num = ew->mode2;
        }
        break;
    case 4:
        time = 0x26;
        break;
    case 5:
    case 16:
        time = 0x18;
        break;
    case 10:
        time = 6;
        break;
    case 0:
    case 6:
    case 7:
    case 12:
    case 13:
    case 17:
    case 18:
        time = all;
        break;
    case 14:
        em = ew->owner;
        time = all;
        if (em->x04 == 3 || em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        flmatCopy(&m, get_joint_wmat(em, ew->u0A.joint));
        v[0] = 0.0f;
        v[1] = -50.0f;
        v[2] = 100.0f;
        flvecApplyMat33_2(v, &m);
        ew->pos[0] = m[3][0] + v[0];
        ew->pos[1] = m[3][1] + v[1];
        ew->pos[2] = m[3][2] + v[2];
        break;
    case 19:
        time = 0xF;
        break;
    case 20:
        time = 0x12;
        break;
    }
    if (++ew->timer >= all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    n = num;
    nstep = step;
    for (i = 0; i < n; i++) {
        switch (ew->arg) {
        case 1:
        case 4:
        case 5:
        case 16:
        case 19:
        case 20:
            idx = eft17_index[ew->arg];
            break;
        case 2:
        case 11:
        case 15:
            time = tt[i];
            break;
        case 3:
            time = 10;
            if (i != 0) {
                time = 35;
            }
            break;
        case 9:
            time = 15;
            if (i != 0) {
                time = 20;
            }
            break;
        }
        if (++p->lag <= 0) {
            p++;
            idx += step;
            continue;
        }
        if (p->lag >= time) {
            p++;
            idx += step;
            continue;
        }
        d = eft17_data[idx++];
        eft_vec_linear(p->lag, d, p->scale);
        switch (ew->arg) {
        case 0:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 80.0f * (f32)p->lag / (f32)time;
            flvecRotY(v, ANG2RAD(ew->u0A.joint));
            p->pos[0] = ew->pos[0] + v[0];
            p->pos[1] = ew->pos[1] + v[1];
            p->pos[2] = ew->pos[2] + v[2];
            break;
        case 5:
        case 16:
            p->rot[2] += p->rot[0];
            break;
        case 1:
            d = eft17_data[idx++];
            eft_vec_linear(p->lag, d, p->pos);
            switch (p->no) {
            case 0:
                break;
            case 1:
                p->pos[0] -= 40.0f;
                break;
            }
            if (i == 1 || i == 3) {
                p->pos[0] = -p->pos[0];
            }
            p->pos[0] *= ew->scale;
            p->pos[1] *= ew->scale;
            p->pos[2] *= ew->scale;
            flvecRotY(p->pos, ANG2RAD(ew->u0A.joint));
            p->pos[0] += ew->pos[0];
            p->pos[1] += ew->pos[1];
            p->pos[2] += ew->pos[2];
            p->rot[2] += p->rot[0];
            break;
        case 2:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 5.0f * (f32)i;
            flvecApplyMat33_2(v, &rview_mat);
            p->pos[0] = ew->pos[0] + v[0];
            p->pos[1] = ew->pos[1] + v[1];
            p->pos[2] = ew->pos[2] + v[2];
            break;
        case 3:
            p->pos[0] = 0.0f;
            p->pos[1] = 0.0f;
            if (i == 0) {
                p->pos[2] = 120.0f - 50.0f * (f32)p->lag / (f32)time;
            } else {
                p->pos[2] = -(30.0f * (f32)p->lag / (f32)time);
            }
            p->rot[2] += p->rot[0];
            flvecRotX(p->pos, ANG2RAD(ew->u0A.joint));
            flvecRotY(p->pos, ANG2RAD(p->rot[1]));
            p->pos[0] += ew->pos[0];
            p->pos[1] += ew->pos[1];
            p->pos[2] += ew->pos[2];
            break;
        case 4:
            flvecApplyMat33(v, p->pos, &rview_mat);
            p->rot[2] += p->rot[0];
            break;
        case 6:
            p->u28.size = ew->scale * (1.0f + 0.2f * eft17_ran_suu_sub());
            flvecCopy(p->pos, ew->pos);
            break;
        case 7:
            p->rot[2] = ran_suu(1);
            p->u28.size = ew->scale * (1.0f + 0.2f * eft17_ran_suu_sub());
            flvecCopy(p->pos, ew->pos);
            if (p->lag >= 0x18) {
                p->alpha = (f32)(time - p->lag) / 13.0f;
            }
            break;
        case 9:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 20.0f + (f32)(i * 3);
            flvecApplyMat33_2(v, &rview_mat);
            p->pos[0] = ew->pos[0] + v[0];
            p->pos[1] = ew->pos[1] + v[1];
            p->pos[2] = ew->pos[2] + v[2];
            break;
        case 10:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = -1000.0f;
            flvecApplyMat33_2(v, &rview_mat);
            p->pos[0] = ew->pos[0] + v[0];
            p->pos[1] = ew->pos[1] + v[1];
            p->pos[2] = ew->pos[2] + v[2];
            break;
        case 11:
            d = eft17_data[idx++];
            eft_vec_linear(p->lag, d, p->pos);
            p->pos[0] *= ew->scale;
            p->pos[1] *= ew->scale;
            p->pos[2] *= ew->scale;
            p->pos[0] += ew->pos[0];
            p->pos[1] += ew->pos[1];
            p->pos[2] += ew->pos[2];
            break;
        case 12:
            p->pos[0] += ew->pos[0];
            p->pos[1] += ew->pos[1];
            p->pos[2] += ew->pos[2];
            break;
        case 14:
            d = eft17_data[idx++];
            eft_vec_linear(p->lag, d, p->pos);
            flvecApplyMat33_2(p->pos, &m);
            p->pos[0] += ew->pos[0];
            p->pos[1] += ew->pos[1];
            p->pos[2] += ew->pos[2];
            break;
        case 15:
            d = eft17_data[idx++];
            eft_vec_linear(p->lag, d, v);
            flvecRotY(v, ANG2RAD(ew->u0A.joint));
            p->pos[0] = 0.0f;
            p->pos[1] = 0.0f;
            p->pos[2] = 5.0f * (f32)i;
            flvecApplyMat33_2(p->pos, &rview_mat);
            p->pos[0] += ew->pos[0] + v[0];
            p->pos[1] += ew->pos[1] + v[1];
            p->pos[2] += ew->pos[2] + v[2];
            break;
        case 18:
            p->rot[2] += (u16)(ew->stg - 0x100);
            p->pos[0] = ew->pos[0];
            p->pos[1] = ew->pos[1];
            p->pos[2] = ew->pos[2];
            break;
        case 19:
        case 20:
            p->pos[0] = ew->pos[0];
            p->pos[1] = ew->pos[1];
            p->pos[2] = ew->pos[2];
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 5.0f * (f32)i;
            flvecApplyMat33_2(v, &rview_mat);
            break;
        }
        if (nstep >= 2 && ew->arg != 0xF) {
            d = eft17_data[idx++];
            eft_alpha_linear(p->lag, d, &p->alpha);
        }
        if (p->prim != 0) {
            switch (ew->arg) {
            case 5:
            case 16:
                p->prim->pos[0] = ew->pos[0] + (f32)p->lag * p->pos[0];
                p->prim->pos[1] = ew->pos[1] + (f32)p->lag * p->pos[1];
                p->prim->pos[2] = ew->pos[2] + (f32)p->lag * p->pos[2];
                break;
            case 4:
            case 19:
            case 20:
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                break;
            default:
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

static void eft17_i08(EFTW *ew) {
    f32 h;
    EFT17_PIECE *p = ew->work;
    s16 i;

    switch (ew->arg) {
    case 8:
        h = 70.0f;
        break;
    }
    ew->timer = 0;
    for (i = 0; i < 10; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->no = (u16)ran_suu(1) % 3;
            p->lag = 0;
            flvecCopy(p->pos, ew->pos);
            p->alpha = ew->scale * (1.5f * (1.0f + eft17_ran_suu_sub()));
            if (i < 5) {
                p->scale[0] = 40.0f * ew->scale * eft17_ran_suu_sub2();
                p->scale[1] = ew->scale * (100.0f + h * eft17_ran_suu_sub()) / p->alpha;
                p->scale[2] = 40.0f * ew->scale * eft17_ran_suu_sub2();
            } else {
                p->scale[0] = 80.0f * ew->scale * eft17_ran_suu_sub2();
                p->scale[1] = ew->scale * (100.0f + h * eft17_ran_suu_sub()) / p->alpha;
                p->scale[2] = 80.0f * ew->scale * eft17_ran_suu_sub2();
            }
            p->rot[0] = ran_suu(1);
            p->rot[1] = ran_suu(1);
            p->rot[2] = ran_suu(1);
            p->u28.spin = ((u16)ran_suu(1) & 0xFFF) - 0x7FF;
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft17_t;
        } else {
            p->prim = 0;
        }
    }
}

static void eft17_m08(EFTW *ew) {
    f32 y;
    EFT17_PIECE *p = ew->work;
    s16 i;
    s16 cnt;

    if (++ew->timer > 0x96) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    cnt = 0;
    for (i = 0; i < 10; i++) {
        if (p->no == -1) {
            p++;
            cnt++;
            continue;
        }
        p->pos[0] += p->scale[0];
        p->pos[1] += p->scale[1];
        p->pos[2] += p->scale[2];
        p->scale[1] += -9.8f;
        p->rot[2] += p->u28.spin;
        y = GetGroundHit(p->pos);
        if (p->pos[1] <= y) {
            p->no--;
            if (p->no < 0) {
                p++;
                continue;
            }
            p->pos[1] = y;
            p->scale[1] *= -0.5f;
        }
        if (p->prim != 0) {
            p->prim->pos[0] = p->pos[0];
            p->prim->pos[1] = p->pos[1];
            p->prim->pos[2] = p->pos[2];
            add_prim(ot1, p->prim, 0x20, 0);
        }
        p++;
    }
    if (cnt >= 10) {
        ew->mode++;
        ew->be_flag = 0;
    }
}

static void eft17_t08(PRIM *pr) {
    FLMAT m;
    f32 rot[3];
    f32 sc[3];
    EFTW *ew = pr->owner;
    EFT17_PIECE *p = &((EFT17_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        rot[0] = ANG2RAD(p->rot[0]);
        rot[1] = ANG2RAD(p->rot[1]);
        rot[2] = ANG2RAD(p->rot[2]);
        sc[0] = p->alpha;
        sc[1] = p->alpha;
        sc[2] = p->alpha;
        make_mat_srt(sc, rot, pr->pos, 0xE, &m);
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[73];
        if (cl != 0 && cl->handle != -1) {
            flSetRenderState(0x60, 0x80);
            flSetRenderState(0x1A, (u32)&m);
            Material_set_sub(mats, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        flSetRenderState(0x60, 0);
        clay_attr_reset();
    }
}

static void eft17_d00(EFTW *ew) {
    s16 n;
    s16 i;
    EFT17_PIECE *p = ew->work;

    ew->mode++;
    n = eft17_num[ew->arg];
    switch (ew->arg) {
    case 3:
        if (ew->mode2 == 0) {
            n = 1;
        } else {
            n = ew->mode2;
        }
        break;
    }
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft17_d08(EFTW *ew) {
    EFT17_PIECE *p = ew->work;
    s16 i;

    ew->mode++;
    for (i = 0; i < 10; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft17_e(EFTW *ew) {
    push_eft_work(ew);
}

static EFTW *eft17_set_com(int arg, s16 ang) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) == 0) {
        return 0;
    }
    ew->type = 0x11;
    ew->move = eft17_move;
    ew->arg = arg;
    ew->u0A.joint = ang;
    return ew;
}

void Eft17_set(EMW *em, s16 joint, int arg, int flag) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        if ((ew = eft17_set_com(arg, joint)) != 0) {
            ew->mode2 = flag;
            get_joint_pos_em(em, joint, ew->pos);
            ew->owner = em;
            switch ((s16)arg) {
            case 1:
            case 2:
                    if ((u16)flag == 0) {
                        ew->scale = 1.0f;
                    } else {
                        ew->scale = 1.5f;
                    }
                break;
            default:
                ew->scale = 1.0f;
                break;
            }
        }
    }
}

void Eft17_type3_set(f32 *pos, s16 time, int ang, int n, int kind) {
    EFTW *ew;

    ew = eft17_set_com(3, (s16)ang);
    if (ew != 0) {
        ew->mode2 = n;
        ew->timer = time;
        ew->x07 = kind;
        ew->scale = 1.0f;
        ew->owner = 0;
        flvecCopy(ew->pos, pos);
    }
}

void Eft17_set_ex(f32 *pos, int ang, int arg, f32 scale) {
    EFTW *ew;

    ew = eft17_set_com(arg, (s16)ang);
    if (ew != 0) {
        ew->scale = scale;
        ew->owner = 0;
        flvecCopy(ew->pos, pos);
    }
}

void Eft17_set_ang(EMW *em, s16 joint, int arg, u16 ang) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        if ((ew = eft17_set_com(arg, joint)) != 0) {
            ew->mode2 = ang >> 8;
            get_joint_pos_em(em, joint, ew->pos);
            ew->owner = em;
            ew->scale = 1.0f;
        }
    }
}

void Eft17_set_pos(f32 *pos, int arg, int kind, f32 scale) {
    EFTW *ew;

    if ((ew = eft17_set_com(arg, 0)) != 0) {
        ew->x07 = kind;
        flvecCopy(ew->pos, pos);
        ew->owner = 0;
        ew->scale = scale;
    }
}

void Eft17_set_pos_ang(f32 *pos, int arg, int kind, int ang, f32 scale) {
    EFTW *ew;

    if ((ew = eft17_set_com(arg, (s16)ang)) != 0) {
        ew->x07 = kind;
        flvecCopy(ew->pos, pos);
        ew->owner = 0;
        ew->scale = scale;
    }
}
