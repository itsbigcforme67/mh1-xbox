/* Quest result / reward screen (SLPM_654.95 main, f_reward range).
 * First part, 0x290E80-0x291D2C: lookup of the "key quest" and "movie add"
 * tables, the result screen backdrop/frame drawing, the check that unlocks a
 * bonus movie after a quest, the result step machine (result_prog) and the
 * money payout (gold_init/gold_main). Meanings are guesses. */
#include "reward.h"

int key_quest_ck(s16 no)
{
    s16 *p = key_quest_tbl;

    while (*p != -1) {
        if (*p == no) {
            return 1;
        }
        p++;
    }
    return 0;
}

int movie_add_ck(no, set)
int no;
int set;
{
    MOVIE_ADD *p = movie_add_tbl;
    int i;

    if (Quest_clear_bit_ck(no) == 0) {
        return 0xFF;
    }
    for (i = 0; i < 10; i++, p++) {
        if (p->quest == (s16)no && Omake_flag_ck(p->flag) == 0) {
            if (set) {
                Omake_flag_set(p->flag);
            }
            return i;
        }
    }
    return 0xFF;
}

void disp_mark(void)
{
    SPR2TF spr;

    SetFilterMode(1);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    spr.size[0] = 0xB9;
    spr.size[1] = 0xB9;
    spr.pos[0] = (0x280 - spr.size[0]) / 2;
    spr.pos[1] = (0x1C0 - spr.size[1]) / 2;
    spr.uv[0] = 0x95;
    spr.uv[1] = 0x69;
    spr.uv[2] = 0xDF;
    spr.uv[3] = 0xB3;
    spr.col = 0xFF808080;
    Put_2TF(spr.pos, spr.size);
}

void trans_result_0(void)
{
    Disp_back();
    disp_mark();
    disp_reward();
}

void trans_result_1(void)
{
    GAME_W *gw = &game_w;
    int i;
    s32 *p;

    Disp_back();
    disp_mark();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    p = card_list_frame_tbl_00357850;
    for (i = 0; i < 5; i++, p++) {
        DispFrameListA(*p, 0, -1, 0xFF);
    }
    if (gw->sub >= 6) {
        Disp_button(1.0f, 0, 0x32, 0x16D, 0x11A);
    }
}

void trans_result_2(void)
{
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
}

void trans_result_3(void)
{
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
    if (game_w.sub != 0) {
        Disp_button(1.0f, 0, 0x230, 0xF7, 0x11A);
    }
}

void end_fade_set(void)
{
    str_fadein_vol(0, 0x1E, 0);
}

int movie_add_check(GAME_W *t, int go)
{
    int r = movie_add_ck(quest_w.no);
    u8 *p;
    int i;

    if (r == 0xFF) {
        if (go) {
            t->mode = 6;
            t->step = 0;
            t->x03 = 0;
            all_reset();
        }
        return 1;
    }
    if (go) {
        t->step = 6;
        t->sub = 0;
        t->x03 = r;
        t->x04 = 0x5A;
        t->x06 = 1;
        str_play(1, 0xF);
        fade_set(2);
        p = card_prim;
        for (i = 0; i < 2; i++, p += 0x20) {
            memset(p, 0, 0x20);
        }
        *(void **)(card_prim + 0x14) = trans_result_3;
    }
    return 0;
}

void result_prog(void)
{
    GAME_W *t = &game_w;
    int i;
    int j;
    PLW *pl;

    switch (t->step) {
    case 0:
        se_stop_all();
        t->x0A = 0;
        for (i = 0; i < 2; i++) {
            memset(card_prim + i * 0x20, 0, 0x20);
        }
        if (game_w.x0D5 == 8) {
            t->step = 8;
            t->sub = 0;
            t->x04 = 0x96;
            t->x06 = 1;
            Quest_price_return();
            fade_set(2);
            *(void **)(card_prim + 0x14) = trans_result_2;
            break;
        }
        if (game_w.x0D5 == 7) {
            Quest_price_return();
            t->step = 10;
            fade_set(10);
            t->x04 = 0x1E;
            end_fade_set();
            return;
        }
        *(void **)(card_prim + 0x14) = trans_result_0;
        quest_price = 0;
        for (i = 0, j = 0; i < 20; i++, j += 4) {
            pl = &player_work[t->master];
            if (Item_data[pl->item[i].id][4] & 0x10) {
                Pl_item_stack(pl, pl->item[i].id, (s16)-pl->item[i].num);
            }
        }
        if (Quest_clear_ck(1) == 1) {
            if (Online_ck() == 1) {
                if (Quest_f_dra_ck((u8)quest_w.no) == 0) {
                    if (key_quest_ck(quest_w.no) != 1 || game_w.x21A != 0) {
                        Quest_clear_bit_set(quest_w.no);
                    }
                }
            } else {
                Quest_clear_bit_set(quest_w.no);
            }
            if (Quest_f_dra_ck((u8)quest_w.no) != 0) {
                if (quest_w.x34 == 0) {
                    quest_w.x14C = 0;
                    if (game_w.x21A != 0) {
                        Quest_clear_bit_set(0x67);
                        Quest_clear_bit_set(0x68);
                        Quest_clear_bit_set(0x69);
                        Quest_clear_bit_set(0x6A);
                    }
                }
                if (game_w.x21A != 0) {
                    USER_x1A = quest_w.x14C;
                }
            }
            remuneration_item_set();
            t->step++;
            t->sub = 0;
            t->x04 = 0x5A;
            reward_init();
            fade_set(2);
        } else {
            t->step = 2;
            ItemCopy_Pl2Ud(&player_work[t->master]);
            gold_init(t);
        }
        break;
    case 1:
        if ((s16)reward_mv() <= 0) {
            t->step++;
            ItemCopy_Pl2Ud(&player_work[t->master]);
            gold_init(t);
        }
        break;
    case 2:
        if (gold_main(t)) {
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            if (Online_ck() != 1) {
                if (movie_add_check(t, 0)) {
                    end_fade_set();
                }
            }
        }
        gold_disp(t);
        break;
    case 3:
        if (--t->x04 <= 0) {
            t->step++;
            if (result_init(t)) {
                return;
            }
            break;
        }
        gold_disp(t);
        break;
    case 4:
        if (result_main(t)) {
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            if (movie_add_check(t, 0)) {
                end_fade_set();
            }
        }
        result_disp(t);
        break;
    case 5:
        if (--t->x04 <= 0) {
            if (movie_add_check(t, 1)) {
                return;
            }
            break;
        }
        result_disp(t);
        break;
    case 6:
        if (t->x04 > 0) {
            t->x04--;
        } else {
            if (--t->x06 <= 0) {
                t->x06 = 0xF;
                t->sub ^= 1;
            }
        }
        if (t->x04 <= 0 && (Psw[2] & 0x20)) {
            t->step++;
            se_req(7, 0x13, 0);
            fade_set(1);
            t->x04 = 0x1E;
            end_fade_set();
        }
        add_disp(t);
        break;
    case 7:
        if (--t->x04 <= 0) {
            t->mode = 6;
            t->step = 0;
            all_reset();
            return;
        }
        add_disp(t);
        break;
    case 8:
        if (--t->x04 <= 0) {
            t->step++;
            fade_set(1);
        }
        error_disp();
        break;
    case 9:
        if (--t->x04 <= 0) {
            t->mode = 6;
            t->step = 0;
            all_reset();
            return;
        }
        error_disp();
        break;
    case 10:
        if (--t->x04 <= 0) {
            t->mode = 6;
            t->step = 0;
            all_reset();
            return;
        }
        break;
    }
    if (*(void **)(card_prim + 0x14) != 0) {
        add_prim2(ot8, card_prim, 0, 1);
    }
}

int gold_init(GAME_W *t)
{
    int i;
    u8 *p;

    t->sub = 0;
    t->x03 = 0;
    t->x08 = 0xE10;
    result_w.gold = 0;
    t->x06 = 0x14;
    p = card_prim;
    for (i = 0; i < 2; i++, p += 0x20) {
        memset(p, 0, 0x20);
    }
    if (Quest_clear_ck(1) == 1) {
        result_w.gold = quest_w.x14;
        if (Online_ck() == 1) {
            if (game_w.x21A != 0) {
                result_w.gold += quest_w.x94[1] * 2;
            }
        } else {
            result_w.gold += quest_w.x94[1];
        }
    }
    *(void **)(card_prim + 0x14) = trans_result_1;
    fade_set(2);
    return 0;
}

int gold_main(GAME_W *t)
{
    int amt;

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
                amt += 0x4D8;
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
            Gold_add(amt);
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
