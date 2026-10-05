/* cnlbs, run 10: __cnet_Login_Return .. __cnet_SendReq_EchoPacket (lobby.bin 0x005AADE0-0x005AB7FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void __cnet_Login_Return(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        res.val = 0;
        CnetSys_w.burst[0].state = 0;
        res.id = 0;
        CnetSys_w.burst[0].x21 = 0;
        CnetSys_w.burst[0].cb(res, &res);
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
    CNET_BATRES *b = &CnetSys_w.batres;

    SetSendCommand(&send_work, 0x19);
    if (b->flag != 0) {
        SetSendData16(&send_work, 0x5678);
        SetSendStringData2(&send_work, b, 0xF);
        SetSendData8(&send_work, 0);
        SetSendData8(&send_work, 0);
        SetSendData8(&send_work, b->flag);
        SetSendData16(&send_work, b->v[0]);
        SetSendData16(&send_work, b->v[1]);
        SetSendData16(&send_work, b->v[2]);
        SetSendData16(&send_work, b->v[3]);
        SetSendData16(&send_work, b->v[4]);
        SetSendData16(&send_work, b->v[5]);
        SetSendData16(&send_work, b->v[6]);
        SetSendData16(&send_work, b->v[7]);
    } else {
        SetSendResult(&send_work, 0xFF);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_AnswerPersonalRecordHeader(void) {
    u8 n;
    int r;
    int i;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&n, recv_work);
            if (n > 2) {
                CNW(u8, 0x30CAC) = n;
                for (i = 0; i < n; i++) {
                    r = GetRecvData8(CNWP(0x30CAD) + i, r);
                }
            } else {
                CNW(u8, 0x30CAC) = 2;
            }
            CNW(s8, 0x30CAD) = 1;
            CNW(s8, 0x30CAE) = 0x11;
        }
        CNW(s8, 0xF5A) = 1;
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordData(void) {
    u8 a;
    u8 b;
    int r;
    int i;
    u8 *p;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&b, GetRecvData8(&a, recv_work));
            p = CNWP(0x30CB8) + a * 0x3480 + b * 0x118;
            p[0] = a;
            p[1] = b;
            r = GetRecvData32(p + 0x28, GetRecvData32(p + 0x24, GetRecvData32(p + 0x20, GetRecvData32(p + 0x1C, GetRecvData32(p + 0x18, GetRecvData32(p + 0x14, GetRecvData8(p + 0xC, GetRecvData32(p + 8, GetRecvData32(p + 4, GetRecvData32(p + 0x10, r))))))))));
            if (a == 1) {
                if (CnetSys_w.rseq2 > 0x27) {
                    for (i = 0; i < 3; i++) {
                        r = GetRecvDataOption3(p + 0x91 + i * 0x11, 0x10, GetRecvDataOption3(p + 0x79 + i * 8, 8, GetRecvData8(p + 0x76 + i, GetRecvData8(p + 0x73 + i, r))));
                    }
                    for (i = 0; i < 3; i++) {
                        r = GetRecvDataOption3(p + 0xE2 + i * 0x11, 0x10, GetRecvDataOption3(p + 0xCA + i * 8, 8, GetRecvData8(p + 0xC7 + i, GetRecvData8(p + 0xC4 + i, r))));
                    }
                }
            }
        }
        CNW(u8, 0xF5A)++;
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordVide(void) {
    u8 a;
    u8 b;
    int r;
    u8 *p;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&b, GetRecvData8(&a, recv_work));
            p = CNWP(0x30CB8) + a * 0x3480 + b * 0x118;
            GetRecvData8(p + 0x72, GetRecvData8(p + 0x71, GetRecvData8(p + 0x70, GetRecvData8(p + 0x6F, GetRecvData8(p + 0x6E, GetRecvData8(p + 0x6D, GetRecvDataString(p + 0x2C, r)))))));
        }
        CNW(u8, 0xF5A)++;
    }
}

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
