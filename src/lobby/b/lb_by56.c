/* lb_by56 - agent B promoted near-match 0x0053BF30-0x0053BFCC: Lb_put_materialBase (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char lit_1225_006555D0[];

void Lb_put_materialBase(void) {
    LB_SHOPITEM *it;

    it = (LB_SHOPITEM *)lbShop.list + lbShop.cur;
    Paint_square(0x120, 0x4C, 0x140, 0xFC, 0xA0202020);
    Draw_menu_square(0x120, 0xC4, 0x140, 0x84, 0, 0);
    flfntSetSize(0x14, 0x14);
    flfntLocate(0x160, 0xD6);
    font_print(lit_1225_006555D0, it->name);
}
