/* eft14 - game.bin 0x0054AE50-0x0054BB50: eft14_m01 to Eft14_set4, after
 * eft14_m00 (still assembly). See eft14.c. */
#include "eft.h"
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

/* Flash effect handed to the renderer with push_senko/pull_senko. */
typedef struct SENKO {
    f32 pos[3];         /* 0x00 */
    f32 size;           /* 0x0C */
    u8 x10;             /* 0x10 */
    u8 _pad11[4];
    u8 x15;             /* 0x15 */
} SENKO;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT14_PIECE {
    s16 uv;             /* 0x00 animation frame */
    s16 lag;            /* 0x02 frame counter, starts negative */
    s16 prim_no;        /* 0x04 */
    s16 no;             /* 0x06 sprite kind, 0xFF = unused */
    u16 rot;            /* 0x08 */
    s16 drot;           /* 0x0A */
    f32 scale[3];       /* 0x0C from the keyframes */
    f32 pos[3];         /* 0x18 */
    f32 alpha;          /* 0x24 */
    f32 size;           /* 0x28 */
    f32 dy;             /* 0x2C */
    PRIM *prim;         /* 0x30 */
} EFT14_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft14_data[26];
extern s16 eft14_num[11];
extern s16 eft14_type4_lag[7];
extern f32 eft14_type3_scale[5][2];
extern s16 type1_uv78[15];
extern s16 uv78_00646630[];
extern s16 flash_flag;
extern s16 flash_timer;

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub(CLAY *, FLMAT *, u16, f32, void *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetTrnslMode(int, int);
void push_senko(SENKO *);
void pull_senko(SENKO *);
void set12_set(int, int, int, f32 *, s16);

void eft14_move(EFTW *ew);
void eft14_i(EFTW *ew);
void eft14_type3_init_sub(EFT14_PIECE *p);
void eft14_type6_init_sub(EFT14_PIECE *p);
void eft14_i00(EFTW *ew);
void eft14_i01(EFTW *ew);
void eft14_m(EFTW *ew);
void eft14_m00(EFTW *ew);
void eft14_m01(EFTW *ew);
void eft14_d(EFTW *ew);
void eft14_e(EFTW *ew);
void eft14_t(PRIM *pr);
void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl);

void eft14_m01(EFTW *ew) {
    ew->timer += (s16)(((u16)ran_suu(1) & 1) + 1);
    if (ew->timer > 16) {
        ew->timer = 1;
    }
    add_prim(ot0, ew->prim, 0x40, 0);
}

void eft14_d(EFTW *ew) {
    EFT14_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    ew->be_flag = 0;
    if (ew->arg == 5) {
        pull_senko((SENKO *)(p + 7));
    }
    n = eft14_num[ew->arg];
    if (n == 0) {
        release_prim(ew->prim_no);
    } else {
        for (i = 0; i < n; i++, p++) {
            if (p->prim != 0) {
                release_prim(p->prim_no);
            }
        }
    }
}

void eft14_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft14_t(PRIM *pr) {
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT14_PIECE *p = &((EFT14_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    s16 cl_no;
    u16 flag = 0;
    void *mats;
    f32 alpha;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        alpha = p->alpha;
        switch (ew->arg) {
        case 0:
            flSetRenderState(0x6C, 0);
            flag |= 2;
            sc[0] = ew->scale * p->scale[0];
            sc[1] = ew->scale * p->scale[1];
            sc[2] = ew->scale * p->scale[2];
            switch (p->no) {
            case 0:
            case 1:
            case 2:
                cl_no = 78;
                flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv / 4), 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 3:
                cl_no = 80;
                break;
            case 4:
                cl_no = 88;
                break;
            }
            break;
        case 1:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            cl_no = 78;
            flag |= 2;
            flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv >> 2), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 2:
            cl_no = 21;
            sc[0] = ew->scale;
            sc[1] = ew->scale;
            sc[2] = ew->scale;
            flSetRenderState(0x6C, 0);
            flag |= 2;
            alpha = 0.05f + 0.025f * (1.0f + flSin(DEG2RAD(360.0f * ((f32)ew->timer / 16.0f) - 90.0f)));
            break;
        case 3:
        case 6:
            cl_no = 96;
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            flSetRenderState(0x6C, 0);
            flag |= 2;
            flmatMakeTrans(&uv, 0.0625f * (f32)(p->uv & 7), 0.0625f * (f32)(p->uv >> 3), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 4:
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            switch (p->no) {
            case 0:
            case 1:
                flag |= 2;
                cl_no = 21;
                break;
            case 2:
                cl_no = 35;
                break;
            case 3:
                cl_no = 72;
                sc[0] *= 2.0f;
                sc[1] *= 2.0f;
                sc[2] *= 2.0f;
                break;
            }
            break;
        case 5:
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            cl_no = pr->no + 81;
            if (p->no < 2) {
                if (flash_flag != 1 && flash_flag != 2) {
                    flag |= 2;
                }
            }
            break;
        case 8:
            flSetRenderState(0x6C, 0);
            sc[0] = p->scale[0] * (ew->scale * p->size);
            sc[1] = p->scale[1] * (ew->scale * p->size);
            sc[2] = p->scale[2] * (ew->scale * p->size);
            if ((p->no & 1) == 0) {
                cl_no = 77;
                flmatMakeTrans(&uv, 0.0f, 0.25f, 0.0f);
            } else {
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                cl_no = 79;
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 9:
            sc[0] = 3.0f * ew->scale * p->scale[0];
            sc[1] = 3.0f * ew->scale * p->scale[1];
            sc[2] = 3.0f * ew->scale * p->scale[2];
            flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            flag |= 2;
            cl_no = 34;
            break;
        }
        flSetRenderState(0x60, 0);
        make_mat_srt(sc, rot, pr->pos, 0, &m);
        if (ew->arg == 10) {
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        } else if (ew->arg <= 1U || ew->arg == 4 || ew->arg == 8 || ew->arg == 9) {
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(p->rot)));
        }
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[cl_no];
        if (ew->arg == 3 || ew->arg == 6) {
            eft_trans_sub_col(cl, &m, ((u8)(255.0f * alpha) << 24) | 0xFF5F00, flag, mats);
        } else {
            if (ew->arg == 5 && (flash_flag == 1 || flash_flag == 2)) {
                flag |= 1;
            }
            eft_trans_sub(cl, &m, flag, alpha, mats);
        }
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}

void eft14_set(f32 *pos, s16 arg, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0xE;
        ew->move = eft14_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = scale;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        if (arg < 5) {
            ew->u0A.ang = ran_suu(1);
        }
        if (arg == 0) {
            eft14_set(pos, 8, scale);
            eft14_set(pos, 9, scale);
        }
    }
}

void Eft14_set2(f32 *pos, s16 arg) {
    EFTW *ew;

    if (arg == 2) {
        ew = pull_eft_work(0);
    } else {
        ew = pull_eft_work(1);
    }
    if (ew != 0) {
        ew->type = 0xE;
        ew->move = eft14_move;
        ew->arg = arg;
        ew->owner = 0;
        if (arg == 2) {
            ew->scale = 2.0f;
        } else {
            ew->scale = 1.0f;
        }
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
    }
}

void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        if ((ew = pull_eft_work(1)) != 0) {
            ew->type = 0xE;
            ew->move = eft14_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            if (arg < 5) {
                ew->u0A.ang = ran_suu(1);
            }
        }
    }
}

void Eft14_set4(PLW *pl, int arg) {
    f32 v[3];
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        if ((ew = pull_eft_work(1)) != 0) {
            ew->type = 0xE;
            ew->move = eft14_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->scale = 1.0f;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 128.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
            ew->pos[0] = pl->pos[0] + v[0];
            ew->pos[1] = pl->x5AC + v[1];
            ew->pos[2] = pl->pos[2] + v[2];
        }
    }
}
