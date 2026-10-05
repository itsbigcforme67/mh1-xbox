/* Player code (SLPM_654.95 0x00148AE0-0x00149698): pl_dm015..pl_dm021, piyo_reset (static: MWCC then knows it leaves a0 alone, which pl_dm019/021 rely on) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_dm015(PLW *pl) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xDA, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        break;
    case 1:
        if (frame_check2(40.0f, pl, 0) == 0) {
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            sp30[2] = -18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 42.0f)) / 360.0f)));
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp20[0];
            pl->pos[2] = pl->pos[2] + sp20[2];
            if ((pl->work39C % 7) == 0) {
                eft13_set(pl, 5, 9);
                eft13_set(pl, 8, 9);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}

void pl_dm016(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        if (arg1 == 0) {
            pl->x07 = 4;
        } else {
            pl->x07 = 1;
        }
        pl_chr_set2(pl, 0xD3, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->work08 = 0;
        break;
    case 1:
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x06++;
            if (pl->x06 >= pl->x07) {
                pl->x05++;
                pl_chr_set2(pl, 0xD4, 2, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}

void pl_dm017(PLW *pl) {
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
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

static void piyo_reset(PLW *pl) {
    pl->x7AC = 0;
    pl->x7AA = 0;
    pl->work886 = 0x1E;
}

void pl_dm019(PLW *pl) {
    s8 n;
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl->flag12 = 0;
        pl->x43E = 0;
        pl_chr_set2(pl, 0x1A, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->work08 = 0x12C;
        pl->x06 = 0;
        pl->x07 = 5;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->ang_y = pl->ang[1];
        func_558A80(pl, 0);
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x06 = 0;
            pl->x07 = pl->x07 - 1;
            pl->work08 = 0x12C;
        } else if ((pl->sw.trg & 0x3FF) || (pl->sw.an_trg & 0x3C3C)) {
            pl->x06 = pl->x06 + 1;
            n = piyo_ret_tbl[pl->x07];
            if (Pl_Skill_ck(pl, 0x16) == 1) {
                n = n * 2;
            }
            if (pl->x06 >= n) {
                pl->x06 = 0;
                pl->x07 = pl->x07 - 1;
                pl->work08 = 0x12C;
            }
        }
        if (pl->x07 == 0) {
            piyo_reset(pl);
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_dm020(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x197, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_dm021(PLW *pl) {
    f32 sp20[3];
    s32 w;
    s32 h;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->flag12 = 0;
        pl->x7B2 = 0;
        piyo_reset(pl);
        pl_chr_set2(pl, 0xE1, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->work08 = 0xF0;
            if (Pl_Skill_ck(pl, 0xB) == 1) {
                pl->work08 = pl->work08 / 2;
            }
            if (Pl_Skill_ck(pl, 0x15) == 1) {
                pl->work08 = pl->work08 * 2;
            }
            pl_chr_set2(pl, 0xD2, 0, 0x9A);
        }
        break;
    case 2:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xCF, 0, 0);
            break;
        }
        sp20[0] = 0.0f;
        sp20[1] = 20.0f;
        h = *(u16 *)&game_w.x1E % 60;
        sp20[2] = 0.0f;
        switch (h) {
        case 0:
        case 0xA:
        case 0x14:
            Eft06_set2(0.8f, pl, 4, 0x14, sp20);
            break;
        }
        Pl_stamina_calc(pl, 2);
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_dm022(PLW *pl, s32 arg1) {
    s16 v;
    u16 r;
    u8 s;

    pl->x06 = pl->x06 + 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->flag12 = 0;
        pl->x7C4 = 0;
        pl->x7B2 = 0;
        piyo_reset(pl);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0xE0, 4, 0);
            pl->work39C = 0;
            pl->x06 = 0;
            Eft06_set(0.75f, pl, 5, 0, 2);
            pl->work934 = 0x96;
            if (Pl_Skill_ck(pl, 0xA) == 1) {
                pl->work934 = pl->work934 / 2;
            }
            if (Pl_Skill_ck(pl, 0x14) == 1) {
                pl->work934 = pl->work934 * 2;
            }
            pl->work40C = 2;
            break;
        }
        pl->x05 = 2;
        if (pl->work934 <= 0) {
            pl->work934 = 0;
        }
        pl_chr_set2(pl, 0xD2, 2, 0x96);
        break;
    case 1:
        pl->work40C = 2;
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD2, 2, 0x96);
            pl->work40C = 0;
        }
        if ((pl->x05 < 3) && !(pl->x06 & 0x1F)) {
            Eft06_set(0.75f, pl, 5, 0, 2);
        }
        break;
    case 2:
        v = pl->work934 - 1;
        pl->work934 = v;
        if (v <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xCF, 0, 0);
            pl->work40C = 2;
            break;
        }
        if ((pl->char0 == 0xD2) && (frame_check(164.0f, pl, 0) != 0) && !((r = ran_suu(1)) & 0x3F)) {
            pl_chr_set2(pl, 0xDF, 0, 0);
        } else if ((pl->char0 == 0xDF) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0xD2, 4, 0xA4);
        }
        if ((pl->x05 < 3) && !(pl->x06 & 0x1F)) {
            Eft06_set(0.75f, pl, 5, 0, 2);
        }
        break;
    case 3:
        pl->work40C = 2;
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            pl->work40C = 6;
        }
        break;
    }
}

void pl_dm023(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3FA, 2, 1);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x8000);
        pl->flag12 = 1;
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
        }
        break;
    }
}
