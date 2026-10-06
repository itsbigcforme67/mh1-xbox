/* boot01 - 0x0022FDD0-0x0022FE00: program entry (main): starts the CRI middleware and the network queue, then runs ACRMain forever. Whole file in boot_nm.c. */
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
