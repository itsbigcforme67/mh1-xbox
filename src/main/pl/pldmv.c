/* Player code (SLPM_654.95 0x00136380-0x001365F0): pl_dm_value_sub, per-frame damage-over-time of a player: poison
   counter (x7BA / work7BE), the near-monster flash effect and the stage environment damage (stage env 1 = hot/cold). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"

void pl_dm_value_sub(PLW *pl) {
    f32 pos[3];
    s16 e, t;
    if (pl->flag14 == 3) return;
    if (pl->x7BA > 0) {
        t = pl->work7BE - 1;
        pl->work7BE = t;
        if (t <= 0) {
            pl->x7BA = pl->x7BA - 1;
            pl->work7BE = 0x1E;
            if (Pl_master_ck(pl) == 1) {
                Pl_vital_calc(pl, -1);
                pos[1] = 20.0f;
                pos[0] = 0;
                pos[2] = 0;
                Eft06_set2(1.0f, pl, 3, 0x14, pos);
            }
            if (pl->vital <= 0 && Pl_master_ck(pl) == 1) {
                pl->vital_red = 0;
                if (pl->st != 2) {
                    func_639F20(pl);
                } else if (act_ck(pl, 0, 9) == 0) {
                    Pl_act_set(pl, 0, 9, 0);
                }
            }
        }
    }
    if (Pl_master_ck(pl) == 0 && (pl->work4D5 & 1) && (*(u16 *)&game_w.x1E % 30) == 0) {
        pos[1] = 20.0f;
        pos[0] = 0;
        pos[2] = 0;
        Eft06_set2(1.0f, pl, 3, 0x14, pos);
    }
    if (Pl_master_ck(pl) == 1) {
        e = Stage_env_ck(pl->stg);
        switch (e) {
        case 1:
            if (Pl_Skill_ck(pl, 0x2D) != 1 && pl->work918 == 0) {
                t = pl->work90C - 1;
                pl->work90C = t;
                if (t <= 0) {
                    pl->work90C = 0x5A;
                    Pl_vital_calc(pl, -1);
                    if (pl->vital <= 0) {
                        pl->vital_red = 0;
                        if (pl->st != 2) {
                            func_639F20(pl);
                            return;
                        }
                        Pl_act_set(pl, 0, 9, 0);
                    }
                }
            }
            break;
        case 2:
            break;
        }
    }
}
