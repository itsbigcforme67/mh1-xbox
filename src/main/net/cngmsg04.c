/* SLPM_654.95 0x0022CB50-0x0022CB78: CngNetSwapByteU16 .. CngNetSwapByteU16. See cngmsg_nm.c. */
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




















u16 CngNetSwapByteU16(u16 v) {
    u16 b = v;

    swapb((u8 *)&b, 2);
    return b;
}
