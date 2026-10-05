/* lb_bz26 - lobby UI/client 0x005B6EC0-0x005B6ED0: cmcs_05 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 COM_R_No_1;

void cmcs_05(void) {
    COM_R_No_1 = (u8) (COM_R_No_1 + 1);
}
