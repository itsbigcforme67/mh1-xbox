/* G online (PC): __cnet_SendSet_ChatMessageTU, written from Monster Hunter G's code at lobby 0x5ACC30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData16();
s32 Write_Socket();
extern u8 send_work;

s32 __cnet_SendSet_ChatMessageTU(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x2E) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData16(&send_work, arg1);
    SetSendData16(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

