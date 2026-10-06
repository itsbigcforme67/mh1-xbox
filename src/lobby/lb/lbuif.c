/* lbui, run 6: DispButtonHelp .. DispButtonHelp (lobby.bin 0x00593360-0x00593A64): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void DispButtonHelp(n)
LB_NETW *n;
{
    u16 pad;
    int p;

    pad = n->x28;
    if (n->x0C == 0) {
        flfntSetSize(0x12, 0x12);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        switch (n->sel) {
        case 14:
            put_button_help(0, 0, 2, (u16)Get_sw2(0) & 0x100);
            put_button_help(1, 1, 3, (u16)Get_sw2(0) & 0x200);
            return;
        case 0:
            if (n->step != 3) {
                p = pad & 0xFFFF;
                put_button_help(1, 2, 2, p & 0x100 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            }
            put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 1:
            p = pad & 0xFFFF;
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 3:
            switch (n->step) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 9:
                p = pad & 0xFFFF;
                put_button_help(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            case 4:
                p = pad & 0xFFFF;
                put_button_help(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 5:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 5:
            switch (n->step) {
            case 0:
            case 5:
            case 6:
            case 8:
            case 11:
                p = pad & 0xFFFF;
                put_button_help(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 9:
            case 10:
                put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 6:
            switch (n->step) {
            case 6:
            case 8:
            case 9:
            case 11:
                p = pad & 0xFFFF;
                put_button_help(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 10:
                put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 4:
            switch (n->step) {
            case 0:
            case 1:
                p = pad & 0xFFFF;
                put_button_help(0, 0xD, 6, p & 0x80 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 0xB, 0, p & 0x20 & 0xFFFF);
                return;
            case 2:
                p = pad & 0xFFFF;
                put_button_help(1, 0xA, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 3:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 7:
            put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 8:
        case 10:
            p = pad & 0xFFFF;
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 0xE, 0, p & 0x20 & 0xFFFF);
            return;
        case 9:
            if ((s32)n->step >= 3) {
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            p = pad & 0xFFFF;
            put_button_help(1, 0xF, 2, p & 0x100 & 0xFFFF);
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 11:
            p = pad & 0xFFFF;
            put_button_help(1, 1, 3, p & 0x200 & 0xFFFF);
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            break;
        }
    }
}
