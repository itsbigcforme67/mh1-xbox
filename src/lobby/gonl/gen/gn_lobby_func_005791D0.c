/* G online (PC): lobby_func_005791D0, written from Monster Hunter G's code at lobby 0x5791D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 tl_msg_tbl;
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */
extern s32 g_lb_636CF0;
extern s32 g_lb_636CF4;
extern s32 g_lb_636CF8;
extern s32 g_lb_636D00;
extern s32 g_lb_636D04;
extern s32 g_lb_636D08;
extern s32 g_lb_636D0C;
extern s32 g_lb_636D10;
extern s32 g_lb_636D14;
extern s32 g_lb_636D18;
extern s32 g_lb_636D1C;
extern s32 g_lb_636D20;
extern s32 g_lb_636D24;
extern s32 g_lb_636D28;
extern s32 g_lb_636D2C;

void lobby_func_005791D0(u32 arg0) {
    s64 temp_s0;

    temp_s0 = ((s16) ((((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) ? 0x60 : 0x78)));
    switch (arg0) {
    case 0:
        font_print_double(0xE0, temp_s0, 1, 5, g_lb_636CF0);
        font_print_double(0x182, temp_s0, 1, 5, g_lb_636D00);
        font_print_double(0x1FA, temp_s0, 1, 5, g_lb_636CF4);
        font_print_double(0x230, temp_s0, 1, 5, g_lb_636CF8);
        return;
    case 1:
        font_print_double(0xE2, temp_s0, 1, 5, g_lb_636D04);
        font_print_double(0x216, temp_s0, 1, 5, g_lb_636D08);
        return;
    case 2:
        font_print_double(0xE2, temp_s0, 1, 5, g_lb_636D0C);
        font_print_double(0x1B8, temp_s0, 1, 5, g_lb_636D10);
        font_print_double(0x1F4, temp_s0, 1, 5, g_lb_636D14);
        flfntSetSize(0xC, 0x12);
        font_print_double(0x22A, temp_s0, 1, 5, g_lb_636D18);
        flfntSetSize(0x12, 0x12);
        return;
    case 3:
        font_print_double(0xE2, temp_s0, 1, 5, g_lb_636CF0);
        font_print_double(0x17A, temp_s0, 1, 5, g_lb_636D00);
        font_print_double(0x1F8, temp_s0, 1, 5, g_lb_636D1C);
        return;
    case 4:
        font_print_double(0xF4, temp_s0, 1, 5, g_lb_636CF0);
        font_print_double(0x1F4, temp_s0, 1, 5, g_lb_636D00);
        return;
    case 5:
        font_print_double(0xE0, temp_s0, 1, 5, g_lb_636D20);
        font_print_double(0x118, temp_s0, 1, 5, g_lb_636D24);
        font_print_double(0x158, temp_s0, 1, 5, g_lb_636D28);
        font_print_double(0x1E4, temp_s0, 1, 5, g_lb_636D00);
        return;
    case 6:
        font_print_double(0xF4, temp_s0, 1, 5, (*(s32 *)((u8 *)&tl_msg_tbl + 0x1C)));
        font_print_double(0x202, temp_s0, 1, 5, (*(s32 *)((u8 *)&tl_msg_tbl + 0x20)));
        font_print_double(0x21D, temp_s0, 1, 5, (*(s32 *)((u8 *)&tl_msg_tbl + 0x24)));
        return;
    case 7:
        font_print_double(0xE0, temp_s0, 1, 5, g_lb_636CF0);
        font_print_double(0x15E, temp_s0, 1, 5, g_lb_636D00);
        font_print_double(0x1DA, temp_s0, 1, 5, g_lb_636CF4);
        font_print_double(0x20E, temp_s0, 1, 5, g_lb_636D2C);
        return;
    case 8:
        font_print_double(0xE0, temp_s0, 1, 5, g_lb_636D00);
        font_print_double(0x1F8, temp_s0, 1, 5, g_lb_636D1C);
        /* fallthrough */
    default:
        return;
    }
}

