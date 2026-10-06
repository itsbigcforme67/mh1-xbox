/* SLPM_654.95 0x292D40-0x2932BC: reward_mv, the reward screen input handling (item list, pick, swap with the pouch). */
#include "reward.h"

int Pl_item_num_ck3();
void ListSelect();
void Menu_select_mv();
void reward_cursor_mv();

s16 reward_mv(void)
{
    REWARD_W *w = &reward_w;
    PLW *pl = &player_work[game_w.master];
    u32 keys;
    u16 k;
    int id;
    s16 n;
    int r;
    PL_ITEM tmp;

    reward_w.x6--;
    keys = (Psw[2] | (u16)reward_key_repeat(Psw[2], Psw[0])) & 0xFFFF;
    switch (reward_w.x0) {
    case 0:
        ListSelect(&w->x1, keys, 2);   /* a2 = 2 left over from the switch compare (asm 0x292DB8) */
        if ((u16)keys & 0x20) {
            if (w->x1 == 0) {
                PitMenu.x12 = 0;
                PitMenu.x10 = 1;
                w->x0++;
                w->x3 = 0;
                w->x2 = 0;
            } else {
                w->x0 = 2;
                w->x6 = 0;
            }
            se_req(7, 0x13, 0);
        }
        break;
    case 1:
        switch (w->x2) {
        case 0:
            if (w->x3 != 0) {
                if ((u16)keys & 0x240) {
                    w->x3 = 0;
                    se_req(7, 0x14, 0);
                }
                keys = (u16)(keys & 0xFFBF);
            } else if ((u16)keys & 0x200) {
                w->x3 = 1;
                se_req(7, 9, 0);
            }
            k = keys;
            if (k & 0x40) {
                PitMenu.x10 = 0;
                w->x0 = 0;
                se_req(7, 0x14, 0);
                break;
            }
            PitMenu.x12 = 0;
            reward_cursor_mv(&w->x4, keys);
            id = game_w.reward_item[w->x4].id;
            n = Pl_item_num_ck3(pl, id);
            if (n == 0xFF || n == 0) {
                w->xB = 0;
            } else {
                w->xB = -1;
            }
            if (k & 0x20) {
                if ((u16)id != 0 && w->xB < 0) {
                r = Pl_item_stack(pl, id, game_w.reward_item[w->x4].num);
                switch ((u16)r) {
                case 0:
                case 1:
                    game_w.reward_item[w->x4].id = 0;
                    game_w.reward_item[w->x4].num = 0;
                    se_req(7, 0x19, 0);
                    break;
                case 2:
                    game_w.reward_item[w->x4].num -= n;
                    se_req(7, 0x19, 0);
                    break;
                case 3:
                    se_req(7, 0x15, 0);
                    break;
                case 5:
                    PitMenu.x12 = 1;
                    w->x5 = 0;
                    w->x2 = 1;
                    w->x3 = 0;
                    se_req(7, 0x13, 0);
                    break;
                }
                } else {
                    se_req(7, 0x15, 0);
                }
            }
            break;
        case 1:
            if (w->x3 != 0) {
                if ((u16)keys & 0x240) {
                    w->x3 = 0;
                    se_req(7, 0x14, 0);
                }
                keys = (u16)(keys & 0xFFBF);
            } else {
                k = keys;
                if (k & 0x200) {
                    w->x3 = 2;
                    se_req(7, 9, 0);
                }
                if (k & 0x40) {
                    PitMenu.x12 = 0;
                    w->x2 = 0;
                    se_req(7, 0x14, 0);
                    break;
                }
            }
            PitMenu.x12 = 1;
            Menu_select_mv(&w->x5, keys, 0x14);
            if ((u16)keys & 0x20) {
                tmp = game_w.reward_item[w->x4];
                game_w.reward_item[w->x4] = pl->item[w->x5];
                pl->item[w->x5] = tmp;
                PitMenu.x12 = 2;
                w->x2++;
                w->x3 = 0;
                se_req(7, 0x13, 0);
            }
            break;
        case 2:
            if ((u16)keys & 0x20) {
                PitMenu.x12 = 0;
                w->xC = 0xFF;
                w->x2 = 0;
                se_req(7, 0x13, 0);
            } else {
                PitMenu.x12 = 2;
                w->xC++;
            }
            break;
        }
        break;
    case 2:
        w->x6 = 0;
        break;
    }
    return w->x6;
}
