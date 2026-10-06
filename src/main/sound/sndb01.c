/* SLPM_654.95 0x0024A240-0x0024A3DC: pl_local_init .. sound_call2. See f_sound_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "fl.h"

extern PLW player_work[];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
void Pl_se_req2_com(PLW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);
void armor_sd_req(PLW *, int);
void eft13_set(PLW *, int, int);
void Eft13_set_scl(PLW *, int, int, f32);
void Eft20_set_pl(f32, PLW *, s16, s16);
void Eft02_set_pos(f32 *, int, int);
void SetVector(f32 *, f32, f32, f32);
void parts_chg(PLW *, int, int);
void func_54BA40(PLW *, int);   /* game.bin Eft14_set4 */
void func_555020(PLW *, int);   /* game.bin Eft21_set */
void func_60E2B0(PLW *, int);   /* lobby Eft25_set */
s32 Code_Make(int, int, int, int);
s32 Pl_stg_ck(PLW *);


void ef_move_sub_0024A790(PLW *pl, u8 *w);












void pl_local_init(PLW *pl) {
}

/* 0x0024A250 */
void pl01_effect_move(PLW *pl) {
    u8 *w = (u8 *)pl + 0x444;
    switch (w[1]) {
    case 0:
        w[1]++;
        *(u16 *)(w + 0x12) = 0;
        break;
    case 1:
        ef_move_sub_0024A790(pl, w);
        break;
    }
}

/* 0x0024A2A0: common-pack sound at an integer frame */
void sound_call_0024A2A0(PLW *pl, int frame, int code) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, code, 0, pl->pos, 1, 0);
    }
}

/* 0x0024A300: as sound_call, request type 3 */
void sound_call_h(PLW *pl, int frame, int code) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, code, 0, pl->pos, 3, 0);
    }
}

/* 0x0024A360: the player's own port (weapon / voice); not while
 * PLW+0x7ED or game_w+0x1DC */
void sound_call2(PLW *pl, int frame, int code) {
    if (PU8(pl, 0x7ED) == 0 && GWU8(0x1DC) == 0) {
        if (frame_check((f32)frame, pl, 0) != 0) {
            Pl_se_req2(pl, code, 0, pl->pos, 1, 0);
        }
    }
}
