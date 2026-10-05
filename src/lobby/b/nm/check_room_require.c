#include "lobby_f.h"
extern char lb_pit[];
extern char lb_pit[];
s32 check_room_require(void) {
    s32 temp_s0;
    void *temp_v0;

    temp_s0 = ((u32) (F(s32, Lbs_GetRoomInfo(F(u8, &lb_sys, 0x73)), 0x158) & 0x1FE) >> 1) & 0xFF;
    if (temp_s0 >= 0xC8) {
        temp_v0 = get_quest_info();
        if ((F(s8, (u8 *)cw, 0x2C2F) == 0) || (F(u8, temp_v0, 0x1D) != temp_s0)) {
            F(s32, &lb_pit, 0) = 0;
            F(s8, &lb_pit, 8) = 4;
            return 1;
        }
        goto block_13;
    }
    switch (temp_s0) {
    case 0x65:
        return *(u8 *)0x3C733B < 9;
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
        return *(u8 *)0x3C733B < 0x11;
    default:
        if ((F(u8, *((u8 *)&lb_quest_all + (temp_s0 * 4)), 2) >= 4) && (*(u8 *)0x3C733B < 0xD)) {
            return 1;
        }
block_13:
        return 0;
    }
}
