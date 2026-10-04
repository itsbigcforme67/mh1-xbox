/* set00 - game.bin 0x006213F0-0x00621C6C. Stage decorations drawn
 * translucent with a scrolling texture: 1-6 models per stage (stages 4, 8,
 * 26, 41, 42, 43), each turned and scaled from small per-stage tables. */
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

/* The set work area (sw->u.work). */
typedef struct SET00_WORK {
    s16 prim_no[6];     /* 0x00 */
    PRIM *prim[6];      /* 0x0C */
} SET00_WORK;

#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern FLMAT rview_matY;
/* Read with an 8-byte stride but 12 bytes per entry (see set00_i); the
 * tables are 12-byte entries, so entries after the first come out shifted.
 * That is what the original does. */
extern f32 set00_st04_pos_tbl[][2];
extern f32 set00_st08_pos_tbl[][2];
extern f32 set00_st26_pos_tbl[][2];
extern f32 set00_st41_pos_tbl[][2];
extern f32 set00_st42_pos_tbl[][2];
extern f32 set00_st43_pos_tbl[][2];
extern s16 set00_st04_dir_tbl[2];
extern s16 set00_st08_dir_tbl[6];
extern f32 set00_st26_scale_tbl[2];
extern s16 set00_st26_dir_tbl[2];
extern f32 set00_st41_scale_tbl[1][3];
extern s16 set00_st41_dir_tbl[1][2];
extern f32 set00_st42_scale_tbl[2];
extern s16 set00_st42_dir_tbl[2];
extern s16 set00_st43_dir_tbl[2];

void SetTrnslMode(int, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatScaleFactor33(FLMAT *, f32, f32, f32);

static void set00_move(SETW *sw);
static void set00_i(SETW *sw);
static void set00_m(SETW *sw);
static void set00_d(SETW *sw);
static void set00_e(SETW *sw);
static void set00_trans(PRIM *pr);

void set00_set(void) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 0;
        sw->move = set00_move;
    }
}

static void set00_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set00_i(sw);
        break;
    case 1:
        set00_m(sw);
        break;
    case 2:
        set00_d(sw);
        break;
    case 3:
        set00_e(sw);
        break;
    }
}

static void set00_i(SETW *sw) {
    PRIM *pr;
    s16 n, i;
    f32 (*tbl)[2];
    SET00_WORK *w = sw->u.work;

    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 4:
        n = 2;
        tbl = set00_st04_pos_tbl;
        break;
    case 8:
        n = 6;
        tbl = set00_st08_pos_tbl;
        break;
    case 0x1A:
        n = 2;
        tbl = set00_st26_pos_tbl;
        break;
    case 0x29:
        n = 1;
        tbl = set00_st41_pos_tbl;
        break;
    case 0x2A:
        n = 2;
        tbl = set00_st42_pos_tbl;
        break;
    case 0x2B:
        n = 2;
        tbl = set00_st43_pos_tbl;
        break;
    }
    for (i = 0; i < n; i++) {
        w->prim_no[i] = get_prim();
        if (w->prim_no[i] != -1) {
            pr = w->prim[i] = get_prim_ptr(w->prim_no[i]);
            pr->owner = sw;
            pr->no = i;
            pr->trans = set00_trans;
            pr->pos[0] = tbl[i][0];
            pr->pos[1] = tbl[i][1];
            pr->pos[2] = tbl[i][2];
        } else if (i == 0) {
            set00_e(sw);
        }
    }
}

static void set00_m(SETW *sw) {
    SET00_WORK *w = sw->u.work;
    s16 n, i;

    sw->timer++;
    switch (game_w.stage) {
    case 4:
        n = 2;
        break;
    case 8:
        n = 6;
        break;
    case 0x1A:
        n = 2;
        break;
    case 0x29:
        n = 1;
        break;
    case 0x2A:
        n = 2;
        break;
    case 0x2B:
        n = 2;
        break;
    }
    for (i = 0; i < n; i++) {
        if (w->prim[i] != 0) {
            add_prim(ot0, w->prim[i], 0x40, 0);
        }
    }
}

static void set00_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set00_e(SETW *sw) {
    push_set_work(sw);
}

static void set00_trans(PRIM *pr) {
    FLMAT mat;
    FLMAT uv;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;
    s16 z;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x6C, 0);
        flSetRenderState(0x60, 0);
        SetTrnslMode(4, 1);
        flmatCopy(&mat, &rview_matY);
        switch (game_w.stage) {
        case 4:
            cl = &mw->clay[1];
            flmatRotX33(&mat, DEG2RAD((f32)set00_st04_dir_tbl[pr->no]));
            break;
        case 8:
            cl = &mw->clay[2];
            flmatRotZ33(&mat, DEG2RAD((f32)set00_st08_dir_tbl[pr->no]));
            break;
        case 0x1A:
            flmatScaleFactor33(&mat, set00_st26_scale_tbl[pr->no], 1.0f, set00_st26_scale_tbl[pr->no]);
            cl = mw->clay;
            flmatRotX33(&mat, DEG2RAD((f32)set00_st26_dir_tbl[pr->no]));
            break;
        case 0x29:
            flmatScaleFactor33(&mat, set00_st41_scale_tbl[pr->no][0], set00_st41_scale_tbl[pr->no][1], set00_st41_scale_tbl[pr->no][2]);
            z = set00_st41_dir_tbl[pr->no][1];
            cl = &mw->clay[1];
            flmatRotX33(&mat, DEG2RAD((f32)set00_st41_dir_tbl[pr->no][0]));
            flmatRotZ33(&mat, DEG2RAD((f32)z));
            break;
        case 0x2A:
            flmatScaleFactor33(&mat, set00_st42_scale_tbl[pr->no], 1.0f, set00_st42_scale_tbl[pr->no]);
            cl = mw->clay;
            flmatRotZ33(&mat, DEG2RAD((f32)set00_st42_dir_tbl[pr->no]));
            break;
        case 0x2B:
            cl = mw->clay;
            flmatRotZ33(&mat, DEG2RAD((f32)set00_st43_dir_tbl[pr->no]));
            break;
        }
        flmatSetTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flSetRenderState(0x1A, (u32)&mat);
        flmatMakeTrans(&uv, 0.01f * (f32)((u16)sw->timer % 100), 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        if (cl->handle != -1) {
            flExecuteClay(cl->handle, 0);
        }
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}
