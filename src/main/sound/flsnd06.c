/* flsnd06 - SLPM_654.95 0x00216050-0x002160CC: flSndOutputMode (mono/stereo for the sound driver) and flSndPackLoadBG2 (background load
 * of a second sound pack, only once the first pack's table exists: hdpack[0] > 0). */
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
