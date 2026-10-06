/* lb_dsi01 - agent C 0x005C2280-0x005C2370: disp_string_id (handle-name select list: ID line; (s8)cw[0xB+n*8] test, han2zen). */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void flfntLocate(int x, s16 y);
void font_print_double(int, s16, int, s8, char *);
extern char lit_273_0065EC20[];
extern char lit_274_0065EC28[];
void disp_string_id(a, b)
u8 a;
u8 b;
{
    char buf[0x1E];
    u8 *e;
    int idx;
    int y;
    s8 sel;

    memset(buf, 0, 0x1E);
    if ((s8)cw[0xB + b * 8] == 0) {
        strcpy(buf, lit_273_0065EC20);
    } else {
        han2zen(cw + 0xB + b * 8, buf);
    }
    flfntSetSize(0x14, 0x14);
    idx = b;
    sel = (a != idx) ? 0 : 4;
    y = idx * 0x58 + 0x7A;
    font_print_double(0x1A4, y, 1, (s8)sel, buf);
    flfntLocate(0x168, y);
    font_print(lit_274_0065EC28);
}
