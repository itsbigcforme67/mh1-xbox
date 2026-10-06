/* aq_nm - f_aq (SLPM_654.95 0x0022CBA0-, main.bin): AQ network session layer (per-player packet buffers,
 * session start/exit, send/receive pump) as near-match C. Not built; matching runs are built from it. */
#include "types.h"

void *memset(void *, int, int);
void *memcpy(void *, const void *, int);
int sprintf(char *, const char *, ...);

/* aq_work (0x476F70, 0x3C bytes) */
typedef struct AQW {
    u8 mode;            /* 0x00 0 idle, 1 poll only, 2 running, 3 reset */
    u8 x01;
    u8 x02;
    s8 buff;            /* 0x03 CngNetAQBuffCheck result */
    s8 session;         /* 0x04 CngNetAQSessionCheck result: connected */
    s8 host;            /* 0x05 this machine is the session host */
    u8 x06;
    u8 id;              /* 0x07 own connect id */
    u32 timer;          /* 0x08 packet timer (ticks) */
    u32 time;           /* 0x0C net time */
    s32 x10;            /* 0x10 last CngNetAQPacketReceive result */
    s32 ip[4];          /* 0x14 host IP address bytes */
    s32 x24;            /* 0x24 packets queued this frame */
    s32 x28;            /* 0x28 bytes queued this frame */
    s32 x2C;            /* 0x2C wait counter */
    s32 recv_flag;      /* 0x30 */
    s32 x34[3];
} AQW;

/* one received-data slot (aqwork[10], 0x28 bytes) */
typedef struct AQU {
    s32 x00;
    s16 pl;             /* 0x04 */
    s16 x06;
    s32 x08;
    s32 x0C;
    s8 x10;
    u8 x11;
    u8 x12;
    s32 x14;
    s32 x18;
    s32 x1C;
    s8 x20;
    u8 x21;
    u8 x22;
    s32 x24;
} AQU;

extern u8 game_w[];
extern char err_str[];
extern f32 session_time;
int Online_ck();
int CngNetAQPoll();
f32 CngNetAQNetTimeGet();
int CngNetAQBuffCheck();
int CngNetAQSessionCheck();
int CngNetAQSessionInit_online();
int CngNetAQConnectIdGet();
int CngNetAQIsHost();
int CngNetAQPacketSend();
int CngNetAQJoinNumGet();
int CngNetAQSessionExit();
int CngNetAQSessionExit_online();
int CngNetAQInit();
u8 *CngNetAQcommandExec();
int mcsls_get_error_code();
int set01_set2();
int Quest_error_set();
void get_AQdata(void);
u8 *ck_exec_aq();
void host_change(void);
void AQ_session_exit_online(void);
void AQ_recv(int);
void AQ_send(void);
void pl_AQ_put(void);
extern char lit_154_0036C8D0[];
extern char lit_182_0036C8F0[];

extern u8 player_work[];
extern char lit_253_0036C910[];
extern char lit_254_0036C930[];
u8 *CngNetAQDataSearch();
int CngNetAQDataTrans2Work();
int CngReceiveBuffAdjust();
int CngNetAQPacketReceive();
int CngNetAQDisconnectUserIDGet();
int set01_set2_use_mem();
int act_ck();
int Pl_act_set();
int net_receive_pl();
int net_receive_host();
int net_receive_chat();
int net_receive_em();
int net_receive_sys();
void pl_AQ_set(int pl, u8 *d, int flag);
void other_data_ctrl(int pl, u8 *d);
void set_other_data(u8 *d, int flag);
void self_data_ctrl(int pl, u8 *d);
void sync_host_sub(u8 *d, int idx);
void other_item_sub(u8 *d, int idx, int x);
void item_ans_send(int a, int b);
int CngNetAQDataPut();
void pl_data_put();
int send_my_data();

typedef struct SYSX { u8 pad[0x34]; u8 x34; u8 x35; } SYSX;

typedef struct PLPUT {
    s16 pl;             /* 0x00 player the pending block belongs to, -1 none */
    s16 len;            /* 0x02 bytes pending */
    u8 *top;            /* 0x04 */
    u8 *cur;            /* 0x08 */
} PLPUT;

typedef struct AQCFG {
    s32 a[10];
    f32 f;
} AQCFG;

extern AQCFG lit_88_0033B110;
int CnInetNetworkAveTcpConfigSet();
typedef u8 P2[2];
extern P2 adrs_tbl[];
extern u8 netmod_load_flag;
extern u8 cng_netAQ[];
extern u8 xrg_AQ_buff[];
extern u8 xrg_AQ_image[];
extern char lit_100_0036C8B0[];
int CngNetAQBuffInit();
int CngNetAQSendBuffInit();
int CngNetAQHostIPSet();
int CngNetAQDropOutSet();
int CngNetAQSendBuffReset();
void init_AQWork(int n);
void AQ_localwk_clr(void);

extern AQW aq_work;
extern u32 aq_timer[2];
extern PLPUT pl_put_buf;
extern u8 put_buff[];
extern AQU aqwork[10];
extern SYSX system_w;
extern s32 aq_max_size[11];
extern s32 recv_buf_adrs[11];
extern s32 recv_buf_one[11];

void AQ_localwk_clr(void) {
    memset(&aq_work, 0, 0x3C);
    memset(aq_timer, 0, 8);
    pl_put_buf.len = 0;
    pl_put_buf.cur = put_buff;
    pl_put_buf.top = put_buff;
    pl_put_buf.pl = -1;
}

void init_AQWork(int n) {
    AQU *p;

    if (n != 0) {
        p = &aqwork[n - 1];
        p->x00 = 0;
        p->pl = n;
        p->x06 = 0;
        p->x08 = 0;
        p->x0C = 0;
        p->x10 = 0;
        p->x11 = 0xE6;
        p->x12 = aq_max_size[n];
        p->x14 = recv_buf_adrs[n];
        p->x18 = 0;
        p->x1C = 0;
        p->x20 = 0;
        p->x21 = 0xE6;
        p->x22 = aq_max_size[n];
        p->x24 = p->x14 + recv_buf_one[n];
    }
}

void AQ_init(int mode) {
    P2 *a;
    char ip[32];
    AQCFG cfg;

    cfg = lit_88_0033B110;
    CnInetNetworkAveTcpConfigSet(&cfg);
    AQ_localwk_clr();
    CngNetAQBuffInit(cng_netAQ, xrg_AQ_buff, 0x4000, 0x80);
    CngNetAQSendBuffInit(cng_netAQ, xrg_AQ_image, 0x200);
    if (mode != 0) {
        a = adrs_tbl + mode * 2;
        aq_work.ip[0] = (*a)[0];
        aq_work.ip[1] = (*a)[1];
        a++;
        aq_work.ip[2] = (*a)[0];
        aq_work.ip[3] = (*a)[1];
        sprintf(ip, lit_100_0036C8B0, aq_work.ip[0], aq_work.ip[1], aq_work.ip[2], aq_work.ip[3]);
        CngNetAQHostIPSet(cng_netAQ, ip);
    }
    CngNetAQDropOutSet(cng_netAQ, 0xFFFF);
    CngNetAQSendBuffReset(cng_netAQ);
    mode = 0;
    do {
        init_AQWork(mode);
        mode++;
    } while (mode < 0xB);
    netmod_load_flag = 1;
    system_w.x34 = 1;
}

void AQ_exec(void) {
    AQW *w;
    u8 *p;
    u8 *q;
    int v;

    if (system_w.x35 == 0) {
        w = &aq_work;
        game_w[0x21C] = game_w[0x21B];
        switch (aq_work.mode) {
        case 0:
            break;
        case 1:
            CngNetAQPoll(cng_netAQ);
            break;
        case 2:
            if (aq_work.session != 0) {
                session_time = CngNetAQNetTimeGet(cng_netAQ);
                w->time = (u32)session_time;
                w->timer++;
                CngNetAQPoll(cng_netAQ);
                w->buff = CngNetAQBuffCheck(cng_netAQ);
                w->session = CngNetAQSessionCheck(cng_netAQ);
                w->x24 = 0;
                w->x28 = 0;
                get_AQdata();
                if (aq_work.session == 0) {
                    w->mode++;
                    q = game_w + 0x208;
                    p = q + game_w[0xD1];
                    v = *p;
                    if (v != 0xFF) {
                        *p = 0xFF;
                        sprintf(err_str, lit_154_0036C8D0, mcsls_get_error_code(v));
                        set01_set2(err_str);
                        Quest_error_set();
                    }
                    return;
                }
            }
            if (system_w.x35 == 0) {
                if (w->x2C > 0) {
                    w->x2C--;
                    break;
                }
                AQ_recv(1);
                AQ_recv(0);
                if (w->timer < aq_timer[0]) {
                    w->timer = aq_timer[0];
                }
                if (w->timer < aq_timer[1]) {
                    w->timer = aq_timer[1];
                }
            }
            break;
        case 3:
            AQ_localwk_clr();
            break;
        }
    }
}

void AQ_exec_load(void) {
    AQW *w;
    u8 *p;
    u8 *q;
    int v;

    if (Online_ck() != 0 && system_w.x34 != 0) {
        w = &aq_work;
        game_w[0x21C] = game_w[0x21B];
        switch (aq_work.mode) {
        case 0:
            break;
        case 1:
            CngNetAQPoll(cng_netAQ);
            break;
        case 2:
            if (aq_work.session != 0) {
                session_time = CngNetAQNetTimeGet(cng_netAQ);
                w->time = (u32)session_time;
                w->timer++;
                CngNetAQPoll(cng_netAQ);
                w->buff = CngNetAQBuffCheck(cng_netAQ);
                w->session = CngNetAQSessionCheck(cng_netAQ);
                w->x24 = 0;
                w->x28 = 0;
                get_AQdata();
                if (aq_work.session == 0) {
                    w->mode++;
                    q = game_w + 0x208;
                    p = q + game_w[0xD1];
                    v = *p;
                    if (v != 0xFF) {
                        *p = 0xFF;
                        sprintf(err_str, lit_182_0036C8F0, mcsls_get_error_code(v));
                        set01_set2(err_str);
                        Quest_error_set();
                    }
                    return;
                }
            }
            break;
        }
        AQ_send();
    }
}

void AQSession_init_online(void) {
    CngNetAQSessionInit_online(cng_netAQ);
    aq_work.mode++;
}

int AQSession_wait(void) {
    int r = 0;
    AQW *w = &aq_work;

    aq_work.session = CngNetAQSessionCheck(cng_netAQ);
    if (aq_work.session != 0) {
        w->mode++;
        w->id = CngNetAQConnectIdGet(cng_netAQ);
        w->host = CngNetAQIsHost(cng_netAQ);
        r = 1;
        game_w[0xD1] = w->id;
    }
    return r;
}

u8 *ck_exec_aq() {
    return CngNetAQcommandExec();
}

void AQ_send(void) {
    if (aq_work.session != 0) {
        pl_AQ_put();
        CngNetAQPacketSend(cng_netAQ, 1);
    }
}

void AQ_recv_flag_set(int v) {
    aq_work.recv_flag = v;
}

int AQ_join_num_get(void) {
    return CngNetAQJoinNumGet(cng_netAQ) & 0xFF;
}

void AQ_session_exit(void) {
    CngNetAQSessionExit(cng_netAQ);
    system_w.x34 = 0;
}

void AQ_session_exit_online(void) {
    CngNetAQSessionExit_online(cng_netAQ);
    system_w.x34 = 0;
    CngNetAQInit(cng_netAQ);
}

void get_AQdata(void) {
    u8 *d;
    u8 *p;
    int id;
    u8 *pw;
    u8 *q;
    AQU *u;
    int idx;
    int v;

    aq_work.x10 = CngNetAQPacketReceive(cng_netAQ);
    switch (aq_work.x10) {
    case 1:
    case 0x80:
        while ((d = CngNetAQDataSearch(cng_netAQ)) != 0) {
            id = *(u16 *)d;
            if (id > 0 && id < 0xB) {
                idx = id - 1;
                u = &aqwork[idx];
                if (CngNetAQDataTrans2Work(cng_netAQ, u, d[0xB]) == 0) {
                    CngReceiveBuffAdjust(cng_netAQ);
                    break;
                }
            }
        }
        break;
    case 0x20:
        break;
    case 4:
        id = CngNetAQDisconnectUserIDGet(cng_netAQ);
        q = game_w + 0x208;
        p = q + id;
        if (*p != 0xFF) {
            *p = 0xFF;
            pw = player_work + id * 0xA00;
            sprintf(err_str, lit_253_0036C910, pw + 0x8D4);
            set01_set2_use_mem(err_str);
            if ((s16)act_ck(pw, 4, 5) == 0) {
                Pl_act_set(pw, 4, 5, 2);
            }
            host_change();
        }
        break;
    case 0x40:
        q = game_w + 0x208;
        p = q + game_w[0xD1];
        v = *p;
        if (v != 0xFF) {
            *p = 0xFF;
            sprintf(err_str, lit_254_0036C930, mcsls_get_error_code(v));
            set01_set2(err_str);
            AQ_session_exit_online();
            Quest_error_set();
        }
        break;
    }
}

void AQ_recv(int kind) {
    AQU *u = aqwork;
    int i = 1;
    u8 *d;
    AQW *w = &aq_work;

    if (kind == i) {
        do {
            d = ck_exec_aq(u, 0x80, -1);
            if (d != 0 && *(u16 *)(d + 4) > 0 && *(u16 *)(d + 4) < 0xB) {
                aq_timer[1] = *(u32 *)d;
                if (aq_timer[1] < *(u32 *)d) {
                    aq_timer[1] = *(u32 *)d;
                }
                other_data_ctrl(i, d);
            }
            i++;
            u++;
        } while (i < 0xB);
        return;
    }
    do {
        d = ck_exec_aq(u, 0, -1);
        if (d != 0 && *(u16 *)(d + 4) > 0 && *(u16 *)(d + 4) < 0xB) {
            aq_timer[0] = *(u32 *)d;
            if (aq_timer[0] < *(u32 *)d) {
                aq_timer[0] = *(u32 *)d;
            }
            self_data_ctrl(i, d + 8);
        }
        i++;
        u++;
    } while (i < 0xB);
    if (w->timer < aq_timer[1]) {
        w->timer = aq_timer[1];
    }
    if (w->timer < aq_timer[0]) {
        w->timer = aq_timer[0];
    }
}

void self_data_ctrl(int pl, u8 *d) {
    if (pl >= 0 && (pl == 7 || d[2] != game_w[0xD1])) {
        switch (pl) {
        case 1:
        case 2:
        case 3:
        case 4:
            pl_AQ_set(pl, d, 0);
            break;
        case 7:
            net_receive_host(pl, d, 0);
            break;
        case 6:
            net_receive_chat(pl, d, 0);
            break;
        case 8:
            net_receive_em(pl, d, 0);
            break;
        case 10:
            net_receive_sys(pl, d, 0);
            break;
        }
    }
}

void other_data_ctrl(int pl, u8 *d) {
    switch (pl) {
    case 1:
    case 2:
    case 3:
    case 4:
        set_other_data(d, 0);
        break;
    case 0:
    case 5:
    case 6:
    case 7:
    case 8:
        break;
    }
}

typedef struct OTHBUF {
    s32 a;
    s32 b;
    u8 data[0x28];
} OTHBUF;

void set_other_data(u8 *d, int flag) {
    OTHBUF buf;
    OTHBUF *b = &buf;
    u8 *src = d + 8;
    u16 pl = *(u16 *)(d + 4);
    int n;

    b->a = 1;
    b->b = *(s32 *)d;
    n = d[9];
    if (n >= 0x21) {
        n = 0x20;
    }
    memcpy(b->data, src, n);
    if (flag == 0) {
        sync_host_sub((u8 *)b, pl - 1);
    }
}

void sync_host_sub(u8 *d, int idx) {
    u8 k;

    if (*(s32 *)d != 0) {
        k = d[0xB];
        switch (k) {
        case 0:
            if (aq_work.host != 0) {
                other_item_sub(d, idx, 0);
            }
            break;
        case 1:
            break;
        case 2:
            break;
        }
    }
}

typedef struct AQPKT {
    u16 pl;             /* 0x00 */
    s8 kind;            /* 0x02 */
    u8 len;             /* 0x03 */
    s32 time;           /* 0x04 */
    u16 id;             /* 0x08 */
    u8 x0A;
    u8 x0B;
    u8 data[0xF4];
} AQPKT;

int send_my_data(pl, d, kind)
int pl;
u8 *d;
int kind;
{
    AQPKT pkt;
    AQPKT *p;
    u32 t;

    if (pl <= 0 || pl >= 0xB) {
        return -1;
    }
    {
        if (d[1] >= 0x80) {
            return -2;
        }
        t = aq_work.timer;
        p = &pkt;
        if (d[0] & 0x80) {
            p->kind = 1;
        } else {
            p->kind = 4;
        }
        p->len = d[1];
        p->time = t;
        p->x0A = d[0];
        d[0] = d[0] & 0x7F;
        switch (kind) {
        case 0:
            p->pl = pl;
            p->id = p->pl;
            p->x0B = 0;
            break;
        case 1:
            p->pl = 7;
            p->id = game_w[0xD1] + 1;
            p->x0B = 0x80;
            break;
        }
        memcpy(p->data, d, p->len);
        aq_work.x24++;
        aq_work.x28 += p->len + 0x10;
        CngNetAQDataPut(cng_netAQ, p);
        return 0;
    }
}

int AQ_data_put(int pl, u8 *d, int mode) {
    AQW *w = &aq_work;
    int v;

    if (aq_work.session == 0) {
        return -1;
    }
    if (w->timer == 0 && pl != 6 && pl != 7) {
        return -2;
    }
    if (pl < 0) {
        return -3;
    }
    if (pl >= 0xB) {
        return -4;
    }
    d[2] = game_w[0xD1];
    if (pl > 0 && pl < 5 && mode == 0) {
        pl_data_put();
        return 1;
    }
    if (w->buff < 2) {
        return -5;
    }
    v = w->x28;
    if (pl_put_buf.len != 0) {
        v += pl_put_buf.len + 0x10;
    }
    if (v >= 0x200) {
        return -6;
    }
    return ((s8)send_my_data(pl, d, mode) != 0) ? -7 : 0;
}

void pl_data_put(int pl, u8 *d) {
    int len = d[1];
    PLPUT *pb = &pl_put_buf;
    int r;
    int v;
    int t;
    s16 l;

    if (len != 0) {
        r = (len + 3) / 4 * 4;
        if (pb->pl != -1 && pb->pl != pl) {
            send_my_data(pl, d);
        }
        v = aq_work.x28;
        l = pb->len;
        if (l != 0) {
            v += l + 0x10;
        }
        if (v < 0x200) {
            pb->pl = pl;
            t = pb->len + r;
            if (t < 0xEC) {
                pb->len = t;
                memcpy(pb->cur, d, len);
                pb->cur += r;
            }
        }
    }
}

void pl_AQ_put(void) {
    u8 buf[0xF0];
    u8 *q;
    PLPUT *pb = &pl_put_buf;

    if (pl_put_buf.pl != -1 && pb->len != 0 && aq_work.buff >= 2) {
        buf[0] = *pb->top;
        buf[1] = (u8)pb->len + 4;
        q = buf + 2;
        q[0] = game_w[0xD1];
        q[1] = 0;
        *pb->top = *pb->top & 0x7F;
        memcpy(q + 2, pb->top, pb->len);
        send_my_data(pb->pl, buf, 0);
    }
    pb->cur = pb->top;
    pb->len = 0;
    pb->pl = -1;
}

void pl_AQ_set(int pl, u8 *d, int flag) {
    int rest = d[1] - 4;
    u8 *p = d + 4;
    int n;

    if (rest > 0) {
        do {
            n = p[1];
            net_receive_pl(pl, p, 0);
            n = (n + 3) / 4 * 4;
            rest -= n;
            p += n;
        } while (rest > 0);
    }
}

void item_ans_send(int a, int b) {
    struct { u8 a, b, c, d, e, f; } buf;

    buf.b = 6;
    buf.f = b;
    buf.e = a + 1;
    buf.a = 0;
    buf.d = 0;
    buf.c = game_w[0xD1];
    send_my_data(7, (u8 *)&buf, 0);
}

void other_item_sub(u8 *d, int idx, int x) {
    switch (d[8]) {
    case 1:
        item_ans_send(idx, 2);
        break;
    case 2:
        break;
    }
}

void host_change(void) {
    s16 i;
    u8 *p;
    u8 *pl;
    u8 *base;

    i = 0;
    if (0 < game_w[0xD3]) {
        p = game_w;
        do {
            if (p[0x208] == 1) {
                game_w[0x21B] = i;
                base = player_work + 0x91F;
                pl = base + game_w[0xD1] * 0xA00;
                if (*pl != 0) {
                    *pl = 0;
                }
                return;
            }
            i++;
            p++;
        } while (i < game_w[0xD3]);
    }
}
