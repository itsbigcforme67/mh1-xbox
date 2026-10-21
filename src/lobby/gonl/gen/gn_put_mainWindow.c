/* G online (PC): put_mainWindow, written from Monster Hunter G's code at lobby 0x573C40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Draw_menu_square(); /* extern */
s32 Draw_square(); /* extern */

void put_mainWindow(s64 arg0, s64 arg1) {
    s64 temp_s1;

    Draw_menu_square(((s16) (((((s16) (arg0))) - 6))), 0x1A0, 0x110, 1, 0xB000240A);
    temp_s1 = ((s16) (arg1));
    Draw_square(arg0, ((s16) ((temp_s1 + 0x26))), 0x192, 1, 0xFF537F56);
    Draw_square(arg0, ((s16) ((temp_s1 + 0xEC))), 0x192, 1, 0xFF537F56);
}

