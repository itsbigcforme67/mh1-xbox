/* lb_l01 - lobby trade/sleep/exit 0x005D0F70-0x005D1000: lb_exit_save. Whole file in lb_l.c. */
#include "lobby_f.h"





void lb_exit_save(PLW *pl) {
    pl_flag_clr(pl, 0x20000);
    Lb_Pl_act_set2(pl, 0, 0x35, 0);
    CW8(0x2C08) = 1;
    *(s8 *)0x3F36AB = 1;
    str_pause(0, 0);
    str_volume(0, 0);
    str_fadein_vol(0, 0x1E, D_32D471[game_w.stage * 2]);
}
