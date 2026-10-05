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
