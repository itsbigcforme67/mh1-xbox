/* G online (PC): lobby_func_005C0A90, written from Monster Hunter G's code at lobby 0x5C0A90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();             /* extern */
s32 SetSendCommandLen();               /* extern */
s32 SetSendData8();               /* extern */
s32 SetSendStringData2();    /* extern */
s32 Write_Socket();                    /* extern */
extern u8 send_work;

s32 lobby_func_005C0A90(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xEF) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendStringData2(&send_work, arg2, arg3);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

