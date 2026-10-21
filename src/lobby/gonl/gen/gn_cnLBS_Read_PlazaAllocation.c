/* G online (PC): cnLBS_Read_PlazaAllocation, written from Monster Hunter G's code at lobby 0x5E2D60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E2DF0();
extern u8 CnetSys_w;
s32 memset();        /* extern */

s32 cnLBS_Read_PlazaAllocation(s32 arg1, s32 arg2) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0)), 0, 0x28);
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 4)) = arg2;
        (*(s32 (**)())((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0)) = lobby_func_005E2DF0;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0x18)) = (s32) (arg1 & 0xF);
        return 0;
    }
    return -1;
}

