/* mcsls_i01 - mcsls_init (SLPM_654.95 0x00230760-0x0023098C): clears the session layer work area, stores the socket, own player number and player
   count, marks every player alive, empties the TCP/UDP/app/per-player byte queues, starts the in-net session and the clock. Written new in this
   pass from an m2c draft (field names from include/mcsls.h). */
#include "types.h"
#include "mcsls.h"

extern u8 _tcp_recv_buff[], _tcp_send_buff[], _udp_send_buff[], _rudp_send_buff[], _app_send_que[], _app_recv_que[], _mcs_recv_que[];
extern CNMSG udp_send_buff;
extern CNMSG rudp_send_buff;
void *memset();
void tmplCCnNetMsg_CnClear();
void CnInetMcsInitialize();
f32 CngNetTimeGet();

void mcsls_init(int sock, s8 me, u8 num) {
    int i;
    f32 t;
    MCSLS *m;
    u8 *b;
    CNMSG *q;
    int j;

    memset(&mcsls_w, 0, 0x16C);
    mcsls_w.me = me;
    i = 0;
    mcsls_w.sock = sock;
    mcsls_w.num = num;
    mcsls_w.alive = num;
    mcsls_w.x151 = 1;
    mcsls_w.x150 = 1;
    send_health_ans = 0;
    if (0 < mcsls_w.num) {
        do {
            mcsls_w.pl[i].alive = 1;
            i++;
        } while (i < mcsls_w.num);
    }
    tmplCCnNetMsg_CnClear(&tcp_recv_buff, _tcp_recv_buff, 0x800);
    tmplCCnNetMsg_CnClear(&tcp_send_buff, _tcp_send_buff, 0x800);
    tmplCCnNetMsg_CnClear(&udp_send_buff, _udp_send_buff, 0x400);
    tmplCCnNetMsg_CnClear(&rudp_send_buff, _rudp_send_buff, 0x400);
    tmplCCnNetMsg_CnClear(&app_send_que, _app_send_que, 0x800);
    tmplCCnNetMsg_CnClear(&app_recv_que, _app_recv_que, 0x4000);
    j = 0;
    if (0 < mcsls_w.num) {
        b = _mcs_recv_que;
        q = mcs_recv_que;
        do {
            tmplCCnNetMsg_CnClear(q, b, 0x1000);
            j++;
            b += 0x1000;
            q++;
        } while (j < mcsls_w.num);
    }
    CnInetMcsInitialize(sock);
    t = CngNetTimeGet();
    mcsls_w.t_prev = t;
    mcsls_w.time = t;
    i = 0;
    if (0 < mcsls_w.num) {
        m = &mcsls_w;
        do {
            i++;
            *(f32 *)((u8 *)m + 0x54) = mcsls_w.time;
            m = (MCSLS *)((u8 *)m + 0x3C);
        } while (i < mcsls_w.num);
    }
    *(u32 *)&mcsls_w.f148 = 0x3ADA740E;
}
