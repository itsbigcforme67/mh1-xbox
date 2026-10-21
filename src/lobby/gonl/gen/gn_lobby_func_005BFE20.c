/* G online (PC): lobby_func_005BFE20, written from Monster Hunter G's code at lobby 0x5BFE20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005BFEA0();
s32 lobby_func_005BFF10();
s32 lobby_func_005BFF40();
extern u8 CnetSys_w;

void lobby_func_005BFE20(void) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 0x20)) != 0) {
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 0x21))) { /* irregular */
        case 0:
            lobby_func_005BFEA0(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0)));
            return;
        case 1:
            lobby_func_005BFF10(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0)));
            return;
        case 2:
            lobby_func_005BFF40(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0)));
            break;
        }
    }
}

