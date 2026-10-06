/* lb_menu_nm - lobby 0x005B4B20-0x005B4D2C: Disp_lb_menu, the top list of
 * the village/town start menu (Lb_ck_menu -> Lb_menu_move_Core /
 * DispLobbyMenu, lbmw = lb_menu_w).
 *
 * Written from the asm (asm/lobby/text/lm_room_member_mv.s) for the PC
 * port; not built for the PS2 and not compared with check.py. menuData is
 * the menu frame (+0xC the list's string table, +0x10 the frame colour).
 * Offline: the 8-entry list lb_menu_msg_off and its title lit_493. Online:
 * pages of 8 (lbmw+4 = cursor: page = n >> 3, entry = n & 7), a "%d" page
 * title (lit_492), a blinking cursor arrow, the player's name or id. */
#include "types.h"

int sprintf(char *, const char *, ...);
void SetTrnslMode(int, int);
f32 flSin(f32);
int Online_ck(void);
int Lbs_InRoomCheck(void);
void DispFrameList(void *a, char *b, int c);
void DispFrameListOptionArrowC(void *fr, u32 col);
void Disp_name_or_id(s16 v);

extern u8 *lbmw;
extern u16 System_timer;
extern u8 menuData[];
extern u8 lb_menu_msg[];
extern u8 lb_menu_msg_off[];
extern u8 lb_menu_msg_entry[];
extern char lit_492_0065E760[];
extern char lit_493_0065E780[];

#define MD_LIST (*(void **)(menuData + 0xC))
#define MD_COL (*(u32 *)(menuData + 0x10))

void Disp_lb_menu(s8 arg0) {
    char buf[32];
    u32 col;
    u8 n, page, sel;

    SetTrnslMode(4, 5);
    if (arg0 == 0) {
        u16 t = (u16)((System_timer & 0x3F) << 10);
        MD_COL = 0xA9182;
        col = ((u32)((s8)(s32)(48.0f * flSin(9.58738e-5f * (f32)t)) + 0xAF) << 8) | 0xF0200020;
    } else {
        MD_COL = 0x808080;
        col = 0xA0207020;
    }
    if (Online_ck() == 1) {
        n = lbmw[4];
        page = n >> 3;
        sel = n & 7;
        if (Lbs_InRoomCheck() == 0)
            MD_LIST = lb_menu_msg + page * 32;
        else
            MD_LIST = lb_menu_msg_entry + page * 32;
        sprintf(buf, lit_492_0065E760, page + 1);
        DispFrameList(menuData, buf, sel);
        DispFrameListOptionArrowC(menuData, col);
        if (arg0 == 0)
            Disp_name_or_id(0x113);
    } else {
        MD_LIST = lb_menu_msg_off;
        DispFrameList(menuData, lit_493_0065E780, lbmw[4]);
    }
}
