/* G online (PC): get_font_col, written from Monster Hunter G's code at lobby 0x586980 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 _ctype_;
extern u8 color_tbl;

u8 get_font_col(s32 arg0) {
    s32 temp_v1;
    u8 var_a0;

    temp_v1 = arg0 & 0xFF;
    if (temp_v1 != 0) {
        if (!((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&_ctype_ + 0x1)) + temp_v1)) & 4)) {
            goto block_3;
        }
        var_a0 = (*(s32 *)((u8 *)&color_tbl + ((temp_v1 - 0x30) & 7)));
    } else {
block_3:
        var_a0 = 0xFF;
    }
    return var_a0;
}

