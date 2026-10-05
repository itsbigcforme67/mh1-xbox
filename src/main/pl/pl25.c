/* Player code (SLPM_654.95 0x001414C0-0x00141BA0): pl_mv103..pl_mv112 (kneel/pitfall/barrel/misc action handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_mv103(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x3C, 4, 0);
        pl->work90A = 0;
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && ((pl->sw.trg & 0x40) || (pl->work90A != 0))) {
            Pl_act_set2(pl, 0, 0x69, 0);
        }
        break;
    }
}

void pl_mv104(PLW *pl) {
    s32 w;
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work90A = 0;
        pl->work08 = 0;
        break;
    case 1:
        w = pl->work08 + 1;
        pl->work08 = w;
        if ((w >= 0x259) || (pl->work90A != 0)) {
            if (pl->work90A == 1) {
                Pl_act_set2(pl, 0, 0x6A, 0);
                break;
            }
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

void pl_mv105(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3D, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv106(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x280, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv107(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl->work40C = 6;
        Pl_stamina_calc(pl, -0x4B);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x3F6, 2, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x3F7, 2, 0);
            break;
        case 2:
        case 3:
            pl_chr_set2(pl, 0x3F8, 2, 0);
            break;
        }
        break;
    case 1:
        if ((arg1 == 3) && (frame_check2(28.0f, pl, 0) == 0)) {
            sp40[2] = -8.0f;
            sp40[0] = 0.0f;
            sp40[1] = 0.0f;
            flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp30[0];
            pl->pos[2] = pl->pos[2] + sp30[2];
        }
        if (pl->work194 == 0) {
            if (arg1 == 2) {
                pl_to_normal(pl, 0, 6, 0);
            } else {
                pl_to_normal(pl, 0, 8, 0);
            }
        }
        break;
    }
}

void pl_mv111(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->work08 = 0;
        pl_chr_set2(pl, 0x1B0, 6, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (frame_check2(78.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x70, 0xC);
            } else {
                pl->x05++;
            }
        }
        break;
    case 2:
        pl_chr_set2(pl, 0x1B0, 0, 0x4E);
        break;
    }
}

void pl_mv112(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->work08 = 0;
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x1B0, 0, 0x4E);
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (Pl_master_ck(pl) == 1) {
            Taru_item_set(pl);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
