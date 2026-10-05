/* eft15 - game.bin 0x0054BB60-0x0054D654. Sprite bursts with nine types
 * (arg): each spawns up to nine sprites (eft15_num) that start after a
 * per-sprite lag, then grow, fade and spin along keyframe tables
 * (eft15_data, through eft15_index/param). Types 4 and 5 loop x07 times
 * (eft15_loop_init) and type 5 follows a monster joint chosen by monster
 * kind and animation. Placed at a point (Eft15_set, Eft15_set2) or at a
 * monster (Eft15_set3). */
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

/* One sprite (0x2C bytes) of the work area. */
typedef struct EFT15_PIECE {
    s16 no;             /* 0x00 */
    s16 prim_no;        /* 0x02 */
    f32 pos[3];         /* 0x04 */
    f32 scale[3];       /* 0x10 from the keyframes */
    f32 size;           /* 0x1C */
    PRIM *prim;         /* 0x20 */
    s16 lag;            /* 0x24 frame counter, starts negative */
    u16 rot;            /* 0x26 */
    s16 drot;           /* 0x28 */
    u8 alpha;           /* 0x2A */
} EFT15_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft15_data[];
extern s16 eft15_num[9];
extern s16 eft15_all_time[9];
extern s16 eft15_param[9];
extern s16 eft15_index[9];
extern s16 eft15_time[9];
extern s16 *eft15_time_tbl[9];
extern s16 eft15_type0_lag[3];
extern s16 eft15_type2_lag[3];
extern s16 eft15_type4_lag[7];
extern u8 fade_type1_72[];
extern u8 fade_type6_72[];
extern u8 fade_type2_94_1[];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void get_joint_pos_em(EMW *, int, f32 *);
int em_frame_check2(EMW *, int, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void Pl_se_req2(EMW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);

static void eft15_move(EFTW *ew);
static void eft15_i(EFTW *ew);
static void eft15_m(EFTW *ew);
static void eft15_d(EFTW *ew);
static void eft15_e(EFTW *ew);
static void eft15_t(PRIM *pr);
static int eft15_loop_init(EFTW *ew, EFT15_PIECE *p);
static void eft15_se_req(EFTW *ew);

static void eft15_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft15_i(ew);
        break;
    case 1:
        eft15_m(ew);
        break;
    case 2:
        eft15_d(ew);
        break;
    case 3:
        eft15_e(ew);
        break;
    }
}

static void eft15_i(EFTW *ew) {
    s16 n;
    s16 i;
    EFT15_PIECE *w = ew->work;

    ew->mode++;
    ew->mode2 = 0;
    ew->stg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft15_num[ew->arg];
    switch (ew->arg) {
    case 4:
        eft15_se_req(ew);
        break;
    }
    for (i = 0; i < n; i++) {
        w[i].prim_no = get_prim();
        w[i].no = i;
        w[i].size = ew->scale;
        w[i].lag = 0;
        switch (ew->arg) {
        case 0:
            w[i].rot = ran_suu(1);
            w[i].lag = eft15_type0_lag[i] - 4;
            flvecCopy(w[i].pos, ew->pos);
            switch (w[i].no) {
            case 0:
                w[i].drot = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                break;
            case 2:
                w[i].drot = ((u16)ran_suu(1) & 0x7FF) - 0x400;
                break;
            default:
                w[i].drot = 0;
                break;
            }
            break;
        case 1:
        case 6:
            w[i].rot = ran_suu(1);
            w[i].lag = -5 * i - 5;
            flvecCopy(w[i].pos, ew->pos);
            w[i].size = w[i].size * (1.0f - 0.2f * (f32)i);
            w[i].drot = (ran_suu(1) & 0xFF) - 0x80;
            break;
        case 2:
            w[i].rot = ran_suu(1);
            w[i].lag = eft15_type2_lag[i] - 5;
            flvecCopy(w[i].pos, ew->pos);
            switch (w[i].no) {
            case 0:
            case 1:
                w[i].size = w[i].size * 0.75f;
                w[i].drot = 0;
                break;
            case 2:
            default:
                w[i].drot = 0;
                break;
            }
            break;
        case 3:
        case 7:
        case 8:
            w[i].lag -= 5;
            w[i].rot = ran_suu(1);
            flvecCopy(w[i].pos, ew->pos);
            if (ew->arg == 8 && w[i].no == 1) {
                w[i].size = w[i].size * 1.3f;
            }
            break;
        case 4:
        case 5:
            w[i].lag = eft15_type4_lag[i];
            switch (i) {
            case 5:
            case 6:
                w[i].rot = ran_suu(1);
                break;
            default:
                w[i].rot = 0;
                break;
            }
            flvecCopy(w[i].pos, ew->pos);
            break;
        }
        if (w[i].prim_no != -1) {
            w[i].prim = get_prim_ptr(w[i].prim_no);
            w[i].prim->owner = ew;
            w[i].prim->no = i;
            w[i].prim->trans = eft15_t;
        } else {
            w[i].prim = 0;
        }
    }
}

static void eft15_m(EFTW *ew) {
    f32 a;
    f32 v[3];
    f32 dv[3];
    s16 *tt;
    s32 n;
    s16 num;
    s16 all;
    s16 step;
    EFT15_PIECE *p = ew->work;
    s16 i;
    EMW *em = ew->owner;
    s16 time;
    s16 idx;
    void *d;
    int joint;

    num = eft15_num[ew->arg];
    all = eft15_all_time[ew->arg];
    step = eft15_param[ew->arg];
    idx = eft15_index[ew->arg];
    time = eft15_time[ew->arg];
    tt = eft15_time_tbl[ew->arg];
    switch (ew->arg) {
    case 4:
        all += (s16)(ew->x07 * 10);
        break;
    case 5:
        all += (s16)(ew->x07 * 10);
        if (em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        switch (em->kind) {
        case 0xB:
        case 1:
            if (!((em->char0 == 0x41E && em_frame_check2(em, 0, 110.0f)) ||
                (em->char0 == 0x41B && em_frame_check2(em, 0, 160.0f)))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x22;
            break;
        case 0x11:
        case 0x16:
            if (!((em->char0 == 0x41E && em_frame_check2(em, 0, 110.0f)) ||
                (em->char0 == 0x452 && em_frame_check2(em, 0, 110.0f)))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x22;
            break;
        case 0xE:
        case 0x1A:
            if (!((em->char0 == 0x457 && em_frame_check2(em, 0, 112.0f)) ||
                (em->char0 == 0x41E && em_frame_check2(em, 0, 110.0f)) ||
                (em->char0 == 0x41B && em_frame_check2(em, 0, 160.0f)))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x22;
            break;
        case 2:
            if (!(em->char0 == 0x408 && em_frame_check2(em, 0, 66.0f))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x33;
            break;
        case 0xF:
            if (!(em->char0 == 0x451 && em_frame_check2(em, 0, 66.0f))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x22;
            break;
        case 7:
            if (!(em->char0 == 0x3EF && em_frame_check2(em, 0, 4.0f))) {
                ew->mode++;
                ew->be_flag = 0;
                return;
            }
            joint = 0x25;
            break;
        }
        get_joint_pos_em(em, joint, ew->pos);
        break;
    default:
        if (ew->timer == 5) {
            eft15_se_req(ew);
        }
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    n = num;
    for (i = 0; i < n; i++, p++) {
        if (tt == 0) {
            idx = eft15_index[ew->arg];
        } else {
            time = tt[i];
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 0.0f;
        if (++p->lag <= 0) {
            idx += step;
            continue;
        }
        if (p->lag > time) {
            if (ew->arg == 4 || ew->arg == 5) {
                if (eft15_loop_init(ew, p) == 0) {
                    idx += step;
                    continue;
                }
            } else {
                idx += step;
                continue;
            }
        }
        switch (ew->arg) {
        case 0:
            d = eft15_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            d = eft15_data[idx++];
            eft_alpha_linear(p->lag, d, &a);
            p->alpha = 255.0f * a;
            switch (p->no) {
            case 0:
            case 2:
                dv[0] = 0.0f;
                dv[1] = 0.0f;
                dv[2] = 100.0f / (f32)time;
                flvecRotY(dv, DEG2RAD(ANG2DEG(ew->u0A.joint)));
                p->pos[0] += dv[0];
                p->pos[1] += dv[1];
                p->pos[2] += dv[2];
                break;
            }
            p->rot += p->drot;
            break;
        case 1:
        case 6:
            d = eft15_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            p->rot += p->drot;
            p->pos[1] += 60.0f / (f32)time;
            v[2] = 10.0f * (f32)i;
            flvecApplyMat33_2(v, &rview_mat);
            break;
        case 2:
            d = eft15_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            p->rot += p->drot;
            break;
        case 3:
        case 7:
        case 8:
            d = eft15_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            d = eft15_data[idx++];
            eft_alpha_linear(p->lag, d, &a);
            p->alpha = 255.0f * a;
            break;
        case 5:
            flvecCopy(p->pos, ew->pos);
        case 4:
            d = eft15_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
            d = eft15_data[idx++];
            eft_alpha_linear(p->lag, d, &a);
            p->alpha = 255.0f * a;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 20.0f - 5.0f * (f32)i;
            flvecApplyMat33_2(v, &rview_mat);
            break;
        }
        if (p->prim != 0) {
            p->prim->pos[0] = v[0] + p->pos[0];
            p->prim->pos[1] = v[1] + p->pos[1];
            p->prim->pos[2] = v[2] + p->pos[2];
            add_prim(ot0, p->prim, 0x40, 0);
        }
    }
}

static void eft15_d(EFTW *ew) {
    s16 n;
    s16 i;
    EFT15_PIECE *p = ew->work;

    ew->mode++;
    n = eft15_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft15_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft15_t(PRIM *pr) {
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    FLMAT uv;
    u32 col;
    EFTW *ew = pr->owner;
    EFT15_PIECE *p = &((EFT15_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;
    u16 flag = 0;
    int order;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        switch (ew->arg) {
        case 0:
            switch (p->no) {
            case 0:
                flag |= 2;
                cl = &mw->clay[54];
                flmatMakeTrans(&uv, 0.25f * (f32)((p->lag - 1) & 3), 0.25f * (f32)((p->lag - 1) >> 2), 0.0f);
                break;
            case 1:
                flag |= 2;
                cl = &mw->clay[66];
                flmatMakeTrans(&uv, 0.0f, 0.75f, 0.0f);
                break;
            case 2:
                cl = &mw->clay[94];
                flmatMakeTrans(&uv, 0.0f, 0.75f, 0.0f);
                break;
            }
            flSetRenderState(0x19, (u32)&uv);
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            order = 2;
            col = (p->alpha << 24) | 0xFFFFFF;
            break;
        case 1:
        case 6:
            cl = &mw->clay[72];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            order = 2;
            eft_rgba_linear(ew->arg == 1 ? fade_type1_72 : fade_type6_72, p->lag, &col);
            break;
        case 2:
            switch (p->no) {
            case 0:
                cl = &mw->clay[p->lag];
                flmatMakeTrans(&uv, 0.5f, 0.124f, 0.0f);
                col = -1;
                break;
            case 1:
                cl = &mw->clay[p->lag] + 27;
                flmatMakeTrans(&uv, 0.5f, 0.004f, 0.0f);
                col = -1;
                break;
            case 2:
            default:
                cl = &mw->clay[94];
                flmatMakeTrans(&uv, 0.75f, 0.75f, 0.0f);
                eft_rgba_linear(fade_type2_94_1, p->lag, &col);
                break;
            }
            flSetRenderState(0x19, (u32)&uv);
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            order = 2;
            break;
        case 3:
        case 7:
        case 8:
            if (ew->arg == 8) {
                flag |= 8;
            } else {
                flag |= 2;
            }
            if (p->no == 0) {
                cl = &mw->clay[66];
                if (ew->arg == 3 || ew->arg == 8) {
                    flmatMakeTrans(&uv, 0.25f, 0.75f, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.0f, 0.75f, 0.0f);
                }
            } else {
                cl = &mw->clay[75];
                if (ew->arg == 3 || ew->arg == 8) {
                    flmatMakeTrans(&uv, 0.0f, 0.046875f * (f32)p->lag, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.0625f, 0.5f + 0.046875f * (f32)p->lag, 0.0f);
                }
            }
            flSetRenderState(0x19, (u32)&uv);
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            order = 2;
            col = (p->alpha << 24) | 0xFFFFFF;
            break;
        case 4:
        case 5:
            switch (p->no) {
            case 0:
                if (ew->arg == 5) {
                    return;
                }
                cl = mw->clay;
                flag |= 2;
                flmatMakeTrans(&uv, 0.125f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 1:
                cl = &mw->clay[64];
                break;
            case 2:
            case 3:
            case 4:
                cl = &mw->clay[88];
                break;
            case 5:
            case 6:
                cl = &mw->clay[35];
                break;
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            order = 2;
            col = (p->alpha << 24) | 0xFFFFFF;
            break;
        }
        make_mat_srt(sc, rot, pr->pos, order, &m);
        flmatMul33_2(&m, &rview_mat);
        eft_trans_sub_col(cl, &m, col, flag, mats);
    }
}

static int eft15_loop_init(EFTW *ew, EFT15_PIECE *p) {
    if (ew->stg >= ew->x07) {
        return 0;
    }
    switch (p->no) {
    case 3:
    case 4:
        if (ew->stg == 0) {
            p->lag = -14;
        } else {
            p->lag = -6;
        }
        break;
    case 6:
        p->rot = ran_suu(1);
        p->lag = 1;
        ew->stg++;
        return 1;
    }
    return 0;
}

static void eft15_se_req(EFTW *ew) {
    switch (ew->arg) {
    case 0:
    case 2:
    case 3:
    case 7:
    case 8:
        Pl_se_req2(ew->owner, 0, 0, ew->pos, 1, 0);
        break;
    case 4:
        se_req2(1, 0x75, 0, ew->pos, 3, 0);
        break;
    }
}

void Eft15_set(f32 *pos, int arg, int ang, PLW *pl, f32 scale) {
    EFTW *ew;

    if (pl != 0 && Pl_stg_ck(pl) == 0) {
        return;
    }
    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0xF;
        ew->move = eft15_move;
        ew->arg = arg;
        flvecCopy(ew->pos, pos);
        ew->u0A.ang = ang;
        ew->owner = (EMW *)pl;
        ew->scale = scale;
    }
}

void Eft15_set2(f32 *pos, int arg, int x07, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0xF;
        ew->move = eft15_move;
        ew->arg = arg;
        flvecCopy(ew->pos, pos);
        ew->x07 = x07;
        ew->owner = 0;
        ew->scale = scale;
    }
}

void Eft15_set3(EMW *em, int arg, int x07, f32 scale) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0xF;
            ew->move = eft15_move;
            ew->arg = arg;
            ew->x07 = x07;
            ew->owner = em;
            ew->scale = scale;
        }
    }
}
