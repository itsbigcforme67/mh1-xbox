/* G online (PC): To_EnterPlaza2Lobby, written from Monster Hunter G's code at lobby 0x57E450 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_clearChatList();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 Lbs_load();
s32 SetDialogData();
extern u8 ClassInfo;
extern u8 cw;
s32 Init_InterruptFlag();                       /* extern */
s32 Pit_reset();                                /* extern */
s32 Plaza_chat_clear();                         /* extern */
s32 all_reset();                                /* extern */
s32 memset();        /* extern */
extern u8 LobbyInfo;
extern u8 put_back;

void To_EnterPlaza2Lobby(s32 arg0) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 2;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9F)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA0)) = 0;
    Init_InterruptFlag();
    Lbc_init_network_work();
    (*(s8 *)((u8 *)&ClassInfo + 0x4)) = 0;
    (*(s8 *)((u8 *)&ClassInfo + 0x8)) = 0;
    memset(&LobbyInfo, 0, 0x2F94);
    Pit_reset();
    all_reset();
    Lbs_load();
    Plaza_chat_clear();
    Lb_clearChatList();
    Lbc_set_prim((s32) &put_back, 0, 0, 0);
    if (arg0 != 0) {
        SetDialogData(0xF);
    }
}

