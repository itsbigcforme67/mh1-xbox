/* lb_dd10 - browser: Disp_TABLE_Line 0x00607C20-0x00607CA0 (draws a table outline). The `& 0xFFFFFFFF` on the colour argument (found by the permuter) fixes the load order. */
#include "lobby_f.h"
extern u8 *bsw;
u8 *pullTableImage();
void stockTableOutline();
void Disp_TABLE_Line(u8 *o) {
    u8 *w = bsw;
    if (*(s8 *)(w + 0x186) == 0 && *(w + 0xE96B) == 0) {
        u8 *img = pullTableImage();
        u16 y = *(u16 *)(o + 0x2A);
        u16 x = *(u16 *)(o + 0x28);
        stockTableOutline(x, y, (x + *(u16 *)(o + 0x1C)) & 0xFFFF, (y + *(u16 *)(o + 0x1E)) & 0xFFFF, o[0x1A], *(int *)(o + 0x54) & 0xFFFFFFFF, o[0x45], *(int *)(o + 0x58), img);
    }
}
