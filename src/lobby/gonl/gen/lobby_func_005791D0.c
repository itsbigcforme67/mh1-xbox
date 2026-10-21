/* G online (PC): lobby_func_005791D0, written from Monster Hunter G's code at lobby 0x5791D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */

void lobby_func_005791D0(u32 arg0) {
    s64 temp_s0;

    temp_s0 = (s64) ((((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) ? 0x60 : 0x78) << 0x30) >> 0x30;
    switch (arg0) {
    case 0:
        font_print_double(0xE0, temp_s0, 1, 5);
        font_print_double(0x182, temp_s0, 1, 5);
        font_print_double(0x1FA, temp_s0, 1, 5);
        font_print_double(0x230, temp_s0, 1, 5);
        return;
    case 1:
        font_print_double(0xE2, temp_s0, 1, 5);
        font_print_double(0x216, temp_s0, 1, 5);
        return;
    case 2:
        font_print_double(0xE2, temp_s0, 1, 5);
        font_print_double(0x1B8, temp_s0, 1, 5);
        font_print_double(0x1F4, temp_s0, 1, 5);
        flfntSetSize(0xC, 0x12);
        font_print_double(0x22A, temp_s0, 1, 5);
        flfntSetSize(0x12, 0x12);
        return;
    case 3:
        font_print_double(0xE2, temp_s0, 1, 5);
        font_print_double(0x17A, temp_s0, 1, 5);
        font_print_double(0x1F8, temp_s0, 1, 5);
        return;
    case 4:
        font_print_double(0xF4, temp_s0, 1, 5);
        font_print_double(0x1F4, temp_s0, 1, 5);
        return;
    case 5:
        font_print_double(0xE0, temp_s0, 1, 5);
        font_print_double(0x118, temp_s0, 1, 5);
        font_print_double(0x158, temp_s0, 1, 5);
        font_print_double(0x1E4, temp_s0, 1, 5);
        return;
    case 6:
        font_print_double(0xF4, temp_s0, 1, 5);
        font_print_double(0x202, temp_s0, 1, 5);
        font_print_double(0x21D, temp_s0, 1, 5);
        return;
    case 7:
        font_print_double(0xE0, temp_s0, 1, 5);
        font_print_double(0x15E, temp_s0, 1, 5);
        font_print_double(0x1DA, temp_s0, 1, 5);
        font_print_double(0x20E, temp_s0, 1, 5);
        return;
    case 8:
        font_print_double(0xE0, temp_s0, 1, 5);
        font_print_double(0x1F8, temp_s0, 1, 5);
        /* fallthrough */
    default:
        return;
    }
}

