/* cngmsg_nm - Cng message ring buffers and byte swap (SLPM_654.95 0x0022C670-0x0022CB80, main.bin):
 * CnInetNetwork* state, CngNet_MSG_* (ring: write offset wr, read offset rd, capacity cap), swapb. Near-match C. */
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

void CnInetNetworkInitialize_online(void) {
    cng_net_lib_w.x02 = 1;
    cng_net_lib_w.on = 1;
    cng_net_lib_w.x08 = -1;
    cng_net_lib_w.state = 2;
    CnInetNetworkAveTcpEnvSet(2);
}

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

void CngNet_MSG_Write(CNGMSG *m, u8 *src, int n) {
    int wr = m->wr;

    if (m->cap < wr + n) {
        mcsls_critical_error(4);
    } else {
        memcpy(m->base + wr, src, n);
        m->wr = m->wr + n;
    }
}

void CngNet_MSG_Read(CNGMSG *m, u8 *dst, int n) {
    if (m->wr < m->rd + n) {
        mcsls_critical_error(4);
    }
    memcpy(dst, m->base + m->rd, n);
    m->rd = m->rd + n;
}

void CngNet_MSG_ReadTop(CNGMSG *m) {
    m->rd = 0;
}

void CngNet_MSG_WriteU8(CNGMSG *m, u8 v) {
    u8 b = v;

    CngNet_MSG_Write(m, &b, 1);
}

u8 CngNet_MSG_ReadU8(CNGMSG *m) {
    u8 b;

    CngNet_MSG_Read(m, &b, 1);
    return b;
}

u32 CngNet_MSG_ReadU32(CNGMSG *m) {
    u32 v;

    CngNet_MSG_Read(m, (u8 *)&v, 4);
    return v;
}

void CngNet_MSG_WriteU16(CNGMSG *m, u16 v) {
    u16 b = v;

    CngNet_MSG_Write(m, (u8 *)&b, 2);
}

u16 CngNet_MSG_ReadU16(CNGMSG *m) {
    u16 v;

    CngNet_MSG_Read(m, (u8 *)&v, 2);
    return v;
}

void CngNet_MSG_WriteFloat32(CNGMSG *m, f32 v) {
    CngNet_MSG_Write(m, (u8 *)&v, 4);
}

f32 CngNet_MSG_ReadFloat32(CNGMSG *m) {
    f32 v;
    f32 r;

    CngNet_MSG_Read(m, (u8 *)&v, 4);
    r = v;
    return r;
}

u16 CngNet_MSGBOB_ReadU16(CNGMSG *m) {
    u16 v;

    CngNet_MSG_Read(m, (u8 *)&v, 2);
    v = CngNetSwapByteU16(v);
    return v;
}

void swapb(u8 *buf, int n) {
    int i = 0;
    int h = n / 2;
    u8 t;

    for (; i < h; i++) {
        t = buf[i];
        buf[i] = buf[n - 1 - i];
        buf[n - 1 - i] = t;
    }
}

u16 CngNetSwapByteU16(u16 v) {
    u16 b = v;

    swapb((u8 *)&b, 2);
    return b;
}
