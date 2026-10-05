/* eft19 - game.bin 0x00554980-0x00555020. A small model that pops up on a
 * joint of its owner (a monster, or a player when x07 is 0) and follows
 * it: grows over 15 frames, then lasts until the monster's state changes
 * (or 60 frames for a player). The model shown depends on the monster's
 * state (EMW+0x884/0x885). */
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

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern f32 *eft19_em_ofs_tbl[];
extern f32 *eft19_pl_ofs_tbl[1];
extern s16 eft19_model_no[4];
extern f32 eft19_type0_trans[];
extern f32 eft19_type0_scale[];
extern f32 eft19_type1_scale[];

u8 Pl_stg_ck(void *);
void release_prim(s16);
void eft_vec_linear(f32, f32 *, f32 *);
void get_joint_pos(PLW *, int, f32 *);
void get_joint_pos_em(EMW *, int, f32 *);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void Material_set_sub(void *, CLAY *);

static void eft19_move(EFTW *ew);
static void eft19_i(EFTW *ew);
static void eft19_m(EFTW *ew);
static void eft19_d(EFTW *ew);
static void eft19_e(EFTW *ew);
static void eft19_t(PRIM *pr);

static void eft19_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft19_i(ew);
        break;
    case 1:
        eft19_m(ew);
        break;
    case 2:
        eft19_d(ew);
        break;
    case 3:
        eft19_e(ew);
        break;
    }
}

static void eft19_i(EFTW *ew) {
    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    if (ew->arg == 0) {
        ew->mode2 = 0;
        ew->timer = 15;
    } else if (ew->arg == 1) {
        ew->mode2 = 3;
        ew->timer = 0;
    }
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->no = 0;
        ew->prim->trans = eft19_t;
    } else {
        ew->prim = 0;
    }
}

static void eft19_m(EFTW *ew) {
    f32 pos[3];
    f32 v[3];
    FLMAT m;
    PLW *pl;
    EMW *em;
    f32 *ofs;
    u8 s;

    if (ew->x07 == 0) {
        pl = (PLW *)ew->owner;
    } else {
        em = ew->owner;
    }
    if (ew->owner == 0 || ew->owner->be_flag == 0 || ew->owner->x04 == 2) {
        ew->mode++;
        return;
    }
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    if (ew->x07 != 0) {
        if (em->x884 == 0 && em->x885 == 0) {
            s = 0;
        } else if (em->x885 != 0) {
            s = 2;
        } else if (em->x884 != 0) {
            s = 1;
        } else {
            s = 0;
        }
        if (s == 0) {
            if (++ew->timer > 60 || ew->mode2 == 0) {
                ew->timer = 60;
                ew->mode2 = 0;
                return;
            }
            eft_vec_linear((ew->timer > 15) ? 15 : ew->timer, eft19_type0_trans, v);
        } else {
            ew->mode2 = s;
            ew->timer = 0;
        }
        get_joint_pos_em(em, ew->u0A.joint, pos);
        flmatCopy(&m, get_joint_wmat_em(em, ew->u0A.joint));
        ofs = eft19_em_ofs_tbl[em->kind];
    } else {
        if (++ew->timer > 60) {
            ew->mode++;
            return;
        }
        get_joint_pos(pl, ew->u0A.joint, pos);
        flmatCopy(&m, &rview_mat);
        ofs = eft19_pl_ofs_tbl[0];
    }
    v[0] += *ofs++;
    v[1] += *ofs++;
    v[2] += *ofs++;
    flvecApplyMat33_2(v, &m);
    if (ew->prim != 0) {
        ew->prim->pos[0] = pos[0] + v[0];
        ew->prim->pos[1] = pos[1] + v[1];
        ew->prim->pos[2] = pos[2] + v[2];
        add_prim(ot0, ew->prim, 0x40, 0);
    }
}

static void eft19_d(EFTW *ew) {
    ew->mode++;
    if (ew->prim != 0) {
        release_prim(ew->prim_no);
    }
}

static void eft19_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft19_t(PRIM *pr) {
    FLMAT m;
    f32 sc[3];
    EFT_MDLW *mw = eft_mdlw[0];
    EFTW *ew = pr->owner;
    void *mats;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0 && Pl_stg_ck(ew->owner) != 0) {
        mats = mw->mat;
        switch (ew->arg) {
        case 0:
            eft_vec_linear((ew->timer > 15) ? 15 : ew->timer, eft19_type0_scale, sc);
            sc[0] *= 1.5f;
            sc[1] *= 1.5f;
            sc[2] *= 1.5f;
            break;
        default:
            eft_vec_linear(ew->timer, eft19_type1_scale, sc);
            break;
        }
        flmatMakeScale(&m, sc[0], sc[1], sc[2]);
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[eft19_model_no[ew->mode2]];
        if (cl != 0 && cl->handle != -1) {
            flSetRenderState(0x60, 0);
            flSetRenderState(0x1A, (u32)&m);
            flSetRenderState(0x67, -1);
            Material_set_sub(mats, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}

void Eft19_set(EMW *owner, int joint, int arg) {
    EFTW *ew = pull_eft_work(0);

    if (ew != 0) {
        ew->type = 19;
        ew->move = eft19_move;
        ew->arg = arg;
        ew->x07 = 1;
        ew->u0A.joint = joint;
        ew->owner = owner;
    }
}
