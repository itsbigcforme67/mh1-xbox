/* set18 - game.bin 0x00625BB0-0x006263B0. Ground models with a scrolling
 * texture: a 6x6 grid of 1000-unit tiles on stage 24, and four placed
 * models (per-stage position tables) on stages 18, 22, 23 and 70. Each
 * one is culled against the view before drawing. */
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

/* Culling sphere: centre and radius. The words around it are never
 * touched here; they only make the stack layout match (as in set08). */
typedef struct SPHERE {
    s32 _00;
    f32 x, y, z, r;     /* 0x04 */
    s32 _14[2];
} SPHERE;

extern SET_MDLW *set_mdlw;
extern f32 uv_pos09[16][2];
extern f32 set18_st18_pos_tbl[4][2];
extern f32 set18_st22_pos_tbl[4][2];
extern f32 set18_st23_pos_tbl[4][2];
extern f32 set18_st70_pos_tbl[4][2];
extern FLMAT view_mat;
extern FLMAT rview_mat;
extern u8 fov[];

void Create_FOV(f32, int);
void reload_tex(int, int);
int flCheckMeshFOV(f32, f32 *, f32 *, FLMAT *, void *);
void flmatGetTrans(f32 *, FLMAT *);

static void set18_move(SETW *sw);
static void set18_i(SETW *sw);
static void set18_m(SETW *sw);
static void set18_d(SETW *sw);
static void set18_e(SETW *sw);
static void set18_trans(PRIM *pr);

void Set18_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 18;
        sw->arg = 0;
        sw->move = set18_move;
    }
}

static void set18_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set18_i(sw);
        break;
    case 1:
        set18_m(sw);
        break;
    case 2:
        set18_d(sw);
        break;
    case 3:
        set18_e(sw);
        break;
    }
}

static void set18_i(SETW *sw) {
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
        sw->prim->trans = set18_trans;
        return;
    }
    push_set_work(sw);
}

static void set18_m(SETW *sw) {
    sw->timer++;
    switch (game_w.stage) {
    case 0x18:
        flmatGetTrans(sw->prim->pos, &rview_mat);
        add_prim(ot1, sw->prim, 0x20, 1);
        break;
    case 0x12:
    case 0x17:
    case 0x46:
        add_prim(ot1, sw->prim, 0x20, 1);
        break;
    }
}

static void set18_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set18_e(SETW *sw) {
    push_set_work(sw);
}

static void set18_trans(PRIM *pr) {
    f32 out[4];
    FLMAT mat;
    FLMAT uv;
    SPHERE sp;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    s16 i, j;
    f32 v, x0, step, x, z;

    if (mw != 0 && mw->flag != 0) {
        switch (game_w.stage) {
        case 0x18:
            cl = mw->clay + 3;
            flSetRenderState(0x60, 0x80);
            Create_FOV(1450.0f, 0);
            sp.r = 720.0f;
            sp.y = 0.0f;
            reload_tex(0x10, 0x12D);
            x = uv_pos09[(sw->timer & 0x1E) >> 1][0];
            v = uv_pos09[(sw->timer & 0x1E) >> 1][1];
            x0 = 7500.0f;
            step = 1000.0f;
            z = x0;
            break;
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x46:
            cl = mw->clay;
            flSetRenderState(0x60, 0x80);
            Create_FOV(1450.0f, 0);
            sp.r = 720.0f;
            sp.y = 0.0f;
            reload_tex(0x10, 0x12D);
            x = 0.0f;
            v = 1.0f - (f32)(sw->timer & 0x3F) / 64.0f;
            break;
        }
        flmatMakeTrans(&uv, x, v, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        switch (game_w.stage) {
        case 0x18:
            for (i = 0; i < 6; i++) {
                x = x0;
                for (j = 0; j < 6; j++) {
                    sp.x = x;
                    sp.z = z;
                    if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                        flmatMakeTrans(&mat, x, 0.0f, z);
                        flSetRenderState(0x1A, (u32)&mat);
                        clay_attr_set(cl->attr);
                        flExecuteClay(cl->handle, 0);
                    }
                    x += step;
                }
                z += step;
            }
            flSetRenderState(0x60, 0x80);
            break;
        case 0x12:
            for (i = 0; i < 4; i++) {
                /* x and z swap roles here only; that is what gives the
                 * original's register choice for this case. */
                z = set18_st18_pos_tbl[i][0];
                x = set18_st18_pos_tbl[i][1];
                sp.x = z;
                sp.z = x;
                if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                    flmatMakeTrans(&mat, z, 0.0f, x);
                    flSetRenderState(0x1A, (u32)&mat);
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
            }
            break;
        case 0x16:
            for (i = 0; i < 4; i++) {
                x = set18_st22_pos_tbl[i][0];
                z = set18_st22_pos_tbl[i][1];
                sp.x = x;
                sp.z = z;
                if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                    flmatMakeTrans(&mat, x, 0.0f, z);
                    flSetRenderState(0x1A, (u32)&mat);
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
            }
            break;
        case 0x17:
            for (i = 0; i < 4; i++) {
                x = set18_st23_pos_tbl[i][0];
                z = set18_st23_pos_tbl[i][1];
                sp.x = x;
                sp.z = z;
                if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                    flmatMakeTrans(&mat, x, 0.0f, z);
                    flSetRenderState(0x1A, (u32)&mat);
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
            }
            break;
        case 0x46:
            for (i = 0; i < 4; i++) {
                x = set18_st70_pos_tbl[i][0];
                z = set18_st70_pos_tbl[i][1];
                sp.x = x;
                sp.z = z;
                if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                    flmatMakeTrans(&mat, x, 0.0f, z);
                    flSetRenderState(0x1A, (u32)&mat);
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
            }
            break;
        }
    }
}
