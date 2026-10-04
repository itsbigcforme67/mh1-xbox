/* set03 - game.bin 0x0061ED10-0x0061F2B8, part 2 (after set03_m). In the original all of these functions are static. Steam vents on stages 47-49:
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

void set03_d(SETW *sw);
void set03_e(SETW *sw);
void set03_trans(PRIM *pr);

void set03_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

void set03_e(SETW *sw) {
    push_set_work(sw);
}

void set03_trans(PRIM *pr) {
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
