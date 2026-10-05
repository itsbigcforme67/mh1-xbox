/* lb_z65 - auto-drafted 0x005FF0A0-0x005FF0D4: Option_tag_close_check (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void Option_tag_close_check(void) {
    if (F(u8, bsw, 0x7F0) != 0) {
        Disp_Send_PullDown();
        F(u8, bsw, 0x7F0) = 0U;
    }
}
