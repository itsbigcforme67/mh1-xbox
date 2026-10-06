/* lb_gaf03 - near-match fixes 0x005D8C10-0x005D903C: vs_square_init_se. Whole file in lb_af.c. */
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

void vs_square_init_se(a, b, c)
int a;
int b;
int c;
{
    int top;
    int s1;
    s8 a1;
    top = pl_area_top;
    *(s8 *)0x3F36C5 = 1;
    if (Online_ck() == 0) {
        pNet[0x11] = 1;
    }
    a1 = lb_sys.x04;
    switch (a1) {
    case 0:
        lb_sys.x04 = a1 + 1;
        flSndPortStop(1, a1);
        s1 = *(s16 *)0x4761B0 | 0x10000;
        FlushCache(0);
        load_bin_req(s1, top);
        return;
    case 1:
        if (load_busy_ck() == 0) {
            lb_sys.x04 = lb_sys.x04 + 1;
            flSndPackLoadBG(top, 1);
        }
        return;
    case 2:
        if (flSndPackLoadStatus() == 0) {
            lb_sys.x04 = lb_sys.x04 + 1;
        }
        return;
    case 3:
        lb_sys.x04 = a1 + 1;
        flSndPortStop(6, a1);
        FlushCache(0);
        load_bin_req(0x10004, top);
        return;
    case 4:
        if (load_busy_ck() == 0) {
            lb_sys.x04 = lb_sys.x04 + 1;
            flSndPackLoadBG(top, 6);
        }
        return;
    case 5:
        if (flSndPackLoadStatus() == 0) {
            lb_sys.x04 = lb_sys.x04 + 1;
        }
        return;
    case 6:
        lb_sys.x04 = a1 + 1;
        flSndPortStop(7, a1);
        s1 = (*(s16 *)0x32D650 + 7) | 0x10000;
        FlushCache(0);
        load_bin_req(s1, top);
        return;
    case 7:
        if (load_busy_ck() == 0) {
            lb_sys.x04 = lb_sys.x04 + 1;
            flSndPackLoadBG(top, 7);
        }
        return;
    case 8:
        if (flSndPackLoadStatus() == 0) {
            if (Online_ck() == 1 || *(u8 *)0x3F36CF == 0) {
                if (Event_flag_ck(2) == 1 || Online_ck() == 1) {
                    lb_sys.x04 = 0xB;
                } else {
                    cw[0x2C08] = 1;
                    fade_set(2);
                    lb_sys.x04 = 0;
                    lb_sys.x03 = 6;
                }
            } else {
                lb_sys.x04 = lb_sys.x04 + 1;
                str_fadeout(0, 0xF);
                *(s32 *)(cw + 0x2C4C) = 0xF;
                McOperationSet(9);
                Lbc_set_prim(put_back, 0, 0);
            }
            *(u8 *)0x3F36C5 = 0;
        }
        return;
    case 9: {
        s32 *c2 = (s32 *)(cw + 0x2C4C);
        int t = *c2;
        if (t != 0) {
            *c2 = t - 1;
            return;
        }
        str_pause(0, 1);
        lb_sys.x04 = lb_sys.x04 + 1;
        fade_set(2);
        pNet[0x11] = 0;
        return;
    }
    case 10:
        pNet[0x11] = 0;
        if (McCardOperation() & 0xFF) {
            lb_sys.x04 = lb_sys.x04 + 1;
            fade_set(0xA);
            str_pause(0, 0);
            str_volume(0, 0);
            str_fadein(0, 0xF);
        }
        return;
    case 11:
        pNet[0x11] = 0;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cw[0x2C08] = 1;
            fade_set(2);
            lobby_bgm_set(game_w.stage);
            lb_sys.x04 = 0;
            lb_sys.x03 = lb_sys.x03 + 2;
            Lb_send_commer();
        }
    default:
        return;
    }
}
