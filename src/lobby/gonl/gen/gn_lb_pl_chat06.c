/* G online (PC): lb_pl_chat06, written from Monster Hunter G's code at lobby 0x5998B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_pl_to_normal();
s32 pl_flag_clr();
s32 pl_flag_set();
s32 Lb_Pl_basic_flagset(); /* extern */
s32 Lb_pl_chr_set(); /* extern */

void lb_pl_chat06(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 5));
    switch (temp_a1) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a1 + 1);
        Lb_pl_chr_set((void *)0x275, 4, 0);
        (*(s32 *)((u8 *)(arg0) + 0x39C)) = 0;
        Lb_Pl_basic_flagset(arg0, 0, 0, 0);
        return;
    case 1:
        if ((*(s32 *)((u8 *)(arg0) + 0x194)) == 0) {
            (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a1 + 1);
            pl_flag_set(arg0, 0x20000);
            Lb_pl_chr_set(arg0, 0xD8, -4, 0x36);
            return;
        }
        return;
    case 2:
        pl_flag_set(arg0, 0x20000);
        if ((*(s32 *)((u8 *)(arg0) + 0x194)) == 0) {
            pl_flag_clr(arg0, 0x20000);
            Lb_pl_to_normal(arg0, 0, 4, 0);
        }
        break;
    }
}

