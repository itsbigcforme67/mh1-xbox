/* eft24 - game.bin 0x00558A80-0x0055925C, part 2 (after eft24_m). In the
 * original these functions are static. Stars circling a stunned head:
 * up to five billboards spin around a joint of a player (Eft24_set) or a
 * monster (Eft24_set_em) while it stays in the stunned animation, or for a
 * set time. */
#include "eft.h"
#include "game.h"
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

/* The effect's work area (ew->work). */
typedef struct EFT24_WORK {
    s16 prim_no[5];     /* 0x00 */
    u8 _pad0A[2];
    PRIM *prim[5];      /* 0x0C */
    f32 pos[3];         /* 0x20 orbit offset */
    u16 act0;           /* 0x2C animation the owner must stay in */
    u16 act1;           /* 0x2E */
    s16 joint;          /* 0x30 */
    s16 time;           /* 0x32 */
} EFT24_WORK;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
s16 act_ck(EMW *, u16, u16);
void release_prim(s16);
FLMAT *get_joint_wmat(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
void se_req2(int, int, int, f32 *, int, int);

void eft24_t(PRIM *pr);

void eft24_d(EFTW *ew) {
    EFT24_WORK *w = ew->work;
    s16 i;

    ew->mode++;
    for (i = 0; i < 5; i++) {
        if (w->prim[i] != 0) {
            release_prim(w->prim_no[i]);
        }
    }
}

void eft24_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft24_t(PRIM *pr) {
    FLMAT m;
    EFT_MDLW *mw = eft_mdlw[0];
    EFTW *ew = pr->owner;
    void *mats;
    CLAY *cl;

    cl = &mw->clay[118];
    mats = mw->mat;
    flmatMakeScale(&m, ew->scale, ew->scale, ew->scale);
    flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
    flmatMul33_2(&m, &rview_mat);
    flSetRenderState(0x67, -1);
    eft_trans_sub_opa(cl, &m, mats);
}
