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
