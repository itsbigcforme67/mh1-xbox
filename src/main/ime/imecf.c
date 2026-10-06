/* imecf: concat_bslen (SLPM_654.95 0x002448E0-0x00244990): length of the run of bunsetu candidates starting at pos that can be merged (-1 when a position has no length). IME engine (name entry), see imead.c for the context. */
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

int concat_bslen(int pos, int end)
{
    int n;
    HCHAR *h;
    int c;

    c = 0;
    h = &hchar[pos];
    n = 0;
    while (pos < end) {
        c = h->x15;
        if (c == 0) {
            return -1;
        }
        if (h->bs != 0 && h->bs != (BS *)-1 && h->bs->x02 != 0x28) {
            break;
        }
        pos += c;
        n += c;
        h += c;
    }
    if (n == 0 || pos >= end) {
        return n;
    }
    return n + c;
}
