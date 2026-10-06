/* imeby: raw_kouho (SLPM_654.95 0x00246520-0x0024658C): builds a candidate for the raw romaji text at pos. IME engine (name entry), see imead.c for the context.
 * create_kouho is called with a stale fifth argument (t0) by the original, as K&R code. */
#include "types.h"

typedef long long s64;
typedef struct KH KH;

extern s64 wdsbuf[128];
void trans_roman();
KH *create_kouho();

KH *raw_kouho(int pos, int len, int mode)
{
    KH *out;
    int cnt;
    u8 *w;

    w = (u8 *)wdsbuf;
    *(u16 *)w = 0xFFFF;
    ((u16 *)w)[1] = 0;
    w[4] = 0;
    trans_roman(w + 5, pos, len, mode);
    return create_kouho(w, 0, len, &out, &cnt);
}
