/* Player code (SLPM_654.95 0x0014A050-0x0014A548): pl_demo001..005: demo (cutscene) player states */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_demo001(PLW *pl) {
    f32 f;
    u8 s;

    pl->work40E = 0xA;
    pl->work40C = 0xA;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xDE, 0, 0);
        pl->st = 2;
        break;
    case 1:
        if (frame_check2(28.0f, pl, 0) != 0) {
            pl->pos[1] = pl->pos[1] - 4.0f;
        }
        f = pl->x5AC;
        if (pl->pos[1] <= f) {
            pl->pos[1] = f;
        }
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD1, 0, 0x46);
            pl->st = 0;
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 0, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_demo002(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work88D = 1;
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

void pl_demo003(PLW *pl) {
    u8 s;
    u8 v;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x26F, 6, 0);
        pl->work39C = 0;
        v = pl->work56B;
        if (v & 0xF) {
            pl->work56B = v & 0xF0;
            func_549200(pl, 4);
        }
        if (Pl_master_ck(pl) == 1) {
            PlayerDieCameraRequest();
        }
        break;
    case 1:
        break;
    }
}

void pl_demo004(PLW *pl) {
    u8 s;

    pl->work40C = 4;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0xD8, 0, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->vital = pl->work792;
            pl->vital_red = pl->work792;
            pl->work882 = 0x12C;
            pl->work8C0 = 0x2A30;
            pl->stamina = pl->work882;
            pl->x7AC = 0;
            pl->work8CC = 0;
            pl->work918 = 0;
            pl->work91A = 0;
            pl->work91C = 0;
            pl->work930 = 0;
            pl->x7AA = 0;
            pl->x7BA = 0;
            pl->x7B2 = 0;
            pl->x7C4 = 0;
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_demo005(PLW *pl) {
    s32 w;
    u8 v;
    u8 s;

    pl->work40C = 0xA;
    pl->work40E = 0xA;
    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl_flag_clr(pl, 0x80000);
    pl->pos[1] = pl->x5AC;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0xD2, 0, 0);
        action_timer_calc(pl, 0);
        pl_voice_req(pl, 0x22);
        pl_flag_clr(pl, 2);
        pl_flag_clr(pl, 0x8000);
        pl->flag12 = 0;
        pl->work08 = 0x12C;
        v = pl->work56B;
        if (v & 0xF) {
            pl->work56B = v & 0xF0;
            func_549200(pl, 4);
        }
        if (Pl_master_ck(pl) == 1) {
            PlayerDieCameraRequest();
        }
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
        }
        break;
    case 2:
        pl->be_flag = 0;
        pl->x01 = 0;
        break;
    }
}
