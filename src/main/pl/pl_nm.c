/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"

void Pl_item_charge(PLW *q) {
    PLW *pl;
    s16 i;
    s16 *p;
    s16 n;
    u8 c = PU8(q, 0x8BF);
    pl = q;
    if (c == 0) return;
    pl->work8BF = c - 1;
    for (i = 0; i < 20; i += 5) {
        PS16(q, 0x828) = 0;
        PS16(q, 0x82A) = 0;
        PS16(q, 0x82C) = 0;
        PS16(q, 0x82E) = 0;
        PS16(q, 0x830) = 0;
        PS16(q, 0x832) = 0;
        PS16(q, 0x834) = 0;
        PS16(q, 0x836) = 0;
        PS16(q, 0x838) = 0;
        PS16(q, 0x83A) = 0;
        q = (PLW *)((u8 *)q + 0x14);
    }
    p = pl_supp_tbl[pl->work8BF + pl->kind * 6];
    n = *p;
    while (n != -1) {
        u16 a = p[1];
        p += 2;
        Pl_item_supply(pl, 0, a, n);
        n = *p;
    }
}

void player_init0(PLW *pl) {
    u32 i;
    s8 *eq;
    u8 *mx;
    if (game_w.pl_state[pl->id] == 0xFF && Pl_master_ck(pl) == 0) {
        pl->be_flag = 0;
        pl->x01 = 0;
        return;
    }
    pl->x10 = 0;
    pl->work01E = 0;
    pl->work300 = 2;
    pl->work350 = 0;
    pl->work351 = -1;
    if (pl->work616 != 0) {
        mx = parts_max_tbl;
        eq = &equip_set[pl->work616 * 6 + pl->work011 * 0x24];
        for (i = 0; i < 6; i++) {
            if (*eq < 0) {
                pl->work352[i] = (ran_suu(1) & 0xFFFF) % mx[pl->work011 * 6] + 1;
            } else if (i == 2 && pl->work011 != 0 && softdip_ck(0x50) != 0) {
                pl->work352[i] = 10;
            } else {
                pl->work352[i] = *eq;
            }
            mx++;
            eq++;
        }
        pl->work5FC = test_hair_col[pl->work616 + pl->work011 * 6] | 0xFF000000;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    armor_create_model(pl);
    yure_init(pl);
}

void pl_work_clr(PLW *pl, u8 no, PLPROG *prog) {
    pl->id = (u8)no;
    if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
        pl->x01 = 1;
    } else {
        pl->x01 = 0;
    }
    pl->prog = prog;
    pl_init_sub(pl);
    pl->chr_no0 = (u8)no + 1;
    pl->prog->init(pl);
    pl->prog->init2(pl);
}

int Pl_Skill_ck(int, int);