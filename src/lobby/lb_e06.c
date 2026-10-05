/* lb_e06 - lobby members/cockpit/icons 0x005CCDF0-0x005CCE88: Lb_put_icon_free2. Whole file in lb_e.c. */
#include "lobby.h"













void Lb_put_icon_free2(a0, a1, a2, a3, no)
int a0;
s16 a1;
s16 a2;
int a3;
s16 no;
{
    struct { s16 w, x, y, z; s32 u; LBS16x2 e; LBS16x2 f; } pk;
    LBS16x2 *t;
    pk.x = a1;
    pk.u = a3;
    t = (LBS16x2 *)&lb_icon_tbl[no * 4];
    pk.y = a2;
    pk.z = a2;
    pk.w = 0.8f * a0;
    pk.e = t[0];
    pk.f = t[1];
    flps0008(&pk, &pk.f, &pk.e, t);
}
