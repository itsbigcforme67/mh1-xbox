/* G online (PC): plaza_chat_log_disp_line, written from Monster Hunter G's code at lobby 0x58ED60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 PitMenu;
void plaza_chat_log_disp_line(s32 arg0) {
    s32 var_a1;

    var_a1 = (*(u8 *)((u8 *)&PitMenu + 0x26)) - (arg0 & 0xFF);
    if (var_a1 != 0) {
        do {
            var_a1 -= 1;
        } while (var_a1 != 0);
    }
}

