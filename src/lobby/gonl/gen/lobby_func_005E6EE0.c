/* G online (PC): lobby_func_005E6EE0, written from Monster Hunter G's code at lobby 0x5E6EE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). HAND-FIXED: the 8-byte argument {u8 kind, u16 length, char *text} read by field. */
#include "../gonl.h"

s32 SetSendCommand();             /* extern */
s32 SetSendCommandLen();               /* extern */
s32 SetSendData8();                /* extern */
s32 SetSendStringData2();    /* extern */
s32 Write_Socket();                    /* extern */
extern u8 send_work;

s32 lobby_func_005E6EE0(s64 arg0) {
    s64 sp28;
    s32 temp_s0;

    sp28 = arg0;
    temp_s0 = SetSendCommand(&send_work, 0x117) & 0xFFFF;
    SetSendData8(&send_work, (u8) sp28);
    SetSendStringData2(&send_work, *(u8 **)((u8 *)&sp28 + 4), *(u16 *)((u8 *)&sp28 + 2));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

