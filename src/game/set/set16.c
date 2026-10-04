/*
 * set16 - game.bin 0x00625120-0x0062562C. The sky/backdrop model: follows
 * the master player (stages 0, 26) or sits far out along the camera
 * (stages 6, 7), with a scrolling texture. */
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

typedef struct STAGE_WORK {
    u8 _pad00[8];
    s16 timer;          /* 0x08 */
    u8 _pad0A[0x3C - 0x0A];
    SET_MDLW *mdl;      /* 0x3C stage model set */
    u8 _pad40[0x64 - 0x40];
} STAGE_WORK;

extern SET_MDLW *set_mdlw;
extern STAGE_WORK stage_work;
extern s32 mdl_tbl_006784E0[4];
extern FLMAT rview_mat;
extern u8 ot3[];

void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flFloor(f32);
void SetFilterMode(int);

static void set16_move(SETW *sw);
static void set16_i(SETW *sw);
static void set16_m(SETW *sw);
static void set16_d(SETW *sw);
static void set16_e(SETW *sw);
static void set16_trans(PRIM *pr);

void set16_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 6;
        sw->arg = 0;
        sw->move = set16_move;
    }
}

static void set16_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set16_i(sw);
        break;
    case 1:
        set16_m(sw);
        break;
    case 2:
        set16_d(sw);
        break;
    case 3:
        set16_e(sw);
        break;
    }
}

static void set16_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->cnt = 0;
    sw->pos[0] = 0.0f;
    sw->pos[1] = 0.0f;
    sw->pos[2] = 0.0f;
    sw->timer = 0;
    sw->work14 = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set16_trans;
    }
}

static void set16_m(SETW *sw) {
    u8 no = game_w.master;
    PLW *pl = &player_work[no];
    f32 v[3];

    sw->timer++;
    switch (game_w.stage) {
    case 0:
    case 0x1A:
        sw->pos[0] = pl->pos[0];
        sw->pos[1] = 0.0f;
        sw->pos[2] = pl->pos[2];
        break;
    case 6:
        case 7:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 18000.0f;
        flvecApplyMat33_2(v, &rview_mat);
        sw->pos[0] = 10000.0f + v[0];
        sw->pos[1] = v[1];
        sw->pos[2] = 10000.0f + v[2];
        break;
    }
    if (sw->work14 == 0) {
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        add_prim(ot3, sw->prim, 8, 1);
    }
}

static void set16_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set16_e(SETW *sw) {
    push_set_work(sw);
}

static void set16_trans(PRIM *pr) {
    FLMAT mat, uv;
    STAGE_WORK *stw = &stage_work;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    f32 u, v;
    int i;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0);
        switch (game_w.stage) {
        case 0:
        case 0x1A:
            /* A one-pass loop over the model table: the compiler unrolls it but
             * keeps &uv, &mat and the table pointer in saved registers,
             * which is what the original does. */
            for (i = 0; i < 1; i++) {
                u = 0.0f;
                v = 1.0f - (1.0f / 30.0f) * stw->timer;
                u -= flFloor(u);
                v -= flFloor(v);
                flmatMakeTrans(&uv, u, v, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                flmatMakeTrans(&mat, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x1A, (u32)&mat);
                cl = &stw->mdl->clay[mdl_tbl_006784E0[i]];
                if (cl->handle != -1) {
                    clay_attr_set(cl->attr);
                    SetFilterMode(1);
                    flExecuteClay(cl->handle, 0);
                }
            }
            break;
        case 6:
        case 7:
            cl = &stw->mdl->clay[2];
            flmatMakeTrans(&uv, (1.0f / 256.0f) * (int)(u8)sw->timer, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            flmatMakeTrans(&mat, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x1A, (u32)&mat);
            if (cl->handle != -1) {
                clay_attr_set(cl->attr);
                flExecuteClay(cl->handle, 0);
            }
            break;
        }
        SetFilterMode(0);
        clay_attr_reset();
    }
}
