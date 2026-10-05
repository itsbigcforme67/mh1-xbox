/* SLPM_654.95 0x0022C6A0-0x0022C760: CnInetNetworkCleanup_online .. CngNet_MSG_GetReadSize. See cngmsg_nm.c. */
#include "types.h"

typedef struct CNGMSG {
    s8 x00;
    u8 pad01[3];
    s32 rd;             /* 0x04 */
    u8 *base;           /* 0x08 */
    s32 wr;             /* 0x0C */
    s32 cap;            /* 0x10 */
} CNGMSG;

typedef struct CNGLIB {
    u8 state;           /* 0x00 last status from CnInetNetworkAveTcpPoll */
    u8 on;              /* 0x01 */
    u8 x02;             /* 0x02 */
    u8 pad03[5];
    s32 x08;            /* 0x08 */
} CNGLIB;
extern CNGLIB cng_net_lib_w;

void *memcpy(void *, const void *, int);
int CnInetNetworkAveTcpEnvSet();
int CnInetNetworkAveTcpPoll();
int mcsls_critical_error();

void CngNet_MSG_Clear(CNGMSG *m);
void CngNet_MSG_Write(CNGMSG *m, u8 *src, int n);
void CngNet_MSG_Read(CNGMSG *m, u8 *dst, int n);
u16 CngNetSwapByteU16(u16 v);
void swapb(u8 *buf, int n);




















void CnInetNetworkCleanup_online(void) {
    if (cng_net_lib_w.on != 0) {
        cng_net_lib_w.on = 0;
        cng_net_lib_w.state = 5;
        CnInetNetworkAveTcpEnvSet(5);
    }
}

int CnInetNetworkStatusCheck(void) {
    if (cng_net_lib_w.on == 0) {
        return -1;
    }
    cng_net_lib_w.state = CnInetNetworkAveTcpPoll();
    return cng_net_lib_w.state;
}

void CngNet_MSG_Init(CNGMSG *m, u8 *buf, int cap) {
    m->base = buf;
    m->cap = cap;
    CngNet_MSG_Clear(m);
}

void CngNet_MSG_Clear(CNGMSG *m) {
    m->wr = 0;
    m->rd = 0;
    m->x00 = 0;
}

int CngNet_MSG_GetReadSize(CNGMSG *m) {
    return m->wr - m->rd;
}
