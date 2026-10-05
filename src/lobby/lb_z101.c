/* lb_z101 - auto-drafted 0x0060D6E0-0x0060D704: Plaza_chat_clear (first drafted by tools/lbauto.py). */
#include "lobby.h"

void Plaza_chat_clear(void) {
    Chat_log_clear();
    *(s8 *)0x39DAD4 = 0;
}
