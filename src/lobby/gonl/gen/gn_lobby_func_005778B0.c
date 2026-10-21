/* G online (PC): lobby_func_005778B0, written from Monster Hunter G's code at lobby 0x5778B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 font_set_palette();
s32 lobby_func_005775C0();
s32 lobby_func_00578A90();
s32 put_mail_input_square();
extern u8 g_main_2EFBAC;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 tl_msg_tbl;
s32 font_print();                 /* extern */
s32 put_titles();          /* extern */
extern u8 lit_2316;

void lobby_func_005778B0(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s1;
    u8 temp_v1;
    u8 *temp_a3;
    u8 *temp_v1_2;

    temp_a3 = (*(void **)&g_main_2EFBAC);
    temp_v1 = (*(u8 *)((u8 *)(temp_a3) + 0x233));
    switch (temp_v1) {
    case 0:
    case 5:
    case 7:
        temp_s0 = ((s16) (arg1));
        temp_s1 = ((s16) (arg0));
        put_titles(((s16) ((temp_s1 + 0xA))), ((s16) ((temp_s0 + 0x28))), (*(s32 *)((u8 *)&tl_msg_tbl + 0x10)), temp_a3);
        temp_s0_2 = ((s16) ((temp_s0 + 0x28)));
        lobby_func_00578A90(((s16) ((temp_s1 + 0xA))), (s32) (((s16) ((temp_s0_2 + 0x16)))));
        put_mail_input_square(((s16) ((temp_s1 + 0xA))), ((s16) ((temp_s0_2 - 0x16))));
        if (((*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) != 0) && (temp_v1_2 = (*(void **)&g_main_2EFBAC), (((*(u8 *)((u8 *)(temp_v1_2) + 0x23E)) + ((*(u8 *)((u8 *)(temp_v1_2) + 0x23C)) + (*(u8 *)((u8 *)(temp_v1_2) + 0x23D)))) != 3))) {
            font_set_palette(0);
        } else {
            font_set_palette(0xA);
        }
        flfntLocate((s32) (((s16) ((temp_s1 + 0xA)))), (s32) (((s16) ((temp_s0_2 + 0xB0)))));
        font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x14)));
        return;
    case 2:
    case 3:
    case 4:
        lobby_func_005775C0(arg0, arg1, (s32) (*(s8 *)((u8 *)(temp_a3) + 0x238)), (s32) (*(s16 *)((u8 *)(temp_a3) + 0x22E)));
        /* fallthrough */
    default:
        return;
    }
}

