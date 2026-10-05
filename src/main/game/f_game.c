/* Game mode state machine, matching part 1: game0 .. game2 (SLPM_654.95 main
 * 0x0010FC70-0x001109D0). game_w+0 is the mode, +1 the step inside a mode,
 * +2 a sub-step.  Names of fields are guesses (see docs/agents/agent-E.md). */
#include "f_game.h"

#define PL8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define PL16(p, o) (*(u16 *)((u8 *)(p) + (o)))

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
    stage_work.x01 = 1;
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
        stage_work.x01 = 1;
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
