/* ave_nm (whole file, ave01/ave02 are the linked parts; Ave_TcpSend and Ave_TcpRecv are near-matches) - SLPM_654.95 0x00233B10-0x002351B0 (f_ave.s): "Ave" layer, thin wrappers that pack arguments into the
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

int Ave_TcpSend(s16 sock, u8 *buf, s16 len) {
    int r;
    s16 n;
    u8 *p;
    volatile int i;

    Ave_SifCallRpcSemaWait();
    r = 0;
    p = SifRpcWork + 0x1C;
    *(s16 *)p = sock;
    while (len > 0) {
        n = len;
        if (n > 0x3CA) {
            n = 0x3CA;
        }
        *(s16 *)(p + 2) = n;
        memcpy(p + 4, buf, n);
        W32(0) = 0;
        W32(4) = 0;
        r = Ave_SifCallRpc(0x10, n + 0x26, n + 0x26);
        if (r < 0) {
            Ave_SifCallRpcSemaSignal();
            return r;
        }
        len = len - n;
        buf += n;
        if (len > 0) {
            for (i = 0; i < 10000; i++) {
            }
        }
    }
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_TcpRecv(s16 sock, void *buf, s16 len) {
    int r;
    s16 *p;
    int n;

    if (len <= 0) {
        return 0;
    }
    Ave_SifCallRpcSemaWait();
    if (len >= 0x3CB) {
        len = 0x3CA;
    }
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = sock;
    p[1] = len;
    r = Ave_SifCallRpc(0x13, len + 0x26, len + 0x26);
    FlushCache(0);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    n = W32(0x18);
    if (!(len < n)) {
        len = n;
    }
    if (len > 0) {
        memcpy(buf, p + 2, len);
    }
    Ave_SifCallRpcSemaSignal();
    return len;
}

s16 Ave_TcpDelete(s16 sock) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W16(0x1C) = sock;
    r = Ave_SifCallRpc(0x16, 0x22, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL2(Ave_RouteAdd, 0x1E)

AVE_CALL2(Ave_RouteDel, 0x1F)

AVE_CALL0(Ave_PppInit, 0x21)

AVE_CALL0(Ave_PppDisp, 0x22)

s16 Ave_PppStart(PPPCFG *c) {
    s16 r;
    u8 *p;

    Ave_SifCallRpcSemaWait();
    W32(0) = c->x01;
    W32(4) = c->x26;
    W32(8) = c->x27;
    p = SifRpcWork + 0x1C;
    memset(p, 0, 0x30E);
    *(s16 *)p = c->x00;
    strcpy(p + 4, c->s04);
    strcpy(p + 0x104, c->s08);
    strcpy(p + 0x204, c->s0C);
    strcpy(p + 0x244, c->s10);
    strcpy(p + 0x284, c->s14);
    *(s16 *)(p + 2) = c->x02;
    strcpy(p + 0x2C4, c->s18);
    *(s16 *)(p + 0x304) = c->x1C;
    *(s16 *)(p + 0x306) = c->x1E;
    *(s16 *)(p + 0x308) = c->x20;
    *(s16 *)(p + 0x30A) = c->x22;
    *(s16 *)(p + 0x30C) = c->x24;
    r = Ave_SifCallRpc(0x23, 0x32E, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_PppFinish, 0x24)

int Ave_PppStatus(PPPST *out) {
    int r;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x25, 0x20, 0x4C);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    *out = *(PPPST *)(SifRpcWork + 0x1C);
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_PppGetUsbDeviceId(int *out) {
    int r;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x26, 0x20, 0x4C);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    *out = W32(0x1C);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_PppSetOption(s16 a, s16 len, void *buf) {
    s16 r;
    s16 *p;

    Ave_SifCallRpcSemaWait();
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = a;
    p[1] = len;
    memcpy(p + 2, buf, len);
    r = Ave_SifCallRpc(0x27, 0x124, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_NdgInit, 0x29)

AVE_CALL0(Ave_NdgDisp, 0x2A)

s16 Ave_NdgStart(s16 x) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W32(0) = x;
    r = Ave_SifCallRpc(0x2B, 0x20, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_NdgPolling, 0x2C)

AVE_CALL0(Ave_NdgFinish, 0x2D)

AVE_CALL0(Ave_DhcpInit, 0x2E)

AVE_CALL0(Ave_DhcpDisp, 0x2F)

int Ave_DhcpGetDns(char *name, int *a, int *b) {
    int r;
    char *p;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x30, 0x20, 0x128);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    p = (char *)(SifRpcWork + 0x1C);
    strcpy(name, p);
    *a = *(int *)(p + 0x100);
    *b = *(int *)(p + 0x104);
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_DhcpGetGateway(int *out) {
    int r;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x31, 0x20, 0x24);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    *out = W32(0x1C);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL2(Ave_DhcpRequestNb, 0x36)

int Ave_DhcpGetIfInfo(int *a, int *b, int *c) {
    int r;
    int *p;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x37, 0x20, 0x2C);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    p = (int *)(SifRpcWork + 0x1C);
    *a = p[0];
    *b = p[1];
    *c = p[2];
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_DhcpTimer, 0x38)

AVE_CALL2(Ave_DhcpReleaseNb, 0x3A)

s16 Ave_DnsInit(char *name, int a, int b) {
    s16 r;
    char *p;

    Ave_SifCallRpcSemaWait();
    p = (char *)(SifRpcWork + 0x1C);
    if (name != 0) {
        strcpy(p, name);
    } else {
        *p = 0;
    }
    *(int *)(p + 0x100) = a;
    *(int *)(p + 0x104) = b;
    r = Ave_SifCallRpc(0x3B, 0x128, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

AVE_CALL0(Ave_DnsDisp, 0x3C)

s16 Ave_DnsGetTicket(char *s) {
    s16 r;
    char *p;

    Ave_SifCallRpcSemaWait();
    p = (char *)(SifRpcWork + 0x1C);
    strncpy(p, s, 0x200);
    p[0x1FF] = 0;
    r = Ave_SifCallRpc(0x3D, 0x220, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_DnsReleaseTicket(s16 t) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W16(0x1C) = t;
    r = Ave_SifCallRpc(0x3E, 0x22, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_DnsLookUp(s16 t, int *out) {
    int r;
    s16 *p;

    Ave_SifCallRpcSemaWait();
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = t;
    r = Ave_SifCallRpc(0x3F, 0x28, 0x28);
    if (0 <= r) {
        *out = *(int *)(p + 2);
    }
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_DgGetDeviceInfoNum(int *out) {
    int r;

    Ave_SifCallRpcSemaWait();
    r = Ave_SifCallRpc(0x46, 0x20, 0x20);
    if (0 <= r) {
        *out = W32(0);
    }
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_DgGetDeviceInfo(int idx, int *a, int *b, int *c) {
    s16 r;
    int *p;

    Ave_SifCallRpcSemaWait();
    W32(0) = idx;
    r = Ave_SifCallRpc(0x47, 0x20, 0x32C);
    p = (int *)(SifRpcWork + 0x1C);
    *a = p[0];
    *b = p[1];
    *c = p[2];
    Ave_SifCallRpcSemaSignal();
    return r;
}

s16 Ave_DgSelectDevice(int dev) {
    s16 r;

    Ave_SifCallRpcSemaWait();
    W32(0) = dev;
    r = Ave_SifCallRpc(0x48, 0x20, 0x20);
    Ave_SifCallRpcSemaSignal();
    return r;
}

int Ave_DgGetOption(int a, int len, void *buf) {
    int r;
    int *p;

    Ave_SifCallRpcSemaWait();
    p = (int *)(SifRpcWork + 0x1C);
    p[0] = a;
    p[1] = len;
    r = Ave_SifCallRpc(0x4A, 0x128, 0x128);
    if (r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    if (r > len) {
    } else {
        memcpy(buf, p + 2, r);
    }
    Ave_SifCallRpcSemaSignal();
    return r;
}
