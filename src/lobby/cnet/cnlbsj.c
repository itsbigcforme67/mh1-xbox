/* cnlbs, run 10: cnLBS_LogoutLobbyServer .. Write_Socket (lobby.bin 0x005AD7E0-0x005AE320): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

int cnLBS_LogoutLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_Logout();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_Logout(void) {
    int cmd = SetSendCommand(&send_work, 2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerLogOut(void) {
    CNET_RES res;

    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage(CnetSys_w.rcat);
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
}

int cnLBS_ShutDownLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ShutDown();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ShutDown(void) {
    int cmd = SetSendCommand(&send_work, 4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerShutDown(void) {
    CNET_RES res;

    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage(CnetSys_w.rcat);
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
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

int GetRecvData8(dst, src)
u8 *dst;
u8 *src;
{
    *dst = *src;
    return (int)(src + 1);
}

int GetRecvData16(dst, src)
u8 *dst;
u8 *src;
{
    dst[1] = src[0];
    dst[0] = src[1];
    return (int)(src + 2);
}

int GetRecvData32(dst, src)
u8 *dst;
u8 *src;
{
    dst[3] = src[0];
    dst[2] = src[1];
    dst[1] = src[2];
    dst[0] = src[3];
    return (int)(src + 4);
}

int GetRecvDataString(dst, src)
char *dst;
u8 *src;
{
    u16 n;
    int m;

    n = (u16)(src[0] << 8) | src[1];
    memcpy(dst, src + 2, n);
    m = n & 0xFFFF;
    dst[m] = 0;
    return (int)(src + (m + 2));
}

int GetRecvDataOption(dst, src, len)
void *dst;
u8 *src;
u16 len;
{
    memcpy(dst, src, len);
    return (int)(src + len);
}

int GetRecvDataOption3(dst, maxlen, src)
void *dst;
u16 maxlen;
u8 *src;
{
    int t;
    int v;
    u16 hi;

    hi = src[0] << 8;
    v = (hi | src[1]) & 0xFFFF;
    t = v;
    if (maxlen < v) v = maxlen;
    if (v != 0) {
        memcpy(dst, src + 2, v & 0xFFFF);
    }
    return (int)(src + (t + 2));
}

void __cnet_Recv_ServerMessage(void) {
    memset(CnetSys_w.srvmsg, 0, 0x300);
    GetRecvDataOption3(CnetSys_w.srvmsg, 0x300, recv_work);
}

void __cnet_Recv_Byte(a0)
void *a0;
{
    GetRecvData8(a0, recv_work);
}

void __cnet_Recv_Word(a0)
void *a0;
{
    GetRecvData16(a0, recv_work);
}

void __cnet_Recv_Long(a0)
void *a0;
{
    GetRecvData32(a0, recv_work);
}

void __cnet_Recv_ByteString(a0, a1)
void *a0;
void *a1;
{
    GetRecvDataString(a1, GetRecvData8(a0, recv_work));
}

void __cnet_Recv_ByteByte(a0, a1)
void *a0;
void *a1;
{
    GetRecvData8(a1, GetRecvData8(a0, recv_work));
}

void __cnet_Recv_WordByte(a0, a1)
void *a0;
void *a1;
{
    GetRecvData8(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_WordWord(a0, a1)
void *a0;
void *a1;
{
    GetRecvData16(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_WordLong(a0, a1)
void *a0;
void *a1;
{
    GetRecvData32(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_ByteByteString(a0, a1, a2)
void *a0;
void *a1;
void *a2;
{
    GetRecvDataString(a2, GetRecvData8(a1, GetRecvData8(a0, recv_work)));
}

u16 SetSendCommand(w, cmd)
SEND_WORK *w;
int cmd;
{
    int c;

    memset(w->data, 0, 0x300);
    c = cmd & 0xFFFF;
    w->cmd_h = lbs_command_tbl_h[c];
    w->cmd_l = lbs_command_tbl_l[c];
    w->cat = lbs_category_tbl[c];
    w->total = 0;
    w->len = 0;
    w->magic = 0x81;
    if (w->cat == 2) {
        send_work.seq_h = recv_header[6];
        send_work.seq_l = recv_header[7];
    } else {
        seq_no++;
        w->seq_h = (int)seq_no >> 8;
        w->seq_l = seq_no;
    }
    w->x0D = 0xFF;
    w->x0E = 0xFF;
    w->x0F = 0xFF;
    w->x0C = 0;
    return seq_no;
}

void Mcs_SetSendCommand(w, cmd)
SEND_WORK *w;
int cmd;
{
    int c;

    memset(w->data, 0, 0x300);
    c = cmd & 0xFFFF;
    w->cmd_h = c >> 8;
    w->cmd_l = c;
    w->total = 0;
    w->len = 0;
    w->magic = 0x82;
    w->x0D = 0xFF;
    w->x0E = 0xFF;
    w->x0F = 0xFF;
    w->cat = 0;
    w->x0C = 0;
    memcpy(&w->seq_h, recv_header + 6, 2);
}

void SetSendCategory(w, v)
SEND_WORK *w;
s8 v;
{
    w->cat = v;
}

void SetSendResult(w, v)
SEND_WORK *w;
s8 v;
{
    w->x0C = v;
}

void SetSendCommandLen(w)
SEND_WORK *w;
{
    w->len_h = (int)w->len >> 8;
    w->len_l = w->len;
}

void SetSendData8(w, v)
SEND_WORK *w;
s8 v;
{
    *((u8 *)w + w->len + 0x10) = v;
    w->total += 1;
    w->len += 1;
}

void SetSendData16(w, v)
SEND_WORK *w;
int v;
{
    int b;
    SEND_WORK *t;

    b = v & 0xFFFF;
    t = (SEND_WORK *)((u8 *)w + w->len);
    t->data[0] = b >> 8;
    t->data[1] = b;
    w->total += 2;
    w->len += 2;
}

void SetSendData32(w, v)
SEND_WORK *w;
u32 v;
{
    u8 *t = (u8 *)w + w->len;

    t[0x10] = v >> 24;
    t[0x11] = v >> 16;
    t[0x12] = v >> 8;
    t[0x13] = v;
    w->total += 4;
    w->len += 4;
}

void SetSendStringData(w, src, len)
SEND_WORK *w;
void *src;
u16 len;
{
    memcpy((u8 *)w + w->len + 0x10, src, len);
    w->total += len;
    w->len += len;
}

void SetSendStringData2(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x108];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((u16)len + 2) & 0xFFFF);
    SetSendData16(w, n & 0xFFFF);
    SetSendStringData(w, buf, len);
}

void SetSendEncodeStringData(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x108];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((u16)len + 2) & 0xFFFF);
    SetSendData16(w, n & 0xFFFF);
    SetSendStringData(w, buf, len);
}

void Write_Socket(w)
u16 *w;
{
    CpInetTcpSend(CnetSys_w.sock, (u8 *)w + 4, (*w + 0xC) << 16 >> 16);
}
