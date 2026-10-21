/* G online (PC): Lb_Pl_stg_ck, written from Monster Hunter G's code at lobby 0x568420 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 memcmp();
extern u8 cw;
extern u8 game_w;
s32 Online_ck();                                    /* extern */
extern s32 lbCommer;

s32 Lb_Pl_stg_ck(void *arg0) {
    s32 temp_a0;
    u8 temp_v1;

    if ((*(u8 *)((u8 *)(arg0) + 0x736)) != (*(u8 *)((u8 *)&game_w + 0x14))) {
        return 0;
    }
    if (Online_ck() == 0) {
        return 1;
    }
    temp_a0 = (*(u16 *)((u8 *)(arg0) + 0xC)) * 0x5C;
    if (memcmp((*(s32 *)&cw) + temp_a0 + 0x38C, (u8 *)&lbCommer + temp_a0, 8) != 0) {
        return 0;
    }
    if ((*(u16 *)((u8 *)(arg0) + 0xC)) != (*(u8 *)((u8 *)&game_w + 0x3A))) {
        temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x736));
        switch (temp_v1) {                          /* irregular */
        case 0x4D:
            /* fallthrough */
        case 0x4C:
            return 1;
        default:
            return 0;
        }
    } else {
        return 1;
    }
}

