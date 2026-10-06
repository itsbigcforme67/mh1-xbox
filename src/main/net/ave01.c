/* ave01 - SLPM_654.95 0x00233B10-0x00234260 (f_ave.s, first part): "Ave" layer, thin wrappers that pack arguments into the
   1 KB SifRpcWork buffer and call the IOP network module through sceSifCallRpc (one RPC number per call, results
   come back as s16 in the same buffer). Every call is bracketed by the RPC semaphore. Working file. */
#include "types.h"

typedef struct SEMAP { int cur, max, init, wait, attr, opt; } SEMAP;

extern int Inet_rpc_sema[4];
extern s16 Inet_rpc_non_blocking_call_result[8];
extern s16 Inet_rpc_non_blocking_call_enable[8];
extern u8 SifRpcWork[0x400];
extern struct { u8 x00[0x24]; int serve; } Ave_wrap_sif;

void *memset();
void *memcpy();
int CreateSema();
int sceSifBindRpc();
int sceSifCallRpc();
int sceSifCheckStatRpc();
int WaitSema();
int SignalSema();
void FlushCache();
char *strcpy();
char *strncpy();

s16 Ave_SifCallRpc();
s16 Ave_SifCallRpcNb();
int Ave_SifCallRpcEnd();
int Ave_SifCallRpcSemaWait();
int Ave_SifCallRpcSemaSignal();

#define W16(o) (*(s16 *)(SifRpcWork + (o)))
#define W32(o) (*(s32 *)(SifRpcWork + (o)))

typedef struct PPPST { f32 v[11]; } PPPST;
typedef struct PPPCFG {
    u8 x00;
    u8 x01;
    s16 x02;
    char *s04;
    char *s08;
    char *s0C;
    char *s10;
    char *s14;
    char *s18;
    s16 x1C;
    s16 x1E;
    s16 x20;
    s16 x22;
    s16 x24;
    u8 x26;
    u8 x27;
} PPPCFG;

#define AVE_CALL0(name, cmd)              \
    s16 name(void) {                      \
        s16 r;                            \
        Ave_SifCallRpcSemaWait();         \
        r = Ave_SifCallRpc(cmd, 0x20, 0x20); \
        Ave_SifCallRpcSemaSignal();       \
        return r;                         \
    }

#define AVE_CALL2(name, cmd)              \
    s16 name(int a, int b) {              \
        s16 r;                            \
        int *p;                           \
        Ave_SifCallRpcSemaWait();         \
        p = (int *)(SifRpcWork + 0x1C);   \
        p[0] = a;                         \
        p[1] = b;                         \
        r = Ave_SifCallRpc(cmd, 0x28, 0x20); \
        Ave_SifCallRpcSemaSignal();       \
        return r;                         \
    }

s16 Ave_SifBindRpc(void) {
    SEMAP sp;
    volatile int i;

    if (Inet_rpc_sema[0] == 0) {
        memset(&sp, 0, 0x18);
        sp.init = 1;
        sp.max = 1;
        Inet_rpc_sema[0] = CreateSema(&sp);
    }
    Inet_rpc_non_blocking_call_enable[0] = 0;
    Inet_rpc_non_blocking_call_result[0] = 0;
    for (;;) {
        if (sceSifBindRpc(&Ave_wrap_sif, 0x1270030, 0) >= 0 && Ave_wrap_sif.serve != 0) {
            break;
        }
        i = 0x10000;
        while (i != 0) {
            i--;
        }
    }
    return 0;
}

s16 Ave_SifUnBindRpc(void) {
    memset(&Ave_wrap_sif, 0, 0x28);
    return 0;
}

s16 Ave_SifCallRpc(int cmd, int ssize, int rsize) {
    int r;

    if (Inet_rpc_non_blocking_call_enable[0] != 0) {
        return Ave_SifCallRpcNb(cmd, ssize, rsize);
    }
    r = sceSifCallRpc(&Ave_wrap_sif, cmd, 0, SifRpcWork, (16 - ssize % 16) + ssize, SifRpcWork, (16 - rsize % 16) + rsize, 0, 0);
    if (r < 0) {
        return -33;
    }
    return W16(0x18);
}

s16 Ave_SifCallRpcNb(int cmd, int ssize, int rsize) {
    int r;

    if (Ave_SifCallRpcEnd() == 0) {
        return -33;
    }
    r = sceSifCallRpc(&Ave_wrap_sif, cmd, 1, SifRpcWork, (16 - ssize % 16) + ssize, SifRpcWork, (16 - rsize % 16) + rsize, 0, 0);
    if (r < 0) {
        return -33;
    }
    return 0;
}

int Ave_SifCallRpcEnd(void) {
    return (s16)!sceSifCheckStatRpc(&Ave_wrap_sif);
}

int Ave_SifCallRpcSemaWait(void) {
    return WaitSema(Inet_rpc_sema[0]);
}

int Ave_SifCallRpcSemaSignal(void) {
    return SignalSema(Inet_rpc_sema[0]);
}

s16 Ave_TcpInitialize(int a, int b) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W32(0) = a;
    W32(4) = b;
    r = Ave_SifCallRpc(5, 0x20, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_TcpTerminate(void) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(6, 0x20, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_Ifconfig(int a, int b, int c) {
    s16 r;
    int *p;

    Ave_SifCallRpcSemaWait();
    p = (int *)(SifRpcWork + 0x1C);
    p[0] = a;
    p[1] = b;
    p[2] = c;
    r = Ave_SifCallRpc(1, 0x2C, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_Ifdown, 2)

s16 Ave_SetOpt(s16 sock, s16 opt, s16 len, void *buf) {
    s16 r;
    s16 *p;

    Ave_SifCallRpcSemaWait();
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = sock;
    p[1] = opt;
    p[2] = len;
    memcpy(p + 3, buf, len);
    r = Ave_SifCallRpc(3, 0x126, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_GetOpt(s16 sock, s16 opt, s16 len, void *buf) {
    int r;
    s16 *p;

    Ave_SifCallRpcSemaWait();
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = sock;
    p[1] = opt;
    p[2] = len;
    r = Ave_SifCallRpc(4, 0x126, 0x126);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    if (r > len) {
    } else {
        memcpy(buf, p + 3, r);
    }
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_TcpOpen(int a, u16 b, u16 c) {
    s16 r;
    int *p;

    Ave_SifCallRpcSemaWait();
    p = (int *)(SifRpcWork + 0x1C);
    p[0] = a;
    *(s16 *)((u8 *)p + 4) = b;
    *(s16 *)((u8 *)p + 6) = c;
    r = Ave_SifCallRpc(7, 0x28, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_TcpClose(s16 sock) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W16(0x1C) = sock;
    r = Ave_SifCallRpc(0xB, 0x22, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_TcpAbort(s16 sock) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W16(0x1C) = sock;
    r = Ave_SifCallRpc(0xD, 0x22, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_TcpStat(s16 sock, u16 *a, u16 *b, u16 *c) {
    s16 r;
    u16 *p;

    Ave_SifCallRpcSemaWait();
    p = (u16 *)(SifRpcWork + 0x1C);
    p[0] = sock;
    r = Ave_SifCallRpc(0xF, 0x2A, 0x2A);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    *a = p[1];
    *b = p[3];
    *c = p[4];
    Ave_SifCallRpcSemaSignal();
    return r;
}


































