#include "lobby_a.h"
typedef struct { s16 x0000; u8 x0002; u8 x0003; s8 x0004; u8 pad0005[0x3]; u8 x0008; } ARG_id_select_01_arg0;

void id_select_01(ARG_id_select_01_arg0 *arg0) {
    s32 temp_a1;
    s32 temp_a1_2;
    u8 temp_a0;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 var_v0;

    temp_a0 = arg0->x0003;
    temp_a1 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        temp_a1_2 = temp_a1 & 0xFFFF;
        if (temp_a1_2 & 0x2000) {
            temp_v0 = arg0->x0008;
            if (temp_v0 == 0) {
                var_v0 = 2;
            } else {
                var_v0 = temp_v0 - 1;
            }
            arg0->x0008 = var_v0;
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a1_2 & 0x1000) {
            temp_v0_2 = arg0->x0008 + 1;
            arg0->x0008 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) >= 3) {
                arg0->x0008 = 0U;
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a1_2 & 0x20) {
            arg0->x0003 = (u8) (temp_a0 + 1);
            arg0->x0000 = 0x28;
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 1:
        if (F(s8, ((arg0->x0008 * 8) + (s32)cw), 0xB) == 0) {
            SetDialogData(6, 2);
        } else {
            SetDialogData(5, 2);
        }
        arg0->x0002 = (u8) (arg0->x0002 + 1);
        arg0->x0003 = 0U;
        arg0->x0004 = 0;
        break;
    }
}
