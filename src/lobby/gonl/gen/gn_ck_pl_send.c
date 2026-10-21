/* G online (PC): ck_pl_send, written from Monster Hunter G's code at lobby 0x567550 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 game_w;
extern u8 player_work;

s32 ck_pl_send(void *arg0) {
    u8 *var_a1;   /* (s32 *) */
    s64 var_a2;
    u8 temp_v1;

    var_a2 = 0;
    var_a1 = &player_work;
    do {
        (*(s16 *)((u8 *)(var_a1) + 0xC)) = (s16) ((s64) (var_a2 << 0x38) >> 0x38);
        (*(s16 *)((u8 *)(var_a1) + 0xA0C)) = (s16) ((s64) ((var_a2 + 1) << 0x38) >> 0x38);
        (*(s16 *)((u8 *)(var_a1) + 0x140C)) = (s16) ((s64) ((var_a2 + 2) << 0x38) >> 0x38);
        (*(s16 *)((u8 *)(var_a1) + 0x1E0C)) = (s16) ((s64) ((var_a2 + 3) << 0x38) >> 0x38);
        (*(s16 *)((u8 *)(var_a1) + 0x280C)) = (s16) ((s64) ((var_a2 + 4) << 0x38) >> 0x38);
        var_a2 = (s64) ((var_a2 + 5) << 0x38) >> 0x38;
        var_a1 += 0x3200;
    } while (var_a2 < 0xA);
    if (((*(u16 *)((u8 *)(arg0) + 0xC)) == (*(u8 *)((u8 *)&game_w + 0x3A))) && ((*(u8 *)((u8 *)(arg0) + 0x1E)) == 0)) {
        temp_v1 = (*(u8 *)((u8 *)&game_w + 0x14));
        if ((temp_v1 != 0x4C) && (temp_v1 != 0x4D)) {
            goto block_9;
        }
        return 1;
    }
block_9:
    return 0;
}

