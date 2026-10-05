/* eft20_nm - NOT BUILT. Near-match C for the four eft20 functions still in
 * asm: eft20_i, eft20_m, eft20_t, eft20_pos_set (logic written from m2c
 * drafts and the asm, believed equivalent; not yet register/order-matched,
 * see docs/agents/agent-D.md). Also holds copies of eft20_d/e (they match;
 * the built copies are in eft20b.c).
 eft20b - SLPM_654.95 0x0021A950-0x0021A9E8: eft20_d / eft20_e.
 * Part of eft20 (whole file 0x00218590-0x0021D464). Monster dust and debris (30
 * types, arg): up to eft20_num[arg] pieces (0x30 bytes) per effect, placed by
 * eft20_pos_set from the monster's kind and joints, or at a player's foot
 * (Eft20_set_pl). On water the effect becomes Eft08 splashes
 * (eft20_water_ck / eft20_water_set; game.bin Eft08_set called by
 * address). The big functions (i, m, t,
 * pos_set) are still asm. */
#include "eft.h"
#include "em.h"
#include "pl.h"
#include "game.h"
#include "prim.h"
#include "fl.h"
#include "clay.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT20_PIECE {
    s8 no;              /* 0x00 */
    s8 x01;             /* 0x01 */
    s16 prim_no;        /* 0x02 */
    f32 scale[3];       /* 0x04 from the keyframes */
    f32 pos[3];         /* 0x10 */
    f32 alpha;          /* 0x1C */
    s16 time;           /* 0x20 */
    u16 rot;            /* 0x22 */
    s16 drot;           /* 0x24 */
    u8 _pad26[0x28 - 0x26];
    f32 size;           /* 0x28 */
    PRIM *prim;         /* 0x2C */
} EFT20_PIECE;

#define COL(a, r, g, b) (((u8)(a) << 24) | ((r) << 16) | ((g) << 8) | (b))

extern s16 eft20_num[30];
extern s16 eft20_water_flag[30];
extern s16 Eft_stg_type[];
extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern u8 Eft_kemuri_rgb[11][4];
extern void *fade_type29_data[2];
extern u8 fade_type25_em02[], fade_type25_em02_2[], fade_type25_74[], fade_type25_74_2[],
    fade_type25_74_3[], fade_type25_74_4[], fade_type25_74_5[], fade_type25_74_6[],
    fade_type25_74_7[], fade_type25_74_8[], fade_type25_132_1[], fade_type25_132_2[],
    fade_type25_132_3[], fade_type25_132_4[];
extern u8 fade_type26_em02_1[], fade_type26_em02_3[], fade_type26_em02_4[],
    fade_type26_74_1[], fade_type26_74_2[], fade_type26_74_3[], fade_type26_74_5[],
    fade_type26_74_6[], fade_type26_74_8[], fade_type26_74_9[], fade_type26_74_10[],
    fade_type26_132_4[], fade_type26_132_7[];

s16 Em_area_ck(int);
u32 ran_suu(int);
FLMAT *get_joint_wmat(void *, s16);
void flmatCopy(FLMAT *, FLMAT *);
u16 calc_mat_angY(FLMAT *);
void func_628690(EFTW *, int);  /* game.bin shell01_set2 */
void func_629C20(EFTW *, int);  /* game.bin shell04_set2 */
extern s16 *eft20_lag[30];
extern f32 eft20_type20_scale[4];
void eft20_se_req(EFTW *ew);
extern void *eft20_data[170];
extern s16 eft20_all_time[30];
extern s16 *eft20_time[30];
extern s16 eft20_index[30];
extern s16 eft20_param[30];
extern f32 eft20_type18_ofs_tbl[3];
extern s16 eft20_type29_order[6];
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void get_joint_pos_em(EMW *, int, f32 *);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatGetTrans(f32 *, FLMAT *);
f32 GetGroundHit(f32 *);
void func_628750(EMW *, f32 *, int);   /* game.bin shell01_set3 */
s16 eft20_water_ck(EMW *chr, f32 *pos, s16 arg);
void eft20_t(PRIM *pr);

#define RAND_ROT(k) (s16)(0.5f + 65536.0f * ((k) * (0.001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200))) / 360.0f)
void eft_rgba_linear(void *, int, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub(CLAY *, FLMAT *, u16, f32, void *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);

u8 Pl_stg_ck(void *);
u8 Em_stg_ck(void *);
void release_prim(s16);
void get_joint_pos(void *, int, f32 *);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
int GetWaterHit(f32 *, f32 *);
void se_req2(int, int, int, f32 *, int, int);
void func_544C90(f32 *, int, int, f32);    /* game.bin Eft08_set */

void eft20_move(EFTW *ew);
void eft20_i(EFTW *ew);
void eft20_m(EFTW *ew);
void eft20_d(EFTW *ew);
void eft20_e(EFTW *ew);
s16 eft20_pos_set(f32 *pos, EMW *em, s16 arg, s16 x07);
s8 eft20_water_set(void *chr, f32 *pos, s16 kind, f32 scale);

void eft20_d(EFTW *ew) {
    s16 n;
    EFT20_PIECE *p = ew->work;
    s16 i;

    ew->mode++;
    n = eft20_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

void eft20_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft20_t(PRIM *pr) {
    u32 col;
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT20_PIECE *p = &((EFT20_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    EFT_MDLW *em;
    u16 flag = 0;
    s16 t;
    s16 area;
    u8 r;
    u8 g;
    u8 b;
    f32 alpha;
    void *mats;
    CLAY *cl;
    void *fd;

    if (mw == 0 || mw->flag == 0) {
        return;
    }
    t = Eft_stg_type[game_w.stage];
    r = Eft_kemuri_rgb[t][0];
    g = Eft_kemuri_rgb[t][1];
    b = Eft_kemuri_rgb[t][2];
    mats = mw->mat;
    alpha = Eft_kemuri_rgb[t][3] / 255.0f;
    switch (ew->arg) {
    case 0:
    case 1:
    case 9:
    case 11:
    case 12:
    case 14:
    case 19:
    case 20:
    case 28:
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        switch (ew->arg) {
        case 14:
            eft_trans_sub(&mw->clay[35], &m, 0, p->alpha, mats);
            break;
        case 12:
            r = 0xFF;
            alpha = 1.0f;
            g = 0xFF;
            b = 0xFF;
        default:
        def:
            eft_trans_sub_col(&mw->clay[74], &m, COL(255.0f * alpha * p->alpha, r, g, b), 0, mats);
            break;
        case 20:
            r = 0x7F;
            alpha = 1.0f;
            g = 0x6D;
            b = 0x54;
            goto def;
        }
        break;
    case 2:
    case 3:
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        if (ew->arg == 3) {
            flmatRotY33(&m, 3.1415927f);
        }
        flmatMul33_2(&m, &rview_mat);
        if (p->no == 0) {
            eft_trans_sub_col(&mw->clay[74], &m, COL(255.0f * alpha * p->alpha, r, g, b), 0, mats);
        } else {
            eft_trans_sub_col(&mw->clay[35], &m, ((u8)(255.0f * p->alpha) << 24) | 0x777777, 0, mats);
        }
        break;
    case 4:
    case 5:
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(p->scale, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        if (p->no == 0) {
            eft_trans_sub_col(&mw->clay[74], &m, COL(255.0f * alpha * p->alpha, r, g, b), 0, mats);
        } else {
            eft_trans_sub_col(&mw->clay[35], &m, ((u8)(255.0f * p->alpha) << 24) | 0x555555, 0, mats);
        }
        break;
    case 6:
    case 8:
    case 15:
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        eft_trans_sub(&mw->clay[85], &m, 0, p->alpha, mats);
        break;
    case 7:
    case 17:
        flSetRenderState(0x6C, 0);
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        make_mat_srt(sc, rot, ew->pos, 0, &m);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(p->rot)));
        cl = &mw->clay[93];
        flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        eft_trans_sub(cl, &m, 0, p->alpha, mats);
        break;
    case 10:
    case 18:
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        if (p->no == 0) {
            eft_trans_sub_col(&mw->clay[74], &m, COL(255.0f * alpha * p->alpha, r, g, b), 0, mats);
        } else {
            eft_trans_sub(&mw->clay[35], &m, 0, p->alpha, mats);
        }
        break;
    case 16:
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        if (p->no != 0) {
            rot[1] = 0.0f;
        } else {
            rot[1] = 3.1415927f;
        }
        make_mat_srt(sc, rot, pr->pos, 4, &m);
        flmatMul33_2(&m, &rview_mat);
        eft_trans_sub(&mw->clay[36], &m, 0, p->alpha, mats);
        break;
    case 25:
        area = Em_area_ck(ew->stg);
        if (area == -1) {
            return;
        }
        em = game_w.area_mdlw[area];
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        switch (ew->stg) {
        case 1:
        case 11:
            if (em == 0 || em->flag == 0) {
                return;
            }
            if (p->no == 0) {
                fd = fade_type25_em02;
                cl = &em->clay[2];
                flmatMakeTrans(&uv, 0.25f * ew->mode2, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                flag = 2;
            } else {
                fd = fade_type25_74;
                cl = &mw->clay[74];
            }
            break;
        case 20:
            cl = &mw->clay[74];
            fd = p->no == 0 ? fade_type25_74_2 : fade_type25_74_3;
            break;
        case 6:
            if (em == 0 || em->flag == 0) {
                return;
            }
            if (p->no == 0) {
                fd = fade_type25_em02_2;
                cl = &em->clay[2];
            } else {
                fd = fade_type25_74_4;
                cl = &mw->clay[74];
            }
            break;
        case 15:
            flag = 2;
            cl = &mw->clay[132];
            fd = p->no == 0 ? fade_type25_132_1 : fade_type25_132_2;
            break;
        case 17:
            flag = 2;
            cl = &mw->clay[132];
            fd = p->no == 0 ? fade_type25_132_3 : fade_type25_132_4;
            break;
        case 22:
            cl = &mw->clay[74];
            fd = p->no == 0 ? fade_type25_74_5 : fade_type25_74_6;
            break;
        default:
            cl = &mw->clay[74];
            fd = p->no == 0 ? fade_type25_74_7 : fade_type25_74_8;
            break;
        }
        eft_rgba_linear(fd, p->time, &col);
        eft_trans_sub_col(cl, &m, col, flag, mats);
        break;
    case 26:
        area = Em_area_ck(ew->stg);
        if (area == -1) {
            return;
        }
        em = game_w.area_mdlw[area];
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        switch (ew->stg) {
        case 1:
        case 11:
            if (em == 0 || em->flag == 0) {
                return;
            }
            if (p->no == 0) {
                flag = 2;
                fd = fade_type26_em02_1;
                cl = &em->clay[2];
                flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
            } else {
                cl = &mw->clay[74];
                fd = fade_type26_74_1;
            }
            break;
        case 20:
            cl = &mw->clay[74];
            fd = fade_type26_74_2;
            break;
        case 6:
            if (em == 0 || em->flag == 0) {
                return;
            }
            if (p->no == 0) {
                fd = fade_type26_em02_3;
                cl = &em->clay[2];
            } else {
                cl = &mw->clay[74];
                fd = fade_type26_74_3;
            }
            break;
        case 15:
            flag = 2;
            cl = &mw->clay[132];
            fd = fade_type26_132_4;
            break;
        case 21:
            cl = &mw->clay[74];
            fd = fade_type26_74_5;
            break;
        case 8:
        case 34:
            cl = &mw->clay[74];
            fd = fade_type26_74_6;
            break;
        case 17:
            flag = 2;
            cl = &mw->clay[132];
            fd = fade_type26_132_7;
            break;
        case 22:
            if (ew->mode2 != 0) {
                flag = 2;
                cl = &mw->clay[132];
                fd = fade_type26_132_7;
            } else {
                cl = &mw->clay[74];
                fd = fade_type26_74_8;
            }
            break;
        case 2:
            if (em == 0 || em->flag == 0) {
                return;
            }
            if (p->no == 0) {
                flag = 2;
                fd = fade_type26_em02_4;
                cl = &em->clay[2];
                flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
            } else {
                cl = &mw->clay[74];
                fd = fade_type26_74_10;
            }
            break;
        default:
            cl = &mw->clay[74];
            fd = fade_type26_74_9;
            break;
        }
        /* NOTE: uv is only set on the em02 paths; the original passes it anyway. */
        flSetRenderState(0x19, (u32)&uv);
        eft_rgba_linear(fd, p->time, &col);
        eft_trans_sub_col(cl, &m, col, flag, mats);
        break;
    case 29:
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[74];
        eft_rgba_linear(fade_type29_data[ew->x07], p->time, &col);
        eft_trans_sub_col(cl, &m, col, 0, mats);
        break;
    default:
        break;
    }
    flSetRenderState(0x6C, 1);
}

void eft20_i(EFTW *ew) {
    FLMAT m;
    EFT20_PIECE *p = ew->work;
    EMW *em = ew->owner;
    s16 *lag;
    s16 n;
    s16 shell;
    s16 i;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    lag = eft20_lag[ew->arg];
    n = eft20_num[ew->arg];
    if (ew->x07 & 0x80) {
        shell = 0;
        ew->x07 ^= 0x80;
    } else {
        shell = 1;
    }
    if (em != 0) {
        ew->u0A.joint = em->ang[1];
        if (ew->stg != 1) {
            switch (ew->arg) {
            case 0:
            case 1:
            case 9:
            case 11:
            case 14:
            case 18:
            case 19:
                if (em->kind != 2) {
                    if (shell != 0) {
                        switch (ew->arg) {
                        case 1:
                            func_628690(ew, 0x21);
                            break;
                        case 9:
                            func_628690(ew, 0x27);
                            break;
                        case 11:
                            func_628690(ew, 0x26);
                            break;
                        }
                    }
                } else if (shell != 0 && ew->arg == 1) {
                    func_629C20(ew, 0x15);
                }
                break;
            case 7:
                ew->scale *= 0.7f + (0.2f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
                break;
            case 10:
                if (shell != 0) {
                    func_628690(ew, 0x20);
                }
                break;
            case 25:
                ew->mode2 = (u16)ran_suu(1) & 1;
                ew->stg = em->kind;
                if (ew->stg == 2) {
                    flmatCopy(&m, get_joint_wmat(em, 0x33));
                } else {
                    flmatCopy(&m, get_joint_wmat(em, 0x24));
                }
                ew->u0A.joint = calc_mat_angY(&m) + 0x4000;
                break;
            case 26:
                ew->stg = em->kind;
                if (ew->stg == 2) {
                    flmatCopy(&m, get_joint_wmat(em, 0x33));
                } else {
                    flmatCopy(&m, get_joint_wmat(em, 0x24));
                    if (ew->stg == 0x16) {
                        if (em->mode == 3 && em->x15 == 4 && em->char0 == 0x456) {
                            ew->mode2 = 1;
                        } else {
                            ew->mode2 = 0;
                        }
                    }
                }
                ew->u0A.joint = calc_mat_angY(&m) + 0x4000;
                break;
            }
        }
    }
    for (i = 0; i < n; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            if (lag != 0) {
                p->time = lag[i];
            } else {
                p->time = 0;
            }
            p->size = ew->scale;
            p->x01 = 0;
            switch (ew->arg) {
            case 0:
                p->no = i;
                p->rot = ran_suu(1);
                p->drot = ((u16)ran_suu(1) & 0x1F) - 0xA;
                p->size = ew->scale;
                break;
            case 1:
                p->no = i;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.15f);
                p->size = 10.0f * ew->scale;
                break;
            case 2:
            case 3:
                p->rot = ran_suu(1);
                if (i < 2) {
                    p->no = 0;
                    p->drot = RAND_ROT(0.4f);
                } else {
                    p->no = 1;
                    if (ew->stg == 1) {
                        p->size *= 0.4f;
                    }
                    p->drot = RAND_ROT(0.1f);
                }
                break;
            case 4:
                p->rot = ran_suu(1);
                if (i == 0) {
                    p->no = 0;
                    p->drot = RAND_ROT(0.3f);
                } else {
                    p->no = 1;
                    p->drot = RAND_ROT(0.1f);
                }
                break;
            case 5:
                p->rot = ran_suu(1);
                if (i < 5) {
                    p->no = 0;
                    p->drot = RAND_ROT(0.4f);
                } else {
                    p->no = 1;
                    p->drot = RAND_ROT(0.1f);
                }
                break;
            case 6:
                p->no = (u16)ran_suu(1) & 1;
                p->size = 0.8f + (0.2f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
                p->rot = ran_suu(1);
                break;
            case 7:
            case 17:
                p->no = 0;
                p->rot = ran_suu(1);
                break;
            case 8:
                p->no = i;
                p->rot = ran_suu(1);
                break;
            case 9:
                p->no = i;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.1f);
                p->size = 0.8f + (0.2f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
                break;
            case 10:
            case 18:
                if (ew->arg == 0x12) {
                    p->time = -3 * i;
                    p->no = 0;
                } else {
                    p->time = 0;
                    p->no = i;
                }
                p->rot = ran_suu(1);
                if (i == 0) {
                    p->drot = RAND_ROT(0.1f);
                } else {
                    p->drot = RAND_ROT(0.03f);
                }
                p->size = ew->scale * (0.8f + (0.4f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF));
                break;
            case 11:
                p->no = i;
                p->rot = ran_suu(1);
                p->size = ew->scale;
                p->drot = RAND_ROT(0.15f);
                break;
            case 12:
                p->rot = 0;
                p->drot = 0;
                break;
            case 19:
                p->no = i;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.1f);
                break;
            case 14:
            case 15:
                p->no = i;
                p->rot = ran_suu(1);
                break;
            case 16:
                p->time = i * -2;
                p->no = (u16)ran_suu(1) & 1;
                p->rot = 0;
                p->size = 0.8f + (0.3f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
                break;
            case 20:
                p->no = i;
                p->rot = ran_suu(1);
                if (i < 2) {
                    p->drot = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                } else {
                    p->drot = (ran_suu(1) & 0xFF) - 0x80;
                }
                p->size = ew->scale * (0.8f * eft20_type20_scale[i]);
                break;
            case 25:
                p->no = i;
                p->time = -i;
                p->x01 = 1;
                p->alpha = 1.0f;
                p->rot = ran_suu(1);
                p->drot = RAND_ROT(0.2f);
                p->size = ew->scale * (0.8f + (0.7f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF));
                flvecCopy(p->pos, ew->owner->pos);
                break;
            case 26:
                p->no = (u16)ran_suu(1) & 1;
                p->x01 = 1;
                p->alpha = 1.0f;
                p->rot = ran_suu(1);
                p->drot = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                p->size = ew->scale * (1.0f + (0.8f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF));
                break;
            case 28:
                p->rot = ran_suu(1);
                p->size = 1.2f;
                p->drot = (s16)(0.5f + 65536.0f * (0.001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200)) / 360.0f);
                break;
            case 29:
                p->time = i * -4;
                p->rot = ran_suu(1);
                p->drot = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                p->no = i;
                p->x01 = 1;
                p->size = ew->scale * (1.5f + 0.0005f * ((u16)ran_suu(1) & 0x3FF));
                break;
            }
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft20_t;
        } else {
            p->prim = 0;
        }
    }
    eft20_se_req(ew);
    if (ew->arg == 0x16 || ew->arg == 0x19) {
        eft20_m(ew);
    }
}

void eft20_m(EFTW *ew) {
    FLMAT m;
    f32 v[3];
    f32 w[3];
    EFT20_PIECE *p = ew->work;
    s16 n = eft20_num[ew->arg];
    u16 all = eft20_all_time[ew->arg];
    s16 *tt = eft20_time[ew->arg];
    s16 step = eft20_param[ew->arg];
    s16 idx = eft20_index[ew->arg];
    u16 time;
    s16 k;
    s16 i;
    int ang;

    switch (ew->arg) {
    case 7:
    case 12:
    case 17:
    case 26:
    case 28:
        time = all;
        break;
    case 0:
        time = 0x41;
        break;
    case 1:
        time = 0x24;
        break;
    case 11:
        time = 0x42;
        break;
    case 16:
        time = 0x18;
        break;
    case 18:
        time = 0x26;
        break;
    case 29:
        time = 0x13;
        break;
    case 25:
        k = (u8)ew->x07 >> 1;
        idx += (s16)(k * 4);
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    if (ew->arg == 0x10) {
        if (ew->owner->be_flag == 0 || ew->owner->x04 == 3) {
            for (i = 0; i < n; i++) {
                p[i].no = -1;
            }
        }
    } else if (ew->arg == 0x19) {
        if (ew->owner->be_flag == 0 || ew->owner->x04 == 3) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        if (k != 3) {
            flmatCopy(&m, get_joint_wmat(ew->owner, 0x24));
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 80.0f;
            flvecApplyMat33_2(v, &m);
            ew->pos[0] = m[3][0] + v[0];
            ew->pos[2] = m[3][2] + v[2];
            ew->u0A.joint = calc_mat_angY(&m) + 0x4000;
        }
    }
    ang = 0;
    for (i = 0; i < n; i++, p++, ang += 0x3333) {
        switch (ew->arg) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 14:
        case 15:
        case 19:
        case 20:
        case 25:
            time = tt[i];
            break;
        case 0:
        case 1:
        case 7:
        case 11:
        case 16:
        case 17:
        case 18:
        case 28:
        case 29:
            idx = eft20_index[ew->arg];
            break;
        }
        if (++p->time <= 0) {
            idx += step;
            continue;
        }
        if (p->time > time) {
            idx += step;
            continue;
        }
        if (ew->arg == 0x10 && p->time == 1) {
            if (p->no == -1) {
                idx += step;
                continue;
            }
            get_joint_pos_em(ew->owner, 0x24, p->pos);
            p->pos[0] += 0.05f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
            p->pos[1] += 0.05f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
            p->pos[2] += 0.05f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
        }
        eft_vec_linear(p->time, eft20_data[idx++], p->scale);
        switch (ew->arg) {
        case 0:
        case 1:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            v[0] *= ew->scale;
            v[1] *= ew->scale;
            v[2] *= ew->scale;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint + ang)));
            p->rot += p->drot;
            break;
        case 4:
        case 5:
        case 9:
        case 10:
        case 12:
        case 18:
        case 19:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            if (ew->arg == 1) {
                v[0] *= ew->scale;
                v[1] *= ew->scale;
                v[2] *= ew->scale;
            } else if (ew->arg == 0x12) {
                v[0] += eft20_type18_ofs_tbl[i];
                if (ew->x07 == 1) {
                    v[0] = -v[0];
                }
                v[2] += 100.0f;
            }
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            p->rot += p->drot;
            break;
        case 2:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            v[0] += 50.0f;
            v[0] *= ew->scale;
            v[1] *= ew->scale;
            v[2] *= ew->scale;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            p->rot += p->drot;
            break;
        case 3:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            v[0] += 50.0f;
            v[0] = -v[0];
            v[0] *= ew->scale;
            v[1] *= ew->scale;
            v[2] *= ew->scale;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            p->rot += p->drot;
            break;
        case 6:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            if (ew->x07 != 0) {
                v[0] = -v[0];
            }
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            break;
        case 7:
        case 17:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = -1000.0f;
            flvecApplyMat33_2(v, &rview_mat);
            break;
        case 8:
        case 14:
        case 15:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            break;
        case 11:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint + ang)));
            p->rot += p->drot;
            break;
        case 16:
            p->pos[1] -= 10.25f;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            break;
        case 20:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            w[0] = 0.0f;
            w[1] = 0.0f;
            w[2] = i * 10;
            flvecApplyMat33_2(w, &rview_mat);
            v[0] += w[0];
            v[1] += w[1];
            v[2] += w[2];
            p->rot += p->drot;
            break;
        case 25:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            if ((ew->x07 & 1) == 1) {
                v[0] = -v[0];
            }
            v[1] += ew->owner->pos[1] - p->pos[1];
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            p->rot += p->drot;
            break;
        case 26:
            v[0] = p->time * (50.0f / time);
            if (ew->x07 == 0) {
                v[0] = -v[0];
            }
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            p->rot += p->drot;
            break;
        case 28:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            if (ew->x07 == 1) {
                v[0] = -v[0];
            }
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint + (i - 1) * 0x2AAB)));
            p->rot += p->drot;
            break;
        case 29:
            eft_vec_linear(p->time, eft20_data[idx++], v);
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint + eft20_type29_order[i] * 0x2AAA)));
            p->rot += p->drot;
            break;
        }
        if (p->x01 == 0) {
            eft_alpha_linear(p->time, eft20_data[idx++], &p->alpha);
        }
        if (p->prim != 0) {
            if (ew->arg == 0x10) {
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
            } else {
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
            }
            add_prim(ot0, p->prim, 0x40, 0);
        }
    }
}

/* Rotate an offset by the monster's facing and add it to pos. */
#define ADD_ROT(pos, em, x, y, z)                         \
    do {                                                   \
        v[0] = (x);                                        \
        v[1] = (y);                                        \
        v[2] = (z);                                        \
        flvecRotY(v, DEG2RAD(ANG2DEG((em)->ang[1])));      \
        (pos)[0] += v[0];                                  \
        (pos)[1] += v[1];                                  \
        (pos)[2] += v[2];                                  \
    } while (0)

s16 eft20_pos_set(f32 *pos, EMW *em, s16 arg, s16 x07) {
    FLMAT m;
    f32 v[3];
    s16 shell = 0;
    int go;
    s16 k;

    if (x07 & 0x80) {
        go = 0;
        x07 ^= 0x80;
    } else {
        go = 1;
    }
    switch (arg) {
    case 0:
    case 1:
    case 9:
    case 11:
    case 14:
    case 18:
    case 19:
        if (em->kind != 2) {
            if (x07 != 9) {
                pos[0] = em->pos[0];
                pos[1] = em->x5AC;
                pos[2] = em->pos[2];
            } else {
                get_joint_pos_em(em, 4, pos);
                pos[1] = em->x5AC;
            }
            if (go != 0 && em->kind != 2) {
                switch (arg) {
                case 1:
                    shell = 0x21;
                    break;
                case 9:
                    shell = 0x27;
                    break;
                case 11:
                    shell = 0x26;
                    break;
                }
            }
        } else {
            if (x07 != 0) {
                get_joint_pos_em(em, 0x13, pos);
            } else {
                get_joint_pos_em(em, 0x1F, pos);
            }
            pos[1] = em->x5AC;
        }
        break;
    case 2:
        switch (x07) {
        case 4:
            get_joint_pos_em(em, 0xA, pos);
            pos[1] = em->x5AC;
            ADD_ROT(pos, em, 0.0f, 0.0f, 20.0f);
            break;
        case 5:
            flvecCopy(pos, em->pos);
            ADD_ROT(pos, em, -85.0f, 0.0f, 0.0f);
            break;
        case 6:
            get_joint_pos_em(em, 2, pos);
            pos[1] = em->x5AC;
            break;
        case 7:
            flmatCopy(&m, get_joint_wmat_em(em, 0x2C));
            flmatGetTrans(pos, &m);
            v[0] = 100.0f;
            v[1] = 0.0f;
            v[2] = -255.0f;
            flvecApplyMat33_2(v, &m);
            pos[0] += v[0];
            pos[1] += v[1];
            pos[2] += v[2];
            pos[1] = em->x5AC;
            break;
        case 8:
            get_joint_pos_em(em, 0x1C, pos);
            pos[1] = em->x5AC;
            break;
        default:
            get_joint_pos_em(em, 0x1A, pos);
            pos[1] = em->x5AC;
            if (x07 == 2) {
                ADD_ROT(pos, em, 0.0f, 0.0f, -50.0f);
            }
            break;
        }
        break;
    case 3:
        switch (x07) {
        case 4:
            get_joint_pos_em(em, 0xA, pos);
            pos[1] = em->x5AC;
            ADD_ROT(pos, em, 0.0f, 0.0f, 20.0f);
            break;
        case 5:
            flvecCopy(pos, em->pos);
            ADD_ROT(pos, em, 85.0f, 0.0f, 0.0f);
            break;
        case 6:
            get_joint_pos_em(em, 2, pos);
            pos[1] = em->x5AC;
            break;
        case 7:
            flmatCopy(&m, get_joint_wmat_em(em, 0x2C));
            flmatGetTrans(pos, &m);
            v[0] = 100.0f;
            v[1] = 0.0f;
            v[2] = -255.0f;
            flvecApplyMat33_2(v, &m);
            pos[0] += v[0];
            pos[1] += v[1];
            pos[2] += v[2];
            pos[1] = em->x5AC;
            break;
        default:
            get_joint_pos_em(em, 0x14, pos);
            pos[1] = em->x5AC;
            break;
        }
        break;
    case 4:
        if (x07 == 0) {
            get_joint_pos_em(em, 0x15, pos);
        } else {
            get_joint_pos_em(em, 0x1B, pos);
        }
        pos[1] = em->x5AC;
        break;
    case 5:
        get_joint_pos_em(em, 0x1B, pos);
        pos[1] = em->x5AC;
        break;
    case 6:
    case 15:
        get_joint_pos_em(em, 0x23, pos);
        ADD_ROT(pos, em, 0.0f, 0.0f, 60.0f);
        pos[1] = GetGroundHit(pos);
        break;
    case 8:
        get_joint_pos_em(em, 0x23, pos);
        ADD_ROT(pos, em, 0.0f, 0.0f, 30.0f);
        pos[1] = GetGroundHit(pos);
        break;
    case 7:
        get_joint_pos_em(em, 0x23, pos);
        v[1] = 0.0f;
        v[0] = 0.1f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
        v[2] = 80.0f + 0.1f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
        flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
        pos[0] += v[0];
        pos[2] += v[2];
        pos[1] = GetGroundHit(pos);
        pos[1] += -5.0f;
        break;
    case 10:
        get_joint_pos_em(em, 0x29, pos);
        pos[1] = GetGroundHit(pos);
        if (go != 0) {
            shell = 0x20;
        }
        break;
    case 25:
        if (em->kind == 2) {
            flmatCopy(&m, get_joint_wmat_em(em, 0x33));
        } else {
            flmatCopy(&m, get_joint_wmat_em(em, 0x24));
        }
        flmatGetTrans(pos, &m);
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 80.0f * em->scale[2];
        flvecApplyMat33_2(v, &m);
        pos[0] += v[0];
        pos[1] += v[1];
        pos[2] += v[2];
        break;
    case 26:
        if (em->kind == 2) {
            flmatCopy(&m, get_joint_wmat_em(em, 0x33));
            v[0] = 0.0f;
            v[1] = -23.7f;
            v[2] = 151.9f;
        } else {
            flmatCopy(&m, get_joint_wmat_em(em, 0x24));
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 50.0f * em->scale[2];
        }
        flmatGetTrans(pos, &m);
        flvecApplyMat33_2(v, &m);
        pos[0] += v[0];
        pos[1] += v[1];
        pos[2] += v[2];
        break;
    case 28:
        flmatCopy(&m, get_joint_wmat_em(em, 2));
        v[0] = x07 == 0 ? -65.0f : 65.0f;
        v[1] = 0.0f;
        v[2] = 0.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
        pos[0] = m[3][0] + v[0];
        pos[1] = em->x5AC;
        pos[2] = m[3][2] + v[2];
        break;
    case 29:
        pos[0] = em->pos[0];
        pos[1] = em->x5AC;
        pos[2] = em->pos[2];
        break;
    }
    k = eft20_water_ck(em, pos, arg);
    if (k != 0 && shell != 0) {
        func_628750(em, pos, shell);
    }
    return k;
}
