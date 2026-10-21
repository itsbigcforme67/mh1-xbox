/* G online (PC): put_mainWindowTex, written from Monster Hunter G's code at lobby 0x573CF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Draw_menu_square();    /* extern */
s32 Draw_square();    /* extern */

void put_mainWindowTex(s64 arg0, s64 arg1) {
    s64 temp_s1;

    Draw_menu_square((s64) ((((s64) (arg0 << 0x30) >> 0x30) - 6) << 0x30) >> 0x30, 0x1A0, 0x110);
    temp_s1 = (s64) (arg1 << 0x30) >> 0x30;
    Draw_square(arg0, (s64) ((temp_s1 + 0x26) << 0x30) >> 0x30, 0x192, 1);
    Draw_square(arg0, (s64) ((temp_s1 + 0xEC) << 0x30) >> 0x30, 0x192, 1);
}

