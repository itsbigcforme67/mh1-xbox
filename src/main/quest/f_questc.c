/* Quest control (SLPM_654.95 main, f_quest range 0x226C30-...): start-up of a
 * quest (Quest_start), the retire/error state, remaining monsters and the
 * reward. Meanings are guesses. */
#include "quest.h"

extern s16 questName[];
int func_63AF40();
void load_file_mdl();
void Quest_pl_stage_init();
int Quest_time_get();
void quest_em_init();
void Ext_pick_point_init();
int Get_hunter_rank();
int Online_ck();
void net_send_sys();
void station_em_set();
void Quest_next_em_set();
void *em_work_serch();
void q_net_send_em_die();
void q_net_send_em_capture();
int quest_enemy_ck_sub();
void em_capture_conv();
void quest_failed_ptr_set();
int Quest_clear_ck();

extern char lit_656_0036B1F0[];
extern char lit_657_0036B210[];
extern char lit_658_0036B230[];
extern char lit_659_0036B250[];
extern char lit_660_0036B270[];
s16 quest_condition_prog();
void Quest_timer_calc();

void func_63ACA0();
void set01_set();
void set01_set2();

#define EM8(e, o) (*(s8 *)((u8 *)(e) + (o)))
void *pull_enemy_work();
void enemy_mv();

s32 *Em_data_com_adrs_get();
s32 *Em_data_st_adrs_get();
void func_5A8170();
void push_em_work();
void release_enemy_model();
void enemy_insurance_sub();
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))

int Quest_condition_judging(void)
{
    int i;
    int sp3C;
    s8 t;
    int m;
    PLW *pl;
    u8 *rank;

    m = game_w.master;
    pl = &player_work[m];
    for (i = 0; i < 4; i++) {
        if (game_w.pl_state[i] != 1) {
            quest_w.x140 |= 1 << (i + 8);
        }
    }
    if (quest_w.no == 0) {
        return 0;
    }
    t = quest_w.x06;
    if (t == 7) {
        return -2;
    }
    if (t == 8) {
        return -2;
    }
    switch (quest_w.x00) {
    case 0:
        if (t == 4) {
            return 1;
        }
        if (t == 6) {
            return -1;
        }
        if (quest_w.x36 == -1) {
            return 1;
        }
        quest_w.x36 = quest_condition_prog(pl, quest_w.x36, &sp3C, t);
        if (game_w.x1E7 != 0) {
            func_63ACA0();
        }
        return 0;
    case 1:
        switch (quest_w.x01) {
        case 0:
            quest_w.x01++;
            quest_w.x10 = 0x96;
            break;
        case 1:
            Quest_timer_calc(1, 2, quest_w.x00, t);
            if (quest_w.x10 <= 0) {
                quest_w.x01++;
                if (quest_w.x40 & 2) {
                    quest_w.x10 = 0x258;
                    if (Online_ck() == 1) {
                        set01_set2(lit_656_0036B1F0);
                    } else {
                        set01_set2(lit_657_0036B210);
                    }
                } else {
                    quest_w.x10 = 0x708;
                    if (Online_ck() == 1) {
                        set01_set2(lit_658_0036B230);
                    } else {
                        set01_set2(lit_659_0036B250);
                    }
                }
            }
            break;
        case 2:
            Quest_timer_calc(1, 2, quest_w.x00, t);
            if (quest_w.x10 <= 0) {
                quest_w.x01 = 0;
                quest_w.x00++;
                set01_set2(lit_660_0036B270);
                set01_set(0, 0xF, 0);
            }
            break;
        }
        break;
    case 2:
        switch (quest_w.x01) {
        case 0:
            quest_w.x01++;
            quest_w.x06 = 4;
            quest_w.x14 = quest_w.x14 / game_w.pl_num;
            rank = (u8 *)quest_w.xB4 + (m & 0xFF) * 2;
            *rank = Get_hunter_rank(&User_data);
            quest_w.x182 = 0;
            quest_w.x181 = 0xC;
            quest_w.x184 = *rank;
            quest_w.x186 = 0;
            net_send_sys(6, m);
            quest_w.x140 |= 1 << game_w.master;
            break;
        case 1:
            for (i = 0; i < 4; i++) {
                if (game_w.pl_state[i] != 1) {
                    quest_w.x140 |= 1 << i;
                }
            }
            if ((quest_w.x140 & 0xF) == 0xF || game_w.pl_state[m] != 1) {
                quest_w.x182 = 0;
                quest_w.x181 = 0xD;
                quest_w.x01++;
                quest_w.x184 = (u32)quest_w.x13C >> 16;
                quest_w.x186 = quest_w.x13C;
                net_send_sys(6, m);
                quest_w.x140 |= 1 << (game_w.master + 4);
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                if (game_w.pl_state[i] != 1) {
                    quest_w.x140 |= 1 << (i + 4);
                }
            }
            if ((u8)quest_w.x140 == 0xFF || game_w.pl_state[m] != 1) {
                quest_w.x01++;
                game_w.x0D5 = 4;
            }
            break;
        case 3:
            break;
        }
        break;
    case 3:
        break;
    }
    return 0;
}
