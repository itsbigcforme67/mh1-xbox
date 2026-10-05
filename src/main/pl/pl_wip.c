/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);









void pl_egg00(PLW *pl, s32 arg1) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work08 = 0x1E;
        pl->work760 = 0;
    case 1:
        if (pl->char0 != 0x33) {
            pl_chr_set2(pl, 0x33, 4, 0);
        }
        if (arg1 == 1) {
            w = pl->work08;
            if (w <= 0) {
                egg_com_ck(pl, 0);
                break;
            }
            pl->work08 = w - 1;
            break;
        }
        egg_com_ck(pl, 0);
        break;
    }
}

void pl_egg01(PLW *pl) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work08 = 0x14;
        pl->work760 = 0x1E;
        pl_chr_set2(pl, 0x34, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        egg_com_ck(pl, 1);
        break;
    }
}

void pl_egg02(PLW *pl) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->work011 == 0) {
            pl_chr_set2(pl, 0x35, 4, 0);
        } else {
            pl_chr_set2(pl, 0x3B, 4, 0);
        }
        pl->work08 = 0x14;
        pl->work760 = 0x1E;
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x02000200);
        break;
    case 1:
        if (front_land_ck2(85.0f, -60.0f, pl, 0) != 0) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 5, 5, 0);
            break;
        }
        Pl_stamina_calc(pl, -1);
        if (pl->stamina <= 0) {
            Pl_act_set2(pl, 5, 4, 0);
            break;
        }
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        egg_com_ck(pl, 1);
        break;
    }
}
