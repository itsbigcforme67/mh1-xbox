/* eft04 - game.bin 0x0053FFD0-0x005412A8: eft04_move to eft04_e. eft04_t is
 * still assembly (near-match in eft04_nm.c); the rest is in eft04b.c. See
 * eft04_nm.c for what the effect does. */
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
extern u8 *eft04_type5_fade_data[2];
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

void eft04_move(EFTW *ew);
void eft04_i(EFTW *ew);
void eft04_m(EFTW *ew);
void eft04_d(EFTW *ew);
void eft04_e(EFTW *ew);
void eft04_t(PRIM *pr);
void eft04_pos_calc(f32 *pos, EMW *em, f32 *ofs, int joint);
void eft04_type0_0_init(EFTW *ew, EFT04_PIECE *p);
void eft04_type3_init(EFTW *ew, EFT04_PIECE *p);
void eft04_type8_init(EFTW *ew, EFT04_PIECE *p);
void eft04_z_adj(FLMAT *m, f32 *pos);

void eft04_move(EFTW *ew) {
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

void eft04_i(EFTW *ew) {
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

void eft04_m(EFTW *ew) {
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

void eft04_d(EFTW *ew) {
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

void eft04_e(EFTW *ew) {
    push_eft_work(ew);
}
