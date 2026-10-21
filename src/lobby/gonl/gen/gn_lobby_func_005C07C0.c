/* G online (PC): lobby_func_005C07C0, written from Monster Hunter G's code at lobby 0x5C07C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();             /* extern */
s32 SetSendCommandLen();               /* extern */
s32 SetSendData8();                /* extern */
s32 Write_Socket();                    /* extern */
extern u8 g_lb_734DFD;
extern u8 g_lb_734DFE;
extern u8 g_lb_734DFF;
extern u8 send_work;

s32 lobby_func_005C07C0(u8 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x121) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendData8(&send_work, g_lb_734DFD);
    SetSendData8(&send_work, g_lb_734DFE);
    SetSendData8(&send_work, g_lb_734DFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

