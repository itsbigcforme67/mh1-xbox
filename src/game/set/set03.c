/* set03 - game.bin 0x0061ED10-0x0061F2B8. Steam vents on stages 47-49:
 * after a random wait one of three spots erupts for 100 frames, with a
 * looping sound and puffs at fixed points of the countdown. */
#include "set.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern f32 set03_st47_pos[3][3];
extern f32 set03_st48_pos[3][3];
extern f32 set03_st49_pos[3][3];
extern FLMAT rview_mat;

u16 ran_suu(int);
void set12_set(int, int, int, f32 *, s16);
void Eft13_set_pos(f32, f32 *, int);

static void set03_move(SETW *sw);
static void set03_i(SETW *sw);
static void set03_m(SETW *sw);
static void set03_d(SETW *sw);
static void set03_e(SETW *sw);
static void set03_trans(PRIM *pr);

void Set03_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 3;
        sw->arg = 0;
        sw->move = set03_move;
    }
}

static void set03_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set03_i(sw);
        break;
    case 1:
        set03_m(sw);
        break;
    case 2:
        set03_d(sw);
        break;
    case 3:
        set03_e(sw);
        break;
    }
}

static void set03_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    sw->timer = (ran_suu(1) & 0x1F) + 10;
    sw->mode2 = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set03_trans;
        return;
    }
    push_set_work(sw);
}

static void set03_m(SETW *sw) {
    f32 (*tbl)[3];
    f32 v[3];
    s16 num;
    s16 len;

    switch (game_w.stage) {
    case 0x2F:
        num = 3;
        len = 100;
        tbl = set03_st47_pos;
        break;
    case 0x30:
        num = 3;
        len = 100;
        tbl = set03_st48_pos;
        break;
    case 0x31:
        num = 3;
        len = 100;
        tbl = set03_st49_pos;
        break;
    default:
        return;
    }
    switch (sw->mode2) {
    case 0:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->se0 = ran_suu(1) % num;
            sw->timer = 0;
        }
        break;
    case 1:
        if (++sw->timer >= len) {
            sw->mode2 = 0;
            sw->timer = ran_suu(1) & 0x3F;
        } else {
            sw->prim->pos[0] = tbl[sw->se0][0];
            sw->prim->pos[1] = tbl[sw->se0][1];
            sw->prim->pos[2] = tbl[sw->se0][2];
            if (sw->timer == 2) {
                set12_set(7, 0x20, 1, sw->prim->pos, len);
            }
            add_prim(ot0, sw->prim, 0x40, 0);
        }
        switch (len - sw->timer) {
        case 15:
        case 20:
        case 25:
        case 30:
        case 35:
            v[0] = sw->prim->pos[0];
            v[1] = -10.0f;
            v[2] = sw->prim->pos[2];
            Eft13_set_pos(1.75f, v, 0);
            break;
        }
        break;
    }
}

static void set03_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set03_e(SETW *sw) {
    push_set_work(sw);
}

static void set03_trans(PRIM *pr) {
    FLMAT mat, uv;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;
    s16 len;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0);
        flSetRenderState(0x67, -1);
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&mat, &rview_mat);
        switch (game_w.stage) {
        case 0x2F:
            len = 100;
            cl = &mw->clay[1];
            break;
        case 0x30:
            len = 100;
            cl = &mw->clay[5];
            break;
        case 0x31:
            len = 100;
            cl = &mw->clay[1];
            break;
        }
        flmatMakeTrans(&uv, 0.0f, 1.0f - (f32)sw->timer / (f32)len, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
