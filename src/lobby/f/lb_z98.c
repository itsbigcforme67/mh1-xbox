/* lb_z98 - auto-drafted 0x00605A40-0x00605A7C: set_TH_TD_position (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void set_TH_TD_position(u8 *arg0, u8 *arg1) {
    F(s16, arg0, 0x28) = (s16) (F(u16, arg0, 0x30) + (F(u16, arg1, 0x28) + F(u16, arg1, 0x2C)));
    F(s16, arg0, 0x2A) = (s16) (F(u16, arg0, 0x30) + (F(u16, arg1, 0x2A) + F(u16, arg1, 0x2E)));
    F(s16, arg0, 0x2C) = 0;
    F(s16, arg0, 0x2E) = 0;
}
