/* Quest result / reward screen (SLPM_654.95 main, f_reward range).
 * First part, 0x290E80-0x29130C: lookup of the "key quest" and "movie add"
 * tables, the result screen's backdrop/frame drawing, and the check that
 * unlocks a bonus movie after a quest. Meanings are guesses. */
#include "f_game.h"
#include "plf.h"

typedef struct QUEST_WR {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest number */
    u8 _pad0A[0x14 - 0xA];
    s32 x14;            /* 0x14 reward money (gold_init) */
    u8 _pad18[0x34 - 0x18];
    s16 x34;            /* 0x34 */
    u8 _pad36[0x64 - 0x36];
    s32 *x64;           /* 0x64 quest data pointer (result_init) */
    u8 _pad68[0x94 - 0x68];
    s32 *x94;           /* 0x94 */
    u8 _pad98[0x14C - 0x98];
    s16 x14C;           /* 0x14C */
} QUEST_WR;
extern QUEST_WR quest_w;

typedef struct MOVIE_ADD {
    s16 quest;          /* 0x00 quest number */
    s16 flag;           /* 0x02 omake flag set when the movie is shown */
} MOVIE_ADD;

extern s16 key_quest_tbl[];
extern MOVIE_ADD movie_add_tbl[];
extern s32 card_list_frame_tbl_00357850[];
extern u8 card_prim[];



int Quest_clear_bit_ck(s16);
int movie_add_ck();
int Omake_flag_ck();
void Omake_flag_set();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void Put_2TF();
void Disp_back();
void disp_reward();
void DispFrameListA();
void Disp_button(f32, int, int, int, int);
void SetTrnslMode();
void flSetRenderState();
void str_fadein_vol();
void str_play();
void *memset(void *, int, int);

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

typedef struct SPR2TF {
    s16 pos[2];         /* 0x00 top-left */
    s16 size[2];        /* 0x04 */
    u32 col;            /* 0x08 */
    s16 uv[4];          /* 0x0C */
} SPR2TF;

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

extern s32 quest_price;
extern u16 Psw[];
extern u8 ot8[4];
int Quest_f_dra_ck();
void Quest_clear_bit_set();
void Quest_price_return();
int Quest_clear_ck();
int Online_ck();
void se_stop_all();
void se_req();
void remuneration_item_set();
void reward_init();
s16 reward_mv();
void ItemCopy_Pl2Ud();
void fade_set();
int gold_init();
int gold_main();
void gold_disp();
int result_init();
int result_main();
void result_disp();
void add_disp();
void error_disp();
void add_prim2();
#define USER_x1A (*(s16 *)((u8 *)&User_data + 0x1A))

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

typedef struct RESULT_W {
    s32 gold;           /* 0x00 points/money left to hand out on the result screen */
    u8 rank_new;        /* 0x04 hunter rank now */
    u8 rank_old;        /* 0x05 rank when the screen started */
    u8 _pad06[2];
} RESULT_W;
extern RESULT_W result_w;       /* small data */
int Get_hunter_rank();
int Hunter_point_add();
void Gold_add();
int sprintf(char *, const char *, ...);
int strlen();
int strlen_sp();
void font_print_sp();
char *Quest_str_get();
extern char lit_465_003865F0[];
extern char lit_466_00386610[];
extern char lit_467_00386620[];
extern char lit_468_00386630[];
extern char lit_469_00386648[];
extern char lit_470_00386658[];
extern char lit_471_00386668[];
extern char lit_472_00386678[];
extern char lit_473_00386690[];
extern char lit_474_003866B0[];
extern char lit_475_003866D0[];

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
extern char lit_632_00386730[];
extern char lit_633_00386750[];
extern char lit_634_00386770[];
extern char lit_635_00386790[];
extern char lit_636_003867B0[];
extern char lit_637_003867D0[];
extern char lit_638_003867F0[];
extern char lit_639_00386810[];
extern char lit_640_00386830[];
extern char lit_641_00386850[];
extern char lit_642_00386880[];
extern char lit_643_003868B0[];
extern char lit_658_00386910[];
extern char lit_659_00386920[];
extern char lit_660_00386940[];
extern char lit_661_00386970[];
extern char lit_662_00386990[];
extern char *hunter_appellation[];

#define CENTER_X(buf) ((s16)(0x140 - strlen_sp(buf) * 18 / 4))

void result_disp(GAME_W *t)
{
    char buf[0x70];
    int v;
    int n;

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    switch (t->sub) {
    default:
    case 6:
        flfntLocate(0x4A, 0x16F);
        font_print_sp(lit_632_00386730);
    case 4:
    case 5:
        sprintf(buf, lit_633_00386750, result_w.gold);
        flfntLocate(CENTER_X(buf), 0x103);
        font_print_sp(lit_467_00386620, buf);
    case 3:
        n = player_work[game_w.master].work91E;
        v = 0;
        if (n > 0) {
            v = n * -10;
        }
        flfntLocate(0x7A, 0xCD);
        font_print_sp(lit_634_00386770, v);
        if (player_work[game_w.master].work91E > 0) {
            flfntLocate(0x176, 0xCD);
            font_print_sp(lit_635_00386790, player_work[game_w.master].work91E);
        }
    case 2:
        flfntLocate(0x176, 0x97);
        if (Quest_clear_ck(1) == 1) {
            v = quest_w.x64[0xE];
            font_print_sp(lit_636_003867B0);
        } else {
            v = quest_w.x64[0xF];
            font_print_sp(lit_637_003867D0);
        }
        flfntLocate(0x7A, 0x97);
        font_print_sp(lit_638_003867F0, v);
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
        flfntLocate(CENTER_X(buf), 0x46);
        font_print_sp(lit_467_00386620, buf);
        sprintf(buf, lit_639_00386810, *(s32 *)((u8 *)&User_data + 0x1C));
        flfntLocate(CENTER_X(buf), 0x127);
        font_print_sp(lit_467_00386620, buf);
        sprintf(buf, lit_640_00386830, result_w.rank_new, hunter_appellation[result_w.rank_new]);
        flfntLocate(CENTER_X(buf), 0x14B);
        font_print_sp(lit_467_00386620, buf);
        switch (t->x03) {
        case 0:
            break;
        case 1:
            font_set_palette(5);
            sprintf(buf, lit_641_00386850);
            flfntLocate(CENTER_X(buf), 0x139);
            font_print_sp(lit_467_00386620, buf);
            break;
        case 2:
            font_set_palette(5);
            sprintf(buf, lit_642_00386880);
            flfntLocate(CENTER_X(buf), 0x139);
            font_print_sp(lit_467_00386620, buf);
            break;
        default:
            font_set_palette(5);
            sprintf(buf, lit_643_003868B0);
            flfntLocate(CENTER_X(buf), 0x139);
            font_print_sp(lit_467_00386620, buf);
            break;
        }
        break;
    }
}

void error_disp(void)
{
    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x113, 0x61);
    font_print_sp(lit_658_00386910);
    flfntLocate(0xB0, 0x85);
    font_print_sp(lit_659_00386920);
    flfntLocate(0xB0, 0x97);
    font_print_sp(lit_660_00386940);
    flfntLocate(0xB0, 0xA9);
    font_print_sp(lit_661_00386970);
    flfntLocate(0xB0, 0xBB);
    font_print_sp(lit_662_00386990);
}

extern char lit_674_003869B0[];
extern char lit_675_003869C0[];
extern char lit_676_003869E0[];
extern char *movie_add_str_tbl[];

void add_disp(GAME_W *t)
{
    char buf[0x70];

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x11C, 0x61);
    font_print_sp(lit_674_003869B0);
    sprintf(buf, lit_675_003869C0);
    flfntLocate(CENTER_X(buf), 0x85);
    font_print_sp(lit_467_00386620, buf);
    sprintf(buf, movie_add_str_tbl[t->x03]);
    flfntLocate(CENTER_X(buf), 0x97);
    font_print_sp(lit_467_00386620, buf);
    sprintf(buf, lit_676_003869E0);
    flfntLocate(CENTER_X(buf), 0xA9);
    font_print_sp(lit_467_00386620, buf);
}

#include "reward.h"


void reward_init(void)
{
    *(s32 *)&reward_w = 0;
    reward_w.x4 = 0;
    reward_w.xB = -1;
    reward_w.xC = 0xFF;
    reward_w.x6 = 0xE10;
    PitMenu.x11 = 6;
    PitMenu.x10 = 0;
    reward_key_repeat(0, 0);
}
