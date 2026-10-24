/* G online (PC): lobby_func_005AD450, written from Monster Hunter G's code at lobby 0x5AD450 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();             /* extern */
s32 SetSendCommandLen();               /* extern */
s32 Write_Socket();                    /* extern */
extern u8 send_work;

s32 lobby_func_005AD450(s32 arg0) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = 0;     /* HAND-FIXED: G returns its caller's s0 for an unknown kind */
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x3F) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x44) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x73) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

