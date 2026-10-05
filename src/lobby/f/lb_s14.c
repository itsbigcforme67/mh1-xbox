/* lb_s14 - browser tag handlers 2 0x006045B0-0x00604664: tagoutprintf_cr (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagoutprintf_cr(s32 arg0) {
    s8 temp_v1;
    u16 temp_v1_2;
    u8 *temp_a0;
    int temp_v0;

    temp_a0 = bsw;
    temp_v1 = F(s8, temp_a0, 0x186);
    if (temp_v1 == -0xA) {
        temp_v1_2 = F(u16, temp_a0, 0xD8C0);
        if (F(u16, temp_a0, 0x182) < temp_v1_2) {
            F(u16, temp_a0, 0x182) = temp_v1_2;
        }
    } else if (temp_v1 == 1) {
        F(u16, (*(int *)arg0), 0) = (u16) F(u16, temp_a0, 0xD8C0);
        temp_v0 = (*(int *)arg0);
        if (F(u16, temp_v0, 2) == 0) {
            F(u16, temp_v0, 2) = (u16) F(u8, bsw, 0x180);
        }
    }
    pos_cr((bsw + 0xD8BC), arg0);
    return ck_line_no_add2(arg0);
}
