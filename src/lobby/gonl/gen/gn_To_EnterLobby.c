/* G online (PC): To_EnterLobby, written from Monster Hunter G's code at lobby 0x580D70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Lobby_JoinUser();
s32 Lb_clearChatList();
s32 cnLbc_CheckInFloorOrder();
s32 fade_set();
extern u8 ClassInfo;
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 lb_sys;
s32 Init_InterruptFlag();                       /* extern */
s32 Plaza_chat_clear();                         /* extern */
s32 cnLBS_Read_PlazaJoinUser(); /* extern */
s32 memset();        /* extern */
extern s32 g_lb_6DE120;

void To_EnterLobby(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 3;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    Init_InterruptFlag();
    memset(&g_lb_6DE120, 0, 0xAE0);
    (*(s8 *)((u8 *)&ClassInfo + 0x8)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA60)) = 0;
    (*(s8 *)&D_0035EFEB) = 1;
    (*(s8 *)((u8 *)&lb_sys + 0x7B)) = 0;
    Plaza_chat_clear();
    Lb_clearChatList();
    cnLBS_Read_PlazaJoinUser(cnLbc_CheckInFloorOrder(1) & 0xFFFF, CallBack_Result_Lobby_JoinUser);
    fade_set(1);
}

