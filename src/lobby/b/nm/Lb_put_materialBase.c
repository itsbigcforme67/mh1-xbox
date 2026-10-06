#include "lobby_s.h"
extern char lit_1225_006555D0[];
void Lb_put_materialBase(void) {
    s32 temp_s0;

    temp_s0 = (int)lbShop.list + (lbShop.cur * 0x28);
    Paint_square(0x120, 0x4C, 0x140, 0xFC);
    Draw_menu_square(0x120, 0xC4, 0x140, 0x84);
    flfntSetSize(0x14, 0x14);
    flfntLocate(0x160, 0xD6);
    font_print(&lit_1225_006555D0, temp_s0 + 4);
}
