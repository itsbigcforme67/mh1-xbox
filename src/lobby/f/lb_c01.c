/* lb_c01 - lobby small helpers 0x005C8750-0x005C8804: lb_rule_seet_trans_ot2, Lb_Matching, get_questLevelNum. Whole file in lb_c.c. */
#include "lobby_f.h"













void lb_rule_seet_trans_ot2(s32 *p) {
    if (SoftKeyboard_alive_check() != 0) {
        font_set_stack_no(p[6]);
        DispSoftkeyboard(1);
    }
}

void Lb_Matching(void) {
    u8 m = CW8(0x32C5);
    if (m == 1) {
        Lbs_MatchStart(m);
    }
}

int get_questLevelNum(void) {
    if (Online_ck() == 1) {
        return 6;
    }
    return 5;
}
