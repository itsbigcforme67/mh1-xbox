/* lb_z130 - auto-drafted 0x00604200-0x0060423C: set_1byte_lineBuf_nbsp (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void set_1byte_lineBuf_nbsp(int arg0, int arg1) {
    int temp_a0;
    int temp_a2;

    temp_a2 = bsw;
    F(s8, (F(s32, temp_a2, 0x1C) + temp_a2), 0x20) = 0x20;
    temp_a0 = bsw;
    F(s32, temp_a0, 0x1C) = (F(s32, temp_a0, 0x1C) + 1);
    F(u16, arg1, 4) = (u16) (F(u16, arg1, 4) + F(u8, bsw, 0x181));
}
