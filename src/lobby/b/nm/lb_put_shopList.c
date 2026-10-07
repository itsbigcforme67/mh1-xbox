#include "lobby_s.h"
extern char lit_836_00654DD0[];
extern char lit_837_00654DF0[];
extern char lit_838_00654DF8[];
extern char lit_837_00654DF0[];
extern char lit_835_00654DB0[];
void lb_put_shopList(void) {
    char buf[0x20];
    LB_SHOPITEM *it;
    int i;
    int y1;
    int y0;

    it = (LB_SHOPITEM *)((u8 *)lbShop.list + lbShop.x6C * 0x118);
    Draw_menu_square(0x118, 0x30, 0x152, 0x122, 0, 0);
    if (lbShop.count == 0) {
        if (lbShop.x8E == 3) {
            flfntSetSize(0x14, 0x14);
            font_print_double(0x140, 0x96, 1, 4, lit_835_00654DB0);
        }
        return;
    }
    Lb_put_shopCursor();
    y1 = 0x50;
    y0 = 0x4E;
    for (i = 0; i < 7; i++, it++, y1 += 0x18, y0 += 0x18) {
        if (i + lbShop.x6C * 7 >= lbShop.count) {
            break;
        }
        if (it->state == 0) {
            font_set_palette(0);
        } else if (it->state == 3) {
            font_set_palette(6);
        } else if (it->state == 4) {
            font_set_palette(6);
        } else {
            font_set_palette(0xA);
        }
        if (it->state == 2) {
            flfntSetSize(0x14, 0x14);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x122, (s16)y1);
            font_print(&lit_836_00654DD0);
        } else {
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x13C, (s16)y1);
            font_print(&lit_837_00654DF0, it->name);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0x208, (s16)y1);
            sprintf(buf, &lit_838_00654DF8, it->price);
            font_print(&lit_837_00654DF0, buf);
            if (lbShop.f44 != 0) {
                lbShop.f44(0x11C, (s16)y0, 0x18, (s16)i);
            }
        }
    }
}
