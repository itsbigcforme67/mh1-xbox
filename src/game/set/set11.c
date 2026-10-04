/* set11 - game.bin 0x00622180-0x006229B0. A five-piece structure on stage
 * 28 that falls in: when game_w.flag1B3 bit 0 is set and monster kind 7 is
 * on the stage, it plays two sounds and three puffs, then over five frames
 * each piece drops by its offset and tips by its angle. Each piece has its
 * own prim and entry in the set work area. */
#include "set.h"
#include "game.h"
#include "em.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

/* One piece, in the SETW work area (sw->u.work). */
typedef struct SET11_PART {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 piece number */
    f32 pos[3];         /* 0x04 */
    u16 rot;            /* 0x10 tilt, 0x10000 = 360 degrees */
    u8 _pad12[2];
    PRIM *prim;         /* 0x14 */
} SET11_PART;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern f32 set11_st28_pos[5][3];
extern s16 set11_st28_model_no[5];
extern u16 set11_st28_ang_y[5];
extern f32 set11_st28_trans[5];
extern s16 set11_st28_rot[5];
extern f32 set11_st28_se_pos[3];
extern f32 set11_st28_eft_pos[3][3];

void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
u8 Em_stg_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Eft13_set_pos(f32, f32 *, int);

static void set11_move(SETW *sw);
static void set11_i(SETW *sw);
static void set11_m(SETW *sw);
static void set11_d(SETW *sw);
static void set11_e(SETW *sw);
static void set11_trans(PRIM *pr);

void set11_set(void) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 11;
        sw->arg = 0;
        sw->move = set11_move;
    }
}

static void set11_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set11_i(sw);
        break;
    case 1:
        set11_m(sw);
        break;
    case 2:
        set11_d(sw);
        break;
    case 3:
        set11_e(sw);
        break;
    }
}

static void set11_i(SETW *sw) {
    SET11_PART *p = sw->u.work;
    s16 i;

    sw->mode++;
    if (game_w.flag1B3 & 1) {
        sw->mode2 = 2;
    } else {
        sw->mode2 = 0;
    }
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    sw->timer = 0;
    for (i = 0; i < 5; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->no = i;
            p->rot = 0;
            flvecCopy(p->pos, set11_st28_pos[i]);
            if (sw->mode2 == 2) {
                p->pos[1] += set11_st28_trans[i];
                p->rot += (u16)(s32)(0.5f + 65536.0f * set11_st28_rot[i] / 360.0f);
            }
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = sw;
            p->prim->no = i;
            p->prim->trans = set11_trans;
        } else {
            p->prim = 0;
        }
    }
}

static void set11_m(SETW *sw) {
    SET11_PART *p = sw->u.work;
    EMW *em = em_work;
    s16 mode2 = sw->mode2;
    s16 i;

    switch (sw->mode2) {
    case 0:
        if (game_w.flag1B3 & 1) {
            for (i = 0; i < 20; i++, em++) {
                if (em->be_flag != 0 && em->x01 != 0 && Em_stg_ck(em) != 0 && em->kind == 7) {
                    Em_se_req2(em, 0x1A, 0, set11_st28_se_pos, 10, 0);
                    Em_se_req2(em, 0x1B, 0, set11_st28_se_pos, 10, 0);
                    break;
                }
            }
            for (i = 0; i < 3; i++) {
                Eft13_set_pos(25.0f, set11_st28_eft_pos[i], 0);
            }
            sw->mode2++;
        }
        break;
    case 1:
        if (++sw->timer >= 5) {
            sw->mode2++;
        }
        break;
    case 2:
        break;
    }
    for (i = 0; i < 5; i++, p++) {
        switch (mode2) {
        case 0:
            if (p->no >= 4) {
                continue;
            }
            break;
        case 1:
            p->pos[1] += set11_st28_trans[p->no] / 5.0f;
            p->rot += (u16)(s32)(0.5f + 65536.0f * (set11_st28_rot[p->no] / 5) / 360.0f);
            break;
        case 2:
            if (p->no < 4) {
                continue;
            }
            break;
        }
        if (p->prim != 0) {
            p->prim->pos[0] = p->pos[0];
            p->prim->pos[1] = p->pos[1];
            p->prim->pos[2] = p->pos[2];
            add_prim(ot1, p->prim, 0x20, 0);
        }
    }
}

static void set11_d(SETW *sw) {
    SET11_PART *p = sw->u.work;
    s16 i;

    sw->mode++;
    sw->be_flag = 0;
    for (i = 0; i < 5; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void set11_e(SETW *sw) {
    push_set_work(sw);
}

static void set11_trans(PRIM *pr) {
    FLMAT mat;
    SET11_PART *p = &((SET11_PART *)((SETW *)pr->owner)->u.work)[pr->no];
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = &mw->clay[set11_st28_model_no[p->no]];
        flSetRenderState(0x60, 0x80);
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatRotXYZ33(&mat, DEG2RAD(ANG2DEG(p->rot)), DEG2RAD(ANG2DEG(set11_st28_ang_y[p->no])), 0.0f);
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != 0) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0);
            clay_attr_reset();
        }
    }
}
