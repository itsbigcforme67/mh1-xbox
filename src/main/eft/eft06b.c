/* eft06b - SLPM_654.95 0x00104D30-0x00105B10: eft06_d/e, the draw
 * callback eft06_t, sound and spawners. See eft06.c. Whole file 0x00102BD0-0x00105B10. Hit sparks on players and
 * monsters with ten types (arg): each spawns up to eft06_num[arg] sprites
 * (0x38-byte pieces in ew->work) placed on a joint of the owner (ew->stg),
 * with per-type start values (eft06_i) and keyframe animation (eft06_m,
 * eft06_t, still asm). Type 7 uses the second prim pool. Spawned by
 * Eft06_set (on a joint), Eft06_set2 (at a point) and Eft06_set_hit (on a
 * hit monster, sized by enemy_shadow/mahi tables of game.bin). Names of the
 * types are not known. */
#include "eft.h"
#include "em.h"
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

/* One sprite (0x38 bytes) of the work area. */
typedef struct EFT06_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 sprite kind, 0xFF = unused */
    f32 scale[3];       /* 0x04 */
    f32 pos[3];         /* 0x10 */
    u32 col;            /* 0x1C */
    u16 rot[3];         /* 0x20 */
    s16 drot;           /* 0x26 */
    f32 size;           /* 0x28 */
    PRIM *prim;         /* 0x2C */
    s16 lag;            /* 0x30 frame counter, starts at or below 0 */
    u16 x32;            /* 0x32 */
    u16 x34;            /* 0x34 */
    u16 x36;            /* 0x36 */
} EFT06_PIECE;

extern s16 eft06_num[10];
extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft06_fade_data[60];
extern u8 fade_type7_61_4[];

void flmatRotX33(FLMAT *, f32);
void eft_rgba_linear(void *, int, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
extern s16 eft06_type3_lag_tbl[6];
extern s16 eft06_type4_lag_tbl[3];
extern f32 eft06_em_scale[35];
extern f32 D_63BD60[];      /* game.bin enemy_mahi_size */

u32 ran_suu(int);
u8 Pl_stg_ck(void *);
int Pl_master_ck(void *);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
FLMAT *get_joint_wmat(void *, int);
void get_joint_pos(void *, int, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void se_req2(int, int, int, f32 *, int, int);
void Pl_se_req2(void *, int, int, f32 *, int, int);

void eft06_move(EFTW *ew);
void eft06_i(EFTW *ew);
void eft06_m(EFTW *ew);
void eft06_d(EFTW *ew);
void eft06_e(EFTW *ew);
void eft06_t(PRIM *pr);
void eft06_se_req(EFTW *ew, f32 *pos);
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);

void eft06_d(EFTW *ew) {
    EFT06_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    n = eft06_num[ew->arg];
    if (n == 0) {
        if (ew->prim != 0) {
            if (ew->prim2 != 0) {
                release_prim2(ew->prim_no);
            } else {
                release_prim(ew->prim_no);
            }
        }
    } else {
        for (i = 0; i < n; i++, p++) {
            if (p->prim != 0) {
                release_prim(p->prim_no);
            }
        }
    }
}

void eft06_e(EFTW *ew) {
    push_eft_work(ew);
}


void eft06_t(PRIM *pr) {
    u32 col;
    u32 a;
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    EFT06_PIECE *p = &((EFT06_PIECE *)ew->work)[pr->no];
    void *mats;
    CLAY *cl;
    u16 flag = 0;
    void *fd;
    void *fa;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        if (ew->arg == 7) {
            switch (ew->mode2) {
            default:
                return;
            case 1:
                fd = eft06_fade_data[54];
                break;
            case 2:
                fd = eft06_fade_data[55];
                break;
            case 3:
                fd = eft06_fade_data[56];
                break;
            }
            fa = fade_type7_61_4;
            flSetRenderState(0x6C, 0);
            if (p->no == 0) {
                cl = &mw->clay[61];
                eft_rgba_linear(fd, p->lag, &col);
            } else {
                cl = &mw->clay[143];
                col = -1;
            }
            eft_rgba_linear(fa, p->x32, &a);
            col &= 0xFFFFFF;
            col |= a & 0xFF000000;
            sc[0] = p->size;
            sc[1] = p->size * p->x32 / 60.0f;
            sc[2] = p->size;
            rot[1] = p->rot[1];
            make_mat_srt(sc, rot, pr->pos, 4, &m);
            eft_trans_sub_col(cl, &m, col, 2, mats);
            flSetRenderState(0x6C, 1);
            return;
        }
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot[2]));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        switch (ew->arg) {
        case 0:
            flmatMul33_2(&m, &rview_mat);
            flSetRenderState(0x6C, 0);
            flag |= 2;
            switch (p->no) {
            case 0:
                cl = &mw->clay[84];
                break;
            case 1:
                cl = &mw->clay[37];
                flmatMakeTrans(&uv, 0.0f, 0.125f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 2:
                cl = &mw->clay[88];
                break;
            }
            break;
        case 1:
            rot[0] = DEG2RAD(ANG2DEG(p->rot[0]));
            rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
            flmatRotX33(&m, rot[0]);
            flmatRotY33(&m, rot[1]);
            flSetRenderState(0x6C, 0);
            flag |= 2;
            cl = &mw->clay[38];
            break;
        case 2:
            flmatMul33_2(&m, &rview_mat);
            flag |= 2;
            cl = &mw->clay[37];
            flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 3:
            flmatMul33_2(&m, &rview_mat);
            flag |= 2;
            flSetRenderState(0x6C, 0);
            if (p->no == 0) {
                cl = &mw->clay[74];
            } else {
                cl = mw->clay + p->no + 44;
            }
            break;
        case 4:
        case 8:
            flmatMul33_2(&m, &rview_mat);
            flag |= 2;
            flSetRenderState(0x6C, 0);
            if (p->no == 0) {
                cl = &mw->clay[39];
            } else if (p->no == 1) {
                cl = &mw->clay[48];
            } else {
                cl = mw->clay + p->no + 43;
            }
            break;
        case 5:
        case 9:
            rot[0] = DEG2RAD(ANG2DEG(p->rot[0]));
            rot[1] = DEG2RAD(ANG2DEG(p->rot[1]));
            flmatRotX33(&m, rot[0]);
            flmatRotY33(&m, rot[1]);
            cl = &mw->clay[49];
            flmatMakeTrans(&uv, 0.0f, 0.03125f * p->lag, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 6:
            flmatMul33_2(&m, &rview_mat);
            flSetRenderState(0x6C, 0);
            flag |= 2;
            switch (p->no) {
            case 0:
                cl = &mw->clay[66];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 1:
            case 2:
                cl = &mw->clay[64];
                break;
            default:
                return;
            }
            break;
        }
        if (ew->arg == 5) {
            flSetRenderState(0x67, -1);
            eft_trans_sub_opa(cl, &m, mats);
        } else {
            eft_trans_sub_col(cl, &m, p->col, flag, mats);
        }
        flSetRenderState(0x6C, 1);
    }
}

void eft06_se_req(EFTW *ew, f32 *pos) {
    switch (ew->arg) {
    case 0:
        se_req2(1, 0x39, 0, pos, 1, 0);
        break;
    case 3:
        se_req2(1, 0x23, 0, pos, 1, 0);
        break;
    case 5:
    case 9:
        se_req2(1, 0x22, 0, pos, 1, 0);
        break;
    case 6:
        switch (ew->x07) {
        case 0:
            Pl_se_req2(ew->owner, 0xC, 0, ew->pos, 1, 0);
            break;
        case 1:
            Pl_se_req2(ew->owner, 0xD, 0, ew->pos, 1, 0);
            break;
        case 2:
            Pl_se_req2(ew->owner, 0xE, 0, ew->pos, 1, 0);
            break;
        }
        break;
    }
}

EFTW *eft06_set_com(void *chr, s16 arg, int x05) {
    EFTW *ew;

    if (arg != 7 && Pl_stg_ck(chr) == 0) {
        return 0;
    }
    ew = pull_eft_work(1);
    if (ew == 0) {
        return 0;
    }
    ew->type = 6;
    ew->move = eft06_move;
    ew->owner = chr;
    ew->arg = arg;
    ew->mode2 = x05;
    return ew;
}

void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale) {
    EFTW *ew;

    if (arg == 6 && Pl_master_ck(chr) == 0) {
        return;
    }
    if ((ew = eft06_set_com(chr, arg, x05)) != 0) {
        ew->scale = scale;
        get_joint_pos(chr, joint, ew->pos);
        ew->stg = joint;
        if (arg == 0) {
            Eft06_set(chr, 1, x05, joint, scale);
        } else if (arg == 7) {
            ew->prim2 = 1;
        }
    }
}

void Eft06_set2(f32 scale, void *chr, s16 arg, int joint, f32 *pos) {
    EFTW *ew = eft06_set_com(chr, arg, 0);

    if (ew != 0) {
        ew->scale = scale;
        ew->stg = joint;
        flvecCopy(ew->pos, pos);
    }
}

void Eft06_set_hit(EMW *em, s16 arg, int x05) {
    EFTW *ew;

    if ((ew = eft06_set_com(em, arg, x05)) != 0) {
        ew->stg = 2;
        get_joint_pos(em, ew->stg, ew->pos);
        switch (ew->arg) {
        case 0:
            if (em->x10 == 0) {
                ew->scale = 4.0f;
            } else {
                ew->scale = em->scale[0] * eft06_em_scale[em->kind];
            }
            Eft06_set(em, 1, x05, ew->stg, ew->scale);
            break;
        case 9:
            if (em->x10 == 0) {
                ew->scale = 0.75f;
            } else {
                ew->scale = em->scale[0] * D_63BD60[em->kind];
            }
            break;
        default:
            ew->scale = 1.0f;
            break;
        }
    }
}
