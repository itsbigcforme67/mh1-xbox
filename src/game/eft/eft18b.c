/* eft18 - game.bin 0x00552D00-0x005546D8: eft18_t00 to eft18_se_req.
 * See eft18.c. */
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
FLMAT *get_joint_wmat(PLW *, int);
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

void eft18_t00(PRIM *pr) {
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    u32 col;
    EFTW *ew = pr->owner;
    EFT18_WORK *wk = ew->work;
    EFT18_PIECE *p = &wk->piece[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    s16 cl_no;
    void *mats;
    void **fade;
    u16 flag = 0;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        switch (ew->arg) {
        case 0:
        case 10:
            flag |= 2;
            flSetRenderState(0x6C, 0);
            switch (p->no) {
            case 0:
                rot[0] = 0.0f;
                rot[1] = 0.0f;
                rot[2] = 0.0f;
                cl_no = 16;
                make_mat_srt(p->scale, rot, pr->pos, 0, &m);
                flmatMul33_2(&m, &rview_mat);
                break;
            case 1:
                if (p->lag == 4) {
                    cl_no = 18;
                } else {
                    cl_no = 17;
                }
                flmatCopy(&m, &wk->mat);
                eft18_z_adj(&m, ew->pos);
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
                break;
            }
            col = -1;
            break;
        case 1:
        case 11:
            flSetRenderState(0x6C, 0);
            if (p->no == 0) {
                flag |= 2;
                cl_no = 78;
                flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv / 4), 0.0f);
                flSetRenderState(0x19, (u32)&uv);
            } else {
                cl_no = 20;
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(p->scale, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            flSetRenderState(0x67, ((u8)(255.0f * p->alpha) << 24) | 0xFFFFFF);
            col = ((u8)(255.0f * p->alpha) << 24) | 0xFFFFFF;
            break;
        case 2:
            cl_no = 20;
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(p->scale, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            col = ((u8)(255.0f * p->alpha) << 24) | 0xFFFFFF;
            break;
        case 5:
            flag |= 2;
            flSetRenderState(0x6C, 0);
            sc[0] = 3.0f * p->scale[0];
            sc[1] = 3.0f * p->scale[1];
            sc[2] = 3.0f * p->scale[2];
            fade = eft18_type5_fade_data[p->no];
            if (fade == 0) {
                col = -1;
            } else {
                eft_rgba_linear(fade[ew->x07], p->lag, &col);
            }
            switch (p->no) {
            case 0:
            case 1:
                cl_no = 121;
                flmatCopy(&m, &wk->mat);
                eft18_z_adj(&m, ew->pos);
                m[0][0] *= sc[0];
                m[0][1] *= sc[0];
                m[0][2] *= sc[0];
                m[1][0] *= sc[1];
                m[1][1] *= sc[1];
                m[1][2] *= sc[1];
                m[2][0] *= sc[2];
                m[2][1] *= sc[2];
                m[2][2] *= sc[2];
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                break;
            case 2:
                cl_no = 66;
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                flmatMul33_2(&m, &rview_mat);
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 3:
                cl_no = 64;
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                flmatMul33_2(&m, &rview_mat);
                break;
            case 4:
                cl_no = 35;
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                flmatMul33_2(&m, &rview_mat);
                break;
            }
            switch (ew->x07) {
            case 0:
            case 5:
            case 6:
            case 7:
                flag |= 2;
                break;
            }
            break;
        case 6:
            sc[0] = 3.0f * p->scale[0];
            sc[1] = 3.0f * p->scale[1];
            sc[2] = 3.0f * p->scale[2];
            cl_no = 72;
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            eft_rgba_linear(fade74_data[ew->x07], p->lag, &col);
            switch (ew->x07) {
            case 0:
            case 5:
            case 6:
            case 7:
                flag |= 2;
                break;
            }
            break;
        case 7:
        case 9:
            if (p->no == 0) {
                flag |= 2;
                cl_no = 64;
                col = 0xFFFFFF7F;
                sc[0] = 4.0f * p->scale[0];
                sc[1] = 4.0f * p->scale[1];
                sc[2] = 4.0f * p->scale[2];
            } else {
                cl_no = 72;
                eft_rgba_linear(fade_type7_data[ew->x07], p->lag, &col);
                sc[0] = 2.0f * (1.0f - 0.1f * (f32)(p->no - 1)) * p->scale[0];
                sc[1] = 2.0f * (1.0f - 0.1f * (f32)(p->no - 1)) * p->scale[1];
                sc[2] = 2.0f * (1.0f - 0.1f * (f32)(p->no - 1)) * p->scale[2];
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 8:
            flag |= 2;
            switch (p->no) {
            case 0:
                cl_no = 64;
                break;
            case 1:
                cl_no = 88;
                break;
            case 2:
                cl_no = 122;
                break;
            }
            eft_rgba_linear(fade_type8_data[p->no], p->lag, &col);
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(p->scale, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        }
        eft_trans_sub_col(&mw->clay[cl_no], &m, col, flag, mats);
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}

void eft18_i01(EFTW *ew) {
    ew->u0A.ang = ran_suu(1);
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft18_t;
        flvecCopy(ew->prim->pos, ew->pos);
    } else {
        push_eft_work(ew);
    }
}

void eft18_m01(EFTW *ew) {
    if (++ew->timer > eft18_all_time[ew->arg]) {
        ew->mode++;
        ew->be_flag = 0;
    } else {
        add_prim(ot0, ew->prim, 0x40, 0);
    }
}

void eft18_t01(PRIM *pr) {
    f32 a;
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    EFTW *ew = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        eft_vec_linear(ew->timer, eft18_data[17], sc);
        eft_alpha_linear(ew->timer, eft18_data[18], &a);
        rot[2] = DEG2RAD(ANG2DEG(ew->u0A.joint));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[88];
        SetTrnslMode(4, 1);
        eft_trans_sub_col(cl, &m, ((u8)(255.0f * a) << 24) | 0xFFFFFF, 2, mats);
        SetTrnslMode(4, 5);
    }
}

void eft18_i02(EFTW *ew) {
    f32 w[3];
    f32 v[3];
    EFT18_ROCK *r = ew->work;
    s16 n = eft18_num[ew->arg];
    s16 i;
    u16 ax;
    u16 ay;

    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = ew->scale;
    ax = ew->mode2 << 8;
    ay = ew->stg << 8;
    flvecRotX(v, DEG2RAD(ANG2DEG(ax)));
    flvecRotY(v, DEG2RAD(ANG2DEG(ay)));
    for (i = 0; i < n; i++, r++) {
        r->prim_no = get_prim();
        if (r->prim_no != -1) {
            r->alive = 1;
            r->x01 = 1;
            r->vel[0] = v[0];
            r->vel[1] = v[1];
            r->vel[2] = v[2];
            r->rx = ax;
            r->ry = ay;
            if (i == 0) {
                r->rz = 0;
            } else {
                r->rz = 0x8000;
            }
            flvecCopy(r->pos, ew->pos);
            w[0] = 0.0f;
            w[1] = 3.0f;
            w[2] = 10.0f;
            flvecRotY(w, DEG2RAD(ANG2DEG(r->ry + (i ? -0x4000 : 0x4000))));
            r->vel[0] += w[0];
            r->vel[1] += w[1];
            r->vel[2] += w[2];
            r->prim = get_prim_ptr(r->prim_no);
            r->prim->owner = ew;
            r->prim->no = i;
            r->prim->trans = eft18_t;
        } else {
            r->prim = 0;
        }
    }
    eft18_m02(ew);
}

void eft18_m02(EFTW *ew) {
    EFT18_ROCK *r = ew->work;
    s16 n = eft18_num[ew->arg];
    s16 i;
    u8 any = 0;

    if (++ew->timer > eft18_all_time[ew->arg]) {
        ew->mode++;
        ew->be_flag = 0;
    }
    for (i = 0; i < n; i++) {
        if (r->alive == 0) {
            r++;
            continue;
        }
        any = 1;
        r->pos[0] += r->vel[0];
        r->pos[1] += r->vel[1];
        r->pos[2] += r->vel[2];
        r->rx -= 0x180;
        if (i == 0) {
            r->ry += 0x200;
        } else {
            r->ry -= 0x200;
        }
        r->vel[1] += -2.5f;
        r->vel[0] *= 0.9f;
        r->vel[1] *= 0.9f;
        r->vel[2] *= 0.9f;
        if (r->pos[1] <= GetGroundHit(r->pos)) {
            r->alive = 0;
        }
        if (r->prim != 0) {
            flvecCopy(r->prim->pos, r->pos);
            add_prim(ot1, r->prim, 0x20, 0);
        }
        r++;
    }
    if (any == 0) {
        ew->mode++;
        ew->be_flag = 0;
    }
}

void eft18_t02(PRIM *pr) {
    FLMAT m;
    EFT18_ROCK *r = &((EFT18_ROCK *)((EFTW *)pr->owner)->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    CLAY *cl;
    void *mats;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        cl = &mw->clay[89];
        flmatInit(&m);
        flmatRotX33(&m, DEG2RAD(ANG2DEG(r->rx)));
        flmatRotY33(&m, DEG2RAD(ANG2DEG(r->ry)));
        RotateZ(&m, DEG2RAD(ANG2DEG(r->rz)));
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        eft_trans_sub_opa(cl, &m, mats);
    }
}

void eft18_d(EFTW *ew) {
    EFT18_PIECE *p = ((EFT18_WORK *)ew->work)->piece;
    EFT18_ROCK *r = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    n = eft18_num[ew->arg];
    if (n == 0) {
        release_prim(ew->prim_no);
    } else {
        for (i = 0; i < n; i++) {
            if (ew->arg == 4) {
                if (r->prim != 0) {
                    release_prim(r->prim_no);
                }
                r++;
            } else {
                if (p->prim != 0) {
                    release_prim(p->prim_no);
                }
                p++;
            }
        }
    }
}

void eft18_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft18_z_adj(FLMAT *m, f32 *pos) {
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

void eft18_se_req(EFTW *ew, f32 *pos) {
    switch (ew->arg) {
    case 7:
        switch (ew->x07) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            se_req2(1, 0x26, 0, pos, 1, 0);
            break;
        }
        break;
    }
}
