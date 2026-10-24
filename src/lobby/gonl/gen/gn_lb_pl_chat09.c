/* G online (PC): lb_pl_chat09, written from Monster Hunter G's code at lobby 0x599BA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_pl_to_normal();
s32 Lb_Pl_basic_flagset(); /* extern */
s32 Lb_pl_chr_set();       /* extern */
extern u16 chat09_chr_tbl_0064E1C0;

void lb_pl_chat09(void *arg0, s32 arg1) {
    u8 temp_a2;

    temp_a2 = (*(u8 *)((u8 *)(arg0) + 5));
    switch (temp_a2) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a2 + 1);
        Lb_pl_chr_set((*(u16 *)((u8 *)&chat09_chr_tbl_0064E1C0 + (arg1 * 4))), 4, 0);
        (*(s32 *)((u8 *)(arg0) + 0x39C)) = 0;
        Lb_Pl_basic_flagset(arg0, 0, 0, 0);
        return;
    case 1:
        if ((*(s32 *)((u8 *)(arg0) + 0x194)) == 0) {
            Lb_pl_to_normal(arg0, 0, (s32) (*(u16 *)((u8 *)&(*(u16 *)((u8 *)&chat09_chr_tbl_0064E1C0 + 0x2)) + (arg1 * 4))), 0);
        }
        return;
    }
}

