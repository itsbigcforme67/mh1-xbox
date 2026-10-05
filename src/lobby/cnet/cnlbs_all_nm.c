/* cnlbs - lobby.bin network layer, machine-converted near-match draft (not compiled/linked). */
#include "lbnet.h"

int GetRecvData16();
int GetRecvData32();
int GetRecvData8();
int GetRecvDataOption();
int GetRecvDataOption3();
int GetRecvDataString();
int SetSendCommand();
int SetSendCommandLen();
int SetSendData16();
int SetSendData32();
int SetSendData8();
int SetSendEncodeStringData();
int SetSendResult();
int SetSendStringData();
int SetSendStringData2();
int Write_Socket();
int __cnet_Recv_Byte();
int __cnet_Recv_ByteByte();
int __cnet_Recv_ByteByteString();
int __cnet_Recv_ByteString();
int __cnet_Recv_Long();
int __cnet_Recv_ServerMessage();
int __cnet_Recv_Word();
int __cnet_Recv_WordByte();
int __cnet_Recv_WordLong();
int __cnet_Recv_WordWord();
int __cnet_SendReq_ConditionSearchUser();
int _cnet_RecvFromLbs_AnswerBrowserMethodGet();
int _sub_InOutRoomMember();
int memcmp();
int memcpy();
int memset();
int mmbbc_encode();
int select_ps2();
int strcpy();
int strlen();
int strncpy();
extern u8 __cnet_bgProg_ReadLobbyAllocation[];
extern u8 __cnet_bgProg_ReadPlazaAllocation[];
extern u8 __cnet_bgProg_ReadRoomAllocation[];
extern u8 __cnet_bgProg_ReadRoomRule[];
extern u8 __cnet_bgProg_RegistPersonalData[];
extern u8 __cnet_bgProg_RoomSetRule[];
extern u8 lit_108_0065E000[];
extern u8 lit_336_0065E008[];
extern u8 pFunc[];
extern u8 recv_header[];
extern u8 recv_work[];

void _cnet_RecvFromLbs_NoticeMailMessage(void) {
    __cnet_Recv_MailMessage();
    _cnetEvent_JumpCallBack(3, 0);
}

s32 cnLBS_SendMessage(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_SendMail(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void cnLBS_Get_RecvMessage(s32 arg0, void *arg1, void *arg2) {
    if (arg0 != 0) {
        strcpy(CNWP(0x308E4));
    }
    if (arg1 != NULL) {
        strcpy(arg1, CNWP(0x308EC));
    }
    if (arg2 != NULL) {
        strcpy(arg2, CNWP(0x30900));
    }
}

void _cnet_RecvFromLbs_AnswerSendMail(void) {
    _cnet_Return_CallBack();
}

s32 __cnet_SendReq_SendMail(s32 arg0, s32 arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xF1) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void __cnet_Recv_MailMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), GetRecvDataString(CNWP(0x308E4), &recv_work)));
}

s32 cnLBS_SerchUserPlace(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_SearchUser(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void cnLBS_Get_SerchUserPlace(u16 *arg0, u16 *arg1, u16 *arg2, u8 *arg3) {
    *arg0 = CNW(u16, 0x30980);
    *arg1 = CNW(u16, 0x30982);
    *arg2 = CNW(u16, 0x30984);
    *arg3 = CNW(u8, 0x30987);
}

void cnLBS_Get_SerchUserPlaceMessage(void) {
    strcpy(CNWP(0x30988));
}

void _cnet_RecvFromLbs_AnswerSearchUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_SearchUser();
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_SearchUser(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xEA) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void __cnet_Recv_SearchUser(void) {
    int sp10;

    GetRecvDataString(CNWP(0x30988), GetRecvData8(CNWP(0x30987), GetRecvData8(CNWP(0x30986), GetRecvData16(CNWP(0x30984), GetRecvData16(CNWP(0x30982), GetRecvData16(CNWP(0x30980), GetRecvDataString(&sp10, &recv_work)))))));
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

s16 cnLBS_ConditionSearchUser(void *arg0, int arg1) {
    int sp20;
    int *var_a2;
    s16 temp_v0_2;
    s16 var_v0;
    s32 var_a1;
    s8 temp_v0;
    void *var_a0;

    var_a0 = arg0;
    var_a2 = &sp20;
    var_a1 = 0x112;
    do {
        var_a1 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a0 + 1);
        *(s8 *)((u8 *)var_a2 + 0) = (s8) *(s8 *)((u8 *)var_a0 + 0);
        var_a0 += 2;
        *(s8 *)((u8 *)var_a2 + 1) = temp_v0;
        var_a2 += 2;
    } while (var_a1 > 0);
    memset(CNWP(0x39D8C), 0, 0x1CC4);
    temp_v0_2 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0_2 != -1) {
        CnetSys_w.bg[temp_v0_2].cmd = __cnet_SendReq_ConditionSearchUser(&sp20);
        CNW(s16, 0x3BA50) = temp_v0_2;
        var_v0 = temp_v0_2;
    }
    return var_v0;
}

s32 cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}

s32 __cnet_Send_ConditionSearchUserCertify(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xEE) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerConditionSearchUser(void) {
    u8 sp3F;
    u8 sp3E;
    u8 sp3D;
    u8 sp3C;
    int *var_v0;
    s32 var_s0;
    void *var_s1;

    if (CNW(s8, 0xFEC) == 0) {
        var_v0 = GetRecvData8(&sp3C, GetRecvData8(&sp3D, GetRecvData8(&sp3E, GetRecvData8(&sp3F, &recv_work))));
        var_s1 = &CnetSys_w + (sp3E * 0x5C) + 0x39D90;
        var_s0 = 0;
        if ((s32) sp3D > 0) {
            do {
                var_v0 = GetRecvDataOption3(var_s1 + 0x1C, 0x40, GetRecvDataOption3(var_s1 + 8, 0x10, GetRecvDataOption3(var_s1, 8, var_v0)));
                var_s0 += 1;
                var_s1 += 0x5C;
            } while (var_s0 < (s32) sp3D);
        }
        if (sp3C == 0) {
            *(CNWP(0x2E) + (CNW(u16, 0x3BA50) * 0x1C)) = __cnet_Send_ConditionSearchUserCertify((sp3D + sp3E) & 0xFF);
            return;
        }
        CNW(u8, 0x39D8C) = sp3F;
        goto block_8;
    }
block_8:
    _cnet_Return_CallBack(0);
}

s32 cnLBS_RegistPersonalData(void *arg0, s32 arg1) {
    s32 var_a0_2;
    s32 var_a2;
    s8 temp_v0;
    s8 temp_v0_2;
    void *var_a0;
    void *var_a2_2;
    void *var_a3;
    void *var_a3_2;

    var_a0 = arg0;
    var_a2 = 0xE8;
    var_a3 = sp;
    do {
        var_a2 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a0 + 1);
        *(s8 *)((u8 *)var_a3 + 0) = (s8) *(s8 *)((u8 *)var_a0 + 0);
        var_a0 += 2;
        *(s8 *)((u8 *)var_a3 + 1) = temp_v0;
        var_a3 += 2;
    } while (var_a2 > 0);
    if (CNW(u8, 0xF7C) == 0) {
        var_a3_2 = sp;
        var_a2_2 = CNWP(0x10FA);
        var_a0_2 = 0xE8;
        do {
            var_a0_2 -= 1;
            temp_v0_2 = *(s8 *)((u8 *)var_a3_2 + 1);
            *(s8 *)((u8 *)var_a2_2 + 0) = (s8) *(s8 *)((u8 *)var_a3_2 + 0);
            var_a3_2 += 2;
            *(s8 *)((u8 *)var_a2_2 + 1) = temp_v0_2;
            var_a2_2 += 2;
        } while (var_a0_2 > 0);
        CNW(s32, 0xF60) = arg1;
        CNW(u8, 0xF7C) = 1U;
        CNW(int *, 0xF5C) = &__cnet_bgProg_RegistPersonalData;
        CNW(s8, 0xF7D) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_RequestPersonalDataChange(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PersonalDataChange();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendReq_PersonalDataChange(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xB2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerPersonalDataChange(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_Send_PersonalData(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_SendSet_PersonalDataName();
        __cnet_SendSet_PersonalDataZip();
        __cnet_SendSet_PersonalDataAddress();
        __cnet_SendSet_PersonalDataTelephone();
        __cnet_SendSet_PersonalDataAge();
        __cnet_SendSet_PersonalDataMailAddress();
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PersonalDataRegisted();
        var_v0 = temp_v0;
    }
    return var_v0;
}

void __cnet_SendSet_PersonalDataName(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10FA);
    SetSendCommand(&send_work, 0xB4);
    SetSendEncodeStringData(&send_work, temp_s0, strlen(temp_s0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataZip(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10FA);
    SetSendCommand(&send_work, 0xB5);
    SetSendEncodeStringData(&send_work, temp_s0 + 0x41, strlen(temp_s0 + 0x41) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataAddress(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10FA);
    SetSendCommand(&send_work, 0xB6);
    SetSendEncodeStringData(&send_work, temp_s0 + 0x4C, strlen(temp_s0 + 0x4C) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataTelephone(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10FA);
    SetSendCommand(&send_work, 0xB7);
    SetSendEncodeStringData(&send_work, temp_s0 + 0xCD, strlen(temp_s0 + 0xCD) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataAge(void) {
    SetSendCommand(&send_work, 0xB8);
    SetSendData8(&send_work, CNW(u8, 0x1248));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataMailAddress(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10FA);
    SetSendCommand(&send_work, 0xB9);
    SetSendEncodeStringData(&send_work, temp_s0 + 0x14F, strlen(temp_s0 + 0x14F) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

s32 __cnet_SendReq_PersonalDataRegisted(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xBA) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerPersonalDataRegisted(void) {
    _cnet_Return_CallBack();
}

void _cnet_CallBack_Result_PersonalDataChange(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xF7E) = 1;
        return;
    }
    CNW(s8, 0xF7E) = 2;
}

void __cnet_KeepEntryFloorInfo(s32 arg0, s8 arg1) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        CNW(s8, 0x4054) = arg1;
        return;
    case 1:
        CNW(s8, 0x4055) = arg1;
        return;
    case 2:
        CNW(s8, 0x4056) = arg1;
        return;
    }
}

void __cnet_ClearEntryFloorInfo(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        CNW(s8, 0x405E) = 0;
        return;
    case 1:
        CNW(s8, 0x405F) = 0;
        return;
    case 2:
        CNW(s8, 0x4060) = 0;
        return;
    }
}

void __cnet_SetEntryFloorInfo(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        CNW(u8, 0x405E) = (u8) CNW(u8, 0x4054);
        return;
    case 1:
        CNW(u8, 0x405F) = (u8) CNW(u8, 0x4055);
        return;
    case 2:
        CNW(u8, 0x4060) = (u8) CNW(u8, 0x4056);
        return;
    }
}

s32 cnLBS_Read_PlazaAllocation(s32 arg1, s32 arg2) {
    if (CNW(u8, 0xE80) == 0) {
        CNW(s32, 0xE64) = arg2;
        CNW(s32, 0xE78) = (s32) (arg1 & 0xF);
        CNW(u8, 0xE80) = 1U;
        CNW(int *, 0xE60) = &__cnet_bgProg_ReadPlazaAllocation;
        CNW(s8, 0xE81) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Read_PlazaCount(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceCount(0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_PlazaCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x404E);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaNumOfPlaza(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x404E));
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Read_PlazaName(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceName(0, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_PlazaName(s32 arg0, int arg1) {
    strcpy(arg1, &CnetSys_w + (((arg0 & 0xFFFF) - 1) * 0x164) + 0x4082);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaName(void) {
    u16 sp5E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceName(&sp5E, &sp10);
        temp_a2 = CNWP(0x3F08);
        temp_a3 = sp5E * 0x164;
        *(temp_a2 + temp_a3) = sp5E;
        strcpy(&CnetSys_w + ((sp5E - 1) * 0x164) + 0x4082, &sp10, temp_a2, temp_a3);
        CNW(u16, 0x6CEC) = sp5E;
        strcpy(CNWP(0x6D02), &sp10);
        temp_v1 = CNWP(0x3F00) + (sp5E * 0x164);
        *temp_v1 |= 4;
    }
    _cnet_Return_CallBack(0xE);
}

s32 cnLBS_Read_PlazaJoinUser(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceJoinUser(0, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_mhPlazaJoinUser(s32 arg0, u16 *arg1, u16 *arg2) {
    s32 temp_a3;

    temp_a3 = (arg0 & 0xFFFF) * 0x164;
    *arg1 = *(CNWP(0x3F0A) + temp_a3);
    *arg2 = *(CNWP(0x3F0C) + temp_a3);
    return 0;
}

void _cnet_RecvFromLbs_BothPlazaJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4064));
    }
    _cnet_Return_CallBack(0xF);
}

s32 cnLBS_Read_PlazaStatus(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceStatus(0, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_PlazaStatus(s32 arg0, u8 *arg1) {
    *arg1 = *(CNWP(0x3F1C) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

void _cnet_RecvFromLbs_BothPlazaStatus(u16 *arg1, u16 arg2, s32 arg3) {
    u8 sp1F;
    u16 sp1C;
    s32 *temp_v1;
    s32 var_a3;
    u16 *var_a1;
    u16 var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceStatus(&sp1C, &sp1F);
        var_a2 = sp1C;
        var_a3 = var_a2 * 0x164;
        var_a1 = CNWP(0x3F08) + var_a3;
        *var_a1 = var_a2;
        temp_v1 = CNWP(0x3F00) + var_a3;
        *(CNWP(0x3F1C) + var_a3) = sp1F;
        CNW(u16, 0x6CEC) = var_a2;
        CNW(u8, 0x6D00) = sp1F;
        *temp_v1 |= 2;
    }
    _cnet_Return_CallBack(0x10, var_a1, var_a2, var_a3);
}

s32 cnLBS_Read_PlazaExplain(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExplain(0, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_BothPlazaExplain(void) {
    u16 sp11E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceExplain(&sp11E, &sp10);
        temp_a2 = CNWP(0x3F08);
        temp_a3 = sp11E * 0x164;
        *(temp_a2 + temp_a3) = sp11E;
        strcpy(&CnetSys_w + ((sp11E - 1) * 0x164) + 0x40C4, &sp10, temp_a2, temp_a3);
        temp_v1 = CNWP(0x3F00) + (sp11E * 0x164);
        *temp_v1 |= 8;
    }
    _cnet_Return_CallBack(0x11);
}

s32 cnLBS_PlazaEntry(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_KeepEntryFloorInfo(0, arg0, __cnet_SendReq_PieceEntry(0, arg0) & 0xFFFF);
        var_v0 = temp_v0;
        CnetSys_w.bg[temp_v0].cmd = M2C_ERROR(/* Read from unset register $a2 */);
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerPlazaEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_PlazaExit(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExit(0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerPlazaExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Read_LobbyAllocation(s32 arg1, s32 arg2) {
    if (CNW(u8, 0xEA4) == 0) {
        CNW(s32, 0xE88) = arg2;
        CNW(s32, 0xE9C) = (s32) (arg1 & 0xF);
        CNW(u8, 0xEA4) = 1U;
        CNW(int *, 0xE84) = &__cnet_bgProg_ReadLobbyAllocation;
        CNW(s8, 0xEA5) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Read_LobbyCount(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceCount(1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_LobbyCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4050);
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyNumOfLobby(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4050));
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Read_LobbyName(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceName(1, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_LobbyName(s32 arg0, int arg1) {
    strcpy(arg1, &CnetSys_w + (((arg0 & 0xFFFF) - 1) * 0x164) + 0x4E6A);
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyName(void) {
    u16 sp5E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceName(&sp5E, &sp10);
        temp_a2 = CNWP(0x4CF0);
        temp_a3 = sp5E * 0x164;
        *(temp_a2 + temp_a3) = sp5E;
        strcpy(&CnetSys_w + ((sp5E - 1) * 0x164) + 0x4E6A, &sp10, temp_a2, temp_a3);
        CNW(u16, 0x6CEC) = sp5E;
        strcpy(CNWP(0x6D02), &sp10);
        temp_v1 = CNWP(0x4CE8) + (sp5E * 0x164);
        *temp_v1 |= 4;
    }
    _cnet_Return_CallBack(0x13);
}

s32 cnLBS_Read_LobbyJoinUser(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceJoinUser(1, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_mhLobbyJoinUser(s32 arg0, u16 *arg1, u16 *arg2) {
    s32 temp_a3;

    temp_a3 = (arg0 & 0xFFFF) * 0x164;
    *arg1 = *(CNWP(0x4CF2) + temp_a3);
    *arg2 = *(CNWP(0x4CF4) + temp_a3);
    return 0;
}

void _cnet_RecvFromLbs_BothLobbyJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4E4C));
    }
    _cnet_Return_CallBack(0x14);
}

s32 cnLBS_Read_LobbyStatus(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceStatus(1, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_LobbyStatus(s32 arg0, u8 *arg1) {
    *arg1 = *(CNWP(0x4D04) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

void _cnet_RecvFromLbs_BothLobbyStatus(u16 *arg1, u16 arg2, s32 arg3) {
    u8 sp1F;
    u16 sp1C;
    s32 *temp_v1;
    s32 var_a3;
    u16 *var_a1;
    u16 var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceStatus(&sp1C, &sp1F);
        var_a2 = sp1C;
        var_a3 = var_a2 * 0x164;
        var_a1 = CNWP(0x4CF0) + var_a3;
        *var_a1 = var_a2;
        temp_v1 = CNWP(0x4CE8) + var_a3;
        *(CNWP(0x4D04) + var_a3) = sp1F;
        CNW(u16, 0x6CEC) = var_a2;
        CNW(u8, 0x6D00) = sp1F;
        *temp_v1 |= 2;
    }
    _cnet_Return_CallBack(0x15, var_a1, var_a2, var_a3);
}

s32 cnLBS_Read_LobbyExplain(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExplain(1, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerLobbyExplain(void) {
    u16 sp11E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceExplain(&sp11E, &sp10);
        temp_a2 = CNWP(0x4CF0);
        temp_a3 = sp11E * 0x164;
        *(temp_a2 + temp_a3) = sp11E;
        strcpy(&CnetSys_w + ((sp11E - 1) * 0x164) + 0x4EAC, &sp10, temp_a2, temp_a3);
        temp_v1 = CNWP(0x4CE8) + (sp11E * 0x164);
        *temp_v1 |= 8;
    }
    _cnet_Return_CallBack(0x16);
}

s32 cnLBS_LobbyEntry(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_KeepEntryFloorInfo(1, arg0, __cnet_SendReq_PieceEntry(1, arg0) & 0xFFFF);
        var_v0 = temp_v0;
        CnetSys_w.bg[temp_v0].cmd = M2C_ERROR(/* Read from unset register $a2 */);
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerLobbyEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_LobbyExit(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExit(1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerLobbyExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Read_RoomAllocation(s32 arg1, s32 arg2) {
    if (CNW(u8, 0xEC8) == 0) {
        CNW(s32, 0xEAC) = arg2;
        CNW(s32, 0xEC0) = (s32) (arg1 & 0xFF);
        CNW(u8, 0xEC8) = 1U;
        CNW(int *, 0xEA8) = &__cnet_bgProg_ReadRoomAllocation;
        CNW(s8, 0xEC9) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Read_RoomCount(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceCount(2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4052);
    return 0;
}

void _cnet_RecvFromLbs_AnswerRoomNumOfRoom(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4052));
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Read_RoomJoinUser(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceJoinUser(2, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_mhRoomJoinUser(s32 arg0, u16 *arg1, u16 *arg2) {
    s32 temp_a3;

    temp_a3 = (arg0 & 0xFFFF) * 0x164;
    *arg1 = *(CNWP(0x606A) + temp_a3);
    *arg2 = *(CNWP(0x606C) + temp_a3);
    return 0;
}

void _cnet_RecvFromLbs_BothRoomJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x61C4));
    }
    _cnet_Return_CallBack(0x19);
}

s32 cnLBS_Read_RoomStatus(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceStatus(2, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomStatus(s32 arg0, u8 *arg1) {
    *arg1 = *(CNWP(0x607C) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

void _cnet_RecvFromLbs_BothRoomStatus(u16 *arg1, u16 arg2, s32 arg3) {
    u8 sp1F;
    u16 sp1C;
    s32 *temp_v1;
    s32 var_a3;
    u16 *var_a1;
    u16 var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceStatus(&sp1C, &sp1F);
        var_a2 = sp1C;
        var_a3 = var_a2 * 0x164;
        var_a1 = CNWP(0x6068) + var_a3;
        *var_a1 = var_a2;
        temp_v1 = CNWP(0x6060) + var_a3;
        *(CNWP(0x607C) + var_a3) = sp1F;
        CNW(u16, 0x6CEC) = var_a2;
        CNW(u8, 0x6D00) = sp1F;
        *temp_v1 |= 2;
    }
    _cnet_Return_CallBack(0x1A, var_a1, var_a2, var_a3);
}

s32 cnLBS_Read_RoomName(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceName(2, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomName(s32 arg0, int arg1) {
    strcpy(arg1, &CnetSys_w + (((arg0 & 0xFFFF) - 1) * 0x164) + 0x61E2);
    return 0;
}

void _cnet_RecvFromLbs_BothRoomName(void) {
    u16 sp11E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceName(&sp11E, &sp10);
        temp_a2 = CNWP(0x6068);
        temp_a3 = sp11E * 0x164;
        *(temp_a2 + temp_a3) = sp11E;
        strcpy(&CnetSys_w + ((sp11E - 1) * 0x164) + 0x61E2, &sp10, temp_a2, temp_a3);
        CNW(u16, 0x6CEC) = sp11E;
        strcpy(CNWP(0x6D02), &sp10);
        temp_v1 = CNWP(0x6060) + (sp11E * 0x164);
        *temp_v1 |= 4;
    }
    _cnet_Return_CallBack(0x18);
}

s32 cnLBS_Read_RoomExplain(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExplain(2, arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomExplain(s32 arg0, int arg1) {
    strcpy(arg1, &CnetSys_w + (((arg0 & 0xFFFF) - 1) * 0x164) + 0x6224);
    return 0;
}

void _cnet_RecvFromLbs_BothRoomExplain(void) {
    u16 sp11E;
    int sp10;
    s32 *temp_v1;
    s32 temp_a3;
    void *temp_a2;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_PieceExplain(&sp11E, &sp10);
        temp_a2 = CNWP(0x6068);
        temp_a3 = sp11E * 0x164;
        *(temp_a2 + temp_a3) = sp11E;
        strcpy(&CnetSys_w + ((sp11E - 1) * 0x164) + 0x6224, &sp10, temp_a2, temp_a3);
        temp_v1 = CNWP(0x6060) + (sp11E * 0x164);
        *temp_v1 |= 8;
    }
    _cnet_Return_CallBack(0x1B);
}

s32 cnLBS_Read_RoomJoinInfo(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomJoinInfo(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomJoinInfo(s32 arg0, u16 *arg1, u16 *arg2, u16 *arg3) {
    s32 temp_t5;

    temp_t5 = (arg0 & 0xFFFF) * 0x164;
    *arg1 = *(CNWP(0x606E) + temp_t5);
    *arg2 = *(CNWP(0x6070) + temp_t5);
    *arg3 = *(CNWP(0x6072) + temp_t5);
    *M2C_ERROR(/* Read from unset register $t0 */) = *(CNWP(0x6074) + temp_t5);
    *M2C_ERROR(/* Read from unset register $t1 */) = *(CNWP(0x6076) + temp_t5);
    return 0;
}

s32 __cnet_SendReq_RoomJoinInfo(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x8D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_BothRoomJoinInfo(void) {
    u16 sp2E;
    void *temp_s0;
    void *temp_v0;

    if (CNW(s8, 0xFEC) == 0) {
        temp_v0 = &CnetSys_w + ((sp2E - 1) * 0x164);
        temp_s0 = temp_v0 + 0x61C4;
        GetRecvData16(temp_s0 + 0x16, GetRecvData16(temp_s0 + 0x12, GetRecvData16(temp_s0 + 0x14, GetRecvData16(temp_s0 + 0x10, GetRecvData16(temp_s0 + 0xE, GetRecvData16(&sp2E, &recv_work), sp2E)))));
        *(s32 *)((u8 *)temp_v0 + 0x61C4) = (s32) (*(s32 *)((u8 *)temp_v0 + 0x61C4) | 0x20);
    }
    _cnet_Return_CallBack(0x1D);
}

s32 cnLBS_RoomCreate(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_KeepEntryFloorInfo(2, arg0, __cnet_SendReq_RoomCreate(arg0) & 0xFFFF);
        var_v0 = temp_v0;
        CnetSys_w.bg[temp_v0].cmd = M2C_ERROR(/* Read from unset register $a2 */);
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_Set_RoomRule(s8 *arg0, s32 arg1) {
    s32 var_v1;
    s8 *var_a0;
    s8 *var_a2;

    var_a0 = arg0;
    var_v1 = 0x16B;
    var_a2 = CNWP(0x12CA);
    do {
        var_v1 -= 1;
        *var_a2 = *var_a0;
        var_a0 += 1;
        var_a2 += 1;
    } while (var_v1 > 0);
    if (CNW(u8, 0xF10) == 0) {
        CNW(s32, 0xEF4) = arg1;
        CNW(u8, 0xF10) = 1U;
        CNW(int *, 0xEF0) = &__cnet_bgProg_RoomSetRule;
        CNW(s8, 0xF11) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Set_RoomRuleFinish(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomSetFinish();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_RoomEntry(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_KeepEntryFloorInfo(2, arg0, __cnet_SendReq_RoomEntry(arg0, arg1) & 0xFFFF);
        var_v0 = temp_v0;
        CnetSys_w.bg[temp_v0].cmd = M2C_ERROR(/* Read from unset register $a2 */);
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerRoomEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

s32 cnLBS_RoomExit(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_PieceExit(2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomProperty(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomProperty(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomProperty(s32 arg0, s32 *arg1) {
    *arg1 = *(CNWP(0x6064) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

s32 __cnet_SendReq_RoomProperty(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x98) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_BothRoomProperty(u16 *arg1, u16 arg2, s32 arg3) {
    u16 sp1E;
    s32 sp18;
    s32 *temp_v1;
    s32 var_a3;
    u16 *var_a1;
    u16 var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_WordLong(&sp1E, &sp18);
        var_a2 = sp1E;
        var_a3 = var_a2 * 0x164;
        var_a1 = CNWP(0x6068) + var_a3;
        *var_a1 = var_a2;
        temp_v1 = CNWP(0x6060) + var_a3;
        *(CNWP(0x6064) + var_a3) = sp18;
        CNW(u16, 0x6CEC) = var_a2;
        *temp_v1 |= 0x80;
    }
    _cnet_Return_CallBack(0x29, var_a1, var_a2, var_a3);
}

s32 cnLBS_Set_RoomProperty(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_SetRoomProperty(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendReq_SetRoomProperty(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x96) & 0xFFFF;
    SetSendData32(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerSetRoomProperty(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_Get_RoomLeaveUser(void *arg0) {
    s32 var_a1;
    s8 temp_v0;
    void *var_a0;
    void *var_a2;

    var_a0 = arg0;
    var_a1 = 0x2E;
    var_a2 = CNWP(0x3692);
    do {
        var_a1 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a2 + 1);
        *(s8 *)((u8 *)var_a0 + 0) = (s8) *(s8 *)((u8 *)var_a2 + 0);
        var_a2 += 2;
        *(s8 *)((u8 *)var_a0 + 1) = temp_v0;
        var_a0 += 2;
    } while (var_a1 > 0);
    return 0;
}

void _cnet_RecvFromLbs_NoticeRoomCommer(void) {
    _sub_InOutRoomMember();
}

void _cnet_RecvFromLbs_NoticeRoomLeaver(void) {
    _sub_InOutRoomMember();
}

void _sub_ReceiveJoinUser(s32 arg0) {
    u16 sp2E;
    u16 sp2C;
    u16 sp2A;
    void *temp_s0;
    void *temp_s0_2;

    if (CNW(s8, 0xFEC) == 0) {
        if (CNW(u8, 0x10D2) != 4) {
            __cnet_Recv_PieceJoinUser(&sp2E, &sp2C);
            temp_s0 = arg0 + ((sp2E - 1) * 0x164);
            *(u16 *)((u8 *)temp_s0 + 8) = sp2E;
            *(u16 *)((u8 *)temp_s0 + 0xA) = sp2C;
            *(s32 *)((u8 *)temp_s0 + 0) = (s32) (*(s32 *)((u8 *)temp_s0 + 0) | 1);
        } else if (CNW(u8, 0x10D2) == 4) {
            __cnet_Recv_PieceJoinUserMH(&sp2E, &sp2C, &sp2A);
            temp_s0_2 = arg0 + ((sp2E - 1) * 0x164);
            *(u16 *)((u8 *)temp_s0_2 + 8) = sp2E;
            *(u16 *)((u8 *)temp_s0_2 + 0xA) = sp2C;
            *(u16 *)((u8 *)temp_s0_2 + 0xC) = sp2A;
            *(s32 *)((u8 *)temp_s0_2 + 0) = (s32) (*(s32 *)((u8 *)temp_s0_2 + 0) | 1);
        }
        CNW(u16, 0x6CEC) = sp2E;
        CNW(u16, 0x6CEE) = sp2C;
    }
}

s32 cnLBS_Read_RoomRuleAllocation(s32 arg0, s32 arg1) {
    if (CNW(u8, 0xEEC) == 0) {
        CNW(s32, 0xED0) = arg1;
        CNW(s32, 0xEE0) = (s32) (arg0 & 0xFFFF);
        CNW(u8, 0xEEC) = 1U;
        CNW(int *, 0xECC) = &__cnet_bgProg_ReadRoomRule;
        CNW(s8, 0xEED) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Read_RoomRuleCount(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_NumOfRule(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomNamePermission(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomNamePermission(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerRoomNamePermission(void) {
    u8 sp1F;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Byte(&sp1F);
        CNW(u8, 0x6E48) = sp1F;
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_RoomNamePermission(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x55) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Read_RoomPasswordPermission(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomPasswordPermission(arg0 & 0xFFFF);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerRoomPasswordPermission(void) {
    u8 sp1F;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Byte(&sp1F);
        CNW(u8, 0x6E49) = sp1F;
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_RoomPasswordPermission(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x57) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Read_RoomPasswordInfo(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomPasswordInfo(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomPasswordInfo(s32 arg0, u8 *arg1) {
    *arg1 = *(CNWP(0x607D) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

s32 __cnet_SendReq_RoomPasswordInfo(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x86) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_BothRoomPasswordInfo(u8 arg1, u16 arg2, s32 arg3) {
    u8 sp1F;
    u16 sp1C;
    s32 *temp_v1;
    s32 var_a3;
    u16 var_a2;
    u8 var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_WordByte(&sp1C, &sp1F);
        var_a2 = sp1C;
        var_a1 = sp1F;
        var_a3 = var_a2 * 0x164;
        temp_v1 = CNWP(0x6060) + var_a3;
        *(CNWP(0x607D) + var_a3) = var_a1;
        *temp_v1 |= 0x10;
        CNW(u16, 0x6CEC) = var_a2;
        CNW(u8, 0x6D01) = var_a1;
    }
    _cnet_Return_CallBack(0x1E, var_a1, var_a2, var_a3);
}

s32 cnLBS_Read_RoomMemberList(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    memset(CNWP(0x39EE), 0, 0x300);
    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomMember(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_RoomMemberList(s32 arg0, int arg1, int arg2, int arg3) {
    void *temp_s2;

    temp_s2 = &CnetSys_w + ((arg0 & 0xFFFF) * 0x60);
    strcpy(arg1, temp_s2 + 0x39EE);
    strcpy(arg2, temp_s2 + 0x39F6);
    memcpy(arg3, temp_s2 + 0x3A0A, 0x40);
    return 0;
}

s32 __cnet_SendReq_RoomMember(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x8B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerRoomMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_RoomMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_RoomMember(void) {
    memset(CNWP(0x39EE), 0, 0x300);
    __cnet_Recv_MemberSub(CNWP(0x302ED), CNWP(0x39EE));
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

s32 cnLBS_TopPageJump(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_TopPageJump();
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerTopPageJump(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
        __cnet_ClearEntryFloorInfo(1);
        __cnet_ClearEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_TopPageJump(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x2C) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Read_RoomRuleCaption(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleListHeadWord(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomRuleChoiceCount(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleNumOfChoice(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomRuleNow(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleListNow(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomRuleChoicePermission(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleListPermission(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomRuleChoiceName(s32 arg0, int arg1, int arg2, int arg3) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg3);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleListName(arg0, arg1, arg2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Read_RoomRuleChoiceControl(s32 arg0, int arg1, int arg2, int arg3) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg3);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RuleControl(arg0, arg1, arg2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnetGet_Room_LastRecvNumber(u16 *arg0) {
    *arg0 = CNW(u16, 0x6CEC);
    return 0;
}

s32 cnLBS_Get_RoomRuleAllocation(void *arg1) {
    s32 var_a0;
    s8 temp_v0;
    void *var_a1;
    void *var_a2;

    var_a1 = arg1;
    var_a2 = CNWP(0x6E48);
    var_a0 = 0x14A52;
    do {
        var_a0 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a2 + 1);
        *(s8 *)((u8 *)var_a1 + 0) = (s8) *(s8 *)((u8 *)var_a2 + 0);
        var_a2 += 2;
        *(s8 *)((u8 *)var_a1 + 1) = temp_v0;
        var_a1 += 2;
    } while (var_a0 > 0);
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

void _cnet_RecvFromLbs_AnswerRuleListHeadWord(void) {
    u8 sp5F;
    int sp10;
    u8 *temp_v1;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ByteString(&sp5F, &sp10);
        strcpy(&CnetSys_w + (sp5F * 0x14A5) + 0x6E4D, &sp10);
        temp_v1 = CNWP(0x6E4C) + (sp5F * 0x14A5);
        *temp_v1 |= 1;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListPermission(u8 arg1, s32 arg2) {
    u8 sp1F;
    u8 sp1E;
    s32 var_a2;
    u8 *temp_v1;
    u8 var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ByteByte(&sp1F, &sp1E);
        var_a1 = sp1E;
        var_a2 = sp1F * 0x14A5;
        temp_v1 = CNWP(0x6E4C) + var_a2;
        *(CNWP(0x6E8E) + var_a2) = var_a1;
        *temp_v1 |= 8;
    }
    _cnet_Return_CallBack(0, var_a1, var_a2);
}

void _cnet_RecvFromLbs_AnswerRuleListNow(u8 arg1, s32 arg2) {
    u8 sp1F;
    u8 sp1E;
    s32 var_a2;
    u8 *temp_v1;
    u8 var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ByteByte(&sp1F, &sp1E);
        var_a1 = sp1E;
        var_a2 = sp1F * 0x14A5;
        temp_v1 = CNWP(0x6E4C) + var_a2;
        *(CNWP(0x6E90) + var_a2) = var_a1;
        *temp_v1 |= 2;
    }
    _cnet_Return_CallBack(0, var_a1, var_a2);
}

void _cnet_RecvFromLbs_AnswerRuleListNumOf(u8 arg1, s32 arg2) {
    u8 sp1F;
    u8 sp1E;
    s32 var_a2;
    u8 *temp_v1;
    u8 var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ByteByte(&sp1F, &sp1E);
        var_a1 = sp1E;
        var_a2 = sp1F * 0x14A5;
        temp_v1 = CNWP(0x6E4C) + var_a2;
        *(CNWP(0x6E8F) + var_a2) = var_a1;
        *temp_v1 |= 4;
    }
    _cnet_Return_CallBack(0, var_a1, var_a2);
}

void _cnet_RecvFromLbs_AnswerRuleListName(u8 arg1) {
    u8 sp5F;
    u8 sp5E;
    int sp10;
    s32 temp_a2;
    u8 *temp_v1;
    u8 var_a1;

    var_a1 = arg1;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ByteByteString(&sp5F, &sp5E, &sp10);
        temp_a2 = sp5F * 0x14A5;
        strcpy((sp5E * 0x41) + (&CnetSys_w + temp_a2) + 0x6EB1, &sp10, temp_a2);
        var_a1 = sp5F;
        temp_v1 = sp5E + (CNWP(0x6E91) + (var_a1 * 0x14A5));
        *temp_v1 |= 1;
    }
    _cnet_Return_CallBack(0, var_a1);
}

void _cnet_RecvFromLbs_AnswerRuleControl(u8 *arg1, u8 arg2, u8 arg3) {
    u8 sp1F;
    u8 sp1E;
    void *sp18;
    s32 temp_a1;
    s32 var_a1_2;
    u8 *temp_v1;
    u8 *var_a1;
    u8 temp_a0;
    u8 var_a2;
    u8 var_a3;
    void *temp_a3;
    void *temp_a3_2;
    void *temp_a3_3;
    void *temp_a3_4;
    void *temp_a3_5;
    void *temp_a3_6;
    void *temp_a3_7;
    void *var_a2_2;
    void *var_v0;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = arg3;
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_RuleControl(&sp1F, &sp1E, &sp18);
        var_a1_2 = 0;
        temp_a0 = *(u8 *)((u8 *)sp18 + 0);
        sp18 += 1;
        if ((s32) temp_a0 > 0) {
            if ((s32) temp_a0 >= 9) {
                var_v0 = (sp1E * 0x60) + (&CnetSys_w + (sp1F * 0x14A5));
                do {
                    var_a1_2 += 8;
                    *(u8 *)((u8 *)var_v0 + 0x76F1) = (u8) *(u8 *)((u8 *)sp18 + 0);
                    *(u8 *)((u8 *)var_v0 + 0x76F2) = (u8) *(u8 *)((u8 *)sp18 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x76F3) = (u8) *(u8 *)((u8 *)sp18 + 2);
                    temp_a3 = sp18 + 3;
                    sp18 = temp_a3;
                    *(u8 *)((u8 *)var_v0 + 0x76F4) = (u8) *(u8 *)((u8 *)sp18 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x76F5) = (u8) *(u8 *)((u8 *)temp_a3 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x76F6) = (u8) *(u8 *)((u8 *)temp_a3 + 2);
                    temp_a3_2 = temp_a3 + 3;
                    sp18 = temp_a3_2;
                    *(u8 *)((u8 *)var_v0 + 0x76F7) = (u8) *(u8 *)((u8 *)temp_a3 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x76F8) = (u8) *(u8 *)((u8 *)temp_a3_2 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x76F9) = (u8) *(u8 *)((u8 *)temp_a3_2 + 2);
                    temp_a3_3 = temp_a3_2 + 3;
                    sp18 = temp_a3_3;
                    *(u8 *)((u8 *)var_v0 + 0x76FA) = (u8) *(u8 *)((u8 *)temp_a3_2 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x76FB) = (u8) *(u8 *)((u8 *)temp_a3_3 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x76FC) = (u8) *(u8 *)((u8 *)temp_a3_3 + 2);
                    temp_a3_4 = temp_a3_3 + 3;
                    sp18 = temp_a3_4;
                    *(u8 *)((u8 *)var_v0 + 0x76FD) = (u8) *(u8 *)((u8 *)temp_a3_3 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x76FE) = (u8) *(u8 *)((u8 *)temp_a3_4 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x76FF) = (u8) *(u8 *)((u8 *)temp_a3_4 + 2);
                    temp_a3_5 = temp_a3_4 + 3;
                    sp18 = temp_a3_5;
                    *(u8 *)((u8 *)var_v0 + 0x7700) = (u8) *(u8 *)((u8 *)temp_a3_4 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x7701) = (u8) *(u8 *)((u8 *)temp_a3_5 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x7702) = (u8) *(u8 *)((u8 *)temp_a3_5 + 2);
                    temp_a3_6 = temp_a3_5 + 3;
                    sp18 = temp_a3_6;
                    *(u8 *)((u8 *)var_v0 + 0x7703) = (u8) *(u8 *)((u8 *)temp_a3_5 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x7704) = (u8) *(u8 *)((u8 *)temp_a3_6 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x7705) = (u8) *(u8 *)((u8 *)temp_a3_6 + 2);
                    temp_a3_7 = temp_a3_6 + 3;
                    sp18 = temp_a3_7;
                    *(u8 *)((u8 *)var_v0 + 0x7706) = (u8) *(u8 *)((u8 *)temp_a3_6 + 3);
                    *(u8 *)((u8 *)var_v0 + 0x7707) = (u8) *(u8 *)((u8 *)temp_a3_7 + 1);
                    *(u8 *)((u8 *)var_v0 + 0x7708) = (u8) *(u8 *)((u8 *)temp_a3_7 + 2);
                    var_v0 += 0x18;
                    sp18 = temp_a3_7 + 3;
                } while (var_a1_2 < (temp_a0 - 8));
            }
            if (var_a1_2 < (s32) temp_a0) {
                var_a2_2 = (var_a1_2 * 3) + ((sp1E * 0x60) + (&CnetSys_w + (sp1F * 0x14A5)));
                do {
                    var_a1_2 += 1;
                    *(u8 *)((u8 *)var_a2_2 + 0x76F1) = (u8) *(u8 *)((u8 *)sp18 + 0);
                    *(u8 *)((u8 *)var_a2_2 + 0x76F2) = (u8) *(u8 *)((u8 *)sp18 + 1);
                    *(u8 *)((u8 *)var_a2_2 + 0x76F3) = (u8) *(u8 *)((u8 *)sp18 + 2);
                    var_a2_2 += 3;
                    sp18 += 3;
                } while (var_a1_2 < (s32) temp_a0);
            }
        }
        var_a3 = sp1F;
        var_a2 = sp1E;
        temp_a1 = var_a3 * 0x14A5;
        var_a1 = var_a2 + (CNWP(0x76D1) + temp_a1);
        temp_v1 = var_a2 + (CNWP(0x6E91) + temp_a1);
        *var_a1 = temp_a0;
        *temp_v1 |= 2;
    }
    _cnet_Return_CallBack(0, var_a1, var_a2, var_a3);
}

void _cnet_RecvFromLbs_AnswerRoomSetName(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetRule(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetFinish(void) {
    _cnet_Return_CallBack();
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

void _cnet_RecvFromLbs_AnswerRoomRestTime(void) {
    int sp1E;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_WordWord(&sp1E, CNWP(0x302FE));
    }
    _cnet_Return_CallBack(0);
}

void __cnet_CallBack_Result_Plaza_NumOfPlaza_005A6E20(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xE82) = 1;
        return;
    }
    CNW(s8, 0xE82) = 2;
}

void _cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60(s64 arg0) {
    s8 sp19;
    s8 sp18;
    s64 sp10;

    sp10 = arg0;
    if ((s8) sp10 == 0) {
        sp18 = 2;
        sp19 = 0xB;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE64)((s64) sp18, &sp18);
        CNW(s8, 0xE82) = 1;
        return;
    }
    CNW(s8, 0xE82) = 2;
}

void _cnet_CallBack_Result_LobbyCount(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xEA6) = 1;
        return;
    }
    CNW(s8, 0xEA6) = 2;
}

void _cnet_CallBack_Result_LobbyAllocation(s64 arg0) {
    s8 sp19;
    s8 sp18;
    s64 sp10;

    sp10 = arg0;
    if ((s8) sp10 == 0) {
        sp18 = 2;
        sp19 = 0xB;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE88)((s64) sp18, &sp18);
        CNW(s8, 0xEA6) = 1;
        return;
    }
    CNW(s8, 0xEA6) = 2;
}

void _cnet_CallBack_Result_Room_NumOfRoom(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xECA) = 1;
        return;
    }
    CNW(s8, 0xECA) = 2;
}

void _cnet_CallBack_Result_RoomJoinJoinUser(s64 arg0) {
    s8 sp19;
    s8 sp18;
    s64 sp10;

    sp10 = arg0;
    if ((s8) sp10 == 0) {
        sp18 = 2;
        sp19 = 0xB;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xEAC)((s64) sp18, &sp18);
        CNW(s8, 0xECA) = 1;
        return;
    }
    CNW(s8, 0xECA) = 2;
}

s32 cnLBS_Get_AllocationProgressCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x1032);
    return 0;
}

void _cnet_CallBack_Result_Rule_NumOfRule(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xEEE) = 1;
        return;
    }
    CNW(s8, 0xEEE) = 2;
}

void _cnet_CallBack_Result_RoomRuleCaption(void) {
    s8 sp19;
    s8 sp18;

    sp18 = 2;
    sp19 = 0xB;
    M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xED0)((s64) sp18, &sp18);
}

void _cnet_CallBack_Result_RoomSetFinish(s64 arg0) {
    s64 sp8;

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        CNW(s8, 0xF12) = 1;
        return;
    }
    CNW(s8, 0xF12) = 2;
}

s32 __cnet_SendReq_PieceCount(s32 arg0) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x32) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x46) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x7B) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_PieceName(s32 arg0, int arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x34) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x48) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x7D) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_PieceJoinUser(s32 arg0, int arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x36) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x4A) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x80) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_PieceStatus(s32 arg0, int arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x39) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x4D) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x83) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_PieceExplain(s32 arg0, int arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x3C) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x50) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x90) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_PieceEntry(s32 arg0, int arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x30) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x42) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x73) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_RoomEntry(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x73) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_PieceExit(s32 arg0) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = saved_reg_s0;
    temp_v1 = arg0 & 0xFFFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s0 = SetSendCommand(&send_work, 0x3F) & 0xFFFF;
        break;
    case 1:
        var_s0 = SetSendCommand(&send_work, 0x44) & 0xFFFF;
        break;
    case 2:
        var_s0 = SetSendCommand(&send_work, 0x75) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return var_s0;
}

s32 __cnet_SendReq_NumOfRule(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x59) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleListHeadWord(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x5B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleNumOfChoice(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x61) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleListNow(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x5F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleListPermission(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x5D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleListName(s32 arg0, int arg1, int arg2) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x63) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RuleControl(s32 arg0, int arg1, int arg2) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x65) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void __cnet_Recv_GameJoin(void) {

}

void __cnet_Recv_NumOfPiece(void) {
    GetRecvData16(&recv_work);
}

void __cnet_Recv_PieceName(int arg1) {
    GetRecvDataString(arg1, GetRecvData16(&recv_work));
}

void __cnet_Recv_PieceJoinUser(int *arg1) {
    GetRecvData16(arg1, GetRecvData16(&recv_work));
}

void __cnet_Recv_PieceJoinUserMH(int *arg1, int *arg2) {
    GetRecvData16(arg2, GetRecvData16(arg1, GetRecvData16(&recv_work)));
}

void __cnet_Recv_PieceStatus(int arg1) {
    GetRecvData8(arg1, GetRecvData16(&recv_work));
}

void __cnet_Recv_PieceExplain(int arg1) {
    GetRecvDataString(arg1, GetRecvData16(&recv_work));
}

void __cnet_Recv_RuleControl(int *arg1, s32 *arg2) {
    *arg2 = GetRecvData8(arg1, GetRecvData8(&recv_work));
}

s32 __cnet_SendReq_RoomCreate(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x53) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RoomSetName(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x67) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RoomSetPassword(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x69) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RoomSetRule(s32 arg0, s32 arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x6B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 __cnet_SendReq_RoomSetFinish(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x6D) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntry(void) {
    _cnet_Return_CallBack();
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x302F4), GetRecvData16(CNWP(0x302F2), GetRecvData16(CNWP(0x302F0), &recv_work)));
    }
    _cnet_Return_CallBack(0x22);
}

void _cnet_RecvFromLbs_AnswerAnnexEntry(void) {
    _cnet_Return_CallBack();
}

void _cnet_RecvFromLbs_AnswerAnnexExit(void) {
    _cnet_Return_CallBack();
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
    memset(CNWP(0x3CEE), 0, 0x300);
    __cnet_Recv_MemberSub(CNWP(0x302EE), CNWP(0x3CEE));
}

void _cnet_RecvFromLbs_NoticeAnnexLeaver(void) {
    _sub_InOutRoomMember();
}

void _cnet_RecvFromLbs_NoticeAnnexCommer(void) {
    _sub_InOutRoomMember();
}

s32 cnLBS_Read_LobbyMemberList(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        memset(CNWP(0x36EE), 0, 0x300);
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_LobbyMemberList(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_LobbyMemberList(s32 arg0, int arg1, int arg2, int arg3) {
    void *temp_s2;

    temp_s2 = &CnetSys_w + ((arg0 & 0xFFFF) * 0x60);
    strcpy(arg1, temp_s2 + 0x36EE);
    strcpy(arg2, temp_s2 + 0x36F6);
    memcpy(arg3, temp_s2 + 0x370A, 0x40);
    return 0;
}

s32 __cnet_SendReq_LobbyMemberList(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xFE) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerLobbyMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_LobbyMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_LobbyMember(void) {
    memset(CNWP(0x36EE), 0, 0x300);
    __cnet_Recv_MemberSub(CNWP(0x302EC), CNWP(0x36EE));
}

void __cnet_Recv_MemberSub(u8 *arg0, s32 arg1) {
    u8 sp4F;
    u8 sp4E;
    u16 sp4C;
    int *var_a2;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;

    var_s2 = arg1;
    var_a2 = GetRecvData8(&sp4E, GetRecvData8(&sp4F, GetRecvData16(&sp4C, &recv_work)));
    if ((s32) sp4E >= 9) {
        sp4E = 8;
    }
    *arg0 = sp4E;
    var_s1 = 0;
    if ((s32) sp4E > 0) {
        do {
            var_a2 = GetRecvDataOption3(var_s2 + 0x1C, 0x40, GetRecvDataOption3(var_s2 + 8, 0x10, GetRecvDataOption3(var_s2, 8, var_a2)));
            var_s2 += 0x60;
            if ((s32) sp4F >= 4) {
                var_s0 = 0;
                if ((sp4F - 3) > 0) {
                    do {
                        var_s0 += 1;
                        var_a2 = GetRecvData16(&sp4C, var_a2, var_a2) + sp4C;
                    } while (var_s0 < (sp4F - 3));
                }
            }
            var_s1 += 1;
        } while (var_s1 < (s32) sp4E);
    }
}

void _cnet_RecvFromLbs_NoticeLobbyLeaver(void) {
    _sub_InOutRoomMember();
}

void _cnet_RecvFromLbs_NoticeLobbyCommer(void) {
    _sub_InOutRoomMember();
}

s32 cnLBS_Read_MatchEntryJoinUser(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_MatchEntryUser(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_MatchEntryJoinUser(s32 arg0, u16 *arg1, u16 *arg2) {
    s32 temp_a0;

    if (arg0 != 0) {
        temp_a0 = (arg0 & 0xFFFF) * 0x164;
        *arg1 = *(CNWP(0x6078) + temp_a0);
        *arg2 = *(CNWP(0x607A) + temp_a0);
    }
    return 0;
}

void _cnet_RecvFromLbs_AnswerMatchEntryUser(void) {
    u16 sp1E;
    s32 *temp_v1;

    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(&CnetSys_w + ((sp1E - 1) * 0x164) + 0x61DE, GetRecvData16(&CnetSys_w + ((sp1E - 1) * 0x164) + 0x61DC, GetRecvData16(&sp1E, &recv_work), sp1E));
        temp_v1 = CNWP(0x6060) + (sp1E * 0x164);
        *temp_v1 |= 0x40;
        CNW(u16, 0x6CEC) = sp1E;
    }
    _cnet_Return_CallBack(0x21);
}

void _cnet_RecvFromLbs_NoticeMatchEntryUser(void) {
    u8 temp_s1;
    void *temp_s0;

    if (CNW(s8, 0xFEC) == 0) {
        temp_s1 = CNW(u8, 0x4060);
        temp_s0 = &CnetSys_w + ((temp_s1 - 1) * 0x164);
        GetRecvData16(temp_s0 + 0x61DE, GetRecvData16(temp_s0 + 0x61DC, &recv_work));
        CNW(s16, 0x6CEC) = (s16) temp_s1;
    }
    _cnet_Return_CallBack(0x21);
}

s32 __cnet_SendReq_MatchEntryUser(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x9D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerRoomMatchEntryTypeList(void) {
    u8 sp5F;
    u8 sp5E;
    int sp5C;
    int sp50;
    int *var_s2;
    s32 var_s0;
    s32 var_s1;
    s32 var_s3;

    if (CNW(s8, 0xFEC) == 0) {
        var_s0 = GetRecvData8(&sp5F, GetRecvData16(&sp5C, &recv_work));
        if (sp5F != 0) {
            var_s1 = 0;
            if ((sp5F & 0xFF) > 0) {
                do {
                    memset(&sp50, 0, 8);
                    var_s0 = GetRecvData8(&sp5E, GetRecvDataOption3(&sp50, 8, var_s0));
                    var_s3 = 0;
                    var_s2 = &CnetSys_w;
loop_4:
                    if (memcmp(&sp50, var_s2 + 0x39EE, 8) == 0) {
                        *(u8 *)((u8 *)var_s2 + 0x3A4A) = sp5E;
                    }
                    var_s3 += 1;
                    var_s2 += 0x60;
                    if (var_s3 < 8) {
                        goto loop_4;
                    }
                    var_s1 += 1;
                } while (var_s1 < (s32) sp5F);
            }
        }
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeRoomMatchEntryTypeList(void) {
    u16 sp2E;
    s32 temp_a2;
    void *temp_s0;

    if (CNW(s8, 0xFEC) == 0) {
        memset(CNWP(0x3FEE), 0, 0x60);
        temp_a2 = GetRecvData16(&sp2E, &recv_work);
        if (sp2E == CNW(u8, 0x4060)) {
            temp_s0 = CNWP(0x39EE);
            CNW(s8, 0x302ED) = 1;
            GetRecvData8(temp_s0 + 0x5C, GetRecvDataOption3(temp_s0, 8, temp_a2));
            goto block_3;
        }
    } else {
block_3:
        _cnet_Return_CallBack(0x28);
    }
}

s32 __cnet_SendReq_RoomSetExplain(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x71) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerRoomSetExplain(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_Read_RoomExplainPermission(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_RoomExplainPermission(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerRoomExplainPermission(void) {
    u8 sp1F;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Byte(&sp1F);
        CNW(u8, 0x6E4A) = sp1F;
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_RoomExplainPermission(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x6F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Read_TimingValue(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_TimingValue();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_TimingValue(s32 *arg0) {
    *arg0 = CNW(s32, 0x3BA54);
    return 0;
}

void _cnet_RecvFromLbs_BothTimingValue(void) {
    s32 sp1C;

    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Long(&sp1C);
        CNW(s32, 0x3BA54) = sp1C;
    }
    _cnet_Return_CallBack(0x2B);
}

s32 __cnet_SendReq_TimingValue(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xD3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Read_CurrentPlace(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_CurrentPlace();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 cnLBS_Get_CurrentPlace(void *arg0) {
    void *temp_v1;

    temp_v1 = CNWP(0x3BA58);
    *(s16 *)((u8 *)arg0 + 0) = (s16) CNW(s16, 0x3BA58);
    *(s16 *)((u8 *)arg0 + 2) = (s16) *(s16 *)((u8 *)temp_v1 + 2);
    *(s16 *)((u8 *)arg0 + 4) = (s16) *(s16 *)((u8 *)temp_v1 + 4);
    return 0;
}

void _cnet_RecvFromLbs_AnswerCurrentPlace(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x3BA5C), GetRecvData16(CNWP(0x3BA5A), GetRecvData16(CNWP(0x3BA58), &recv_work)));
    }
    _cnet_Return_CallBack(0);
}

s32 __cnet_SendReq_CurrentPlace(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xD6) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void cnLBS_Init_LoginLobbyServer(void) {
    *(s32 *)((u8 *)CNWP(8)) = 0;
    *(s32 *)((u8 *)CNWP(0)) = 0;
    memset(CNWP(0x18), 0, 0xE00);
    memset(CNWP(0xE18), 0, 0x1B0);
    memset(CNWP(0x4058), 0, 0xA);
    memset(CNWP(0x1436), 0, 0x28);
}

s32 cnLBS_LoginLobbyServer(void *arg0, s32 arg1) {
    int sp20;
    int *var_a2;
    int *var_a2_2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_a0_2;
    s32 var_a1;
    void *var_a0;
    void *var_a1_2;

    var_a0 = arg0;
    var_a2 = &sp20;
    var_a1 = 8;
    do {
        var_a1 -= 1;
        temp_v0 = *(s32 *)((u8 *)var_a0 + 4);
        *(s32 *)((u8 *)var_a2 + 0) = (s32) *(s32 *)((u8 *)var_a0 + 0);
        var_a0 += 8;
        *(s32 *)((u8 *)var_a2 + 4) = temp_v0;
        var_a2 += 8;
    } while (var_a1 > 0);
    if (CNW(u8, 0xE38) == 0) {
        var_a2_2 = &sp20;
        var_a1_2 = CNWP(0x1034);
        var_a0_2 = 8;
        do {
            var_a0_2 -= 1;
            temp_v0_2 = *(s32 *)((u8 *)var_a2_2 + 4);
            *(s32 *)((u8 *)var_a1_2 + 0) = (s32) *(s32 *)((u8 *)var_a2_2 + 0);
            var_a2_2 += 8;
            *(s32 *)((u8 *)var_a1_2 + 4) = temp_v0_2;
            var_a1_2 += 8;
        } while (var_a0_2 > 0);
        CNW(s8, 0x14) = 0;
        *(s32 *)((u8 *)CNWP(0)) = 1;
        CNW(s8, 0x1030) = 0;
        CNW(s16, 0x102E) = 0;
        memset(CNWP(0x1074), 0, 0x5C);
        memset(CNWP(0x1462), 0, 0x170);
        CNW(s32, 0xFF0) = 0;
        CNW(s32, 0xFF4) = 0x1000;
        CNW(void *, 0xFF8) = (void *) (CNWP(0x37BC0));
        memset(CNWP(0x268E), 0, 0x1004);
        memset(CNWP(0x37BC0), 0, 0x2000);
        CNW(s32, 0xE1C) = arg1;
        CNW(u8, 0xE38) = 1U;
        CNW(s8, 0xE39) = 0;
        CNW(s32, 0xE18) = 0;
        return 0;
    }
    return -1;
}

s32 cnLBS_Set_LoginFirstData(void *arg0) {
    s16 temp_v0;
    s32 var_a1;
    void *var_a0;
    void *var_a2;

    var_a0 = arg0;
    var_a1 = 9;
    var_a2 = CNWP(0x10D0);
    do {
        var_a1 -= 1;
        temp_v0 = *(s16 *)((u8 *)var_a0 + 2);
        *(s16 *)((u8 *)var_a2 + 0) = (s16) *(s16 *)((u8 *)var_a0 + 0);
        var_a0 += 4;
        *(s16 *)((u8 *)var_a2 + 2) = temp_v0;
        var_a2 += 4;
    } while (var_a1 > 0);
    return 0;
}

s32 cnLBS_Send_LoginUserAccount(int *arg0, int *arg1, int arg2) {
    s32 temp_v0;

    memset(CNWP(0x1074), 0, 0x5C);
    strncpy(CNWP(0x107C), arg1, strlen(arg1));
    memcpy(CNWP(0x1090), arg2, 0x40);
    if (arg0 == NULL) {
        strncpy(CNWP(0x1576), &lit_108_0065E000, 6);
        memset(CNWP(0x1074), 0, 8);
    } else {
        strncpy(CNWP(0x1576), arg0, 6);
        strncpy(CNWP(0x1074), arg0, 6);
    }
    temp_v0 = strlen(arg1);
    strncpy(CNWP(0x157E), arg1, temp_v0);
    *(CNWP(0x157E) + temp_v0) = 0;
    __cnet_SendReq_UserID();
    return 0;
}

u8 cnetGet_Login_NoOfUserAccount(void) {
    return CNW(u8, 0x145E);
}

s32 cnetGet_Login_UserID(s32 arg0, int arg1) {
    s32 temp_a2;

    temp_a2 = arg0 & 0xFF;
    strcpy(arg1, &CnetSys_w + (temp_a2 * 0x5C) + 0x1462, temp_a2);
    return 0;
}

s32 cnetGet_Login_UserHandle(s32 arg0, int arg1) {
    s32 temp_a2;

    temp_a2 = arg0 & 0xFF;
    strcpy(arg1, &CnetSys_w + (temp_a2 * 0x5C) + 0x146A, temp_a2);
    return 0;
}

s32 cnetGet_Login_UserMiniData(s32 arg0, int arg1) {
    s32 temp_a3;

    temp_a3 = arg0 & 0xFF;
    memcpy(arg1, &CnetSys_w + (temp_a3 * 0x5C) + 0x147E, 0x40, temp_a3);
    return 0;
}

s32 cnetGet_Login_DecideUserID(void) {
    strcpy(CNWP(0x1576));
    return 0;
}

s32 cnetGet_Login_DecideUserHandle(void) {
    strcpy(CNWP(0x157E));
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

void _cnet_RecvFromLbs_AnswerEchoPacket(void) {
    s32 temp_a0;

    temp_a0 = (CNW(u16, 0x102C) * 0x10) & 0xFFFF;
    CNW(u16, 0x102E) = (u16) (CNW(u16, 0x102E) + temp_a0);
    CNW(u8, 0x1030) = (u8) (CNW(u8, 0x1030) + 1);
    if ((s32) CNW(u8, 0x1030) < 4) {
        __cnet_SendReq_EchoPacket(temp_a0);
        return;
    }
    CNW(s16, 0x10EC) = (s16) ((u16) CNW(u16, 0x102E) >> 2);
    __cnet_SendSet_FirstData(temp_a0);
}

void _cnet_RecvFromLbs_NoticeUserId(void) {
    s8 sp19;
    s8 sp18;

    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_UserIDandHandle();
        sp18 = 0;
        sp19 = 1;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
    }
}

void _cnet_RecvFromLbs_AnswerUserId(void) {
    s8 sp19;
    s8 sp18;

    if (CNW(u8, 0xE38) != 0) {
        if (CNW(s8, 0xFEC) == 0) {
            __cnet_Recv_UserID();
            cnetGet_Login_DecideUserID(CNWP(0x15D2));
            cnetGet_Login_DecideUserHandle(CNWP(0x15DA));
            sp18 = 0;
            sp19 = 2;
            M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
            return;
        }
        __cnet_Recv_ServerMessage();
        sp18 = -1;
        sp19 = 7;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
    }
}

s32 cnLBS_Send_UserMiniData(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendSet_MiniDataRegist(arg0, arg1);
        var_v0 = temp_v0;
    }
    return var_v0;
}

void _cnet_RecvFromLbs_AnswerMiniDataRegist(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_Get_NoticeUserMiniData(void *arg0) {
    s32 var_a1;
    s8 temp_v0;
    void *var_a0;
    void *var_a2;

    var_a0 = arg0;
    var_a1 = 0x2E;
    var_a2 = CNWP(0x162E);
    do {
        var_a1 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a2 + 1);
        *(s8 *)((u8 *)var_a0 + 0) = (s8) *(s8 *)((u8 *)var_a2 + 0);
        var_a2 += 2;
        *(s8 *)((u8 *)var_a0 + 1) = temp_v0;
        var_a0 += 2;
    } while (var_a1 > 0);
    return 0;
}

void _cnet_RecvFromLbs_NoticeMiniData(void) {
    if (CNW(s8, 0xFEC) == 0) {
        memset(CNWP(0x162E), 0, 0x5C);
        GetRecvDataOption3(CNWP(0x164A), 0x40, GetRecvDataOption3(CNWP(0x162E), 8, &recv_work));
    }
    _cnet_Return_CallBack(0x2C);
}

void _cnet_RecvFromLbs_NoticeLoginOk(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Login_Return();
    }
}

void _cnet_RecvFromLbs_RequestWarningMessage(void) {
    s8 sp29;
    s8 sp28;
    s32 temp_s0;
    s32 var_a1;
    u32 temp_v1;
    void *temp_s0_2;

    if (CNW(u8, 0xE38) != 0) {
        var_a1 = 0x600;
        if ((u32) CNW(u32, 0xFE0) >= 2U) {

        } else {
            var_a1 = (((((*(u8 *)((u8 *)&recv_header + 4) << 8) & 0xFFFF) + *(u8 *)((u8 *)&recv_header + 5)) & 0xFFFF) - CNW(u16, 0xFF0)) & 0xFFFF;
        }
        temp_v1 = CNW(s32, 0xFF4) - (s32) CNW(u16, 0xFF0);
        if ((u32) (var_a1 & 0xFFFF) >= temp_v1) {
            var_a1 = (temp_v1 - 1) & 0xFFFF;
        }
        temp_s0 = var_a1 & 0xFFFF;
        memcpy(CNW(s32, 0xFF8) + (s32) CNW(u16, 0xFF0), &recv_work, temp_s0);
        CNW(u16, 0xFF0) = (s32) ((s32) CNW(u16, 0xFF0) + temp_s0);
        if (CNW(u32, 0xFE0) == 1) {
            temp_s0_2 = CNWP(0x268E);
            GetRecvDataString(temp_s0_2 + 4, GetRecvDataString(temp_s0_2 + 4, GetRecvData16(temp_s0_2 + 2, GetRecvData8(temp_s0_2, CNWP(0x37BC0)))));
            sp28 = 0;
            sp29 = 4;
            M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp28, &sp28);
        }
    }
}

void cnLBS_Get_LoginWarningMessage(void *arg0) {
    s16 temp_v1;
    s32 var_a2;
    void *var_a0;
    void *var_a3;

    var_a0 = arg0;
    var_a2 = 0x401;
    var_a3 = CNWP(0x268E);
    do {
        var_a2 -= 1;
        temp_v1 = *(s16 *)((u8 *)var_a3 + 2);
        *(s16 *)((u8 *)var_a0 + 0) = (s16) *(s16 *)((u8 *)var_a3 + 0);
        var_a3 += 4;
        *(s16 *)((u8 *)var_a0 + 2) = temp_v1;
        var_a0 += 4;
    } while (var_a2 > 0);
}

s32 cnLBS_Answer_LoginWarningMessage(void) {
    __cnet_SendAns_WarningMessage();
    return 0;
}

void __cnet_SendAns_WarningMessage(s32 arg0) {
    SetSendCommand(&send_work, 0x14);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

s32 cnLBS_Send_LoginFinish(void) {
    __cnet_SendSet_LoginFinish();
    return 0;
}

void _cnet_RecvFromLbs_AnswerBillEstimate(void) {

}

void _cnet_RecvFromLbs_AnswerUserBinary(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_Read_TopInformation(s32 arg0) {
    s32 temp_s0;
    s32 var_v0;

    temp_s0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    CNW(s32, 0xFF0) = 0;
    CNW(s32, 0xFF4) = 0x1000;
    CNW(void *, 0xFF8) = (void *) (CNWP(0x37BC0));
    memset(CNWP(0x168A), 0, 0x1004);
    memset(CNWP(0x37BC0), 0, 0x2000);
    CNW(s32, 0xFF0) = 0;
    CNW(s32, 0xFF4) = 0x1000;
    CNW(void *, 0xFF8) = (void *) (CNWP(0x37BC0));
    memset(CNWP(0x168A), 0, 0x1004);
    memset(CNWP(0x37BC0), 0, 0x2000);
    var_v0 = -1;
    if (temp_s0 != -1) {
        CnetSys_w.bg[temp_s0].cmd = __cnet_SendReq_TopInformation();
        var_v0 = temp_s0;
    }
    return var_v0;
}

s32 cnLBS_Get_TopInformation(void *arg0) {
    s32 var_a1;
    s8 temp_v0;
    void *var_a0;
    void *var_a2;

    var_a0 = arg0;
    var_a1 = 0x802;
    var_a2 = CNWP(0x168A);
    do {
        var_a1 -= 1;
        temp_v0 = *(s8 *)((u8 *)var_a2 + 1);
        *(s8 *)((u8 *)var_a0 + 0) = (s8) *(s8 *)((u8 *)var_a2 + 0);
        var_a2 += 2;
        *(s8 *)((u8 *)var_a0 + 1) = temp_v0;
        var_a0 += 2;
    } while (var_a1 > 0);
    return 0;
}

s32 __cnet_SendReq_TopInformation(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x1F) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerTopInformation(void) {
    s32 temp_s0;
    s32 var_a1;
    u32 temp_v1;
    void *temp_s0_2;

    if (CNW(s8, 0xFEC) == 0) {
        var_a1 = 0x600;
        if ((u32) CNW(u32, 0xFE0) >= 2U) {

        } else {
            var_a1 = (((((*(u8 *)((u8 *)&recv_header + 4) << 8) & 0xFFFF) + *(u8 *)((u8 *)&recv_header + 5)) & 0xFFFF) - CNW(u16, 0xFF0)) & 0xFFFF;
        }
        temp_v1 = CNW(s32, 0xFF4) - (s32) CNW(u16, 0xFF0);
        if ((u32) (var_a1 & 0xFFFF) >= temp_v1) {
            var_a1 = (temp_v1 - 1) & 0xFFFF;
        }
        temp_s0 = var_a1 & 0xFFFF;
        memcpy(CNW(s32, 0xFF8) + (s32) CNW(u16, 0xFF0), &recv_work, temp_s0);
        CNW(u16, 0xFF0) = (s32) ((s32) CNW(u16, 0xFF0) + temp_s0);
        if (CNW(u32, 0xFE0) == 1) {
            temp_s0_2 = CNWP(0x168A);
            GetRecvDataString(temp_s0_2 + 4, GetRecvData8(temp_s0_2, CNWP(0x37BC0)));
            goto block_9;
        }
    } else {
block_9:
        _cnet_Return_CallBack(0);
    }
}

void __cnet_Login_Return(void) {
    s8 sp19;
    s8 sp18;

    if (CNW(u8, 0xE38) != 0) {
        sp18 = 0;
        CNW(u8, 0xE38) = 0U;
        sp19 = 0;
        CNW(s8, 0xE39) = 0;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
    }
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

void __cnet_SendAns_BattleResult(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x1436);
    SetSendCommand(&send_work, 0x19);
    if (*(u8 *)((u8 *)temp_s0 + 0x10) != 0) {
        SetSendData16(&send_work, 0x5678U);
        SetSendStringData2(&send_work, temp_s0, 0xF);
        SetSendData8(&send_work, 0U);
        SetSendData8(&send_work, 0U);
        SetSendData8(&send_work, *(u8 *)((u8 *)temp_s0 + 0x10));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x14));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x16));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x18));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1A));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1C));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1E));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x20));
        SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x22));
    } else {
        SetSendResult(&send_work, 0xFF);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_AnswerPersonalRecordHeader(void) {
    u8 sp3F;
    int *var_a1;
    int *var_s0;
    s32 var_s1;

    if (CNW(u8, 0xF58) != 0) {
        if ((CNW(u16, 0xFE6) == 2) && (CNW(s8, 0xFEC) == 0)) {
            var_a1 = GetRecvData8(&sp3F, &recv_work);
            if ((s32) sp3F >= 3) {
                CNW(u8, 0x30CAC) = sp3F;
                var_s1 = 0;
                if ((s32) sp3F > 0) {
                    var_s0 = &CnetSys_w;
                    do {
                        var_s1 += 1;
                        var_a1 = GetRecvData8(var_s0 + 0x30CAD, var_a1);
                        var_s0 += 1;
                    } while (var_s1 < (s32) sp3F);
                }
            } else {
                CNW(u8, 0x30CAC) = 2U;
            }
            CNW(s8, 0x30CAD) = 1;
            CNW(s8, 0x30CAE) = 0x11;
        }
        CNW(s8, 0xF5A) = 1;
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordData(void) {
    u8 sp6F;
    u8 sp6E;
    int *temp_a1;
    int *var_a1;
    s32 var_s1;
    s32 var_s4;
    void *temp_s0;
    void *temp_s1;
    void *temp_s2;
    void *temp_v0;
    void *var_s2;
    void *var_s3;
    void *var_s3_2;
    void *var_s4_2;

    if (CNW(u8, 0xF58) != 0) {
        if ((CNW(u16, 0xFE6) == 2) && (CNW(s8, 0xFEC) == 0)) {
            temp_a1 = GetRecvData8(&sp6E, GetRecvData8(&sp6F, &recv_work));
            temp_v0 = (sp6E * 0x118) + (&CnetSys_w + (sp6F * 0x3480));
            temp_s0 = temp_v0 + 0x30CB8;
            *(u8 *)((u8 *)temp_v0 + 0x30CB8) = sp6F;
            *(u8 *)((u8 *)temp_s0 + 1) = sp6E;
            var_a1 = GetRecvData32(temp_s0 + 0x28, GetRecvData32(temp_s0 + 0x24, GetRecvData32(temp_s0 + 0x20, GetRecvData32(temp_s0 + 0x1C, GetRecvData32(temp_s0 + 0x18, GetRecvData32(temp_s0 + 0x14, GetRecvData8(temp_s0 + 0xC, GetRecvData32(temp_s0 + 8, GetRecvData32(temp_s0 + 4, GetRecvData32(temp_s0 + 0x10, temp_a1, sp6F * 0x69, sp6F))))))))));
            if (sp6F == 1) {
                var_s4 = 0;
                if ((s32) CNW(u16, 0xFEA) >= 0x28) {
                    var_s3 = temp_s0;
                    var_s2 = temp_s0;
                    do {
                        temp_s1 = temp_s0 + var_s4;
                        var_a1 = GetRecvDataOption3(var_s2 + 0x91, 0x10, GetRecvDataOption3(var_s3 + 0x79, 8, GetRecvData8(temp_s1 + 0x76, GetRecvData8(temp_s1 + 0x73, var_a1))));
                        var_s4 += 1;
                        var_s3 += 8;
                        var_s2 += 0x11;
                    } while (var_s4 < 3);
                    var_s1 = 0;
                    var_s4_2 = temp_s0;
                    var_s3_2 = temp_s0;
                    do {
                        temp_s2 = temp_s0 + var_s1;
                        var_s1 += 1;
                        var_a1 = GetRecvDataOption3(var_s3_2 + 0xE2, 0x10, GetRecvDataOption3(var_s4_2 + 0xCA, 8, GetRecvData8(temp_s2 + 0xC7, GetRecvData8(temp_s2 + 0xC4, var_a1))));
                        var_s4_2 += 8;
                        var_s3_2 += 0x11;
                    } while (var_s1 < 3);
                }
            }
        }
        CNW(u8, 0xF5A) = (u8) (CNW(u8, 0xF5A) + 1);
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordVide(void) {
    u8 sp2F;
    u8 sp2E;
    void *temp_s0;

    if (CNW(u8, 0xF58) != 0) {
        if ((CNW(u16, 0xFE6) == 2) && (CNW(s8, 0xFEC) == 0)) {
            temp_s0 = (sp2E * 0x118) + (&CnetSys_w + (sp2F * 0x3480)) + 0x30CB8;
            GetRecvData8(temp_s0 + 0x72, GetRecvData8(temp_s0 + 0x71, GetRecvData8(temp_s0 + 0x70, GetRecvData8(temp_s0 + 0x6F, GetRecvData8(temp_s0 + 0x6E, GetRecvData8(temp_s0 + 0x6D, GetRecvDataString(temp_s0 + 0x2C, GetRecvData8(&sp2E, GetRecvData8(&sp2F, &recv_work)), sp2F * 0x69)))))));
        }
        CNW(u8, 0xF5A) = (u8) (CNW(u8, 0xF5A) + 1);
    }
}

void __cnet_SendSet_ConnectionPair(void) {
    int sp10;

    SetSendCommand(&send_work, 0xD);
    mmbbc_encode(&sp10, CNWP(0x1036), (((*(u8 *)((u8 *)&send_work + 0xA) << 8) & 0xFFFF) + *(u8 *)((u8 *)&send_work + 0xB)) & 0xFFFF);
    SetSendData16(&send_work, 0xA);
    SetSendStringData(&send_work, &sp10, 0xA);
    SetSendEncodeStringData(&send_work, CNWP(0x1041), strlen(CNWP(0x1041)) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_FirstData(void) {
    void *temp_s0;

    temp_s0 = CNWP(0x10D0);
    SetSendCommand(&send_work, 0x11);
    SetSendData8(&send_work, CNW(u8, 0x10D0));
    SetSendData8(&send_work, *(u8 *)((u8 *)temp_s0 + 1));
    SetSendData8(&send_work, *(u8 *)((u8 *)temp_s0 + 2));
    SetSendStringData2(&send_work, temp_s0 + 4, 0xA);
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x14));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x16));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x18));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1A));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1C));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x1E));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x20));
    SetSendData16(&send_work, *(u16 *)((u8 *)temp_s0 + 0x22));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_UserID(void) {
    SetSendCommand(&send_work, 0x16);
    SetSendStringData2(&send_work, CNWP(0x1576), 6);
    SetSendStringData2(&send_work, CNWP(0x157E), strlen(CNWP(0x157E)) & 0xFFFF);
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
    SetSendStringData2(&send_work, CNWP(0x1060), strlen(CNWP(0x1060)) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

s32 __cnet_SendSet_MiniDataRegist(s32 arg0, int arg1) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x21) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void __cnet_SendReq_EchoPacket(void) {
    CNW(s16, 0x102C) = 0;
    SetSendCommand(&send_work, 0xA);
    SetSendStringData2(&send_work, &lit_336_0065E008, 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

s32 __cnet_Recv_UserIDandHandle(void) {
    u8 sp3F;
    int *var_s0;
    s32 var_s1;
    s32 var_v0;

    memset(CNWP(0x1462), 0, 0x170);
    var_v0 = GetRecvData8(&sp3F, &recv_work);
    if ((s32) sp3F >= 4) {
        sp3F = 3;
    }
    CNW(u8, 0x145E) = sp3F;
    var_s1 = 0;
    if ((s32) sp3F > 0) {
        var_s0 = &CnetSys_w;
        do {
            var_v0 = GetRecvDataOption3(var_s0 + 0x147E, 0x40, GetRecvDataOption3(var_s0 + 0x146A, 0x10, GetRecvDataOption3(var_s0 + 0x1462, 8, var_v0)));
            var_s1 += 1;
            var_s0 += 0x5C;
        } while (var_s1 < (s32) sp3F);
    }
    return 0;
}

s32 __cnet_Recv_UserID(void) {
    GetRecvDataOption3(CNWP(0x1576), 8, &recv_work);
    return 0;
}

void cnLBS_Send_ChatMessage(s32 arg0, int arg1) {
    __cnet_SendSet_ChatMessage(0, arg1);
}

void cnLBS_Get_ChatMessage(void *arg0) {
    s32 var_a2;
    s8 temp_v1;
    void *var_a0;
    void *var_a3;

    var_a0 = arg0;
    var_a2 = 0x90;
    var_a3 = CNWP(0x30B8A);
    do {
        var_a2 -= 1;
        temp_v1 = *(s8 *)((u8 *)var_a3 + 1);
        *(s8 *)((u8 *)var_a0 + 0) = (s8) *(s8 *)((u8 *)var_a3 + 0);
        var_a3 += 2;
        *(s8 *)((u8 *)var_a0 + 1) = temp_v1;
        var_a0 += 2;
    } while (var_a2 > 0);
}

void __cnet_SendSet_ChatMessage(s32 arg0, int arg1, int arg2) {
    SetSendCommand(&send_work, 0xE8);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatMessage(void) {
    memset(CNWP(0x30B8A), 0, 0x120);
    GetRecvData8(CNWP(0x30CA9), GetRecvData8(CNWP(0x30CA8), GetRecvData8(CNWP(0x30CA7), GetRecvData8(CNWP(0x30CA6), GetRecvDataOption3(CNWP(0x30BA6), 0x100, GetRecvDataOption3(CNWP(0x30B92), 0x10, GetRecvDataOption3(CNWP(0x30B8A), 8, &recv_work)))))));
    _cnetEvent_JumpCallBack(5, 0);
}

void cnLBS_Send_ChatBinary(void) {
    __cnet_SendSet_ChatBinary();
}

void cnLBS_Get_ChatBinary(void *arg0) {
    s32 var_a2;
    s8 temp_v1;
    void *var_a0;
    void *var_a3;

    var_a0 = arg0;
    var_a2 = 0x184;
    var_a3 = CNWP(0x375B8);
    do {
        var_a2 -= 1;
        temp_v1 = *(s8 *)((u8 *)var_a3 + 1);
        *(s8 *)((u8 *)var_a0 + 0) = (s8) *(s8 *)((u8 *)var_a3 + 0);
        var_a3 += 2;
        *(s8 *)((u8 *)var_a0 + 1) = temp_v1;
        var_a0 += 2;
    } while (var_a2 > 0);
}

void __cnet_SendSet_ChatBinary(s32 arg0, int arg1) {
    SetSendCommand(&send_work, 0xF6);
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatBinary(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

s32 cnLBS_Send_ChatMessageTU(s32 arg0, int arg1, int arg2, int arg3) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg3);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendSet_ChatMessageTU(arg0, arg1, arg2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendSet_ChatMessageTU(s32 arg0, s32 arg1, int arg2) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xF8) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, 0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerChatMessageTU(void) {
    _cnet_Return_CallBack();
}

void _cnet_RecvFromLbs_NoticeChatMessageTU(void) {
    memset(CNWP(0x30B8A), 0, 0x120);
    GetRecvData8(CNWP(0x30CA9), GetRecvData8(CNWP(0x30CA8), GetRecvData8(CNWP(0x30CA7), GetRecvData8(CNWP(0x30CA6), GetRecvDataOption3(CNWP(0x30BA6), 0x100, GetRecvDataOption3(CNWP(0x30B92), 0x10, GetRecvDataOption3(CNWP(0x30B8A), 8, &recv_work)))))));
    _cnetEvent_JumpCallBack(0x2A, 0);
}

s32 cnLBS_Send_ChatBinaryTU(s32 arg0, int arg1, int arg2, int arg3) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg3);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendSet_ChatBinaryTU(arg0, arg1, arg2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendSet_ChatBinaryTU(s32 arg0, s32 arg1, int arg2) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xFB) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerChatBinaryTU(void) {
    _cnet_Return_CallBack();
}

void _cnet_RecvFromLbs_NoticeChatBinaryTU(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

void _cnet_RecvFromLbs_MatchStart(void) {
    _cnetEvent_JumpCallBack(1);
}

s32 cnLBS_MatchEntry(s32 arg0, int arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendReq_MatchEntry(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendReq_MatchEntry(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0x9B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_MatchEntry(void) {
    _cnet_Return_CallBack();
}

s32 cnLBS_MatchStart(void) {
    SetSendCommand(&send_work, 0xA1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return 0;
}

s32 cnLBS_Read_MatchInfomation(s32 arg0) {
    if (CNW(u8, 0xF34) == 0) {
        memset(CNWP(0x30310), 0, 0x5D4);
        __cnet_SendReq_MatchJoin();
        CNW(s32, 0xF18) = arg0;
        CNW(u8, 0xF34) = 1U;
        CNW(s8, 0xF35) = 0;
        CNW(s32, 0xF14) = 0;
        return 0;
    }
    return -1;
}

void _cnet_RecvFromLbs_MatchJoin(void) {
    u8 sp1F;
    s8 sp18;

    if (CNW(u8, 0xF34) != 0) {
        if (CNW(u16, 0xFE6) == 2) {
            if (CNW(s8, 0xFEC) == 0) {
                __cnet_Recv_Byte(&sp1F);
                CNW(u8, 0x30310) = sp1F;
                goto block_6;
            }
            sp18 = -1;
            __cnet_Recv_ServerMessage(&sp1F);
            __cnet_Return_MatchInformation((s64) sp18);
            return;
        }
block_6:
        __cnet_SendReq_MatchPlSide(0);
    }
}

void _cnet_RecvFromLbs_MatchPlSide(void) {
    u8 sp1F;
    s8 sp18;

    if (CNW(u8, 0xF34) != 0) {
        if (CNW(u16, 0xFE6) == 2) {
            if (CNW(s8, 0xFEC) == 0) {
                __cnet_Recv_Byte(&sp1F);
                if (sp1F != 0) {
                    sp1F -= 1;
                }
                CNW(u8, 0x30311) = sp1F;
                goto block_8;
            }
            sp18 = -1;
            __cnet_Recv_ServerMessage(&sp1F);
            __cnet_Return_MatchInformation((s64) sp18);
            return;
        }
block_8:
        M2C_FIELD(saved_reg_gp, s8 *, -0x4350) = 1;
        __cnet_SendReq_MatchOpponentInfo(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentInfo(void) {
    u8 sp2F;
    s8 sp28;
    u8 temp_a0;
    void *temp_s0;

    if ((CNW(u8, 0xF34) != 0) && (CNW(u16, 0xFE6) != 0x10)) {
        if (CNW(u16, 0xFE6) == 2) {
            if (CNW(s8, 0xFEC) == 0) {
                temp_s0 = CNWP(0x30310);
                GetRecvData8(temp_s0 + ((sp2F - 1) * 0x98) + 0x1A9, GetRecvDataString(temp_s0 + ((sp2F - 1) * 0x98) + 0x170, GetRecvDataString(temp_s0 + ((sp2F - 1) * 0x98) + 0x130, GetRecvDataString(temp_s0 + ((sp2F - 1) * 0x98) + 0x11C, GetRecvDataString(temp_s0 + ((sp2F - 1) * 0x98) + 0x114, GetRecvData8(temp_s0 + ((sp2F - 1) * 0x98) + 0x1AA, GetRecvData8(&sp2F, &recv_work)))))));
                M2C_FIELD(((sp2F * 0x98) + temp_s0), u8 *, 0x110) = sp2F;
                goto block_8;
            }
            sp28 = -1;
            __cnet_Recv_ServerMessage(CNW(u16, 0xFE6), &recv_work);
            __cnet_Return_MatchInformation((s64) sp28);
            return;
        }
block_8:
        M2C_FIELD(saved_reg_gp, u8 *, -0x4350) = (u8) (M2C_FIELD(saved_reg_gp, u8 *, -0x4350) + 1);
        temp_a0 = M2C_FIELD(saved_reg_gp, u8 *, -0x4350);
        if ((s32) CNW(u8, 0x30310) >= (s32) temp_a0) {
            __cnet_SendReq_MatchOpponentInfo(temp_a0);
            return;
        }
        M2C_FIELD(saved_reg_gp, u8 *, -0x4350) = 1U;
        __cnet_SendReq_MatchOpponentStatus(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentStatus(void) {
    u8 sp2F;
    s8 sp28;
    u8 temp_a0;
    void *temp_s0;

    if ((CNW(u8, 0xF34) != 0) && (CNW(u16, 0xFE6) != 0x10)) {
        if (CNW(u16, 0xFE6) == 2) {
            if (CNW(s8, 0xFEC) == 0) {
                temp_s0 = CNWP(0x30310);
                GetRecvData32(temp_s0 + ((sp2F - 1) * 0x98) + 0x1A4, GetRecvData32(temp_s0 + ((sp2F - 1) * 0x98) + 0x1A0, GetRecvData32(temp_s0 + ((sp2F - 1) * 0x98) + 0x19C, GetRecvData32(temp_s0 + ((sp2F - 1) * 0x98) + 0x198, GetRecvData32(temp_s0 + ((sp2F - 1) * 0x98) + 0x194, GetRecvData16(temp_s0 + ((sp2F - 1) * 0x98) + 0x190, GetRecvData8(&sp2F, &recv_work)))))));
                M2C_FIELD(((sp2F * 0x98) + temp_s0), u8 *, 0x110) = sp2F;
                goto block_8;
            }
            sp28 = -1;
            __cnet_Recv_ServerMessage(CNW(u16, 0xFE6), &recv_work);
            __cnet_Return_MatchInformation((s64) sp28);
            return;
        }
block_8:
        M2C_FIELD(saved_reg_gp, u8 *, -0x4350) = (u8) (M2C_FIELD(saved_reg_gp, u8 *, -0x4350) + 1);
        temp_a0 = M2C_FIELD(saved_reg_gp, u8 *, -0x4350);
        if ((s32) CNW(u8, 0x30310) >= (s32) temp_a0) {
            __cnet_SendReq_MatchOpponentStatus(temp_a0);
            return;
        }
        __cnet_SendReq_MatchBattleCode(temp_a0);
    }
}

void _cnet_RecvFromLbs_MatchBattleCode(void) {
    s8 sp18;
    void *temp_a0;

    if ((CNW(u8, 0xF34) != 0) && (CNW(u16, 0xFE6) != 0x10)) {
        if (CNW(u16, 0xFE6) == 2) {
            temp_a0 = CNWP(0x30312);
            if (CNW(s8, 0xFEC) == 0) {
                GetRecvDataString(temp_a0, &recv_work);
                goto block_7;
            }
            sp18 = -1;
            __cnet_Recv_ServerMessage(temp_a0);
            __cnet_Return_MatchInformation((s64) sp18);
            return;
        }
block_7:
        __cnet_SendReq_MatchGameRule();
    }
}

void _cnet_RecvFromLbs_MatchGameRule(void) {
    s8 sp18;
    void *temp_a0;

    if ((CNW(u8, 0xF34) != 0) && (CNW(u16, 0xFE6) != 0x10)) {
        if (CNW(u16, 0xFE6) == 2) {
            temp_a0 = CNWP(0x30323);
            if (CNW(s8, 0xFEC) == 0) {
                GetRecvDataString(temp_a0, &recv_work);
                goto block_7;
            }
            sp18 = -1;
            __cnet_Recv_ServerMessage(temp_a0);
            __cnet_Return_MatchInformation((s64) sp18);
            return;
        }
block_7:
        __cnet_SendReq_MatchMcsIpAddr();
    }
}

void _cnet_RecvFromLbs_MatchGameServerAddr(void) {
    s8 sp18;
    void *temp_a0;

    if (CNW(u8, 0xF34) != 0) {
        if (CNW(u16, 0xFE6) == 2) {
            temp_a0 = CNWP(0x30300);
            if (CNW(s8, 0xFEC) == 0) {
                sp18 = 0;
                GetRecvDataString(CNWP(0x30308), GetRecvDataString(temp_a0, &recv_work));
                goto block_7;
            }
            sp18 = -1;
            __cnet_Recv_ServerMessage(temp_a0);
            __cnet_Return_MatchInformation((s64) sp18);
            return;
        }
block_7:
        sp18 = 0;
        __cnet_Return_MatchInformation((s64) sp18);
    }
}

void __cnet_Return_MatchInformation(s64 arg0) {
    s64 sp18;

    sp18 = arg0;
    if ((s8) sp18 == -1) {
        __cnet_SendReq_MatchRejection((s8) sp18);
    }
    if (M2C_FIELD(&CnetSys_w, int (**)(s64, s64 *), 0xF18) != NULL) {
        CNW(s8, 0xF34) = 0;
        CNW(s8, 0xF35) = 0;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s64 *), 0xF18)(sp18, &sp18);
    }
}

s32 __cnet_SendReq_MatchJoin(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xA3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void __cnet_SendReq_MatchPlSide(s32 arg0) {
    SetSendCommand(&send_work, 0xA5);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentInfo(s32 arg0) {
    SetSendCommand(&send_work, 0xA9);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentStatus(s32 arg0) {
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

s32 __cnet_SendReq_MatchRejection(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xAD) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Get_MatchInfomation(s32 *arg0) {
    s32 *var_a0;
    s32 *var_a1;
    s32 var_v1;

    var_a0 = arg0;
    var_v1 = 0x175;
    var_a1 = CNWP(0x30310);
    do {
        var_v1 -= 1;
        *var_a0 = *var_a1;
        var_a1 += 4;
        var_a0 += 4;
    } while (var_v1 > 0);
    return 0;
}

void cnLBS_Get_GameServerAddress(s32 *arg0, s16 *arg1) {
    s32 temp_v1;

    *arg0 = ((CNW(u8, 0x30303) << 0x18) & 0xFF000000) | (((CNW(u8, 0x30302) << 0x10) & 0xFF0000) | (CNW(u8, 0x30300) | ((CNW(u8, 0x30301) << 8) & 0xFF00)));
    temp_v1 = (CNW(u8, 0x30309) + (CNW(u8, 0x30308) << 8)) & 0xFFFF;
    *arg1 = ((temp_v1 << 8) & 0xFF00) | ((temp_v1 >> 8) & 0xFF);
}

void _cnet_RecvFromLbs_NoticePatchStart(void) {
    if (CNW(u8, 0xE38) != 0) {
        switch (CNW(u16, 0xFE6)) { /* irregular */
        case 16:
            __cnet_Recv_PatchStart(CNW(u16, 0xFE6));
            CNW(s32, 0x1000) = 0;
            CNW(s16, 0x1004) = 0;
            CNW(s32, 0xFFC) = (s32) CNW(s32, 0x1054);
            return;
        }
    } else {
    case 2:
    }
}

void __cnet_Recv_PatchStart(void) {
    int sp14;
    int sp10;

    memset(&sp10, 0, 0x18);
    GetRecvData32(CNWP(0x100C), GetRecvData32(CNWP(0x1008), GetRecvData16(CNWP(0x1006), GetRecvDataString(&sp10, &recv_work))));
    memset(CNWP(0x1024), 0, 8);
    memcpy(CNWP(0x1024), &sp10, 4);
    memset(CNWP(0x1014), 0, 0x10);
    memcpy(CNWP(0x1014), &sp14, 0xA);
}

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}

void __cnet_Recv_PatchData(void) {
    u16 sp1E;
    u16 sp1C;

    GetRecvDataOption(CNW(s32, 0xFFC), GetRecvData16(&sp1E, GetRecvData16(&sp1C, &recv_work)), sp1E);
    CNW(s32, 0xFFC) = (s32) (CNW(s32, 0xFFC) + sp1E);
}

void _cnet_RecvFromLbs_ReqestPatchLineCheck(void) {
    u16 sp1E;

    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_Word(&sp1E);
        __cnet_Send_PatchLineCheck(sp1E);
    }
}

s32 __cnet_Send_PatchLineCheck(s32 arg0) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xC2) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_NoticePatchFooter(void) {

}

void _cnet_RecvFromLbs_RequestPatchFinish(void) {
    s8 sp19;
    s8 sp18;

    if (CNW(u8, 0xE38) != 0) {
        if (__cnet_CheckCheckSum(CNW(s32, 0x1054), CNW(s32, 0x1008), CNW(s32, 0x100C)) != 0) {
            sp18 = 0;
            sp19 = 3;
            M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
            return;
        }
        sp18 = -1;
        sp19 = 9;
        M2C_FIELD(&CnetSys_w, int (**)(s64, s8 *), 0xE1C)((s64) sp18, &sp18);
    }
}

s32 cnLBS_Answer_PatchFinish(void) {
    __cnet_Send_PatchFinish();
    return 0;
}

s32 __cnet_Send_PatchFinish(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 0xC4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

s32 cnLBS_Get_PatchInformation(s32 *arg0) {
    memset(0, 0x1C);
    strncpy(arg0 + 4, CNWP(0x1014), 0xA);
    strncpy(arg0 + 0x14, CNWP(0x1024), 4);
    *arg0 = CNW(s32, 0x1008);
    return 0;
}

s32 __cnet_CheckCheckSum(u8 *arg0, u32 arg1, s32 arg2) {
    s32 var_t1;
    u32 var_t2;
    u8 *var_a0;
    void *temp_a0;
    void *temp_a0_2;
    void *temp_a0_3;

    var_a0 = arg0;
    var_t1 = 0;
    var_t2 = 0;
    if (arg1 != 0) {
        if (arg1 >= 9U) {
            do {
                var_t2 += 8;
                temp_a0 = var_a0 + 2;
                temp_a0_2 = temp_a0 + 2;
                temp_a0_3 = temp_a0_2 + 2;
                var_t1 = var_t1 + *(u8 *)((u8 *)var_a0 + 0) + *(u8 *)((u8 *)var_a0 + 1) + *(u8 *)((u8 *)var_a0 + 2) + *(u8 *)((u8 *)temp_a0 + 1) + *(u8 *)((u8 *)temp_a0 + 2) + *(u8 *)((u8 *)temp_a0_2 + 1) + *(u8 *)((u8 *)temp_a0_2 + 2) + *(u8 *)((u8 *)temp_a0_3 + 1);
                var_a0 = temp_a0_3 + 2;
            } while (var_t2 < (u32) (arg1 - 8));
        }
        if (var_t2 < arg1) {
            do {
                var_t2 += 1;
                var_t1 += *var_a0;
                var_a0 += 1;
            } while (var_t2 < arg1);
        }
    }
    return arg2 == var_t1;
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

void cnLBS_Set_CallBackNoticeEvent(s32 arg0, s32 arg1) {
    *(&pFunc + (arg0 * 4)) = arg1;
}

void _cnetEvent_JumpCallBack(s8 arg0) {
    s8 sp19;
    s8 sp18;
    int (*temp_v1)(s64, int);

    sp19 = arg0;
    sp18 = 1;
    temp_v1 = *(&pFunc + ((arg0 & 0xFFFF) * 4));
    if (temp_v1 != NULL) {
        temp_v1((s64) sp18, 0);
    }
}

void _cnet_Return_CallBack(s32 arg0) {
    s8 sp18;

    switch (CNW(u16, 0xFE6)) {  /* irregular */
    case 16:
        if (arg0 != 0) {
            _cnetEvent_JumpCallBack(0);
        }
        return;
    case 2:
        if (CNW(s8, 0xFEC) == 0) {
            sp18 = 0;
        } else {
            sp18 = -1;
            __cnet_Recv_ServerMessage(CNW(u16, 0xFE6));
        }
        __cnetSub_Return_BgProcess((s64) sp18, 1, 0);
        return;
    }
}

void cnLBS_Init_LobbyBgProcess(void) {
    memset(CNWP(0x18), 0);
}

void cnLBS_Init_LobbyBgBurstProcess(void) {
    memset(CNWP(0xE18), 0);
}

s32 __cnetSub_Set_BgProcess(s8 arg0, s32 arg1, s32 arg2) {
    int *var_a3;
    s32 temp_t1;
    s32 var_v0;

    var_v0 = 0;
    var_a3 = &CnetSys_w;
loop_1:
    if (*(u8 *)((u8 *)var_a3 + 0x30) == 0) {
        temp_t1 = var_v0 * 0x1C;
        *(CNWP(0x30) + temp_t1) = arg0;
        *(CNWP(0x31) + temp_t1) = 0;
        *(CNWP(0x1C) + temp_t1) = arg2;
        *(CNWP(0x18) + temp_t1) = arg1;
        return var_v0;
    }
    var_v0 += 1;
    var_a3 += 0x1C;
    if (var_v0 >= 0x80) {
        return -1;
    }
    goto loop_1;
}

s32 __cnetSub_Return_BgProcess(s64 arg0, s32 arg1, s32 arg2) {
    s64 sp28;
    int (*temp_v0)(s64, s64 *, int *);
    int (*temp_v0_2)(s64, s64 *);
    int *var_a2;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 var_s0;

    sp28 = arg0;
    switch (arg1) {                                 /* irregular */
    case 1:
        var_s0 = 0;
        var_a2 = &CnetSys_w;
loop_2:
        if ((arg1 == 1) && (CNW(u16, 0xFE8) == *(u16 *)((u8 *)var_a2 + 0x2E))) {
            temp_a1 = var_s0 * 0x1C;
            *(CNWP(0x30) + temp_a1) = 0;
            *(CNWP(0x31) + temp_a1) = 0;
            temp_v0 = *(CNWP(0x1C) + temp_a1);
            if (temp_v0 != NULL) {
                temp_v0(sp28, &sp28, var_a2);
            }
            return var_s0;
        }
        var_s0 += 1;
        var_a2 += 0x1C;
        if (var_s0 >= 0x80) {
        default:
            return -1;
        }
        goto loop_2;
    case 2:
        temp_a1_2 = arg2 * 0x1C;
        *(CNWP(0x30) + temp_a1_2) = 0;
        *(CNWP(0x31) + temp_a1_2) = 0;
        temp_v0_2 = *(CNWP(0x1C) + temp_a1_2);
        if (temp_v0_2 != NULL) {
            temp_v0_2(sp28, &sp28);
        }
        return arg2;
    }
}

void __cnetSub_Run_BgProcess(void) {
    int (*temp_v1)(s32);
    int (*temp_v1_2)(s32);
    int *var_s0;
    int *var_s1_2;
    s32 var_s0_2;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = &CnetSys_w;
    do {
        if (*(u8 *)((u8 *)var_s0 + 0x30) == 2) {
            temp_v1 = M2C_FIELD(var_s0, int (**)(s32), 0x18);
            if (temp_v1 != NULL) {
                temp_v1(var_s1);
            }
        }
        var_s1 += 1;
        var_s0 += 0x1C;
    } while (var_s1 < 0x80);
    var_s0_2 = 0;
    var_s1_2 = &CnetSys_w;
    do {
        if (*(u8 *)((u8 *)var_s1_2 + 0xE38) == 1) {
            temp_v1_2 = M2C_FIELD(var_s1_2, int (**)(s32), 0xE18);
            if (temp_v1_2 != NULL) {
                temp_v1_2(var_s0_2);
            }
        }
        var_s0_2 += 1;
        var_s1_2 += 0x24;
    } while (var_s0_2 < 0xC);
}

void __cnetSub_Get_RestBgWork(void) {
    int *var_a0;
    s32 var_a1;

    var_a1 = 0;
    var_a0 = &CnetSys_w;
    do {
        if (*(u8 *)((u8 *)var_a0 + 0x30) == 0) {

        }
        var_a1 += 1;
        var_a0 += 0x1C;
    } while (var_a1 < 0x80);
}

s32 __cnet_RecvFromLbs(s32 arg0, s32 arg2) {
    int (**var_t0)(s32, s32, s32, s32);
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a3;
    s32 var_t5;
    u8 *var_t1;
    u8 *var_t2;
    u8 *var_t3;
    u8 *var_t4;

    temp_a3 = arg0 & 0xFFFF;
    temp_a2 = arg2 & 0xFF;
    var_t5 = 0;
    var_t4 = &lbs_command_tbl_h;
    var_t3 = &lbs_command_tbl_l;
    var_t2 = &lbs_fromto_tbl;
    var_t1 = &lbs_category_tbl;
    var_t0 = &lbs_command_jmp;
loop_1:
    temp_a1 = (*var_t4 << 8) & 0xFFFF;
    temp_a0 = (temp_a1 | *var_t3) & 0xFFFF;
    if ((*var_t2 != 8) && (temp_a3 == (temp_a0 & 0xFFFF)) && (*var_t1 == temp_a2) && (*var_t0 != NULL)) {
        (&lbs_command_jmp)[var_t5](temp_a0, temp_a1, temp_a2, temp_a3);
        return 1;
    }
    var_t5 += 1;
    var_t4 += 1;
    var_t3 += 1;
    var_t2 += 1;
    var_t1 += 1;
    var_t0 += 4;
    if (var_t5 >= 0x102) {
        return 0;
    }
    goto loop_1;
}

s32 cnLBS_RecvData(s32 arg0) {
    s32 var_s0;
    s32 var_s1;

    if (*(s32 *)((u8 *)CNWP(0)) == 0) {
        return 0;
    }
    CNW(s32, 0xFDC) = arg0;
    var_s0 = 0;
    var_s1 = 0;
    CNW(u16, 0x102C) = (u16) (CNW(u16, 0x102C) + 1);
    do {
        CNW(s32, 0xFE0) = select_ps2(CNW(s32, 0xFDC), &recv_header, &recv_work, 0x600);
        if ((CNW(s32, 0xFE0) != -1) && (CNW(s32, 0xFE0) != 0)) {
            var_s0 = 1;
            __cnetSub_RecvThreeData();
        }
        var_s1 += 1;
    } while (var_s1 < 4);
    __cnetSub_Run_BgProcess();
    return var_s0;
}

void __cnetSub_RecvThreeData(void) {
    s16 temp_a0;

    temp_a0 = ((*(u8 *)((u8 *)&recv_header + 2) << 8) & 0xFFFF) | *(u8 *)((u8 *)&recv_header + 3);
    CNW(s16, 0xFE6) = (s16) *(u8 *)((u8 *)&recv_header + 1);
    CNW(s8, 0xFEC) = (s8) *(s8 *)((u8 *)&recv_header + 8);
    CNW(s16, 0xFE4) = temp_a0;
    CNW(u16, 0xFE8) = (u16) (*(u8 *)((u8 *)&recv_header + 6) << 8);
    CNW(u16, 0xFEA) = (u16) (*(u8 *)((u8 *)&recv_header + 4) << 8);
    CNW(u16, 0xFE8) = (u16) (CNW(u16, 0xFE8) | *(u8 *)((u8 *)&recv_header + 7));
    CNW(u16, 0xFEA) = (u16) (CNW(u16, 0xFEA) | *(u8 *)((u8 *)&recv_header + 5));
    __cnet_RecvFromLbs(temp_a0 & 0xFFFF, ((s32) *(u8 *)((u8 *)&recv_header + 0) >> 4) & 0xF, *(u8 *)((u8 *)&recv_header + 1), *(u8 *)((u8 *)&recv_header + 5));
}

s32 cnLBS_LogoutLobbyServer(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendSet_Logout();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendSet_Logout(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerLogOut(void) {
    s8 sp18;

    if (CNW(u16, 0xFE6) == 2) {
        if (CNW(s8, 0xFEC) == 0) {
            sp18 = 0;
        } else {
            sp18 = -1;
            __cnet_Recv_ServerMessage(CNW(u16, 0xFE6));
        }
        __cnetSub_Return_BgProcess((s64) sp18, 1, 0);
    }
}

s32 cnLBS_ShutDownLobbyServer(s32 arg0) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg0);
    var_v0 = -1;
    if (temp_v0 != -1) {
        CnetSys_w.bg[temp_v0].cmd = __cnet_SendSet_ShutDown();
        var_v0 = temp_v0;
    }
    return var_v0;
}

s32 __cnet_SendSet_ShutDown(void) {
    s32 temp_s0;

    temp_s0 = SetSendCommand(&send_work, 4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s0;
}

void _cnet_RecvFromLbs_AnswerShutDown(void) {
    s8 sp18;

    if (CNW(u16, 0xFE6) == 2) {
        if (CNW(s8, 0xFEC) == 0) {
            sp18 = 0;
        } else {
            sp18 = -1;
            __cnet_Recv_ServerMessage(CNW(u16, 0xFE6));
        }
        __cnetSub_Return_BgProcess((s64) sp18, 1, 0);
    }
}

s32 cnLBS_Get_ServerMessage(void) {
    strcpy(CNWP(0x378C0));
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
    _cnetEvent_JumpCallBack(6);
}

void _cnet_RecvFromLbs_NoticeMatchCancel(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(7, 0);
}

void _cnet_RecvFromLbs_NoticeLobbyFull(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(8, 0);
}
