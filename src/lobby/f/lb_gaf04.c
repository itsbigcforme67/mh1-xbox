/* lb_gaf04 - near-match fixes 0x005D91A0-0x005D93A0: vs_square_exit. Whole file in lb_af.c. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 *pNet;
extern u8 D_32D471[];
extern s32 pl_area_top;
extern u8 lit_277_00665D90[];
extern u8 D_3C6FC8[];
extern u8 my_user_id[8];
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

void vs_square_exit(a)
int a;
{
    PLW *pl;
    int v;
    u8 st;
    pl = &player_work[game_w.master];
    switch (lb_sys.x04) {
    case 0:
        st = game_w.stage;
        if (st != 0x4F) {
            if (st == 0x50) {
                goto b8;
            }
        } else {
b8:
            if (lb_sys.x78 != 0 && pl->x73A == 0x4C) {
                Lbc_SendMiniData();
            }
        }
        lb_sys.x04 = lb_sys.x04 + 1;
    case 1:
        fade_set(1);
        str_fadeout(0, 0x10);
        Lb_move_common();
        if (pl->x73A == 0x4E) {
            lb_sys.x04 = lb_sys.x04 + 1;
            return;
        }
        lb_sys.x04 = lb_sys.x04 + 2;
        return;
    case 2:
        v = Lbc_getDate();
        switch (v) {
        case 0:
        case 1:
            lb_sys.x04 = lb_sys.x04 + 1;
        }
        Lb_move_common();
        return;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if (Online_ck() == 0) {
                str_pause(0, 1);
            } else {
                str_stop_all();
            }
            Lb_reset();
            if (pl->x73A == 0x63) {
                lb_sys.x03 = 3;
                return;
            }
            lb_sys.x05 = 0;
            lb_sys.x03 = 3;
            return;
        }
        Lb_move_common();
    }
}
