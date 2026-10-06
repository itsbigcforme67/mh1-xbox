/* ave03 - Ave_TcpRecv (SLPM_654.95 0x002343D0-0x002344F0), whole file in ave_nm.c. */
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
int Ave_TcpRecv(s16 sock, void *buf, int len) {
    int r;
    s16 *p;
    int n;
    s16 l;

    l = len;
    if (l <= 0) {
        return 0;
    }
    Ave_SifCallRpcSemaWait();
    if (l > 0x3CA) {
        len = 0x3CA;
    }
    l = len;
    p = (s16 *)(SifRpcWork + 0x1C);
    p[0] = sock;
    p[1] = len;
    r = Ave_SifCallRpc(0x13, l + 0x26, l + 0x26);
    FlushCache(0);
    if ((s16)r < 0) {
        Ave_SifCallRpcSemaSignal();
        return r;
    }
    n = W32(0x18);
    if (n <= l) {
        len = (s16)n;
    }
    if (0 < (s16)len) {
        memcpy(buf, p + 2, (s16)len);
    }
    Ave_SifCallRpcSemaSignal();
    return len;
}
