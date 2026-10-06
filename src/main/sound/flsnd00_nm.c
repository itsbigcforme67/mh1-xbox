/* Near-matches next to flsnd00.c (not built): flSndOutputMode 8 off, flSndPackLoadBG2 7 off. In the original
 * the `else`/zero part comes first: `bgtz` over a `li v0,0; b end` fall-through, call part out of line. */
#include "types.h"

extern int hdpack[];

void SdrSetOutputMode(int);
int flSndPackLoadSub2(void *, void *, int);

void flSndOutputMode(int mode) {
    if (mode == 0) {
        SdrSetOutputMode(0);
        return;
    }
    SdrSetOutputMode(1);
}

int flSndPackLoadBG2(void *pk, void *dst, int slot) {
    int r;

    if (hdpack[0] > 0) {
        r = flSndPackLoadSub2(pk, dst, slot);
        if (r >= 0) {
            return 0;
        } else {
            return r;
        }
    } else {
        return 0;
    }
}
