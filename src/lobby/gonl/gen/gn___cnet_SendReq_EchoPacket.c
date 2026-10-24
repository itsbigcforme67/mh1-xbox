/* G online (PC): __cnet_SendReq_EchoPacket, written from Monster Hunter G's code at lobby 0x5B0070 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendStringData2();
s32 Write_Socket();
extern u8 CnetSys_w;
extern s32 lit_336_0065E008;
extern s32 send_work;

s32 __cnet_SendReq_EchoPacket(void) {
    s32 temp_s0;

    (*(s16 *)((u8 *)&CnetSys_w + 0x102C)) = 0;
    temp_s0 = SetSendCommand(&send_work, 0xA) & 0xFFFF;
    SetSendStringData2(&send_work, &lit_336_0065E008, 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

