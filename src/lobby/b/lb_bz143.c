/* lb_bz143 - lobby UI/client 0x005C2180-0x005C2278: id_select_02 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s8 ret_stat_0038A900;
typedef struct { u8 pad0000[0x2]; s8 x0002; u8 x0003; u8 pad0004[0x8]; s8 x000C; } ARG_id_select_02_arg0;

void id_select_02(ARG_id_select_02_arg0 *arg0) {
    s32 temp_v0;
    u8 temp_a0;

    Get_sw2(0);
    arg0->x000C = 1;
    temp_a0 = arg0->x0003;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        arg0->x0003 = (u8) (temp_a0 + 1);
        SetDialogYesNo(0, 1);
        return;
    case 1:                                         /* switch 1 */
        temp_v0 = Lb_select(temp_a0, 1);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            fade_set(1);
            arg0->x0003 = (u8) (arg0->x0003 + 1);
            return;
        case 3:                                     /* switch 2 */
            arg0->x0002 = 1;
            arg0->x0003 = 0U;
            SetSceneTitle(1, 0);
            SetHelpLineMsg(1, 1);
            return;
        }
        break;
    case 2:                                         /* switch 1 */
        if ((Fade_busy_ck(temp_a0, 1) & 0xFF) != 1) {
            ret_stat_0038A900 = 0;
        }
    }
}
