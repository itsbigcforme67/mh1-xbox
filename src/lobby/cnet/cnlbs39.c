/* cnlbs, run 40: __cnet_Recv_ServerMessage .. SetSendData32 (lobby.bin 0x005ADCE0-0x005AE160): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
