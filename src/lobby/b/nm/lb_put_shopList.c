#include "lobby_s.h"
extern char lit_836_00654DD0[];
extern char lit_837_00654DF0[];
extern char lit_838_00654DF8[];
extern char lit_837_00654DF0[];
void lb_put_shopList(void) {
    int sp50;
    s16 temp_v1;
    int var_s0;
    int var_s1;
    int var_s2;
    int var_s3;

    var_s3 = (int)lbShop.list + (lbShop.x6C * 0x118);
    Draw_menu_square(0x118, 0x30, 0x152, 0x122);
    if (lbShop.count == 0) {
        if (lbShop.x8E == 3) {
            flfntSetSize(0x14, 0x14);
            font_print_double(0x140, 0x96, 1, 4);
        }
        return;
    }
    Lb_put_shopCursor();
    var_s2 = 0;
    var_s1 = 0x50;
    var_s0 = 0x4E;
loop_5:
    if ((var_s2 + (lbShop.x6C * 7)) < lbShop.count) {
        temp_v1 = F(s16, var_s3, 0x24);
        switch (temp_v1) {                          /* irregular */
        case 0:
            font_set_palette(0, lbShop.x6C);
            break;
        case 3:
            font_set_palette(6, lbShop.x6C);
            break;
        case 4:
            font_set_palette(6, lbShop.x6C);
            break;
        default:
            font_set_palette(0xA, lbShop.x6C);
            break;
        }
        if (F(s16, var_s3, 0x24) == 2) {
            flfntSetSize(0x14, 0x14);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x122,  (var_s1 << 0x30) >> 0x30);
            font_print(&lit_836_00654DD0);
        } else {
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x13C,  (var_s1 << 0x30) >> 0x30);
            font_print(&lit_837_00654DF0, var_s3 + 4);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x208,  (var_s1 << 0x30) >> 0x30);
            sprintf(&sp50, &lit_838_00654DF8, F(s32, var_s3, 0));
            font_print(&lit_837_00654DF0, &sp50);
            if (lbShop.f44 != 0) {
                lbShop.f44(0x11C,  (var_s0 << 0x30) >> 0x30, 0x18,  (var_s2 << 0x30) >> 0x30);
            }
        }
        var_s2 += 1;
        var_s3 += 0x28;
        var_s1 += 0x18;
        var_s0 += 0x18;
        if (var_s2 < 7) {
            goto loop_5;
        }
    }
}
