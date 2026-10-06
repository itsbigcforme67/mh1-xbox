#include "lobby_a.h"
typedef struct { s16 x0000; s8 x0002; u8 x0003; u8 pad0004[0x7]; u8 x000B; s8 x000C; } ARG_server_select_sub_04_arg0;

void server_select_sub_04(ARG_server_select_sub_04_arg0 *arg0) {
    u8 temp_a2;

    temp_a2 = arg0->x0003;
    switch (temp_a2) {                              /* irregular */
    case 0:
        arg0->x0003 = (u8) (temp_a2 + 1);
        arg0->x0000 = 8;
        SetDialogData(0x27, 2, temp_a2);
        arg0->x000B = 1U;
        SetDialogYesNo(arg0->x000B);
        return;
    case 1:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        arg0->x000C = 1;
        if (arg0->x0000 < 0) {
            arg0->x0003 = (u8) (arg0->x0003 + 1);
            return;
        }
        return;
    case 2:
        arg0->x000C = 1;
        if (tk_sw_on_ck(0x20, 2, temp_a2) != 0) {
            cnWrap_SoundRequest(0);
            arg0->x0003 = (u8) (arg0->x0003 + 1);
            arg0->x0000 = 8;
            return;
        }
        if (tk_sw_on_ck(0x40) != 0) {
            cnWrap_SoundRequest(3);
            if (arg0->x000B != 1) {
                arg0->x000B = 1U;
                return;
            }
            arg0->x0002 = 2;
            arg0->x0003 = 0U;
            return;
        }
        tk_lever_ck((u8 *)arg0 + 0xB, 1, 3);
        SetDialogYesNo(arg0->x000B);
        return;
    case 3:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        if (arg0->x0000 < 0) {
            arg0->x0003 = (u8) (arg0->x0003 + 1);
            return;
        }
        break;
    case 4:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        if (arg0->x0000 < 0) {
            if (arg0->x000B == 0) {
                arg0->x0002 = 2;
                arg0->x0003 = 2U;
                cnWrap_SoundRequest(3, temp_a2);
                arg0->x0000 = 0x28;
                str_fadeout(0, 0xF);
                return;
            }
            arg0->x0002 = 2;
            arg0->x0003 = 0U;
        }
        break;
    }
}
