/* G online (PC): __cnet_SendReq_PieceExit, written from Monster Hunter G's code at lobby 0x5AEF70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 Write_Socket();
extern u8 send_work;

s32 __cnet_SendReq_PieceExit(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x61) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

