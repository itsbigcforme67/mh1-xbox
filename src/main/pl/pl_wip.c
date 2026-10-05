/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"








void pl_at014(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 0, 0);
        func_6362B0(pl, 4);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_at020(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x32, 4, 0);
        func_6362B0(pl, 5);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_at021(PLW *pl) {
    u8 s;

    pl->x763 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work01C = pl->work01D;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

s32 we02_hit_sub(PLW *pl) {
    u8 s;
    u8 t;

    s = pl->x07;
    switch (s) {
    case 0:
        if (pl->work019 != 0) {
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                pl->x07 = pl->x07 + 1;
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set2(pl, 1, 0x1F, 0);
                return 1;
            case 2:
            case 3:
                pl->x07++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 1);
                break;
            }
        }
        break;
    case 1:
        if (pl->x610 <= 0) {
            pl->x07 = s + 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}
