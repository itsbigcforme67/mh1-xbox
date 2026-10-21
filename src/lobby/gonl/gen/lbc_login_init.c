/* G online (PC): lbc_login_init, written from Monster Hunter G's code at lobby 0x57A420 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 net_common_w;
extern u8 netr_ret;
extern u8 pNet;
extern u8 stage_hit_area_f;
s32 CallBackWaitInit();                         /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 DeviceGetOptionalStatus(); /* extern */
s32 Lbc_init_network_work();                    /* extern */
s32 Lbc_set_prim(); /* extern */
s32 Lbs_load();                                 /* extern */
s32 SetDialogData();                     /* extern */
s32 SetSceneTitle();                     /* extern */
s32 To_LogOut();                         /* extern */
s32 all_reset();                             /* extern */
s32 cnLBS_Init_LoginLobbyServer();              /* extern */
s32 cnLBS_LoginLobbyServer();    /* extern */
s32 cnLBS_Set_CallBackNoticeEvent(); /* extern */
s32 cnLBS_Set_LoginFirstData();   /* extern */
s32 memset();             /* extern */
s32 strncpy();     /* extern */
extern u8 CallBack_Event_AdminMessage;
extern u8 CallBack_Event_ChatMessage;
extern u8 CallBack_Event_ChatMessageTU;
extern u8 CallBack_Event_LbsBinary;
extern u8 CallBack_Event_LobbyCommer;
extern u8 CallBack_Event_LobbyFull;
extern u8 CallBack_Event_LobbyJoinUser;
extern u8 CallBack_Event_LobbyLeaver;
extern u8 CallBack_Event_LobbyRemove;
extern u8 CallBack_Event_MatchCancel;
extern u8 CallBack_Event_MatchEntryUser;
extern u8 CallBack_Event_MatchStart;
extern u8 CallBack_Event_PlazaJoinUser;
extern u8 CallBack_Event_PlazaName;
extern u8 CallBack_Event_PlazaRemove;
extern u8 CallBack_Event_RecvMail;
extern u8 CallBack_Event_RoomCapacity;
extern u8 CallBack_Event_RoomCommer;
extern u8 CallBack_Event_RoomJoinUser;
extern u8 CallBack_Event_RoomLeaver;
extern u8 CallBack_Event_RoomName;
extern u8 CallBack_Event_RoomPasswordInfo;
extern u8 CallBack_Event_RoomProperty;
extern u8 CallBack_Event_RoomRemove;
extern u8 CallBack_Event_RoomStatus;
extern u8 CallBack_Event_ShutDownOpponent;
extern u8 CallBack_Event_System_ShutDown;
extern u8 CallBack_NoticeUserMiniData;
extern u8 CallBack_Result_LoginLobbyServer;
extern u8 MediaVersion;
extern u8 bsCsvWork;
extern u8 lobby_func_0057A370;
extern u8 lobby_func_005871D0;
extern u8 lobby_func_00587260;
extern u8 lobby_func_005872A0;
extern u8 lobby_func_005872E0;
extern u8 lobby_func_00587810;

void lbc_login_init(void) {
    union { s16 v; u8 b[0x2]; } u_sp8E;
#define sp8E (u_sp8E.v)
    union { s16 v; u8 b[0x4]; } u_sp8C;
#define sp8C (u_sp8C.v)
    union { s16 v; u8 b[0x6]; } u_sp8A;
#define sp8A (u_sp8A.v)
    union { s16 v; u8 b[0x8]; } u_sp88;
#define sp88 (u_sp88.v)
    s32 sp60;
    union { s32 v; u8 b[0x43]; } u_sp4D;
#define sp4D (u_sp4D.v)
    union { s32 v; u8 b[0x4E]; } u_sp42;
#define sp42 (u_sp42.v)
    s8 sp41;
    union { s8 v; u8 b[0x50]; } u_sp40;
#define sp40 (u_sp40.v)
    s16 sp32;
    s16 sp30;
    s16 sp2E;
    s16 sp2C;
    s16 sp2A;
    s16 sp28;
    s16 sp26;
    s16 sp24;
    union { s32 v; u8 b[0x7C]; } u_sp14;
#define sp14 (u_sp14.v)
    s8 sp12;
    s8 sp11;
    union { s8 v; u8 b[0x80]; } u_sp10;
#define sp10 (u_sp10.v)
    s32 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;
    u8 *temp_v1_4;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9C;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        all_reset(temp_a0);
        Lbc_init_network_work();
        Lbc_set_prim(&lobby_func_0057A370 + 0x30, 0, 0, 0);
        Lbs_load();
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
        cnLBS_Set_CallBackNoticeEvent(1, &CallBack_Event_MatchStart);
        cnLBS_Set_CallBackNoticeEvent(2, &CallBack_Event_System_ShutDown);
        cnLBS_Set_CallBackNoticeEvent(0xB, &CallBack_Event_ChatMessage);
        cnLBS_Set_CallBackNoticeEvent(0xC, &CallBack_Event_ChatMessageTU);
        cnLBS_Set_CallBackNoticeEvent(3, &CallBack_Event_RecvMail);
        cnLBS_Set_CallBackNoticeEvent(4, &CallBack_Event_AdminMessage);
        cnLBS_Set_CallBackNoticeEvent(5, &CallBack_Event_ShutDownOpponent);
        cnLBS_Set_CallBackNoticeEvent(6, &CallBack_Event_MatchCancel);
        cnLBS_Set_CallBackNoticeEvent(7, &CallBack_Event_LobbyFull);
        cnLBS_Set_CallBackNoticeEvent(0x11, &CallBack_Event_PlazaJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x10, &CallBack_Event_PlazaName);
        cnLBS_Set_CallBackNoticeEvent(0x12, &lobby_func_005871D0);
        cnLBS_Set_CallBackNoticeEvent(0x14, &lobby_func_00587260);
        cnLBS_Set_CallBackNoticeEvent(0x16, &lobby_func_005872A0);
        cnLBS_Set_CallBackNoticeEvent(0x19, &lobby_func_005872E0);
        cnLBS_Set_CallBackNoticeEvent(0x15, &CallBack_Event_LobbyJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x1A, &CallBack_Event_LobbyCommer);
        cnLBS_Set_CallBackNoticeEvent(0x1B, &CallBack_Event_LobbyLeaver);
        cnLBS_Set_CallBackNoticeEvent(0x1D, &CallBack_Event_RoomName);
        cnLBS_Set_CallBackNoticeEvent(0x1F, &CallBack_Event_RoomStatus);
        cnLBS_Set_CallBackNoticeEvent(0x23, &CallBack_Event_RoomProperty);
        cnLBS_Set_CallBackNoticeEvent(0x1E, &CallBack_Event_RoomJoinUser);
        cnLBS_Set_CallBackNoticeEvent(0x21, &CallBack_Event_RoomCapacity);
        cnLBS_Set_CallBackNoticeEvent(0x22, &CallBack_Event_RoomPasswordInfo);
        cnLBS_Set_CallBackNoticeEvent(0x20, &lobby_func_00587810);
        cnLBS_Set_CallBackNoticeEvent(8, &CallBack_Event_PlazaRemove);
        cnLBS_Set_CallBackNoticeEvent(9, &CallBack_Event_LobbyRemove);
        cnLBS_Set_CallBackNoticeEvent(0xA, &CallBack_Event_RoomRemove);
        cnLBS_Set_CallBackNoticeEvent(0x24, &CallBack_Event_RoomCommer);
        cnLBS_Set_CallBackNoticeEvent(0x25, &CallBack_Event_RoomLeaver);
        cnLBS_Set_CallBackNoticeEvent(0x26, &CallBack_Event_MatchEntryUser);
        cnLBS_Set_CallBackNoticeEvent(0xD, &CallBack_Event_LbsBinary);
        cnLBS_Set_CallBackNoticeEvent(0x2C, &CallBack_NoticeUserMiniData);
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
        SetSceneTitle(0);
        SetDialogData(0);
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
        CallBackWaitInit();
        cnLBS_LoginLobbyServer(&sp40, &CallBack_Result_LoginLobbyServer);
        break;
    case 3:
        Check_CallBackWait(temp_a0);
        break;
    default:
        To_LogOut(4);
        break;
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
}

