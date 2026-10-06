#include "lobby_a.h"
extern char Lb_gh_board_trans[];
s32 Lb_gh_board(void) {
    s32 temp_s0;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (F(s8, &lb_sys, 6)) {          /* irregular */
    case 0:
        F(s8, &lb_sys, 6) = (s8) (F(s8, &lb_sys, 6) + 1);
        cnWrap_SoundRequest(6);
        break;
    case 1:
        Lb_put_hint(0, 0x16);
        if (temp_s0 & 0xFFFF & 0x240) {
            F(s8, &lb_sys, 6) = 0;
            cnWrap_SoundRequest(3);
            return 1;
        }
        Lbc_set_prim(&Lb_put_help, &Lb_gh_board_trans, 0);
        break;
    }
    return 0;
}
