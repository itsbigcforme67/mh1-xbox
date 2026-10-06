/* lb_by44 - agent B promoted near-match 0x00538320-0x005383CC: Lb_put_shopYesNo (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lit_869_00654E00[];
extern char lit_870_00654E10[];
extern char lit_871_00654E28[];
extern char lit_872_00654E40[];

void Lb_put_shopYesNo(void) {
    if (F(s8, &lbShop, 0x78) == 0) {
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x1B0, 0x194);
        font_set_palette(2);
        font_print(&lit_869_00654E00);
        font_set_palette(0);
        font_print(&lit_870_00654E10);
        return;
    }
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x1B0, 0x194);
    font_set_palette(0);
    font_print(&lit_871_00654E28);
    font_set_palette(2);
    font_print(&lit_872_00654E40);
}
