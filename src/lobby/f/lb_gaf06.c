/* lb_gaf06 - plaza init phase 0x005D8920-0x005D8C0C: vs_square_init_init (my_user_id is declared 16 bytes so that it is not gp-addressed; the stage store is done in each branch). Whole file in lb_af.c. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 *pNet;
extern u8 D_32D471[];
extern s32 pl_area_top;
extern u8 lit_277_00665D90[];
extern u8 D_3C6FC8[];
extern u8 my_user_id[0x10];
extern u8 lit_278_00665D98[];
extern u8 put_back[];
extern void (*vs_square_func_213[])();
extern void (*vs_square_func_227[])();
extern u8 network_work[];
extern u8 my_user_handle[0x10];
void Lb_move_common();
void DemoCameraRequest();
int DemoCameraCheck();
void adx_se_set();
void fade_set();
int Fade_busy_ck();
void Disp_NowLoading();
void str_stop_all();
void Lb_reset();
void Lb_stage_load();
void Lb_npc_set();
void lobby_bgm_set();
void lobby_bgm_set2();
void Event_flag_set();
int Event_flag_ck();
void font_set_stack_no();
void lbc_text_lobby_trans();
void Pit_reset();
void Lbc_set_prim();
int Online_ck();
void all_reset();
void Set_userdata();
void Lb_set_mini_data();
void Lb_make_quest_tbl_local();
void Q_camera_init();
void fade_reset();
void InitRenderState();
void Lb_set_player();
void Lb_load_player_all();
void Lb_send_statusReq();
void Lbc_connect();
void load_pit();
void load_eft();
void load_shadow();
void Quest_init();
void Pit_init();
void eft01_set();
void flSndPortStop();
void FlushCache();
void load_bin_req();
int load_busy_ck();
void flSndPackLoadBG();
int flSndPackLoadStatus();
void str_fadeout();
void McOperationSet();
void str_pause();
int McCardOperation();
void str_volume();
void str_fadein();
void str_fadein_vol();
void Lb_send_commer();
void Lbc_SendMiniData();
int Lbc_getDate();
void Lb_check_newCommer();

void vs_square_init_init(void) {
    int i;
    u8 *pl;
    PLW *p;
    u8 m;
    m = game_w.master;
    p = &player_work[m];
    if (Online_ck() == 0) {
        pNet[0x11] = 1;
    }
    switch (lb_sys.x04) {
    case 0:
        if (Online_ck() == 0) {
            Lb_make_quest_tbl_local();
            Q_camera_init();
        }
        cw[0x2C08] = 0;
        fade_reset();
        lb_sys.x03 = lb_sys.x03 + 1;
        InitRenderState(0);
        Disp_NowLoading();
        if (Online_ck() == 0) {
            *(s8 *)0x3F34C3 = 1;
            Lb_set_player(0, lit_277_00665D90, D_3C6FC8);
            memcpy(my_user_handle, D_3C6FC8, 0x11);
            memcpy(my_user_id, lit_278_00665D98, 8);
            memcpy((u8 *)lbCommer + game_w.master * 0x5C + 8, my_user_handle, 0x10);
            memcpy((u8 *)lbCommer + game_w.master * 0x5C, my_user_id, 8);
        }
        Lb_load_player_all();
        Lb_send_statusReq();
        Lbc_connect();
        load_pit();
        load_eft();
        Lbc_connect();
        load_shadow();
        Lbc_connect();
        Quest_init();
        Pit_init();
        lb_sys.x71 = -1;
        if (Online_ck() == 1) {
            if (cw[0x35D2] == 0) {
                p->x73A = 0x4C;
            } else {
                lb_sys.x71 = 0;
                p->x73A = 0x4D;
            }
        } else {
            p->x73A = 0x57;
        }
        Lb_stage_load(p->x73A);
        cw[0x35D2] = 0;
        Lbc_connect();
        Lb_npc_set(p->x73A);
        i = 0;
        pl = (u8 *)player_work;
        do {
            eft01_set(pl, 1);
            i = (s16)(i + 1);
            pl += 0xA00;
        } while (i < 8);
        fade_set(0xA);
        return;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            lb_sys.x04 = 0;
            lb_sys.x03 = lb_sys.x03 + 1;
            cw[0x35D2] = 0;
        }
    }
}
