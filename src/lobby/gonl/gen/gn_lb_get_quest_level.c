/* G online (PC): lb_get_quest_level, written from Monster Hunter G's code at lobby 0x55B430 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
s32 Online_ck();                                    /* extern */
s32 Quest_clear_bit_ck();                    /* extern */

s32 lb_get_quest_level(s64 arg0) {
    s32 var_v0;
    s32 var_v0_2;
    u8 temp_v0;

    if (Online_ck() == 1) {
        temp_v0 = (*(u8 *)((u8 *)&User_data + 0xC23));
        if ((s32) temp_v0 < 5) {
            return 0;
        }
        if ((s32) temp_v0 < 9) {
            return 1;
        }
        if ((s32) temp_v0 < 0xD) {
            return 2;
        }
        if ((s32) temp_v0 < 0x11) {
            return 3;
        }
        if ((s32) temp_v0 < 0x15) {
            return 4;
        }
        if ((s32) temp_v0 < 0x18) {
            return 5;
        }
        if ((s32) temp_v0 < 0x1B) {
            return 6;
        }
        if ((s32) temp_v0 < 0x1F) {
            var_v0_2 = 7;
        } else {
            var_v0_2 = 0;
        }
        return var_v0_2;
    }
    if (Quest_clear_bit_ck(0x2739) == 1) {
        var_v0 = 4;
        if (((s64) (arg0 << 0x38) >> 0x38) == 1) {
            var_v0 = 5;
        }
        return var_v0;
    }
    if (Quest_clear_bit_ck(0x2730) == 1) {
        return 4;
    }
    if (Quest_clear_bit_ck(0x2726) == 1) {
        return 3;
    }
    if (Quest_clear_bit_ck(0x271E) == 1) {
        return 2;
    }
    if (Quest_clear_bit_ck(0x2716) == 1) {
        return 1;
    }
    return -(Quest_clear_bit_ck(0x2711) != 1);
}

