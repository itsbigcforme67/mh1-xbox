/* lb_by139 - agent B 0x00537860-0x00537ACC: Lb_shop_trans2 (shop menu draw: list / item window / equip compare by lbShop.x84, help, tags). */
#include "lobby_s.h"
extern u8 shop_tex_tbl[];
void ItemboxWindowX(f32 x, s16 y, int flags);

void Lb_shop_trans2(int arg0) {
    u16 v;

    if (*(u8 *)&lbShop.x1B != 0) {
        font_set_stack_no(F(s32, arg0, 0x18));
        flfntSetSize(0x14, 0x14);
        font_set_palette(0);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        if (lbShop.x15 >= 5) {
            switch (lbShop.x84) {
            case 0:
                if (lbShop.x8E == 0 || lbShop.x8E == 3 || lbShop.x1C != 1) {
                    if (lbShop.x6D != 0 || lbShop.x8E == 3) {
                        lb_put_shopList();
                        if (lbShop.x6D != 0) {
                            Put_page_num(0x20E, 0x38, lbShop.x6C, lbShop.x6D, 1);
                        }
                    }
                } else {
                    if (lbShop.f3C != 0) {
                        lbShop.f3C();
                    }
                }
                Lb_put_gold();
                break;
            case 1:
                if (lbShop.mode == 0 && lbShop.x1A == 1) {
                    v = 0x100;
                } else {
                    v = 0xC0;
                }
                if (lbShop.x1C != 0) {
                    ItemboxWindowX(292.0f, lbShop.x70, v | 0xC | (lbShop.x6E & 3) | 0x10);
                } else {
                    ItemboxWindowX(292.0f, lbShop.x70, v | 8);
                }
                break;
            case 2:
                Lb_put_gold();
                flfntSetSize(0x12, 0x12);
                EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C, 0x80);
                break;
            }
            lb_put_shopHelp();
        }
        if (lbShop.x84 != 1) {
            lb_put_mk_tags();
            if (lbShop.x15 < 5 && lbShop.x15 < 3) {
                Lb_put_2TF(&shop_tex_tbl[0x28], 1);
            }
        }
    }
}
