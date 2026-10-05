/* lb_f01 - lobby gold/player init 0x005CD930-0x005CD9EC: Lb_pl_init. Whole file in lb_f.c. */
#include "lobby.h"

typedef unsigned __int128 u128;
typedef struct LBQ20 { u128 q; f32 f; } LBQ20;






void Lb_pl_init(int a0) {
    int i = 0;
    PLW *pl = player_work;
    do {
        if (pl->be_flag != 0) {
            if (pl->id == game_w.master) {
                Eft26_set(pl);
            }
            PLU8(pl, 0x8C4) = 0;
            pl->work568 = get_prim();
            a0 = pl->work568;
            if (a0 != -1) {
                pl->work564 = get_prim_ptr(a0);
                a0 = *(u16 *)&pl->id;
                *(s32 *)((u8 *)pl->work564 + 0x18) = a0;
                *(void **)((u8 *)pl->work564 + 0x14) = Lb_trans_pl;
            }
        }
        i++;
        pl++;
    } while (i < 8);
    hit_chk_init(a0);
}
