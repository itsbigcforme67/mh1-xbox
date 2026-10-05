/* lb_z128 - auto-drafted 0x00604CC0-0x00604D14: Disp_Title (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void Disp_Title(int arg0) {
    int temp_a1;

    temp_a1 = bsw;
    if ((F(s8, temp_a1, 0x186) == 0) && (F(u8, temp_a1, 0xE96B) == 0)) {
        stockTitle(arg0, temp_a1);
    }
    F(s32, bsw, 4) = 0;
    (*(s8 *)arg0) = 0;
}
