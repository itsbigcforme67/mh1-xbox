/* lb_fz03 0x005D22B0-0x005D25EC: lb_pl_mv088 */
/* Lobby: player actions (sleep/bed, bell, cart), receive dispatcher, hand-written from m2c drafts. */
#include "lobby_f.h"
extern u8 *pNet;
extern s16 ang_894;
void McOperationSet();
int McCardOperation();
void SetDialogData();
void SetDialogYesNo();
void str_fadeout();
void str_stop();
void str_pause();
void str_fadein_vol();
void fade_set();
int Lb_select();
void pl_sleeping();
void lb_exit_save();
void Lbs_LogOutRequest();
void Lb_Pl_basic_flagset();
void Lb_pl_chr_set();
void Lb_pl_to_normal();
int Get_sw2();
int Online_ck();
void adx_se_set();
void Eft25_set();
int Lb_get_angle();
void NPCZoomInCameraCancel();
void lb_set_pl_pos();
void lb_set_pl_status();
void lb_set_pl_stage();
void lb_check_chair();
void lb_set_chair();
void lb_recv_myChair();
void lb_chidori_off();
void Lbc_SendNetComment();
void lb_trade_start();
void lb_trade_check();
void lb_trade_result();
void lb_send_my_status();
void lb_commer_message();
void lb_pl_mv088(PLW *pl) {
    u16 pad;
    int st;
    int a;
    int k;
    pad = Get_sw2(0);
    st = pl->x05;
    switch (st) {
    case 0:
        pl->x05 = st + 1;
        pl->work8CA = 0;
        Lb_pl_chr_set(pl, 0x25B, 6, 0);
        adx_se_set(pl, 0xA);
        str_fadein_vol(0, 0xF, 0x5F);
        break;
    case 1:
        pl->work8CA++;
        ang_894 = Lb_get_angle(pl, (u8 *)pl->x3B0 + 0xAC);
        *(u16 *)&pl->ang_y += (ang_894 / 5) & 0xFFFF;
        a = pl->work8CA;
        if (a >= 0x118) {
            pl->work8D0 = 3;
        } else if (a >= 0x10E) {
            if (a == 0x10E) {
                Eft25_set(pl, 8);
            }
            pl->work8D0 = 2;
        } else if (a >= 0xB4) {
            pl->work8D0 = 1;
        } else {
            pl->work8D0 = 0;
        }
        if (!(pad & 0x20) && pl->work8CA < 0x12C) {
            break;
        }
        k = pl->work8D0;
        switch (k) {
        case 0:
        case 1:
            Lb_act_set(pl->x3B0, 0, 0x8F);
            pl->x05 = pl->x05 + 1;
            Lb_pl_chr_set(pl, 0x26B, 4, 0);
            adx_se_set(pl, 0xB);
            str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
            break;
        case 2:
            Lb_act_set(pl->x3B0, 0, 0x89);
            pl->x05 = pl->x05 + 1;
            Lb_pl_chr_set(pl, 0x26B, 4, 0);
            adx_se_set(pl, 4);
            str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
            break;
        case 3:
            pl->x05 = 3;
            Lb_act_set(pl->x3B0, 0, 0x88);
            adx_se_set(pl, 0xD);
            str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
            break;
        }
        break;
    case 2:
        if (pl->work194 <= 0) {
            Lb_pl_to_normal(pl, 0, 8, 0);
            if (pl->work8D0 != 2) {
                NPCZoomInCameraCancel();
                lb_sys.x6C = 0;
                lb_sys.x68 = 0;
            }
        }
        break;
    case 3:
        break;
    }
}
