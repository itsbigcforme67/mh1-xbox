/* lb_f01 - lobby gold/player init 0x005CD930-0x005CD9EC: Lb_pl_init. Whole file in lb_f.c. */
#include "lobby_f.h"

typedef unsigned __int128 u128;
typedef struct LBQ20 { u128 q; f32 f; } LBQ20;






#ifdef VERSION_MH2
#ifdef VERSION_MH2
#include "mh2_lb.h"
void Lb_pl_init(int a0) {
    int i = 0;
    PLM *pl = (PLM *)player_work;
    u8 flag = a0;
    do {
        if (pl->be_flag != 0) {
            if (*(s32 *)0x8BDC10 != 0xE && pl->id == game_w.master) {
                Eft26_set((PLW *)pl);
            }
            pl->x81C = 0;
            if (flag != 0) {
                pl->prim = get_prim();
                if (pl->prim != -1) {
                    pl->prim_ptr = get_prim_ptr(pl->prim);
                    a0 = *(u16 *)&pl->id;
                    *(s32 *)((u8 *)pl->prim_ptr + 0x18) = a0;
                    *(void **)((u8 *)pl->prim_ptr + 0x14) = Lb_trans_pl;
                }
            }
        }
        i++;
        pl++;
    } while (i < 10);
    hit_chk_init();
}
#else
void Lb_pl_init(u8 a0) {
    int i = 0;
    PLW *pl = player_work;
    do {
        if (pl->be_flag != 0) {
            if (*(s32 *)0x8BDC10 != 0xE && pl->id == game_w.master) {
                Eft26_set(pl);
            }
            PLU8(pl, 0x81C) = 0;
            if (a0 != 0) {
                F(s16, pl, 0x530) = get_prim();
                if (F(s16, pl, 0x530) != -1) {
                    F(s32, pl, 0x52C) = (s32)get_prim_ptr(F(s16, pl, 0x530));
                    *(s32 *)(F(s32, pl, 0x52C) + 0x18) = *(u16 *)&pl->id;
                    *(s32 *)(F(s32, pl, 0x52C) + 0x14) = (s32)Lb_trans_pl;
                }
            }
        }
        i++;
        pl = (PLW *)((u8 *)pl + 0x970);
    } while (i < 10);
    hit_chk_init();
}
#endif
#else
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
#endif
