/* NONMATCHING: eft05_t (0x00543200), not built. Control flow and stack
 * layout match; the spline point arithmetic is scheduled differently
 * (75 instructions differ). Tried every order of the three spline terms
 * and of each multiply's operands. The rest matches, built from eft05.c
 * and eft05b.c. */
/* eft05 - game.bin 0x00542D40-0x00543718. A weapon's slash trail. Each
 * frame the weapon tip's matrix (joint 14 or 18, plus an offset per arg) is
 * pushed into a six-entry history while the player stays in the attack
 * animation, and eft05_t draws a skinned ribbon through it. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT05_WORK {
    u8 _pad00[0x10];
    FLMAT hist[6];      /* 0x10 newest first */
} EFT05_WORK;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x10];
    u8 *skin;           /* 0x24 skin matrix list (count-ish at +0xC2) */
    u8 _pad28[8];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern f32 eft05_ofs[2][3];
extern EFT_MDLW *eft_mdlw[5];
extern f32 mat_calc[6][2];
extern f32 sprine_calc[6][3];
extern u8 Eft_blood_rgb[];

u8 Pl_stg_ck(PLW *);
void release_prim(s16);
FLMAT *get_joint_wmat(PLW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatBlend(FLMAT *, FLMAT *, FLMAT *, f32, f32);
void flSetSkinTrans(void *);
void SetFilterMode(int);
void Material_set_sub(void *, CLAY *);
void Eft_rendope_set(int);

static void eft05_move(EFTW *ew);
static void eft05_i(EFTW *ew);
static void eft05_m(EFTW *ew);
static void eft05_d(EFTW *ew);
static void eft05_e(EFTW *ew);
static void eft05_t(PRIM *pr);

static void eft05_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft05_i(ew);
        break;
    case 1:
        eft05_m(ew);
        break;
    case 2:
        eft05_d(ew);
        break;
    case 3:
        eft05_e(ew);
        break;
    }
}

static void eft05_i(EFTW *ew) {
    f32 v[3];
    FLMAT m;
    EFT05_WORK *w = ew->work;
    PLW *pl = (PLW *)ew->owner;
    s32 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft05_t;
    } else {
        ew->prim = 0;
    }
    v[0] = eft05_ofs[ew->arg][0];
    v[1] = eft05_ofs[ew->arg][1];
    v[2] = eft05_ofs[ew->arg][2];
    if (pl->work615 != 0) {
        flmatCopy(&m, get_joint_wmat(pl, 0xE));
    } else {
        flmatCopy(&m, get_joint_wmat(pl, 0x12));
    }
    flvecApplyMat33_2(v, &m);
    m[3][0] += v[0];
    m[3][1] += v[1];
    m[3][2] += v[2];
    for (i = 0; i < 6; i++) {
        flmatCopy(&w->hist[i], &m);
    }
    eft05_m(ew);
}

static void eft05_m(EFTW *ew) {
    f32 v[3];
    FLMAT m;
    EFT05_WORK *w = ew->work;
    PLW *pl = (PLW *)ew->owner;
    s32 i;

    if (pl->char0 != ew->u0A.ang || --ew->timer <= 0) {
        ew->be_flag = 0;
        ew->mode++;
    }
    for (i = 5; i > 0; i--) {
        flmatCopy(&w->hist[i], &w->hist[i - 1]);
    }
    v[0] = eft05_ofs[ew->arg][0];
    v[1] = eft05_ofs[ew->arg][1];
    v[2] = eft05_ofs[ew->arg][2];
    if (pl->work615 != 0) {
        flmatCopy(&m, get_joint_wmat(pl, 0xE));
    } else {
        flmatCopy(&m, get_joint_wmat(pl, 0x12));
    }
    flvecApplyMat33_2(v, &m);
    m[3][0] += v[0];
    m[3][1] += v[1];
    m[3][2] += v[2];
    flmatCopy(&w->hist[0], &m);
    flmatGetTrans(ew->pos, &w->hist[0]);
    if (ew->prim != 0 && ew->owner->x01 != 0) {
        ew->prim->pos[0] = ew->pos[0];
        ew->prim->pos[1] = ew->pos[1];
        ew->prim->pos[2] = ew->pos[2];
        add_prim(ot0, ew->prim, 0x40, 0);
    }
}

static void eft05_d(EFTW *ew) {
    ew->mode++;
    if (ew->prim != 0) {
        release_prim(ew->prim_no);
    }
}

static void eft05_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft05_t(PRIM *pr) {
    f32 v[3];
    FLMAT m;
    FLMAT a;
    FLMAT b;
    FLMAT c;
    EFTW *ew = pr->owner;
    EFT05_WORK *w;
    EFT_MDLW *mw = eft_mdlw[4];
    u16 flag = 0;
    PLW *pl;
    u8 *skin;
    s32 n;
    s32 j, k;
    f32 *mc;
    f32 *sc;
    CLAY *cl;
    void *mats;
    u8 r, g, bl;

    w = ew->work;
    pl = (PLW *)ew->owner;
    if (mw == 0 || pl->x01 == 0) {
        return;
    }
    if (mw->flag == 0) {
        return;
    }
    skin = mw->skin;
    n = *(s16 *)(skin + 0xC2);
    v[0] = eft05_ofs[ew->arg][0];
    v[1] = eft05_ofs[ew->arg][1];
    v[2] = eft05_ofs[ew->arg][2];
    if (pl->work615 != 0) {
        flmatCopy(&m, get_joint_wmat(pl, 0xE));
    } else {
        flmatCopy(&m, get_joint_wmat(pl, 0x12));
    }
    flvecApplyMat33_2(v, &m);
    m[3][0] += v[0];
    m[3][1] += v[1];
    m[3][2] += v[2];
    flmatCopy((FLMAT *)skin, &m);
    flmatCopy((FLMAT *)(skin + 0x190), &m);
    skin += 0x320;
    n -= 2;
    for (j = 0; j < 4; j++) {
        flmatCopy(&a, &w->hist[j]);
        flmatCopy(&b, &w->hist[j + 1]);
        flmatCopy(&c, &w->hist[j + 2]);
        for (k = 0, mc = mat_calc[0], sc = sprine_calc[0]; k < 6; k++, mc += 2, sc += 3) {
            flmatBlend(&m, &a, &b, mc[0], mc[1]);
            flmatSetTrans(&m,
                          (a[3][0] + b[3][0]) / 2.0f * sc[0] + sc[2] * (b[3][0] + c[3][0]) / 2.0f + b[3][0] * sc[1],
                          (a[3][1] + b[3][1]) / 2.0f * sc[0] + sc[2] * (b[3][1] + c[3][1]) / 2.0f + b[3][1] * sc[1],
                          (a[3][2] + b[3][2]) / 2.0f * sc[0] + sc[2] * (b[3][2] + c[3][2]) / 2.0f + b[3][2] * sc[1]);
            flmatCopy((FLMAT *)skin, &m);
            if (--n <= 0) {
                goto done;
            }
            skin += 0x190;
        }
    }
done:
    flSetSkinTrans(mw->skin);
    SetFilterMode(1);
    cl = &mw->clay[ew->arg];
    mats = mw->mat;
    if (ew->arg == 0) {
        r = Eft_blood_rgb[0];
        g = Eft_blood_rgb[1];
        bl = Eft_blood_rgb[2];
    } else {
        r = 0xFF;
        g = r;
        bl = r;
        flag |= 2;
    }
    flSetRenderState(0x6C, 0);
    flSetRenderState(0x67, (r << 16 | 0xFF000000) | (g << 8) | bl);
    if (cl != 0 && cl->handle != -1) {
        Material_set_sub(mats, cl);
        clay_attr_set(cl->attr);
        Eft_rendope_set(flag);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
    flSetRenderState(0x6C, 1);
}

void Eft05_set(PLW *pl, int timer, int arg) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(2);
        if (ew != 0) {
            ew->type = 5;
            ew->move = eft05_move;
            ew->work14 = 0;
            ew->owner = (EMW *)pl;
            ew->x1E = pl->id;
            ew->timer = timer;
            ew->u0A.ang = pl->char0;
            ew->arg = arg;
        }
    }
}
