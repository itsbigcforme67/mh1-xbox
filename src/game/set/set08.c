/* set08 - game.bin 0x00620B70-0x006213F0. Stage floor tiles with scrolling
 * texture coordinates, drawn in two layers. On stages 0 and 26 a 8x10 grid of
 * 2000-unit tiles is culled against the view and each tile's model comes
 * from st00_obj_type0/1; elsewhere a 3x3 grid of 1000-unit tiles follows the
 * master player. */
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
    u8 _pad00[0x3C];
    SET_MDLW *mdl;      /* 0x3C stage model set */
} STAGE_WORK;

/* Culling sphere: centre and radius. The words around it are never
 * touched here; they only make the stack layout match. */
typedef struct SPHERE {
    s32 _00;
    f32 x, y, z, r;     /* 0x04 */
    s32 _14[2];
} SPHERE;

extern SET_MDLW *set_mdlw;
extern STAGE_WORK stage_work;
extern f32 uv_pos00_00677E40[16][2];
extern f32 uv_pos06[];
extern u16 st00_obj_type0[];
extern u16 st00_obj_type1[];
extern FLMAT view_mat;
extern u8 fov[];
extern u8 ot3[];

int flCheckMeshFOV(f32, f32 *, f32 *, FLMAT *, void *);
f32 flFloor(f32);
void SetFilterMode(int);

static void set08_move(SETW *sw);
static void set08_i(SETW *sw);
static void set08_m(SETW *sw);
static void set08_d(SETW *sw);
static void set08_e(SETW *sw);
static void set08_trans(PRIM *pr);

void set08_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 6;
        sw->arg = 0;
        sw->move = set08_move;
    }
}

static void set08_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set08_i(sw);
        break;
    case 1:
        set08_m(sw);
        break;
    case 2:
        set08_d(sw);
        break;
    case 3:
        set08_e(sw);
        break;
    }
}

static void set08_i(SETW *sw) {
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
        sw->prim->trans = set08_trans;
    }
}

static void set08_m(SETW *sw) {
    PLW *pl = &player_work[game_w.master];

    sw->timer++;
    sw->pos[0] = pl->pos[0];
    sw->pos[1] = 0.0f;
    sw->pos[2] = pl->pos[2];
    if (sw->work14 == 0) {
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        if (game_w.stage == 0 || game_w.stage == 26) {
            add_prim(ot3, sw->prim, 8, 1);
        } else if (game_w.stage == 6 || game_w.stage == 7) {
            add_prim(ot1, sw->prim, 0x20, 1);
        } else {
            add_prim(ot0, sw->prim, 0x40, 1);
        }
    }
}

static void set08_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set08_e(SETW *sw) {
    push_set_work(sw);
}

static void set08_trans(PRIM *pr) {
    f32 out[4];
    FLMAT mat;
    FLMAT uv;
    SPHERE sp;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    STAGE_WORK *stw = &stage_work;
    CLAY *cl;
    int layers, cols;
    int k, i, j;
    int rows;
    int n, t;
    f32 u;
    f32 v;
    f32 x;
    f32 z;
    f32 y;
    f32 w;

    if (mw != 0 && mw->flag != 0) {
        cl = mw->clay;
        if (game_w.stage == 0 || game_w.stage == 26) {
            flSetRenderState(0x60, 0);
        } else {
            flSetRenderState(0x60, 0x80);
        }
        switch (game_w.stage) {
        case 0:
        case 26:
            layers = 2;
            rows = 8;
            cols = 10;
            sp.r = sw->speed = 2000.0f;
            sp.y = 0.0f;
            break;
        case 6:
        case 7:
        default:
            layers = 2;
            rows = 3;
            cols = 3;
            sw->speed = 1000.0f;
            break;
        }
        for (k = 0; k < layers; k++, cl++) {
            switch (game_w.stage) {
            case 0:
            case 26:
                if (k == 0) {
                    u = 0.0f;
                    v = 10.0f * (0.0016666667f * (f32)((u16)sw->timer % 60));
                } else {
                    u = uv_pos00_00677E40[(sw->timer & 0x1E) >> 1][0];
                    v = uv_pos00_00677E40[(sw->timer & 0x1E) >> 1][1];
                }
                w = 0.0f;
                break;
            case 6:
            case 7:
            default:
                w = 0.0f;
                u = uv_pos06[((sw->timer & 0x1E) >> 1) * 4 + k * 2];
                v = uv_pos06[((sw->timer & 0x1E) >> 1) * 4 + k * 2 + 1];
                break;
            }
            u -= flFloor(u);
            v -= flFloor(v);
            flmatMakeTrans(&uv, u, v, w);
            flSetRenderState(0x19, (u32)&uv);
            switch (game_w.stage) {
            case 0:
            case 26:
                x = 0.0f;
                z = x;
                break;
            default:
                x = 1000.0f * flFloor((500.0f + sw->pos[0]) / 1000.0f) - 1000.0f;
                z = 1000.0f * flFloor((500.0f + sw->pos[2]) / 1000.0f) - 1000.0f;
                break;
            }
            if (game_w.stage == 0 || game_w.stage == 26) {
                y = 42.0f + 4.0f * (f32)k;
            } else {
                y = 0.0f;
            }
            for (i = 0; i < rows; i++) {
                f32 zz = z;

                for (j = 0; j < cols; j++) {
                    switch (game_w.stage) {
                    case 0:
                    case 26:
                        sp.x = x;
                        sp.z = zz;
                        if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                            flmatMakeTrans(&mat, x, y, zz);
                            flSetRenderState(0x1A, (u32)&mat);
                            n = (u16)(x / 2000.0f + 10.0f * (zz / 2000.0f));
                            if (k == 0) {
                                t = st00_obj_type0[n];
                            } else {
                                t = st00_obj_type1[n];
                            }
                            if (t != 0) {
                                cl = stw->mdl->clay + t + 3;
                                if (cl->handle != -1) {
                                    clay_attr_set(cl->attr);
                                    SetFilterMode(1);
                                    flExecuteClay(cl->handle, 0);
                                }
                            }
                        }
                        break;
                    default:
                        flmatMakeTrans(&mat, x, y, zz);
                        flSetRenderState(0x1A, (u32)&mat);
                        if (cl->handle != -1) {
                            clay_attr_set(cl->attr);
                            SetFilterMode(1);
                            flExecuteClay(cl->handle, 0);
                        }
                        break;
                    }
                    zz += sw->speed;
                }
                x += sw->speed;
            }
        }
        SetFilterMode(0);
        flSetRenderState(0x60, 0);
        clay_attr_reset();
    }
}
