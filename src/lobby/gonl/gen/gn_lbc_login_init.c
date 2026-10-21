/* G online (PC): lbc_login_init, written from Monster Hunter G's code at lobby 0x57A420 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Event_AdminMessage();
s32 CallBack_Event_ChatMessage();
s32 CallBack_Event_ChatMessageTU();
s32 CallBack_Event_LbsBinary();
s32 CallBack_Event_LobbyCommer();
s32 CallBack_Event_LobbyFull();
s32 CallBack_Event_LobbyJoinUser();
s32 CallBack_Event_LobbyLeaver();
s32 CallBack_Event_LobbyRemove();
s32 CallBack_Event_MatchCancel();
s32 CallBack_Event_MatchEntryUser();
s32 CallBack_Event_MatchStart();
s32 CallBack_Event_PlazaJoinUser();
s32 CallBack_Event_PlazaName();
s32 CallBack_Event_PlazaRemove();
s32 CallBack_Event_RecvMail();
s32 CallBack_Event_RoomCapacity();
s32 CallBack_Event_RoomCommer();
s32 CallBack_Event_RoomJoinUser();
s32 CallBack_Event_RoomLeaver();
s32 CallBack_Event_RoomName();
s32 CallBack_Event_RoomPasswordInfo();
s32 CallBack_Event_RoomProperty();
s32 CallBack_Event_RoomRemove();
s32 CallBack_Event_RoomStatus();
s32 CallBack_Event_ShutDownOpponent();
s32 CallBack_Event_System_ShutDown();
s32 CallBack_NoticeUserMiniData();
s32 CallBack_Result_LoginLobbyServer();
s32 Check_CallBackWait();
s32 DeviceGetOptionalStatus();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 Lbs_load();
s32 SetDialogData();
s32 SetSceneTitle();
s32 To_LogOut();
s32 all_reset();
s32 lobby_func_0057A370();
s32 lobby_func_005871D0();
s32 lobby_func_00587260();
s32 lobby_func_005872A0();
s32 lobby_func_005872E0();
s32 lobby_func_00587810();
void * memset();
void * strncpy();
extern u8 cw;
extern u8 net_common_w;
extern u8 netr_ret;
extern u8 pNet;
extern u8 stage_hit_area_f;
s32 cnLBS_Init_LoginLobbyServer();              /* extern */
s32 cnLBS_LoginLobbyServer(); /* extern */
s32 cnLBS_Set_CallBackNoticeEvent(); /* extern */
s32 cnLBS_Set_LoginFirstData();   /* extern */
extern u8 MediaVersion;
extern u8 bsCsvWork;

void lbc_login_init(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0x90] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s8 *)(g_frame + 0x10))
#define sp11 (*(s8 *)(g_frame + 0x11))
#define sp12 (*(s8 *)(g_frame + 0x12))
#define sp14 (*(s32 *)(g_frame + 0x14))
#define sp24 (*(s16 *)(g_frame + 0x24))
#define sp26 (*(s16 *)(g_frame + 0x26))
#define sp28 (*(s16 *)(g_frame + 0x28))
#define sp2A (*(s16 *)(g_frame + 0x2A))
#define sp2C (*(s16 *)(g_frame + 0x2C))
#define sp2E (*(s16 *)(g_frame + 0x2E))
#define sp30 (*(s16 *)(g_frame + 0x30))
#define sp32 (*(s16 *)(g_frame + 0x32))
#define sp40 (*(s8 *)(g_frame + 0x40))
#define sp41 (*(s8 *)(g_frame + 0x41))
#define sp42 (*(s32 *)(g_frame + 0x42))
#define sp4D (*(s32 *)(g_frame + 0x4D))
#define sp60 (*(s32 *)(g_frame + 0x60))
#define sp88 (*(s16 *)(g_frame + 0x88))
#define sp8A (*(s16 *)(g_frame + 0x8A))
#define sp8C (*(s16 *)(g_frame + 0x8C))
#define sp8E (*(s16 *)(g_frame + 0x8E))
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;
    u8 *temp_v1_4;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        all_reset();
        Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        Lbc_set_prim((s32) (lobby_func_0057A370 + 0x30), 0, 0, 0);
        Lbs_load(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        temp_v1_3 = (*(void **)&cw);
        (*(s32 *)&netr_ret) = 1;
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA7B)) = 1;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA7C)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
        memset((*(void **)&cw) + 0xBC4, 0, 0x31C);
        memset((*(void **)&cw) + 0xFA8A, 0, 0x1004);
        memset((*(void **)&cw) + 0x10A8E, 0, 0x1004);
        cnLBS_Init_LoginLobbyServer();
        cnLBS_Set_CallBackNoticeEvent(1, CallBack_Event_MatchStart);
        cnLBS_Set_CallBackNoticeEvent(2, CallBack_Event_System_ShutDown);
        cnLBS_Set_CallBackNoticeEvent(0xB, CallBack_Event_ChatMessage);
        cnLBS_Set_CallBackNoticeEvent(0xC, CallBack_Event_ChatMessageTU);
        cnLBS_Set_CallBackNoticeEvent(3, CallBack_Event_RecvMail);
        cnLBS_Set_CallBackNoticeEvent(4, CallBack_Event_AdminMessage);
        cnLBS_Set_CallBackNoticeEvent(5, CallBack_Event_ShutDownOpponent);
        cnLBS_Set_CallBackNoticeEvent(6, CallBack_Event_MatchCancel);
        cnLBS_Set_CallBackNoticeEvent(7, CallBack_Event_LobbyFull);
        cnLBS_Set_CallBackNoticeEvent(0x11, CallBack_Event_PlazaJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x10, CallBack_Event_PlazaName);
        cnLBS_Set_CallBackNoticeEvent(0x12, lobby_func_005871D0);
        cnLBS_Set_CallBackNoticeEvent(0x14, lobby_func_00587260);
        cnLBS_Set_CallBackNoticeEvent(0x16, lobby_func_005872A0);
        cnLBS_Set_CallBackNoticeEvent(0x19, lobby_func_005872E0);
        cnLBS_Set_CallBackNoticeEvent(0x15, CallBack_Event_LobbyJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x1A, CallBack_Event_LobbyCommer);
        cnLBS_Set_CallBackNoticeEvent(0x1B, CallBack_Event_LobbyLeaver);
        cnLBS_Set_CallBackNoticeEvent(0x1D, CallBack_Event_RoomName);
        cnLBS_Set_CallBackNoticeEvent(0x1F, CallBack_Event_RoomStatus);
        cnLBS_Set_CallBackNoticeEvent(0x23, CallBack_Event_RoomProperty);
        cnLBS_Set_CallBackNoticeEvent(0x1E, CallBack_Event_RoomJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x21, CallBack_Event_RoomCapacity);
        cnLBS_Set_CallBackNoticeEvent(0x22, CallBack_Event_RoomPasswordInfo);
        cnLBS_Set_CallBackNoticeEvent(0x20, lobby_func_00587810);
        cnLBS_Set_CallBackNoticeEvent(8, CallBack_Event_PlazaRemove);
        cnLBS_Set_CallBackNoticeEvent(9, CallBack_Event_LobbyRemove);
        cnLBS_Set_CallBackNoticeEvent(0xA, CallBack_Event_RoomRemove);
        cnLBS_Set_CallBackNoticeEvent(0x24, CallBack_Event_RoomCommer);
        cnLBS_Set_CallBackNoticeEvent(0x25, CallBack_Event_RoomLeaver);
        cnLBS_Set_CallBackNoticeEvent(0x26, CallBack_Event_MatchEntryUser);
        cnLBS_Set_CallBackNoticeEvent(0xD, CallBack_Event_LbsBinary);
        cnLBS_Set_CallBackNoticeEvent(0x2C, CallBack_NoticeUserMiniData);
        memset(&sp10, 0, 0x24);
        DeviceGetOptionalStatus(&sp8E, &sp8C, &sp8A, &sp88);
        sp11 = 1;
        sp12 = 7;
        sp10 = 0;
        strncpy(&sp14, &MediaVersion, 0xA);
        sp2C = 0;
        sp2E = 0;
        sp24 = sp8E;
        sp26 = sp8C;
        sp28 = sp8A;
        sp2A = sp88;
        sp30 = 0;
        sp32 = 0;
        cnLBS_Set_LoginFirstData(&sp10, sp8C, sp8E);
        SetSceneTitle(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        SetDialogData(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        break;
    case 1:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        /* fallthrough */
    case 2:
        temp_v1_4 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) + 1);
        memset(&sp40, 0, 0x40);
        sp40 = 1;
        sp41 = (*(s8 *)((u8 *)&net_common_w + 0x12));
        sp60 = (*(s32 *)&stage_hit_area_f);
        strncpy(&sp42, &(*(u8 *)((u8 *)&bsCsvWork + 0x11)), 0xA);
        strncpy(&sp4D, &bsCsvWork, 0x10);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        cnLBS_LoginLobbyServer(*(struct { u8 b[0x40]; } *)&sp40, CallBack_Result_LoginLobbyServer);
        break;
    case 3:
        Check_CallBackWait(temp_v1 + 0xB9C, arg1, arg2, arg3);
        break;
    default:
        To_LogOut(4, arg1, arg2, arg3);
        break;
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
}

