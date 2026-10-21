/* G online (PC): To_EnterPlaza, written from Monster Hunter G's code at lobby 0x57E340 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_clearChatList();
s32 Lbc_init_network_work();
s32 SetDialogData();
extern u8 ClassInfo;
extern u8 cw;
s32 Init_InterruptFlag();                       /* extern */
s32 Plaza_chat_clear();                         /* extern */
s32 memset();        /* extern */
extern u8 LobbyInfo;

void To_EnterPlaza(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 2;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    Init_InterruptFlag();
    (*(s8 *)((u8 *)&ClassInfo + 0x4)) = 0;
    (*(s8 *)((u8 *)&ClassInfo + 0x8)) = 0;
    memset(&LobbyInfo, 0, 0x2F94);
    Plaza_chat_clear();
    Lb_clearChatList();
    Lbc_init_network_work();
    SetDialogData(0xE);
}

