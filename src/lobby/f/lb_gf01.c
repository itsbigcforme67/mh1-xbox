/* lb_gf01 - near-match fixes 0x005CD9F0-0x005CDA88: Lb_player_load. Whole file in lb_f.c. */
#include "lobby_f.h"

typedef unsigned __int128 u128;
typedef struct LBQ20 { u128 q; f32 f; } LBQ20;






void Lb_player_load(PLW *pl) {
    CW8(0x2C07) = 1;
    pl_create_model(pl->id);
    armor_create_model(pl);
    yure_init(pl);
    pl_chr_set3(pl, 1, 0, 0, 0);
    pl_chr_set3(pl, 0x65, 0, 0, 1);
    parts_init(pl);
    CW8(0x2C07) = 0;
    cw[pl->id + 0x2BFE] = 1;
}
