#include "lbnet.h"

int GetRecvData16();
int GetRecvData8();
int GetRecvDataOption3();
int GetRecvDataString();
int SetSendCommand();
int SetSendCommandLen();
int SetSendData8();
int SetSendStringData2();
int Write_Socket();
int __cnet_ClearEntryFloorInfo();
int __cnet_Login_Return();
int __cnet_Recv_Byte();
int __cnet_Recv_MemberSub();
int __cnet_Recv_PatchData();
int __cnet_Recv_SearchUser();
int __cnet_Recv_ServerMessage();
int __cnet_Recv_Word();
int __cnet_SendAns_BattleResult();
int __cnet_SendReq_EchoPacket();
int __cnet_SendReq_MatchJoin();
int __cnet_SendSet_ConnectionPair();
int __cnet_SendSet_FirstData();
int __cnet_Send_PatchFinish();
int __cnet_SetEntryFloorInfo();
int _cnetEvent_JumpCallBack();
int _cnet_RecvFromLbs_AnswerBrowserMethodGet();
int _cnet_Return_CallBack();
int _sub_InOutRoomMember();
int _sub_ReceiveJoinUser();
int memset();
int strcpy();
int strlen();
int strncpy();
void __cnet_bgProg_ReadLobbyAllocation();
void __cnet_bgProg_ReadPlazaAllocation();
void __cnet_bgProg_ReadRoomAllocation();
void __cnet_bgProg_ReadRoomRule();
void _cnet_RecvFromLbs_NoticeMailMessage();
void cnLBS_Get_RecvMessage();
void _cnet_RecvFromLbs_AnswerSendMail();
void __cnet_Recv_MailMessage();
void cnLBS_Get_SerchUserPlace();
void cnLBS_Get_SerchUserPlaceMessage();
void _cnet_RecvFromLbs_AnswerSearchUser();
void _cnet_RecvFromLbs_RequestAdminMessage();
void cnLBS_AnswerAdminMessage();
int cnLBS_Get_ConditionSearchUser();
void _cnet_RecvFromLbs_AnswerPersonalDataChange();
void __cnet_SendSet_PersonalDataAge();
void _cnet_RecvFromLbs_AnswerPersonalDataRegisted();
int cnLBS_Read_PlazaAllocation();
int cnLBS_Get_PlazaCount();
void _cnet_RecvFromLbs_AnswerPlazaNumOfPlaza();
int cnLBS_Get_PlazaName();
void _cnet_RecvFromLbs_BothPlazaJoinUser();
int cnLBS_Get_PlazaStatus();
void _cnet_RecvFromLbs_AnswerPlazaEntry();
void _cnet_RecvFromLbs_AnswerPlazaExit();
int cnLBS_Read_LobbyAllocation();
int cnLBS_Get_LobbyCount();
void _cnet_RecvFromLbs_AnswerLobbyNumOfLobby();
int cnLBS_Get_LobbyName();
void _cnet_RecvFromLbs_BothLobbyJoinUser();
int cnLBS_Get_LobbyStatus();
void _cnet_RecvFromLbs_AnswerLobbyEntry();
void _cnet_RecvFromLbs_AnswerLobbyExit();
int cnLBS_Read_RoomAllocation();
int cnLBS_Get_RoomCount();
void _cnet_RecvFromLbs_AnswerRoomNumOfRoom();
void _cnet_RecvFromLbs_BothRoomJoinUser();
int cnLBS_Get_RoomStatus();
int cnLBS_Get_RoomName();
int cnLBS_Get_RoomExplain();
void _cnet_RecvFromLbs_AnswerRoomCreate();
void _cnet_RecvFromLbs_AnswerRoomEntry();
int cnLBS_Get_RoomProperty();
void _cnet_RecvFromLbs_AnswerSetRoomProperty();
void _cnet_RecvFromLbs_NoticeRoomCommer();
void _cnet_RecvFromLbs_NoticeRoomLeaver();
int cnLBS_Read_RoomRuleAllocation();
int cnLBS_Get_RoomPasswordInfo();
void _cnet_RecvFromLbs_AnswerRoomMember();
void __cnet_Recv_RoomMember();
void _cnet_RecvFromLbs_AnswerAppointJump();
void _cnet_RecvFromLbs_AnswerTopPageJump();
int cnetGet_Room_LastRecvNumber();
void _cnet_RecvFromLbs_BothGameJoin();
void _cnet_RecvFromLbs_AnswerRoomNumOfRule();
void _cnet_RecvFromLbs_AnswerRoomSetName();
void _cnet_RecvFromLbs_AnswerRoomSetRule();
void _cnet_RecvFromLbs_AnswerRoomSetFinish();
void _cnet_RecvFromLbs_BothRoomExit();
void _cnet_RecvFromLbs_NoticePlazaRemove();
void _cnet_RecvFromLbs_NoticeLobbyRemove();
void _cnet_RecvFromLbs_NoticeRoomRemove();
int cnLBS_Get_AllocationProgressCount();
void __cnet_Recv_GameJoin();
void __cnet_Recv_NumOfPiece();
void __cnet_Recv_PieceName();
void __cnet_Recv_PieceJoinUser();
void __cnet_Recv_PieceJoinUserMH();
void __cnet_Recv_PieceStatus();
void __cnet_Recv_PieceExplain();
void __cnet_Recv_RuleControl();
void _cnet_RecvFromLbs_AnswerLobbyMatchEntry();
void _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser();
void _cnet_RecvFromLbs_AnswerAnnexEntry();
void _cnet_RecvFromLbs_AnswerAnnexExit();
void _cnet_RecvFromLbs_BothAnnexJoinUser();
void _cnet_RecvFromLbs_AnswerAnnexMember();
void __cnet_Recv_AnnexMember();
void _cnet_RecvFromLbs_NoticeAnnexLeaver();
void _cnet_RecvFromLbs_NoticeAnnexCommer();
void _cnet_RecvFromLbs_AnswerLobbyMember();
void __cnet_Recv_LobbyMember();
void _cnet_RecvFromLbs_NoticeLobbyLeaver();
void _cnet_RecvFromLbs_NoticeLobbyCommer();
void _cnet_RecvFromLbs_AnswerRoomSetExplain();
int cnLBS_Get_TimingValue();
void _cnet_RecvFromLbs_AnswerCurrentPlace();
void cnLBS_Init_LoginLobbyServer();
u8 cnetGet_Login_NoOfUserAccount();
int cnetGet_Login_DecideUserID();
int cnetGet_Login_DecideUserHandle();
void _cnet_RecvFromLbs_RequestConnectionPair();
void _cnet_RecvFromLbs_RequestFirstData();
void _cnet_RecvFromLbs_AnswerMiniDataRegist();
void _cnet_RecvFromLbs_NoticeMiniData();
void _cnet_RecvFromLbs_NoticeLoginOk();
int cnLBS_Answer_LoginWarningMessage();
void __cnet_SendAns_WarningMessage();
int cnLBS_Send_LoginFinish();
void _cnet_RecvFromLbs_AnswerBillEstimate();
void _cnet_RecvFromLbs_AnswerUserBinary();
void _cnet_RecvFromLbs_RequestTelephoneNumber();
void _cnet_RecvFromLbs_RequestPersonalDataRegist();
void _cnet_RecvFromLbs_RequestBattleResult();
void __cnet_SendReq_UserID();
void __cnet_SendSet_LoginFinish();
void __cnet_SendSet_TelephoneNumber();
int __cnet_Recv_UserID();
void cnLBS_Send_ChatMessage();
void __cnet_SendSet_ChatMessage();
void _cnet_RecvFromLbs_NoticeChatMessage();
void cnLBS_Send_ChatBinary();
void __cnet_SendSet_ChatBinary();
void _cnet_RecvFromLbs_NoticeChatBinary();
void _cnet_RecvFromLbs_AnswerChatMessageTU();
void _cnet_RecvFromLbs_NoticeChatMessageTU();
void _cnet_RecvFromLbs_AnswerChatBinaryTU();
void _cnet_RecvFromLbs_NoticeChatBinaryTU();
void _cnet_RecvFromLbs_MatchStart();
void _cnet_RecvFromLbs_MatchEntry();
int cnLBS_MatchStart();
int cnLBS_Read_MatchInfomation();
void __cnet_SendReq_MatchPlSide();
void __cnet_SendReq_MatchOpponentInfo();
void __cnet_SendReq_MatchOpponentStatus();
void __cnet_SendReq_MatchGameRule();
void __cnet_SendReq_MatchBattleCode();
void __cnet_SendReq_MatchMcsIpAddr();
void _cnet_RecvFromLbs_NoticePatchData();
void _cnet_RecvFromLbs_NoticePatchFooter();
int cnLBS_Answer_PatchFinish();
int cnLBS_Get_PatchInformation();
void _cnet_RecvFromLbs_RequestRegurationVersion();
void _cnet_RecvFromLbs_NoticeRegurationAddress();
void _cnet_RecvFromLbs_AnswerRegurationData();
void cnLBS_Send_RegurationAgree();
void _cnet_RecvFromLbs_AnswerRegurationAgree();
int cnLBS_Get_ServerMessage();
void _cnet_RecvFromLbs_RequestLineCheck();
void __cnet_SendSet_LineCheck();
void _cnet_RecvFromLbs_NoticeShutDown();
void _cnet_RecvFromLbs_NoticeShutDownOpponent();
void _cnet_RecvFromLbs_NoticeMatchCancel();
void _cnet_RecvFromLbs_NoticeLobbyFull();
extern u8 recv_work[];

void _cnet_RecvFromLbs_NoticeMailMessage(void) {
    __cnet_Recv_MailMessage();
    _cnetEvent_JumpCallBack(3, 0);
}

void cnLBS_Get_RecvMessage(char *a, char *b, char *c) {
    if (a != 0) {
        strcpy(a, CNWP(0x308E4));
    }
    if (b != 0) {
        strcpy(b, CNWP(0x308EC));
    }
    if (c != 0) {
        strcpy(c, CNWP(0x30900));
    }
}

void _cnet_RecvFromLbs_AnswerSendMail(void) {
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_MailMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), GetRecvDataString(CNWP(0x308E4), &recv_work)));
}

void cnLBS_Get_SerchUserPlace(u16 *arg0, u16 *arg1, u16 *arg2, u8 *arg3) {
    *arg0 = CNW(u16, 0x30980);
    *arg1 = CNW(u16, 0x30982);
    *arg2 = CNW(u16, 0x30984);
    *arg3 = CNW(u8, 0x30987);
}

void cnLBS_Get_SerchUserPlaceMessage(char *d) {
    strcpy(d, CNWP(0x30988));
}

void _cnet_RecvFromLbs_AnswerSearchUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_SearchUser();
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_RequestAdminMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), &recv_work));
    _cnetEvent_JumpCallBack(4, 0);
}

void cnLBS_AnswerAdminMessage(void) {
    SetSendCommand(&send_work, 0xF5);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPersonalDataChange(void) {
    _cnet_Return_CallBack(0);
}

void __cnet_SendSet_PersonalDataAge(void) {
    SetSendCommand(&send_work, 0xB8);
    SetSendData8(&send_work, CNW(u8, 0x1248));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_AnswerPersonalDataRegisted(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_PlazaAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[2].state == 0) {
        CnetSys_w.burst[2].cb = (void *)cb;
        CnetSys_w.burst[2].val = val & 0xF;
        CnetSys_w.burst[2].state = 1;
        CnetSys_w.burst[2].run = __cnet_bgProg_ReadPlazaAllocation;
        CnetSys_w.burst[2].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Get_PlazaCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x404E);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaNumOfPlaza(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x404E));
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_PlazaName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x4082);
    return 0;
}

void _cnet_RecvFromLbs_BothPlazaJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4064));
    }
    _cnet_Return_CallBack(0xF);
}

int cnLBS_Get_PlazaStatus(int idx, u8 *d) {
    *d = CnetSys_w.plaza[(u16)idx].status;
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerPlazaExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_LobbyAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[3].state == 0) {
        CnetSys_w.burst[3].cb = (void *)cb;
        CnetSys_w.burst[3].val = val & 0xF;
        CnetSys_w.burst[3].state = 1;
        CnetSys_w.burst[3].run = __cnet_bgProg_ReadLobbyAllocation;
        CnetSys_w.burst[3].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Get_LobbyCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4050);
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyNumOfLobby(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4050));
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_LobbyName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x4E6A);
    return 0;
}

void _cnet_RecvFromLbs_BothLobbyJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4E4C));
    }
    _cnet_Return_CallBack(0x14);
}

int cnLBS_Get_LobbyStatus(int idx, u8 *d) {
    *d = CnetSys_w.lobby[(u16)idx].status;
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerLobbyExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_RoomAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[4].state == 0) {
        CnetSys_w.burst[4].cb = (void *)cb;
        CnetSys_w.burst[4].val = val & 0xFF;
        CnetSys_w.burst[4].state = 1;
        CnetSys_w.burst[4].run = __cnet_bgProg_ReadRoomAllocation;
        CnetSys_w.burst[4].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Get_RoomCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4052);
    return 0;
}

void _cnet_RecvFromLbs_AnswerRoomNumOfRoom(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4052));
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_BothRoomJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x61C4));
    }
    _cnet_Return_CallBack(0x19);
}

int cnLBS_Get_RoomStatus(int idx, u8 *d) {
    *d = CnetSys_w.room[(u16)idx].status;
    return 0;
}

int cnLBS_Get_RoomName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x61E2);
    return 0;
}

int cnLBS_Get_RoomExplain(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x6224);
    return 0;
}

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRoomEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_RoomProperty(int idx, int *d) {
    *d = CnetSys_w.room[(u16)idx].prop;
    return 0;
}

void _cnet_RecvFromLbs_AnswerSetRoomProperty(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeRoomCommer(void) {
    _sub_InOutRoomMember(2);
}

void _cnet_RecvFromLbs_NoticeRoomLeaver(void) {
    _sub_InOutRoomMember(3);
}

int cnLBS_Read_RoomRuleAllocation(int val, int cb) {
    if (CnetSys_w.burst[5].state == 0) {
        CnetSys_w.burst[5].cb = (void *)cb;
        CnetSys_w.burst[5].val = val & 0xFFFF;
        CnetSys_w.burst[5].state = 1;
        CnetSys_w.burst[5].run = __cnet_bgProg_ReadRoomRule;
        CnetSys_w.burst[5].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Get_RoomPasswordInfo(int idx, u8 *d) {
    *d = CnetSys_w.room[(u16)idx].pwinfo;
    return 0;
}

void _cnet_RecvFromLbs_AnswerRoomMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_RoomMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_RoomMember(void) {
    memset(CnetSys_w.room_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_room_member, CnetSys_w.room_member);
}

void _cnet_RecvFromLbs_AnswerAppointJump(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(0);
        __cnet_SetEntryFloorInfo(1);
        __cnet_SetEntryFloorInfo(2);
        __cnet_Recv_ServerMessage();
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerTopPageJump(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
        __cnet_ClearEntryFloorInfo(1);
        __cnet_ClearEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnetGet_Room_LastRecvNumber(u16 *arg0) {
    *arg0 = CNW(u16, 0x6CEC);
    return 0;
}

void _cnet_RecvFromLbs_BothGameJoin(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_GameJoin();
    }
    _cnet_Return_CallBack(0xD);
}

void _cnet_RecvFromLbs_AnswerRoomNumOfRule(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Byte(CNWP(0x6E4B));
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRoomSetName(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetRule(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetFinish(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_BothRoomExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticePlazaRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(9, 0);
}

void _cnet_RecvFromLbs_NoticeLobbyRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(0xA, 0);
}

void _cnet_RecvFromLbs_NoticeRoomRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(0xB, 0);
}

int cnLBS_Get_AllocationProgressCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x1032);
    return 0;
}

void __cnet_Recv_GameJoin(void) {

}

void __cnet_Recv_NumOfPiece(u16 *d) {
    GetRecvData16(d, recv_work);
}

void __cnet_Recv_PieceName(void *a, void *b) {
    GetRecvDataString(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceJoinUser(void *a, void *b) {
    GetRecvData16(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceJoinUserMH(void *a, void *b, void *c) {
    GetRecvData16(c, GetRecvData16(b, GetRecvData16(a, recv_work)));
}

void __cnet_Recv_PieceStatus(void *a, void *b) {
    GetRecvData8(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceExplain(void *a, void *b) {
    GetRecvDataString(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_RuleControl(void *a, void *b, void *c) {
    *(int *)c = GetRecvData8(b, GetRecvData8(a, recv_work));
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntry(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x302F4), GetRecvData16(CNWP(0x302F2), GetRecvData16(CNWP(0x302F0), &recv_work)));
    }
    _cnet_Return_CallBack(0x22);
}

void _cnet_RecvFromLbs_AnswerAnnexEntry(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerAnnexExit(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_BothAnnexJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Word(CNWP(0x302FC));
    }
    _cnet_Return_CallBack(0x23);
}

void _cnet_RecvFromLbs_AnswerAnnexMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_AnnexMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_AnnexMember(void) {
    memset(CnetSys_w.annex_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_annex_member, CnetSys_w.annex_member);
}

void _cnet_RecvFromLbs_NoticeAnnexLeaver(void) {
    _sub_InOutRoomMember(5);
}

void _cnet_RecvFromLbs_NoticeAnnexCommer(void) {
    _sub_InOutRoomMember(4);
}

void _cnet_RecvFromLbs_AnswerLobbyMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_LobbyMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_LobbyMember(void) {
    memset(CnetSys_w.lobby_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_lobby_member, CnetSys_w.lobby_member);
}

void _cnet_RecvFromLbs_NoticeLobbyLeaver(void) {
    _sub_InOutRoomMember(1);
}

void _cnet_RecvFromLbs_NoticeLobbyCommer(void) {
    _sub_InOutRoomMember(0);
}

void _cnet_RecvFromLbs_AnswerRoomSetExplain(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_TimingValue(int *arg0) {
    *arg0 = CNW(int, 0x3BA54);
    return 0;
}

void _cnet_RecvFromLbs_AnswerCurrentPlace(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x3BA5C), GetRecvData16(CNWP(0x3BA5A), GetRecvData16(CNWP(0x3BA58), &recv_work)));
    }
    _cnet_Return_CallBack(0);
}

void cnLBS_Init_LoginLobbyServer(void) {
    *(int *)((u8 *)CNWP(8)) = 0;
    *(int *)((u8 *)CNWP(0)) = 0;
    memset(CNWP(0x18), 0, 0xE00);
    memset(CNWP(0xE18), 0, 0x1B0);
    memset(CNWP(0x4058), 0, 0xA);
    memset(CNWP(0x1436), 0, 0x28);
}

u8 cnetGet_Login_NoOfUserAccount(void) {
    return CNW(u8, 0x145E);
}

int cnetGet_Login_DecideUserID(char *d) {
    strcpy(d, CNWP(0x1576));
    return 0;
}

int cnetGet_Login_DecideUserHandle(char *d) {
    strcpy(d, CNWP(0x157E));
    return 0;
}

void _cnet_RecvFromLbs_RequestConnectionPair(void) {
    GetRecvData16(CNWP(0xFEE), &recv_work);
    __cnet_SendSet_ConnectionPair();
}

void _cnet_RecvFromLbs_RequestFirstData(void) {
    if (CNW(u8, 0x1034) != 0) {
        __cnet_SendReq_EchoPacket();
        return;
    }
    __cnet_SendSet_FirstData();
}

void _cnet_RecvFromLbs_AnswerMiniDataRegist(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeMiniData(void) {
    if (CnetSys_w.rres == 0) {
        memset(CnetSys_w.minidata, 0, 0x5C);
        GetRecvDataOption3(&CnetSys_w.minidata[0x1C], 0x40, GetRecvDataOption3(CnetSys_w.minidata, 8, recv_work));
    }
    _cnet_Return_CallBack(0x2C);
}

void _cnet_RecvFromLbs_NoticeLoginOk(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Login_Return();
    }
}

int cnLBS_Answer_LoginWarningMessage(void) {
    __cnet_SendAns_WarningMessage();
    return 0;
}

void __cnet_SendAns_WarningMessage(int arg0) {
    SetSendCommand(&send_work, 0x14);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_Send_LoginFinish(void) {
    __cnet_SendSet_LoginFinish();
    return 0;
}

void _cnet_RecvFromLbs_AnswerBillEstimate(void) {

}

void _cnet_RecvFromLbs_AnswerUserBinary(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_RequestTelephoneNumber(void) {
    __cnet_SendSet_TelephoneNumber();
}

void _cnet_RecvFromLbs_RequestPersonalDataRegist(void) {

}

void _cnet_RecvFromLbs_RequestBattleResult(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_SendAns_BattleResult();
    }
}

void __cnet_SendReq_UserID(void) {
    SetSendCommand(&send_work, 0x16);
    SetSendStringData2(&send_work, CnetSys_w.uid, 6);
    SetSendStringData2(&send_work, CnetSys_w.uhandle, strlen(CnetSys_w.uhandle) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_LoginFinish(void) {
    SetSendCommand(&send_work, 0x1A);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_TelephoneNumber(void) {
    SetSendCommand(&send_work, 0xF);
    SetSendStringData2(&send_work, CnetSys_w.tel, strlen(CnetSys_w.tel) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_Recv_UserID(void) {
    GetRecvDataOption3(CNWP(0x1576), 8, &recv_work);
    return 0;
}

void cnLBS_Send_ChatMessage(int a, int b) {
    __cnet_SendSet_ChatMessage(0, a, b);
}

void __cnet_SendSet_ChatMessage(int arg0, int arg1, int arg2) {
    SetSendCommand(&send_work, 0xE8);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatMessage(void) {
    memset(CnetSys_w.chat_from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat_d, GetRecvData8(&CnetSys_w.chat_c, GetRecvData8(&CnetSys_w.chat_b, GetRecvData8(&CnetSys_w.chat_a, GetRecvDataOption3(CnetSys_w.chat_msg, 0x100, GetRecvDataOption3(CnetSys_w.chat_x, 0x10, GetRecvDataOption3(CnetSys_w.chat_from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(5, 0);
}

void cnLBS_Send_ChatBinary(void) {
    __cnet_SendSet_ChatBinary();
}

void __cnet_SendSet_ChatBinary(int arg0, int arg1) {
    SetSendCommand(&send_work, 0xF6);
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatBinary(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

void _cnet_RecvFromLbs_AnswerChatMessageTU(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeChatMessageTU(void) {
    memset(CnetSys_w.chat_from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat_d, GetRecvData8(&CnetSys_w.chat_c, GetRecvData8(&CnetSys_w.chat_b, GetRecvData8(&CnetSys_w.chat_a, GetRecvDataOption3(CnetSys_w.chat_msg, 0x100, GetRecvDataOption3(CnetSys_w.chat_x, 0x10, GetRecvDataOption3(CnetSys_w.chat_from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(0x2A, 0);
}

void _cnet_RecvFromLbs_AnswerChatBinaryTU(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeChatBinaryTU(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

void _cnet_RecvFromLbs_MatchStart(void) {
    _cnetEvent_JumpCallBack(1, 0);
}

void _cnet_RecvFromLbs_MatchEntry(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_MatchStart(void) {
    SetSendCommand(&send_work, 0xA1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return 0;
}

int cnLBS_Read_MatchInfomation(int cb) {
    if (CnetSys_w.burst[7].state == 0) {
        memset(CnetSys_w.matchinfo, 0, 0x5D4);
        __cnet_SendReq_MatchJoin();
        CnetSys_w.burst[7].cb = (void *)cb;
        CnetSys_w.burst[7].state = 1;
        CnetSys_w.burst[7].x21 = 0;
        CnetSys_w.burst[7].run = 0;
        return 0;
    }
    return -1;
}

void __cnet_SendReq_MatchPlSide(int arg0) {
    SetSendCommand(&send_work, 0xA5);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentInfo(int arg0) {
    SetSendCommand(&send_work, 0xA9);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentStatus(int arg0) {
    SetSendCommand(&send_work, 0xAB);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchGameRule(void) {
    SetSendCommand(&send_work, 0xA7);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchBattleCode(void) {
    SetSendCommand(&send_work, 0xAE);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchMcsIpAddr(void) {
    SetSendCommand(&send_work, 0xB0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}

void _cnet_RecvFromLbs_NoticePatchFooter(void) {

}

int cnLBS_Answer_PatchFinish(void) {
    __cnet_Send_PatchFinish();
    return 0;
}

int cnLBS_Get_PatchInformation(u8 *p) {
    memset(p, 0, 0x1C);
    strncpy(p + 4, CnetSys_w.patch_a, 0xA);
    strncpy(p + 0x14, CnetSys_w.patch_b, 4);
    *(int *)p = CnetSys_w.patch_ver;
    return 0;
}

void _cnet_RecvFromLbs_RequestRegurationVersion(void) {

}

void _cnet_RecvFromLbs_NoticeRegurationAddress(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationData(void) {
    _cnet_RecvFromLbs_AnswerBrowserMethodGet();
}

void cnLBS_Send_RegurationAgree(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationAgree(void) {

}

int cnLBS_Get_ServerMessage(char *d) {
    strcpy(d, CNWP(0x378C0));
    return 0;
}

void _cnet_RecvFromLbs_RequestLineCheck(void) {
    __cnet_SendSet_LineCheck();
    CNW(s16, 0x10) = 1;
}

void __cnet_SendSet_LineCheck(void) {
    SetSendCommand(&send_work, 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeShutDown(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(2, 0);
}

void _cnet_RecvFromLbs_NoticeShutDownOpponent(void) {
    _cnetEvent_JumpCallBack(6, 0);
}

void _cnet_RecvFromLbs_NoticeMatchCancel(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(7, 0);
}

void _cnet_RecvFromLbs_NoticeLobbyFull(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(8, 0);
}
