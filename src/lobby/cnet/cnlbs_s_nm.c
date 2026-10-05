#include "lbnet.h"

int SetSendCommand();
int SetSendCommandLen();
int SetSendData16();
int SetSendData32();
int SetSendData8();
int SetSendStringData2();
int Write_Socket();
int strlen();

int __cnet_SendReq_SendMail(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0xF1) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_SearchUser(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEA) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_Send_ConditionSearchUserCertify(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEE) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PersonalDataChange(void) {
    int cmd = SetSendCommand(&send_work, 0xB2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PersonalDataRegisted(void) {
    int cmd = SetSendCommand(&send_work, 0xBA) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomJoinInfo(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x8D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomProperty(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x98) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_SetRoomProperty(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x96) & 0xFFFF;
    SetSendData32(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomNamePermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x55) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomPasswordPermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x57) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomPasswordInfo(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x86) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomMember(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x8B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_TopPageJump(void) {
    int cmd = SetSendCommand(&send_work, 0x2C) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomEntry(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x73) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_NumOfRule(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x59) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListHeadWord(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleNumOfChoice(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x61) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListNow(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListPermission(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListName(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0x63) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleControl(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0x65) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomCreate(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x53) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetName(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x67) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetPassword(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x69) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetRule(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x6B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetFinish(void) {
    int cmd = SetSendCommand(&send_work, 0x6D) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_LobbyMemberList(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xFE) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_MatchEntryUser(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetExplain(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x71) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomExplainPermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x6F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_TimingValue(void) {
    int cmd = SetSendCommand(&send_work, 0xD3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_CurrentPlace(void) {
    int cmd = SetSendCommand(&send_work, 0xD6) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_TopInformation(void) {
    int cmd = SetSendCommand(&send_work, 0x1F) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendSet_MiniDataRegist(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x21) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendSet_ChatMessageTU(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0xF8) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, 0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendSet_ChatBinaryTU(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0xFB) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_MatchEntry(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_MatchJoin(void) {
    int cmd = SetSendCommand(&send_work, 0xA3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_MatchRejection(void) {
    int cmd = SetSendCommand(&send_work, 0xAD) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_Send_PatchLineCheck(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xC2) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_Send_PatchFinish(void) {
    int cmd = SetSendCommand(&send_work, 0xC4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendSet_Logout(void) {
    int cmd = SetSendCommand(&send_work, 2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendSet_ShutDown(void) {
    int cmd = SetSendCommand(&send_work, 4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
