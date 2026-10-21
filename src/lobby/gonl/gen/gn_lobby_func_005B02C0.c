/* G online (PC): lobby_func_005B02C0, written from Monster Hunter G's code at lobby 0x5B02C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();
s32 SetSendCommandLen();
s32 Write_Socket();
extern u8 send_work;

void lobby_func_005B02C0(void) {
    SetSendCommand(&send_work, 0x22);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

