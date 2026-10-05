/* NONMATCHING: eft24_m (0x00558E10), not built. The original keeps a
 * divide-by-zero trap on `0x10000 / n` although n was just checked for zero;
 * our compiler proves n non-zero and drops it, which also reschedules the
 * loop body (53 instructions differ, all from that). Same compiler quirk as
 * set03_m / set22_m. The rest matches, built from eft24.c and eft24b.c. */
/* eft24 - game.bin 0x00558A80-0x0055925C. Stars circling a stunned head:
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

static void eft24_move(EFTW *ew);
static void eft24_i(EFTW *ew);
static void eft24_m(EFTW *ew);
static void eft24_d(EFTW *ew);
static void eft24_e(EFTW *ew);
static void eft24_t(PRIM *pr);

void Eft24_set(PLW *pl, int arg) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 24;
            ew->move = eft24_move;
            ew->work14 = 0;
            ew->owner = (EMW *)pl;
            ew->x1E = pl->id;
            ew->arg = arg;
            ew->pos[0] = 0.0f;
            ew->pos[1] = 30.0f;
            ew->pos[2] = 25.0f;
            ew->scale = 1.0f;
            ew->u0A.joint = 20;
        }
    }
}

void Eft24_set_em(EMW *em, int arg, int joint, int time, f32 y, f32 scale) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 24;
            ew->move = eft24_move;
            ew->work14 = 0;
            ew->owner = em;
            ew->x1E = em->id;
            ew->arg = arg;
            ew->timer = time;
            ew->pos[1] = y;
            ew->pos[0] = 0.0f;
            ew->pos[2] = 25.0f;
            ew->scale = scale;
            ew->u0A.joint = joint;
        }
    }
}

static void eft24_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft24_i(ew);
        break;
    case 1:
        eft24_m(ew);
        break;
    case 2:
        eft24_d(ew);
        break;
    case 3:
        eft24_e(ew);
        break;
    }
}

static void eft24_i(EFTW *ew) {
    EFT24_WORK *w = ew->work;
    s16 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->stg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    switch (ew->arg) {
    case 0:
        w->act0 = 2;
        w->act1 = 0x13;
        break;
    case 1:
        w->time = ew->timer;
        w->act0 = ew->owner->mode;
        w->act1 = ew->owner->x15;
        break;
    case 2:
        w->time = ew->timer;
        w->act0 = ew->owner->mode;
        w->act1 = ew->owner->x15;
        break;
    }
    w->joint = ew->u0A.joint;
    flvecCopy(w->pos, ew->pos);
    w->pos[2] *= ew->scale;
    ew->timer = 0;
    ew->u0A.joint = ran_suu(1);
    for (i = 0; i < 5; i++) {
        w->prim_no[i] = get_prim();
        if (w->prim_no[i] != -1) {
            w->prim[i] = get_prim_ptr(w->prim_no[i]);
            w->prim[i]->owner = ew;
            w->prim[i]->no = i;
            w->prim[i]->trans = eft24_t;
        } else {
            w->prim[i] = 0;
        }
    }
}

static void eft24_m(EFTW *ew) {
    FLMAT m;
    EFT24_WORK *w = ew->work;
    EMW *em = ew->owner;
    s16 i;
    s16 n;

    if (ew->arg == 2) {
        if (em->be_flag == 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        if (em->mode == 5) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
    } else if (em->be_flag == 0 || act_ck(em, w->act0, w->act1) == 0) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    ew->timer++;
    switch (ew->arg) {
    case 0:
        n = em->x07;
        if (ew->timer % 23 == 1) {
            se_req2(1, 0x38, 0, em->pos, 1, 0);
        }
        break;
    case 1:
    case 2:
        if (ew->timer > w->time) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        n = 5;
        break;
    }
    if (n == 0) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    if (n > 5) {
        n = 5;
    }
    flmatCopy(&m, get_joint_wmat(em, w->joint));
    flmatGetTrans(ew->pos, &m);
    ew->u0A.joint += 0x800;
    for (i = 0; i < n; i++) {
        if (w->prim[i] != 0) {
            flvecCopy(w->prim[i]->pos, w->pos);
            flvecRotY(w->prim[i]->pos, DEG2RAD(ANG2DEG(ew->u0A.joint + 0x10000 / n * i)));
            flvecApplyMat33_2(w->prim[i]->pos, &m);
            w->prim[i]->pos[0] += ew->pos[0];
            w->prim[i]->pos[1] += ew->pos[1];
            w->prim[i]->pos[2] += ew->pos[2];
            add_prim(ot1, w->prim[i], 0x20, 0);
        }
    }
}

static void eft24_d(EFTW *ew) {
    EFT24_WORK *w = ew->work;
    s16 i;

    ew->mode++;
    for (i = 0; i < 5; i++) {
        if (w->prim[i] != 0) {
            release_prim(w->prim_no[i]);
        }
    }
}

static void eft24_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft24_t(PRIM *pr) {
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
