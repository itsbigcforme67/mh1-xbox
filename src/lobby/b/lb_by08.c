/* lb_by08 - agent B promoted near-match 0x005B52A0-0x005B537C: internet_connect_00 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 CurDevice;
extern s8 COM_R_No_2;
extern u8 COM_R_No_1;

s32 internet_connect_00(void) {
    s32 temp_v0;
    s32 var_s0;
    u8 temp_v1;

    temp_v1 = COM_R_No_1;
    var_s0 = 0;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        InetConnectInitialize();
        COM_R_No_2 = 0;
        COM_R_No_1 = 0xAU;
        break;
    case 1:                                         /* switch 1 */
        temp_v0 = InetConnectStart();
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:
        case 4:
            break;
        case 1:                                     /* switch 2 */
            COM_R_No_1 = 0xAU;
            break;
        case -1:                                    /* switch 2 */
            COM_R_No_1 = 0U;
            if (CurDevice != 0) {
                var_s0 = 3;
            } else {
                var_s0 = 2;
            }
            break;
        }
        break;
    case 10:                                        /* switch 1 */
        var_s0 = connecting_proc();
        break;
    }
    return var_s0;
}
