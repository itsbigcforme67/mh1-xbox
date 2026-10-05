/* eft04 - game.bin 0x0053FFD0-0x00542??? (see config/c_files.txt). Monster
 * attack effects with nine types (arg): each spawns up to eft04_num pieces
 * that grow and fade along keyframe tables (eft04_data, through
 * eft04_index/param) and are drawn with the monster area's model set
 * (game_w.area_mdlw). Types 0, 3 and 8 follow a monster joint while the
 * monster plays one animation (0x45A, 0x417, 0x40F); type 0 scatters four
 * pieces over the joints in eft04_em15_pos. Placed at a monster
 * (Eft04_set, Eft04_set_time) or at a point (Eft04_set_pos). */
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

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT04_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 */
    f32 pos[3];         /* 0x04 */
    s16 lag;            /* 0x10 frame counter, starts at 0 or below */
    u16 rot[3];         /* 0x12 */
    f32 size;           /* 0x18 */
    PRIM *prim;         /* 0x1C */
    f32 scale[3];       /* 0x20 from the keyframes */
    u8 col;             /* 0x2C 0: opaque (ot1), else blended (ot0) */
    u8 joint;           /* 0x2D type 0: entry in eft04_em15_pos */
    u8 alpha;           /* 0x2E */
    u8 speed;           /* 0x2F type 2: rise per frame times time */
} EFT04_PIECE;

/* Joint offsets for type 0 (0x14 bytes each). */
typedef struct EFT04_JPOS {
    s16 joint;          /* 0x00 */
    f32 ofs[3];         /* 0x04 */
    f32 size;           /* 0x10 */
} EFT04_JPOS;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern f32 D_3F2090[3];
extern void *eft04_data[];
extern s16 eft04_num[9];
extern s16 eft04_all_time[9];
extern s16 eft04_param[9];
extern s16 eft04_index[9];
extern s16 *eft04_time_tbl[9];
extern EFT04_JPOS eft04_em15_pos[];
extern u8 *eft04_type4_fade_data[];
extern u8 *eft04_type5_fade_data[];
extern u8 fade_type3_em02_00644B70[];
extern u8 fade_type3_em03[];
extern u8 fade_type3_em05[];
extern u8 fade_type3_em07[];

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
s16 Em_area_ck(int);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flmatInit(FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void PointToPoint(f32 *, f32 *, f32 *);
FLMAT *get_joint_wmat_em(EMW *, int);
int em_frame_check2(EMW *, int, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
void Eft13_set_pos(f32 *, int, f32);

static void eft04_move(EFTW *ew);
static void eft04_i(EFTW *ew);
static void eft04_m(EFTW *ew);
static void eft04_d(EFTW *ew);
static void eft04_e(EFTW *ew);
static void eft04_t(PRIM *pr);
static void eft04_pos_calc(f32 *pos, EMW *em, f32 *ofs, int joint);
static void eft04_type0_0_init(EFTW *ew, EFT04_PIECE *p);
static void eft04_type3_init(EFTW *ew, EFT04_PIECE *p);
static void eft04_type8_init(EFTW *ew, EFT04_PIECE *p);
static void eft04_z_adj(FLMAT *m, f32 *pos);

static void eft04_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft04_i(ew);
        break;
    case 1:
        eft04_m(ew);
        break;
    case 2:
        eft04_d(ew);
        break;
    case 3:
        eft04_e(ew);
        break;
    }
}

static void eft04_i(EFTW *ew) {
    f32 v[3];
    s16 n;
    EFT04_PIECE *p = ew->work;
    EMW *em = ew->owner;
    s16 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->stg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft04_num[ew->arg];
    switch (ew->arg) {
    case 4:
        v[0] = 0.0f;
        v[1] = -25.0f;
        v[2] = 100.0f;
        eft04_pos_calc(ew->pos, em, v, 0x22);
        break;
    }
    for (i = 0; i < n; p++, i++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->lag = 0;
            p->no = i;
            p->scale[0] = 1.0f;
            p->scale[1] = 1.0f;
            p->scale[2] = 1.0f;
            p->size = ew->scale;
            switch (ew->arg) {
            case 0:
                if (i == 0) {
                    p->no = 0;
                    p->rot[0] = ran_suu(1);
                    p->rot[1] = ran_suu(1);
                    p->rot[2] = ran_suu(1);
                    p->size = 3.5f;
                    p->col = 0;
                } else {
                    p->no = 1;
                    p->rot[2] = ran_suu(1);
                    p->col = 1;
                }
                break;
            case 1:
                p->rot[2] = ran_suu(1);
                p->col = 1;
                flvecCopy(p->pos, ew->pos);
                break;
            case 2:
                p->lag = -2 - i * 2;
                p->rot[2] = ran_suu(1);
                p->col = 1;
                p->pos[0] = ew->pos[0] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                p->pos[1] = ew->pos[1] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                p->pos[2] = ew->pos[2] + 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                flvecCopy(p->pos, ew->pos);
                p->size = 1.2f - 0.2f * (f32)i;
                p->speed = ((u16)ran_suu(1) & 0x1F) + 0x4B;
                break;
            case 3:
                if (i == 4) {
                    p->no = 3;
                    p->lag = -5;
                }
                p->rot[2] = ran_suu(1);
                if (p->no == 2) {
                    p->col = 0;
                } else {
                    p->col = 1;
                }
                break;
            case 4:
                switch (i) {
                case 0:
                    p->rot[0] = 0;
                    p->rot[1] = em->ang[1];
                    break;
                case 1:
                    p->rot[0] = 0;
                    p->rot[1] = em->ang[1];
                    p->size = p->size * 0.75f;
                    break;
                case 2:
                    p->rot[2] = ran_suu(1);
                    break;
                case 4:
                    p->lag = -1;
                    p->rot[2] = ran_suu(1);
                    break;
                case 5:
                    p->lag = -4;
                    p->rot[1] = em->ang[1] + 0x8000;
                    p->size = p->size * 0.5f;
                    break;
                }
                p->col = 1;
                flvecCopy(p->pos, ew->pos);
                p->size = p->size * 2.0f;
                break;
            case 5:
                if (i == 1) {
                    p->no = 0;
                    p->lag = -2;
                } else if (i == 2) {
                    p->no = 1;
                }
                p->rot[2] = ran_suu(1);
                p->col = 1;
                flvecCopy(p->pos, ew->pos);
                break;
            case 6:
                p->lag = -i * 3;
                p->rot[2] = ran_suu(1);
                p->col = 1;
                flvecCopy(p->pos, ew->pos);
                break;
            case 7:
                p->lag = -4;
                flvecCopy(p->pos, ew->pos);
                switch (p->no) {
                case 0:
                    p->rot[2] = ran_suu(1);
                    break;
                case 2:
                    p->rot[1] = ran_suu(1);
                    p->pos[1] = p->pos[1] + 5.0f;
                    break;
                }
                p->col = 1;
                break;
            case 8:
                switch (p->no) {
                case 0:
                    p->rot[0] = ran_suu(1);
                    p->rot[1] = ran_suu(1);
                    p->rot[2] = ran_suu(1);
                    p->size = p->size * 0.5f;
                    p->col = 0;
                    break;
                default:
                    p->rot[2] = ran_suu(1);
                    p->size = p->size * 3.0f;
                    p->col = 1;
                    break;
                }
                break;
            }
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft04_t;
        } else {
            p->prim = 0;
        }
    }
    eft04_m(ew);
}

static void eft04_m(EFTW *ew) {
    f32 a;
    f32 v[3];
    s16 *tt;
    u8 flag;
    s32 n;
    s16 num;
    s16 all;
    EFT04_PIECE *p = ew->work;
    s16 i;
    EMW *em = ew->owner;
    s16 time;
    s16 idx;
    s32 k;
    void *d;
    s16 step;

    num = eft04_num[ew->arg];
    all = eft04_all_time[ew->arg];
    tt = eft04_time_tbl[ew->arg];
    idx = eft04_index[ew->arg];
    step = eft04_param[ew->arg];
    if (ew->arg == 3 || ew->arg == 8) {
        all = ew->u0A.joint;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    switch (ew->arg) {
    case 0:
        if (em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        if (em->char0 != 0x45A) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        eft04_type0_0_init(ew, p);
        break;
    case 2:
        time = 0x16;
        break;
    case 3:
        if (em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        if (em->char0 != 0x417) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        v[0] = 0.0f;
        v[1] = -25.0f;
        v[2] = 100.0f;
        eft04_pos_calc(ew->pos, em, v, 0x22);
    case 7:
        if (ew->timer == all) {
            Eft13_set_pos(ew->pos, 0x1B, 1.0f);
        }
        break;
    case 8:
        if (em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        if (em->char0 != 0x40F) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        v[0] = 0.0f;
        v[1] = 14.0f;
        v[2] = 24.0f;
        eft04_pos_calc(ew->pos, em, v, 0x17);
        break;
    }
    n = num;
    for (i = 0; i < n; i++, p++) {
        p->lag++;
        if (tt != 0) {
            time = tt[i];
        } else {
            idx = eft04_index[ew->arg];
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 0.0f;
        switch (ew->arg) {
        case 0:
            if (p->no == 0) {
                if (em_frame_check2(em, 0, 115.0f) == 0) {
                    flag = 1;
                    continue;
                }
                flag = 0;
                if (p->lag >= 4) {
                    p->lag = 0;
                    p->rot[0] = ran_suu(1);
                    p->rot[1] = ran_suu(1);
                    p->rot[2] = ran_suu(1);
                }
                eft04_pos_calc(p->pos, em, v, 3);
            } else if (flag) {
                p->col = 2;
            } else {
                p->col = 1;
            }
            break;
        case 1:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            k = idx;
            eft_vec_linear(p->lag, eft04_data[k], p->scale);
            idx += 2;
            eft_alpha_linear(p->lag, eft04_data[(s16)(k + 1)], &a);
            p->alpha = 255.0f * a;
            if (p->no == 0) {
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 20.0f;
                flvecApplyMat33_2(v, &rview_mat);
            } else {
                p->pos[1] -= 50.0f / (f32)time;
            }
            break;
        case 2:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            k = idx;
            eft_vec_linear(p->lag, eft04_data[k], p->scale);
            idx += 2;
            eft_alpha_linear(p->lag, eft04_data[(s16)(k + 1)], &a);
            p->alpha = 255.0f * a;
            p->pos[1] += (f32)p->speed / (f32)time;
            break;
        case 3:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                eft04_type3_init(ew, p);
                if (p->lag <= 0) {
                    idx += step;
                    continue;
                }
            }
            d = eft04_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            flvecCopy(p->pos, ew->pos);
            break;
        case 4:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            d = eft04_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            break;
        case 5:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            d = eft04_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            break;
        case 6:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            d = eft04_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            switch (p->no) {
            case 0:
                p->rot[2] = ran_suu(1);
                break;
            case 1:
                p->rot[2] += 0x1000;
                break;
            }
            break;
        case 7:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                idx += step;
                continue;
            }
            k = idx;
            eft_vec_linear(p->lag, eft04_data[k], p->scale);
            idx += 2;
            eft_alpha_linear(p->lag, eft04_data[(s16)(k + 1)], &a);
            p->alpha = 255.0f * a;
            switch (p->no) {
            case 0:
                p->rot[2] += 0x1000;
                break;
            }
            break;
        case 8:
            if (p->lag <= 0) {
                idx += step;
                continue;
            }
            if (p->lag > time) {
                eft04_type8_init(ew, p);
            }
            flvecCopy(p->pos, ew->pos);
            break;
        }
        if (p->prim != 0) {
            p->prim->pos[0] = p->pos[0] + v[0];
            p->prim->pos[1] = p->pos[1] + v[1];
            p->prim->pos[2] = p->pos[2] + v[2];
            if (p->col != 0) {
                add_prim(ot0, p->prim, 0x40, 0);
            } else {
                add_prim(ot1, p->prim, 0x20, 0);
            }
        }
    }
}

static void eft04_d(EFTW *ew) {
    s16 n;
    s16 i;
    EFT04_PIECE *p = ew->work;

    ew->mode++;
    n = eft04_num[ew->arg];
    if (ew->prim != 0) {
        release_prim(ew->prim_no);
    }
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft04_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft04_t(PRIM *pr) {
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    FLMAT uv;
    u32 col;
    EFTW *ew = pr->owner;
    EFT04_PIECE *p = &((EFT04_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw;
    void *mats;
    CLAY *cl;
    u8 *fade;
    u16 flag = 0;
    u16 order;
    u16 mul;
    s16 area;
    u8 alpha;

    if (eft_mdlw[0] != 0 && eft_mdlw[0]->flag != 0) {
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        mul = 1;
        area = Em_area_ck(ew->x07);
        if (area != -1) {
        mw = game_w.area_mdlw[area];
        if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        switch (ew->arg) {
        case 0:
            if (p->no == 0) {
                cl = &mw->clay[7];
                rot[0] = DEG2RAD(ANG2DEG(p->rot[0]));
                rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 0xE;
                flmatMakeTrans(&uv, 0.0f, 0.046875f * (f32)p->lag, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                alpha = 0xFF;
                mul = 0;
            } else {
                cl = mw->clay;
                flag |= 2;
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                if (p->col == 1) {
                    alpha = 0xFF;
                } else {
                    alpha = 0x80;
                    sc[0] *= 0.8f;
                    sc[1] *= 0.8f;
                    sc[2] *= 0.8f;
                }
            }
            make_mat_srt(sc, rot, pr->pos, order, &m);
            col = (alpha << 24) | 0xFFFFFF;
            break;
        case 1:
            if (p->no == 0) {
                flag |= 2;
                cl = &mw->clay[3];
            } else {
                cl = &mw->clay[11];
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            col = (p->alpha << 24) | 0xFFFFFF;
            break;
        case 2:
            cl = &mw->clay[10];
            rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            col = (p->alpha << 24) | 0xFFFFFF;
            break;
        case 3:
            switch (p->no) {
            case 0:
                fade = fade_type3_em02_00644B70;
                cl = &mw->clay[2];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flag |= 2;
                break;
            case 1:
                fade = fade_type3_em03;
                order = 0;
                flag |= 2;
                cl = &mw->clay[3];
                break;
            case 2:
                fade = fade_type3_em07;
                cl = &mw->clay[7];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flmatMakeTrans(&uv, 0.0f, 0.0234375f * (f32)p->lag, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                mul = 0;
                break;
            case 3:
                fade = fade_type3_em05;
                cl = &mw->clay[5];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flag |= 2;
                break;
            }
            make_mat_srt(sc, rot, pr->pos, order, &m);
            eft_rgba_linear(fade, p->lag, &col);
            if (ew->timer < 20) {
                col = (((u32)((f32)((col >> 24) & 0xFF) * ((f32)ew->timer / 20.0f)) & 0xFF) << 24) |
                      (((col >> 16) & 0xFF) << 16) | (((col >> 8) & 0xFF) << 8) | (col & 0xFF);
            }
            break;
        case 4:
            switch (p->no) {
            case 0:
            case 1:
                cl = &mw->clay[1];
                flmatInit(&m);
                flmatRotXYZ33(&m, DEG2RAD(ANG2DEG(p->rot[0])), DEG2RAD(ANG2DEG(p->rot[1])), 0.0f);
                eft04_z_adj(&m, pr->pos);
                m[0][0] *= p->scale[0];
                m[0][1] *= p->scale[0];
                m[0][2] *= p->scale[0];
                m[1][0] *= p->scale[1];
                m[1][1] *= p->scale[1];
                m[1][2] *= p->scale[1];
                m[2][0] *= p->scale[2];
                m[2][1] *= p->scale[2];
                m[2][2] *= p->scale[2];
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                mul = 0;
                flag |= 2;
                break;
            case 2:
                cl = &mw->clay[2];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                flag |= 2;
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                break;
            case 3:
                flag |= 2;
                cl = &mw->clay[3];
                make_mat_srt(sc, rot, pr->pos, 0, &m);
                break;
            case 4:
                cl = &mw->clay[4];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                flag |= 2;
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                break;
            case 5:
                cl = &mw->clay[8];
                rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
                make_mat_srt(sc, rot, pr->pos, 4, &m);
                flmatMakeTrans(&uv, 0.0234375f * (f32)p->lag, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                mul = 0;
                break;
            }
            fade = eft04_type4_fade_data[p->no];
            if (fade == 0) {
                col = -1;
            } else {
                eft_rgba_linear(fade, p->lag, &col);
            }
            break;
        case 5:
            switch (p->no) {
            case 0:
                cl = &mw->clay[5];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flag |= 2;
                break;
            case 1:
                flag |= 2;
                order = 0;
                cl = &mw->clay[3];
                make_mat_srt(sc, rot, pr->pos, 0, &m);
                break;
            }
            fade = eft04_type5_fade_data[p->no];
            if (fade == 0) {
                col = -1;
            } else {
                eft_rgba_linear(fade, p->lag, &col);
            }
            make_mat_srt(sc, rot, pr->pos, order, &m);
            break;
        case 6:
            cl = &mw->clay[6];
            rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
            flag |= 2;
            col = -1;
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 7:
            flSetRenderState(0x6C, 0);
            switch (p->no) {
            case 0:
                cl = &mw->clay[6];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 1:
                order = 0;
                cl = &mw->clay[4];
                break;
            case 2:
                cl = &mw->clay[7];
                rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
                order = 4;
                mul = 0;
                break;
            }
            flag |= 2;
            col = (p->alpha << 24) | 0xFFFFFF;
            make_mat_srt(sc, rot, pr->pos, order, &m);
            break;
        case 8:
            flSetRenderState(0x6C, 0);
            if (p->no != 0) {
                flag |= 2;
                cl = &mw->clay[6];
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 2;
                flmatMakeTrans(&uv, 0.0f, 0.25f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
            } else {
                cl = mw->clay;
                rot[0] = DEG2RAD(ANG2DEG(p->rot[0]));
                rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
                rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
                order = 0xE;
                mul = 0;
            }
            col = -1;
            make_mat_srt(sc, rot, pr->pos, order, &m);
            break;
        }
        if (mul != 0) {
            flmatMul33_2(&m, &rview_mat);
        }
        if (p->col != 0) {
            eft_trans_sub_col(cl, &m, col, flag, mats);
        } else {
            flSetRenderState(0x67, col);
            eft_trans_sub_opa(cl, &m, mats);
        }
        flSetRenderState(0x6C, 1);
        }
        }
    }
}

static void eft04_pos_calc(f32 *pos, EMW *em, f32 *ofs, int joint) {
    FLMAT m;
    f32 v[3];

    flmatCopy(&m, get_joint_wmat_em(em, joint));
    flvecApplyMat33(v, ofs, &m);
    pos[0] = m[3][0] + v[0];
    pos[1] = m[3][1] + v[1];
    pos[2] = m[3][2] + v[2];
}

static void eft04_type0_0_init(EFTW *ew, EFT04_PIECE *p) {
    s16 sel[4];
    s16 used[4];
    s16 i;
    s16 j;
    s16 n;
    EFT04_JPOS *jp;

    p++;
    for (i = 0; i < 4; i++, p++) {
        sel[i] = (u16)ran_suu(1) % (10 - i);
        for (j = 0; j < i; j++) {
            used[j] = 0;
        }
        while (1) {
            n = 0;
            for (j = 0; j < i; j++) {
                if (used[j] == 0 && sel[j] <= sel[i]) {
                    used[j] = 1;
                    n++;
                }
            }
            if (n <= 0) {
                break;
            }
            sel[i] += n;
        }
        p->joint = sel[i];
        p->rot[2] = ran_suu(1);
        p->size = eft04_em15_pos[p->joint].size * (0.9f + 0.00020000001f * (f32)((u16)ran_suu(1) & 0x3FF));
        jp = &eft04_em15_pos[p->joint];
        eft04_pos_calc(p->pos, ew->owner, jp->ofs, jp->joint);
    }
}

static void eft04_type3_init(EFTW *ew, EFT04_PIECE *p) {
    switch (p->no) {
    case 0:
        p->lag = 1;
        p->rot[2] = ran_suu(1);
        break;
    case 1:
        p->lag = 1;
        break;
    case 2:
        p->lag = 1;
        p->rot[2] = ran_suu(1);
        break;
    case 3:
        p->lag = -1;
        p->rot[2] = ran_suu(1);
        break;
    }
}

static void eft04_type8_init(EFTW *ew, EFT04_PIECE *p) {
    p->lag = 1;
    switch (p->no) {
    case 0:
        p->rot[0] = ran_suu(1);
        p->rot[1] = ran_suu(1);
        p->rot[2] = ran_suu(1);
        break;
    default:
        p->rot[2] = ran_suu(1);
        p->size = 1.2f + 0.0006f * (f32)((u16)ran_suu(1) & 0x3FF);
        break;
    }
}

static void eft04_z_adj(FLMAT *m, f32 *pos) {
    f32 a[3];
    f32 b[3];
    f32 c[3];
    f32 d[3];

    PointToPoint(a, D_3F2090, pos);
    flvecCopy(b, (*m)[2]);
    flvecOuterProduct(c, a, b);
    flvecNormalize(c);
    flvecOuterProduct(d, b, c);
    if (flvecInnerProduct(d, d) > 0.0001f) {
        flvecOuterProduct(c, d, b);
        flvecCopy((*m)[0], c);
        flvecCopy((*m)[1], d);
    }
}

void Eft04_set(EMW *em, int arg) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 4;
            ew->move = eft04_move;
            ew->arg = arg;
            ew->owner = em;
            ew->x07 = em->kind;
            ew->prim = 0;
            ew->scale = 1.0f;
        }
    }
}

void Eft04_set_pos(f32 *pos, int arg, int kind, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 4;
        ew->move = eft04_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->x07 = kind;
        ew->prim = 0;
        ew->scale = scale;
        flvecCopy(ew->pos, pos);
    }
}

void Eft04_set_time(EMW *em, int arg, int time, f32 scale) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 4;
            ew->move = eft04_move;
            ew->arg = arg;
            ew->owner = em;
            ew->x07 = em->kind;
            ew->prim = 0;
            ew->scale = scale;
            ew->u0A.joint = time;
        }
    }
}
