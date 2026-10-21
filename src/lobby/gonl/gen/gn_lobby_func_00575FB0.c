/* G online (PC): lobby_func_00575FB0, written from Monster Hunter G's code at lobby 0x575FB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 tl_member_buff;
extern u8 tl_msg_tbl;
s32 Lb_put_msg_type2(); /* extern */
s32 Put_page_num(); /* extern */
s32 flfntLocate();                  /* extern */
s32 font_print_double(); /* extern */
s32 font_print_uf();                        /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                        /* extern */
s32 lobby_func_005799F0(); /* extern */
s32 lobby_func_005935C0();    /* extern */
s32 put_main_cursor();                       /* extern */
extern u8 g_lb_6B46D1;
extern u8 g_lb_6B4759;
extern s8 g_lb_6B475C;
extern s8 seekStr;

void lobby_func_00575FB0(void) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s8 *)(g_frame + 0x20))

    if ((*(u8 *)((u8 *)&tl_member_buff + 0x1383)) == 2) {
        put_main_cursor(7);
    } else if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
        lobby_func_005799F0(0xD8, 0x50, 1, 0xFF051F05, 0xFF32563C);
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 0) {
            put_main_cursor(1);
        } else {
            put_main_cursor(7);
        }
    } else {
        lobby_func_005799F0(0xD8, 0x50, 3, 0xFF051F05, 0xFF32563C);
        lobby_func_005799F0(0xD8, 0x50, 5, 0xFF051F05, 0xFF32563C);
        lobby_func_005799F0(0xD8, 0x50, 1, 0xFF051F05, 0xFF32563C);
        put_main_cursor(((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 2) + 1);
    }
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
        font_print_double(0xE2, 0x78, 1, 5, (*(s32 *)((u8 *)&tl_msg_tbl + 0x40)));
        font_set_palette(0);
        Lb_put_msg_type2(0xE2, 0xA4, 0);
    } else {
        font_print_double(0xE2, 0x78, 1, 5, (*(s32 *)((u8 *)&tl_msg_tbl + 0x50)));
        font_set_palette(0);
        Lb_put_msg_type2(0xE2, 0xA4, 4);
    }
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 0) {
        han2zen(&seekStr, &sp20);
        flfntLocate(lobby_func_005935C0(&sp20, 0x1B0, 0x12), 0xA4);
        font_print_uf(&sp20);
        Lb_put_msg_type2(0xE2, 0xD0, 4);
        han2zen(&(*(s8 *)((u8 *)&seekStr + 0x44)), &sp20);
        flfntLocate(lobby_func_005935C0(&sp20, 0x1B0, 0x12), 0xD0);
        font_print_uf(&sp20);
        Lb_put_msg_type2(0xE2, 0xFC, 4);
        han2zen(&g_lb_6B475C, &sp20);
        flfntLocate(lobby_func_005935C0(&sp20, 0x1B0, 0x12), 0xFC);
        font_print_uf(&sp20);
    } else {
        flfntLocate(lobby_func_005935C0(&seekStr, 0x1B0, 0x12), 0xA4);
        font_print_uf(&seekStr);
    }
    Put_page_num(0x208, 0x78, (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 2, 0);
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 2) {
        font_set_palette(0xA);
        if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 0) {
            if ((g_lb_6B46D1 == 6) || ((*(u8 *)((u8 *)&seekStr + 0x41)) == 6) || (g_lb_6B4759 == 6)) {
                font_set_palette(0);
            }
        } else if (seekStr != 0) {
            font_set_palette(0);
        }
    } else {
        font_set_palette(0);
    }
    Lb_put_msg_type2(0xE2, 0x128, 0x41);
}

