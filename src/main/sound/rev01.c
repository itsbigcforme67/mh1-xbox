/* Reverb presets (SLPM_654.95 0x0021E270-0x0021E2D8): Init_rev_set, Zero_rev_set. HdMerge (the .hd bank merger
 * after them) is not done. */
#include "types.h"

int flSndSetRev();

void Init_rev_set(void) {
    flSndSetRev(0, 4, 0x1800, 0, 0);
    flSndSetRev(1, 4, 0xA00, 0, 0);
}

void Zero_rev_set(void) {
    flSndSetRev(0, 4, 0, 0, 0);
    flSndSetRev(1, 4, 0, 0, 0);
}
