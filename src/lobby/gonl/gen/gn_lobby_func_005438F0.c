/* G online (PC): lobby_func_005438F0, written from Monster Hunter G's code at lobby 0x5438F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();
s32 lobby_func_005437F0();
s32 get_questLevelNum();                     /* extern */

s32 lobby_func_005438F0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_v0;

    if (lobby_func_005437F0(arg0, arg1, arg2, arg3) == 1) {
        return 1;
    }
    temp_v0 = arg0 & 0xFFFF;
    switch (temp_v0) {                              /* irregular */
    case 0x271B:
        return Quest_clear_bit_ck(0x271F) != 1;
    case 0x2731:
    case 0x3FD:
        return get_questLevelNum(0x2F) != 1;
    case 0x2738:
    case 0x2736:
    case 0x409:
        return get_questLevelNum(0x30) != 1;
    case 0x7DD:
    case 0x7D4:
    case 0x7D2:
        return get_questLevelNum(0xE) != 1;
    case 0x7DE:
    case 0x7DA:
    case 0x7D8:
    case 0x7D6:
        return get_questLevelNum(0xF) != 1;
    case 0x7F0:
    case 0x7E5:
    case 0x7E3:
    case 0x7E1:
        return get_questLevelNum(0x13) != 1;
    case 0x7ED:
    case 0x7E9:
    case 0x7E7:
        return get_questLevelNum(0x14) != 1;
    case 0x7F6:
    case 0x7F3:
        return get_questLevelNum(0x17) != 1;
    case 0x7FF:
    case 0x7F8:
        return get_questLevelNum(0x18) != 1;
    case 0x7FD:
    case 0x7FA:
        return get_questLevelNum(0x19) != 1;
    case 0x7FE:
    case 0x7FC:
        return get_questLevelNum(0x1A) != 1;
    case 0x801:
    case 0x7F4:
    case 0x7F1:
        return get_questLevelNum(0x1B) != 1;
    default:
        return 2;
    }
}

