/* lb_n06 - lobby senders 0x005C8EF0-0x005C8F90: Lb_guild_trans. Whole file in lb_n.c. */
#include "lobby_f.h"













extern u8 D_3E55F0[], D_3E5FF0[], D_3E69F0[], D_3E73F0[], D_3E7DF0[], D_3E87F0[], D_3E91F0[];


void Lb_guild_trans(u8 *p) {
    if (lb_sys.x06 >= 2) {
        font_set_stack_no(F(s32, p, 0x18));
        font_set_palette(0);
        switch (lb_sys.x06) {
        case 3:
            lb_select_quest_level_trans(lb_sys.x06);
            return;
        case 15:
        case 5:
            lb_questpage_trans(p);
            return;
        case 8:
            lb_rule_seet_trans(p);
            break;
        }
    }
}
