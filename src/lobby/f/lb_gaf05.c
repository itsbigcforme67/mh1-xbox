/* lb_gaf05 - plaza event phase 0x005D8470-0x005D8678: vs_square_event (case 1: `break` after the inner switch, not `return`; Lb_move_common takes one argument, fade_set and Disp_NowLoading none). Whole file in lb_af.c. */
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

void vs_square_event(void) {
    PLW *pl;
    int v;
    u8 m;
    int a0;
    m = game_w.master;
    pl = &player_work[m];
    Lb_move_common(m);
    switch (lb_sys.x04) {
    case 0:
        DemoCameraRequest(0xB, 0);
        adx_se_set(pl, 0x41);
        lb_sys.x6C = 1;
        lb_sys.x68 = 0x21;
        lb_sys.x04 = lb_sys.x04 + 1;
        return;
    case 1:
        pl->x01 = 0;
        v = DemoCameraCheck();
        switch (v) {
        case 0:
        case -1:
            lb_sys.x04 = lb_sys.x04 + 1;
            pl->x73A = 0x56;
            lb_sys.x71 = -1;
            fade_set(0xA);
            return;
        }
        break;
    case 2:
        pl->x01 = 0;
        a0 = Fade_busy_ck() & 0xFF;
        if (a0 == 1) {
            return;
        }
        Disp_NowLoading();
        str_stop_all();
        Lb_reset();
        Lb_stage_load(pl->x73A);
        lb_sys.x6C = 1;
        lb_sys.x68 = 0x21;
        Lb_npc_set(pl->x73A);
        fade_set(2);
        lb_sys.x04 = lb_sys.x04 + 1;
        lobby_bgm_set(game_w.stage);
        DemoCameraRequest(0xC, 0);
        return;
    case 3:
        pl->x01 = 1;
        v = DemoCameraCheck();
        switch (v) {
        case 0:
        case -1:
            lb_sys.x04 = 0;
            lb_sys.x6C = 0;
            lb_sys.x03 = 4;
            Event_flag_set(2);
        }
    }
}
