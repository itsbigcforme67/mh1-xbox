/* G online (PC): lobby_func_005AE360, written from Monster Hunter G's code at lobby 0x5AE360 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData16();
s32 Write_Socket();
extern s32 send_work;

s32 lobby_func_005AE360(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x59) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

