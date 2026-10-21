/* G online (PC): lobby_func_00542A60, written from Monster Hunter G's code at lobby 0x542A60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Event_flag_ck();
s32 Quest_clear_bit_ck();
s32 lobby_func_005426D0();
s32 lobby_func_00542790();
s32 lobby_func_00542820();
s32 lobby_func_005429A0();
s32 lobby_func_00542A30();
s32 get_questLevelNum();                     /* extern */
s32 sub_main_func_004C8600(); /* extern */
extern u8 User_data;

s32 lobby_func_00542A60(s32 arg0) {
    s32 temp_v0;

    temp_v0 = arg0 & 0xFF;
    switch (temp_v0) {                              /* irregular */
    case 0x0:
        return Event_flag_ck(4);
    case 0x1:
        return Event_flag_ck(4);
    case 0x2:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 4) {
            return lobby_func_00542790(0, 0);
        }
block_127:
    default:
        return 0;
    case 0x3:
        if ((Event_flag_ck(0x51) == 0) && (lobby_func_005426D0(0) != 0)) {
            return 1;
        }
        goto block_127;
    case 0x4:
        return Quest_clear_bit_ck(0x27);
    case 0x5:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 8) {
            return lobby_func_00542790(0, 1);
        }
        goto block_127;
    case 0x6:
        return Quest_clear_bit_ck(9);
    case 0x7:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0xC) {
            return lobby_func_00542790(0, 2);
        }
        goto block_127;
    case 0x8:
        return Quest_clear_bit_ck(0xBB9);
    case 0x9:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x10) {
            return lobby_func_00542790(0, 3);
        }
        goto block_127;
    case 0xA:
        return Quest_clear_bit_ck(0x40B);
    case 0x2F:
        return sub_main_func_004C8600(&User_data, 0xB, 0x72);
    case 0x30:
        return sub_main_func_004C8600(&User_data, 1, 0x76);
    case 0xB:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x14) {
            return lobby_func_00542790(0, 4);
        }
        goto block_127;
    case 0xC:
        return Quest_clear_bit_ck(0xBBA);
    case 0xD:
        return Event_flag_ck(0x59);
    case 0xE:
        return sub_main_func_004C8600(&User_data, 6, 0x7A);
    case 0xF:
        return sub_main_func_004C8600(&User_data, 0x14, 0x7D);
    case 0x10:
        return sub_main_func_004C8600(&User_data, 8, 0x7A);
    case 0x11:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x17) {
            return lobby_func_00542790(0, 5);
        }
        goto block_127;
    case 0x12:
        return Quest_clear_bit_ck(0x7EC);
    case 0x13:
        return sub_main_func_004C8600(&User_data, 1, 0x81);
    case 0x14:
        return sub_main_func_004C8600(&User_data, 0xF, 0x87);
    case 0x15:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x1A) {
            return lobby_func_00542790(0, 6);
        }
        goto block_127;
    case 0x16:
        return Quest_clear_bit_ck(0x802);
    case 0x17:
        return sub_main_func_004C8600(&User_data, 0xB, 0x7F);
    case 0x18:
        return sub_main_func_004C8600(&User_data, 0x15, 0x86);
    case 0x19:
        return sub_main_func_004C8600(&User_data, 0xE, 0x8B);
    case 0x1A:
        return sub_main_func_004C8600(&User_data, 0x11, 0x87);
    case 0x1B:
        if ((get_questLevelNum(0xE) == 1) && (get_questLevelNum(0xF) == 1) && (get_questLevelNum(0x10) == 1) && (get_questLevelNum(0x13) == 1) && (get_questLevelNum(0x14) == 1) && (get_questLevelNum(0x17) == 1) && (get_questLevelNum(0x18) == 1) && (get_questLevelNum(0x19) == 1) && (get_questLevelNum(0x1A) == 1) && (sub_main_func_004C8600(&User_data, 8, 0x7A) == 1) && (sub_main_func_004C8600(&User_data, 0x16, 0x81) == 1) && (sub_main_func_004C8600(&User_data, 0x1A, 0x7F) == 1)) {
            return 1;
        }
        goto block_127;
    case 0x1C:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x1C) {
            return lobby_func_00542790(0, 7);
        }
        goto block_127;
    case 0x1D:
        if (Event_flag_ck(0x1F) == 0) {
            return Quest_clear_bit_ck(0xBBB);
        }
        goto block_127;
    case 0x1E:
        if ((s32) (*(u8 *)((u8 *)&User_data + 0xC23)) >= 0x1E) {
            return Event_flag_ck(0x1F);
        }
        goto block_127;
    case 0x1F:
        if ((Quest_clear_bit_ck(0xBBC) == 1) && (Quest_clear_bit_ck(0xBBD) == 1) && (Quest_clear_bit_ck(0xBBE) == 1) && (Quest_clear_bit_ck(0xBBF) == 1)) {
            return 1;
        }
        goto block_127;
    case 0x21:
        if (Event_flag_ck(0x5D) == 0) {
            return lobby_func_00542820();
        }
        goto block_127;
    case 0x22:
        return Quest_clear_bit_ck(0xBBB);
    case 0x3B:
        if (Event_flag_ck(0x5A) == 0) {
            return lobby_func_005429A0();
        }
        goto block_127;
    case 0x3C:
        if (Event_flag_ck(0x5B) == 0) {
            return lobby_func_00542A30();
        }
        goto block_127;
    }
}

