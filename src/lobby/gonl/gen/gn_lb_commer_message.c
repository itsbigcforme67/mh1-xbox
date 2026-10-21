/* G online (PC): lb_commer_message, written from Monster Hunter G's code at lobby 0x558E70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_chat();
extern u8 cw;
extern u8 lbCommer;

void lb_commer_message(s64 arg0) {
    s32 temp_a1;

    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        temp_a1 = (((s8) (arg0))) * 0x5C;
        if ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lbCommer + 0x8)) + temp_a1)) != 0) {
            Lb_put_chat(3, (s32) ((u8 *)&lbCommer + temp_a1 + 8));
        }
    }
}

