/* G online (PC): lobby_func_00559E20, written from Monster Hunter G's code at lobby 0x559E20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_get_quest_type();                      /* extern */
void *lobby_func_00559C30();                        /* extern */
extern u8 lb_quest_font_color;

s16 lobby_func_00559E20(s32 arg0) {
    s32 temp_a0;
    s32 var_v0;
    u8 *temp_v0;

    temp_v0 = lobby_func_00559C30();
    temp_a0 = arg0 & 0xFFFF;
    if (temp_a0 >= 0xC350) {
        if (temp_v0 == NULL) {
            return 0xA;
        }
        if ((*(u16 *)((u8 *)(temp_v0) + 0x1E)) == temp_a0) {
            var_v0 = Lb_get_quest_type(temp_v0);
            goto block_8;
        }
        return 0xA;
    }
    var_v0 = Lb_get_quest_type(temp_v0);
block_8:
    return *(&lb_quest_font_color + (var_v0 * 2));
}

