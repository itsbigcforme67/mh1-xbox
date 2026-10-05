/* cnlbs, run 26: cnetGet_Login_NoOfUserAccount .. _cnet_RecvFromLbs_RequestFirstData (lobby.bin 0x005AA430-0x005AA5FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

u8 cnetGet_Login_NoOfUserAccount(void) {
    return CNW(u8, 0x145E);
}

int cnetGet_Login_UserID(idx, d)
int idx;
char *d;
{
    int k = idx & 0xFF;

    strcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x1462);
    return 0;
}

int cnetGet_Login_UserHandle(idx, d)
int idx;
char *d;
{
    int k = idx & 0xFF;

    strcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x146A);
    return 0;
}

int cnetGet_Login_UserMiniData(idx, d)
int idx;
void *d;
{
    int k = idx & 0xFF;

    memcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x147E, 0x40);
    return 0;
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
