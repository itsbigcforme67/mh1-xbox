/* SLPM_654.95 0x0022F010-0x0022F050: CngNetAQDataSearch .. CngNetAQDataTrans2Work. See cng_nm.c. */
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







































u8 *CngNetAQDataSearch(CNGAQ *q) {
    if (q->rxm.rd < q->rxm.wr) {
        return q->rxm.base + q->rxm.rd;
    }
    return 0;
}

int CngNetAQDataTrans2Work(CNGAQ *q, u8 *w) {
    return CngNetAQdataToObj(w, q);
}
