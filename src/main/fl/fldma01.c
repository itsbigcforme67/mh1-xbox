/* fl library DMA queue (VIF1 control block flPs2VIF1Control) and IOP module loaders. SLPM_654.95 0x0016E6E0-0x0016F284.
 * IntGsStoreImageHandler (file-static), flPS2DmaInterrupt and flPS2DmaSend use `ei` / 128 bit register moves and stay original bytes
 * (config/c_rawfuncs.txt). The control block holds two queues (double buffered by flPs2State.tmp_idx): buffer handle, entry count,
 * write and read index; +0x2C is the tag being sent, +0x30 a store-image-in-progress flag, +0x34 / +0x38 the last queued tag and its address. */
#include "types.h"

typedef struct DMACTL {
    int chan;       /* 0x00 */
    int size;       /* 0x04 queue length in words */
    int handler;    /* 0x08 */
    int buf[2];     /* 0x0C system memory handles */
    int cnt[2];     /* 0x14 */
    int w[2];       /* 0x1C */
    int r[2];       /* 0x24 */
    u32 cur;        /* 0x2C */
    int busy;       /* 0x30 */
    u32 last;       /* 0x34 */
    u32 *lastp;     /* 0x38 */
} DMACTL;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern DMACTL flPs2VIF1Control;
extern int flPs2GsHandler;
extern char lit_213_0035BDC0[], lit_242_0035BDE0[];

int flPS2GetSystemMemoryHandle();
int flPS2GetSystemBuffAdrs();
int AddDmacHandler();
void EnableDmac();
int AddIntcHandler();
int sceDmaSync();
int sceSifLoadModule();
int sceSifLoadStartModule();
int printf();
void flPS2DmaSend(void);
int flPS2DmaWait(void);
void flPS2SystemTmpBuffFlush(void);
int flPS2DmaInterrupt(int);
int IntGsStoreImageHandler();

/* IntGsStoreImageHandler (0x0016E6E0, 172 bytes): original bytes, ends with `ei`. */
static asm int IntGsStoreImageHandler()
{
#include "IntGsStoreImageHandler.inc"
}

void flPS2DmaInitControl(DMACTL *c, int size, int handler) {
    int i;
    DMACTL *p;

    i = 0;
    p = c;
    c->size = size;
    do {
        p->buf[0] = flPS2GetSystemMemoryHandle(size * 4, 1);
        i++;
        p->cnt[0] = 0;
        p->w[0] = 0;
        p->r[0] = 0;
        p = (DMACTL *)((u8 *)p + 4);
    } while (i < 2);
    c->handler = AddDmacHandler(c->chan, handler, 0);
    c->busy = 0;
    EnableDmac(c->chan);
    flPs2GsHandler = AddIntcHandler(0, IntGsStoreImageHandler, 0);
}

int flPS2DmaAddQueue(u32 tag, DMACTL *c) {
    int i = PS2S(int, 0x404);
    u32 *q;

    if ((u32)c->cnt[i] <= (u32)(c->size - 1)) {
        q = (u32 *)flPS2GetSystemBuffAdrs(c->buf[i]);
        c->cnt[i]++;
        q[c->w[i]++] = tag;
        if (c->w[i] == c->size) {
            c->w[i] = 0;
        }
        c->last = 0;
        c->lastp = 0;
        return 0;
    }
    return -1;
}

/* original bytes: build/raw/flPS2DmaAddQueue2.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int flPS2DmaAddQueue2(int mode, u32 tag, u32 *x, DMACTL *c)
{
#include "flPS2DmaAddQueue2.inc"
}
#else
int flPS2DmaAddQueue2(int mode, u32 tag, u32 *x, DMACTL *c) {
    int i;
    u32 *q;
    u32 *p;
    u32 w;
    u32 k;

    if (mode == 2) {
enq:
        i = PS2S(int, 0x404);
        if (!((u32)(c->size - 1) < (u32)c->cnt[i])) {
            q = (u32 *)flPS2GetSystemBuffAdrs(c->buf[i]);
            c->cnt[i]++;
            q[c->w[i]++] = tag;
            if (c->w[i] == c->size) {
                c->w[i] = 0;
            }
            c->last = tag;
            c->lastp = x;
            return 0;
        }
        goto fail;
    }
    if (c->last == 0) {
        goto enq;
    }
    if (c->lastp == 0) {
        goto enq;
    }
    k = (c->last & 0xF0000000) >> 28;
    p = c->lastp;
    switch (k) {
    case 0:
    case 3:
    case 4:
    case 5:
        w = p[0];
        if ((w & 0x70000000) != 0x70000000) {
            goto enq;
        }
        p[0] = (w & 0xFFFF) + 0x20000000;
        p[1] = (unsigned long)tag & 0xFFFFFFFUL;
        break;
    case 1:
        w = p[0];
        if (w & 0x70000000) {
            goto enq;
        }
        p[0] = (w & 0xFFFF) + 0x30000000;
        break;
    case 2:
    default:
        goto enq;
    }
    if (mode == 1) {
        c->last = 0;
        c->lastp = 0;
    } else {
        c->last = tag;
        c->lastp = x;
    }
    return 0;
fail:
    c->last = 0;
    c->lastp = 0;
    return -1;
}
#endif

/* flPS2DmaInterrupt (0x0016EAE0, 796 bytes) and flPS2DmaSend (0x0016EE00, 464 bytes): original bytes. */
asm int flPS2DmaInterrupt(int arg)
{
#include "flPS2DmaInterrupt.inc"
}

asm void flPS2DmaSend(void)
{
#include "flPS2DmaSend.inc"
}

int flPS2DmaWait(void) {
    volatile int *p;

    p = &flPs2VIF1Control.cnt[PS2S(int, 0x404) ^ 1];
    while (*p != 0) {
    }
    while (sceDmaSync(PS2S(int, 0x3C8), 1, 0) != 0) {
    }
    return 1;
}

int flPS2DmaTerminate(void) {
    if (flPs2VIF1Control.cnt[0] == 0 && flPs2VIF1Control.cnt[1] == 0) {
        return 0;
    }
    flPS2DmaWait();
    if (flPs2VIF1Control.cnt[PS2S(int, 0x404)] == 0) {
        return 0;
    }
    flPS2DmaSend();
    flPS2DmaWait();
    flPS2SystemTmpBuffFlush();
    return 1;
}

void flPS2IopModuleLoad(int a, int b, int c, int tries) {
    int i;

    if (tries == 0) {
        while (sceSifLoadModule(a, b, c) < 0) {
        }
        return;
    }
    i = 0;
    if (0 < tries) {
        do {
            if (sceSifLoadModule(a, b, c) > 0) {
                return;
            }
            i++;
        } while (i < tries);
    }
    printf(lit_213_0035BDC0, a);
}

void flPS2IopModuleLoadStart(int a, int b, int c, int d, int tries) {
    int i;

    if (tries == 0) {
        while (sceSifLoadStartModule(a, b, c, d) < 0) {
        }
        return;
    }
    i = 0;
    if (0 < tries) {
        do {
            if (sceSifLoadStartModule(a, b, c, d) > 0) {
                return;
            }
            i++;
        } while (i < tries);
    }
    printf(lit_242_0035BDE0, a);
}
