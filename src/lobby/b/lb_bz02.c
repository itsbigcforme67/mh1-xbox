/* lb_bz02 - lobby UI/client 0x005AFC90-0x005AFCC0: lb_monster_list_check (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 lb_monster_list_check(s32 arg0) {
    if ((arg0 >= 0x125) && (arg0 < 0x143)) {
        return 1;
    }
    return 0;
}
