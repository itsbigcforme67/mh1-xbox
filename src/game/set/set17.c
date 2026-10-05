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

static void set17_move(SETW *sw);
static void set17_i(SETW *sw);
static void set17_m(SETW *sw);
static void set17_d(SETW *sw);
static void set17_e(SETW *sw);
void set17_trans(PRIM *pr);

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
