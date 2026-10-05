/* lb_k04 - lobby chat handlers 0x005D4E00-0x005D4ED0: lb_pl_chat04. Whole file in lb_k.c. */
#include "lobby.h"
int frame_check2(f32, PLW *, int);
void Pl_basic_flagset();
void Lb_Pl_act_set2();















void lb_pl_chat04(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x270, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xCF, 4, 0x3C);
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
