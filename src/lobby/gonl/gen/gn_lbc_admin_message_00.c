/* G online (PC): lbc_admin_message_00, written from Monster Hunter G's code at lobby 0x584E90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Info_Initialization();
s32 fade_set();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 font_set_stack_no();
s32 lobby_func_00584E10();
extern u8 cw;
s32 font_print();                 /* extern */
extern s32 lit_3323;

void lbc_admin_message_00(void) {
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xED7));
    switch (temp_a0) {                              /* irregular */
    case 0:
        if (Fade_busy_ck() & 0xFF) {
            fade_set(2);
        }
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xED7)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xED7)) + 1);
        (*(s16 *)((u8 *)((*(void **)&cw)) + 0xEDC)) = 0x12C;
        Info_Initialization();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBC4)) = 2;
        *(s32 (**)(s32, s32, s32, s32))0x355944 = lobby_func_00584E10;
        /* fallthrough */
    case 1:
        temp_v1 = (*(void **)&cw);
        (*(s16 *)((u8 *)(temp_v1) + 0xEDC)) = (s16) ((*(s16 *)((u8 *)(temp_v1) + 0xEDC)) - 1);
        temp_v1_2 = (*(void **)&cw);
        if ((*(s16 *)((u8 *)(temp_v1_2) + 0xEDC)) == 0) {
            (*(u8 *)((u8 *)(temp_v1_2) + 0xED6)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xED6)) + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xED7)) = 0U;
            (*(s16 *)((u8 *)((*(void **)&cw)) + 0xEDC)) = 0;
        }
        font_set_stack_no(5);
        font_set_palette(0);
        flfntSetSize(0x14, 0x14);
        flfntLocate(0x1CC, 0x50);
        temp_v1_3 = (*(void **)&cw);
        font_print(&lit_3323, ((*(s16 *)((u8 *)(temp_v1_3) + 0xEDC)) / 30) + ((u32) (*(s16 *)((u8 *)(temp_v1_3) + 0xEDC)) >> 0x1F));
        return;
    default:
        (*(u8 *)((u8 *)(temp_a1) + 0xED6)) = 2U;
        return;
    }
}

