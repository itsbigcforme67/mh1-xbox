/* imecb: bs_prefer, calc_point (SLPM_654.95 0x00247D70-0x00247F2C). bs_prefer picks the best bunsetu candidate of position pos (highest score x08), finishes it with bs_ctd and returns its length (-1 when none). IME engine (name entry), see imead.c for the context. */
#include "types.h"

typedef struct PW PW;
typedef struct KH KH;
typedef struct BS BS;

struct BS {
    s16 len;        /* 0x00 */
    u8 x02;
    u8 x03;
    PW *pw;         /* 0x04 */
    u16 x08;
    s16 x0A;
    BS *next;       /* 0x0C */
};

/* 0x1C-byte edit character / bunsetu record, hchar[80] */
typedef struct HCHAR {
    s32 x00;
    void *ch;       /* 0x04 chmem list */
    BS *bs;         /* 0x08 bsmem list */
    KH *kh;         /* 0x0C candidate list */
    s32 x10;
    s8 x14;
    s8 x15;         /* bunsetu length */
    s8 x16;
    s8 x17;
    s8 x18;
    s8 x19;
    s8 x1A;
    s8 x1B;
} HCHAR;

extern HCHAR hchar[80];
int bs_point();
void bs_ctd();
int setu_point();

int bs_prefer(int pos, int end, int len)
{
    BS *b;
    BS *best;
    BS *p;
    HCHAR *h;

    h = &hchar[pos];
    for (b = h->bs; b != 0; b = b->next) {
        if (len < 0 || b->len == len) {
            if (bs_point(b, pos, end) == -1) {
                return -1;
            }
        }
    }
    if (h == 0 || (best = h->bs) == 0) {
        return -1;
    } else {
        for (p = best->next; p != 0; p = p->next) {
            if (p->x08 > best->x08) {
                best = p;
            }
        }
        bs_ctd(best, pos, end);
        return best->len;
    }
}

int calc_point(int pos, BS *b, BS *next)
{
    int a;
    int c;
    int f;
    u16 pri;

    if (next == 0) {
        a = b->len;
        c = 0;
        f = 1;
    } else {
        c = b->len;
        a = next->len;
        f = 0;
    }
    pri = b->x0A;
    return f * 0x32 + (pri + (c * 0x10 + a * 0x11) + setu_point(b, next));
}
