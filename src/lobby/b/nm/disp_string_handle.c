#include "lobby_a.h"
#define FLOC(x, y) ((void (*)(int, s16))flfntLocate)(x, y)
#define FPD(a, b, c, d, e) ((void (*)(int, s16, int, s8, char *))font_print_double)(a, b, c, d, e)
extern char lit_312_0065EC30[];
void font_print_double();
extern char **net_etc_mes_tbl[];
void cnWrap_SetFontSize(f32);
void disp_string_handle(a, b)
int a;
int b;
{
    char buf[0x20];
    u8 *e;
    int idx;
    int y;
    s8 sel;

    memset(buf, 0, 0x1E);
    e = (u8 *)cw + (b & 0xFF) * 0x11;
    if (*(s8 *)(e + 0x2B) == 0) {
        strcpy(buf, *net_etc_mes_tbl[8]);
    } else {
        strcpy(buf, e + 0x2B);
    }
    cnWrap_SetFontSize(20.0f);
    idx = (u8)b;
    if ((a & 0xFF) == idx) {
        sel = 4;
    } else {
        sel = 0;
    }
    flfntSetSize(0x18, 0x14);
    y = idx * 0x58 + 0x62;
    FPD(0x1A4, y, 1, sel, buf);
    flfntSetSize(0x14, 0x14);
    FLOC(0x168, y);
    font_print(&lit_312_0065EC30);
}
