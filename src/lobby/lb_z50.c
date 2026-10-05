/* lb_z50 - auto-drafted 0x005F2940-0x005F29B4: BsPoster01_PostData1, BsPoster02_PostData2, BsPoster03_PostData3, BsPoster04_PostData4 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsSys;

void BsPoster01_PostData1(void) {
    void *temp_a0;

    temp_a0 = bsSys;
    F(u8, temp_a0, 2) = (u8) (F(u8, temp_a0, 2) + 1);
}

void BsPoster02_PostData2(void) {
    void *temp_a0;

    temp_a0 = bsSys;
    F(u8, temp_a0, 2) = (u8) (F(u8, temp_a0, 2) + 1);
}

void BsPoster03_PostData3(void) {
    void *temp_a0;

    temp_a0 = bsSys;
    F(u8, temp_a0, 2) = (u8) (F(u8, temp_a0, 2) + 1);
}

void BsPoster04_PostData4(void) {
    void *temp_a0;

    temp_a0 = bsSys;
    F(u8, temp_a0, 2) = (u8) (F(u8, temp_a0, 2) + 1);
}
