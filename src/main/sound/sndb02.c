/* SLPM_654.95 0x0024A5A0-0x0024A78C: ashi_eft_req .. move_default_0024A750. See f_sound_nm.c. */
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












/* 0x0024A5A0: dust at joint j */
void ashi_eft_req(f32 frame, PLW *pl, int j, int kind) {
    if (frame_check(frame, pl, 0) != 0) {
        switch ((s16)kind) {
        case 0: eft13_set(pl, j, 0); break;
        case 1: eft13_set(pl, j, 3); break;
        case 2: eft13_set(pl, 5, 4); break;
        case 3: eft13_set(pl, j, 4); break;
        case 4: eft13_set(pl, j, 5); break;
        case 5: eft13_set(pl, j, 6); break;
        case 6: eft13_set(pl, j, 7); break;
        case 7: eft13_set(pl, j, 8); break;
        case 8: eft13_set(pl, j, 0x14); break;
        }
    }
}

/* 0x0024A6E0: armour rattle */
void yoroi_sd_req(f32 frame, PLW *pl, int kind) {
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        armor_sd_req(pl, kind);
    }
}

/* 0x0024A750: hands back to their default models */
void move_default_0024A750(PLW *pl, u8 *w) {
    parts_chg(pl, 0x12, 0);
    parts_chg(pl, 0xE, 0);
}
