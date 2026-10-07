/* SLPM_654.95 0x0022D2F0-0x0022D4E8: get_AQdata (quest network: pulls received packets into the per-player slots, handles disconnect/error states). See aq_nm.c. The callee's third argument is a u8 in its prototype (changes the argument evaluation order). */
#include "types.h"
#include "game.h"

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
int CngNetAQDataTrans2Work(void *, void *, u8);
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
        q = ((u8 *)&game_w) + 0x208;
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
        q = ((u8 *)&game_w) + 0x208;
        p = q + game_w.master;
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
