/* G online (PC): lobby_func_005AB940, written from Monster Hunter G's code at lobby 0x5AB940 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;
s32 _cnet_CallBack_Result_RoomRuleCaption(); /* extern */
s32 lobby_func_005AB9C0();         /* extern */
s32 lobby_func_005ABA60();         /* extern */

void lobby_func_005AB940(void) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x20)) != 0) {
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x21))) { /* irregular */
        case 0:
            lobby_func_005AB9C0(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8)), (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x21)));
            return;
        case 1:
            lobby_func_005ABA60(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8)), (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x21)));
            return;
        case 2:
            _cnet_CallBack_Result_RoomRuleCaption(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8)), (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDE8))) + 0x21)));
            break;
        }
    }
}

