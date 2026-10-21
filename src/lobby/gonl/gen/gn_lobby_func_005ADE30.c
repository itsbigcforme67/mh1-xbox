/* G online (PC): lobby_func_005ADE30, written from Monster Hunter G's code at lobby 0x5ADE30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005ADEB0();
s32 lobby_func_005ADF20();
s32 lobby_func_005ADF50();
extern u8 CnetSys_w;

void lobby_func_005ADE30(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x20)) != 0) {
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x21))) { /* irregular */
        case 0:
            lobby_func_005ADEB0(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38)), (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x21)), arg2, arg3);
            return;
        case 1:
            lobby_func_005ADF20(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38)), (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x21)), arg2, arg3);
            return;
        case 2:
            lobby_func_005ADF50(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38)), (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xE38))) + 0x21)), arg2, arg3);
            break;
        }
    }
}

