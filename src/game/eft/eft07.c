/* eft07 - game.bin 0x00543720-0x00543F94. Monster eye effects. Arg 0: a
 * glint that now and then flashes on joint 2 (with a random tilt) or joint
 * 23, with a sound, while the eyes are shown (EMW+0x8B6). Arg 1: a
 * billboard glow on joint 13 that pulses in size. */
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

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern FLMAT rview_mat;

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
s16 Em_area_ck(int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void release_prim(s16);
void get_joint_pos_em(EMW *, int, f32 *);
FLMAT *get_joint_wmat(EMW *, int);
void flmatInit(FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatCopy33(FLMAT *, FLMAT *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void flvecCopy(f32 *, f32 *);
void Material_set_sub(void *, CLAY *);

static void eft07_move(EFTW *ew);
static void eft07_i(EFTW *ew);
static void eft07_m(EFTW *ew);
static void eft07_d(EFTW *ew);
static void eft07_e(EFTW *ew);
static void eft07_t(PRIM *pr);

static void eft07_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft07_i(ew);
        break;
    case 1:
        eft07_m(ew);
        break;
    case 2:
        eft07_d(ew);
        break;
    case 3:
        eft07_e(ew);
        break;
    }
}

static void eft07_i(EFTW *ew) {
    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft07_t;
    } else {
        push_eft_work(ew);
    }
}

static void eft07_m(EFTW *ew) {
    EMW *em = ew->owner;

    if (em->be_flag == 0 || em->x07 == 3) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    switch (ew->arg) {
    case 0:
        switch (ew->mode2) {
        case 0:
            if (em->x8B6 != 0 && --ew->timer <= 0) {
                Em_se_req2(em, 0x19, 0, em->pos, 1, 0);
                if (((u16)ran_suu(1) & 3) == 0) {
                    ew->mode2 = 1;
                    ew->u0A.joint = ran_suu(1);
                    ew->stg = ran_suu(1);
                    ew->x07 = ran_suu(1);
                } else {
                    ew->mode2 = 2;
                }
                ew->timer = 0;
            }
            break;
        case 1:
            if (++ew->timer >= 10) {
                ew->mode2 = 0;
                ew->timer = ((u16)ran_suu(1) & 0xF) + 4;
            } else {
                get_joint_pos_em(em, 2, ew->pos);
                flvecCopy(ew->prim->pos, ew->pos);
                add_prim(ot1, ew->prim, 0x20, 0);
            }
            break;
        case 2:
            if (++ew->timer >= 12) {
                ew->mode2 = 0;
                ew->timer = ((u16)ran_suu(1) & 0xF) + 4;
            } else {
                get_joint_pos_em(em, 0x17, ew->pos);
                flvecCopy(ew->prim->pos, ew->pos);
                add_prim(ot1, ew->prim, 0x20, 0);
            }
            break;
        }
        break;
    case 1:
        ew->timer++;
        ew->scale = 1.0f + 0.1f * flSin(DEG2RAD(ANG2DEG(ew->timer << 10)));
        get_joint_pos_em(em, 0xD, ew->pos);
        flvecCopy(ew->prim->pos, ew->pos);
        if (em->x8B6 != 0) {
            add_prim(ot0, ew->prim, 0x40, 0);
        }
        break;
    }
}

static void eft07_d(EFTW *ew) {
    ew->mode++;
    release_prim(ew->prim_no);
}

static void eft07_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft07_t(PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    void *mats;
    EFT_MDLW *mw;
    CLAY *cl;
    s16 area;
    s16 n;

    if (Em_stg_ck(ew->owner) != 0) {
        area = Em_area_ck(ew->owner->kind);
        if (area != -1) {
            mw = game_w.area_mdlw[area];
            if (mw != 0 && mw->flag != 0) {
                mats = mw->mat;
                switch (ew->arg) {
                case 0:
                    flSetRenderState(0x60, 0x80);
                    flmatInit(&m);
                    switch (ew->mode2) {
                    case 1:
                        n = 10;
                        flmatCopy33(&m, get_joint_wmat(ew->owner, 2));
                        flmatRotXYZ33(&m, DEG2RAD(ANG2DEG(ew->u0A.joint)), DEG2RAD(ANG2DEG(ew->stg << 8)),
                                      DEG2RAD(ANG2DEG(ew->x07 << 8)));
                        break;
                    case 2:
                        n = 12;
                        cl = &mw->clay[8];
                        flmatMakeTrans(&uv, 0.0f, 0.1875f * (ew->timer / 12.0f), 0.0f);
                        flSetRenderState(0x19, (u32)&uv);
                        flmatCopy(&m, get_joint_wmat(ew->owner, 2));
                        flSetRenderState(0x1A, (u32)&m);
                        if (cl != 0) {
                            if (cl->handle != -1) {
                                Material_set_sub(mats, cl);
                                clay_attr_set(cl->attr);
                                flExecuteClay(cl->handle, 0);
                            }
                            clay_attr_reset();
                        }
                        flmatInit(&m);
                        flmatCopy33(&m, get_joint_wmat(ew->owner, 0x17));
                        break;
                    default:
                        return;
                    }
                    flmatMakeTrans(&uv, 0.0f, 0.1875f * (ew->timer / (f32)n), 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    cl = mw->clay + ew->mode2 - 1;
                    break;
                case 1:
                    flSetRenderState(0x60, 0);
                    flmatMakeScale(&m, ew->scale, ew->scale, ew->scale);
                    flmatMul33_2(&m, &rview_mat);
                    cl = &mw->clay[2];
                    flSetRenderState(0x67, 0x80FFFFFF);
                    break;
                }
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                flSetRenderState(0x1A, (u32)&m);
                if (cl != 0) {
                    if (cl->handle != -1) {
                        Material_set_sub(mats, cl);
                        clay_attr_set(cl->attr);
                        flExecuteClay(cl->handle, 0);
                    }
                    clay_attr_reset();
                }
            }
        }
    }
}

void eft07_set(EMW *em, int arg) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 7;
            ew->move = eft07_move;
            ew->owner = em;
            ew->arg = arg;
        }
    }
}
