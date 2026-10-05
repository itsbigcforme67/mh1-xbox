#include "lobby_a.h"
typedef struct { u8 pad0000[0x18]; int x0018; } ARG_check_erase_dialog_arg0;

s32 check_erase_dialog(ARG_check_erase_dialog_arg0 *arg0) {
    u8 temp_v1;

    temp_v1 = F(u8, (u8 *)cw, 0x2F79);
    if (temp_v1 == F(u8, arg0->x0018, 0x14)) {
        return 1;
    }
    return temp_v1 == 0x4C;
}
