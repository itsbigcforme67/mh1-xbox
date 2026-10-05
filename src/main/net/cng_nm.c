/* cng_nm - Cng* network game layer (SLPM_654.95 0x0022DF28-0x0022F2D0, main.bin): P2P session (mcsls) glue and
 * the AQ packet queue (cng_netAQ, 0x1F0 bytes) used by f_aq. Near-match C, not built. */
#include "types.h"

/* message ring (CngNet_MSG_*, 0x14 bytes) */
typedef struct CNGMSG {
    s32 x00;
    s32 rd;             /* 0x04 read offset */
    u8 *base;           /* 0x08 */
    s32 wr;             /* 0x0C write offset */
    s32 x10;
} CNGMSG;

typedef struct CNGP2P {
    s32 state;          /* 0x00 0 idle .. 3 running, 5 error */
    CNGMSG rxm;         /* 0x04 */
    CNGMSG txm;         /* 0x18 */
    CNGMSG usrm;        /* 0x2C */
    s32 id;             /* 0x40 own connect id */
    s8 host;            /* 0x44 */
    u8 pad45[3];
    s32 count;          /* 0x48 */
    s32 ids[8];         /* 0x4C */
    f32 t0;             /* 0x6C */
    f32 t1;             /* 0x70 */
    f32 t2;             /* 0x74 */
    u8 pad78[4];
    char ip[0x100];     /* 0x7C host ip */
    s32 x17C;
} CNGP2P;

/* cng_netAQ (0x4EF1D0, 0x1F0 bytes): +0x00 receive ring, +0x14 send ring, +0x28 P2P session (mcsls) */
typedef struct CNGAQ {
    CNGMSG rxm;         /* 0x00 */
    CNGMSG txm;         /* 0x14 */
    CNGP2P p2p;         /* 0x28 */
    u8 *rbuf;           /* 0x1A8 receive packet buffer */
    u8 *rcur;           /* 0x1AC next free slot (0 = none) */
    u16 rsize;          /* 0x1B0 total size */
    u8 pad1B2[2];
    u16 esize;          /* 0x1B4 slot size */
    u8 *sbuf;           /* 0x1B8 send image buffer */
    u8 *scur;           /* 0x1BC */
    u16 ssize;          /* 0x1C0 */
    u8 pad1C2[2];
    u8 cnt;             /* 0x1C4 packets queued */
    u8 pad1C5[3];
    s32 bytes;          /* 0x1C8 bytes queued */
    s16 x1CC;           /* 0x1CC */
    u8 x1CE;            /* 0x1CE disconnect user id */
    u8 pad1CF[0x1E0 - 0x1CF];
    s16 x1E0;           /* 0x1E0 */
    u8 pad1E2[6];
    u16 drop;           /* 0x1E8 drop-out count */
    u8 pad1EA[6];
} CNGAQ;

typedef struct TCPST {
    s16 state;
    u8 pad02[2];
    u16 a;
    u16 b;
} TCPST;
typedef struct F8 { f32 f[11]; } F11;
extern F11 ave_cnf;
extern s8 InetGame[0x14];
extern u8 ConnWork[];
extern u8 aRecvBuff[];
extern u8 aSendBuff[];
extern u8 aRecvUserBuff[];
extern s32 CurDevice;
extern s32 UsbDeviceNum;
extern s32 TotalDeviceNum;
extern s8 LoadedDeviceType;
extern s8 SrvType;
extern u8 SrvDomain[0x100];
extern s32 SrvIPAddress;
extern s16 SrvPort;
extern s8 MyEtherInitMode;
extern s8 MyDialType;
extern u8 MyDialNumber[0xF0];
extern u8 MyDialOutline[0x10];
extern u8 MyDomain[0x100];
extern s32 MyDns1;
extern s32 MyDns2;
extern u8 MyUserName[0x100];
extern u8 MyPassword[0x100];
extern s32 MySnapHandle;
extern s8 NdgNegoMode;
extern s8 PppRecognize;
extern s32 MyIPAddr;
extern s32 MyNetmask;
extern s32 MyGateway;
extern s32 MySnapPerformance;
extern u8 MySnapRegWeb[0x100];
extern u8 DeviceWork[0x3D4];
extern s32 _local_ip_address;
int CpInetTcpGetStatus();
int mcsls_preinit();
int mcsls_move();
int mcsls_critical_error();
int mcsls_get_execute_state();
int mcsls_app_push_is_ready();
int mcsls_app_push();
int mcsls_app_pull();
int mcsls_app_purge();
int CnInetNetworkStatusCheck();
int InetConnectStart();
int CngNet_MSG_Clear();
int CngNet_MSG_Write();
int mcsls_init();
int mcsls_run_game_move();
f32 CngNetTimeGet();
int CnInetNetworkInitialize_online();
extern s8 ave_env[0x10];
extern u8 aq_receive_buff[];
extern u8 aq_send_buff[];

void *memset(void *, int, int);
char *strcpy(char *, const char *);
int CngNet_MSG_Init();
void CngNetMcsP2PInit(CNGP2P *p);
int CngNetAQdataToObj();
int CngNetAQSessionWait();
int DeviceModuleInitialize_blocking();
int CnInetNetworkCleanup_online();
void CngSessionStart_online(CNGP2P *p);

s8 CngIsHost(CNGP2P *s);
int CngGetConnectID(CNGP2P *s);
u8 *CngGetSessionInfo(CNGP2P *s);
void CngHostIPSet(CNGP2P *s, char *ip);
f32 CngSessionTimeGet(CNGP2P *s);
int CngNetAQSessionWait();
int CngNetAQdataToObj();

void CpInetSetLocalIpAddr(int a) {
    _local_ip_address = a;
}

void CnInetNetworkAveTcpEnvSet(s8 v) {
    ave_env[0] = v;
}

int CpInetTcpSelect2(int sock, int kind) {
    TCPST st;
    int r;

    r = CpInetTcpGetStatus(sock, &st);
    if (r == 0) {
        switch (kind) {
        case 0:
            if (st.state == 4) {
                r = 1;
            }
            break;
        case 1:
            break;
        case 2:
            if (st.state != 4) {
                r = -6;
            } else if (st.b != 0) {
                r = 1;
            }
            break;
        case 3:
            if (st.state != 4) {
                r = -6;
            } else if (st.a != 0) {
                r = 1;
            }
            break;
        }
    }
    return r;
}

void CnInetNetworkAveTcpConfigSet(F11 *c) {
    ave_cnf = *c;
}

void CngNetPS2ModuleBootInitialize(void) {
    DeviceModuleInitialize_blocking();
}

void InetSramInitialize(void) {
    CurDevice = 0;
    memset(DeviceWork, 0xFF, 0x3D4);
    UsbDeviceNum = -1;
    TotalDeviceNum = -1;
    LoadedDeviceType = 0;
    SrvType = 0;
    memset(SrvDomain, 0, 0x100);
    SrvIPAddress = 0;
    SrvPort = 0;
    MyEtherInitMode = 0;
    MyDialType = 0;
    memset(MyDialNumber, 0, 0xF0);
    memset(MyDialOutline, 0, 0x10);
    memset(MyDomain, 0, 0x100);
    MyDns1 = 0;
    MyDns2 = 0;
    memset(MyUserName, 0, 0x100);
    memset(MyPassword, 0, 0x100);
    MySnapHandle = -1;
    NdgNegoMode = 0;
    PppRecognize = 0;
    MyIPAddr = 0;
    MyNetmask = 0;
    MyGateway = 0;
    MySnapPerformance = 0;
    memset(MySnapRegWeb, 0, 0x100);
}

void CngNetMcsP2PInit(CNGP2P *p) {
    mcsls_preinit();
    CngNet_MSG_Init(&p->rxm, aRecvBuff, 0x2000);
    CngNet_MSG_Init(&p->txm, aSendBuff, 0x2000);
    CngNet_MSG_Init(&p->usrm, aRecvUserBuff, 0x2000);
    p->state = 0;
    p->id = -1;
    p->ip[0] = 0;
    p->x17C = 0;
}

void CngSessionStart_online(CNGP2P *p) {
    int i;

    p->t0 = CngNetTimeGet();
    p->t1 = CngNetTimeGet();
    p->t2 = CngNetTimeGet();
    CnInetNetworkInitialize_online();
    p->count = InetGame[0];
    p->id = InetGame[1];
    i = 0;
    if (0 < InetGame[0]) {
        u8 *q = (u8 *)p;
        do {
            *(s32 *)(q + 0x4C) = i;
            i++;
            q += 4;
        } while (i < InetGame[0]);
    }
    p->host = (InetGame[1] == 0) ? 1 : 0;
    p->t0 = CngNetTimeGet();
    p->t1 = 0;
    mcsls_init(*(s32 *)(ConnWork + 4), InetGame[1], InetGame[0]);
    mcsls_run_game_move();
    p->state = 3;
}

void CngNetMcsP2PPoll(CNGP2P *p) {
    f32 t;
    int i;
    int r;
    int st;

    t = CngNetTimeGet();
    t = t - p->t0;
    p->t2 = t - p->t1;
    p->t1 = t;
    st = CnInetNetworkStatusCheck();
    if (st == 2) {
        switch (p->state) {
        case 1:
            p->state = 2;
            break;
        case 2:
            r = InetConnectStart(p->state, 2);
            if (r == 3) {
                p->count = InetGame[0];
                p->id = InetGame[1];
                i = 0;
                if (0 < InetGame[0]) {
                    u8 *q = (u8 *)p;
                    do {
                        *(s32 *)(q + 0x4C) = i;
                        i++;
                        q += 4;
                    } while (i < InetGame[0]);
                }
                p->host = (InetGame[1] != 0) ? 0 : 1;
                p->t0 = CngNetTimeGet();
                p->t1 = 0;
                mcsls_init(*(s32 *)(ConnWork + 4), InetGame[1], InetGame[0]);
                mcsls_run_game_move();
                p->state = 3;
            } else if (r == 4) {
                p->state = 5;
            }
            break;
        case 3:
            if (mcsls_move(p->state, 2) != 0) {
                p->state = 5;
            }
            break;
        case 4:
        case 5:
            break;
        }
    } else {
        switch (st) {
        case 4:
            p->state = 5;
            mcsls_critical_error(5, 2);
            break;
        }
    }
}

int CngGetTrafficLevel(s32 *s) {
    if (*s == 3 && mcsls_get_execute_state() != 1 && mcsls_app_push_is_ready(0x2BC) != 0) {
        return 1;
    }
    return 3;
}

int CngSendMsg(s32 *s, int a1, u8 *msg) {
    if (*s != 3) {
        return 0;
    }
    mcsls_app_push(*(s32 *)(msg + 8), *(s32 *)(msg + 0xC));
    return 1;
}

void CngExitSession() {
    /* empty */
}

void CngExitSession_online() {
    CnInetNetworkCleanup_online();
}

int CngSessionStatGet(s32 *s) {
    return *s;
}

s8 CngIsHost(CNGP2P *s) {
    return s->host;
}

int CngGetConnectID(CNGP2P *s) {
    return s->id;
}

u8 *CngGetSessionInfo(CNGP2P *s) {
    return (u8 *)&s->count;
}

void CngHostIPSet(CNGP2P *s, char *ip) {
    strcpy(s->ip, ip);
}

f32 CngSessionTimeGet(CNGP2P *s) {
    return s->t1;
}

void CngNetAQInit(CNGAQ *q) {
    q->x1CC = 0;
    CngNet_MSG_Init(q, aq_receive_buff, 0x2000);
    CngNet_MSG_Init(&q->txm, aq_send_buff, 0x2000);
    CngNetMcsP2PInit(&q->p2p);
}

void CngNetAQSendBuffReset(CNGAQ *q) {
    if (q->rcur == 0) {
        q->rcur = q->rbuf;
    }
    q->scur = q->sbuf;
}

void CngNetAQSessionInit_online(CNGAQ *q) {
    CngSessionStart_online(&q->p2p);
}

void CngNetAQSessionExit(CNGAQ *q) {
    CngExitSession(&q->p2p);
}

void CngNetAQSessionExit_online(CNGAQ *q) {
    CngExitSession_online(&q->p2p);
}

int CngNetAQSessionCheck() {
    return CngNetAQSessionWait();
}

u8 *CngNetAQDataSearch(CNGAQ *q) {
    if (q->rxm.rd < q->rxm.wr) {
        return q->rxm.base + q->rxm.rd;
    }
    return 0;
}

int CngNetAQDataTrans2Work(CNGAQ *q, u8 *w) {
    return CngNetAQdataToObj(w, q);
}

s8 CngNetAQIsHost(CNGAQ *q) {
    return CngIsHost(&q->p2p);
}

int CngNetAQConnectIdGet(CNGAQ *q) {
    return CngGetConnectID(&q->p2p) & 0xFF;
}

u8 CngNetAQJoinNumGet(CNGAQ *q) {
    return *CngGetSessionInfo(&q->p2p);
}

u8 CngNetAQBuffCheck(CNGAQ *q) {
    int v = (q->rbuf + q->rsize - q->rcur) * 5;

    return v / q->rsize;
}

void CngNetAQBuffInit(CNGAQ *q, u8 *buf, int size, u8 esize) {
    q->rbuf = buf;
    q->rcur = buf;
    q->rsize = size;
    q->esize = esize;
    memset(q->rbuf, 0, q->rsize);
}

void CngNetAQSendBuffInit(CNGAQ *q, u8 *buf, int size) {
    q->sbuf = buf;
    q->scur = buf;
    q->ssize = size;
    memset(q->sbuf, 0, q->ssize);
}

void CngNetAQHostIPSet(CNGAQ *q, char *ip) {
    if (ip != 0) {
        CngHostIPSet(&q->p2p, ip);
    }
}

void CngNetAQDropOutSet(CNGAQ *q, int n) {
    q->drop = n;
    q->x1E0 = 0;
}

f32 CngNetAQNetTimeGet(CNGAQ *q) {
    return 1000.0f * CngSessionTimeGet(&q->p2p);
}

u8 CngNetAQDisconnectUserIDGet(CNGAQ *q) {
    return q->x1CE;
}

int CngNetAQBuffEmptyCheck(CNGAQ *q, u8 mode) {
    u8 *p;
    int ret;

    p = (u8 *)q + 8;
    if ((mode & 0xFF) & 0x80) {
        p = (u8 *)q + 0x18;
    }
    if (p[8] < p[9]) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
