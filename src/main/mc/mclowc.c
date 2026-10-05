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

int mc_get_dir(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcGetDir(w->port, 0, w->name, 0, 0x100, w->buf) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            int r;

            w->step = 0;
            r = w->res;
            if (r < 0) {
                w->cnt = 0;
                return 2;
            }
            w->cnt = r;
            return 0;
        }
        break;
    }
    return -1;
}
