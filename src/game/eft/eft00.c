/* eft00 - game.bin 0x00550F70-0x00551824. Flying debris: nine tumbling
 * pieces thrown out from a point with random speeds (higher for arg 1),
 * under gravity, for 30 frames. x07 picks the debris model set. */
#include "eft.h"
#include "game.h"
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

/* One piece (0x38 bytes) of the work area. */
typedef struct EFT00_PIECE {
    s16 kind;           /* 0x00 model variant */
    s16 prim_no;        /* 0x02 */
    f32 pos[3];         /* 0x04 */
    f32 vel[3];         /* 0x10 */
    f32 acc[3];         /* 0x1C */
    s32 rot[3];         /* 0x28 */
    PRIM *prim;         /* 0x34 */
} EFT00_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void SetFilterMode(int);
void Material_set_sub(void *, CLAY *);
void Eft_rendope_set(int);

static void eft00_move(EFTW *ew);
static void eft00_i(EFTW *ew);
static void eft00_m(EFTW *ew);
static void eft00_d(EFTW *ew);
static void eft00_e(EFTW *ew);
static void eft00_t(PRIM *pr);

static void eft00_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft00_i(ew);
        break;
    case 1:
        eft00_m(ew);
        break;
    case 2:
        eft00_d(ew);
        break;
    case 3:
        eft00_e(ew);
        break;
    }
}

static void eft00_i(EFTW *ew) {
    EFT00_PIECE *p = ew->work;
    s32 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    for (i = 0; i < 9; i++, p++) {
        p->pos[0] = ew->pos[0];
        p->pos[1] = ew->pos[1];
        p->pos[2] = ew->pos[2];
        p->rot[0] = (u16)ran_suu(1);
        p->rot[1] = (u16)ran_suu(1);
        p->rot[2] = (u16)ran_suu(1);
        switch (ew->arg) {
        case 0:
            p->vel[0] = 2.0f * (3.0f + 0.3f * ((u16)ran_suu(1) % 10));
            p->vel[1] = 2.0f * (5.0f + 0.4f * ((u16)ran_suu(1) % 10));
            p->vel[2] = 2.0f * (3.0f + 0.3f * ((u16)ran_suu(1) % 10));
            break;
        case 1:
            p->vel[0] = 2.0f * (3.0f + 0.3f * ((u16)ran_suu(1) % 10));
            p->vel[1] = 2.0f * (1.5f * (10.0f + 0.5f * ((u16)ran_suu(1) % 10)));
            p->vel[2] = 2.0f * (3.0f + 0.3f * ((u16)ran_suu(1) % 10));
            break;
        }
        switch (i % 4) {
        case 0:
            break;
        case 1:
            p->vel[0] *= -1.0f;
            p->vel[2] *= -1.0f;
            break;
        case 2:
            p->vel[0] *= -1.0f;
            break;
        case 3:
            p->vel[2] *= -1.0f;
            break;
        }
        p->acc[0] = 0.0f;
        p->acc[2] = 0.0f;
        p->acc[1] = 2.0f * (-1.0f * (p->vel[1] / 15.0f));
        p->kind = (u16)ran_suu(1) % 5;
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft00_t;
        } else {
            p->prim = 0;
        }
    }
}

static void eft00_m(EFTW *ew) {
    EFT00_PIECE *p = ew->work;
    s32 i;

    if (ew->timer++ >= 30) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < 9; i++, p++) {
        p->pos[0] += p->vel[0];
        p->pos[1] += p->vel[1];
        p->pos[2] += p->vel[2];
        p->vel[0] += p->acc[0];
        p->vel[1] += p->acc[1];
        p->vel[2] += p->acc[2];
        p->rot[0] += 0x200;
        p->rot[1] += 0x240;
        p->rot[2] += 0x300;
        if (p->prim != 0) {
            p->prim->pos[0] = p->pos[0];
            p->prim->pos[1] = p->pos[1];
            p->prim->pos[2] = p->pos[2];
            add_prim(ot1, p->prim, 0x20, 0);
        }
    }
}

static void eft00_d(EFTW *ew) {
    EFT00_PIECE *p = ew->work;
    s32 i;

    ew->mode++;
    for (i = 0; i < 9; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft00_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft00_t(PRIM *pr) {
    FLMAT m;
    EFTW *ew = pr->owner;
    EFT00_PIECE *p = &((EFT00_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        SetFilterMode(0);
        flmatMakeTrans(&m, p->pos[0], p->pos[1], p->pos[2]);
        flmatRotXYZ33(&m, DEG2RAD(ANG2DEG(p->rot[0])), DEG2RAD(ANG2DEG(p->rot[1])), DEG2RAD(ANG2DEG(p->rot[2])));
        flSetRenderState(0x1A, (u32)&m);
        if (ew->x07 == 0) {
            cl = mw->clay + p->kind + 32;
        } else if (ew->x07 == 1) {
            cl = &mw->clay[43];
        } else {
            cl = &mw->clay[73];
        }
        if (cl != 0 && cl->handle != -1) {
            Material_set_sub(mats, cl);
            clay_attr_set(cl->attr);
            Eft_rendope_set(0x10);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
        SetFilterMode(1);
    }
}

void eft00_set(EMW *em, int arg, f32 *pos, int x07) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0;
            ew->move = eft00_move;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->owner = em;
            ew->arg = arg;
            ew->x07 = x07;
        }
    }
}
