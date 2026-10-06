/* lb_by60 - agent B promoted near-match 0x0053D720-0x0053D758: lb_armor_listItem (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

void lb_armor_listItem(int arg0, int arg1, int arg2, s16 arg3) {
    int p;

    p = (int)lbShop.tbl + (arg3 + lbShop.x6C * 7) * 8;
    Lb_put_armorIcon(arg0, arg1, arg2, *(s16 *)p, *(s16 *)(p + 4));
}
