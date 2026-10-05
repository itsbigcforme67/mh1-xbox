/* edit09 - select.bin 0x00534A80-0x00534C20: edit_pl_init (continue screen model setup). Whole file in edit_nm.c. */
#include "select.h"

extern f32 D_2F2624[];
extern f32 D_2F2628[];

void edit_pl_init(PLW *pl, s16 mode, u16 no) {
    EDIT_W *e = &edit_w;
    pl->be_flag = 1;
    pl->x01 = 1;
    pl->id = no;
    pl->x10 = 0;
    pl->work8C5 = 1;
    pl->work798 = 1.0f;
    pl->work300 = 2;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    pl->chr_spd0 = 1.0f;
    pl->chr_spd1 = 1.0f;
    pl->flag12 = 0;
    pl->pos[0] = ((f32 *)stage_start_pos)[game_w.stage * 3];
    pl->pos[1] = D_2F2624[game_w.stage * 3];
    pl->pos[2] = D_2F2628[game_w.stage * 3];
    pl->ang[1] = 0;
    pl->ang[0] = 0;
    pl->ang[2] = 0;
    if (mode == 0) {
        pl->work011 = *(u8 *)((u8 *)e + 4);
        pl->work5FC = e->col;
        pl->work352[0] = 1;
        pl->work352[1] = 1;
        pl->work352[2] = 1;
        pl->work352[3] = 1;
        pl->work352[4] = 1;
        pl->work352[5] = 1;
        B8(pl, 0x607) = 0;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    pl_create_model(pl->id);
    armor_create_model(pl);
    yure_init(pl);
    parts_init(pl);
    pl->work568 = get_prim();
    if (pl->work568 != -1) {
        pl->work564 = get_prim_ptr(pl->work568);
        B32(pl->work564, 0x18) = pl->id;
        BP(pl->work564, 0x14) = (void *)trans_pl_sub;
    }
}
