/* cnlbs, run 7: __cnet_Login_Return .. cnLBS_Get_MatchInfomation (lobby.bin 0x005AADE0-0x005ACB84): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

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

int __cnet_Recv_UserIDandHandle(void) {
    u8 n;
    int i;
    u8 *s0;
    int p;

    memset(CnetSys_w.login_users, 0, 0x170);
    p = GetRecvData8(&n, recv_work);
    if (n > 3) n = 3;
    CnetSys_w.n_login_user = n;
    i = 0;
    if (0 < n) {
        s0 = (u8 *)&CnetSys_w;
        do {
            p = GetRecvDataOption3(s0 + 0x147E, 0x40, GetRecvDataOption3(s0 + 0x146A, 0x10, GetRecvDataOption3(s0 + 0x1462, 8, p)));
            i++;
            s0 += 0x5C;
        } while (i < n);
    }
    return 0;
}

int __cnet_Recv_UserID(void) {
    GetRecvDataOption3(CNWP(0x1576), 8, &recv_work);
    return 0;
}

void cnLBS_Send_ChatMessage(int a, int b) {
    __cnet_SendSet_ChatMessage(0, a, b);
}

void cnLBS_Get_ChatMessage(CNET_CHAT *d) {
    *d = CnetSys_w.chat;
}

void __cnet_SendSet_ChatMessage(int arg0, int arg1, int arg2) {
    SetSendCommand(&send_work, 0xE8);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatMessage(void) {
    memset(CnetSys_w.chat.from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat.d, GetRecvData8(&CnetSys_w.chat.c, GetRecvData8(&CnetSys_w.chat.b, GetRecvData8(&CnetSys_w.chat.a, GetRecvDataOption3(CnetSys_w.chat.msg, 0x100, GetRecvDataOption3(CnetSys_w.chat.x, 0x10, GetRecvDataOption3(CnetSys_w.chat.from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(5, 0);
}

void cnLBS_Send_ChatBinary(void) {
    __cnet_SendSet_ChatBinary();
}

void cnLBS_Get_ChatBinary(CNET_B308 *d) {
    *d = CnetSys_w.chatbin;
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

int cnLBS_Send_ChatMessageTU(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ChatMessageTU(arg0, arg1, arg2);
        return slot;
    }
    return -1;
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

void _cnet_RecvFromLbs_AnswerChatMessageTU(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeChatMessageTU(void) {
    memset(CnetSys_w.chat.from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat.d, GetRecvData8(&CnetSys_w.chat.c, GetRecvData8(&CnetSys_w.chat.b, GetRecvData8(&CnetSys_w.chat.a, GetRecvDataOption3(CnetSys_w.chat.msg, 0x100, GetRecvDataOption3(CnetSys_w.chat.x, 0x10, GetRecvDataOption3(CnetSys_w.chat.from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(0x2A, 0);
}

int cnLBS_Send_ChatBinaryTU(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ChatBinaryTU(arg0, arg1, arg2);
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ChatBinaryTU(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0xFB) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
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

int cnLBS_MatchEntry(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_MatchEntry(arg0);
        return slot;
    }
    return -1;
}

int __cnet_SendReq_MatchEntry(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
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
        memset(&CnetSys_w.matchinfo, 0, 0x5D4);
        __cnet_SendReq_MatchJoin();
        CnetSys_w.burst[7].cb = (void *)cb;
        CnetSys_w.burst[7].state = 1;
        CnetSys_w.burst[7].x21 = 0;
        CnetSys_w.burst[7].run = 0;
        return 0;
    }
    return -1;
}

void _cnet_RecvFromLbs_MatchJoin(void) {
    u8 v;
    R7 res;
    if (CnetSys_w.burst[7].state != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                __cnet_Recv_Byte(&v);
                CNW(u8, 0x30310) = v;
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchPlSide(0);
    }
}

void _cnet_RecvFromLbs_MatchPlSide(void) {
    u8 v;
    R7 res;

    if (CnetSys_w.burst[7].state != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                __cnet_Recv_Byte(&v);
                if (v != 0) v -= 1;
                CNW(u8, 0x30311) = v;
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        pl_infoget_ctr = 1;
        __cnet_SendReq_MatchOpponentInfo(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentInfo(void) {
    u8 idx;
    R7 r;
    u8 *p;

    if (CNW(u8, 0xF34) != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                p = CNWP(0x30310);
                GetRecvData8(p + (idx - 1) * 0x98 + 0x1A9, GetRecvDataString(p + (idx - 1) * 0x98 + 0x170, GetRecvDataString(p + (idx - 1) * 0x98 + 0x130, GetRecvDataString(p + (idx - 1) * 0x98 + 0x11C, GetRecvDataString(p + (idx - 1) * 0x98 + 0x114, GetRecvData8(p + (idx - 1) * 0x98 + 0x1AA, GetRecvData8(&idx, recv_work)))))));
                (p + idx * 0x98)[0x110] = idx;
            } else {
                r.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(r);
                return;
            }
        }
        pl_infoget_ctr++;
        if (pl_infoget_ctr <= CNW(u8, 0x30310)) {
            __cnet_SendReq_MatchOpponentInfo(pl_infoget_ctr);
            return;
        }
        pl_infoget_ctr = 1;
        __cnet_SendReq_MatchOpponentStatus(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentStatus(void) {
    u8 idx;
    R7 res;
    u8 *p;

    if (CNW(u8, 0xF34) != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                p = CNWP(0x30310);
                GetRecvData32(p + (idx - 1) * 0x98 + 0x1A4, GetRecvData32(p + (idx - 1) * 0x98 + 0x1A0, GetRecvData32(p + (idx - 1) * 0x98 + 0x19C, GetRecvData32(p + (idx - 1) * 0x98 + 0x198, GetRecvData32(p + (idx - 1) * 0x98 + 0x194, GetRecvData16(p + (idx - 1) * 0x98 + 0x190, GetRecvData8(&idx, recv_work)))))));
                (p + idx * 0x98)[0x110] = idx;
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        pl_infoget_ctr++;
        if (pl_infoget_ctr <= CNW(u8, 0x30310)) {
            __cnet_SendReq_MatchOpponentStatus(pl_infoget_ctr);
            return;
        }
        __cnet_SendReq_MatchBattleCode();
    }
}

void _cnet_RecvFromLbs_MatchBattleCode(void) {
    CNET_RES res;
    u8 *p;

    if (CnetSys_w.burst[7].state != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(CNWP(0x30312), recv_work);
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchGameRule();
    }
}

void _cnet_RecvFromLbs_MatchGameRule(void) {
    CNET_RES res;
    u8 *p;

    if (CnetSys_w.burst[7].state != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(CNWP(0x30323), recv_work);
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchMcsIpAddr();
    }
}

void _cnet_RecvFromLbs_MatchGameServerAddr(void) {
    CNET_RES res;

    if (CNW(u8, 0xF34) != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                res.val = 0;
                GetRecvDataString(CNWP(0x30308), GetRecvDataString(CNWP(0x30300), recv_work));
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        res.val = 0;
        __cnet_Return_MatchInformation(res);
    }
}

void __cnet_Return_MatchInformation(CNET_RES res) {
    if (res.val == -1) {
        __cnet_SendReq_MatchRejection(res.val);
    }
    if (CnetSys_w.burst[7].cb != 0) {
        CnetSys_w.burst[7].state = 0;
        CnetSys_w.burst[7].x21 = 0;
        CnetSys_w.burst[7].cb(res, &res);
    }
}

int __cnet_SendReq_MatchJoin(void) {
    int cmd = SetSendCommand(&send_work, 0xA3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
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

int __cnet_SendReq_MatchRejection(void) {
    int cmd = SetSendCommand(&send_work, 0xAD) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Get_MatchInfomation(CNET_W5D4 *d) {
    *d = CnetSys_w.matchinfo;
    return 0;
}
