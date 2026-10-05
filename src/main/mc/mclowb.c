/* PS2 memory card access state machines, SLPM_654.95 main 0x27EF60-0x27FDF0.
 * MemcardWork (0xB4 bytes): step at +4, result count at +0xC, port at +0x14,
 * sceMcSync results at +0x18/+0x1C, file name at +0x50, file handle at +0x90.
 * Each mc_* function is a small step machine called every frame: it returns -1
 * while busy, 0 when done, 1 on error (mc_check_card returns the card state).
 * Field names are guesses. */
#include "types.h"

typedef struct MCW {
    s32 x00;        /* 0x00 */
    s32 step;       /* 0x04 */
    s32 x08;        /* 0x08 */
    s32 cnt;        /* 0x0C */
    s32 x10;        /* 0x10 */
    s32 port;       /* 0x14 */
    s32 cmd;        /* 0x18 sceMcSync command result */
    s32 res;        /* 0x1C sceMcSync result */
    s32 retry;      /* 0x20 */
    s32 x24;        /* 0x24 */
    s32 type;       /* 0x28 sceMcGetInfo: card type */
    s32 free;       /* 0x2C */
    s32 fmt;        /* 0x30 */
    s32 state[3];   /* 0x34 */
    s32 info[3];    /* 0x40 */
    s32 changed;    /* 0x4C bit mask of ports with a changed card */
    u8 name[0x40];  /* 0x50 */
    s32 fd;         /* 0x90 */
    u8 *buf;        /* 0x94 */
    s32 len;        /* 0x98 */
    s32 x9C;        /* 0x9C */
    s32 xA0;        /* 0xA0 */
    s32 xA4;        /* 0xA4 */
    s32 xA8;        /* 0xA8 */
    u8 _padAC[8];
} MCW;

extern MCW MemcardWork;
extern u8 keep_rtc[8];
extern u8 info_attr[];
extern u8 mc_dir[];
extern char mc_path[];
extern char lit_421_00384300[], lit_422_00384308[], lit_423_00384310[];

void *memset();
void *memcpy();
int sceCdReadClock();
int sceMcInit();
int sceMcSync();
int sceMcGetInfo();
int sceMcGetDir();
int sceMcOpen();
int sceMcRead();
int sceMcWrite();
int sceMcClose();
int sceMcMkdir();
int sceMcSetFileInfo();
int sceMcFormat();
int sceMcDelete();
void sceScfGetLocalTimefromRTC();
int sprintf();
int mc_sync();

#define BCD(x) (((x) & 0xF) + ((((s32)(x)) >> 4) & 0xF) * 10)

/* out: u8 weekday, second, minute, hour(?), day, month, u16 year (struct of 8 bytes) */

void MemcardInit(void)
{
    memset(&MemcardWork, 0, 0xB4);
    MemcardWork.x10 = 2;
    do {
    } while (sceMcInit() < 0);
    sceCdReadClock(keep_rtc);
}

void McReadClock(out)
u8 *out;
{
    u8 clk[8];
    u16 year;
    int mon;

    if (sceCdReadClock(clk) == 0) {
        memcpy(clk, keep_rtc, 8);
    }
    sceScfGetLocalTimefromRTC(clk);
    *(u16 *)(out + 6) = BCD(clk[7]) + 2000;
    out[5] = BCD(clk[6]);
    out[4] = BCD(clk[5]);
    out[3] = BCD(clk[3]);
    out[2] = BCD(clk[2]);
    out[1] = BCD(clk[1]);
    year = *(u16 *)(out + 6);
    mon = out[5];
    out[0] = (out[4] + (year + year / 4 - year / 100 + year / 400 + (mon * 13 + 8) / 5)) % 7;
}

int mc_sync(w)
MCW *w;
{
    w->cmd = 0;
    w->res = 0;
    return -(sceMcSync(1, &w->cmd, &w->res) == 0);
}
