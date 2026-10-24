/* G online (PC): disp_lm_room_member, written from Monster Hunter G's code at lobby 0x596810 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_act_set();
s32 Pl_master_ck();
s32 Lb_Pl_basic_flagset(); /* extern */
s32 Lb_pl_chr_set(); /* extern */

void disp_lm_room_member(void *arg0) {
    u8 temp_a2;

    temp_a2 = (*(u8 *)((u8 *)(arg0) + 5));
    switch (temp_a2) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a2 + 1);
        Lb_Pl_basic_flagset(1, 0, 0);
        Lb_pl_chr_set(arg0, 0x265, 6, 0);
        return;
    case 1:
        if ((*(s32 *)((u8 *)(arg0) + 0x194)) <= 0) {
            (*(u8 *)((u8 *)(arg0) + 5)) = (u8) (temp_a2 + 1);
            return;
        }
        return;
    case 2:
        if ((Pl_master_ck(arg0) == 1) && ((*(u16 *)((u8 *)(arg0) + 0x368)) & 0x3C60)) {
            Lb_act_set(arg0, 0, 0x29, M2C_ERROR(/* Read from unset register $a3 */));
        }
        break;
    }
}

