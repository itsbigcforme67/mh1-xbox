/* G online (PC): lb_pl_chat02, written from Monster Hunter G's code at lobby 0x5994E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Pl_master_ck();
s32 Lb_Pl_act_set(); /* extern */
s32 Lb_Pl_basic_flagset(); /* extern */
s32 Lb_Pl_chat_act_set();        /* extern */
s32 Lb_pl_chr_set();   /* extern */

void lb_pl_chat02(void *arg0) {
    s32 temp_v0;
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 5));
    switch (temp_a1) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a1 + 1);
        Lb_pl_chr_set(0x26C, 6, 0);
        (*(s32 *)((u8 *)(arg0) + 0x39C)) = 0;
        Lb_Pl_basic_flagset(arg0, 0, 0, 0);
        (*(s32 *)((u8 *)(arg0) + 8)) = 0x3C;
        return;
    case 1:
        temp_v0 = (*(s32 *)((u8 *)(arg0) + 8));
        if (temp_v0 > 0) {
            (*(s32 *)((u8 *)(arg0) + 8)) = (s32) (temp_v0 - 1);
        }
        if ((Pl_master_ck(arg0) == 1) && ((*(s32 *)((u8 *)(arg0) + 8)) == 0)) {
            if ((s32) (*(u16 *)((u8 *)(arg0) + 0x384)) >= 0x28) {
                Lb_Pl_act_set(arg0, 1, 0x11, 0);
                return;
            }
            if ((*(u8 *)((u8 *)(arg0) + 0x8C4)) != 0) {
                Lb_Pl_chat_act_set(arg0, 1);
            }
        } else {
            return;
        }
        break;
    }
}

