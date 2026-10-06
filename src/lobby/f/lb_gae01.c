/* lb_gae01 - near-match fixes 0x005D1820-0x005D1AC4: lb_pl_mv076. Whole file in lb_ae.c. */
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

void lb_pl_mv076(PLW *pl, int a1) {
    u16 t;
    u8 st;
    st = pl->x05;
    switch (st) {
    case 0:
        pl->x05 = st + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        if (a1 == 1) {
            pl->x05 = 2;
            pl->work08 = 0xE10;
            pl->work08 = 0x3C;
            if (*(u16 *)((u8 *)pl + 0x2DC) != 0x261) {
                Lb_pl_chr_set(pl, 0x261, 6, 0);
            }
            break;
        }
        Lb_pl_chr_set(pl, 0x260, -4, 0);
        return;
    case 1:
        if (*(s32 *)((u8 *)pl + 0x194) == 0) {
            if (pl->id == game_w.master && game_w.stage != 0x4D && lb_sys.x66 != 0x10) {
                Lb_eat_to_bell();
                return;
            }
            pl->work08 = 0xE10;
            pl->work08 = 0x3C;
            pl->x05 = pl->x05 + 1;
            Lb_pl_chr_set(pl, 0x261, 4, 0);
            return;
        }
        break;
    case 2:
        if ((pl->sw.trg & 0x200) && lb_sys.x68 == 0) {
            Lb_Pl_act_set(pl, 0, 0x4D, 0);
            return;
        }
        if (game_w.stage == 0x4D && Pl_master_ck(pl) == 1) {
            t = pl->sw.trg;
            if (t & 0x40) {
                Lb_act_set(pl, 0, 0x5A);
                return;
            }
            if (t & 0x20) {
                Lb_act_set(pl, 0, 0x59);
                return;
            }
            if (t & 0x2000) {
                Lb_act_set(pl, 0, 0x60);
                return;
            }
            if (t & 0x1000) {
                Lb_act_set(pl, 0, 0x5E);
                return;
            }
            if (t & 0x800) {
                Lb_act_set(pl, 0, 0x5C);
                return;
            }
            if (t & 0x400) {
                Lb_act_set(pl, 0, 0x54);
                return;
            }
        }
        break;
    }
}
