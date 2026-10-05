/* lb_gae02 - near-match fixes 0x005D1000-0x005D14C8: lb_pl_mv052. Whole file in lb_ae.c. */
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

void lb_pl_mv052(PLW *pl, int a1, int a2) {
    int v;
    Get_sw2(0);
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    switch (lb_sys.x68) {
    case 0x23:
    case 0x24:
    case 0x25:
        pl_sleeping(pl);
        return;
    }
    {
        switch (pl->x05) {
        case 0:
            cw[0x2C08] = 0;
            pl->x05 = pl->x05 + 1;
            Lb_Pl_basic_flagset(pl, 0, 0, 0);
            pl->work39C = 0;
            Lb_pl_chr_set(pl, 0x1AB, 0, 0);
            if (Online_ck() == 0) {
                McOperationSet(5);
                SetDialogData(0x30, 2);
                SetDialogYesNo(0);
                return;
            }
            McOperationSet(6);
            pl->x05 = 2;
            pl->work08 = 0x3C;
            str_fadeout(0, pl->work08);
            return;
        case 1:
            pNet[0xC] = 1;
            v = Lb_select();
            switch (v) {
            case 0:
                pl->x05 = pl->x05 + 1;
                pl->work08 = 0x41;
                str_fadeout(0, pl->work08);
                break;
            case 3:
                SetDialogData(0x37, 2);
                SetDialogYesNo(1);
                pl->x05 = 4;
                break;
            }
            pl_sleeping(pl);
            return;
        case 2:
            v = pl->work08;
            if (v == 0) {
                switch (McCardOperation() & 0xFF) {
                case 1:
                    if (Online_ck() == 1) {
                        lb_exit_save(pl);
                    } else {
                        pl->x05 = pl->x05 + 1;
                        cw[0x2C08] = 1;
                        SetDialogData(0x37, 2);
                        SetDialogYesNo(1);
                    }
                    break;
                case 2:
                    if (Online_ck() == 1) {
                        lb_exit_save(pl);
                    } else {
                        SetDialogData(0x37, 2);
                        SetDialogYesNo(1);
                        pl->x05 = 4;
                    }
                    break;
                }
            } else {
                int w = v - 1;
                pl->work08 = w;
                if (w == 5) {
                    str_stop(1);
                    str_pause(0, 1);
                    cnWrap_SoundRequest(0xB);
                }
            }
            pl_sleeping(pl);
            return;
        case 3:
            pNet[0xC] = 1;
            v = Lb_select();
            switch (v) {
            case 0:
                if (Online_ck() == 1) {
                    Lbs_LogOutRequest(1);
                } else {
                    lb_sys.x68 = 0x23;
                    fade_set(1);
                }
                break;
            case 3:
                lb_exit_save(pl);
                return;
            }
            pl_sleeping(pl);
            return;
        case 4:
            pNet[0xC] = 1;
            v = Lb_select();
            switch (v) {
            case 0:
                if (Online_ck() == 1) {
                    Lbs_LogOutRequest();
                } else {
                    pl->x05 = pl->x05 + 1;
                    SetDialogData(0x39, 4);
                }
                break;
            case 3:
                lb_exit_save(pl);
                return;
            }
            pl_sleeping(pl);
            return;
        case 5:
            pNet[0xC] = 1;
            v = Lb_select();
            switch (v) {
            case 0:
                pl->x05 = 2;
                pl->work08 = 0x3C;
                str_fadeout(0, pl->work08);
                if (Online_ck() == 0) {
                    McOperationSet(5);
                    return;
                }
                McOperationSet(6);
                return;
            case 3:
                lb_sys.x68 = 0x23;
                fade_set(1);
                return;
            }
        }
    }
}
