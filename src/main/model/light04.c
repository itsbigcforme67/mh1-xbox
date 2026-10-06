/* light04 - light_move (SLPM_654.95 0x0011DD60-0x0011DE60): per frame for the two light sets of light_work (0x140 bytes each): runs the flash
   effect (type 1) and, for the second set (not on stage 0x11), turns the direction (0,0,-1) by the view matrix into its light direction at +0x11C.
   Written new in this pass; field meanings are guesses. */
#include "types.h"
#include "game.h"
extern GAME_W game_w;
extern u8 light_work[];
extern f32 rview_mat[];
void flash_move(void *);
void flvecApplyMat33(f32 *, s32 *, f32 *);
void light_move(void) {
    u8 *w;
    u8 *p;
    u8 *q;
    int i;
    s32 in[3];
    f32 out[3];

    i = 0;
    w = light_work;
    do {
        p = w + 0x10;
        switch (w[0x11]) {
        case 0:
            break;
        case 1:
            flash_move(p);
            break;
        }
        if (i == 1 && game_w.stage != 0x11) {
            in[0] = 0;
            in[1] = 0;
            in[2] = 0xBF800000;
            q = p + 0xD8;
            flvecApplyMat33(out, in, rview_mat);
            *(f32 *)(q + 0x34) = out[0];
            *(f32 *)(q + 0x38) = out[1];
            *(f32 *)(q + 0x3C) = out[2];
        }
        i++;
        w += 0x140;
    } while (i < 2);
}
