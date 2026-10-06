/* lb_f03 - lobby 0x005CCFF0-0x005CD084: Lb_put_gold (gold counter box). Whole file in lb_f.c.
   The sprite descriptor is a local initializer: the compiler emits it as a 20-byte .data object (slot lobby:data 0x0064E180, was lit_693_0064E180)
   and copies it with lq/lwc1 through a pointer register. IBICON = {x, y, w, h, colour, u0, v0, u1, v1} as read by flps0008 / Put_2TF. */
#include "lobby_f.h"
typedef struct IBICON { s16 x, y, w, h; s32 color; s16 u0, v0, u1, v1; } IBICON;

void Lb_put_gold(void) {
    IBICON t = { 460, 28, 140, 22, 0x80FFFFFF, 136, 48, 256, 72 };
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&t);
    flfntSetSize(0x1C, 0x14);
    font_set_palette(0);
    font_print_ex(0x1DA, 0x1E, 0, lit_695_00664CB8, *(s32 *)0x3C6FE0);
}
