/* lb_cmcs04 - agent C 0x005B6CC0-0x005B6EB8: cmcs_04 (ConnWork/InetGame symbols). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern u8 InetGame[0x14];
extern s32 ConnWork[];
extern u8 COM_R_No_1;
extern u8 recv_header[];
extern u8 recv_work[];
extern char D_3A3B71[];
typedef struct { u16 total; u16 len; u8 x04[6]; u8 seq_h; u8 seq_l; u8 x0C[4]; u8 data[0x300]; } SW;
extern SW send_work;
void cmcs_04(void) {
    char pkt[0x20];
    char enc[0x10];
    s16 cnt;
    int len;

    cnt = Vs_Cnt_0;
    Vs_Cnt_0 = (s16) (cnt - 1);
    if (cnt < 0) {
        COM_R_No_1 = 7U;
        InetGame[3] = 5;
        return;
    }
    switch (select_ps2(ConnWork[1], recv_header, recv_work, 0x600)) {
    case 1:
        break;
    case 0:
        return;
    case -1:
        COM_R_No_1 = 7U;
        InetGame[3] = 5;
        return;
    }
    switch (((u16)(recv_header[2] << 8) | recv_header[3]) & 0xFFFF) {
    case 0x1031:
        mmbbc_encode(enc, &D_3A3B71, ((u16)(recv_header[6] << 8) + recv_header[7]) & 0xFFFF);
        Mcs_SetSendCommand(&send_work, 0x1031);
        SetSendCategory(&send_work, 2);
        send_work.seq_h = recv_header[6];
        send_work.seq_l = recv_header[7];
        SetSendStringData(&send_work, enc, 0xA);
        SetSendCommandLen(&send_work);
        memcpy(pkt, (u8 *)&send_work + 4, 0xC);
        memcpy(pkt + 0xC, send_work.data, send_work.total);
        len = send_work.total + 0xC;
        if (len != CpInetTcpSend(ConnWork[1], pkt, (s16) len)) {
            COM_R_No_1 = 7U;
            InetGame[3] = 5;
            return;
        }
        COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    }
}
