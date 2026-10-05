/* eft05 - game.bin 0x00542D40-0x00543718; eft05_t is still assembly (see
 * eft05_nm.c), Eft05_set is in eft05b.c. A weapon's slash trail. Each
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

void eft05_move(EFTW *ew);
static void eft05_i(EFTW *ew);
static void eft05_m(EFTW *ew);
static void eft05_d(EFTW *ew);
static void eft05_e(EFTW *ew);
void eft05_t(PRIM *pr);

void eft05_move(EFTW *ew) {
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
