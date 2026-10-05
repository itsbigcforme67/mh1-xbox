/* lb_i01 - lobby move dispatch 0x005CFA50-0x005CFA98: Lb_Pl_chat_act_set. Whole file in lb_i.c. */
#include "lobby.h"
extern u8 D_3E4ECC[];






void Lb_Pl_chat_act_set(PLW *pl) {
    u8 t = PLU8(pl, 0x8C4);
    if (t != 0) {
        Lb_Pl_act_set(pl, 1, chat_act_tbl_0064E198[t], 0);
        PLU8(pl, 0x8F0) = 0;
    }
}
