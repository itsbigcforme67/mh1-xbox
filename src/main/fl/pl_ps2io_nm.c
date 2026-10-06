/* Near-match (not linked): ps2McInit (0x00194A50), 2 of 34 instructions differ (the original sets a1 = 0 after the first address load). */
#include "types.h"

typedef struct PS2SLOTW { u8 b[12]; } PS2SLOTW;
extern u8 Ps2_card_work[0x64];
extern PS2SLOTW Ps2_slot_work[2];
void flMemset(void *, int, int);
int sceMcInit(void);

int ps2McInit(void) {
    int i;

    while (sceMcInit() == -101) {
    }
    flMemset(Ps2_card_work, 0, 0x64);
    for (i = 0; i < 2; i++) {
        flMemset(&Ps2_slot_work[i], 0, 12);
    }
    return 1;
}

