/* G online (PC): DispNameAndIDonDialog, written from Monster Hunter G's code at lobby 0x5A1A00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Draw_square(); /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */
extern u8 lit_226_0065C500;
extern u8 lit_227_0065C508;

void DispNameAndIDonDialog(s64 arg0, s32 *arg1, s32 *arg2) {
    s64 temp_s0;
    s64 temp_s1;
    u8 *temp_a0;

    temp_a0 = (*(void **)&cw);
    if (((*(s8 *)((u8 *)(temp_a0) + 0xB71)) == 0) && ((*(s8 *)((u8 *)(temp_a0) + 0xB72)) == 0)) {
        if ((*(s8 *)((u8 *)(temp_a0) + 0xB73)) != 0) {

        } else if ((*(u8 *)((u8 *)(temp_a0) + 0xB99)) != 5) {
            flfntSetSize(0x1C, 0x14);
            font_print_double(0xEE, arg0, 1, 4, arg1);
            temp_s1 = ((s16) (arg0));
            temp_s0 = temp_s1 + 0x1E;
            font_print_double(0xEE, ((s16) (temp_s0)), 1, 4, arg2);
            flfntSetSize(0x14, 0x14);
            font_print_double(0xB2, arg0, 1, 4, &lit_226_0065C500);
            font_print_double(0xB2, ((s16) (temp_s0)), 1, 4, &lit_227_0065C508);
            Draw_square(0xB2, ((s16) ((temp_s1 + 0x16))), 0x11C, 1, -1);
            Draw_square(0xB2, ((s16) ((temp_s1 + 0x34))), 0x11C, 1, -1);
        }
    }
}

