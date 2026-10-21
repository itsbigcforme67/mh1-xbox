/* G online (PC): lobby_func_00543180, written from Monster Hunter G's code at lobby 0x543180 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Event_flag_ck();
s32 Quest_clear_bit_ck();
s32 lobby_func_00542790();
s32 lobby_func_005428F0();
s32 lobby_func_005429A0();
s32 lobby_func_00542A30();
s32 get_questLevelNum();                     /* extern */
s32 sub_main_func_004C8600(); /* extern */
s32 sub_main_func_004C8A10();     /* extern */
s32 sub_main_func_004C9410();                       /* extern */
extern u8 User_data;

s32 lobby_func_00543180(s32 arg0) {
    s32 temp_v0;

    temp_v0 = arg0 & 0xFF;
    switch (temp_v0) {
    case 0x23:
        return Event_flag_ck(2);
    case 0x24:
        return Event_flag_ck(0);
    case 0x25:
        return Quest_clear_bit_ck(0x2711);
    case 0x26:
        return lobby_func_00542790(1, 0);
    case 0x27:
        return Quest_clear_bit_ck(0x2716);
    case 0x28:
        return lobby_func_00542790(1, 1);
    case 0x29:
        return Quest_clear_bit_ck(0x271E);
    case 0x2A:
        return lobby_func_00542790(1, 2);
    case 0x2B:
        return Quest_clear_bit_ck(0x2726);
    case 0x2C:
        return lobby_func_00542790(1, 3);
    case 0x2D:
        return Quest_clear_bit_ck(0x2730);
    case 0x2E:
        return Event_flag_ck(1);
    case 0x2F:
        return sub_main_func_004C8600(&User_data, 0xB, 0x72);
    case 0x30:
        return sub_main_func_004C8600(&User_data, 1, 0x76);
    case 0x31:
        return lobby_func_00542790(1, 4);
    case 0x32:
        return Quest_clear_bit_ck(0x2739);
    case 0x33:
        return sub_main_func_004C8600(&User_data, 0x1A, 0x74);
    case 0x34:
        if (get_questLevelNum(0x32) == 1) {
            return sub_main_func_004C8A10(&User_data, 0);
        }
    default:
block_50:
        return 0;
    case 0x35:
        if (get_questLevelNum(0x32) == 1) {
            return sub_main_func_004C8A10(&User_data, 1);
        }
        goto block_50;
    case 0x36:
        if (get_questLevelNum(0x32) == 1) {
            return sub_main_func_004C8A10(&User_data, 2);
        }
        goto block_50;
    case 0x37:
        if ((Quest_clear_bit_ck(0x273E) == 1) && (Quest_clear_bit_ck(0x273D) == 1) && (Quest_clear_bit_ck(0x273B) == 1)) {
            return 1;
        }
        goto block_50;
    case 0x38:
        return Quest_clear_bit_ck(0x273C);
    case 0x39:
        if ((lobby_func_005428F0() == 1) && (sub_main_func_004C9410() == 1)) {
            return 1;
        }
        goto block_50;
    case 0x3A:
        if (Event_flag_ck(0x52) == 0) {
            return Quest_clear_bit_ck(0x2740);
        }
        goto block_50;
    case 0x3B:
        if (Event_flag_ck(0x5A) == 0) {
            return lobby_func_005429A0();
        }
        goto block_50;
    case 0x3C:
        if (Event_flag_ck(0x5B) == 0) {
            return lobby_func_00542A30();
        }
        goto block_50;
    }
}

