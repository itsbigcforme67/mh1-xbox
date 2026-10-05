/* eft09 - game.bin 0x00544DD0-0x005454C8. A monster's cut-off tail. It
 * waits for the monster's tail-cut moment (per monster kind), then either
 * becomes a pick-up point (Ext_pick_point_*) or stays as a body part drawn
 * with the monster's skinned model at the spot where it fell. */
#include "eft.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct MAT_LIST {
    u8 _pad00[0x42];
    u8 list[0x420 - 0x42];
} MAT_LIST;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern u8 mat_palett[];
extern MAT_LIST mat_list[];

int get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
s32 Em_tail_hagi_point_set(EFTW *, int);
u16 calc_mat_angY(void *);
void flmatGetTrans(f32 *, void *);
void flmatCopy(void *, void *);
void flCalcTransSI(void *, FLMAT *);
void flSetSkinTransMatrixList(void *, void *);
void flSetMatrixList(void *, void *);
int em_frame_check2(EMW *, int, f32);
void Ext_pick_point_clr();
void Ext_pick_point_pos(int, f32 *);
int Ext_pick_point_cnt_ck(int);
int softdip_ck(int);
void pl_light_change(EMW *, int);
void Pl_light_set(EMW *);
void reload_tex(int, int);
void em_material_sub(EMW *, int, CLAY *);

static void eft09_move(EFTW *ew);
static void eft09_i(EFTW *ew);
void tail_off(EFTW *ew);
static void eft09_m(EFTW *ew);
static void eft09_d(EFTW *ew);
static void eft09_e(EFTW *ew);
static void eft09_t(PRIM *pr);

static void eft09_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft09_i(ew);
        break;
    case 1:
        eft09_m(ew);
        break;
    case 2:
        eft09_d(ew);
        break;
    case 3:
        eft09_e(ew);
        break;
    }
}

static void eft09_i(EFTW *ew) {
    EMW *em = ew->owner;

    ew->mode++;
    ew->mode2 = 0;
    ew->x07 = 0xFF;
    ew->arg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    if (em->kind == 0x22) {
        ew->prim2 = 0;
    } else {
        ew->prim2 = 1;
    }
    if (ew->prim2 == 1) {
        ew->prim_no = get_prim2();
    } else {
        ew->prim_no = get_prim();
    }
    if (ew->prim_no != -1) {
        if (ew->prim2 == 1) {
            ew->prim = get_prim_ptr2(ew->prim_no);
        } else {
            ew->prim = get_prim_ptr(ew->prim_no);
        }
        ew->prim->owner = ew;
        ew->prim->trans = eft09_t;
    } else {
        push_eft_work(ew);
    }
}

void tail_off(EFTW *ew) {
    EMW *em = ew->owner;
    s32 n;
    u8 *bone;

    if (ew->mode2 <= 0) {
        n = Em_tail_hagi_point_set(ew, 1);
        if (n == -1) {
            ew->x07 = 0xFF;
            ew->mode2 = 2;
        } else {
            ew->x07 = n;
            ew->mode2 = 1;
        }
    }
    ew->arg = 1;
    bone = em->mdl->bone;
    flmatGetTrans(ew->pos, bone + 0x4330);
    ew->u0A.ang = calc_mat_angY(bone + 0x4330);
}

static void eft09_m(EFTW *ew) {
    EMW *em = ew->owner;

    if (em->be_flag == 0) {
        ew->mode++;
        if (ew->x07 != 0xFF) {
            Ext_pick_point_clr();
        }
        return;
    }
    switch (ew->mode2) {
    case 0:
        ew->stg = em->stg;
        switch (em->kind) {
        case 1:
        case 0xB:
        case 0xE:
        case 0x1A:
            if (em->mode == 4 && em->x15 == 0xF) {
                tail_off(ew);
            }
            break;
        case 0x11:
        case 0x16:
            if (em->mode == 4 && em->x15 == 0x11) {
                tail_off(ew);
            }
            break;
        default:
            if (em->char0 == 0x429 && em_frame_check2(em, 0, 300.0f) != 0) {
                tail_off(ew);
            }
            break;
        }
        break;
    case 1:
        if (ew->x07 != 0xFF) {
            Ext_pick_point_pos(ew->x07, ew->pos);
            if (Ext_pick_point_cnt_ck(ew->x07) == 0) {
                ew->mode2++;
                Ext_pick_point_clr(ew->x07);
                ew->x07 = 0xFF;
            }
        } else {
            ew->mode2++;
        }
        break;
    case 2:
        break;
    }
    if (game_w.stage == ew->stg) {
        ew->prim->pos[0] = ew->pos[0];
        ew->prim->pos[1] = ew->pos[1];
        ew->prim->pos[2] = ew->pos[2];
        add_prim(ot1, ew->prim, 0x20, 1);
    }
}

static void eft09_d(EFTW *ew) {
    ew->mode++;
    if (ew->prim2 == 1) {
        release_prim2(ew->prim_no);
    } else {
        release_prim(ew->prim_no);
    }
}

static void eft09_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft09_t(PRIM *pr) {
    FLMAT mat;
    FLMAT sc;
    EFTW *ew = pr->owner;
    EMW *em = ew->owner;
    EM_MDL *mdl = em->mdl;
    u8 *bone;
    u8 *mtx;
    CLAY *cl;

    if (softdip_ck(0x24) == 0 && em->x01 != 0 && mdl != 0 && mdl->flag != 0) {
        pl_light_change(em, 1);
        Pl_light_set(em);
        bone = em->mdl->bone;
        mtx = em->mdl->mtx;
        if (ew->arg == 0) {
            flmatCopy(mtx, bone + 0x4330);
            flmatCopy(mtx + 0x190, bone + 0x4330);
            flmatCopy(mtx + 0x320, bone + 0x44C0);
        } else {
            flmatMakeScale(&sc, em->scale[0], em->scale[1], em->scale[2]);
            flmatMakeTrans(&mat, ew->pos[0], ew->pos[1], ew->pos[2]);
            flmatRotY33(&mat, DEG2RAD(ANG2DEG(ew->u0A.ang + 0x4000)));
            flmatMul33_2(&mat, &sc);
            flCalcTransSI(mtx, &mat);
        }
        flSetSkinTransMatrixList(mat_palett, mtx);
        cl = &mdl->clay[1];
        reload_tex(10, em->mdl_no * 20 + 0x9A);
        if (cl->handle != -1) {
            em_material_sub(em, 1, mdl->clay);
            flSetMatrixList(mat_list[em->mdl_no].list, mat_palett);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}

void eft09_set(EMW *em) {
    EFTW *ew = pull_eft_work(0);

    if (ew != 0) {
        ew->type = 9;
        ew->move = eft09_move;
        ew->owner = em;
        em->tail = ew;
    }
}
