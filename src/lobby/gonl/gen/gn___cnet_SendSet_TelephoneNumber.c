/* G online (PC): __cnet_SendSet_TelephoneNumber, written from Monster Hunter G's code at lobby 0x5B01E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;
s32 SetSendCommand();         /* extern */
s32 SetSendCommandLen();               /* extern */
s32 SetSendData8();                /* extern */
s32 Write_Socket();                    /* extern */
extern u8 send_work;

void __cnet_SendSet_TelephoneNumber(void) {
    SetSendCommand(&send_work, 0x20);
    SetSendData8(&send_work, (*(u8 *)((u8 *)&CnetSys_w + 0x10D2)));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

