/* sdr04 - SLPM_654.95 0x00214160-0x00214390: sound driver start-up (host side): SdrDelayThread (sleep for a number of ticks with an
 * alarm), sdr_initQue (clear the request ring), sdr_initDev (bind an RPC client, retrying until the IOP answers), SdrInit
 * (binds the two clients, creates the two semaphores, prints a banner). wait_alarm (0x214130, alarm callback with ei) is asm. */
#include "types.h"

typedef struct SEMAPARAM {
    int count;          /* 0x00 */
    int max_count;      /* 0x04 */
    int init_count;     /* 0x08 */
    int wait_threads;   /* 0x0C */
    int attr;           /* 0x10 */
    int option;         /* 0x14 */
} SEMAPARAM;

typedef struct SNDQUE {
    int cmd;
    s8 vol;
    s8 pan;
    s16 pitch;
    s8 x08;
    u8 pad[3];
} SNDQUE;

extern SNDQUE sndque_tbl[];
extern int sque_w_idx[];
extern int sque_r_idx[];
extern int sbuff_idx[];
extern u8 ClientData[];
extern u8 GetStClientData[];
extern int SmId_send;
extern int SmId_get;
extern char lit_151_003671C0[];
extern void wait_alarm();

int GetThreadId();
int SetAlarm(int, void *, int);
int SleepThread();
int sceSifBindRpc(void *, int, int);
int CreateSema(SEMAPARAM *);
int printf(const char *, ...);
int SdrDelayThread(int);
void sdr_initQue(void);
int sdr_initDev(u8 *, int);

int SdrDelayThread(int ticks)
{
    if (SetAlarm(ticks & 0xFFFF, wait_alarm, GetThreadId()) < 0) {
        return -2;
    }
    return SleepThread();
}

void sdr_initQue(void)
{
    int i;

    sbuff_idx[0] = 0;
    sque_r_idx[0] = 0;
    sque_w_idx[0] = 0;
    for (i = 31; i >= 0; i--) {
        sndque_tbl[i].cmd = -1;
    }
}

int sdr_initDev(u8 *client, int id)
{
    for (;;) {
        if (sceSifBindRpc(client, id, 0) < 0) {
            return -1;
        }
        if (*(int *)(client + 0x24) == 0) {
            if (SdrDelayThread(1) < 0) {
                return -1;
            }
            continue;
        }
        return 0;
    }
}

extern int flag_1st_131;

int SdrInit(void)
{
    SEMAPARAM sp;
    int r;

    if (flag_1st_131 != 0) {
        return 0;
    }
    sdr_initQue();
    if (sdr_initDev(ClientData, 0) < 0) {
        return -1;
    }
    if (sdr_initDev(GetStClientData, 1) < 0) {
        return -1;
    }
    sp.option = 0;
    sp.init_count = 1;
    sp.max_count = 1;
    if (SmId_send < 0) {
        r = CreateSema(&sp);
        if (r <= 0) {
            return -1;
        }
        SmId_send = r;
    }
    if (SmId_get < 0) {
        r = CreateSema(&sp);
        if (r <= 0) {
            return -1;
        }
        SmId_get = r;
    }
    flag_1st_131 = 1;
    printf(lit_151_003671C0, 2, 0);
    return 0;
}
