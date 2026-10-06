/* lb_gaf07 - plaza init phase 0x005D9050-0x005D9198: vs_square_init (pl = &player_work[m] before the cw store, then the phase counter). Whole file in lb_af.c. */
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

void vs_square_init(void) {
    u8 m;
    u8 st;
    PLW *pl;
    m = game_w.master;
    pl = &player_work[m];
    cw[0x2C08] = 0;
    lb_sys.x04 = lb_sys.x04 + 1;
    Disp_NowLoading();
    Lb_stage_load(pl->x73A);
    if (Online_ck() == 0) {
        str_pause(0, 0);
        str_volume(0, 0);
        str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
    } else {
        st = game_w.stage;
        if (st == 0x4E && *(u32 *)(cw + 0xBF3C) >= 4U) {
            lobby_bgm_set2(0x11);
        } else {
            lobby_bgm_set(st);
        }
    }
    Lb_npc_set(pl->x73A);
    fade_set(2);
    lb_sys.x04 = 0;
    lb_sys.x87 = 0x14;
    lb_sys.x03 = lb_sys.x03 + 1;
    Lb_check_newCommer();
    cw[0x2C08] = 1;
}
