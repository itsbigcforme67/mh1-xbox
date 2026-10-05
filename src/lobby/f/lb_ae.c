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
void lb_pl_mv088(PLW *pl, int a1, int a2) {
    s16 a;
    int pad;
    u8 st;
    u8 k;
    st = pl->x05;
    pad = Get_sw2(0) & 0xFFFF;
    switch (st) {
    case 0:
        pl->x05 = st + 1;
        *(s16 *)((u8 *)pl + 0x8CA) = 0;
        Lb_pl_chr_set(pl, 0x25B, 6, 0);
        adx_se_set(pl, 0xA);
        str_fadein_vol(0, 0xF, 0x5F);
        return;
    case 1:
        *(s16 *)((u8 *)pl + 0x8CA) = *(s16 *)((u8 *)pl + 0x8CA) + 1;
        ang_894 = Lb_get_angle(pl, (u8 *)pl->x3B0 + 0xAC);
        pl->ang_y = pl->ang_y + (((ang_894 / 5) + ((u32)ang_894 >> 0x1F)) & 0xFFFF);
        a = *(s16 *)((u8 *)pl + 0x8CA);
        if (a >= 0x118) {
            *((u8 *)pl + 0x8D0) = 3;
        } else if (a >= 0x10E) {
            if (a == 0x10E) {
                Eft25_set(pl, 8);
            }
            *((u8 *)pl + 0x8D0) = 2;
        } else if (a >= 0xB4) {
            *((u8 *)pl + 0x8D0) = 1;
        } else {
            *((u8 *)pl + 0x8D0) = 0;
        }
        if ((pad & 0xFFFF & 0x20) || *(s16 *)((u8 *)pl + 0x8CA) >= 0x12C) {
            k = *((u8 *)pl + 0x8D0);
            switch (k) {
            case 1:
            case 0:
                Lb_act_set(pl->x3B0, 0, 0x8F);
                pl->x05 = pl->x05 + 1;
                Lb_pl_chr_set(pl, 0x26B, 4, 0);
                adx_se_set(pl, 0xB);
                str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
                return;
            case 2:
                Lb_act_set(pl->x3B0, 0, 0x89);
                pl->x05 = pl->x05 + 1;
                Lb_pl_chr_set(pl, 0x26B, 4, 0);
                adx_se_set(pl, 4);
                str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
                return;
            case 3:
                pl->x05 = 3;
                Lb_act_set(pl->x3B0, 0, 0x88);
                adx_se_set(pl, 0xD);
                str_fadein_vol(0, 0xF, D_32D471[game_w.stage * 2]);
                return;
            }
        } else {
    case 3:
            return;
        }
        break;
    case 2:
        if (*(s32 *)((u8 *)pl + 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 8);
            k = *((u8 *)pl + 0x8D0);
            if (k != 2) {
                NPCZoomInCameraCancel(k);
                lb_sys.x6C = 0;
                lb_sys.x68 = 0;
            }
        }
        break;
    }
}
void Lb_check_receipt(a, p)
u8 *a;
u8 *p;
{
    int id;
    u8 t;
    u8 *q;
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
        if (k != 8 && k != 7 && k != 0xB) {
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
        lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << *(s8 *)(p + 1));
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
