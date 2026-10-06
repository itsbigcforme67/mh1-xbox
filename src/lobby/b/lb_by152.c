/* lb_by152 - agent B 0x005B4B20-0x005B4D2C: Disp_lb_menu (start menu frame: page list online, plain list offline; the page / selection come from a signed int n so n % 8 keeps the sign fix-up). */
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
    int n;
    u8 page, sel;

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
        page = (u32)n >> 3;
        sel = n % 8;
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
