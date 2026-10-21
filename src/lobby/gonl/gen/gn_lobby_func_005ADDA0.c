/* G online (PC): lobby_func_005ADDA0, written from Monster Hunter G's code at lobby 0x5ADDA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005ADE30();
extern u8 CnetSys_w;
s32 memset();        /* extern */

s32 lobby_func_005ADDA0(s32 arg0, s32 arg1) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38)), 0, 0x28);
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 4)) = arg1;
        (*(void (**)())((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0)) = lobby_func_005ADE30;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x14)) = (s32) (arg0 & 0xFFFF);
        return 0;
    }
    return -1;
}

