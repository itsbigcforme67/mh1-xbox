/* boot01 - 0x0022FDD0-0x00230060: program entry (main), path helper and the
 * lobby-to-MCS network setup. main only starts the CRI middleware and the
 * network queue, then runs ACRMain forever. */
#include "types.h"

void mwInit(void);
void CngNetAQInit(void *);
void ACRMain(void);
void *memset(void *, int, int);
int CpInetTcpGetOption(int, s32 *);
int CpInetTcpSetOption(int, s32 *);
int CpInetPppGetStatus(void *);
int atoi(const char *);

extern u8 cng_netAQ[];
extern u8 InetSys[0x10];
extern u8 InetGame[0x14];
extern u8 InetBuf[0x1000];
extern u8 PppStatusv[0x14];
extern u8 ConnWork[0x2C];
extern s32 *CurDevice;

void main(void) {
    mwInit();
    CngNetAQInit(cng_netAQ);
    for (;;) {
        ACRMain();
    }
}

int SlashToBackslash(char *s) {
    while (*s != 0) {
        if (*s == '/') {
            *s = '\\';
        }
        s++;
    }
    return 0;
}

void InetDbgPrint(char *fmt, int a, int b, int c, ...) {
    /* debug output removed */
}

void InetConnectInitialize(void) {
    memset(InetSys, 0, 0x10);
    memset(InetGame, 0, 0x14);
    memset(InetBuf, 0, 0x1000);
    memset(PppStatusv, 0, 0x14);
    memset(ConnWork, 0, 0x2C);
    *(s32 *)(ConnWork + 4) = -1;
}

void LobbyToMcsInitSocket(s32 sock) {
    s32 opt[2];

    *(s32 *)(ConnWork + 4) = sock;
    CpInetTcpGetOption(*(s32 *)(ConnWork + 4), opt);
    opt[0] = 1;
    opt[1] = 0;
    CpInetTcpSetOption(*(s32 *)(ConnWork + 4), opt);
}

void LobbyToMcsInit(s8 a, s8 b, char *name) {
    *(s32 *)(PppStatusv + 8) = 0x1863C;
    switch (*CurDevice) {
    case 2:
    case 3:
        CpInetPppGetStatus(PppStatusv);
        break;
    default:
        break;
    }
    InetGame[0] = a;
    InetGame[1] = b;
    InetGame[2] = 0;
    InetGame[3] = 0;
    *(s16 *)(InetGame + 4) = 0;
    *(s16 *)(InetGame + 6) = 0;
    *(s16 *)(InetGame + 8) = 0;
    *(s16 *)(InetGame + 10) = 0;
    *(s16 *)(InetGame + 12) = 0;
    *(s16 *)(InetGame + 14) = 0;
    *(s16 *)(InetGame + 16) = 0;
    *(s16 *)(InetGame + 18) = *(s32 *)(PppStatusv + 8) / 100;
    atoi(name + 5);
}
