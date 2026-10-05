/* Quest control (SLPM_654.95 main, f_quest range 0x226C30-...): start-up of a
 * quest (Quest_start), the retire/error state, remaining monsters and the
 * reward. Meanings are guesses. */
#include "quest.h"
#include "plf.h"

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

#define EM8(e, o) (*(s8 *)((u8 *)(e) + (o)))
void *pull_enemy_work();
void enemy_mv();

EMW *Em_direct_set(q)
QEM *q;
{
    EMW *em;
    int i;
    u8 *p;

    for (i = 0, p = (u8 *)&game_w; i < 4; i++, p++) {
        if (q->id == p[0x28]) {
            break;
        }
    }
    if (i >= 4) {
        return 0;
    }
    em = pull_enemy_work(p);
    if (em != 0) {
        em->mdl_no = i;
        em->kind = q->id;
        em->type = q->x02;
        EM8(em, 0x9EB) = q->x2C;
        em->stg = game_w.stage;
        em->hungry = q->x0C;
        em->thirst = q->x10;
        em->x8A0 = q->x14;
        EM8(em, 0x95B) = q->x06;
        em->pos[0] = q->pos[0];
        em->pos[1] = q->pos[1];
        em->pos[2] = q->pos[2];
        em->ang[1] = q->x1C;
        enemy_mv(em);
        q->x0A = em->id;
        if (q->x08 == -1) {
            q->x08 = em->x302;
            if (q->x05 != 0) {
                quest_w.xB0 = em;
            }
        } else {
            em->x302 = q->x08;
        }
    }
    return em;
}

s32 *Em_data_com_adrs_get();
s32 *Em_data_st_adrs_get(s32 *, int, int, s8);
void func_5A8170();
void push_em_work();
void release_enemy_model();
void enemy_insurance_sub();
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))

void Quest_next_em_clr(arg0, arg1)
int arg0;
int arg1;
{
    s32 *list;
    EMW *e;
    PLW *pl;
    int i;
    int found;
    QEM *q;
    s32 *p;
    int j;
    int v;
    u8 *g;
    s32 *p2;
    int v2;
    s32 *t74;

    e = em_work;
    pl = player_work;
    list = Em_data_com_adrs_get(quest_w.x78, 0);
    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0) {
            if (list != 0) {
                p = list;
                for (;;) {
                    v = *p;
                    if (v == -1) {
                        found = 0;
                        break;
                    }
                    if (e->x765 != 0) {
                        found = 1;
                        break;
                    }
                    if (e->kind == v) {
                        found = 1;
                        break;
                    }
                    p++;
                }
            } else {
                found = 0;
            }
            if ((q = em_work_serch(e)) != 0 && q->x2E != 1 && found == 0) {
                if (e->kind == 0x17) {
                    func_5A8170(e);
                }
                if (e->x04 >= 2 || e->mode == 5) {
                    Quest_enemy_die(e);
                    if (q->x2E & 4) {
                        q->x2E = 2;
                        q->x08 = -1;
                    }
                } else {
                    q->x08 = e->x302;
                }
                q->x0A = -1;
            }
            if (found == 0) {
                if (e->kind == 0x12) {
                    for (j = 0; j < game_w.pl_num; j++, pl++) {
                        if (pl->be_flag != 0 && pl->x824 == e) {
                            pl->x824 = 0;
                        }
                    }
                }
                push_em_work(e);
            }
        }
    }
    enemy_insurance_sub(arg0);
    for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
        v = g[0x28];
        if (v > 0) {
            if (list != 0) {
                p2 = list;
                for (;;) {
                    v2 = *p2;
                    if (v2 == -1) {
                        found = 0;
                        break;
                    }
                    if (v == v2) {
                        found = 1;
                        break;
                    }
                    p2++;
                }
                if (found != 0) {
                    continue;
                }
            }
            t74 = quest_w.x74;
            list = Em_data_st_adrs_get(t74, arg1, 0, quest_w.x3A);
            if (list != 0) {
                for (;;) {
                    if (*list == -1) {
                        found = 0;
                        break;
                    }
                    if (g[0x28] == *list) {
                        found = 1;
                        break;
                    }
                    list++;
                }
                if (found != 0) {
                    continue;
                }
            }
            release_enemy_model((s16)i);
            g[0x28] = 0;
        }
    }
}

void em_next_tbl_ck();
void func_5589F0();
void em_create_model();
void em_herb_set();
EMW *Em_direct_set();

void Quest_next_em_set(n)
int n;
{
    s32 *l0;
    QEM *l1;
    int i;
    int v;
    u8 *g;

    em_next_tbl_ck();
    func_5589F0(n);
    if (n == game_w.x2F) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (g[0x28] <= 0) {
                game_w.x28[i] = 0x12;
                em_create_model(i);
                break;
            }
        }
    }
    if (n == 5 || n == 0x10 || n == 0x29) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (g[0x28] <= 0) {
                game_w.x28[i] = 0xA;
                em_create_model(i);
                break;
            }
        }
    }
    l0 = Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A);
    l1 = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
    if (l0 != 0) {
        while (*l0 != -1) {
            v = *l0;
            for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
                if (g[0x28] == v) {
                    break;
                }
            }
            if (i < 4) {
                l0++;
                continue;
            }
            for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
                if (g[0x28] <= 0) {
                    ((u8 *)&game_w)[0x28 + i] = v;
                    em_create_model(i);
                    break;
                }
            }
            l0++;
        }
        while (l1->id >= 0) {
            if (!(l1->x2E & 1)) {
                Em_direct_set(l1);
                l1->x07 = game_w.stage;
            }
            l1 = (QEM *)((u8 *)l1 + 0x3C);
        }
    }
    if (n == 5 || n == 0x10 || n == 0x29) {
        em_herb_set();
    }
}

extern s8 item_regained_tbl[8];
char *func_5C5E20();
void adx_se_set();
s16 Share_item_num_ck();
int share_item_ck_ck();
s16 Quest_share_item_num_ck();
int Share_item_stack();

char *Quest_str_get(int n)
{
    if (game_w.x1DC) {
        return func_5C5E20();
    }
    return (char *)(quest_w.x84[n] + (int)mission_area);
}

s16 stolen_item_num_ck(item)
u16 item;
{
    s16 i;
    u8 *q;

    for (i = 0, q = (u8 *)&quest_w; i < 5; i++, q += 4) {
        if (*(u16 *)(q + 0x98) == item) {
            return quest_w.x98[i].f;
        }
    }
    return 0;
}

int stolen_item_stack(int item, s16 num)
{
    s16 i;
    u16 r;
    s8 mx;
    u8 *q;

    if (stolen_item_num_ck(item) == 0) {
        r = 5;
        for (i = 0, q = (u8 *)&quest_w; i < 5; i++, q += 4) {
            if (*(u16 *)(q + 0x98) == 0 && num > 0) {
                quest_w.x98[i].v = item;
                quest_w.x98[i].f = num;
                r = 0;
                break;
            }
        }
        if (r == 5) {
            quest_w.x98[quest_w.xAC].v = item;
            quest_w.x98[quest_w.xAC].f = num;
            quest_w.xAC++;
            if (quest_w.xAC >= 5) {
                quest_w.xAC = 0;
            }
        }
    } else {
        for (i = 0, q = (u8 *)&quest_w; i < 5; i++, q += 4) {
            if (*(u16 *)(q + 0x98) == (u16)item) {
                mx = ((s8 *)Item_data)[((u16)item << 4) + 3];
                if (num > 0 && quest_w.x98[i].f >= mx) {
                    quest_w.x98[i].f = mx;
                    r = 3;
                } else {
                    quest_w.x98[i].f += (s8)num;
                    if (quest_w.x98[i].f <= 0) {
                        r = 4;
                        quest_w.x98[i].v = 0;
                        quest_w.x98[i].f = 0;
                    } else if (mx < quest_w.x98[i].f) {
                        quest_w.x98[i].f = mx;
                        r = 2;
                    } else {
                        r = 1;
                    }
                }
                break;
            }
        }
    }
    return r;
}

void Item_stolen(pl, item, num)
void *pl;
int item;
s16 num;
{
    game_w.xCC = item;
    game_w.xCE = num;
    game_w.x0D0 = 1;
    stolen_item_stack(item, -num);
    quest_w.xAD = 1;
}

void Item_regained(pl)
PLW *pl;
{
    int i;
    int found = 0;
    int r;
    u8 *p;

    if (quest_w.xAD != 0) {
    r = item_regained_tbl[(u16)ran_suu(0) & 7];
    if (r != 2) {
        if (r != 1) {
            for (i = 0, p = (u8 *)&quest_w; i < 5; i++, p += 4) {
                if (*(u16 *)(p + 0x98) != 0) {
                    switch ((u16)Pl_item_stack(pl, *(u16 *)(p + 0x98), *(s8 *)(p + 0x9A))) {
                    case 0:
                    case 1:
                    case 2:
                        if (Pl_master_ck(pl) == 1) {
                            set01_set(1, 0, *(s16 *)(p + 0x98));
                        }
                        found = 1;
                        break;
                    case 3:
                        if (Pl_master_ck(pl) == 1) {
                            set01_set(1, 3, *(s16 *)(p + 0x98));
                        }
                        found = 1;
                        break;
                    }
                }
            }
            if (found == 0 && Pl_master_ck(pl) == 1) {
                set01_set(0, 0, 0);
            }
        } else {
            switch ((u16)Pl_item_stack(pl, 0x59, 1)) {
            case 0:
            case 1:
            case 2:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(1, 0, 0x59);
                }
                break;
            case 3:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(1, 3, 0x59);
                }
                break;
            default:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(0, 0, 0);
                }
                break;
            }
        }
    } else if (Pl_master_ck(pl) == 1) {
        set01_set(0, 0, 0);
    }
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
    return;
    }
    if (Pl_master_ck(pl) == 1) {
        set01_set(0, 0, 0);
    }
}

int Share_item_stack(pl, item, num)
PLW *pl;
int item;
s16 num;
{
    s16 room;
    s16 i;
    int have;
    s16 base;

    have = (s16)Share_item_num_ck(item, share_item_ck_ck());
    room = quest_w.x24[Quest_share_item_num_ck(item)] - have;
    if (room <= 0) {
        return 0;
    }
    i = 0;
    if (Share_item_num_ck(item, 1) == 0) {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == 0) {
                pl->share[i].id = item;
                if (num >= room) {
                    num = room;
                }
                pl->share[i].num = (s8)num;
                Pl_item_stack(pl, item, -num);
                return (u16)num;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == (u16)item) {
                if (num >= room) {
                    num = room;
                }
                pl->share[i].num += (s8)num;
                Pl_item_stack(pl, item, -num);
                return (u16)num;
            }
        }
    }
    return (u16)num;
}

int Net_Share_item_stack(pl, item, num)
PLW *pl;
int item;
s16 num;
{
    s16 room;
    s16 i;
    int have;
    s16 cur;
    int r;

    have = (s16)Share_item_num_ck(item, share_item_ck_ck());
    room = quest_w.x24[Quest_share_item_num_ck(item)] - have;
    if (room <= 0) {
        return 2;
    }
    cur = 0;
    for (i = 0; i < 4; i++) {
        if (pl->share[i].id == (u16)item) {
            cur = pl->share[i].num;
        }
    }
    r = 0;
    if (cur == 0) {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == 0) {
                pl->share[i].id = item;
                if (num >= room) {
                    r = 2;
                }
                pl->share[i].num = (s8)num;
                return r;
            }
        }
        return 0;
    }
    r = 1;
    for (i = 0; i < 4; i++) {
        if (pl->share[i].id == (u16)item) {
            if (num >= room) {
                r = 2;
            }
            pl->share[i].num = (s8)num;
            return r;
        }
    }
    return 1;
}

s16 Share_item_num_ck(item, mode)
int item;
int mode;
{
    int i;
    int j;
    s16 sum = 0;
    PLW *pl;

    switch (mode) {
    case 0:
        for (i = 0; i < game_w.pl_num; i++) {
            pl = &player_work[i];
            for (j = 0; j < 4; j++) {
                if (pl->share[j].id == (u16)item) {
                    sum += pl->share[j].num;
                }
            }
        }
        return sum;
    case 1:
        pl = &player_work[game_w.master];
        for (j = 0; j < 4; j++) {
            if (pl->share[j].id == (u16)item) {
                return pl->share[j].num;
            }
        }
        return 0;
    default:
        pl = &player_work[mode - 2];
        for (j = 0; j < 4; j++) {
            if (pl->share[j].id == (u16)item) {
                return pl->share[j].num;
            }
        }
        return 0;
    }
}

int share_item_ck_ck(void)
{
    return (quest_w.x40 & 0x100) != 0;
}

s16 Quest_share_item_num_ck(item)
int item;
{
    s16 i;
    u8 *q;

    for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
        if (*(s16 *)(q + 0x1C) == (u16)item) {
            return i;
        }
    }
    return -1;
}

void Share_item_conv(pl)
PLW *pl;
{
    int i;
    int j;
    int any;
    u8 *q;

    if (Quest_clear_ck(1) == 0) {
        any = 0;
        for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
            if (*(s16 *)(q + 0x1C) != 0) {
                for (j = 0; j < 20; j++) {
                    if (pl->item[j].id == *(s16 *)(q + 0x1C) && (u16)Share_item_stack(pl, pl->item[j].id, pl->item[j].num) > 0) {
                        any = 1;
                        quest_w.x182 = 0;
                        quest_w.x181 = 9;
                        quest_w.x184 = *(s16 *)(q + 0x1C);
                        quest_w.x186 = Share_item_num_ck((u16) * (s16 *)(q + 0x1C), 1);
                        net_send_sys(6, game_w.master);
                        set01_set(1, 8, *(s16 *)(q + 0x1C));
                    }
                }
            }
        }
        if (any != 0) {
            adx_se_set(pl, 7);
        }
    }
}

void Ext_pick_point_init(void)
{
    s8 *t;
    STIEM *s;
    int i;

    quest_w.x3B = 0;
    s = StiEM_data;
    t = stiem_stack_tbl;
    for (i = 0; i < 20; i += 5) {
        s[0].id = 0xFFFF;
        t[0] = -1;
        s[1].id = 0xFFFF;
        t[1] = -1;
        s += 2;
        s[0].id = 0xFFFF;
        t[2] = -1;
        s[1].id = 0xFFFF;
        t[3] = -1;
        s += 2;
        s[0].id = 0xFFFF;
        t[4] = -1;
        s += 1;
        t += 5;
    }
}

typedef struct HAGI {
    u16 id;             /* 0x00 */
    u16 cnt;            /* 0x02 */
    f32 rad;            /* 0x04 */
    s16 x08;            /* 0x08 */
    u8 x0A;             /* 0x0A */
    u8 joint;           /* 0x0B */
} HAGI;
extern HAGI *em_hagi_type_tbl[];
void get_joint_pos_em();
void ext_pick_point_fifo_ck();
void ext_pick_point_tbl_set();
void ext_pick_point_tbl_clr_ex();
void ext_pick_point_tbl_clr();
void Ext_pick_point_clr();
void Ext_pick_point_pos();
void Em_hagi_point_clr();
f32 flSqrt(f32);
int Item_get_ck();

int Ext_pick_point_set(a, pos)
STIEM *a;
f32 *pos;
{
    int i;
    STIEM *s = StiEM_data;

    ext_pick_point_fifo_ck();
    for (i = 0; i < 20; i++, s++) {
        if (s->id == 0xFFFF) {
            if (pos == 0) {
                s->pos[0] = a->pos[0];
                s->pos[1] = a->pos[1];
                s->pos[2] = a->pos[2];
            } else {
                s->pos[0] = pos[0];
                s->pos[1] = pos[1];
                s->pos[2] = pos[2];
            }
            s->rad = a->rad;
            s->id = a->id;
            s->cnt = a->cnt;
            s->x14 = 2;
            s->stg = a->stg;
            s->x19 = a->x19;
            s->x1A = a->x1A;
            ext_pick_point_tbl_set((s8)i);
            return i;
        }
    }
    return -1;
}

u16 Ext_pick_point_cnt_ck(n)
int n;
{
    return StiEM_data[n].cnt;
}

void Ext_pick_point_clr(n)
int n;
{
    if (n != -1) {
        StiEM_data[n].id = 0xFFFF;
        ext_pick_point_tbl_clr_ex((s8)n);
    }
}

void Ext_pick_point_pos(n, p)
int n;
f32 *p;
{
    StiEM_data[n].pos[0] = p[0];
    StiEM_data[n].pos[1] = p[1];
    StiEM_data[n].pos[2] = p[2];
}

void Ext_pick_point_st(n, st)
int n;
s8 st;
{
    StiEM_data[n].stg = st;
}

u16 Ext_pick_point_ck(pl)
PLW *pl;
{
    STIEM *s;
    int i;
    f32 dx, dz;
    u16 r;

    s = StiEM_data;
    for (i = 0; i < 20; i++, s++) {
        if (s->id != 0xFFFF && s->stg == pl->stg) {
            if (!(pl->pos[1] < s->pos[1] - 200.0f) && pl->pos[1] < s->pos[1] + 100.0f) {
                dx = pl->pos[0] - s->pos[0];
                dz = pl->pos[2] - s->pos[2];
                if (flSqrt(dx * dx + dz * dz) <= s->rad) {
                    r = s->id;
                    if (pl->pos[1] + 80.0f <= s->pos[1]) {
                        r |= 0x8000;
                    }
                    return r;
                }
            }
        }
    }
    return 0xFFFF;
}

int Ext_pick_point_ck2(pl)
PLW *pl;
{
    STIEM *s;
    int i;
    f32 dx, dz;
    int r;
    u16 c;

    s = StiEM_data;
    for (i = 0; i < 20; i++, s++) {
        if (s->id != 0xFFFF && s->stg == pl->stg) {
            if (!(pl->pos[1] < s->pos[1] - 200.0f) && pl->pos[1] < s->pos[1] + 100.0f) {
                dx = pl->pos[0] - s->pos[0];
                dz = pl->pos[2] - s->pos[2];
                if (flSqrt(dx * dx + dz * dz) <= s->rad) {
                    if ((s32)s->cnt > 0) {
                        c = s->cnt;
                        r = Item_get_ck(s->id & 0x7FFF) & 0xFFFF;
                        if (c != 0xFF) {
                            s->cnt = c - 1;
                        }
                    } else {
                        r = 0xFFFE & 0xFFFF;
                    }
                    return r;
                }
            }
        }
    }
    return 0;
}

void ext_pick_point_fifo_ck(void)
{
    int n;
    int i;
    s8 *t;

    n = quest_w.x3B - 1;
    if (quest_w.x3B >= 20) {
        i = 0;
        if (n > 0) {
            t = stiem_stack_tbl;
            for (; i < n; i++, t++) {
                if (!(StiEM_data[*t].x19 & 1)) {
                    break;
                }
            }
        }
        Ext_pick_point_clr(stiem_stack_tbl[i]);
    }
}

void ext_pick_point_tbl_clr(n)
s8 n;
{
    int last;
    s8 *t;

    last = quest_w.x3B - 1;
    if (n < last) {
        t = stiem_stack_tbl + n;
        do {
            n++;
            t[0] = t[1];
            t++;
        } while (n < last);
    }
    stiem_stack_tbl[n] = -1;
    quest_w.x3B--;
}

void ext_pick_point_tbl_clr_ex(n)
int n;
{
    int i;
    s8 *t;

    i = 0;
    if (quest_w.x3B >= 1) {
        t = stiem_stack_tbl;
        do {
            if ((s8)n == *t) {
                ext_pick_point_tbl_clr((s8)i);
                return;
            }
            i++;
            t++;
        } while (i < quest_w.x3B);
    }
}

void Ext_pick_point_st_clr(void)
{
    int i;
    s8 *t;

    i = quest_w.x3B - 1;
    if (i >= 0) {
        t = stiem_stack_tbl + i;
        do {
            if (!(StiEM_data[*t].x19 & 1)) {
                Ext_pick_point_clr(*t);
            }
            i--;
            t--;
        } while (i >= 0);
    }
}

void ext_pick_point_tbl_set(n)
s8 n;
{
    stiem_stack_tbl[quest_w.x3B] = n;
    quest_w.x3B++;
}

s8 Em_hagi_point_set(em, n)
EMW *em;
int n;
{
    STIEM sp;
    f32 jp[3];
    u8 *h;
    u8 kind;

    kind = em->kind;
    h = (u8 *)em_hagi_type_tbl[kind];
    if (h == 0) {
        em->x88D = -1;
    } else {
    if (quest_w.x14E > 0) {
        h += 0xC;
    }
    if (kind == 3 && em->type == 1) {
        h += 0x18;
    }
    h += n * 0xC;
    sp.rad = *(f32 *)(h + 4);
    sp.id = *(u16 *)h;
    sp.cnt = *(u16 *)(h + 2);
    sp.stg = em->stg;
    sp.x1A = *(s16 *)(h + 8);
    sp.x19 = h[0xA];
    em->x876 = h[0xB];
    if (em->x876 != 0) {
        get_joint_pos_em(em, em->x876, jp);
        jp[1] = em->x5AC;
        em->x88D = Ext_pick_point_set(&sp, jp);
    } else {
        em->x88D = Ext_pick_point_set(&sp, em->pos);
    }
    }
    return em->x88D;
}

int Em_tail_hagi_point_set(tail)
u8 *tail;
{
    STIEM sp;
    u8 *h;

    h = (u8 *)em_hagi_type_tbl[(*(EMW **)(tail + 0x34))->kind];
    if (h == 0) {
        return -1;
    }
    h += 0x18;
    if (quest_w.x14E > 0) {
        h += 0xC;
    }
    sp.rad = *(f32 *)(h + 4);
    sp.id = *(u16 *)h;
    sp.cnt = *(u16 *)(h + 2);
    sp.stg = tail[6];
    sp.x1A = *(s16 *)(h + 8);
    sp.x19 = h[0xA];
    return Ext_pick_point_set(&sp, (f32 *)(tail + 0x24));
}

int Em_hagi_point_cnt_ck(em)
EMW *em;
{
    s8 n;
    f32 jp[3];
    int r;

    r = -1;
    n = em->x88D;
    if (n != -1) {
        if ((s32)StiEM_data[n].cnt <= 0) {
            Em_hagi_point_clr(em);
            return -1;
        }
        if (em->x876 != 0) {
            get_joint_pos_em(em->x876, jp);
            jp[1] = em->x5AC;
            Ext_pick_point_pos(em->x88D, jp);
        } else {
            Ext_pick_point_pos(n, em->pos);
        }
        r = StiEM_data[em->x88D].cnt;
        return r;
    }
    return r;
}

void Em_hagi_point_clr(em)
EMW *em;
{
    Ext_pick_point_clr(em->x88D);
    em->x88D = -1;
}
