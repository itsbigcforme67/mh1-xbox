/* Quest control (SLPM_654.95 main, f_quest range 0x226C30-...): start-up of a
 * quest (Quest_start), the retire/error state, remaining monsters and the
 * reward. Meanings are guesses. */
#include "quest.h"
#include "plf.h"

typedef char *va_list;
void str_gattai(char *dst, char *fmt, ...);
QEM *em_work_serch2(s16, s16);
u16 stolen_item_stack(int, s16);
void Quest_start();
void Quest_retire_set();
void Quest_error_set2();
void Quest_error_set();
void Quest_restart();
void Quest_em_init_set();
void Quest_enemy_die();
void Quest_enemy_escape();
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
EMW * Em_direct_set();
void Quest_next_em_clr();
char * Quest_str_get();
void Item_stolen();
void Item_regained();
void Share_item_conv();
u16 Ext_pick_point_cnt_ck();
void Ext_pick_point_st();
u16 Ext_pick_point_ck();
void Ext_pick_point_st_clr();
s8 Em_hagi_point_set();
void Quest_forfeit_message();
QEM * em_work_serch();
void Quest_timer_reset();
void remuneration_item_set();
u16 * quest_supplies_get();
s16 quest_enemy_ck2();
void quest_presuccess_ptr_set();
void Quest_net_sub();
void quest_em_die();
void quest_timer_send();

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
QEM *em_work_serch();
void q_net_send_em_die();
void q_net_send_em_capture();
int quest_enemy_ck_sub();
int em_capture_conv();
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
s32 *Em_data_st_adrs_get(s32 *, int, int, s8);
void func_5A8170();
void push_em_work();
void release_enemy_model();
void enemy_insurance_sub();
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))

void em_next_tbl_ck();
void func_5589F0();
void em_create_model();
void em_herb_set();
EMW *Em_direct_set();

extern s8 item_regained_tbl[8];
char *func_5C5E20();
void adx_se_set();
s16 Share_item_num_ck();
int share_item_ck_ck();
s16 Quest_share_item_num_ck();
int Share_item_stack();

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

int strlen();
int vsprintf();
void set01_set2_use_mem();

extern char lit_1730_0036B290[];

void em_data_st_adrs_set();

void quest_em_init_sub();
void quest_em_init_sub2();

int Quest_f_dra_ck();

extern u16 capture_type_tbl[];
int Pl_item_num_ck();
void em_herb_set();
int quest_enemy_ck_sub2();
void func_535D20();

extern s32 quest_timer_disp_tbl[][2];
extern char lit_2393[];
extern char lit_2394[];
extern char lit_2395[];
extern char lit_2396[];
void set01_set2();
void QuestClearCameraRequest();
int func_53B5C0();

int Net_Share_item_stack();

extern u16 rem_item_calc_tbl[];
extern s8 *rem_item_exit_sel_tbl[];

/* quest_w.x88 entry (8 bytes): kind, unused, offset of its item table in mission_area. */
typedef struct QREM {
    u16 id;             /* 0x00 condition kind, 0xFFFF ends the list */
    u16 x02;            /* 0x02 */
    s32 tbl;            /* 0x04 offset of the REMI table */
} QREM;

/* Reward item table entry (6 bytes), table ends with weight 0xFFFF. */
typedef struct REMI {
    u16 w;              /* 0x00 weight */
    u16 id;             /* 0x02 item */
    u16 num;            /* 0x04 count */
} REMI;

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

void Quest_next_em_set(n)
int n;
{
    QEM *l1;
    s32 *l0;
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
        for (;;) {
            v = *l0;
            if (v == -1) {
                break;
            }
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
        for (;;) {
            if (l1->id < 0) {
                break;
            }
            if (!(l1->x2E & 1)) {
                Em_direct_set(l1);
                l1->x07 = game_w.stage;
            }
            l1++;
        }
    }
    if (n == 5 || n == 0x10 || n == 0x29) {
        em_herb_set();
    }
}

char *Quest_str_get(int n)
{
    if (game_w.x1DC) {
        return func_5C5E20();
    }
    return (char *)(mission_area + quest_w.x84[n]);
}

static s16 stolen_item_num_ck(item)
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

u16 stolen_item_stack(int item, s16 num)
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
                mx = *((s8 *)Item_data + 3 + (item & 0xFFFF) * 16);
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

int Ext_pick_point_set(a, pos)
STIEM *a;
f32 *pos;
{
    STIEM *s = StiEM_data;
    int i;

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
                        r = Item_get_ck(s->id & 0x7FFF) & 0xFFFF;
                        if (s->cnt != 0xFF) {
                            s->cnt = s->cnt - 1;
                        }
                    } else {
                        r = 0xFFFE;
                        r = r & 0xFFFF;
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
            get_joint_pos_em(em, em->x876, jp);
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

void Quest_forfeit_message(void)
{
    char buf[0x20];
    int n;

    str_gattai(buf, lit_1730_0036B290, quest_w.x18);
    n = strlen(buf);
    if (n & 1) {
        buf[n] = 0x20;
        buf[n + 1] = 0;
    }
    buf[0x1F] = 0;
    set01_set2_use_mem(buf);
}

void str_gattai(char *dst, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsprintf(dst, fmt, ap);
}

void enemy_insurance_sub(n)
int n;
{
    QEM *e;

    if (Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A) != 0) {
        e = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if (e->x2E & 4) {
                e->x2E = 2;
                e->x08 = -1;
            }
            e++;
        }
    }
}

void em_next_tbl_ck(n)
int n;
{
    s32 *l0;
    QEM *e;
    int any;

    l0 = Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A);
    if (l0 != 0) {
        e = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
        any = 0;
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if (!(e->x2E & 5)) {
                any = 1;
            }
            e++;
        }
        if (any == 0 && e->id == -2) {
            em_data_st_adrs_set(quest_w.x74, n, e + 1, 1);
            em_data_st_adrs_set(quest_w.x74, n, (int)l0 + 0x10, 0);
        }
    }
}

void em_data_st_adrs_set(tbl, n, val, k)
s32 *tbl;
int n;
int val;
int k;
{
    s32 *p;
    s32 base;
    s32 v;
    s32 w;

    tbl += quest_w.x3A;
    base = *tbl;
    if (base != 0 && quest_w.no != 0) {
        p = (s32 *)(base + (int)mission_area);
loop:
        v = *p;
        if (v != 0) {
            if (v != n) {
                p += 4;
                goto loop;
            }
            p += k + 2;
            w = *p;
            if (w == 0 || w == -1) {
                return;
            }
            *p = val - (int)mission_area;
        }
    }
}

void quest_em_init(void)
{
    int n;

    n = 0;
    quest_em_init_sub(quest_w.x78, &n);
    quest_em_init_sub2(quest_w.x74, &n);
}

void quest_em_init_sub(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;

    for (;;) {
        l0 = Em_data_com_adrs_get(p, 0);
        e = (QEM *)Em_data_com_adrs_get(p, 1);
        if (l0 == 0) {
            break;
        }
        if (e != 0) {
            for (;;) {
                if (e->id == -1) {
                    break;
                }
                e->x2E = 2;
                e->x08 = -1;
                e->x0A = -1;
                e->x2C = *cnt;
                if (e->x05 != 0) {
                    quest_w.x34 += e->x04;
                }
                e++;
                (*cnt)++;
            }
        }
        p += 4;
    }
}

void quest_em_init_sub2(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;
    s32 *q;

    for (;;) {
        q = (s32 *)*p;
        if (q == 0) {
            break;
        }
        if (quest_w.no == 0) {
        } else {
            q = (s32 *)((int)q + (int)mission_area);
        }
        for (;;) {
            l0 = Em_data_com_adrs_get(q, 0);
            e = (QEM *)Em_data_com_adrs_get(q, 1);
            q += 4;
            if (l0 == 0) {
                break;
            }
            if (e != 0) {
                for (;;) {
                if (e->id == -1) {
                    break;
                }
                    e->x2E = 2;
                    e->x08 = -1;
                    e->x0A = -1;
                    e->x2C = *cnt;
                    if (e->x05 != 0) {
                        quest_w.x34 += e->x04;
                    }
                    e++;
                    (*cnt)++;
                }
            }
        }
        p++;
    }
}

void station_em_set(void)
{
    s16 *l0;
    int i;
    QEM *e;
    EMW *em;
    u8 *g;
    s32 *l;
    s32 v;

    l = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l != 0 && e != 0) {
        for (;;) {
            v = *l;
            if (v == -1) {
                break;
            }
            for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
                if (g[0x28] == v) {
                    break;
                }
            }
            if (i < 4) {
                l++;
                continue;
            }
            for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
                if (g[0x28] <= 0) {
                    ((u8 *)&game_w)[0x28 + i] = v;
                    em_create_model(i);
                    break;
                }
            }
            l++;
        }
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if ((em = Em_direct_set(e)) != 0) {
                if (em->kind == 2 && Quest_f_dra_ck(*(u8 *)&quest_w.no) != 0) {
                    quest_w.x14C = em->x302;
                }
                em->stg = e->x07;
            }
            e++;
        }
    }
}

QEM *em_work_serch(em)
EMW *em;
{
    int i;
    s32 *l0;
    QEM *e;

    i = 0;
    if (quest_w.x3A < 0) {
    } else {
        do {
            l0 = Em_data_st_adrs_get(quest_w.x74, em->stg, 0, (s8)i);
            e = (QEM *)Em_data_st_adrs_get(quest_w.x74, em->stg, 1, (s8)i);
            if (l0 != 0) {
                for (;;) {
                    if (e->id < 0) {
                        break;
                    }
                    if (em->id == e->x0A) {
                        return e;
                    }
                    e++;
                }
            }
            i++;
        } while (quest_w.x3A >= i);
    }
    l0 = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l0 == 0) {
        return 0;
    }
    if (e == 0) {
        return 0;
    }
    for (;;) {
        if (e->id < 0) {
            break;
        }
        if (em->id == e->x0A) {
            return e;
        }
        e++;
    }
    return 0;
}

QEM *em_work_serch2(s16 no, s16 stg)
{
    int i;
    s32 *l0;
    QEM *e;
    s8 k;

    i = 0;
    k = quest_w.x3A;
    if (k >= 0) {
        do {
            l0 = Em_data_st_adrs_get(quest_w.x74, stg, 0, k);
            e = (QEM *)Em_data_st_adrs_get(quest_w.x74, stg, 1, quest_w.x3A);
            if (l0 != 0) {
                for (;;) {
                    if (e->id < 0) {
                        break;
                    }
                    if (no == e->x2C) {
                        return e;
                    }
                    e++;
                }
            }
            i++;
        } while (quest_w.x3A >= i);
    }
    l0 = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l0 == 0) {
        return 0;
    }
    if (e == 0) {
        return 0;
    }
    for (;;) {
        if (e->id < 0) {
            break;
        }
        if (no == e->x2C) {
            return e;
        }
        e++;
    }
    return 0;
}

void Quest_timer_calc(n)
int n;
{
    if (game_w.info_stop == 0 && quest_w.x10 > 0) {
        quest_w.x10 -= n;
        if (quest_w.x10 < 0) {
            quest_w.x10 = 0;
        }
    }
}

void Quest_timer_reset(void)
{
    quest_w.x10 = Quest_time_get(1);
}

s16 quest_condition_prog(pl, i)
PLW *pl;
s16 i;
{
    QCMD *p;
    QCMD *q;
    char buf[0x20];
    int n;

    if (game_w.x0D5 == 2 && quest_w.x10 >= 0) {
        if (quest_w.xAF > 0) {
            s32 t;
            t = quest_timer_disp_tbl[quest_w.xAF][0];
            if (!(t < quest_w.x10)) {
                str_gattai(buf, lit_2393, t / 1800);
                n = strlen(buf);
                if (n & 1) {
                    buf[n] = 0x20;
                    buf[n + 1] = 0;
                }
                buf[0x1F] = 0;
                set01_set2_use_mem(buf);
                quest_w.xAF--;
            }
        }
        Quest_timer_calc(1);
        if (quest_w.x10 <= 0) {
            s32 t;
            quest_w.x10 = 0;
            q = (QCMD *)quest_w.x6C;
            i = 0;
            quest_timer_send();
            quest_w.x10 = -1;
            t = quest_w.x0A;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
        } else if (quest_w.x10 % 9000 == 0) {
            quest_timer_send();
        }
    }
    for (;;) {
again:
        if (quest_w.x0B != 0 && quest_share_item_ck(&quest_w)) {
            s32 t;
            i = 0;
            q = (QCMD *)quest_w.x6C;
            t = quest_w.x0B;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
            quest_w.x0B = 0;
        }
        if (quest_w.x14F != 0 && quest_w.x150 != 0) {
            s32 t;
            t = (s16)quest_w.x14F;
            q = (QCMD *)quest_w.x6C;
            i = 0;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
            quest_w.x14F = 0;
        }
        q = (QCMD *)quest_w.x6C;
        p = q + i;
        switch (p->cmd) {
        case 5:
            quest_w.x0C = p->a;
            i++;
            break;
        case 6: {
            s32 t;
            t = quest_w.x0C - 1;
            quest_w.x0C = t;
            if (t > 0) {
                break;
            }
            i++;
            break;
        }
        case 7:
            set01_set2(quest_w.x70[p->a] + (int)mission_area);
            i++;
            break;
        case 0:
            if (quest_w.x34 > 0) {
                break;
            }
            i++;
            goto again;
        case 1:
            if (quest_enemy_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x24: {
            s16 r;
            r = quest_enemy_ck2(&quest_w, *((u8 *)p + 2));
            if (p->b < r) {
                break;
            }
            i++;
            goto again;
        }
        case 2:
            quest_enemy_set(p->a, p->b);
            i++;
            break;
        case 0x1D:
            if (quest_item_ck2(&quest_w, p) == 0) {
                break;
            }
            i++;
            goto again;
        case 3:
            if (quest_item_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 8:
            if (quest_item_ck(&quest_w) == 0) {
                i = quest_w.x38;
                break;
            }
            if (pl->stg != p->a) {
                break;
            }
            i++;
            goto again;
        case 4:
            quest_item_set(p->a, p->b);
            i++;
            break;
        case 0xA: {
            s32 t;
            t = p->a;
            p = (QCMD *)((u8 *)p + 4);
            switch (t) {
            case -1:
                quest_supplies_get(pl, p);
                break;
            case -3:
                if (pl->kind == 1 || pl->kind == 5) {
                    quest_supplies_get(pl, p);
                }
                break;
            default:
                if (pl->kind == t) {
                    quest_supplies_get(pl, p);
                }
                break;
            }
            i++;
            break;
        }
        case 0xB:
            quest_w.x38 = i + 1;
            i++;
            break;
        case 0xD:
            quest_w.x10 = (u16)p->a;
            i++;
            break;
        case 0xE:
            quest_w.x10 += (u16)p->a;
            i++;
            break;
        case 0xF:
            i++;
            p++;
            quest_w.x04++;
            for (;;) {
                if (p->a == game_w.master || p->a == -4 || p->cmd == 0x12) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case 0x10:
            quest_w.x04++;
            i++;
            p++;
            for (;;) {
                if (p->cmd == 0x12) {
                    break;
                }
                if (p->cmd == 0x11) {
                    s32 t;
                    t = p->a;
                    if (t == -3) {
                        if (pl->kind == 1 || pl->kind == 5) {
                            break;
                        }
                    } else if (t == -2) {
                        if (pl->kind == 0 || (u8)(pl->kind - 2) < 2 || pl->kind == 4) {
                            break;
                        }
                    } else if (t == -4 || pl->kind == t) {
                        break;
                    }
                }
                i++;
                p++;
            }
            i++;
            goto again;
        case 0x11:
        case 0x12:
            quest_w.x04--;
            for (;;) {
                if (p->cmd == 0x12) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case -1:
            if (Quest_f_dra_ck(*(u8 *)&quest_w.no) != 0 && quest_w.xB0 != 0 && ((EMW *)quest_w.xB0)->kind == 2) {
                quest_w.x14C = ((EMW *)quest_w.xB0)->x302;
            }
            QuestClearCameraRequest();
            quest_w.x182 = 0;
            i++;
            quest_w.x06 = 3;
            game_w.x0D5 = 3;
            quest_w.x184 = 0;
            quest_w.x186 = 0;
            quest_w.x181 = 2;
            quest_w.x140 |= 1 << (game_w.master + 8);
            net_send_sys(6, game_w.master);
            goto again;
        case 0x2D:
            if ((quest_w.x140 & 0xF00) != 0xF00 && game_w.pl_state[game_w.master] == 1) {
                break;
            }
            i++;
            break;
        case 0x1E:
            quest_w.x01 = 0;
            i = -1;
            quest_w.x00++;
            break;
        case 0x17:
            i = quest_w.x38;
            quest_w.x36 = i;
            goto again;
        case 0x18:
            if ((s16)act_ck(pl, 4, 0) == 0) {
                break;
            }
            i++;
            goto again;
        case -2:
            quest_w.x182 = 0;
            i++;
            quest_w.x184 = 0;
            game_w.x0D5 = 5;
            quest_w.x06 = 5;
            quest_w.x186 = 0;
            quest_w.x181 = 4;
            net_send_sys(6, game_w.master);
            goto again;
        case 0x1F:
            quest_w.x06 = 6;
            game_w.x0D5 = 6;
            i = -1;
            set01_set2(lit_2394);
            set01_set(0, 0xF, 0);
            break;
        case 0x1B:
            quest_w.x0A = p->a;
            n = Quest_time_get(1);
            str_gattai(buf, lit_2395, n / 1800);
            n = strlen(buf);
            if (n & 1) {
                buf[n] = 0x20;
                buf[n + 1] = 0;
            }
            buf[0x1F] = 0;
            set01_set2_use_mem(buf);
            i++;
            break;
        case 0x20:
            quest_w.x3A = p->a;
            quest_w.x182 = 0;
            quest_w.x181 = 6;
            quest_w.x186 = 0;
            quest_w.x184 = quest_w.x3A;
            net_send_sys(6, game_w.master);
            i++;
            goto again;
        case 0x21:
            if (quest_item_ck3(&quest_w, p) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x22:
            if (quest_share_item_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x23:
            quest_w.x0B = p->a;
            i++;
            break;
        case 0x2C:
            quest_w.x14F = p->a;
            i++;
            break;
        case 0x25:
            game_w.x0D5 = 5;
            quest_w.x06 = 5;
            set01_set2(lit_2396);
            i++;
            quest_w.x182 = 0;
            quest_w.x181 = 0xA;
            quest_w.x184 = 0;
            quest_w.x186 = 0;
            net_send_sys(6, game_w.master);
            goto again;
        case 0x26:
            quest_w.x182 = 0;
            i++;
            quest_w.x184 = 0;
            quest_w.x06 = 3;
            game_w.x0D5 = 3;
            quest_w.x186 = 0;
            quest_w.x181 = 0xB;
            quest_w.x140 |= 1 << (game_w.master + 8);
            net_send_sys(6, game_w.master);
            goto again;
        case 0x28: {
            s32 t;
            t = ((EMW *)quest_w.xB0)->x302;
            if (quest_w.x14C - p->a < t) {
                i++;
                p++;
                for (;;) {
                    if (p->cmd == 0x29) {
                        i++;
                        goto again;
                    }
                    i++;
                    p++;
                }
            }
            quest_w.x14C = t;
            i++;
            goto again;
        }
        case 0x2A:
            if ((u8)func_53B5C0() == 1) {
                i++;
                goto again;
            }
            i++;
            p++;
            for (;;) {
                if (p->cmd == 0x2B) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case 0x1C: {
            s32 t;
            t = p->a;
            i = 0;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    goto again;
                }
                i++;
                q++;
            }
        }
        case 0x1A:
            i++;
            goto again;
        default:
            break;
        }
        return i;
    }
}

void remuneration_item_set(void)
{
    PLW *pl;
    int n;
    QREM *e;
    REMI *tbl;
    u8 *out;
    int i;
    int th;
    int j;
    u8 *a;
    u8 *b;
    u8 *g;
    REMI *q;
    u16 *q16;
    s8 *sel;
    int r;
    int acc;
    u16 *tb;
    int ck;
    u16 cnt;

    game_w.x1A8 = 0;
    game_w.x1AC = 0;
    pl = &player_work[game_w.master];
    g = (u8 *)&game_w;
    for (i = 0; i < 0x20; i += 8) {
        *(s16 *)(g + 0x128) = 0;
        *(s16 *)(g + 0x12A) = 0;
        *(s16 *)(g + 0x12C) = 0;
        *(s16 *)(g + 0x12E) = 0;
        *(s16 *)(g + 0x130) = 0;
        *(s16 *)(g + 0x132) = 0;
        *(s16 *)(g + 0x134) = 0;
        *(s16 *)(g + 0x136) = 0;
        *(s16 *)(g + 0x138) = 0;
        *(s16 *)(g + 0x13A) = 0;
        *(s16 *)(g + 0x13C) = 0;
        *(s16 *)(g + 0x13E) = 0;
        *(s16 *)(g + 0x140) = 0;
        *(s16 *)(g + 0x142) = 0;
        *(s16 *)(g + 0x144) = 0;
        *(s16 *)(g + 0x146) = 0;
        g += 0x20;
    }
    e = (QREM *)quest_w.x88;
    n = 0;
    out = (u8 *)&game_w;
    for (; e->id != 0xFFFF; e++) {
        switch (e->id) {
        case 0x8000:
            break;
        case 1:
            if (!(quest_w.x13C != 0)) {
                continue;
            }
            break;
        case 2:
            if (!(quest_w.x13C & 1)) {
                continue;
            }
            break;
        case 3:
            if (!(quest_w.x13C & 2)) {
                continue;
            }
            break;
        case 4:
            if (!(quest_w.x13C & 4)) {
                continue;
            }
            break;
        case 5:
            if (!(quest_w.x13C & 8)) {
                continue;
            }
            break;
        case 6:
            if (!(quest_w.x13C & 0x10)) {
                continue;
            }
            break;
        case 7:
            if (!(quest_w.x13C & 0x20)) {
                continue;
            }
            break;
        case 8:
            if (!(quest_w.x13C & 0x40)) {
                continue;
            }
            break;
        case 9:
            if (!(quest_w.x13C & 0x80)) {
                continue;
            }
            break;
        case 0xA:
            if (!(quest_w.x13C & 0x100)) {
                continue;
            }
            break;
        case 0x10:
            if (!(quest_w.x13C & 0x4000)) {
                continue;
            }
            break;
        case 0x11:
            if (!(quest_w.x13C & 0x8000)) {
                continue;
            }
            break;
        case 0x12:
            if (!(quest_w.x13C & 0x10000)) {
                continue;
            }
            break;
        case 0x13:
            if (!(quest_w.x13C & 0x20000)) {
                continue;
            }
            break;
        case 0x14:
            if (!(quest_w.x13C & 0x40000)) {
                continue;
            }
            break;
        case 0xB:
            if (game_w.x218 > 0x6400) {
                if (quest_w.x14C > 0x6400) {
                    continue;
                }
                break;
            }
            continue;
        case 0xC:
            if (game_w.x218 > 0x4B00) {
                if (quest_w.x14C > 0x4B00) {
                    continue;
                }
                break;
            }
            continue;
        case 0xD:
            if (game_w.x218 > 0x3200) {
                if (quest_w.x14C > 0x3200) {
                    continue;
                }
                break;
            }
            continue;
        case 0xE:
            if (game_w.x218 > 0x1900) {
                if (quest_w.x14C > 0x1900) {
                    continue;
                }
                break;
            }
            continue;
        case 0xF:
            if (game_w.x218 > 0x1900) {
                if (quest_w.x14C > 0x1900) {
                    continue;
                }
                break;
            }
            continue;
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
            tb = &rem_item_calc_tbl[e->id - 0x15];
            ck = share_item_ck_ck();
            cnt = Share_item_num_ck((u16)quest_w.x1C[0], ck);
            if (cnt < tb[0]) {
                continue;
            }
            if (!(cnt < tb[1])) {
                continue;
            }
            break;
        default:
            break;
        }
        tbl = (REMI *)(e->tbl + (int)mission_area);
        for (i = 0;; i++) {
            if (e->id == 0x8000) {
                if (Pl_Skill_ck(pl, 0x34) == 1) {
                    sel = rem_item_exit_sel_tbl[2];
                } else if (Pl_Skill_ck(pl, 0x35) == 1) {
                    sel = rem_item_exit_sel_tbl[3];
                } else {
                    sel = rem_item_exit_sel_tbl[0];
                }
            } else {
                sel = rem_item_exit_sel_tbl[1];
            }
            th = sel[i];
            if (!(((u16)ran_suu(0) & 0x1F) < th)) {
                break;
            }
            th = 0;
            if (tbl->w != 0xFFFF) {
                q = tbl;
                do {
                    th = (th + q->w) & 0xFFFF;
                    q = (REMI *)((u8 *)q + 6);
                } while (q->w != 0xFFFF);
            }
            r = (u16)ran_suu(0) % (u16)th;
            if (e->id == 0x8000 && i == 0) {
                r = 0;
            }
            acc = 0;
            q16 = (u16 *)tbl;
            if (*q16 != 0xFFFF) {
                do {
                    acc = (acc + *q16) & 0xFFFF;
                    q16 += 1;
                    if (r < acc) {
                        break;
                    }
                    q16 += 2;
                } while (*q16 != 0xFFFF);
            }
            n++;
            *(u16 *)(out + 0x128) = q16[0];
            *(u16 *)(out + 0x12A) = q16[1];
            if (n >= 0x20) {
                break;
            }
            out += 4;
        }
    }
    a = (u8 *)&game_w;
    for (i = 0; i < 0x20; i++) {
        b = (u8 *)&game_w + (i + 1) * 4;
        for (j = i + 1; j < 0x20; j++) {
            u16 bi = *(u16 *)(b + 0x128);
            u16 ai = *(u16 *)(a + 0x128);
            if (bi < ai && bi != 0) {
                s16 an = *(s16 *)(a + 0x12A);
                *(u16 *)(a + 0x128) = bi;
                *(s16 *)(a + 0x12A) = *(s16 *)(b + 0x12A);
                *(u16 *)(b + 0x128) = ai;
                *(s16 *)(b + 0x12A) = an;
            }
            b += 4;
        }
        a += 4;
    }
}

u16 *quest_supplies_get(pl, p)
PLW *pl;
u16 *p;
{
    u16 a = p[0];
    s16 b = p[1];

    p += 2;
    Pl_item_stack(pl, a, b);
    return p;
}

int quest_item_set(id, num)
s16 id;
s16 num;
{
    int i;
    u8 *q;
    int t;

    for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
        if (*(s16 *)(q + 0x1C) == 0) {
            t = i * 2;
            *(s16 *)((u8 *)quest_w.x1C + t) = id;
            *(s16 *)((u8 *)quest_w.x24 + t) = num;
            return 0;
        }
    }
    return -1;
}

int quest_enemy_set(id, num)
s16 id;
s16 num;
{
    int i;
    u8 *q;
    int t;

    for (i = 0, q = (u8 *)&quest_w; i < 2; i++, q += 2) {
        if (*(s16 *)(q + 0x2C) == 0) {
            t = i * 2;
            *(s16 *)((u8 *)quest_w.x2C + t) = id;
            *(s16 *)((u8 *)quest_w.x144 + t) = id;
            *(s16 *)((u8 *)quest_w.x30 + t) = num;
            *(s16 *)((u8 *)quest_w.x148 + t) = num;
            return 0;
        }
    }
    return -1;
}

int quest_item_ck(q)
QUEST_W *q;
{
    return (s16)Pl_item_num_ck(&player_work[game_w.master], (u16)q->x1C[0]) >= q->x24[0];
}

int quest_item_ck2(q, em)
QUEST_W *q;
QEM *em;
{
    PLW *pl;
    int i;
    s16 n;
    s16 id;
    u8 *p;
    s16 *a;
    s16 *b;

    pl = &player_work[game_w.master];
    if (pl->stg == em->x02) {
        for (i = 0, p = (u8 *)q; i < 4; i++, p += 2) {
            id = *(s16 *)(p + 0x1C);
            if (id != 0 && (n = Pl_item_num_ck(pl, id & 0xFFFF)) != 0) {
                a = &q->x24[i];
                if (*a < n) {
                    n = *a;
                }
                *a -= n;
                b = &q->x1C[i];
                Pl_item_stack(pl, (u16)*b, -n);
                id = *(s16 *)&Item_data[*b][8];
                if (id != 0) {
                    Share_item_stack(pl, id & 0xFFFF, n);
                }
                set01_set(1, 8, *b);
                quest_w.x182 = 0;
                quest_w.x181 = 1;
                quest_w.x184 = *b;
                quest_w.x186 = n;
                net_send_sys(6, game_w.master);
                if (*a <= 0) {
                    *b = 0;
                }
                break;
            }
        }
    }
    for (i = 0, p = (u8 *)q; i < 4; i++, p += 2) {
        if (*(s16 *)(p + 0x1C) != 0) {
            return 0;
        }
    }
    return 1;
}

int quest_item_ck3(unused, p)
int unused;
s16 *p;
{
    u16 id;
    s16 num;

    id = p[1];
    p += 2;
    num = *p;
    return (s16)Pl_item_num_ck(&player_work[game_w.master], id, id) >= num;
}

int quest_share_item_ck(q)
QUEST_W *q;
{
    int i;

    for (i = 0; i < 4; i++) {
        if (q->x1C[i] != 0 && quest_w.x24[i] - (s16)Share_item_num_ck((u16)q->x1C[i], share_item_ck_ck()) > 0) {
            return 0;
        }
    }
    return 1;
}

int quest_enemy_ck(q)
QUEST_W *q;
{
    int i;

    for (i = 0; i < 2; i++) {
        if (q->x2C[i] != 0) {
            return 0;
        }
    }
    return 1;
}

s16 quest_enemy_ck2(q, kind)
QUEST_W *q;
int kind;
{
    int i;

    for (i = 0; i < 2; i++) {
        if (q->x2C[i] == (kind & 0xFF)) {
            return q->x30[i];
        }
    }
    return 0;
}

int quest_enemy_ck_sub(q, kind)
QUEST_W *q;
int kind;
{
    int i;
    s16 t;
    s16 v;

    for (i = 0; i < 2; i++) {
        v = q->x2C[i];
        if (v == 0x63) {
            t = q->x30[i] - 1;
            q->x30[i] = t;
            if (t <= 0) {
                q->x2C[i] = 0;
                return 1;
            }
        } else {
            if (v == (kind & 0xFF)) {
                t = q->x30[i] - 1;
                q->x30[i] = t;
                if (t <= 0) {
                    q->x2C[i] = 0;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int quest_enemy_ck_sub2(q, kind)
QUEST_W *q;
int kind;
{
    int i;
    s16 v;

    for (i = 0; i < 2; i++) {
        v = q->x2C[i];
        if (v != 0x63) {
            if (v == (kind & 0xFF) && q->x30[i] < 2) {
                return 1;
            }
        } else if (q->x30[i] < 2) {
            return 1;
        }
    }
    return 0;
}

int em_capture_conv(kind)
int kind;
{
    u16 *p;

    p = capture_type_tbl;
    while (*p != 0xFF) {
        if ((kind & 0xFF) == (s8)*p) {
            Share_item_stack(&player_work[game_w.master], p[1], 1);
            return 1;
        }
        p += 2;
    }
    return 0;
}

void quest_failed_ptr_set(flag)
int flag;
{
    s16 *p;
    s16 i;
    s16 k;

    k = flag == 0 ? -2 : 0x25;
    p = (s16 *)quest_w.x6C;
    i = 0;
    for (;;) {
        if (*p == k) {
            break;
        }
        p += 4;
        i++;
    }
    quest_w.x36 = i;
}

void quest_presuccess_ptr_set(flag)
int flag;
{
    s16 *p;
    s16 i;
    s16 k;

    k = flag == 0 ? -1 : 0x26;
    p = (s16 *)quest_w.x6C;
    i = 0;
    for (;;) {
        if (*p == k) {
            break;
        }
        p += 4;
        i++;
    }
    quest_w.x36 = i;
}

void em_herb_set(void)
{
    QEM q;

    q.id = 0xA;
    q.x02 = 0;
    q.x08 = -1;
    q.pos[0] = 0;
    q.pos[1] = 0;
    q.pos[2] = 0;
    q.x1C = 0;
    Em_direct_set(&q);
}

void Quest_net_sub(void)
{
    int i;
    s16 id;
    s32 t;

    switch ((u8)quest_w.x181) {
    case 1:
        id = quest_w.x184;
        for (i = 0; i < 4; i++) {
            if (quest_w.x1C[i] == id) {
                quest_w.x24[i] = quest_w.x24[i] - quest_w.x186;
                break;
            }
        }
        break;
    case 2:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 8);
        switch (game_w.x0D5) {
        case 0:
        case 1:
        case 2:
            quest_presuccess_ptr_set(0);
            break;
        }
        break;
    case 4:
        switch (game_w.x0D5) {
        case 5:
        case 6:
            break;
        default:
            quest_failed_ptr_set(0);
            break;
        }
        break;
    case 5:
        t = ((s32)quest_w.x184 << 16 & 0xFFFF0000) | (u16)quest_w.x186;
        if (t < quest_w.x10) {
            quest_w.x10 = t;
        }
        break;
    case 6:
        id = quest_w.x184;
        if (quest_w.x3A < id) {
            quest_w.x3A = id;
        }
        break;
    case 7:
        quest_em_die();
        break;
    case 9:
        Net_Share_item_stack(&player_work[(u8)quest_w.x180], (u16)quest_w.x184, quest_w.x186);
        break;
    case 10:
        switch (game_w.x0D5) {
        case 5:
        case 6:
            break;
        default:
            quest_failed_ptr_set(1);
            break;
        }
        break;
    case 11:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 8);
        switch (game_w.x0D5) {
        case 0:
        case 1:
        case 2:
            quest_presuccess_ptr_set(1);
            break;
        }
        break;
    case 12:
        quest_w.x140 |= 1 << (u8)quest_w.x180;
        quest_w.xB4[(u8)quest_w.x180].a = quest_w.x184;
        break;
    case 13:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 4);
        quest_w.x13C |= quest_w.x184 << 16;
        quest_w.x13C |= quest_w.x186;
        break;
    }
}

void quest_em_die(void)
{
    QEM *e;
    EMW *em;
    int hp;
    int i;
    int fl;
    int no;

    em = em_work;
    hp = (u16)quest_w.x186 >> 8 & 0xFF;
    if ((e = em_work_serch2(quest_w.x184, quest_w.x186 & 0xFF)) != 0 && ((fl = e->x2E) & 6) != 0) {
        no = e->x0A;
        i = 0;
        for (;;) {
            if (em->id == no && *(u8 *)((u8 *)em + 0x9EB) == *(u8 *)&e->x2C) {
                if (em->kind != e->id) {
                    return;
                }
                if (em->be_flag == 0) {
                    return;
                }
                if (!(fl & 4)) {
                    if (em->x04 >= 2) {
                        return;
                    }
                    if (em->mode == 5) {
                        return;
                    }
                } else if (!(e->x04 > (u16)hp)) {
                    return;
                }
                {
                    if (em->x04 >= 2 || em->mode == 5) {
                        Em_hagi_point_clr(em, fl, no, i);
                    }
                    func_535D20(em, 5, 0);
                    *(s32 *)((u8 *)em + 0x798) = 0x3F800000;
                    e->x2E = 2;
                    em->x302 = 0;
                    if (quest_enemy_ck_sub2(&quest_w, em->kind) != 0) {
                        quest_w.x3C = em;
                    }
                }
                return;
            }
            i++;
            em++;
            if (i >= 20) {
                break;
            }
        }
        if (e->x04 > (u16)hp) {
            quest_enemy_ck_sub(&quest_w, (u8)e->id, no, i);
            if (e->x05 != 0) {
                quest_w.x34--;
            }
            e->x04--;
            if (e->x04 > 0) {
                e->x2E = 2;
                return;
            }
            e->x2E = 1;
            e->x0A = -1;
        }
    }
}

void quest_timer_send(void)
{
    if (game_w.info_stop == 0) {
        quest_w.x182 = 0;
        quest_w.x181 = 5;
        quest_w.x186 = quest_w.x10;
        quest_w.x184 = (u32)quest_w.x10 >> 16;
        net_send_sys(6, game_w.master);
    }
}

void q_net_send_em_die(e, em)
QEM *e;
EMW *em;
{
    quest_w.x182 = 0;
    quest_w.x181 = 7;
    quest_w.x184 = e->x2C;
    quest_w.x186 = em->stg | (u16)(e->x04 << 8);
    net_send_sys(6, game_w.master);
}

void q_net_send_em_capture(e, em)
QEM *e;
EMW *em;
{
    quest_w.x182 = 0;
    quest_w.x181 = 8;
    quest_w.x184 = e->x2C;
    quest_w.x186 = em->stg;
    net_send_sys(6, game_w.master);
}
