/* lb_z132 - auto-drafted 0x00609300-0x0060938C: lineBuf_print_t (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void lineBuf_print_t(int arg0) {
    int temp_a0;
    int temp_a1;
    int temp_v1;

    temp_a1 = bsw;
    if (F(s32, temp_a1, 0x1C) != 0) {
        F(s16, temp_a1, 0xD8C8) = (s16) (F(u16, temp_a1, 0xD8D2) - F(u16, temp_a1, 0xD8CE));
        temp_v1 = bsw;
        F(s8, (F(s32, temp_v1, 0x1C) + temp_v1), 0x20) = 0;
        Disp_Text_t(arg0, temp_a1);
        temp_a0 = bsw;
        F(u16, temp_a0, 0xD8CE) = (u16) F(u16, temp_a0, 0xD8D2);
        F(s32, bsw, 0x1C) = 0;
        F(s8, bsw, 0x18D) = 0;
    }
}
