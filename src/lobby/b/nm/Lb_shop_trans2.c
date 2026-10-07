#include "lobby_s.h"
extern u8 shop_tex_tbl[];

void Lb_shop_trans2(int arg0) {
    s32 var_v1;

    if (lbShop.x1B != 0) {
        font_set_stack_no(F(s32, arg0, 0x18));
        flfntSetSize(0x14, 0x14);
        font_set_palette(0);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        if (lbShop.x15 >= 5) {
            switch (lbShop.x84) { /* irregular */
            case 0:
                if ((lbShop.x8E != 0) && (lbShop.x8E != 3)) {
                    if (lbShop.x1C != 1) {
                        goto block_10;
                    }
                    if (lbShop.f3C != 0) {
                        lbShop.f3C(lbShop.x8E);
                    }
                } else {
block_10:
                    if ((lbShop.x6D != 0) || (lbShop.x8E == 3)) {
                        lb_put_shopList(lbShop.x8E);
                        if (lbShop.x6D != 0) {
                            Put_page_num(0x20E, 0x38, lbShop.x6C, lbShop.x6D);
                        }
                    }
                }
                Lb_put_gold();
                break;
            case 1:
                if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
                    var_v1 = 0x100;
                } else {
                    var_v1 = 0xC0;
                }
                if (lbShop.x1C != 0) {
                    ItemboxWindowX(0x43920000, lbShop.x70, (var_v1 & 0xFFFF) | 0xC | (lbShop.x6E & 3) | 0x10);
                } else {
                    ItemboxWindowX(0x43920000, lbShop.x70, (var_v1 & 0xFFFF) | 8);
                }
                break;
            case 2:
                Lb_put_gold(lbShop.x84);
                flfntSetSize(0x12, 0x12);
                EquipmentCompareWindow((int)&lbShop + 0x54, (int)&lbShop + 0x5A, 0x126, 0x3C, 0x80);   /* as the matched lb_by139 */
                break;
            }
            lb_put_shopHelp();
        }
        if (lbShop.x84 != 1) {
            lb_put_mk_tags(lbShop.x84);
            if ((lbShop.x15 < 5) && (lbShop.x15 < 3)) {
                Lb_put_2TF((int)&shop_tex_tbl + 0x28, 1);
            }
        }
    }
}
