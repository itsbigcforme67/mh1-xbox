/* set17_trans near-match, NOT built (67/218 instructions differ: float constant
 * load order and register numbering of rows/cols/step/z). The rest of set17 is
 * set17.c (matching). */
/* set17 - game.bin 0x00625630-0x00625BA8. Stage terrain tiles: a grid of
 * parts (per-stage id tables) on stages 1, 2, 3 and 46, each culled against
 * the view before drawing. Follows the master player like set16. */
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

/* Culling sphere: centre and radius. The words around it are never
 * touched here; they only make the stack layout match (as in set08). */
typedef struct SPHERE {
    s32 _00;
    f32 x, y, z, r;     /* 0x04 */
    s32 _14[2];
} SPHERE;

extern SET_MDLW *set_mdlw;
extern u8 st01_parts_id_tbl[4];
extern u8 st02_parts_id_tbl[20];
extern u8 st03_parts_id_tbl[12];
extern u8 st46_parts_id_tbl[30];
extern FLMAT view_mat;
extern u8 fov[];
extern u8 ot3[];

void Create_FOV(f32, int);
void reload_tex(int, int);
int flCheckMeshFOV(f32, f32 *, f32 *, FLMAT *, void *);

void set17_trans(PRIM *pr);
static void set17_move(SETW *sw);
static void set17_i(SETW *sw);
static void set17_m(SETW *sw);
static void set17_d(SETW *sw);
static void set17_e(SETW *sw);

void set17_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 17;
        sw->arg = 0;
        sw->move = set17_move;
    }
}

static void set17_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set17_i(sw);
        break;
    case 1:
        set17_m(sw);
        break;
    case 2:
        set17_d(sw);
        break;
    case 3:
        set17_e(sw);
        break;
    }
}

static void set17_i(SETW *sw) {
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
        sw->prim->trans = set17_trans;
        return;
    }
    push_set_work(sw);
}

static void set17_m(SETW *sw) {
    PLW *pl = &player_work[game_w.master];

    sw->timer++;
    sw->pos[0] = pl->pos[0];
    sw->pos[1] = 0.0f;
    sw->pos[2] = pl->pos[2];
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot3, sw->prim, 8, 1);
}

static void set17_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set17_e(SETW *sw) {
    push_set_work(sw);
}

void set17_trans(PRIM *pr) {
    f32 out[4];
    FLMAT mat;
    SPHERE sp;
    u8 id;
    SET_MDLW * mw = set_mdlw;
    CLAY * cl;
    int base;
    s16 rows;
    s16 cols;
    s16 i;
    s16 j;
    s16 n;
    u8 * tbl;
    f32 z;
    f32 step;
    f32 x0;
    f32 x;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0x80);
        switch (game_w.stage) {
        case 1:
            x0 = 8000.0f;
            rows = 1;
            cols = 4;
            tbl = st01_parts_id_tbl;
            sp.r = 2828.0f;
            z = 10000.0f;
            step = 2000.0f;
            Create_FOV(1500.0f, 0);
            break;
        case 2:
            rows = 5;
            x0 = 7000.0f;
            cols = 4;
            z = 6000.0f;
            step = 2000.0f;
            sp.r = 2828.0f;
            tbl = st02_parts_id_tbl;
            Create_FOV(1100.0f, 0);
            break;
        case 3:
            x0 = 8000.0f;
            cols = 3;
            rows = 4;
            sp.r = 2828.0f;
            z = 6000.0f;
            tbl = st03_parts_id_tbl;
            step = 2000.0f;
            Create_FOV(1500.0f, 0);
            break;
        case 0x2E:
            z = 6000.0f;
            x0 = 4000.0f;
            rows = 6;
            step = 2000.0f;
            cols = 5;
            tbl = st46_parts_id_tbl;
            sp.r = 2828.0f;
            Create_FOV(1100.0f, 0);
            break;
        }
        sp.y = 0.0f;
        reload_tex(0x10, 0x12D);
        base = 0;
        for (i = 0; i < rows; i++) {
            x = x0;
            for (j = 0; j < cols; j++) {
                sp.x = x;
                sp.z = z;
                n = j + base;
                if (flCheckMeshFOV(sp.r, &sp.x, out, &view_mat, fov) != 0) {
                    id = tbl[n];
                    if (id != 0xFF) {
                        cl = &mw->clay[id & 0xF];
                        flmatMakeTrans(&mat, x, 0.0f, z);
                        flSetRenderState(0x1A, (u32)&mat);
                        clay_attr_set(cl->attr);
                        flExecuteClay(cl->handle, 0);
                    }
                }
                x += step;
            }
            base += cols;
            z += step;
        }
    }
}
