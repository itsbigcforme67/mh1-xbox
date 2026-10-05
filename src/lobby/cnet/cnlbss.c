/* cnlbs, run 19: __cnet_SendSet_ConnectionPair .. __cnet_SendReq_EchoPacket (lobby.bin 0x005AB430-0x005AB7FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void __cnet_SendSet_ConnectionPair(void) {
    char sp10[0x10];

    SetSendCommand(&send_work, 0xD);
    mmbbc_encode(sp10, CnetSys_w.login.key, (((send_work.seq_h << 8) & 0xFFFF) + send_work.seq_l) & 0xFFFF);
    SetSendData16(&send_work, 0xA);
    SetSendStringData(&send_work, sp10, 0xA);
    SetSendEncodeStringData(&send_work, CnetSys_w.login.pass, strlen(CnetSys_w.login.pass) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_FirstData(void) {
    u8 *s0 = (u8 *)&CnetSys_w.firstdata;

    SetSendCommand(&send_work, 0x11);
    SetSendData8(&send_work, s0[0]);
    SetSendData8(&send_work, s0[1]);
    SetSendData8(&send_work, s0[2]);
    SetSendStringData2(&send_work, s0 + 4, 0xA);
    SetSendData16(&send_work, *(u16 *)(s0 + 0x14));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x16));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x18));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1A));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1C));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1E));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x20));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x22));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_UserID(void) {
    SetSendCommand(&send_work, 0x16);
    SetSendStringData2(&send_work, CnetSys_w.login_users[3].id, 6);
    SetSendStringData2(&send_work, CnetSys_w.login_users[3].handle, strlen(CnetSys_w.login_users[3].handle) & 0xFFFF);
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
    SetSendStringData2(&send_work, CnetSys_w.login.tel, strlen(CnetSys_w.login.tel) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_SendSet_MiniDataRegist(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x21) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void __cnet_SendReq_EchoPacket(void) {
    CnetSys_w.rcnt = 0;
    SetSendCommand(&send_work, 0xA);
    SetSendStringData2(&send_work, "0", 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}
