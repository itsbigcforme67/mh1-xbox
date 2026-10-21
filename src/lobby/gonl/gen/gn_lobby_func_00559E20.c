/* G online (PC): lobby_func_00559E20, written from Monster Hunter G's code at lobby 0x559E20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559C30();
s32 Lb_get_quest_type();                         /* extern */
extern u16 lb_quest_font_color;

s16 lobby_func_00559E20(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = lobby_func_00559C30(arg0, arg1, arg2, arg3);
    temp_a0 = arg0 & 0xFFFF;
    if (temp_a0 >= 0xC350) {
        if (temp_v0 == 0) {
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
    return (*(u16 *)((u8 *)&lb_quest_font_color + (var_v0 * 2)));
}

