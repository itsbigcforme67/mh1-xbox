/* G online (PC): Lb_player_release, written from Monster Hunter G's code at lobby 0x566F00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 armor_model_free();
s32 release_prim();
extern u8 game_w;
s32 flCompact();                              /* extern */
s32 func_00114AC0();                            /* extern */

void Lb_player_release(void *arg0) {
    u8 temp_a0;

    (*(s8 *)((u8 *)(arg0) + 1)) = 0;
    (*(s8 *)((u8 *)(arg0) + 0)) = 0;
    func_00114AC0();
    armor_model_free(arg0);
    if ((*(s32 *)((u8 *)(arg0) + 0x564)) != 0) {
        release_prim((s32) (*(s16 *)((u8 *)(arg0) + 0x568)));
        (*(s32 *)((u8 *)(arg0) + 0x564)) = 0;
    }
    temp_a0 = (*(u8 *)((u8 *)&game_w + 0x14));
    if (temp_a0 != 0x4C) {
        if (temp_a0 == 0x4D) {
            goto block_5;
        }
    } else {
block_5:
        flCompact(temp_a0);
    }
}

