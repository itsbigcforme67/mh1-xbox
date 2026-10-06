/* lb_crr01 - agent C 0x005B0A80-0x005B0B98: check_room_require (can this hunter enter the selected guild room: quest/rank requirement; qq = lb_quest_all[lv] hoisted before the switch). */
#include "lobby_b.h"
typedef struct { u8 pad[0x158]; s32 x158; } LBROOM;
extern LBROOM *Lbs_GetRoomInfo();
s32 check_room_require(void) {
    s32 lv;
    u8 *q;
    LBQUEST *qq;

    lv = ((u32)(Lbs_GetRoomInfo(lb_sys.x73)->x158 & 0x1FE) >> 1) & 0xFF;
    if (lv >= 0xC8) {
        q = (u8 *)get_quest_info();
        if (F(s8, cw, 0x2C2F) == 0 || q[0x1D] != lv) {
            lb_pit.x0 = 0;
            lb_pit.x08 = 4;
            return 1;
        }
    } else {
        qq = lb_quest_all[lv];
        switch (lv) {
        case 0x65:
            return *(u8 *)0x3C733B < 9;
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
            return *(u8 *)0x3C733B < 0x11;
        default:
            if (qq->_pad00[2] >= 4 && *(u8 *)0x3C733B < 0xD) {
                return 1;
            }
        }
    }
    return 0;
}
