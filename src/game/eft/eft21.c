/* eft21 - game.bin 0x00555020-0x00555A8C. Four small bits that pop out of
 * a player's joint (which joint and size by arg), fall, bounce once off
 * the ground and fade after 100 frames. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

/* One material table entry (0x4C bytes). */
typedef struct MATERIAL {
    u8 _pad00[4];
    f32 col[3];         /* 0x04 */
    u8 _pad10[0x4C - 0x10];
} MATERIAL;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    MATERIAL *mat;      /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One bit (0x34 bytes) of the work area. */
typedef struct EFT21_BIT {
    u8 alive;           /* 0x00 */
    u8 vis;             /* 0x01 */
    u8 state;           /* 0x02 */
    u8 _pad03;
    s16 timer;          /* 0x04 */
    s16 prim_no;        /* 0x06 */
    f32 pos[3];         /* 0x08 */
    f32 vel[3];         /* 0x14 */
    s32 rot[3];         /* 0x20 */
    PRIM *prim;         /* 0x2C */
    f32 scale;          /* 0x30 */
} EFT21_BIT;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
FLMAT *get_joint_wmat(PLW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatSetXYZ33(FLMAT *, f32, f32, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void AddVector(f32 *, f32 *, f32 *);
void RotateY(FLMAT *, f32);

static void eft21_move(EFTW *ew);
static void eft21_i(EFTW *ew);
static void eft21_m(EFTW *ew);
static void eft21_d(EFTW *ew);
static void eft21_e(EFTW *ew);
static void eft21_t(PRIM *pr);

void Eft21_set(PLW *pl, int arg) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 21;
            ew->move = eft21_move;
            ew->work14 = 0;
            ew->owner = (EMW *)pl;
            ew->x1E = pl->id;
            ew->arg = arg;
        }
    }
}

static void eft21_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft21_i(ew);
        break;
    case 1:
        eft21_m(ew);
        break;
    case 2:
        eft21_d(ew);
        break;
    case 3:
        eft21_e(ew);
        break;
    }
}

static void eft21_i(EFTW *ew) {
    f32 v[3];
    FLMAT m;
    PLW *pl = (PLW *)ew->owner;
    EFT21_BIT *p = ew->work;
    s16 i;
    s16 found;
    f32 sc;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    switch (ew->arg) {
    default:
        flmatCopy(&m, get_joint_wmat(pl, 10));
        sc = 1.0f;
        flmatGetTrans(ew->pos, &m);
        break;
    case 1:
        flmatCopy(&m, get_joint_wmat(pl, 0x24));
        sc = 1.5f;
        flmatGetTrans(ew->pos, &m);
        break;
    case 2:
        flmatCopy(&m, get_joint_wmat(pl, 0x27));
        sc = 1.5f;
        v[0] = 0.0f;
        v[1] = 30.0f;
        v[2] = 100.0f;
        flmatGetTrans(ew->pos, &m);
        flvecApplyMat33_2(v, &m);
        AddVector(ew->pos, ew->pos, v);
        RotateY(&m, 3.1415927f);
        break;
    }
    found = 1;
    for (i = 0; i < 4; i++, p++) {
        p->state = 0;
        p->vis = 0;
        p->scale = sc * (0.3f + 0.01f * ((u16)ran_suu(1) & 0x3F));
        p->timer = (u16)ran_suu(1) & 0xF;
        v[0] = ((u16)ran_suu(1) & 0x7F) - 64.0f;
        v[1] = ((u16)ran_suu(1) & 0x3F) - 32.0f;
        v[2] = -1.0f * ((u16)ran_suu(1) & 0x3F) - 32.0f;
        p->pos[0] = ew->pos[0] + v[0];
        p->pos[1] = ew->pos[1] + v[1];
        p->pos[2] = ew->pos[2] + v[2];
        p->rot[0] = (u16)ran_suu(1);
        p->rot[1] = (u16)ran_suu(1);
        p->rot[2] = (u16)ran_suu(1);
        v[0] = 0.1f * ((u16)ran_suu(1) & 0x3F) - 3.2f;
        v[1] = 0.0f;
        v[2] = 2.0f + 0.1f * ((u16)ran_suu(1) & 0xF);
        flvecApplyMat33_2(v, &m);
        p->vel[0] = v[0];
        p->vel[1] = 0.0f;
        p->vel[2] = v[2];
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft21_t;
            found = 0;
            p->alive = 1;
        } else {
            p->prim = 0;
            p->alive = 0;
        }
    }
    ew->pos[1] = pl->x5AC;
    if (found != 0) {
        push_eft_work(ew);
    }
}

void ef21_rate_add(EFTW *ew, EFT21_BIT *p) {
    p->pos[0] += p->vel[0];
    p->pos[1] += p->vel[1];
    p->pos[2] += p->vel[2];
    p->vel[1] += -0.72727275f;
}

static void eft21_m(EFTW *ew) {
    EFT21_BIT *p = ew->work;
    s16 i;
    s16 dead = 1;
    PLW *pl = (PLW *)ew->owner;

    for (i = 0; i < 4; i++) {
        if (p->alive != 0) {
            dead = 0;
            switch (p->state) {
            case 0:
                if (--p->timer <= 0) {
                    p->state++;
                    p->vis = 1;
                }
                break;
            case 1:
                ef21_rate_add(ew, p);
                if (p->pos[1] <= ew->pos[1]) {
                    p->state++;
                    p->timer = 100;
                    p->pos[1] = ew->pos[1];
                    p->vel[0] *= 1.8f;
                    p->vel[2] *= 1.8f;
                    p->vel[1] = 3.0f + 0.15f * ((u16)ran_suu(1) & 0x1F);
                }
                break;
            case 2:
                ef21_rate_add(ew, p);
                if (--p->timer <= 0) {
                    p->alive = 0;
                }
                break;
            }
            if (p->vis != 0) {
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
            /* Only live entries advance the pointer (as in the original). */
            p++;
        }
    }
    if (dead != 0 || Pl_stg_ck(pl) == 0) {
        ew->mode++;
    }
}

static void eft21_d(EFTW *ew) {
    EFT21_BIT *p = ew->work;
    s16 i;

    ew->mode++;
    for (i = 0; i < 4; i++, p++) {
        if (p->prim_no != -1) {
            release_prim(p->prim_no);
        }
    }
}

static void eft21_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft21_t(PRIM *pr) {
    FLMAT m;
    FLMAT sc;
    EFTW *ew = pr->owner;
    EFT21_BIT *p = &((EFT21_BIT *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    MATERIAL *mt;
    MATERIAL *mm;
    CLAY *cl;
    s32 i;

    switch (ew->arg) {
    case 0:
        cl = &mw->clay[43];
        flSetRenderState(0x67, -1);
        break;
    default:
    case 1:
        cl = &mw->clay[41];
        flSetRenderState(0x67, -1);
        break;
    case 2:
        cl = &mw->clay[41];
        flSetRenderState(0x67, 0xFFD08E65);
        break;
    }
    mt = mw->mat;
    flmatMakeScale(&sc, p->scale, p->scale, p->scale);
    flmatMakeTrans(&m, p->pos[0], p->pos[1], p->pos[2]);
    flmatSetXYZ33(&m, DEG2RAD(ANG2DEG(p->rot[0])), DEG2RAD(ANG2DEG(p->rot[1])), DEG2RAD(ANG2DEG(p->rot[2])));
    flmatMul33_2(&m, &sc);
    flSetRenderState(0x1A, (u32)&m);
    if (cl != 0 && cl->handle != -1) {
        for (i = 0; i < cl->mat_num; i++) {
            mm = &mt[cl->mat_no[i]];
            mm->col[0] = 1.0f;
            mm->col[1] = 1.0f;
            mm->col[2] = 1.0f;
            flSetRenderState((u8)(i + 0x3A), (u32)mm);
        }
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}
