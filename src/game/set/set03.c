/* set03 - game.bin 0x0061ED10-0x0061F2B8, split in two around set03_m
 * (see set03_nm.c). In the original all of these functions are static. Steam vents on stages 47-49:
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
void set03_m(SETW *sw);
void set03_d(SETW *sw);
void set03_e(SETW *sw);
void set03_trans(PRIM *pr);

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
