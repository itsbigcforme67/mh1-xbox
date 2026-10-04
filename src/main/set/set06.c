/* set06 - SLPM_654.95 0x00156330-0x001567B4.
 * A model placed on stage 0x4E (position per set argument) or 0x57; on 0x57
 * it disappears once the master player's animation 0x287 reaches frame 232. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern f32 set06_st78_pos[][3];
extern f32 set06_st87_pos[3];
extern f32 set06_st87_rot[3];

int frame_check(PLW *, int, int, f32);

static void set06_move(SETW *sw);
static void set06_i(SETW *sw);
static void set06_m(SETW *sw);
static void set06_d(SETW *sw);
static void set06_e(SETW *sw);
static void set06_trans(PRIM *pr);

void Set06_set(int arg) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 6;
        sw->arg = arg;
        sw->move = set06_move;
    }
}

static void set06_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set06_i(sw);
        break;
    case 1:
        set06_m(sw);
        break;
    case 2:
        set06_d(sw);
        break;
    case 3:
        set06_e(sw);
        break;
    }
}

static void set06_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    switch (game_w.stage) {
    default:
        break;
    case 0x4E:
        sw->pos[0] = set06_st78_pos[sw->arg][0];
        sw->pos[1] = set06_st78_pos[sw->arg][1];
        sw->pos[2] = set06_st78_pos[sw->arg][2];
        break;
    case 0x57:
        sw->pos[0] = set06_st87_pos[0];
        sw->pos[1] = set06_st87_pos[1];
        sw->pos[2] = set06_st87_pos[2];
        break;
    }
    sw->prim_no = get_prim();
    sw->prim = get_prim_ptr(sw->prim_no);
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set06_trans;
    } else {
        push_set_work(sw);
    }
}

static void set06_m(SETW *sw) {
    u8 no = game_w.master;
    PLW *pl = &player_work[no];

    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    switch (game_w.stage) {
    default:
        break;
    case 0x57:
        if (pl->char0 == 0x287 && frame_check(pl, 0, no, 232.0f) != 0) {
            sw->be_flag = 0;
        }
        break;
    }
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set06_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set06_e(SETW *sw) {
    push_set_work(sw);
}

static void set06_trans(PRIM *pr) {
    FLMAT mat;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    s32 none;

    if (sw->be_flag != 0) {
        if (mw != 0) {
            if (mw->flag != 0) {
                flmatInit(&mat);
                switch (game_w.stage) {
                case 0x57:
                    flSetRenderState(0x1A, (u32)&mat);
                    cl = &mw->clay[1];
                    none = -1;
                    flSetRenderState(0x67, none);
                    if (cl != 0 && cl->handle != none) {
                        clay_attr_set(cl->attr);
                        flExecuteClay(cl->handle, 0);
                    }
                    flmatRotXYZ33(&mat, set06_st87_rot[0], set06_st87_rot[1], set06_st87_rot[2]);
                    break;
                default:
                    none = -1;
                    flSetRenderState(0x67, none);
                    break;
                }
                flmatSetTrans(&mat, sw->pos[0], sw->pos[1], sw->pos[2]);
                flSetRenderState(0x1A, (u32)&mat);
                cl = &mw->clay[sw->arg];
                if (cl != 0 && cl->handle != none) {
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
                clay_attr_reset();
            }
        }
    }
}
