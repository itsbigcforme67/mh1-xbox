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

void Quest_start(void)
{
    MISSION *m;
    s32 *p;
    int i;

    game_w.x0D0 = 0;
    game_w.x0D5 = 2;
    quest_w.no = game_w.quest = select_w.xAC;
    game_w.x1E7 = func_63AF40(game_w.quest);
    Ext_pick_point_init();
    for (i = 0; i < 32; i++) {
        quest_w.xBC[i].v = 0;
        quest_w.xBC[i].f = 0;
    }
    quest_w.x140 = 0;
    quest_w.x13C = 0;
    quest_w.x150 = 0;
    quest_w.x14F = 0;
    quest_w.x0B = 0;
    quest_w.xB4[0].a = 0;
    quest_w.xB4[0].b = 0;
    quest_w.xB4[1].a = 0;
    quest_w.xB4[1].b = 0;
    quest_w.xB4[2].a = 0;
    quest_w.xB4[2].b = 0;
    quest_w.xB4[3].a = 0;
    quest_w.xB4[3].b = 0;
    quest_w.xB4[game_w.master].b = Get_hunter_rank(&User_data);
    quest_w.xAE = 1;
    game_w.x28[0] = 0;
    game_w.x28[1] = 0;
    game_w.x28[2] = 0;
    game_w.x28[3] = 0;
    quest_w.x98[0].v = 0;
    quest_w.x98[0].f = 0;
    quest_w.x98[1].v = 0;
    quest_w.x98[1].f = 0;
    quest_w.x98[2].v = 0;
    quest_w.x98[2].f = 0;
    quest_w.x98[3].v = 0;
    quest_w.x98[3].f = 0;
    quest_w.x98[4].v = 0;
    quest_w.x98[4].f = 0;
    quest_w.xAC = 0;
    quest_w.xAD = 0;
    quest_w.x03 = 0;
    quest_w.x00 = 0;
    quest_w.x01 = 0;
    quest_w.x3A = 0;
    quest_w.x3C = 0;
    quest_w.x10 = 0;
    quest_w.x04 = 0;
    quest_w.x1C[0] = 0;
    quest_w.x24[0] = 0;
    quest_w.x1C[1] = 0;
    quest_w.x24[1] = 0;
    quest_w.x1C[2] = 0;
    quest_w.x24[2] = 0;
    quest_w.x1C[3] = 0;
    quest_w.x24[3] = 0;
    quest_w.x2C[0] = 0;
    quest_w.x30[0] = 0;
    quest_w.x2C[1] = 0;
    quest_w.x30[1] = 0;
    if (quest_w.no != 0) {
        if (quest_w.no < 0xC8) {
            load_file_mdl(mission_area, questName[quest_w.no]);
        }
        m = (MISSION *)mission_area;
        Quest_pl_stage_init(0xFF);
        quest_w.x94 = (MISSION2 *)(m->o[0] + (int)mission_area);
        quest_w.x10 = Quest_time_get(1);
        quest_w.xAF = quest_w.x10 / 9000;
        if (quest_w.xAF > 2) {
            quest_w.xAF = 2;
        }
        quest_w.x64 = m;
        quest_w.x38 = 0;
        quest_w.x6C = (s32 *)(m->o[4] + (int)mission_area);
        quest_w.x36 = 0;
        quest_w.x7C = (s32 *)(m->o[7] + (int)mission_area);
        quest_w.x70 = (s32 *)(m->o[12] + (int)mission_area);
        quest_w.x84 = (s32 *)(quest_w.x94->x18 + (int)mission_area);
        quest_w.x74 = (s32 *)(m->o[5] + (int)mission_area);
        quest_w.x78 = (s32 *)(m->o[6] + (int)mission_area);
        quest_w.x80 = (s32 *)(m->o[8] + (int)mission_area);
        quest_w.x8C = (s32 *)(m->o[10] + (int)mission_area);
        quest_w.x90 = (s32 *)(m->o[11] + (int)mission_area);
        quest_w.x14 = quest_w.x94->x08;
        quest_w.x18 = quest_w.x94->x0C;
        quest_w.x88 = (s32 *)(m->o[3] + (int)mission_area);
        quest_w.x40 = quest_w.x94->x00;
        quest_w.x14E = m->o[13];
        p = (s32 *)(m->o[9] + (int)mission_area);
        game_w.x2E = p[0];
        game_w.x2F = p[1];
        quest_em_init(quest_w.x94);
    } else {
        s16 st = select_w.x0A;

        game_w.x2E = 2;
        game_w.stage = st;
        game_w.x15 = st;
    }
}

void Quest_retire_set(void)
{
    if (Quest_clear_ck(1) == 0) {
        quest_w.x36 = -1;
        quest_w.x06 = game_w.x0D5 = 7;
        if (Online_ck() != 0) {
            net_send_sys(0xC, game_w.master);
        }
    }
}

void Quest_error_set2(void)
{
    game_w.x0D5 = 8;
    quest_w.x06 = 8;
    quest_w.x36 = -1;
}

void Quest_error_set(void)
{
    if (Quest_clear_ck(1) == 0) {
        Quest_error_set2();
    }
}

void Quest_pl_stage_init(arg)
int arg;
{
    u8 *tbl;

    if (quest_w.no == 0) {
        if ((u16)arg == 0xFF) {
            game_w.stage = select_w.x0A;
            game_w.x15 = select_w.x0A;
        } else {
            player_work[(u16)arg].stg = game_w.stage;
        }
        return;
    }
    tbl = mission_area + *(s32 *)(mission_area + 4);
    if ((u16)arg == 0xFF) {
        game_w.x15 = game_w.stage = *(s32 *)(tbl + game_w.master * 0x10);
    } else {
        player_work[(u16)arg].stg = *(s32 *)(tbl + (u16)arg * 0x10);
    }
}

void Quest_restart(void)
{
    s32 *p = quest_w.x6C;
    s16 i = 0;

    if (Quest_clear_ck(1) == 0) {
        for (;;) {
            if (*(s16 *)p == 0x16) {
                break;
            }
            p += 2;
            i++;
        }
        game_w.x11 = 0;
        quest_w.x36 = i + 1;
    }
}

int Quest_clear_ck(arg)
int arg;
{
    int v = game_w.x0D5;

    if (v == 3 || v == 4) {
        return 1;
    }
    if ((v == 5 || v == 6 || v == 7) && arg != 0) {
        return -1;
    }
    return 0;
}

int Quest_time_get(arg)
int arg;
{
    switch (arg) {
    case 0:
        if (quest_w.x10 < 0) {
            return 0;
        }
        return quest_w.x10;
    case 1:
        return quest_w.x94->x10;
    default:
        if (quest_w.x10 < 0) {
            return 0;
        }
        return quest_w.x10;
    }
}

void Quest_em_init_set(arg)
int arg;
{
    station_em_set();
    Quest_next_em_set(arg);
}

int Quest_enemy_revival_ck(arg)
int arg;
{
    QEM *q;

    if (Quest_clear_ck(0) != 0) {
        return 0;
    }
    q = em_work_serch(arg);
    if (q != 0) {
        if (q->x2E & 1) {
            return 0;
        }
        if (q->x04 > 0) {
            return 1;
        }
    }
    return 0;
}

int Quest_enemy_revival_set(e)
EMW *e;
{
    QEM *q = em_work_serch(e);

    if (q != 0) {
        if (q->x2E & 1) {
            return 0;
        }
        q->x2E = 2;
        q->x08 = e->x302;
        e->pos[0] = q->pos[0];
        e->pos[1] = q->pos[1];
        e->pos[2] = q->pos[2];
        e->ang[1] = q->x1C;
        e->stg = q->x07;
        return q->x04;
    }
    return 0;
}

void Quest_enemy_die(e)
EMW *e;
{
    QEM *q;

    if ((q = em_work_serch(e)) != 0) {
        if (!(q->x2E & 5)) {
            if (q->x05 != 0) {
                quest_w.x34--;
            }
            q->x04--;
            if (q->x04 > 0) {
                q->x2E = 4;
            } else {
                q->x0A = -1;
                q->x2E = 1;
            }
            if (e->x8C3 == 0) {
                q_net_send_em_die(q, e);
            }
            if (quest_enemy_ck_sub(&quest_w, e->kind) != 0) {
                quest_w.x3C = e;
            }
        }
    }
}

void Quest_enemy_escape(e)
EMW *e;
{
    QEM *q = em_work_serch(e);

    if (q != 0 && !(q->x2E & 1)) {
        q->x08 = e->x302;
        q->x0A = -1;
    }
}

void Quest_enemy_capture(e)
EMW *e;
{
    QEM *q;

    if ((q = em_work_serch(e)) != 0 && !(q->x2E & 8)) {
        em_capture_conv(e->kind);
        if (!(q->x2E & 5)) {
            q_net_send_em_capture(q, e);
            if (q->x05 != 0) {
                quest_w.x34--;
            }
            q->x04--;
            if (q->x04 > 0) {
                q->x2E = 4;
            } else {
                q->x0A = -1;
                q->x2E = 1;
            }
        }
        q->x2E |= 8;
    }
}

void Quest_enemy_hagi_set(a, b)
int a;
int b;
{
    quest_w.x13C = quest_w.x13C | b;
}

int Quest_remuneration_calc(void)
{
    int v;

    if (quest_w.x14 <= 0) {
        return 2;
    }
    v = quest_w.x14 - quest_w.x18;
    quest_w.x14 = v;
    if (v <= 0) {
        if (Quest_clear_ck(1) == 0) {
            quest_failed_ptr_set(0);
        }
        if (quest_w.x14 <= 0) {
            quest_w.x14 = 0;
        }
        return 1;
    }
    return 0;
}

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
