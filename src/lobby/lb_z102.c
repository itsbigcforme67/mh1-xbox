/* lb_z102 - auto-drafted 0x0060DBC0-0x0060DC34: Plaza_disp_chat, Plaza_chatlog_i (first drafted by tools/lbauto.py). */
#include "lobby.h"

void Plaza_disp_chat(void) {
    SetFilterMode(0);
    plaza_disp_chat_log_sub(0, 0, *(u8 *)0x39DAD4);
}

void Plaza_chatlog_i(void) {
    *(s8 *)0x39DAE0 = 0;
    if (Plaza_get_chat_line_num() < 0xAU) {
        *(s8 *)0x39DAE1 = 1;
        return;
    }
    *(u8 *)0x39DAE1 = 0;
}
