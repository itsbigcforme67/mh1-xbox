/* Quest result screen, second part (0x291D30-0x292508): money payout display
 * (gold_disp) and the hunter-point payout (result_init/result_main). Split
 * from f_reward.c so that each file owns only its own jump tables. */
#include "reward.h"

void gold_disp(GAME_W *t)
{
    char buf[0x70];
    int v;

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    switch (t->sub) {
    default:
    case 6:
        flfntLocate(0x4A, 0x16F);
        font_print_sp(lit_465_003865F0);
    case 4:
    case 5:
        sprintf(buf, lit_466_00386610, result_w.gold);
        flfntLocate((s16)(0x140 - strlen_sp(buf) * 18 / 4), 0x103);
        font_print_sp(lit_467_00386620, buf);
    case 3:
        v = 0;
        if (Quest_clear_ck(1) == 1) {
            if (Online_ck() == 1) {
                if (game_w.x21A != 0) {
                    v = quest_w.x94[1];
                }
            } else {
                v = quest_w.x94[1];
            }
        }
        flfntLocate(0x101, 0xCD);
        if (Quest_clear_ck(1) == 1 && Online_ck() == 1 && game_w.x21A != 0) {
            font_print_sp(lit_468_00386630, v);
        } else {
            font_print_sp(lit_469_00386648, v);
        }
    case 2:
        if (Quest_clear_ck(1) == 1) {
            v = quest_w.x14;
        } else {
            v = 0;
        }
        flfntLocate(0x101, 0x97);
        font_print_sp(lit_470_00386658, v);
    case 1:
        flfntLocate(0x10A, 0x61);
        if (Quest_clear_ck(1) == 1) {
            font_print_sp(lit_471_00386668);
        } else {
            font_print_sp(lit_472_00386678);
        }
        v = (int)Quest_str_get(0);
        flfntLocate((s16)(0x140 - (strlen(v) / 2) * 18 / 2), 0x73);
        font_print_sp(lit_467_00386620, v);
    case 0:
        flfntLocate(0x176, 0x16F);
        font_print_sp(lit_473_00386690, t->x08 / 30);
        sprintf(buf, lit_474_003866B0, (u8 *)&User_data + 8);
        flfntLocate((s16)(0x140 - strlen_sp(buf) * 18 / 4), 0x46);
        font_print_sp(lit_467_00386620, buf);
        sprintf(buf, lit_475_003866D0, *(s32 *)((u8 *)&User_data + 0x20));
        flfntLocate((s16)(0x140 - strlen_sp(buf) * 18 / 4), 0x127);
        font_print_sp(lit_467_00386620, buf);
        break;
    }
}

int result_init(GAME_W *t)
{
    int i;
    u8 *p;
    int n;

    if (Online_ck() != 1) {
        return movie_add_check(t, 1) != 0;
    }
    t->sub = 0;
    t->x03 = 0;
    t->x08 = 0xE10;
    result_w.gold = 0;
    t->x06 = 0x14;
    result_w.rank_new = result_w.rank_old = Get_hunter_rank(&User_data);
    for (i = 0, p = card_prim; i < 2; i++, p += 0x20) {
        memset(p, 0, 0x20);
    }
    if (Quest_clear_ck(1) == 1) {
        result_w.gold = quest_w.x64[0xE];
    } else {
        result_w.gold = quest_w.x64[0xF];
    }
    n = player_work[game_w.master].work91E;
    if (n > 0) {
        result_w.gold -= n * 10;
    }
    *(void **)(card_prim + 0x14) = trans_result_1;
    fade_set(2);
    return 0;
}

int result_main(GAME_W *t)
{
    int amt;
    int r;

    if (t->x08 > 0) {
        t->x08--;
    }
    switch (t->sub) {
    case 0:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            se_req(7, 0x10, 0);
        }
        break;
    case 1:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            se_req(7, 0x11, 0);
        }
        break;
    case 2:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            se_req(7, 0x11, 0);
        }
        break;
    case 3:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            se_req(7, 0x13, 0);
        }
        break;
    case 4:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
        }
        break;
    case 5:
        if (result_w.gold != 0) {
            amt = 1;
            if (Psw[0] & 0x20) {
                amt += 0x14;
            }
            if (result_w.gold > 0) {
                if (result_w.gold < amt) {
                    amt = result_w.gold;
                }
            } else {
                amt = -amt;
                if (amt < result_w.gold) {
                    amt = result_w.gold;
                }
            }
            se_req(7, 8, 0);
            r = Hunter_point_add(amt);
            if (t->x03 == 0) {
                t->x03 = r;
            }
            result_w.rank_new = Get_hunter_rank(&User_data);
            if (result_w.rank_new != result_w.rank_old) {
                if (result_w.rank_old < result_w.rank_new) {
                    str_play(1, 0xF);
                } else {
                    se_req(7, 0xD, 0);
                }
                result_w.rank_old = result_w.rank_new;
            }
            result_w.gold -= amt;
        } else {
            t->sub++;
            se_req(7, 0x19, 0);
        }
        break;
    case 6:
        if (t->x08 <= 0 || ((Psw[2] & 0x20) && t->x08 < 0xDB7)) {
            se_req(7, 0x13, 0);
            return 1;
        }
        break;
    }
    return 0;
}
