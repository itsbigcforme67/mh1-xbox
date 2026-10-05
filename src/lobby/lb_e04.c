/* lb_e04 - lobby members/cockpit/icons 0x005CCC90-0x005CCDE4: Lb_put_status, Lb_put_icon_free. Whole file in lb_e.c. */
#include "lobby.h"










void Lb_put_status(int a0, int a1, int a2, int a3, int no) {
    switch ((s16)no) {
    case 0:
        return;
    case 1:
        no = 0x10;
        break;
    case 2:
        no = 0x11;
        break;
    case 3:
        no = 8;
        break;
    case 4:
        no = 0x12;
        break;
    }
    Lb_put_icon_free(a0, a1, a2, a3, no);
}

void Lb_put_icon_free(a0, a1, a2, a3, no)
int a0;
s16 a1;
s16 a2;
int a3;
s16 no;
{
    struct { s16 w, x, y, z; s32 u; LBS16x2 e; LBS16x2 f; } pk;
    LBS16x2 *t = (LBS16x2 *)&lb_icon_tbl[no * 4];
    pk.x = a1;
    pk.w = 0.8f * a0;
    pk.y = 0.8f * a2;
    if (pk.y < 0) {
        pk.y = -pk.y;
    }
    pk.z = a2;
    pk.u = a3;
    pk.e = t[0];
    pk.f = t[1];
    flps0008(&pk, &pk.f, &pk.e);
}
