/* lb_z49 - auto-drafted 0x005F2820-0x005F2858: BsPosterInit (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 bsMainRetVal;
extern u8 * bsSys;

void BsPosterInit(void) {
    void *temp_a0;

    BsRequestPostClear();
    temp_a0 = bsSys;
    F(u8, temp_a0, 1) = (u8) (F(u8, temp_a0, 1) + 1);
    F(s8, bsSys, 2) = 0;
    bsMainRetVal = 0;
}
