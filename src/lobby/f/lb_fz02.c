/* lb_fz02 0x005C4E60-0x005C50F0: Lb_check_receipt */
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
void Lb_check_receipt(a, p)
u8 *a;
u8 *p;
{
    int id;
    u8 *q;
    int t;
    t = p[0];
    q = p + 1;
    if (cw[0x35D5] != 0) {
        id = Lb_get_plID() & 0xFF;
        if (id == 0xFF) {
            return;
        }
    } else {
        int k = t & 0xFF;
        id = (int)a;
        switch (k) {
        case 0xB:
        case 7:
        case 8:
            break;
        default:
            return;
        }
    }
    switch (t & 0xFF) {
    case 0:
        lb_set_pl_pos(id, q, t);
        return;
    case 1:
    case 10:
        lb_set_pl_status(id, q);
        return;
    case 2:
        lb_set_pl_pos(id, q, t);
        return;
    case 3:
        lb_set_pl_stage(id, q);
        return;
    case 4:
        lb_check_chair(a, q);
        return;
    case 9:
        lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << *(s8 *)q);
        return;
    case 5:
        lb_set_chair((s8)id, q);
        return;
    case 17:
        lb_recv_myChair(q);
        return;
    case 6:
        lb_chidori_off((s8)id);
        return;
    case 7:
        Lbc_SendNetComment(a);
        return;
    case 8:
        if (cw[0x35D5] != 0) {
            memcpy(cw + (id & 0xFF) * 0x62 + 0x288C, q, 0x62);
            memcpy(cw + 0x2B9C, q, 0x62);
            return;
        }
        memcpy(cw + 0x288C, q, 0x62);
        memcpy(cw + 0x2B9C, q, 0x62);
        return;
    case 12:
        lb_trade_start((s8)id, q);
        return;
    case 13:
        lb_trade_check(a, q);
        return;
    case 14:
        lb_trade_result(q);
        return;
    case 15:
        lb_send_my_status(a);
        return;
    case 16:
        lb_commer_message((s8)id, q);
    default:
        return;
    }
}
