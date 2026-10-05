/* Lobby: gold display, player init/load/release, timers (SLPM_654.95 lobby overlay 0x5CCFF0-). Whole file; runs split into lb_fNN.c */
#include "lobby_f.h"

typedef unsigned __int128 u128;
typedef struct LBQ20 { u128 q; f32 f; } LBQ20;

void Lb_put_gold(void) {
    f32 q[5];
    u8 *src = lit_693_0064E180;
    *(u128 *)q = *(u128 *)src;
    q[4] = *(f32 *)(src + 0x10);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(q);
    flfntSetSize(0x1C, 0x14);
    font_set_palette(0);
    font_print_ex(0x1DA, 0x1E, 0, lit_695_00664CB8, *(s32 *)0x3C6FE0);
}

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
