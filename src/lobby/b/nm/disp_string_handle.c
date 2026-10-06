#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void flfntLocate(int x, s16 y);
void font_print_double(int, s16, int, s8, char *);
extern char lit_312_0065EC30[];
extern char **net_etc_mes_tbl[];
void cnWrap_SetFontSize(f32);
void disp_string_handle(a, b)
u8 a;
u8 b;
{
    char buf[0x20];
    u8 *e;
    int idx;
    int y;
    s8 sel;

    memset(buf, 0, 0x1E);
    if ((s8)cw[0x2B + b * 0x11] == 0) {
        strcpy(buf, *net_etc_mes_tbl[8]);
    } else {
        strcpy(buf, cw + 0x2B + b * 0x11);
    }
    cnWrap_SetFontSize(20.0f);
    idx = b;
    sel = (a != idx) ? 0 : 4;
    flfntSetSize(0x18, 0x14);
    y = idx * 0x58 + 0x62;
    font_print_double(0x1A4, y, 1, sel, buf);
    flfntSetSize(0x14, 0x14);
    flfntLocate(0x168, y);
    font_print(&lit_312_0065EC30);
}
