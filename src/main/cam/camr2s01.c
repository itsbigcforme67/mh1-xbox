/* camr2s01 - 0x00223760-0x0022386C: GetNearSection, index of the rail section whose start point is closest
 * to a position (rail+0x100: points of 12 bytes, rail+0x260: point count), then a step back one section if the
 * next point is nearer than the previous one. */
#include "types.h"

f32 flvecCalcDistance(void *, void *);

int GetNearSection(u8 *rail, void *pos) {
    f32 dist[16];
    f32 best;
    u8 n;
    u8 *pt;
    f32 *d;
    int i;
    int sec;

    pt = rail + 0x100;
    d = dist;
    best = 1e7f;
    sec = 0;
    for (i = 0; i < rail[0x260]; i++) {
        f32 r = flvecCalcDistance(pos, pt);
        *d = r;
        if (best > r) {
            best = r;
            sec = i;
        }
        pt += 12;
        d++;
    }
    n = rail[0x260];
    if (sec != 0 && sec < n - 1) {
        if (dist[sec - 1] < dist[sec + 1]) {
            sec--;
        }
    } else if (sec >= n - 1) {
        sec = n - 2;
    }
    return sec;
}
