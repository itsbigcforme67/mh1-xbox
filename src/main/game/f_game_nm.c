/* Game mode state machine: whole file as near-match C (not built).
 * Game_task is 690/788 instructions off by diff (mostly a shifted nop and
 * register choice for tsk->step, see docs/agents/agent-E.md); game3 is 10
 * instructions off in the sprite-struct init order.  The rest of the file
 * matches and lives in f_game.c / f_gameb.c. */
#include "f_game.h"

void game0(void) {
    s16 i;

    game_w.step = 0;
    game_w.sub = 0;
    game_w.mode = 1;
    game_w.x12 = 1;
    game_w.x03 = 0;
    for (i = 0; i < 4; i++) {
        game_w.x1E8[i][0] = 0;
        game_w.x1E8[i][1] = 0;
        game_w.x1E8[i][2] = 0;
        game_w.x1E8[i][3] = 0;
        game_w.x1E8[i][4] = 0;
        game_w.x1E8[i][5] = 0;
        game_w.x1E8[i][6] = 0;
        game_w.x1E8[i][7] = 0;
    }
    Copy_user_id(game_w.master);
    Disp_NowLoading2(5);
    Load_overlay(2, 1);
    Tsk_Execute(Plsel_task, 6);
}

void game10(void) {
    Disp_NowLoading2(4);
    if (game_w.x12 == 0) {
        all_reset();
        game_w.step++;
        Quest_init();
        net_send_sys(8, game_w.master);
    }
}

void game11(void) {
    s16 i;

    system_w.x35 = 1;
    GWS8(0x1DD) = option_w.x04;
    game_w.x0F = option_w.x07;
    for (i = 0; i < game_w.pl_num; i++) {
        game_w.x30[i] = select_w.x0C[i];
        game_w.x70[i] = select_w.x54[i];
        game_w.x40[i] = select_w.x5C[i];
        game_w.x78[i] = select_w.x94[i];
    }
    flInitPhaseStarted();
    Info_Initialization();
    Quest_start();
    Start_item_init();
    Disp_load_start(GW16(0x2C));
    EvDemoInitialize();
    stage_init();
    stage_load();
    smell_init();
    smoke_init();
    senko_init();
    ear_init();
    em_yobi_init();
    GW8(0x80) = 0;
    GW8(0x81) = 0;
    GW8(0x82) = 0;
    GW8(0x83) = 0;
    GW8(0x84) = 0;
    GW8(0x85) = 0;
    GW8(0x86) = 0;
    GW8(0x87) = 0;
    round_init(1);
    flInitPhaseFinished();
    GW8(0xD8) = 0;
    GW8(0xD9) = 0;
    GW8(0xDA) = 0;
    GW8(0xDB) = 0;
    GW32(0x124) = 0;
    GW8(0xD6) = 0;
    system_w.x35 = 0;
    game_w.step++;
    net_send_sys(8, game_w.master);
}

void game12(void) {
    u8 *top = pl_area_top;
    int n, idx;

    system_w.x35 = 1;
    Disp_NowLoading2(1);
    switch (game_w.sub) {
    case 0:
        game_w.sub++;
        game_w.x03 = 0;
        flSndPortStop(1);
        FlushCache(0);
        load_bin_req(0x10001, top);
        return;
    case 1:
        if (load_busy_ck() == 0) {
            game_w.sub++;
            flSndPackLoadBG(top, 1);
        }
        return;
    case 2:
        if (flSndPackLoadStatus() == 0) {
            game_w.sub++;
        }
        return;
    case 3:
        flSndPortStop(7);
        if (game_w.stage == 0xC) {
            idx = Snd_steft_tbl[8] + 7 | 0x10000;
        } else {
            idx = Snd_steft_tbl[game_w.x2E] + 7 | 0x10000;
        }
        FlushCache(0);
        load_bin_req(idx, top);
        game_w.sub++;
        return;
    case 4:
        if (load_busy_ck() == 0) {
            game_w.sub++;
            flSndPackLoadBG(top, 7);
        }
        return;
    case 5:
        if (flSndPackLoadStatus() == 0) {
            game_w.sub++;
        }
        return;
    case 6:
    case 9:
    case 12:
    case 15:
        game_w.sub++;
        flSndPortStop((s16)((game_w.sub - 6) / 3) + 2);
        FlushCache(0);
        snd_joint_load_init(2);
        return;
    case 7:
    case 10:
    case 13:
    case 16:
        n = (s16)((game_w.sub - 7) / 3);
        if (snd_joint_load_pl(top, &player_work[n]) != 0) {
            game_w.sub++;
            flSndPackLoadBG2(top, top + 0x20000, n + 2);
        }
        return;
    case 8:
    case 11:
    case 14:
    case 17:
        if (flSndPackLoadStatus() == 0) {
            if (game_w.sub == 0x11) {
                flSndPortStop(6);
                snd_joint_load_init(4);
            }
            game_w.sub++;
        }
        return;
    case 18:
        if (snd_joint_load(top, (u8 *)&game_w + 0x28) != 0) {
            game_w.sub++;
        }
        return;
    case 19:
        if (load_busy_ck() == 0) {
            game_w.sub++;
            flSndPackLoadBG2(top, top + 0x4000, 6);
        }
        return;
    case 20:
        if (flSndPackLoadStatus() == 0) {
            system_w.x35 = 0;
            game_w.step++;
            game_w.sub++;
        }
        return;
    default:
        system_w.x35 = 0;
        break;
    }
}

void game13(void) {
    Disp_NowLoading2(1);
    if (Online_ck() == 1 && net_start_ck() == 0) {
        return;
    }
    move();
    stage_work[1] = 1;
    game_w.pad_on = 1;
    player_work[0].x01 = player_work[0].be_flag;
    player_work[1].x01 = player_work[1].be_flag;
    player_work[2].x01 = player_work[2].be_flag;
    player_work[3].x01 = player_work[3].be_flag;
    player_work[4].x01 = player_work[4].be_flag;
    player_work[5].x01 = player_work[5].be_flag;
    player_work[6].x01 = player_work[6].be_flag;
    player_work[7].x01 = player_work[7].be_flag;
    swset();
    ss_time = 0;
    GW8(0x1B3) = 0;
    ss_flag = 0;
    GW8(0x1B2) = 0;
    GW8(0x21D) = 0;
    GW8(0x21E) = 0;
    Quest_timer_reset();
    game_w.x10 = 0;
    game_w.x11 = 0;
    stage_fog_set(game_w.stage);
    stage_set_set(game_w.stage);
    stage_bgm_set(game_w.stage);
    game_w.step = 0;
    game_w.mode = 2;
    flCompact();
    view_reset();
    CameraMove();
    fade_set(2);
}

void game1(void) {
    swset();
    switch (game_w.step) {
    case 0:
        game10();
        break;
    case 1:
        game11();
        break;
    case 2:
        game12();
        break;
    case 3:
        game13();
        break;
    }
}

#define PL8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define PL16(p, o) (*(u16 *)((u8 *)(p) + (o)))

/* In-quest main loop (mode 2/3?): step 0 runs the quest, 2-6 reload the
 * stage for the next area. Guesses on meaning. */
void game2(void) {
    PLW *pl = &player_work[game_w.master];
    int idx;

    if (Online_ck() == 1) {
        net_receive_pl_pos_set();
    }
    switch (game_w.step) {
    case 0:
    case 1:
        EvDemoMove();
        if (game_core() != 0) {
            return;
        }
        Info_control();
        Quest_condition_judging();
        if (Game_clear_ck(2) == 1) {
            game_w.step = 0;
            game_w.mode = 3;
            PL8(pl, 0x763) = 0;
            PL8(pl, 0x764) = 0;
            pl->dm_flag = 0;
            pl->dm_vital = 0;
            PL8(pl, 0x40A) = 0;
            PL8(pl, 0x409) = 0;
            PL8(pl, 0x917) = 0;
            PL8(pl, 0x8C4) = 0;
            PL8(pl, 0x908) = 0;
            if (GW8(0xD5) == 8) {
                game_w.mode = 5;
            }
        } else if (pl->x738 != 0) {
            game_w.step++;
            if (game_w.step == 2) {
                se_stop_all();
                Zero_rev_set();
                str_pause(0, 1);
                str_stop(1);
            }
        } else {
            game_w.step = 0;
        }
        break;
    case 2:
        swset();
        system_w.x35 = 1;
        vib_stop_all();
        flFlip(0);
        flFlip(0);
        flFlip(0);
        flFlip(0);
        flFlip(0);
        flFlip(0);
        flFlip(0);
        stage_free();
        Quest_next_em_clr(pl->stg, pl->x73A);
        Ext_pick_point_st_clr();
        flCompact();
        game_w.stage = pl->x73A;
        pl->stg = pl->x73A;
        DispWholeMap();
        flFlip(0);
        DispWholeMap();
        flFlip(0);
        st_model_load(game_w.stage);
        clr_move_work();
        light_move();
        ot_init();
        prim_init();
        system_w.x32 = 0;
        system_w.x33 = 0;
        init_light_work();
        round_init(0xFF);
        stage_w_init();
        stage_work[1] = 1;
        stage_fog_set(game_w.stage);
        stage_set_set(game_w.stage);
        em_effect_pull();
        Quest_next_em_set(game_w.stage);
        flSndPortStop(6);
        snd_joint_load_init(4);
        system_w.x35 = 0;
        game_w.step++;
        return;
    case 3:
        swset();
        {
            u8 *top = pl_area_top;
            system_w.x35 = 1;
            if (snd_joint_load(top, (u8 *)&game_w + 0x28) != 0) {
                flSndPackLoadBG2(top, top + 0x4000, 6);
                game_w.step++;
            }
        }
        system_w.x35 = 0;
        return;
    case 4:
        swset();
        if (flSndPackLoadStatus() != 0) {
            return;
        }
        {
            game_w.step++;
            flSndPortStop(7);
            if (game_w.stage == 0xC) {
                idx = Snd_steft_tbl[8] + 7 | 0x10000;
            } else {
                idx = Snd_steft_tbl[game_w.x2E] + 7 | 0x10000;
            }
            FlushCache(0);
            load_bin_req(idx, pl_area_top);
        }
        return;
    case 5:
        swset();
        if (load_busy_ck() != 0) {
            return;
        }
        game_w.step++;
        flSndPackLoadBG(pl_area_top, 7);
        break;
    case 6:
        swset();
        if (flSndPackLoadStatus() == 0) {
            game_w.step = 0;
            stage_bgm_set(game_w.stage);
            pl_init(1);
            view_reset();
            CameraMove();
            fade_set(2);
        }
    }
}

/* Sprite request passed to SpritePut: a full-screen fade quad (guess) */
typedef struct SPRQ {
    s32 x0, y0, z0, w0;
    s32 w;              /* 0x10 640.0f */
    s32 h;              /* 0x14 448.0f */
    s32 one0;           /* 0x18 1.0f */
    s32 one1;
    u8 _pad20[0x10];
    f32 alpha;          /* 0x30 */
    s32 z;
    s32 kind;           /* 0x38 */
    u32 col;            /* 0x3C */
    u8 _pad40[0x10];
} SPRQ;

/* Mode 4 (game3): fade-in then wait screen shown before the quest, ends by
 * marking this player ready.  Guess. */
void game3(void) {
    SPRQ sp;
    GAME_W *gw = &game_w;
    s16 t;

    swset();
    switch (game_w.step) {
    case 0:
        gw->step++;
        gw->x04 = 0;
        break;
    case 1:
        t = gw->x04 + 1;
        gw->x04 = t;
        if (t >= 0xC0) {
            gw->step++;
            gw->x06 = 0;
        }
        break;
    case 2:
        t = gw->x06 + 1;
        gw->x06 = t;
        if (t >= 0x96) {
            gw->step++;
        }
        break;
    case 3:
        gw->mode = 5;
        gw->step = 0;
        select_w.ready[game_w.master] = 1;
        if (Online_ck() != 0) {
            net_send_sys(2, game_w.master);
        }
        break;
    }
    if (gw->step >= 2) {
        flfntSetSize(0x20, 0x20);
        flfntLocate(0x80, 0xD0);
        switch (GW8(0xD5)) {
        case 4:
            font_set_palette(5);
            font_print(lit_778_00358160);
            break;
        case 6:
            font_set_palette(2);
            font_print(lit_779_00358180);
            break;
        default:
        case 7:
            font_set_palette(2);
            font_print(lit_780_003581A0);
            break;
        }
    }
    sp.w = 0x44200000;
    sp.one1 = 0x3F800000;
    sp.h = 0x43E00000;
    sp.x0 = 0;
    sp.y0 = 0;
    sp.w0 = 0;
    sp.alpha = gw->x04 / 255.0f;
    sp.z0 = 0;
    sp.kind = 5;
    sp.z = 0;
    sp.one0 = 0x3F800000;
    sp.col = 0xFF010101;
    SpritePut(&sp);
    Info_control();
    move();
    trans();
}

/* Mode 5 (game4): wait until every player is ready (select_w ready flags),
 * then count down and return to mode 0.  Shows a "waiting" text. Guess. */
void game4(void) {
    GAME_W *gw = &game_w;
    SELECT_W *sel = &select_w;
    s16 i, n, all, t;
    int u;

    switch (gw->step) {
    case 0:
        gw->step++;
        gw->sub = 0;
        all_reset();
        net_send_sys(2, game_w.master);
        break;
    case 1:
        all = 1;
        n = 0;
        for (i = 0; i < game_w.pl_num; i++) {
            if (sel->ready[i] == 0) {
                all = 0;
            } else {
                n++;
            }
        }
        if (all != 0) {
            gw->step++;
            gw->x04 = 0x3C;
        } else {
            if (System_timer & 0x10) {
                flfntLocate(0x6E, 0xD6);
                font_set_palette(0);
                flfntSetSize(0x14, 0x14);
                font_print(lit_824_003581C0);
                flfntLocate(0xD2, 0xF4);
                font_print(lit_825_003581F0, n, game_w.pl_num);
            }
            u = gw->sub + 1;
            gw->sub = u;
            if ((u8)u == 0x3C) {
                gw->sub = 0;
                net_send_sys(2, game_w.master);
            }
        }
        break;
    case 2:
        t = gw->x04 - 1;
        gw->x04 = t;
        if (t <= 0) {
            gw->mode = 0;
        } else {
            flfntLocate(0xD2, 0xD6);
            font_set_palette(0);
            flfntSetSize(0x14, 0x14);
            flfntLocate(0xD2, 0xD6);
            font_print(lit_826_003581F8, lit_827_00358200);
        }
        break;
    }
    font_draw();
}

/* Mode 6 (game5): result screen with a 60-frame network sync tick. */
void game5(void) {
    s16 t;

    result_prog();
    t = game_w.x0A + 1;
    game_w.x0A = t;
    if (t >= 0x3C) {
        game_w.x0A = 0;
        net_send_sys(8, game_w.master);
    }
    trans();
    font_draw();
}

int game_core(void) {
    swset();
    move();
    trans();
    hit_check();
    return 0;
}

/* Game task: top-level online/offline flow around the game modes
 * (step 0-2 session setup, 3 wait for players, 4 run the mode in
 * game_w.mode).  Names of the called network functions are the original
 * symbols; what each does is a guess. */
void Game_task(TSK *tsk) {
    s16 i, t;
    int u;

    (*(u16 *)&game_w.x1E)++;
    if (*(u16 *)&game_w.x1E % 30 == 0) {
        User_data.x374++;
        if (User_data.x374 > 0x22550FF) {
            User_data.x374 = 0x22550FF;
        }
    }
    switch (tsk->step) {
    case 0:
        fade_set();
        tsk->step++;
        game_init();
        system_w.x3F = 0;
        game_w.x0D2 = 1;
        game_w.x21B = 0;
        if (Online_ck() == 1) {
            Load_overlay(3, 1);
            game_w.step = 0;
            func_5D58C0();
            game_w.x1DC = 1;
        }
    case 1:
        if (Online_ck() == 1) {
            AQ_init(game_w.x0D2);
            game_w.step = 0;
            network_work_init();
        } else {
            Load_overlay(3, 1);
            func_5D58C0();
            game_w.x1DC = 1;
        }
        all_reset();
        tsk->step++;
        return;
    case 2:
        if (softdip_ck(0x13) != 0 && softdip_ck(0xC2) == 0) {
            func_5C4D40();
            return;
        }
        if (Online_ck() == 1) {
            switch (game_w.step) {
            case 0:
                switch (ms_network_sub()) {
                case 1:
                    cw = D_6DD7E0;
                    game_w.step++;
                    cw[0x2C31] = 0;
                    cw[0x2C32] = 0;
                    cw[0x2C33] = 0;
                    cw[0x2C34] = 0;
                    system_w.x40 = 0;
                    break;
                case -1:
                    game_w.step = 5;
                    break;
                }
                return;
            case 1:
                switch (server_connect()) {
                case 1:
                    game_w.x1DC = 1;
                    game_w.step++;
                    func_5C0680();
                    return;
                case -1:
                    COM_R_No_1 = 0;
                    game_w.step = 7;
                    COM_R_No_3 = 5;
                    break;
                case -2:
                    game_w.step = 4;
                    break;
                }
                trans();
                return;
            case 2:
                if (func_5B70D0() == 0) {
                    u = game_w.step;
                    *(u8 *)0x6EF8F5 = 3;
                    game_w.step = u + 1;
                    fade_reset();
                }
                trans();
                return;
            case 3:
                Disp_NowLoading2(4);
                switch (func_5B69E0()) {
                case 1:
                    tsk->step++;
                    InetSys.x0 = 4;
                    game_w.step = 0;
                    InetSys.x1 = 0;
                    InetSys.x2 = 0;
                    InetSys.x4 = 0;
                    game_w.x1DC = 0;
                    system_w.x40 = 0;
                    break;
                case -1:
                    COM_R_No_0 = 0;
                    COM_R_No_1 = 0;
                    game_w.step = 8;
                    COM_R_No_2 = 0;
                    COM_R_No_3 = 0;
                    break;
                }
                trans();
                return;
            case 4:
                switch (connect_error()) {
                case 1:
                    COM_R_No_1 = 0;
                    game_w.step = 7;
                    COM_R_No_3 = 5;
                    break;
                case 2:
                    game_w.step = 1;
                    break;
                case 3:
                    COM_R_No_1 = 0;
                    COM_R_No_3 = 0;
                    game_w.step = 7;
                    break;
                }
                trans();
                return;
            case 5:
                game_w.step = 0;
                AQ_session_exit();
                all_reset();
                game_w.x1DC = 0;
                Load_overlay(1, 1);
                Tsk_Exit(tsk);
                Select_Tsk_Execute();
                return;
            case 6:
                switch (session_connect()) {
                case 1:
                    game_w.step = 1;
                    COM_R_No_0 = 0;
                    COM_R_No_1 = 0;
                    COM_R_No_2 = 0;
                    COM_R_No_3 = 0;
                    COM_R_No_4 = 0;
                    break;
                case -1:
                    game_w.step = 4;
                    COM_R_No_1 = 0;
                    COM_R_No_2 = 0;
                    COM_R_No_0 = 1;
                    COM_R_No_3 = 0;
                    COM_R_No_4 = 0;
                    break;
                }
                trans();
                return;
            case 7:
                switch (return_mc_save()) {
                case 1:
                    COM_R_No_0 = 0;
                    COM_R_No_1 = 0;
                    COM_R_No_2 = 0;
                    COM_R_No_4 = 0;
                    game_w.step = COM_R_No_3;
                    COM_R_No_3 = 0;
                    break;
                }
                trans();
                return;
            case 8:
                switch (COM_R_No_4) {
                case 0:
                    if (mem_tex.x534 != 0) {
                        if (net_common_w.x2C != 0) {
                            COM_R_No_5 = 0;
                            COM_R_No_4 = 2;
                            Vs_Cnt_0 = 0x14;
                            Ncm_spr_kill_all_ex_BG();
                        }
                    } else {
                        COM_R_No_4 = 1;
                        all_reset();
                        COM_R_No_5 = 0;
                        Net_all_reset(0);
                    }
                    break;
                case 1:
                    switch (COM_R_No_5) {
                    case 0:
                        if (Ncm_mmbb_spr_load() != 0) {
                            COM_R_No_5++;
                        }
                        break;
                    case 1:
                        if (Ncm_mmbb_spr_create() != 0) {
                            COM_R_No_5 = 0;
                            Vs_Cnt_0 = 0x14;
                            COM_R_No_4++;
                            Ncm_spr_BG_set();
                        }
                        break;
                    }
                    break;
                case 2:
                    t = Vs_Cnt_0 - 1;
                    Vs_Cnt_0 = t;
                    if (t <= 0) {
                        COM_R_No_4++;
                        Vs_Cnt_0 = 0x258;
                        Ncm_spr_set_diarog_b();
                    }
                    break;
                case 3:
                    Ncm_err_mssage_disp_req(0xE);
                    t = Vs_Cnt_0 - 1;
                    Vs_Cnt_0 = t;
                    if (t <= 0) {
                        COM_R_No_4++;
                    } else if (Vs_Cnt_0 <= 0x1E0) {
                        if (net_shot_ok_ck(2) != 0) {
                            COM_R_No_4++;
                        }
                    }
                    break;
                case 4:
                    COM_R_No_4++;
                    Ncm_spr_kill_all();
                    break;
                case 5:
                    game_w.step = 1;
                    COM_R_No_0 = 1;
                    *(u8 *)0x6EF8F5 = 3;
                    COM_R_No_1 = 0;
                    COM_R_No_2 = 0;
                    COM_R_No_3 = 0;
                    COM_R_No_4 = 0;
                    COM_R_No_5 = 0;
                    break;
                }
                Net_trans_set(0);
                trans();
                return;
            }
            return;
        }
        switch (func_5D8680()) {
        case 0:
            trans();
            return;
        case 1:
            tsk->step++;
            InetSys.x2 = 0x14;
            system_w.x3F = 1;
            system_w.x40 = 0;
            return;
        case -1:
            game_w.step = 0;
            all_reset();
            game_w.x1DC = 0;
            Load_overlay(1, 1);
            Tsk_Exit(tsk);
            Select_Tsk_Execute();
            system_w.x40 = 0;
            return;
        }
        return;
    case 3:
        if (Online_ck() == 1) {
            AQ_exec();
            fade_reset();
            Disp_NowLoading2(4);
            system_w.x40++;
            if (system_w.x40 >= 0xE10) {
                tsk->step = 4;
                game_w.mode = 5;
                game_w.step = 0;
                Quest_error_set2();
                return;
            }
            if (AQSession_wait() == 0) {
                return;
            }
            game_w.pl_num = AQ_join_num_get();
            game_w.x21B = 0;
            for (i = 0; i < 4; i++) {
                if (i < game_w.pl_num) {
                    game_w.pl_ent[i] = 1;
                } else {
                    game_w.pl_ent[i] = 0;
                }
            }
        } else {
            game_w.pl_num = 1;
            for (i = 0; i < 4; i++) {
                if (i < game_w.pl_num) {
                    game_w.pl_ent[i] = 1;
                } else {
                    game_w.pl_ent[i] = 0;
                }
            }
        }
        tsk->step++;
        AQ_recv_flag_set(1);
    case 4:
        if (Online_ck() == 1) {
            AQ_exec();
        }
        switch (game_w.mode) {
        case 0:
            game0();
        case 1:
            game1();
            break;
        case 2:
            game2();
            break;
        case 3:
            game3();
            break;
        case 4:
            game4();
            break;
        case 5:
            game5();
            break;
        case 6:
            if (Online_ck() != 0) {
                switch (game_w.step) {
                case 0:
                    game_w.step++;
                    all_reset();
                    Load_overlay(3, 1);
                    func_5D58C0();
                    AQ_session_exit_online();
                    break;
                case 1:
                    i = func_5B7020();
                    if (i != 0) {
                        InetSys.x8 = i;
                        InetSys.x6 = 100;
                        game_w.step++;
                        return;
                    }
                    break;
                case 2:
                    InetSys.x6--;
                    if (InetSys.x6 > 0) {
                        return;
                    }
                    if (InetSys.x8 == 2) {
                        COM_R_No_1 = 0;
                        *(u8 *)0x6EF8F5 = 3;
                        COM_R_No_0 = 1;
                        COM_R_No_2 = 0;
                        COM_R_No_3 = 0;
                        COM_RET = 0;
                        tsk->step = 2;
                        game_w.step = 1;
                        game_w.mode = 0;
                    } else {
                        COM_R_No_5 = 1;
                        COM_R_No_0 = 0;
                        COM_R_No_1 = 0;
                        COM_R_No_2 = 0;
                        COM_R_No_3 = 0;
                        COM_R_No_4 = 0;
                        COM_RET = 0;
                        tsk->step = 2;
                        game_w.step = 6;
                        game_w.mode = 0;
                    }
                    InetSys.x1 = 0;
                    return;
                }
            } else {
                all_reset();
                game_w.mode = 0;
                game_w.step = 0;
                tsk->step = 1;
            }
            break;
        }
        if (Online_ck() == 1) {
            AQ_send();
        }
        return;
    }
}
