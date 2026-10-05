/* lb_z61 - auto-drafted 0x005FD3B0-0x005FD3DC: ScrollXY (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsCur;
extern u8 * bsSys;

void ScrollXY(s32 arg0, s32 arg1) {
    void *temp_a0;
    void *temp_a2;

    temp_a2 = bsSys;
    F(s32, temp_a2, 4) = (F(s32, temp_a2, 4) + arg0);
    temp_a0 = bsSys;
    F(s32, temp_a0, 8) = (F(s32, temp_a0, 8) + arg1);
    F(s8, bsCur, 4) = 0;
}
