/* lb_l02 - lobby 0x005D0E60-0x005D0F70: pl_sleeping (zzz effect over a sleeping member). Whole file in lb_l.c.
   q is a local initializer {0, 20, 0}: emitted as a 12-byte .data object (slot lobby:data 0x0064E1A8, was lit_584_0064E1A8) and copied with ld/lwc1 through pointers. */
#include "lobby_f.h"

void pl_sleeping(PLW *pl) {
    f32 q[3] = { 0.0f, 20.0f, 0.0f };
    int t;
    u16 r;
    if (pl->char0 == 0x1AB && ((r = ran_suu(1)) & 0x3F) == 0) {
        Lb_pl_chr_set(pl, 0x1AC, 0, 0);
    } else if (pl->char0 == 0x1AC && F(s32, pl, 0x194) == 0) {
        Lb_pl_chr_set(pl, 0x1AB, 0, 0);
    }
    t = *(u16 *)0x3F340E % 60;
    switch (t) {
    case 0:
    case 0xA:
    case 0x14:
        Eft06_set2(0.6f, pl, 4, 0x14, q);
    }
}
