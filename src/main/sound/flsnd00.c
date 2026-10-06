/* flsnd00 - 0x00216010-0x002160CC: thin fl sound wrappers over the Sdr (SPU2 driver) calls: flSndAllStop,
 * flSndPortStop, flSndSetRev, flSndOutputMode, flSndPackLoadBG2 (load a sound pack if a joint list is
 * set up; the real work is flSndPackLoadSub2). hdpack: [0] pack count, [1..4] HD pointers, [5] HD bytes,
 * [6] BD bytes, [7] HD buffer, [8] BD buffer (filled by flSndJointInit/flSndJointSet). */
#include "types.h"

extern int hdpack[];

void SdrAllStop(void);
void SdrPortStop(u8);
void SdrSetRev(int, int, s16, u8, u8);
void SdrSetOutputMode(int);
int flSndPackLoadSub2(void *, void *, int);

void flSndAllStop(void) {
    SdrAllStop();
}

void flSndPortStop(int port) {
    SdrPortStop(port);
}

void flSndSetRev(int a, int b, int c, int d, int e) {
    SdrSetRev(a, b, c, d, e);
}

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
