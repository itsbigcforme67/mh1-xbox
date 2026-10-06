/* fl texture palette conversion: flPS2Conv4_8_32 and its file-static workers (4/8 bit swizzled pages to 32 bit). SLPM_654.95 0x0018AC90-0x0018B3A4.
 * Conv4to32 / Conv8to32 copy a 128x128 (4 bit) or 128x64 (8 bit) page worth of rows into a stack buffer and hand it to PageConv*, which walks the
 * block order table (block_table4/8) and calls BlockConv* for each 16 byte wide block through the idx32_h / idx32_v position tables. */
#include "types.h"

extern int block_table4[32];
extern int block_table8[32];
extern int idx32_h[32];
extern int idx32_v[32];
typedef struct CLMP { int a, b; } CLMP;
extern CLMP *clm_tbl4_ptr[4];
extern CLMP *clm_tbl8_ptr[4];

void *memcpy(void *, const void *, int);

static void Conv4to32();
static void Conv8to32();
static void PageConv4to32(u8 *buf, u8 *dst, int bw);
static void PageConv8to32(u8 *buf, u8 *dst, int bw);
static void BlockConv4to32(u8 *src, u8 *dst, int bw);
static void BlockConv8to32(u8 *src, u8 *dst, int bw);

int flPS2Conv4_8_32(a, b, c, d, mode)
int a;
int b;
int c;
int d;
int mode;
{
    switch (mode) {
    case 0:
        Conv4to32();
        return 0;
    case 1:
        Conv8to32();
        return 0;
    default:
        return -1;
    }
}

static void Conv4to32(w, h, src, dst)
int w;
int h;
u8 *src;
u8 *dst;
{
    u8 buf[0x2000];
    int bw;
    int bh;
    int i;
    int j;
    int k;
    u8 *fp;
    u8 *d;
    u8 *s;
    u8 *b;
    int off;

    bw = (w - 1) / 128 + 1;
    bh = (h - 1) / 128 + 1;
    for (i = 0; i < bh; i++) {
        off = bw * (i * 0x2000);
        fp = src + off;
        d = dst + off;
        for (j = 0; j < bw; j++) {
            s = fp;
            b = buf;
            for (k = 0; k < 0x80; k++) {
                memcpy(b, s, 0x40);
                s += bw << 6;
                b += 0x40;
            }
            PageConv4to32(buf, d, bw);
            fp += 0x40;
            d += 0x100;
        }
    }
}

static void PageConv4to32(u8 *buf, u8 *dst, int bw) {
    int *tp = block_table4;
    int r;
    int c;
    u8 *p;
    int t;

    for (r = 0; r < 8; r++) {
        p = buf + (r << 6);
        for (c = 0; c < 4; c++) {
            t = *tp++;
            BlockConv4to32(p, dst + bw * (idx32_v[t] << 11) + (idx32_h[t] << 5), bw);
            p += 0x10;
        }
    }
}

static void BlockConv4to32(u8 *src, u8 *dst, int bw) {
    CLMP **tp = clm_tbl4_ptr;
    CLMP *e;
    int a;
    int b;
    int x;
    int lo;
    int hi;
    int n;

    for (a = 0; a < 4; a++) {
        e = *tp++;
        for (b = 0; b < 2; b++) {
            for (x = 0; x < 0x20; x++) {
                n = e->a;
                if (n & 1) {
                    lo = (src[n / 2] & 0xF0) >> 4;
                } else {
                    lo = src[n / 2] & 0xF;
                }
                n = e->b;
                e++;
                if (n & 1) {
                    hi = src[n / 2] & 0xF0;
                } else {
                    hi = (src[n / 2] & 0xF) * 0x10;
                }
                *dst++ = (lo & 0xFF) | (hi & 0xFF);
            }
            dst += (bw << 8) - 0x20;
        }
        src += 0x100;
    }
}

static void Conv8to32(w, h, src, dst)
int w;
int h;
u8 *src;
u8 *dst;
{
    u8 buf[0x2000];
    int bw;
    int bh;
    int i;
    int j;
    int k;
    u8 *fp;
    u8 *d;
    u8 *s;
    u8 *b;
    int off;

    bw = (w - 1) / 128 + 1;
    bh = (h - 1) / 64 + 1;
    for (i = 0; i < bh; i++) {
        off = bw * (i * 0x2000);
        fp = src + off;
        d = dst + off;
        for (j = 0; j < bw; j++) {
            s = fp;
            b = buf;
            for (k = 0; k < 0x40; k++) {
                memcpy(b, s, 0x80);
                s += bw << 7;
                b += 0x80;
            }
            PageConv8to32(buf, d, bw);
            fp += 0x80;
            d += 0x100;
        }
    }
}

static void PageConv8to32(u8 *buf, u8 *dst, int bw) {
    int *tp = block_table8;
    int r;
    int c;
    u8 *p;
    int t;

    for (r = 0; r < 4; r++) {
        p = buf + (r << 7);
        for (c = 0; c < 8; c++) {
            t = *tp++;
            BlockConv8to32(p, dst + bw * (idx32_v[t] << 11) + (idx32_h[t] << 5), bw);
            p += 0x10;
        }
    }
}

static void BlockConv8to32(u8 *src, u8 *dst, int bw) {
    CLMP **tp = clm_tbl8_ptr;
    CLMP *e;
    int a;
    int b;
    int x;

    for (a = 0; a < 4; a++) {
        e = *tp++;
        for (b = 0; b < 2; b++) {
            for (x = 0; x < 0x20; x += 8) {
                *dst++ = src[e->a];
                *dst++ = src[e->b];
                e++;
                *dst++ = src[e->a];
                *dst++ = src[e->b];
                e++;
                *dst++ = src[e->a];
                *dst++ = src[e->b];
                e++;
                *dst++ = src[e->a];
                *dst++ = src[e->b];
                e++;
            }
            dst += (bw << 8) - 0x20;
        }
        src += 0x200;
    }
}
