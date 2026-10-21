/* G online (PC): lobby_func_005E6C10, written from Monster Hunter G's code at lobby 0x5E6C10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData8();
s32 Write_Socket();
extern u8 send_work;

s32 lobby_func_005E6C10(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x107) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

