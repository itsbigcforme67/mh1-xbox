/* G online (PC): __cnet_SendSet_TelephoneNumber, written from Monster Hunter G's code at lobby 0x5B01E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 SetSendData8();
s32 Write_Socket();
extern u8 CnetSys_w;
extern u8 send_work;

void __cnet_SendSet_TelephoneNumber(void) {
    SetSendCommand(&send_work, 0x20);
    SetSendData8(&send_work, (s32) (*(u8 *)((u8 *)&CnetSys_w + 0x10D2)));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

