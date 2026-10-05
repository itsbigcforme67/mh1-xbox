/* lb_k05 - lobby chat handlers 0x005D4FC0-0x005D5090: lb_pl_chat06. Whole file in lb_k.c. */
#include "lobby_f.h"
int frame_check2(f32, PLW *, int);
void Pl_basic_flagset();
void Lb_Pl_act_set2();















void lb_pl_chat06(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x275, 4, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xD8, 4, 0x36);
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
            pl->work8F0 = 1;
        }
        break;
    }
}
