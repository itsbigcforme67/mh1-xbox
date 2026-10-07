/* lb_pz02 - lobby.bin 0x0059C660-0x0059C74C: lb_put_comment, draws the comment bubble (two frames when f != 0) and the comment text. x/y are int params narrowed with (s16) where the original narrows. */
#pragma readonly_strings on
#include "lbui_proto.h"
void Put_comment();

void lb_put_comment(x, y, c, f)
int x;
int y;
int c;
s8 f;
{
    if (f != 0) {
        Draw_square((s16)((s16)x - 6), (s16)((s16)y - 2), 0x12C, 0x42, 0xFFAA8820);
        Draw_square((s16)((s16)x - 7), (s16)((s16)y - 3), 0x12E, 0x44, 0xFFAA8820);
    }
    KinshiYogo_chk(c);
    Put_comment(x, (s16)((s16)y - 0x16), 0x16, c);
}
