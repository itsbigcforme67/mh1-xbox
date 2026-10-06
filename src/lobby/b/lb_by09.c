/* lb_by09 - agent B promoted near-match 0x005B5C90-0x005B5D0C: connect_10 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s8 COM_R_No_0;
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;

s32 connect_10(void) {
    s32 temp_v0;
    s32 var_s0;

    var_s0 = 0;
    temp_v0 = tcp_init(1);
    switch (temp_v0) {
    case -1:
        var_s0 = -1;
        break;
    case -2:
        break;
    case 0:
        COM_R_No_1 = 0;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        COM_R_No_0 = 3;
        var_s0 = 1;
        Vs_Cnt_0 = 0;
        break;
    }
    return var_s0;
}
