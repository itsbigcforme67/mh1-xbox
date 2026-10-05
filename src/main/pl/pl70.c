/* Player code (SLPM_654.95 0x00153A70-0x00153D9C): tame count, stamina, chat action */
#include "pl.h"
#include "game.h"
#include "plf.h"
extern u8 chat_act_tbl_002F1860[0xD];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
#include "flow.h"
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void tame_cnt_up(PLW *pl) {
    s16 t = pl->work87C;
    if (t < 0x3E8) {
        pl->work87C = t + 1;
        if (pl->work87C == 0x3C) {
            Pl_se_req2(pl, 9, 0, pl->pos, 1, 0);
        }
    } else {
        pl->work87C = 0x3E8;
    }
}

void Pl_stamina_calc(PLW *pl, int dv) {
    if (Pl_master_ck(pl) != 0) {
        if ((s16)dv < 0 && pl->work8CC != 0) {
            return;
        }
        pl->stamina = pl->stamina + dv;
        if (pl->stamina <= 0) {
            pl->stamina = 0;
        }
        if (pl->stamina >= pl->work882) {
            pl->stamina = pl->work882;
        }
    }
}

void Pl_max_stamina_calc(PLW *pl, int dv) {
    if (Pl_master_ck(pl) != 0) {
        if ((s16)dv < 0 && pl->work8CC != 0) {
            return;
        }
        pl->work882 = pl->work882 + dv;
        if (pl->work882 < 0x4C) {
            pl->work882 = 0x4B;
        } else if (pl->work882 > 0x1C2) {
            pl->work882 = 0x1C2;
            pl->work8C0 = 0x2A30;
        }
        if (pl->work882 < pl->stamina) {
            pl->stamina = pl->work882;
        }
    }
}

void Pl_stamina_reduce(PLW *pl) {
    int n;
    if (Pl_master_ck(pl) != 0 && Pl_Skill_ck(pl, 0x12) != 1) {
        if ((s16)Stage_env_ck(pl->stg) == 2 && Pl_Skill_ck(pl, 0x2C) == 0 && pl->work91A == 0) {
            n = 3;
        } else {
            n = 1;
        }
        if (Pl_Skill_ck(pl, 0xD) != 1 || (PU16(&game_w, 0x1E) & 1)) {
            if (Pl_Skill_ck(pl, 0x17) == 1) {
                n = (s16)(n * 2);
            }
            pl->work8C0 = pl->work8C0 - n;
            if (pl->work8C0 <= 0) {
                pl->work8C0 = 0x2A30;
                Pl_max_stamina_calc(pl, -0x4B);
            }
        }
    }
}

void Pl_chat_act_req(int act) {
    PLW *p = &player_work[game_w.master];
    p->work8C4 = act;
}

void Pl_chat_act_set(PLW *pl) {
    u8 a = pl->work8C4;
    if (a != 0) {
        Pl_act_set(pl, 6, chat_act_tbl_002F1860[a], 0);
    }
}
