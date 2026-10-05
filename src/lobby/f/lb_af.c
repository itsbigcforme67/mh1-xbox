/* Lobby: plaza ("vs_square") main loop phases: init, event, exit, hand-written from m2c drafts. */
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
void vs_square_event(a, b)
int a;
int b;
{
    PLW *pl;
    int v;
    u8 m;
    int a1;
    int a0;
    m = game_w.master;
    pl = &player_work[m];
    Lb_move_common(m, b);
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
            a1 = lb_sys.x04 + 1;
            lb_sys.x04 = a1;
            pl->x73A = 0x56;
            lb_sys.x71 = -1;
            fade_set(0xA, a1);
            return;
        }
        return;
    case 2:
        pl->x01 = 0;
        a0 = Fade_busy_ck() & 0xFF;
        if (a0 == 1) {
            return;
        }
        Disp_NowLoading(a0);
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
int Local_main(void) {
    pNet[0xC] = 0;
    pNet[0x11] = 0;
    font_set_stack_no(0);
    vs_square_func_213[lb_sys.x03]();
    if (pNet[0x11] == 0) {
        lbc_text_lobby_trans(network_work);
    }
    switch (lb_sys.x68) {
    case 37:
        Pit_reset();
        Lbc_set_prim(0, 0, 0);
        return -1;
    case 32:
        *(s8 *)0x3F35CC = 0;
        Pit_reset();
        Lbc_set_prim(0, 0, 0);
        return 1;
    default:
        return 0;
    }
}
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
void vs_square_init_init(void) {
    int i;
    u8 *pl;
    u16 st;
    u8 *p;
    u8 m;
    m = game_w.master;
    p = (u8 *)player_work + m * 0xA00;
    if (Online_ck() == 0) {
        pNet[0x11] = 1;
    }
    switch (lb_sys.x04) {
    case 0:
        if (Online_ck() == 0) {
            Lb_make_quest_tbl_local();
            Q_camera_init();
        }
        cw[0x2C08] = 0;
        fade_reset();
        lb_sys.x03 = lb_sys.x03 + 1;
        InitRenderState(0);
        Disp_NowLoading();
        if (Online_ck() == 0) {
            *(s8 *)0x3F34C3 = 1;
            Lb_set_player(0, lit_277_00665D90, D_3C6FC8);
            memcpy(my_user_handle, D_3C6FC8, 0x11);
            memcpy(my_user_id, lit_278_00665D98, 8);
            memcpy((u8 *)lbCommer + game_w.master * 0x5C + 8, my_user_handle, 0x10);
            memcpy((u8 *)lbCommer + game_w.master * 0x5C, my_user_id, 8);
        }
        Lb_load_player_all();
        Lb_send_statusReq();
        Lbc_connect();
        load_pit();
        load_eft();
        Lbc_connect();
        load_shadow();
        Lbc_connect();
        Quest_init();
        Pit_init();
        lb_sys.x71 = -1;
        if (Online_ck() == 1) {
            st = 0x4C;
            if (cw[0x35D2] == 0) {
            } else {
                lb_sys.x71 = 0;
                st = 0x4D;
            }
        } else {
            st = 0x57;
        }
        *(u16 *)(p + 0x73A) = st;
        Lb_stage_load(*(u16 *)(p + 0x73A));
        cw[0x35D2] = 0;
        Lbc_connect();
        Lb_npc_set(*(u16 *)(p + 0x73A));
        i = 0;
        pl = (u8 *)player_work;
        do {
            eft01_set(pl, 1);
            i = (s16)(i + 1);
            pl += 0xA00;
        } while (i < 8);
        fade_set(0xA);
        return;
    case 1:
        if ((Fade_busy_ck(lb_sys.x04) & 0xFF) != 1) {
            lb_sys.x04 = 0;
            lb_sys.x03 = lb_sys.x03 + 1;
            cw[0x35D2] = 0;
        }
        return;
    }
}
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
        u8 *c2 = cw;
        int t = *(s32 *)(c2 + 0x2C4C);
        if (t != 0) {
            *(s32 *)(c2 + 0x2C4C) = t - 1;
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
            Lb_send_commer(a);
        }
    default:
        return;
    }
}
void vs_square_init(a)
int a;
{
    int a0;
    u8 m;
    u8 st;
    m = game_w.master;
    cw[0x2C08] = 0;
    a0 = m * 0xA00;
    lb_sys.x04 = lb_sys.x04 + 1;
    Disp_NowLoading(a0, m);
    Lb_stage_load();
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
    Lb_npc_set(*(u16 *)((u8 *)player_work + a0 + 0x73A));
    fade_set(2);
    lb_sys.x04 = 0;
    lb_sys.x87 = 0x14;
    lb_sys.x03 = lb_sys.x03 + 1;
    Lb_check_newCommer(a);
    cw[0x2C08] = 1;
}
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
                Lbc_SendMiniData(lb_sys.x04, player_work);
            }
        }
        lb_sys.x04 = lb_sys.x04 + 1;
    case 1:
        fade_set(1);
        str_fadeout(0, 0x10);
        Lb_move_common(a);
        if (pl->x73A == 0x4E) {
            lb_sys.x04 = lb_sys.x04 + 1;
            return;
        }
        lb_sys.x04 = lb_sys.x04 + 2;
        return;
    case 2:
        v = Lbc_getDate();
        if (v != 1 && v != 0) {
        } else {
            lb_sys.x04 = lb_sys.x04 + 1;
        }
        Lb_move_common(a);
        return;
    case 3:
        if ((Fade_busy_ck(lb_sys.x04, player_work) & 0xFF) != 1) {
            if (Online_ck() == 0) {
                str_pause(0, 1);
            } else {
                str_stop_all(0);
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
        Lb_move_common(a);
        return;
    }
}
