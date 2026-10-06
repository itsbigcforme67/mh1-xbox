#include "lobby_s.h"

void Lb_shop_trans_sub(int arg0) {
    if ((lbShop.x15 >= 5) && (lbShop.x84 == 0)) {
        if ((lbShop.x8E != 0) && (lbShop.x8E != 3)) {
            if (lbShop.x1C != 1) {
                goto block_6;
            }
        } else {
block_6:
            reload_tex(1, 0x157);
            SetTextureStage(0x157);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            font_set_stack_no(F(s32, arg0, 0x18));
            flfntSetSize(0x14, 0x14);
            font_set_palette(0);
            if (lbShop.f3C != 0) {
                lbShop.f3C();
            }
        }
    }
}
