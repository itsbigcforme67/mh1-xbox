/* lb_bz03 - lobby UI/client 0x005B1E80-0x005B1E8C: cnLbc_EraseDialog (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void cnLbc_EraseDialog(s8 arg0) {
    F(s8, (u8 *)cw, 0x2F79) = arg0;
}
