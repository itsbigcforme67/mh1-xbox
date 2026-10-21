/* G online (PC): DispSceneSubTitle, written from Monster Hunter G's code at lobby 0x5A0BE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntSetSize();
s32 font_print_double();
extern u8 g_main_2EFAE8;
extern u8 cw;
extern u8 g_main_2EFAE8;
extern u8 game_w;
s32 Draw_menu_square(); /* extern */
extern s32 g_lb_654E00;

void DispSceneSubTitle(void) {
    u8 *temp_v1;

    flfntSetSize(0x12, 0x12);
    temp_v1 = (*(void **)&cw);
    if (((*(u8 *)((u8 *)(temp_v1) + 0xFA61)) != 0) && ((*(s8 *)((u8 *)(((*(u8 *)((u8 *)&game_w + 0x3A)) + temp_v1)) + 0xB64)) != 0)) {
        Draw_menu_square(0xD4, 0x30, 0xC0, 0x20, 0, 0xA0000000);
        font_print_double(0xE1, 0x38, 1, 0, &g_lb_654E00);
        return;
    }
    Draw_menu_square(0xD4, 0x44, 0xC0, 0x20, 1, (*(s32 *)&g_main_2EFAE8));
    font_print_double(0xE1, 0x4C, 1, 0, &g_lb_654E00);
}

