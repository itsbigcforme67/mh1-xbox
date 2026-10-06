#include "lobby_a.h"

s32 lobby_80(void) {
    all_reset();
    F(s8, &lb_sys, 0x70) = 0;
    F(s8, &lb_sys, 2) = 0;
    F(s8, &lb_sys, 1) = (s8) (F(s8, &lb_sys, 1) + 1);
    return 0;
}
