/* flSndOutputMode and flSndPackLoadBG2 now match and are built from flsnd06.c; kept here for reference. */
#include "types.h"

extern int hdpack[];

int SdrSetOutputMode(int);
int flSndPackLoadSub2(void *, void *, int);

int flSndOutputMode(int mode) {
    if (mode == 0) {
        return SdrSetOutputMode(0);
    }
    return SdrSetOutputMode(1);
}

int flSndPackLoadBG2(void *pk, void *dst, int slot) {
    int r;

    if (hdpack[0] <= 0) {
        return 0;
    }
    r = flSndPackLoadSub2(pk, dst, slot);
    if (r < 0) {
        return r;
    }
    return 0;
}
