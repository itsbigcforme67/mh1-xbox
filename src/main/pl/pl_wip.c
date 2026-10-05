/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);

void pl_demo000(PLW *pl) {
    EMW *e;
    u8 *q;
    s16 i;
    s16 k;
    u8 *g;
    u8 s;

    pl->work40E = 0xA;
    pl->work40C = 0xA;
    pl->x01 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        k = -1;
        pl->x06 = 0;
        i = 0;
        g = (u8 *)&game_w;
        do {
            if ((g[0x28] == 0x12) && ((e = pull_enemy_work()) != 0)) {
                q = (u8 *)(i + (int)game_w.x28);
                e->mdl_no = i;
                e->kind = *q;
                e->stg = game_w.stage;
                e->type = 0;
                e->type = 0xFF;
                e->x616 = pl->id;
                enemy_mv(e);
                e->pos[0] = pl->pos[0];
                e->pos[1] = pl->pos[1];
                e->pos[2] = pl->pos[2];
                e->ang[1] = pl->ang[1];
                pl->em_demo = e;
                k = *q;
                break;
            }
            i++;
            g++;
        } while (i < 4);
        if (k != 0x12) {
            pl_to_normal(pl, 0, 4, 0);
        } else {
            pl->x07 = 0;
            pl_chr_set2(pl, 0xDD, 0, 0);
        }
        if (Pl_master_ck(pl) == 1) {
            PlComebackCameraRequest();
        }
        break;
    case 1:
        e = pl->em_demo;
        if (e == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        get_joint_pos_em(e, 0x1D, pl->pos);
        if (e->char0 == 0x3EB) {
            Pl_act_set2(pl, 4, 1, 2);
        }
        pl->ang[1] = e->ang[1];
        pl->ang_y = pl->ang[1];
        break;
    }
}

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
