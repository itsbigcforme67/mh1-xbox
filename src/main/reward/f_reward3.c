/* Quest result screen, third part (0x292510-0x292D34): result display,
 * error and "bonus movie added" screens, reward screen init. */
#include "reward.h"

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
