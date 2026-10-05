/* AQ session layer (0x0022CD60-0x0022D2E8): init_AQWork, AQ_exec, AQ_exec_load, session init/wait, ck_exec_aq. See aq_nm.c. */
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


















typedef struct OTHBUF {
    s32 a;
    s32 b;
    u8 data[0x28];
} OTHBUF;



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
