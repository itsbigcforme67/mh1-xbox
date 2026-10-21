/* G online (PC): lobby_func_005AB8B0, written from Monster Hunter G's code at lobby 0x5AB8B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005AB940();
void * memset();
extern u8 CnetSys_w;

s32 lobby_func_005AB8B0(s32 unused_a0, s32 arg1, s32 arg2) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8)), 0, 0x28);
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 4)) = arg2;
        (*(s32 (**)(s32, s32, s32, s32))((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0)) = lobby_func_005AB940;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x18)) = (s32) (arg1 & 0x8F);
        return 0;
    }
    return -1;
}

