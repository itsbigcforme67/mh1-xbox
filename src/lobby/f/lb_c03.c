/* lb_c03 - lobby small helpers 0x005CB360-0x005CB384: Lb_menu_quest_info. Whole file in lb_c.c. */
#include "lobby_f.h"













void Lb_menu_quest_info(s32 *p) {
    *(s32 **)0x3C74D4 = p;
    *(s32 *)0x3C7450 = p[4];
    *(s32 *)0x3C7454 = p[2];
}
