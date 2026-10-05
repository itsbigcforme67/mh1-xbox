/* lb_z100 - auto-drafted 0x00609750-0x00609770: Lb_ItemBox_init (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * ib;
extern char item_box[];

void Lb_ItemBox_init(void) {
    ib = (u8 *)&item_box;
    F(s32, ib, 4) = 0;
    F(s16, ib, 2) = 0;
}
