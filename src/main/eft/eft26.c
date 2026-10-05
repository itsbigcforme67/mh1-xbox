/* eft26 - SLPM_654.95 0x0027CA50-0x0027CF10. A spinning marker model that
 * floats and bobs above the player another player points at (PLW+0x3B0),
 * tinted by that player's colour (col_tbl, by +0x8ED) and raised by
 * ofs_tbl. What +0x3B0 really points to is a guess (it has a position at
 * +0xAC like PLW). */
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

/* What PLW+0x3B0 points to (only the fields used here). */
typedef struct EFT26_TGT {
    u8 _pad00[2];
    u8 x02;             /* 0x02 row of ofs_tbl when x1E is set */
    u8 _pad03[0x1E - 0x03];
    u8 x1E;             /* 0x1E */
    u8 _pad1F[0xAC - 0x1F];
    f32 pos[3];         /* 0xAC */
    u8 _padB8[0x8ED - 0xB8];
    u8 col;             /* 0x8ED row of col_tbl */
} EFT26_TGT;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern f32 ofs_tbl[4];
extern u32 col_tbl[6];

u8 Pl_stg_ck(PLW *);
void release_prim(s16);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);

static void eft26_move(EFTW *ew);
static void eft26_i(EFTW *ew);
static void eft26_m(EFTW *ew);
static void eft26_d(EFTW *ew);
static void eft26_e(EFTW *ew);
static void eft26_t(PRIM *pr);

void Eft26_set(PLW *pl) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 26;
            ew->move = eft26_move;
            ew->work14 = 0;
            ew->owner = (EMW *)pl;
            ew->x1E = pl->id;
        }
    }
}

static void eft26_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft26_i(ew);
        break;
    case 1:
        eft26_m(ew);
        break;
    case 2:
        eft26_d(ew);
        break;
    case 3:
        eft26_e(ew);
        break;
    }
}

static void eft26_i(EFTW *ew) {
    ew->mode++;
    ew->mode2 = 0;
    ew->stg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft26_t;
    } else {
        ew->prim = 0;
        eft26_e(ew);
    }
}

static void eft26_m(EFTW *ew) {
    PLW *pl = (PLW *)ew->owner;
    EFT26_TGT *tgt;
    s16 n;
    f32 y;

    if (pl->x3B0 != 0) {
        if (pl->be_flag == 0 || pl->x01 == 0 || Pl_stg_ck(pl) == 0) {
            return;
        }
        tgt = pl->x3B0;
        ew->timer++;
        if (tgt->x1E == 0) {
            n = 0;
        } else {
            n = tgt->x02;
        }
        y = 4.0f * flSin(DEG2RAD(ANG2DEG((u16)(ew->timer << 10))));
        ew->prim->pos[0] = tgt->pos[0];
        ew->prim->pos[1] = y + (tgt->pos[1] + ofs_tbl[n]);
        ew->prim->pos[2] = tgt->pos[2];
        add_prim(ot0, ew->prim, 0x40, 0);
    }
}

static void eft26_d(EFTW *ew) {
    ew->mode++;
    if (ew->prim != 0) {
        release_prim(ew->prim_no);
    }
}

static void eft26_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft26_t(PRIM *pr) {
    FLMAT m;
    EFTW *ew = pr->owner;
    PLW *pl = (PLW *)ew->owner;
    EFT26_TGT *tgt = pl->x3B0;
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;

    if (tgt != 0) {
        if (game_w.x1DC != 0) {
            if ((u8)pl->flag14 == 1) {
                return;
            }
            if (*(s8 *)0x6EAED6 != 0) {
                *(s8 *)0x6EAED6 = 0;
                return;
            }
        }
        cl = &mw->clay[134];
        mats = mw->mat;
        flmatMakeScale(&m, 7.0f, 7.0f, 7.0f);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(ew->timer << 8)));
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flSetRenderState(0x67, col_tbl[tgt->col]);
        eft_trans_sub_opa(cl, &m, mats);
    }
}
