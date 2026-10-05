/* eft01 - SLPM_654.95 0x00101E40-0x00102BC8. Shadows under players and
 * monsters (arg = kind):
 *   0, 3  the owner's skinned shadow model, flattened (3 = per-bone ground
 *         snap through SetSkinTransKKK)
 *   1     two blob shadows under the owner's feet (+0x124/+0x130 bones)
 *   2     five small blobs under joints from type02_tbl, each in its own prim
 *   4     a monster blob, sized by enemy_shadow_size (or the lobby's copy)
 * The owner's fields are only named by offset (EFT01_CHR): it is a PLW or
 * EMW depending on the kind. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table (0x4C bytes per material) */
    u8 _pad14[0x10];
    void *skin;         /* 0x24 skin handle (flCalcTrans/flSetSkinTrans) */
    u8 _pad28[4];
    s16 clay_num;       /* 0x2C */
    u8 _pad2E[2];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One material of a model set (0x4C bytes). */
typedef struct EFT_MAT {
    u8 _pad00[0x10];
    f32 alpha;          /* 0x10 */
    u8 _pad14[0x4C - 0x14];
} EFT_MAT;

/* A skeleton node walked by SetSkinTransKKK. */
typedef struct SKIN_NODE {
    FLMAT mat;          /* 0x00 world matrix */
    u8 _pad40[0x40];
    FLMAT local;        /* 0x80 */
    u8 _padC0[4];
    s16 no;             /* 0xC4 render-state slot - 0x1A */
    u8 _padC6[6];
    struct SKIN_NODE *next;     /* 0xCC sibling */
    struct SKIN_NODE *child;    /* 0xD0 */
} SKIN_NODE;

typedef struct EFT01_BONE {
    u8 _pad00[0x40];
    FLMAT mat;          /* 0x40 */
} EFT01_BONE;

/* The owner (PLW or EMW), fields used here. */
typedef struct EFT01_CHR {
    u8 be_flag;         /* 0x000 */
    u8 x01;             /* 0x001 */
    u8 kind;            /* 0x002 monster kind (row of enemy_shadow_size) */
    u8 _pad003[0x0C - 0x03];
    u16 id;             /* 0x00C */
    u8 _pad00E[2];
    u8 x10;             /* 0x010 non-zero: monster */
    u8 _pad011[0xA4 - 0x11];
    u16 ang;            /* 0x0A4 facing */
    u8 _pad0A6[0xB8 - 0xA6];
    f32 scale[3];       /* 0x0B8 */
    u8 _pad0C4[0x124 - 0xC4];
    EFT01_BONE *foot0;  /* 0x124 */
    u8 _pad128[0x130 - 0x128];
    EFT01_BONE *foot1;  /* 0x130 */
    u8 _pad134[0x138 - 0x134];
    PL_HAND *x138;      /* 0x138 position at +0x70 */
    u8 _pad13C[0x5AC - 0x13C];
    f32 x5AC;           /* 0x5AC ground height */
    u8 _pad5B0[0x736 - 0x5B0];
    u8 stg;             /* 0x736 */
    u8 _pad737[0x763 - 0x737];
    u8 x763;            /* 0x763 */
    u8 _pad764[0x798 - 0x764];
    f32 alpha;          /* 0x798 shadow alpha */
    u8 _pad79C[0x7D6 - 0x79C];
    u8 x7D6;            /* 0x7D6 non-zero: no shadow */
} EFT01_CHR;

/* Work area for kind 2. */
typedef struct EFT01_WORK {
    PRIM *prim[5];      /* 0x00 */
    s16 prim_no[5];     /* 0x14 */
} EFT01_WORK;

typedef f32 SHADOW_SIZE[2];

typedef struct CW {
    u8 _pad0[0x2BFE];
    s8 x2BFE[4];        /* per player */
} CW;

extern CW *cw;
extern EFT_MDLW *eft_mdlw[5];
extern s16 type02_tbl[5];
extern f32 type02_scl[5];
extern f32 D_610300[][2];   /* lobby.bin enemy_shadow_size_lb */
extern f32 D_63BC40[][2];   /* game.bin enemy_shadow_size */

u8 Pl_stg_ck(EFT01_CHR *);
int Pl_master_ck(EFT01_CHR *);
s16 Em_area_ck(int);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
void get_joint_pos_em(EFT01_CHR *, int, f32 *);
FLMAT *get_joint_wmat(EFT01_CHR *, int);
f32 GetGroundHit(f32 *);
void SetFilterMode(int);
f32 flConvertStoR(u16);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatMul(FLMAT *, FLMAT *, FLMAT *);
void flCalcTrans(void *, FLMAT *);
void flSetSkinTrans(void *);

static void eft01_move(EFTW *ew);
static void eft01_i(EFTW *ew);
static void eft01_m(EFTW *ew);
static void eft01_d(EFTW *ew);
static void eft01_e(EFTW *ew);
static void eft01_t(PRIM *pr);

static void eft01_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft01_i(ew);
        break;
    case 1:
        eft01_m(ew);
        break;
    case 2:
        eft01_d(ew);
        break;
    case 3:
        eft01_e(ew);
        break;
    }
}

static void eft01_i(EFTW *ew) {
    EFT01_WORK *w;
    s16 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    switch (ew->arg) {
    case 2:
        w = ew->work;
        for (i = 0; i < 5; i++) {
            w->prim_no[i] = get_prim();
            if (w->prim_no[i] != -1) {
                w->prim[i] = get_prim_ptr(w->prim_no[i]);
                w->prim[i]->owner = ew;
                w->prim[i]->no = i;
                w->prim[i]->trans = eft01_t;
            } else {
                w->prim[i] = 0;
            }
            ew->prim = 0;
        }
        break;
    default:
        if (ew->prim2 != 0) {
            ew->prim_no = get_prim2();
        } else {
            ew->prim_no = get_prim();
        }
        if (ew->prim_no != -1) {
            if (ew->prim2 != 0) {
                ew->prim = get_prim_ptr2(ew->prim_no);
            } else {
                ew->prim = get_prim_ptr(ew->prim_no);
            }
            ew->prim->owner = ew;
            ew->prim->trans = eft01_t;
        } else {
            eft01_e(ew);
            return;
        }
        break;
    }
    eft01_m(ew);
}

static void eft01_m(EFTW *ew) {
    EFT01_WORK *w;
    s16 i;
    EFT01_CHR *chr = (EFT01_CHR *)ew->owner;
    f32 g;

    switch (ew->arg) {
    case 1:
        if (game_w.x1DC != 0) {
            if (chr->be_flag == 0 || cw->x2BFE[chr->id] == 0) {
                return;
            }
        } else if (chr->be_flag == 0) {
            return;
        }
        ew->pos[0] = chr->x138->pos[0];
        ew->pos[1] = chr->x5AC;
        ew->pos[2] = chr->x138->pos[2];
        break;
    case 2:
        w = ew->work;
        if (chr->be_flag == 0) {
            ew->mode++;
            return;
        }
        if (chr->x01 == 0 || Pl_stg_ck(chr) == 0) {
            return;
        }
        for (i = 0; i < 5; i++) {
            if (w->prim[i] != 0) {
                flmatGetTrans(w->prim[i]->pos, get_joint_wmat(chr, type02_tbl[i]));
                g = GetGroundHit(w->prim[i]->pos);
                if (w->prim[i]->pos[1] >= 8.0f + g) {
                    w->prim[i]->pos[1] = 5.0f + g;
                    add_prim(ot0, w->prim[i], 0x40, 0);
                }
            }
        }
        return;
    default:
        if (chr->be_flag == 0) {
            ew->mode++;
            return;
        }
        get_joint_pos_em(chr, 2, ew->pos);
        ew->pos[1] = chr->x5AC;
        break;
    }
    if (chr->x01 == 0 || Pl_stg_ck(chr) == 0) {
        return;
    }
    ew->prim->pos[0] = ew->pos[0];
    ew->prim->pos[1] = ew->pos[1];
    ew->prim->pos[2] = ew->pos[2];
    add_prim(ot0, ew->prim, 0x40, 0);
}

static void eft01_d(EFTW *ew) {
    EFT01_WORK *w;
    s16 i;

    ew->mode++;
    switch (ew->arg) {
    case 2:
        w = ew->work;
        for (i = 0; i < 5; i++) {
            if (w->prim[i] != 0) {
                release_prim(w->prim_no[i]);
            }
        }
        break;
    default:
        if (ew->prim != 0) {
            if (ew->prim2 != 0) {
                release_prim2(ew->prim_no);
            } else {
                release_prim(ew->prim_no);
            }
        }
        break;
    }
}

static void eft01_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft01_material_set(EFT01_CHR *chr, EFT_MAT *mats, CLAY *cl) {
    int i;
    EFT_MAT *mt;

    for (i = 0; i < cl->mat_num; i++) {
        mt = &mats[cl->mat_no[i]];
        mt->alpha = chr->alpha;
        flSetRenderState((u8)(i + 0x3A), (u32)mt);
    }
}

void SetSkinTransKKK(SKIN_NODE *node, u32 h) {
    FLMAT r;
    FLMAT m;
    f32 v[3];
    f32 g;

    flmatCopy(&m, &node->mat);
    v[0] = m[3][0];
    v[1] = m[3][1];
    v[2] = m[3][2];
    g = GetGroundHit(v);
    m[3][1] = g + (5.0f + 0.3f * h);
    flmatMul(&r, &node->local, &m);
    flSetRenderState((u8)(node->no + 0x1A), (u32)&r);
    if (node->child != 0) {
        SetSkinTransKKK(node->child, h);
    }
    if (node->next != 0) {
        SetSkinTransKKK(node->next, h);
    }
}

static void eft01_t(PRIM *pr) {
    f32 v[3];
    FLMAT mt;
    FLMAT mr;
    FLMAT ms;
    EFTW *ew = pr->owner;
    s16 area;
    f32 g;
    EFT_MDLW *mw;
    void *mats;
    EFT01_CHR *chr = (EFT01_CHR *)ew->owner;
    CLAY *cl;
    SHADOW_SIZE *tbl;
    int i;

    if (chr->x7D6 != 0) {
        return;
    }
    if (ew->arg == 0) {
        mw = eft_mdlw[1];
    } else if (ew->arg == 4) {
        area = Em_area_ck(chr->kind);
        if (area == -1) {
            return;
        }
        mw = game_w.area_mdlw[area];
    } else {
        mw = eft_mdlw[0];
    }
    if (chr->x10 == 0) {
        if (Pl_master_ck(chr) == 1 && chr->x763 != 0) {
            return;
        }
    } else if (game_w.x1DC != 0) {
        tbl = D_610300;
    } else {
        tbl = D_63BC40;
    }
    if (game_w.stage != chr->stg) {
        return;
    }
    if (mw == 0 || chr->x01 == 0) {
        return;
    }
    if (mw->flag == 0) {
        return;
    }
    flSetRenderState(0x60, 0);
    flSetRenderState(0x6C, 0);
    SetFilterMode(1);
    mats = mw->mat;
    switch (ew->arg) {
    case 0:
    case 3:
        cl = mw->clay;
        if (chr->x10 == 0) {
            flmatMakeScale(&ms, 4.0f, 1.0f, 4.0f);
        } else {
            flmatMakeScale(&ms, chr->scale[0] * tbl[chr->kind][0], 1.0f, chr->scale[2] * tbl[chr->kind][1]);
        }
        flmatMakeTrans(&mt, ew->pos[0], 3.0f + ew->pos[1], ew->pos[2]);
        flmatRotY33(&mt, flConvertStoR(((EFT01_CHR *)ew->owner)->ang));
        flmatMul33_2(&mt, &ms);
        flCalcTrans(mw->skin, &mt);
        if (ew->arg == 0) {
            flSetSkinTrans(mw->skin);
        } else {
            SetSkinTransKKK(mw->skin, ew->x1E);
        }
        for (i = 0; i < mw->clay_num; i++, cl++) {
            if (cl != 0 && cl->handle != -1) {
                eft01_material_set(chr, mats, cl);
                clay_attr_set(cl->attr);
                flExecuteClay(cl->handle, 0);
            }
        }
        break;
    case 1:
        cl = &mw->clay[51];
        flmatMakeScale(&ms, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                flmatGetTrans(v, &chr->foot0->mat);
            } else {
                flmatGetTrans(v, &chr->foot1->mat);
            }
            v[1] -= 13.0f;
            g = GetGroundHit(v);
            if (!(g < v[1])) {
                g = v[1];
            }
            flmatMakeTrans(&mt, v[0], 4.0f + g, v[2]);
            flmatMul(&mr, &ms, &mt);
            flSetRenderState(0x1A, (u32)&mr);
            eft01_material_set(chr, mats, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 2:
        cl = &mw->clay[50];
        flmatMakeScale(&ms, type02_scl[pr->no], type02_scl[pr->no], type02_scl[pr->no]);
        flmatMakeTrans(&mt, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul(&mr, &ms, &mt);
        flSetRenderState(0x1A, (u32)&mr);
        eft01_material_set(chr, mats, cl);
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
        break;
    case 4:
        cl = &mw->clay[3];
        flmatMakeScale(&ms, tbl[chr->kind][0], 1.0f, tbl[chr->kind][1]);
        flmatMakeTrans(&mt, ew->pos[0], 3.0f + ew->pos[1], ew->pos[2]);
        flmatMul(&mr, &ms, &mt);
        flSetRenderState(0x1A, (u32)&mr);
        flSetRenderState(0x67, 0x80FFFFFF);
        eft01_material_set(chr, mats, cl);
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
        break;
    }
    flSetRenderState(0x6C, 1);
    clay_attr_reset();
}

void eft01_set(EFT01_CHR *chr, s16 kind) {
    EFTW *ew;

    if (kind == 2) {
        if ((ew = pull_eft_work(1)) == 0) {
            return;
        }
    } else if ((ew = pull_eft_work(0)) == 0) {
        return;
    }
    {
        ew->type = 1;
        ew->move = eft01_move;
        ew->work14 = 0;
        ew->owner = (EMW *)chr;
        ew->arg = kind;
        if (ew->arg == 1) {
            ew->prim2 = 1;
        } else {
            ew->x1E = chr->id;
            get_joint_pos_em(chr, 10, ew->pos);
            ew->pos[1] = chr->x5AC;
        }
    }
}
