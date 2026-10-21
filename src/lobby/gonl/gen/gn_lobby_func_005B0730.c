/* G online (PC): lobby_func_005B0730, written from Monster Hunter G's code at lobby 0x5B0730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData8();
s32 SetSendStringData2();
s32 Write_Socket();
extern s32 send_work;

s32 lobby_func_005B0730(s32 arg0, void *arg1, void *arg2, s32 arg3) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xFD) & 0xFFFF;
    SetSendStringData2(&send_work, arg1, 6);
    SetSendStringData2(&send_work, arg2, arg3);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

