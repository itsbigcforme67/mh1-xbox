/* set07 - game.bin 0x006206D0-0x00620B64. Scenery on stages 26 and 42:
 * eight camera-facing models with a scrolling texture. */
#include "set.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET07_W {
    s16 prim_no[8];     /* 0x00 */
    PRIM *prim[8];      /* 0x10 */
} SET07_W;

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern f32 set07_st26_pos_tbl[8][3];
extern f32 set07_st26_scale_tbl[8][3];
extern f32 set07_st42_pos_tbl[8][3];
extern f32 set07_st42_scale_tbl[8][3];
extern f32 set07_uv[16][2];
extern FLMAT rview_matY;

static void set07_move(SETW *sw);
static void set07_i(SETW *sw);
static void set07_m(SETW *sw);
static void set07_d(SETW *sw);
static void set07_e(SETW *sw);
static void set07_trans(PRIM *pr);

void set07_set(void) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 7;
        sw->arg = 0;
        sw->move = set07_move;
    }
}

static void set07_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set07_i(sw);
        break;
    case 1:
        set07_m(sw);
        break;
    case 2:
        set07_d(sw);
        break;
    case 3:
        set07_e(sw);
        break;
    }
}

static void set07_i(SETW *sw) {
    s16 n;
    s16 i;
    f32 (*pos)[3];
    SET07_W *w = sw->u.work;
    PRIM *p;

    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    switch (game_w.stage) {
    case 0x1A:
        n = 8;
        pos = set07_st26_pos_tbl;
        break;
    case 0x2A:
        n = 8;
        pos = set07_st42_pos_tbl;
        break;
    }
    for (i = 0; i < n; i++) {
        w->prim_no[i] = get_prim();
        if (w->prim_no[i] != -1) {
            p = w->prim[i] = get_prim_ptr(w->prim_no[i]);
            p->owner = sw;
            p->no = i;
            p->trans = set07_trans;
            p->pos[0] = pos[i][0];
            p->pos[1] = pos[i][1];
            p->pos[2] = pos[i][2];
        } else {
            w->prim[i] = 0;
            if (i == 0) {
                set07_e(sw);
            }
        }
    }
}

static void set07_m(SETW *sw) {
    SET07_W *w = sw->u.work;
    s16 n;
    s16 i;

    sw->timer++;
    switch (game_w.stage) {
    case 0x1A:
        n = 8;
        break;
    case 0x2A:
        n = 8;
        break;
    }
    for (i = 0; i < n; i++) {
        if (w->prim[i] != 0) {
            add_prim(ot0, w->prim[i], 0x40, 0);
        }
    }
}

static void set07_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set07_e(SETW *sw) {
    push_set_work(sw);
}

static void set07_trans(PRIM *pr) {
    FLMAT mat, uv;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;
    f32 (*scl)[3];
    int k;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0);
        cl = &mw->clay[4];
        switch (game_w.stage) {
        case 0x1A:
            scl = set07_st26_scale_tbl;
            break;
        case 0x2A:
            scl = set07_st42_scale_tbl;
            break;
        }
        k = (sw->timer & 0x1E) >> 1;
        flmatMakeTrans(&uv, set07_uv[k][0], set07_uv[k][1], 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flmatMakeScale(&mat, scl[pr->no][0], scl[pr->no][1], scl[pr->no][2]);
        flmatSetTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&mat, &rview_matY);
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
