/* lb_s26 - small browser/http fixes 0x00605A80-0x00605B58: set_TH_TD_disp_data (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void set_TH_TD_disp_data(s32 arg0, s32 arg1) {
    int temp_a1;

    F(u16, bsw, 0xD8D6) = (u16) F(u16, arg0, 0x28);
    F(u16, bsw, 0xD8D8) = (u16) F(u16, arg0, 0x2A);
    F(s16, bsw, 0xD8D2) = 0;
    F(s16, bsw, 0xD8CE) = 0;
    F(s16, bsw, 0xD8D4) = 0;
    F(s16, bsw, 0xD8D0) = 0;
    temp_a1 = F(u16, arg0, 0x1C);
    F(u16, bsw, 0xD8DC) = temp_a1;
    F(s8, bsw, 0xD8DE) = 0;
    F(s16, bsw, 0x16) = 0;
    F(s16, arg0, 0x3A) = 0;
    F(s16, arg0, 0x3C) = 0;
    if (F(s8, bsw, 0x186) == 0) {
        set_valign_data(arg0, temp_a1);
        set_align_data(arg0);
    }
}
