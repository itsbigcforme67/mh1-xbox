/* lb_by41 - agent B promoted near-match 0x005931B0-0x00593360: put_button_help (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { s16 x; s16 y; s16 tx; s16 ty; } BHPOS;
extern BHPOS bh_pos[];
extern char lit_1085_0065DA48[];
extern char *strButtonHelp[];
typedef struct { u8 pad0000[0x2BFE]; s8 x2BFE[8]; u8 pad2C06[0x35D5 - 0x2C06]; u8 x35D5; } CWS_put_button_help;
#define BHCW ((CWS_put_button_help *)cw)

void put_button_help(id, txt, btn, on)
s8 id;
s8 txt;
int btn;
s32 on;
{
    char buf[0x40];
    int tx;
    int ty;
    s32 off;
    int x;
    int y;

    off = 0;
    if (on & 0xFFFF) {
        off = 2;
    }
    x = bh_pos[id].x;
    y = bh_pos[id].y;
    if (BHCW->x35D5 != 0 && ((s8 *)(*(u8 *)0x3F34C1 + (int)cw))[0x2BFE] != 0) {
        y = (s16)(y - 0x18);
    }
    if (btn == 9) {
        x = (s16)(x - 0x1E);
        y = (s16)(y - 2);
        flfntLocate((s16)(x + 3), (s16)(y + 6));
        font_print(lit_1085_0065DA48);
    }
    Lb_put_button(x, (s16)((s16)y + (s16)off), btn);
    tx = bh_pos[id].tx;
    ty = bh_pos[id].ty;
    if (BHCW->x35D5 != 0 && ((s8 *)(*(u8 *)0x3F34C1 + (int)cw))[0x2BFE] != 0) {
        ty = (s16)(ty - 0x18);
    }
    strcpy(buf, strButtonHelp[txt]);
    font_print_double(tx, ty, 1, 0, buf);
}
