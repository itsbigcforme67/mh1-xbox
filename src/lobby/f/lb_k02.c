/* lb_k02 - lobby chat handlers 0x005D4ED0-0x005D4FB8: lb_pl_chat05. Whole file in lb_k.c. */
#include "lobby_f.h"
int frame_check2(f32, PLW *, int);
void Pl_basic_flagset();
void Lb_Pl_act_set2();















void lb_pl_chat05(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x274, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (Pl_master_ck(pl) == 1 && frame_check2(380.0f, pl, 0) != 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_Pl_act_set2(pl, 1, 6, 0);
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}
