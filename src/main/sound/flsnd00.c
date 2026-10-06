/* flsnd00 - 0x00216010-0x00216044: thin fl sound wrappers over the Sdr (SPU2 driver) calls: flSndAllStop,
 * flSndPortStop, flSndSetRev. The sound pack
 * state hdpack ([0] pack count, [1..4] HD pointers, [5] HD bytes, [6] BD bytes, [7] HD buffer, [8] BD buffer) is
 * filled by flsnd02.c. */
#include "types.h"

void SdrAllStop(void);
void SdrPortStop(u8);
void SdrSetRev(int, int, s16, u8, u8);

void flSndAllStop(void) {
    SdrAllStop();
}

void flSndPortStop(int port) {
    SdrPortStop(port);
}

void flSndSetRev(int a, int b, int c, int d, int e) {
    SdrSetRev(a, b, c, d, e);
}
