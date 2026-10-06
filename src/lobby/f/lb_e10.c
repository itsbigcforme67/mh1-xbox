/* lb_e10 - room members list 0x005CB220-0x005CB308: lb_put_room_member_005CB220. Whole file in lb_e.c. */
#include "lobby_f.h"
extern char lit_249_00664AE0[];
void lb_put_room_member_005CB220(void) {
    u32 i;
    int y;
    int off;
    u8 *p;
    if (Online_ck() == 1) {
        y = 0xA2;
        i = 0;
        off = 0;
        do {
            if (*(s8 *)(cw + off + 0x73C) != 0) {
                flfntLocate(0x179, y);
                p = cw + off;
                font_print(lit_249_00664AE0, p + 0x73C, p + 0x744);
            }
            y = (s16)(y + 0x28);
            off += 0x2FC;
            i += 1;
        } while (i < 4U);
        return;
    }
    flfntLocate(0x179, 0xA2);
    p = (u8 *)&lb_player[game_w.master];
    font_print(lit_249_00664AE0, p + 0x24, p + 4);
}
