/* G online (PC): lobby_func_005A0E80, written from Monster Hunter G's code at lobby 0x5A0E80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 System_timer;
extern u8 cw;
extern u8 game_w;
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s64 lobby_func_005935C0();          /* extern */

void lobby_func_005A0E80(s32 arg0) {
    s64 temp_s0;
    u8 *temp_v1;

    if (((s32) (*(u16 *)&System_timer) % 40) < 0x15) {
        temp_s0 = ((s16) (lobby_func_005935C0(0x1A0, 0x12)));
        flfntSetSize(0x14, 0x14);
        font_set_palette(0);
        temp_v1 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_v1) + 0xFA61)) != 0) && ((*(s8 *)((u8 *)(((*(u8 *)((u8 *)&game_w + 0x3A)) + temp_v1)) + 0xB64)) != 0)) {
            font_print_double(temp_s0, 0x12B, 1, 4, arg0);
            return;
        }
        font_print_double(temp_s0, 0x143, 1, 4, arg0);
    }
}

