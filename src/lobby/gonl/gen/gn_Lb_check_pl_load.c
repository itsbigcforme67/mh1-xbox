/* G online (PC): Lb_check_pl_load, written from Monster Hunter G's code at lobby 0x5665A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 player_work;

s32 Lb_check_pl_load(s32 arg0) {
    s32 temp_a0;

    temp_a0 = ((s8) (arg0));
    if ((*(&player_work + (temp_a0 * 0xA00)) != 0) && ((*(s8 *)((u8 *)((temp_a0 + (*(s32 *)&cw))) + 0xB64)) == 0)) {
        return 0;
    }
    return 1;
}

