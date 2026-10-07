/* lbui, run 9: getUserInfo .. Lb_get_comment (lobby.bin 0x00594FD0-0x005950B8): the matching functions of lbui_nm.c. */
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

int getUserInfo(void) {
    switch (Lbs_SeekId()) {
    case 0:
        Lbc_RequestNetComment(CW->x2F80);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

int Lb_get_comment(a)
int a;
{
    int id = Lb_get_plID() & 0xFF;

    if (id != 0xFF) {
        memset(CW->comment[id], 0, 0x62);
        Lbc_RequestNetComment(a);
        return 1;
    }
    return 0;
}
