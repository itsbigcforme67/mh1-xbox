/* lb_f02 - lobby gold/player init 0x005CDA90-0x005CDB68: Lb_player_release, Lb_pl_chr_set. Whole file in lb_f.c. */
#include "lobby_f.h"

typedef unsigned __int128 u128;
typedef struct LBQ20 { u128 q; f32 f; } LBQ20;






void Lb_player_release(PLW *pl) {
    u8 st;
    PLU8(pl, 1) = 0;
    PLU8(pl, 0) = 0;
    armor_model_free();
    if (pl->work564 != 0) {
        release_prim(pl->work568);
        pl->work564 = 0;
    }
    st = game_w.stage;
    if (st == 0x4C || st == 0x4D) {
        flCompact(st);
    }
}

void Lb_pl_chr_set(PLW *pl, int c, int blend, int tm) {
    pl->work81D = 0;
    lb_pl_chr_set_com(pl, c, blend, tm, 0);
    lb_pl_chr_set_com(pl, c + 0x64, blend, tm, 1);
}
