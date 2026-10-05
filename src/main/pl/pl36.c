/* Player code (SLPM_654.95 0x00148870-0x00148AE0): pl_dm009, pl_dm011: damage reaction handlers */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_dm009(PLW *pl, s32 arg1) {
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 v;
    u16 r;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        if (pl->flag12 != 0) {
            pl_chr_set2(pl, 0x4BB, 0, 0);
        } else {
            pl_chr_set2(pl, 0xC9, 0, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        sp70[0] = 0;
        sp70[2] = 0;
        sp70[1] = *(u16 *)&pl->ang_y;
        cpRotMatrix(sp70, sp30);
        sp80[0] = 0.0f;
        sp80[1] = 0.0f;
        if (arg1 != 0) {
            v = 4.0f;
        } else {
            v = -4.0f;
        }
        sp80[2] = v;
        flvecApplyMat33(pl->vel, sp80, sp30);
        break;
    case 1:
        if (frame_check2(16.0f, pl, 0) == 0) {
            rate_add(pl);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_dm011(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x585, 0, 0);
        action_timer_calc(pl, 0);
        Pl_se_req2_com(pl, 0x67, 0, pl->pos, 1, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl_flag_set(pl, 0x8000);
        vib_set_pl(pl, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
        }
        break;
    }
}
