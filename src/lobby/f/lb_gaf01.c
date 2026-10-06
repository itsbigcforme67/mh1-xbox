/* lb_gaf01 - near-match fixes 0x005D8760-0x005D8914: vs_square, vs_square_init_pre. Whole file in lb_af.c. */
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

void vs_square(void) {
    vs_square_func_227[lb_sys.x03]();
}

void vs_square_init_pre(a, b, c)
int a;
int b;
int c;
{
    if (Online_ck() == 0) {
        pNet[0x11] = 1;
    }
    switch (lb_sys.x04) {
    case 0:
        if (Online_ck() == 0) {
            all_reset();
            Set_userdata(player_work);
            Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
            Lb_set_mini_data((u8 *)lbCommer + game_w.master * 0x5C + 0x1C);
            *(s8 *)0x3F3603 = 0;
            *(s8 *)0x3F3604 = 0;
            *(s8 *)0x3F3605 = 0;
            *(s16 *)0x3F3606 = 0;
        }
        fade_set(0xA);
        lb_sys.x04 = lb_sys.x04 + 1;
        return;
    case 1:
        if ((Fade_busy_ck(lb_sys.x04) & 0xFF) != 1) {
            lb_sys.x04 = lb_sys.x04 + 1;
        }
        break;
    case 2:
        if ((Fade_busy_ck(lb_sys.x04) & 0xFF) != 1) {
            lb_sys.x04 = 0;
            lb_sys.x03 = lb_sys.x03 + 1;
        }
    }
}
