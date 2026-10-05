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
