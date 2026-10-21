/* G online (PC): __cnet_SendReq_RoomEntry, written from Monster Hunter G's code at lobby 0x5AD390 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData16();
s32 SetSendStringData2();
s32 Write_Socket();
s32 strlen();
extern s32 g_lb_644C58;
extern u8 send_work;

s32 __cnet_SendReq_RoomEntry(s32 arg0, void *arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x42) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    if (arg1 == NULL) {
        SetSendStringData2(&send_work, &g_lb_644C58, 0);
    } else {
        SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

