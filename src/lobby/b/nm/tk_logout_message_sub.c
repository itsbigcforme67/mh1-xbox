#include "lobby_a.h"

void tk_logout_message_sub(s32 arg0, s32 arg1) {
    s32 temp_a0;
    s32 temp_a0_2;

    if (arg0 == 0) {
        temp_a0 = arg1 & 0xFF;
        if ((temp_a0 != 5) && (temp_a0 != 3) && (temp_a0 != 1)) {

        }
    } else {
        temp_a0_2 = arg1 & 0xFF;
        if ((temp_a0_2 != 5) && (temp_a0_2 != 3) && (temp_a0_2 != 1)) {

        }
    }
}
