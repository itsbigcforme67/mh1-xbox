/* lb_id01 - agent C 0x005C2030-0x005C2178: id_select_01 (handle-name select menu; s32 sw = Get_sw2() & 0xFFFF, (s8)cw[0xB + n*8]). */
#include "lobby_a.h"
typedef struct { s16 x0000; u8 x0002; u8 x0003; s8 x0004; u8 pad0005[0x3]; u8 x0008; } ARG_id_select_01_arg0;

void id_select_01(ARG_id_select_01_arg0 *arg0) {
    s32 sw;
    s32 t;
    u8 step;
    u8 v;

    sw = Get_sw2(0) & 0xFFFF;
    step = arg0->x0003;
    switch (step) {
    case 0:
        t = sw & 0xFFFF;
        if (t & 0x2000) {
            v = arg0->x0008;
            if (v == 0) {
                arg0->x0008 = 2;
            } else {
                arg0->x0008 = v - 1;
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (t & 0x1000) {
            v = arg0->x0008 + 1;
            arg0->x0008 = v;
            if (v > 2) {
                arg0->x0008 = 0;
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (t & 0x20) {
            arg0->x0003 = step + 1;
            arg0->x0000 = 0x28;
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 1:
        if ((s8)cw[0xB + arg0->x0008 * 8] == 0) {
            SetDialogData(6, 2);
        } else {
            SetDialogData(5, 2);
        }
        arg0->x0002 = (u8) (arg0->x0002 + 1);
        arg0->x0003 = 0;
        arg0->x0004 = 0;
        break;
    }
}
