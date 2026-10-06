/* camq1 - 0x00225F50-0x00225FBC: Fish Wyvern (monster state 0x15) and Legend
 * Sword demo camera requests. */
#include "types.h"

void DemoCameraRequest(int, s32);
extern u8 em_work[];

void FishWyvernCameraRequest(void) {
    u8 *e = em_work;
    int i = 20;

    do {
        if (e[0] != 0 && e[2] == 0x15) {
            DemoCameraRequest(0x11, (s32)e);
            return;
        }
        i--;
        e += 0xA10;
    } while (i != 0);
}

void LegendSwordCameraRequest(void) {
    DemoCameraRequest(0x19, 0);
}
