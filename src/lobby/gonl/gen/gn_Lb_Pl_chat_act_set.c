/* G online (PC): Lb_Pl_chat_act_set, written from Monster Hunter G's code at lobby 0x569050 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_Pl_act_set();        /* extern */
extern u8 chat_act_tbl_0064E198;

void Lb_Pl_chat_act_set(void *arg0) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x8C4));
    if (temp_v1 != 0) {
        Lb_Pl_act_set(1, *(&chat_act_tbl_0064E198 + temp_v1), 0);
    }
}

