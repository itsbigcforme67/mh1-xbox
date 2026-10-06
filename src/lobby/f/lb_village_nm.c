/* lb_village_nm.c - the village (lobby.bin offline: Kokoto village) code that
 * was not decompiled yet, written from the asm (m2c drafts via
 * tools/draft2c.py, then corrected by hand against the asm) so the PC port
 * can run the village on the game's own code. NOT built for the PS2 and
 * not compared with check.py: near-match quality at best, logic believed
 * to follow the asm. Addresses (lobby overlay, SLPM_654.95):
 *   Local_main 0x5D8680, Clear_lobby_ram 0x5D58C0, vs_square_* 0x5D8470-
 *   0x5D93A0 (village mode steps), Lb_stage_load 0x5D7050, Lb_npc_set
 *   0x5D7790, the village player (lb_basic_master 0x5D3660, lb_pl_turn_sub,
 *   lb_pl_mv052/088, Lb_St_unique_adr_set), NPCs (Lb_npc_mk/mv, lb_npc_*),
 *   talk targets (Lb_check_target, lb_check_status, Lb_talk_check_default),
 *   the quest counter (Lb_guild 0x5C5F30, lb_select_quest*, quest pages)
 *   and the menu drawing helpers they call.
 * Struct fields are raw offsets (M2C_FIELD) as in the drafts. */
#include "types.h"
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))
typedef double f64;
#define NULL ((void *)0)

int CameraInit();
int DemoCameraCheck();
int DemoCameraRequest();
int Disp_NowLoading();
int Eft25_set();
int Event_flag_ck();
int Event_flag_set();
int Ex_quest_ck();
int Fade_busy_ck();
int FlushCache();
int GetGroundHitStatusAreaEm();
int Get_sw2();
int Get_weapon_job();
int Gold_add();
int Gunner_wasure_ck();
int InitRenderState();
int Lb_Em_pos_adj();
int Lb_ItemBox_mv();
int Lb_ItemBox_open();
int Lb_Matching();
int Lb_Pl_act_set();
int Lb_Pl_act_set2();
int Lb_Pl_adj_calc();
int Lb_Pl_basic_flagset();
int Lb_Pl_chat_act_set();
int Lb_Pl_stg_ck();
int Lb_World_calc();
int Lb_act_ck();
int Lb_act_set();
int Lb_armor_shop();
int Lb_back_money();
int Lb_check_existF();
int Lb_check_hotel();
int Lb_check_money();
int Lb_cursorUD();
int Lb_eat();
int Lb_event_GH();
int Lb_event_guild();
int Lb_event_guildstart();
int Lb_event_localLast();
int Lb_event_localLv5End();
int Lb_event_market();
int Lb_get_angle();
int Lb_get_cursor_col();
int Lb_get_pl_stat2();
int Lb_get_quest_str();
int Lb_get_quest_type();
int Lb_gh_board();
int Lb_guild_check_requireF();
int Lb_hit_stop_calc();
int Lb_join();
int Lb_menu_quest_info();
int Lb_mix();
int Lb_move_common();
int Lb_npc_move_sub();
int Lb_num_to_str();
int Lb_pl_chr_set();
int Lb_pl_chr_set0();
int Lb_pl_init();
int Lb_pl_status_i();
int Lb_pl_status_m();
int Lb_pl_timer_calc();
int Lb_pl_to_chair();
int Lb_pl_to_normal();
int Lb_player_load();
int Lb_process_shop();
int Lb_put_chat();
int Lb_put_gold();
int Lb_put_icon();
int Lb_put_icon_free();
int Lb_put_job();
int Lb_put_msg();
int Lb_put_msg_type2();
int Lb_put_new_mail();
int Lb_put_npc_default();
int Lb_put_set01();
int Lb_put_status();
int Lb_put_unique_act_hint();
int Lb_reset();
int Lb_reset_talk_count();
int Lb_select();
int Lb_send_chair_req();
int Lb_send_commer();
int Lb_send_data_to_myself();
int Lb_send_myChair();
int Lb_send_pl_chidori_off();
int Lb_send_pl_status();
int Lb_send_pl_warp();
int Lb_send_stage();
int Lb_send_statusReq();
int Lb_send_trade_start();
int Lb_shop();
int Lb_shop_init();
int Lb_stick_dir_set();
int Lb_stick_pow_get();
int Lb_talk();
int Lbc_SendMiniData();
int Lbc_connect();
int Lbc_getDate();
int Lbc_init_network_work();
void Lbc_set_prim(void *, void *, void *);
int Lbs_GetRoomInfo();
int Lbs_InRoomCheck();
int Lbs_LobbyExit();
int Lbs_LogOutRequest();
int Lbs_MatchEntry();
int Lbs_MatchEntryCancel();
int Lbs_RoomExit();
int LegendSwordCameraRequest();
int McCardOperation();
int McOperationSet();
int NPCZoomInCameraCancel();
int NPCZoomInCameraRequest();
int NPC_Message();
int Online_ck();
int Paint_square();
int Pit_init();
int Pit_reset();
int Pl_light_set();
int Put_2TF();
int Q_camera_init();
int Quest_clear_bit_ck();
int Quest_init();
int Set06_set();
int SetDialogData();
int SetDialogData_HTML();
int SetDialogYesNo();
int SetFilterMode();
int SetTextureStage();
void SetVector(void *, f32, f32, f32);
int Set_userdata();
int SoftKeyboard_alive_check();
int SoftKeyboard_exit();
int Stage_unique_data_get();
int To_EnterPlaza2Lobby();
int Warehouse_equip_stack();
int Warehouse_search_space();
int add_prim();
int adx_se_set();
int all_reset();
int calc_vec_ang2();
int clay_attr_reset();
int clay_attr_set();
int cnWrap_SoundRequest();
int com_motion_load();
int cpAng2Rad_all();
int cpRotMatrix();
int cpRotMatrixYXZ2();
int eft01_set();
int em_work_set();
int fade_reset();
int fade_set();
int flCalcTransSI();
int flCompact();
f32 flConvertStoR(int);
int flExecuteClay();
int flMemset();
int flSetRenderState();
int flSetSkinTrans();
int flSndPackLoadBG();
int flSndPackLoadStatus();
int flSndPortStop();
f32 flSqrt(f32);
int flfntLocate();
int flfntSetSize();
int flmatCopy();
int flmatInit();
void flmatMakeScale(void *, f32, f32, f32);
int flmatMul33_2();
void flmatRotY33(void *, f32);
void flmatSetTrans(void *, f32, f32, f32);
int flps0002();
int flps0008();
int flvecApplyMat33();
f32 flvecCalcDistance(void *, void *);
int flvecCopy();
int flvecrRotTransPers();
int font_print();
int font_print_double();
int font_set_palette();
int font_set_stack_no();
int frame_init();
int frame_move();
int get_joint_mat_em();
int get_prim();
int get_prim_ptr();
int get_questLevelNum();
int get_quest_info();
int han2zen();
int init_set_work();
int lb_cat_material();
int lb_ck_unique_act();
int lb_exit_save();
int lb_goto_guest_room();
int lb_guild_end();
int lb_guild_make_room();
int lb_key_quest_ck();
int lb_member_changeCheck();
int lb_member_inCheck();
int lb_member_outCheck();
int lb_normal_material();
int lb_npc_item_trans();
int lb_put_sprite();
int lb_rule_seet_set();
void lbc_text_lobby_trans(u8 *);
int light_init();
int load_bin_req();
int load_busy_ck();
int load_eft();
int load_pit();
int load_shadow();
int lobby_bgm_set();
int lobby_bgm_set2();
int memcmp();
int npc_create_model();
int pl_flag_set();
int pl_light_change();
int pl_sleeping();
int pull_enemy_work();
int push_em_work();
int ran_suu();
int reload_tex();
int se_req();
int set_se_type();
int set_viewproj();
int softdip_ck();
int sound_call_005D3640();
int st_model_load();
int stage_fog_set();
int stage_set_set();
int stage_w_init();
int str_fadein();
int str_fadein_vol();
int str_fadeout();
int str_pause();
int str_stop();
int str_stop_all();
int str_volume();
int strcat();
int strrchr();
int trade_get_ck_005D0750();
int view_reset();
extern u8 ClassInfo[];
extern u8 D_32D471[];
extern u8 D_3C6FC8[];
extern u8 D_3C738C[];
extern u8 D_3E4C05[];
extern u8 D_3E4FA0[];
extern u8 D_3E5505[];
extern u8 D_3E5506[];
extern u8 D_3E55F0[];
extern u8 D_3E5FF0[];
extern u8 D_3E69F0[];
extern u8 D_3E73F0[];
extern u8 D_3E7DF0[];
extern u8 D_3E87F0[];
extern u8 D_3E91F0[];
extern u8 D_3F3714[];
extern u8 D_3F3728[];
extern u8 Lb_guild_trans[];
extern u8 Lb_trans_pl[];
extern u8 St_unique_tbl[];
extern u8 User_data[];
extern u8 client_work[];
extern u8 em_work[];
extern u8 flag_quest_tbl[];
extern u8 flag_quest_tbl_local[];
extern u8 guildStr[];
extern u8 helpLineTbl[];
extern u8 lbCommer[];
extern u8 lb_button_tbl[];
extern u8 lb_guild_str[];
extern u8 lb_npc_jijii_tbl[];
extern u8 lb_npc_prog_tbl[];
extern u8 lb_num_str[];
extern u8 lb_pit[];
extern u8 lb_player[];
extern u8 lb_quest_all[];
extern u8 lb_quest_attribute[];
extern u8 lb_quest_clear[];
extern u8 lb_quest_color_tbl[];
extern u8 lb_quest_color_tex[];
extern u8 lb_quest_data_tbl[];
extern u8 lb_quest_exp[];
extern u8 lb_quest_font_color[];
extern u8 lb_quest_info[];
extern u8 lb_quest_level_str[];
extern u8 lb_quest_message[];
extern u8 lb_return_pos[];
extern u8 lb_rule_exp[];
extern u8 lb_rule_msg_etc[];
extern u8 lb_start_pos[];
extern u8 lb_sys[];
extern u8 lit_1489_00664A90[];
extern u8 lit_1576_00664A98[];
extern u8 lit_2316[];
extern u8 lit_2418[];
extern u8 lit_2419[];
extern u8 lit_277_00665D90[];
extern u8 lit_278_00665D98[];
extern u8 lit_427_00664C20[];
extern u8 lit_428_00664C30[];
extern u8 lit_429_00664C38[];
extern u8 lit_430_00664C40[];
extern u8 lit_462_00664988[];
extern u8 map_name[];
extern u8 mhRule[];
extern u8 my_user_handle[];
extern u8 my_user_id[];
extern u8 my_user_mini_data[];
extern u8 network_work[];
extern u8 npcMv26_EVENT[];
extern u8 npcMv33_EVENT[];
extern u8 npcMv34_EVENT[];
extern u8 npc_dialog_table[];
extern u8 npc_disp_parts_00647910[];
extern u8 npc_sound_tbl[];
extern u8 ot1[];
extern u8 pl01_adr_tbl[];
extern u8 player_work[];
extern u8 put_back[];
extern u8 quest_local_tbl[];
extern u8 quest_lv_tbl[];
extern u8 quest_title[];
extern u8 room_price[];
extern u8 stage_start_pos[];
extern u8 vs_square_func_213[];
extern u16 System_timer;
extern s16 ang_894;
extern u8 * cw;
extern s32 guildPrice;
extern u8 key_quest;
extern s8 key_quest_num;
extern s8 lbSendInterval;
extern u8 * lbmw;
extern s32 pDetail;
extern u8 * pNet;
extern s32 pl_area_top;
extern s32 quest_price;
extern u8 sw_flag_1260;

s32 Local_main();
void vs_square_init_pre();
void vs_square_init_init();
void vs_square_init_se();
void vs_square_init();
void vs_square_exit();
void vs_square_event();
void Lb_stage_load();
void Lb_npc_set();
void Lb_load_player_all();
void Lb_set_player();
void Lb_set_mini_data();
void Lb_set_mini_data_to_pl();
void Lb_make_quest_tbl_local();
s32 lb_set_key_quest_local();
u8 get_flag_quest();
u8 get_new_quest();
void Lb_check_newCommer();
void lb_basic_master();
void lb_pl_turn_sub();
void Lb_St_unique_adr_set();
void lb_pl_mv052();
void lb_pl_mv088();
void Lb_npc_mk();
s32 Lb_npc_mv();
void lb_npc_init();
void lb_npc_init_sub();
void lb_npc_move();
void lb_npc_chr_sub();
void lb_npc_die();
void lb_npc_erase();
void lb_npc_effect_move();
void lb_set_npc();
void set_event_npc();
void lb_npc_trans();
void Lb_check_target();
s32 lb_check_target(u8 *pl, u8 *tgt, u8 **list, s32 lim, s32 mode, f32 range);
void lb_target_angle();
void lb_insert_target_list();
s32 Lb_ck_target();
s32 Lb_talk_check_default();
void lb_check_status();
s32 Lb_shop_sw();
void Lb_ck_menu();
s32 Lb_menu_move();
void Lb_menu_exit();
void lb_menu_init();
void Lb_guild();
void lb_guild_talk();
s32 lb_guild_startMsg();
s32 lb_select_quest();
s32 lb_select_quest_level();
s32 check_questLevelSelect();
s32 lb_get_quest_level();
void lb_set_questpage_info();
void lb_questpage_trans();
void lb_select_quest_level_trans();
void guild_trans_ot0();
void lb_disp_name();
void Lb_put_help();
s32 Lb_put_button();
void Lb_draw_square();
void Lb_put_2TF();

/* Local_main (0x5D8680): one tick of the village. lb_sys+3 is the step:
 * vs_square_func_213[step] = init_pre, init_init, init_se, init,
 * Lb_move_common (the walking loop), exit, event. lb_sys+0x68 = 32: a quest
 * was accepted (return 1), 37: leave (return -1). */
s32 Local_main(void) {
    M2C_FIELD(pNet, s8 *, 0xC) = 0;
    M2C_FIELD(pNet, u8 *, 0x11) = 0;
    font_set_stack_no(0);
    ((void (**)(void))vs_square_func_213)[M2C_FIELD(lb_sys, s8 *, 3)]();
    if (M2C_FIELD(pNet, u8 *, 0x11) == 0) {
        lbc_text_lobby_trans(network_work);
    }
    switch (M2C_FIELD(lb_sys, s32 *, 0x68)) {
    case 32:
        *(s8 *)0x3F35CC = 0;
        Pit_reset();
        Lbc_set_prim(0, 0, 0);
        return 1;
    case 37:
        Pit_reset();
        Lbc_set_prim(0, 0, 0);
        return -1;
    default:
        return 0;
    }
}

/* step 0 (0x5D8790): offline: all_reset, the hunter's save data into
 * player_work (Set_userdata), fade out */
void vs_square_init_pre(void) {
    if (Online_ck() == 0) {
        M2C_FIELD(pNet, s8 *, 0x11) = 1;
    }
    switch (M2C_FIELD(lb_sys, s8 *, 4)) {
    case 0:
        if (Online_ck() == 0) {
            all_reset();
            Set_userdata(player_work);
            Lb_set_mini_data(cw + (*(u8 *)0x3F34C1 * 0x2FC) + 0x1346);
            Lb_set_mini_data(lbCommer + (*(u8 *)0x3F34C1 * 0x5C) + 0x1C);
            *(s8 *)0x3F3603 = 0;
            *(s8 *)0x3F3604 = 0;
            *(s8 *)0x3F3605 = 0;
            *(s16 *)0x3F3606 = 0;
        }
        fade_set(0xA);
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        return;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        }
        return;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            M2C_FIELD(lb_sys, s8 *, 4) = 0;
            M2C_FIELD(lb_sys, s8 *, 3) = (s8)(M2C_FIELD(lb_sys, s8 *, 3) + 1);
        }
        break;
    }
}

/* step 1 (0x5D8920): quest table, camera, the master player, HUD, effects,
 * the village stage (0x57 offline) and its NPCs */
void vs_square_init_init(void) {
    u8 *pl;
    u16 st;
    int i;

    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    if (Online_ck() == 0) {
        M2C_FIELD(pNet, s8 *, 0x11) = 1;
    }
    switch (M2C_FIELD(lb_sys, s8 *, 4)) {
    case 0:
        if (Online_ck() == 0) {
            Lb_make_quest_tbl_local();
            Q_camera_init();
        }
        M2C_FIELD(cw, s8 *, 0x2C08) = 0;
        fade_reset();
        M2C_FIELD(lb_sys, s8 *, 3) = (s8)(M2C_FIELD(lb_sys, s8 *, 3) + 1);
        InitRenderState(0);
        Disp_NowLoading();
        if (Online_ck() == 0) {
            *(s8 *)0x3F34C3 = 1;
            Lb_set_player(0, lit_277_00665D90, D_3C6FC8);
            memcpy(my_user_handle, D_3C6FC8, 0x11);
            memcpy(my_user_id, lit_278_00665D98, 8);
            memcpy(lbCommer + (*(u8 *)0x3F34C1 * 0x5C) + 8, my_user_handle, 0x10);
            memcpy(lbCommer + (*(u8 *)0x3F34C1 * 0x5C), my_user_id, 8);
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
        M2C_FIELD(lb_sys, s8 *, 0x71) = -1;
        if (Online_ck() == 1) {
            st = 0x4C;
            if (M2C_FIELD(cw, u8 *, 0x35D2) != 0) {
                M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                st = 0x4D;
            }
        } else {
            st = 0x57;
        }
        M2C_FIELD(pl, u16 *, 0x73A) = st;
        Lb_stage_load(M2C_FIELD(pl, u16 *, 0x73A));
        M2C_FIELD(cw, u8 *, 0x35D2) = 0;
        Lbc_connect();
        Lb_npc_set(M2C_FIELD(pl, u16 *, 0x73A));
        for (i = 0; i < 8; i++)
            eft01_set(player_work + 0xA00 * i, 1);
        fade_set(0xA);
        return;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            M2C_FIELD(lb_sys, s8 *, 4) = 0;
            M2C_FIELD(lb_sys, s8 *, 3) = (s8)(M2C_FIELD(lb_sys, s8 *, 3) + 1);
            M2C_FIELD(cw, u8 *, 0x35D2) = 0;
        }
        return;
    }
}

/* step 2 (0x5D8C10): sound packs (stage SE 0x4761B0, 0x10004, the
 * village's), then either the first-visit event (step 6) or a memory card
 * check, then the walking loop (step 4) */
void vs_square_init_se(void) {
    s32 area = pl_area_top;
    s8 st;

    *(s8 *)0x3F36C5 = 1;
    if (Online_ck() == 0) {
        M2C_FIELD(pNet, s8 *, 0x11) = 1;
    }
    st = M2C_FIELD(lb_sys, s8 *, 4);
    switch (st) {
    case 0:
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(st + 1);
        flSndPortStop(1);
        FlushCache(0);
        load_bin_req(*(s16 *)0x4761B0 | 0x10000, area);
        return;
    case 1:
        if (load_busy_ck() == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            flSndPackLoadBG(area, 1);
        }
        return;
    case 2:
        if (flSndPackLoadStatus() == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        }
        return;
    case 3:
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(st + 1);
        flSndPortStop(6);
        FlushCache(0);
        load_bin_req(0x10004, area);
        return;
    case 4:
        if (load_busy_ck() == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            flSndPackLoadBG(area, 6);
        }
        return;
    case 5:
        if (flSndPackLoadStatus() == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        }
        return;
    case 6:
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(st + 1);
        flSndPortStop(7);
        FlushCache(0);
        load_bin_req((*(s16 *)0x32D650 + 7) | 0x10000, area);
        return;
    case 7:
        if (load_busy_ck() == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            flSndPackLoadBG(area, 7);
        }
        return;
    case 8:
        if (flSndPackLoadStatus() == 0) {
            if ((Online_ck() == 1) || (*(u8 *)0x3F36CF == 0)) {
                if ((Event_flag_ck(2) == 1) || (Online_ck() == 1)) {
                    M2C_FIELD(lb_sys, s8 *, 4) = 0xB;
                } else {
                    M2C_FIELD(cw, s8 *, 0x2C08) = 1;
                    fade_set(2);
                    M2C_FIELD(lb_sys, s8 *, 4) = 0;
                    M2C_FIELD(lb_sys, s8 *, 3) = 6;
                }
            } else {
                M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
                str_fadeout(0, 0xF);
                M2C_FIELD(cw, s32 *, 0x2C4C) = 0xF;
                McOperationSet(9);
                Lbc_set_prim(put_back, 0, 0);
            }
            *(u8 *)0x3F36C5 = 0;
        }
        return;
    case 9:
        if (M2C_FIELD(cw, s32 *, 0x2C4C) != 0) {
            M2C_FIELD(cw, s32 *, 0x2C4C) -= 1;
            return;
        }
        str_pause(0, 1);
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        fade_set(2);
        M2C_FIELD(pNet, s8 *, 0x11) = 0;
        return;
    case 10:
        M2C_FIELD(pNet, s8 *, 0x11) = 0;
        if (McCardOperation() & 0xFF) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            fade_set(0xA);
            str_pause(0, 0);
            str_volume(0, 0);
            str_fadein(0, 0xF);
        }
        return;
    case 11:
        M2C_FIELD(pNet, s8 *, 0x11) = 0;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            M2C_FIELD(cw, s8 *, 0x2C08) = 1;
            fade_set(2);
            lobby_bgm_set(*(u8 *)0x3F3404);
            M2C_FIELD(lb_sys, s8 *, 4) = 0;
            M2C_FIELD(lb_sys, s8 *, 3) = (s8)(M2C_FIELD(lb_sys, s8 *, 3) + 2);
            Lb_send_commer();
        }
        return;
    default:
        return;
    }
}

/* step 3 (0x5D9050): a stage change inside the village (house, farm ...) */
void vs_square_init(void) {
    u8 *pl;
    u8 st;

    M2C_FIELD(cw, s8 *, 0x2C08) = 0;
    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
    Disp_NowLoading();
    Lb_stage_load(M2C_FIELD(pl, u16 *, 0x73A));
    if (Online_ck() == 0) {
        str_pause(0, 0);
        str_volume(0, 0);
        str_fadein_vol(0, 0xF, *(D_32D471 + (*(u8 *)0x3F3404 * 2)));
    } else {
        st = *(u8 *)0x3F3404;
        if ((st == 0x4E) && (M2C_FIELD(cw, u32 *, 0xBF3C) >= 4U)) {
            lobby_bgm_set2(0x11);
        } else {
            lobby_bgm_set(st);
        }
    }
    Lb_npc_set(M2C_FIELD(pl, u16 *, 0x73A));
    fade_set(2);
    M2C_FIELD(lb_sys, s8 *, 4) = 0;
    M2C_FIELD(lb_sys, s8 *, 0x87) = 0x14;
    M2C_FIELD(lb_sys, s8 *, 3) = (s8)(M2C_FIELD(lb_sys, s8 *, 3) + 1);
    Lb_check_newCommer();
    M2C_FIELD(cw, s8 *, 0x2C08) = 1;
}

/* step 5 (0x5D91A0): leaving a village stage: fade, then step 3 loads the
 * next one (player+0x73A) */
void vs_square_exit(void) {
    u8 *pl;
    u8 st;
    s32 r;

    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    switch (M2C_FIELD(lb_sys, s8 *, 4)) {
    case 0:
        st = *(u8 *)0x3F3404;
        if (st == 0x4F || st == 0x50) {
            if ((M2C_FIELD(lb_sys, u8 *, 0x78) != 0) && (M2C_FIELD(pl, u16 *, 0x73A) == 0x4C)) {
                Lbc_SendMiniData();
            }
        }
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        /* fallthrough */
    case 1:
        fade_set(1);
        str_fadeout(0, 0x10);
        Lb_move_common();
        if (M2C_FIELD(pl, u16 *, 0x73A) == 0x4E) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            return;
        }
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 2);
        return;
    case 2:
        r = Lbc_getDate();
        if (r == 1 || r == 0) {
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        }
        Lb_move_common();
        return;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if (Online_ck() == 0) {
                str_pause(0, 1);
            } else {
                str_stop_all(0);
            }
            Lb_reset();
            if (M2C_FIELD(pl, u16 *, 0x73A) == 0x63) {
                M2C_FIELD(lb_sys, s8 *, 3) = 3;
                return;
            }
            M2C_FIELD(lb_sys, s8 *, 5) = 0;
            M2C_FIELD(lb_sys, s8 *, 3) = 3;
            return;
        }
        Lb_move_common();
        return;
    }
}

/* step 6 (0x5D8470): the first visit's event (demo cameras 0xB / 0xC,
 * stage 0x56 then back) */
void vs_square_event(void) {
    u8 *pl;
    s32 r;

    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    Lb_move_common();
    switch (M2C_FIELD(lb_sys, s8 *, 4)) {
    case 0:
        DemoCameraRequest(0xB, 0);
        adx_se_set(pl, 0x41);
        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
        M2C_FIELD(lb_sys, s32 *, 0x68) = 0x21;
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        return;
    case 1:
        M2C_FIELD(pl, s8 *, 1) = 0;
        r = DemoCameraCheck();
        if ((r != -1) && (r != 0)) {
            return;
        }
        M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
        M2C_FIELD(pl, u16 *, 0x73A) = 0x56;
        M2C_FIELD(lb_sys, s8 *, 0x71) = -1;
        fade_set(0xA);
        return;
    case 2:
        M2C_FIELD(pl, s8 *, 1) = 0;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            Disp_NowLoading();
            str_stop_all();
            Lb_reset();
            Lb_stage_load(M2C_FIELD(pl, u16 *, 0x73A));
            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x21;
            Lb_npc_set(M2C_FIELD(pl, u16 *, 0x73A));
            fade_set(2);
            M2C_FIELD(lb_sys, s8 *, 4) = (s8)(M2C_FIELD(lb_sys, s8 *, 4) + 1);
            lobby_bgm_set(*(u8 *)0x3F3404);
            DemoCameraRequest(0xC, 0);
        }
        return;
    case 3:
        M2C_FIELD(pl, s8 *, 1) = 1;
        r = DemoCameraCheck();
        if ((r != -1) && (r != 0)) {
            return;
        }
        M2C_FIELD(lb_sys, s8 *, 4) = 0;
        M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
        M2C_FIELD(lb_sys, s8 *, 3) = 4;
        Event_flag_set(2);
        break;
    }
}

/* copy 8 bytes (an ld/sd pair of two floats) without going through a float */
#define CP8(d, s) memcpy((u8 *)(d), (u8 *)(s), 8)

/* place the master player from a 0x10-byte start record {x, y, z, u16 ang} */
static void lb_put_at(u8 *pl, u8 *rec)
{
    M2C_FIELD(pl, f32 *, 0xAC) = M2C_FIELD(rec, f32 *, 0);
    CP8(pl + 0xB0, rec + 4);
    M2C_FIELD(pl, u16 *, 0xE) = M2C_FIELD(rec, u16 *, 0xC);
    M2C_FIELD(pl, s32 *, 0xA4) = M2C_FIELD(rec, u16 *, 0xC);
}

/* the stage's own start position (stage_start_pos, angle 0) */
static void lb_put_start(u8 *pl, int st)
{
    u8 *p = stage_start_pos + st * 0xC;
    M2C_FIELD(pl, f32 *, 0xAC) = M2C_FIELD(p, f32 *, 0);
    CP8(pl + 0xB0, p + 4);
    M2C_FIELD(pl, u16 *, 0xE) = 0;
    M2C_FIELD(pl, s32 *, 0xA4) = 0;
}

/* a St_unique_tbl[st] record (0x18 bytes: +4 x, +8 y z, +0x14 angle) */
static void lb_put_unique(u8 *pl, int st, int k)
{
    u8 *p = *(u8 **)(St_unique_tbl + st * 4) + k * 0x18;
    M2C_FIELD(pl, f32 *, 0xAC) = M2C_FIELD(p, f32 *, 4);
    CP8(pl + 0xB0, p + 8);
    M2C_FIELD(pl, s32 *, 0xA4) = M2C_FIELD(p, u16 *, 0x14);
    M2C_FIELD(pl, u16 *, 0xE) = M2C_FIELD(p, u16 *, 0x14);
}

/* Lb_stage_load (0x5D7050): load village stage st (0x57 Kokoto village,
 * 0x56 ...): stage files (st_model_load), camera, the players' work, light,
 * fog, set objects; then put the master player at the stage's start
 * (lb_sys+0x71 = -1: arriving, else the entrance index from St_unique_tbl) */
void Lb_stage_load(s32 st) {
    u8 *pl;
    int k;

    st &= 0xFF;

    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    if (M2C_FIELD(lb_sys, u8 *, 0x70) == 0) {
        Lb_load_player_all();
    }
    flCompact();
    *(u8 *)0x3F3404 = st;
    M2C_FIELD(pl, u8 *, 0x736) = st;
    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
    stage_w_init();
    *(s8 *)0x3D8231 = 1;
    set_viewproj(0);
    view_reset();
    fade_reset();
    st_model_load(st);
    Lbc_connect();
    CameraInit();
    Lb_pl_init();
    light_init(st);
    *(s8 *)0x3F36BF = 0xFF;
    stage_fog_set(st);
    stage_set_set(st);
    if (M2C_FIELD(lb_sys, s32 *, 0x68) != 0x21) {
        M2C_FIELD(lb_sys, s32 *, 0x68) = 0;
    }
    M2C_FIELD(pl, s8 *, 0x8EC) = 0;
    M2C_FIELD(pl, s8 *, 0x90F) = 0;
    M2C_FIELD(pl, s32 *, 0x878) = 0;
    M2C_FIELD(pl, s8 *, 0x936) = 0;
    M2C_FIELD(pl, s8 *, 0x7ED) = 0;
    if (*(u8 *)0x3F3404 == 0x4E) {
        switch (M2C_FIELD(cw, s32 *, 0xBF3C) & 3) {
        case 1:
            Set06_set(2);
            break;
        case 2:
            Set06_set(0);
            break;
        case 3:
            Set06_set(1);
            break;
        }
    } else if ((*(u8 *)0x3F3404 == 0x57) && (Event_flag_ck(1) == 0)) {
        Set06_set(0);
    }
    if (M2C_FIELD(lb_sys, s8 *, 0x71) == -1) {
        switch (*(u8 *)0x3F3404) {
        case 0x57:
            lb_put_at(pl, lb_start_pos + 0xD0);
            break;
        case 0x56:
            if (M2C_FIELD(lb_sys, s32 *, 0x68) == 0x21) {
                lb_put_at(pl, lb_start_pos + 0xE0);
                Lb_act_set(pl, 0, 0x4F);
                Lb_pl_chr_set(pl, 0x1AB, 0, 0);
            } else {
                lb_put_start(pl, st);
            }
            break;
        case 0x4C:
            k = M2C_FIELD(cw, u8 *, 0x35D1) & 7;
            lb_put_at(pl, lb_start_pos + k * 0x10);
            break;
        default:
            lb_put_start(pl, st);
            break;
        }
    } else if (M2C_FIELD(lb_sys, s8 *, 0x71) == 0) {
        switch (st) {
        case 0x4D:
            if (M2C_FIELD(cw, u8 *, 0x35D2) == 0) {
                lb_put_at(pl, lb_start_pos + 0x80);
            } else {
                k = M2C_FIELD(cw, u8 *, 0x35D1) & 3;
                lb_put_at(pl, lb_return_pos + k * 0x10);
            }
            break;
        case 0x50:
            lb_put_at(pl, lb_start_pos + 0x90);
            break;
        case 0x52:
            lb_put_at(pl, lb_start_pos + 0xA0);
            break;
        case 0x53:
            lb_put_at(pl, lb_start_pos + 0xB0);
            break;
        case 0x55:
            lb_put_at(pl, lb_start_pos + 0xC0);
            break;
        case 0x56:
            lb_put_at(pl, lb_start_pos + 0xA0);
            break;
        case 0x57:
            lb_put_at(pl, lb_start_pos + 0xF0);
            break;
        default:
            lb_put_unique(pl, st, M2C_FIELD(lb_sys, s8 *, 0x71));
            break;
        }
    } else {
        lb_put_unique(pl, st, M2C_FIELD(lb_sys, s8 *, 0x71));
    }
    M2C_FIELD(pl, s32 *, 0x3B0) = 0;
    if (M2C_FIELD(lb_sys, s32 *, 0x68) != 0x21) {
        if (M2C_FIELD(pl, u8 *, 0x11) == 1) {
            Lb_pl_to_normal(pl, 0, 0, 0);
            Lb_pl_chr_set(pl, 0x3F, 0, 0x6A);
        } else {
            Lb_pl_to_normal(pl, 0, 0, 0);
        }
    }
    if (Online_ck() == 1) {
        Lb_send_stage();
    }
    Lb_send_pl_warp(pl);
    Lb_send_pl_status(pl);
    if (*(u8 *)0x3F3404 == 0x4C || *(u8 *)0x3F3404 == 0x4D) {
        int i;
        for (i = 0; i < 8; i++) {
            u8 *p = player_work + 0xA00 * i;
            if (*p != 0 && M2C_FIELD(cw + i, s8 *, 0x2BFE) == 0) {
                M2C_FIELD(cw + i, s8 *, 0x2BFE) = 1;
                Lb_player_load(p, cw + i + 0x2BFE);
                Lbc_connect();
            }
        }
    }
}

/* Lb_npc_set (0x5D7790): take npc_dialog_table[0x174 + st] em_work slots
 * as the stage's NPCs (+0x1E = 1 marks an NPC; +0xC / +0x1B its number).
 * Village (0x57): one more once quest-clear bit 0x88 is set. */
void Lb_npc_set(s32 st) {
    s32 n, i, k;
    u8 *em;

    n = npc_dialog_table[0x174 + st];
    switch (st) {
    case 0x57:
        if (Quest_clear_bit_ck(0x88) == 1) {
            n += 1;
        }
        break;
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
        k = (s16)(*(u8 *)0x3F3404 - 0x51);
        if (k < 0 || k >= 0x56) {
            k = 0;
        }
        if (lb_sys[0x88 + k] == 1) {
            n -= 1;
        }
        break;
    }
    for (i = 0; i < n; i++) {
        em = (u8 *)pull_enemy_work();
        if (em != NULL) {
            M2C_FIELD(em, s8 *, 0x1E) = 1;
            M2C_FIELD(em, s8 *, 0x34F) = 0;
            M2C_FIELD(em, s16 *, 0xC) = (s16)i;
            M2C_FIELD(em, s8 *, 0x1B) = (s8)i;
            M2C_FIELD(em, u8 *, 0x736) = *(u8 *)0x3F3404;
        }
    }
}

/* Lb_load_player_all (0x5CCF60 region): common motions, every player's
 * model, the four NPC model sets (once per lobby visit, lb_sys+0x70) */
void Lb_load_player_all(void) {
    u32 i;

    if (M2C_FIELD(lb_sys, u8 *, 0x70) == 0) {
        if (M2C_FIELD(cw, s8 *, 0x2C06) == 0) {
            M2C_FIELD(cw, s8 *, 0x2C06) = 1;
            com_motion_load(1);
        }
        for (i = 0; i < 8; i++) {
            u8 *p = player_work + 0xA00 * i;
            if (*p != 0 && M2C_FIELD(cw + i, s8 *, 0x2BFE) == 0) {
                M2C_FIELD(cw + i, s8 *, 0x2BFE) = 1;
                Lb_player_load(p);
                Lbc_connect();
            }
        }
        npc_create_model(0);
        Lbc_connect();
        npc_create_model(1);
        Lbc_connect();
        npc_create_model(2);
        Lbc_connect();
        npc_create_model(3);
        Lbc_connect();
        M2C_FIELD(lb_sys, u8 *, 0x70) = 1;
    }
}

/* Lb_set_player (lobby): player no in use, name (17 bytes) and id
 * (8 bytes) into lb_player[no] and the PLW, scale 1, a draw prim
 * (Lb_trans_pl), the normal action and pl01_adr_tbl's init (+0xC) */
void Lb_set_player(s32 no, u8 *id, u8 *name) {
    u8 *lp, *pl;
    s16 h;

    no &= 0xFF;
    lp = lb_player + no * 0x38;
    pl = player_work + no * 0xA00;
    *(u8 **)lp = pl;
    M2C_FIELD(pl, s8 *, 0) = 1;
    M2C_FIELD(pl, u16 *, 0xC) = (u16)no;
    memcpy(lp + 4, name, 0x11);
    memcpy(lp + 0x24, id, 8);
    memcpy(pl + 0x8D4, name, 0x11);
    M2C_FIELD(pl, s32 *, 0xC0) = 0x3F800000;
    M2C_FIELD(pl, s32 *, 0xBC) = 0x3F800000;
    M2C_FIELD(pl, s32 *, 0xB8) = 0x3F800000;
    M2C_FIELD(pl, s16 *, 0x300) = 2;
    M2C_FIELD(pl, s32 *, 0x798) = 0x3F800000;
    M2C_FIELD(pl, s32 *, 0x1A0) = 0x3F800000;
    M2C_FIELD(pl, s32 *, 0x1F0) = 0x3F800000;
    M2C_FIELD(pl, s16 *, 0x568) = get_prim();
    M2C_FIELD(pl, s8 *, 0x4D4) = 1;
    h = M2C_FIELD(pl, s16 *, 0x568);
    if (h != -1) {
        M2C_FIELD(pl, u8 **, 0x564) = (u8 *)get_prim_ptr(h);
        M2C_FIELD(M2C_FIELD(pl, u8 **, 0x564), s32 *, 0x18) = M2C_FIELD(pl, u16 *, 0xC);
        M2C_FIELD(M2C_FIELD(pl, u8 **, 0x564), u8 **, 0x14) = Lb_trans_pl;
    }
    M2C_FIELD(pl, s8 *, 0x8F0) = 1;
    Lb_pl_to_normal(pl, 0, 0, 0);
    M2C_FIELD(pl, u8 **, 0x3CC) = pl01_adr_tbl;
    (*(void (**)(u8 *))(pl01_adr_tbl + 0xC))(pl);
}

/* Lb_set_mini_data (lobby): the master player's 0x18-byte summary
 * (job, rank, armour/look bytes, weapon type/id) for the other players */
void Lb_set_mini_data(u8 *out) {
    u8 b[0x18];
    u8 *pl;

    memset(b, 0, sizeof b);
    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    M2C_FIELD(b, s32 *, 4) = M2C_FIELD(pl, s32 *, 0x5FC);
    memcpy(b + 0xE, pl + 0x352, 6);
    b[3] = M2C_FIELD(pl, u8 *, 0x11);
    b[2] = M2C_FIELD(pl, u8 *, 0x915);
    b[0x15] = M2C_FIELD(pl, u8 *, 0x916);
    b[1] = *(u8 *)0x3C733B;
    M2C_FIELD(b, s16 *, 8) = M2C_FIELD(D_3C738C, s16 *, 0);
    M2C_FIELD(b, s16 *, 0xA) = M2C_FIELD(D_3C738C, s16 *, 2);
    M2C_FIELD(b, s16 *, 0xC) = M2C_FIELD(D_3C738C, s16 *, 4);
    b[0] = (u8)Get_weapon_job(D_3C738C);
    if (b[0] == 5) {
        b[0] = 1;
    }
    b[0x14] = *(u8 *)0x3C6FC3;
    b[0x16] = *(u8 *)0x3C7397;
    memcpy(out, b, 0x18);
}

/* Lb_set_mini_data_to_pl (0x...): the reverse, into player_work[no] */
void Lb_set_mini_data_to_pl(s32 no, u8 *src) {
    u8 b[0x18];
    u8 *pl;

    pl = player_work + (s8)no * 0xA00;
    memcpy(b, src, 0x18);
    M2C_FIELD(pl, s32 *, 0x5FC) = M2C_FIELD(b, s32 *, 4);
    M2C_FIELD(pl, u8 *, 0x11) = b[3];
    M2C_FIELD(pl, u8 *, 0x34E) = b[0x14];
    M2C_FIELD(pl, u8 *, 0x8D3) = b[0x16];
    M2C_FIELD(pl, u8 *, 0x915) = b[2];
    M2C_FIELD(pl, u8 *, 0x916) = b[0x15];
    M2C_FIELD(pl, s16 *, 0x35E) = M2C_FIELD(b, s16 *, 8);
    M2C_FIELD(pl, s16 *, 0x360) = M2C_FIELD(b, s16 *, 0xA);
    M2C_FIELD(pl, s16 *, 0x362) = M2C_FIELD(b, s16 *, 0xC);
    memcpy(pl + 0x352, b + 0xE, 6);
}

void Lb_make_quest_tbl_local(void) {
    s32 sp100;
    u32 spF0;
    s32 spE0;
    u8 spD0;
    u8 spC0;
    u8 *spB0;  /* was u8 **spB0 */
    s8 *spA0;
    u8 *var_a1;  /* was int *var_a1 */
    u8 *var_s3;  /* was int *var_s3 */
    s32 temp_s5;
    s32 var_a0;
    s32 var_a2_2;
    s32 var_s0;
    s32 var_s1;
    s32 var_t1;
    s8 *var_a2;
    u32 temp_fp;
    u32 temp_hi;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_s2;
    u8 temp_a0_2;
    u8 temp_a1_3;
    u8 temp_s4;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 *var_a3;  /* was u8 **var_a3 */
    u8 *temp_s6;
    u8 *temp_t0;

    key_quest = 0U;
    key_quest_num = 0;
    spF0 = 0;   /* never set before its first read on the PS2 (stack garbage) */
    spE0 = 0;
    spD0 = 0;
    spC0 = 0;
    sp100 = (s32) (((s8)(lb_get_quest_level(1))));
    var_t1 = 0;
    var_a3 = quest_local_tbl;
    var_a2 = (s8 *)lb_quest_clear;
    var_a1 = lb_quest_info;
    do {
        temp_t0 = (*(u8 **)var_a3);
        var_t1 += 1;
        *var_a2 = 0;
        var_a3 += 4;
        var_a2 += 1;
        M2C_FIELD(var_a1, u8 *, 0) = (u8) M2C_FIELD(temp_t0, u8 *, 0);
        M2C_FIELD(var_a1, u8 *, 1) = (u8) M2C_FIELD(temp_t0, u8 *, 1);
        M2C_FIELD(var_a1, u8 *, 2) = (u8) M2C_FIELD(temp_t0, u8 *, 2);
        M2C_FIELD(var_a1, u8 *, 3) = (u8) M2C_FIELD(temp_t0, u8 *, 3);
        M2C_FIELD(var_a1, u8 *, 4) = (u8) M2C_FIELD(temp_t0, u8 *, 4);
        var_a1 += 5;
    } while (var_t1 < 5);
    M2C_FIELD(lb_quest_info, u8 *, 0x19) = 0U;
    M2C_FIELD(lb_quest_info, s8 *, 0x1A) = 0;
    M2C_FIELD(lb_quest_info, s8 *, 0x1B) = 0;
    M2C_FIELD(lb_quest_info, s8 *, 0x1C) = 0;
    M2C_FIELD(lb_quest_info, s8 *, 0x1D) = 0;
    if (sp100 == -1) {
        key_quest = 0x83U;
    } else {
        spB0 = quest_local_tbl;
        var_s1 = 0;
        var_s3 = lb_quest_info;
        spA0 = (s8 *)lb_quest_clear;
        do {
            temp_s6 = (*(u8 **)spB0);
            temp_s4 = get_new_quest(var_s1) & 0xFF;
            if (temp_s4 == 0) {
                *spA0 = 1;
            }
            if (sp100 == 5) {
                key_quest = 0xAAU;
            } else if (sp100 == var_s1) {
                spE0 = 1;
                temp_v1 = get_flag_quest(sp100) & 0xFF;
                spD0 = temp_v1;
                if (temp_v1 == 0) {
                    spE0 = 0;
                    switch (sp100) {                /* irregular */
                    case 0:
                        key_quest = 0x88U;
                        break;
                    case 1:
                        key_quest = 0x89U;
                        break;
                    case 2:
                        key_quest = 0x9AU;
                        break;
                    case 3:
                        key_quest = 0x8BU;
                        break;
                    case 4:
                        key_quest = 0xABU;
                        break;
                    }
                } else if (var_s1 != 0) {
                    var_a2_2 = 0;
                    if (var_s1 != 1) {
loop_31:
                        temp_a1 = var_s3 + var_a2_2;
                        if (spD0 == *temp_a1) {
                            *temp_a1 = M2C_FIELD(var_s3, u8 *, 4);
                            M2C_FIELD(var_s3, u8 *, 4) = spD0;
                        } else {
                            var_a2_2 += 1;
                            if (var_a2_2 < 5) {
                                goto loop_31;
                            }
                        }
                        if (var_a2_2 == 5) {
                            M2C_FIELD(var_s3, u8 *, 4) = spD0;
                        }
                    }
                }
            }
            if (temp_s4 != 0) {
                if ((var_s1 != 0) && (var_s1 != 1)) {
                    spE0 = 1;
                    var_a0 = 0;
loop_40:
                    temp_a1_2 = var_s3 + var_a0;
                    if (temp_s4 == *temp_a1_2) {
                        *temp_a1_2 = M2C_FIELD(var_s3, u8 *, 0);
                        M2C_FIELD(var_s3, u8 *, 0) = temp_s4;
                    } else {
                        var_a0 += 1;
                        if (var_a0 < 5) {
                            goto loop_40;
                        }
                    }
                    if (var_a0 == 5) {
                        M2C_FIELD(var_s3, u8 *, 0) = temp_s4;
                    }
                }
            } else {
                spC0 += 1;
            }
            if ((var_s1 != 0) && (var_s1 != 1)) {
                var_s0 = 5;
                if (M2C_FIELD(temp_s6, u8 *, 5) != 0) {
                    temp_fp = 5 - spE0;
                    do {
                        if (spF0 == 0) {
                            spF0 = ran_suu(1) & 0xFFFF;
                        }
                        temp_s2 = temp_s6 + var_s0;
                        if (((*temp_s2 != 0xAB) || (Quest_clear_bit_ck(0xAB) == 1)) && ((spF0 >> (var_s0 % 32)) & 1)) {
                            temp_v1_2 = *temp_s2;
                            if ((spD0 != temp_v1_2) && (temp_s4 != temp_v1_2)) {
                                temp_hi = (u32) (ran_suu(1) & 0xFFFF) % temp_fp;
                                if (0) {

                                }
                                temp_a1_3 = *temp_s2;
                                temp_s5 = spE0 + temp_hi;
                                if ((temp_a1_3 != 0x90) || (Quest_clear_bit_ck(0x94, temp_a1_3, temp_hi) == 1)) {
                                    temp_a0 = var_s3 + temp_s5;
                                    if ((spD0 != *temp_a0) && ((temp_s5 != 0) || (temp_s4 == 0))) {
                                        *temp_a0 = *temp_s2;
                                    }
                                }
                            }
                        }
                        var_s0 += 1;
                    } while (*(temp_s6 + var_s0) != 0);
                }
            }
            var_s1 += 1;
            spB0 += 4;
            spA0 += 1;
            var_s3 += 5;
        } while (var_s1 < 5);
    }
    if ((spC0 == 5) && (Event_flag_ck(0x4C) == 0)) {
        Event_flag_set(0x4C);
    }
    temp_a0_2 = key_quest;
    if (temp_a0_2 == 0xAA) {
        key_quest_num = lb_set_key_quest_local(temp_a0_2);
        return;
    }
    if (temp_a0_2 != 0) {
        M2C_FIELD(lb_quest_info, u8 *, 0x19) = temp_a0_2;
        key_quest_num = 1;
    }
}

s32 lb_set_key_quest_local(void) {
    s32 var_s0;
    s8 *temp_v1;
    s8 *temp_v1_2;
    s8 *temp_v1_3;
    s8 *temp_v1_4;

    if (Event_flag_ck(0x4C) == 0) {
        return 0;
    }
    var_s0 = 0;
    if ((Quest_clear_bit_ck(0xAF) == 1) && (Quest_clear_bit_ck(0xAE) == 1) && (Quest_clear_bit_ck(0xAD) == 1) && (Quest_clear_bit_ck(0xAC) == 1)) {
        var_s0 = ((s8)(1));
        M2C_FIELD(lb_quest_info, s8 *, 0x19) = 0xAA;
    }
    if (Ex_quest_ck(User_data, 2) == 1) {
        temp_v1 = (s8 *)lb_quest_info + 0x19 + (((s8)(var_s0)));
        var_s0 = ((s8)((var_s0 + 1)));
        *temp_v1 = 0xAF;
    }
    if (Ex_quest_ck(User_data, 1) == 1) {
        temp_v1_2 = (s8 *)lb_quest_info + 0x19 + (((s8)(var_s0)));
        var_s0 = ((s8)((var_s0 + 1)));
        *temp_v1_2 = 0xAE;
    }
    if (Ex_quest_ck(User_data, 0) == 1) {
        temp_v1_3 = (s8 *)lb_quest_info + 0x19 + (((s8)(var_s0)));
        var_s0 = ((s8)((var_s0 + 1)));
        *temp_v1_3 = 0xAD;
    }
    if (Quest_clear_bit_ck(0xAD) == 1) {
        temp_v1_4 = (s8 *)lb_quest_info + 0x19 + (((s8)(var_s0)));
        var_s0 = ((s8)((var_s0 + 1)));
        *temp_v1_4 = 0xAC;
    }
    return var_s0;
}

u8 get_flag_quest(s32 arg0) {
    s32 temp_hi;
    s32 temp_s1;
    s32 temp_v1;
    s32 var_s2;
    s32 var_s3;
    s32 var_v1;
    u8 *var_v0;  /* was u8 **var_v0 */
    u8 *temp_s0;
    u8 *temp_s4;

    if (arg0 == -1) {
        return 0U;
    }
    if (Online_ck() == 1) {
        var_v0 = flag_quest_tbl + (arg0 * 4);
    } else {
        var_v0 = flag_quest_tbl_local + (arg0 * 4);
    }
    temp_s4 = (*(u8 **)var_v0);
    var_v1 = 0;
    if (*temp_s4 != 0) {
        do {
            var_v1 = (var_v1 + 1) & 0xFF;
        } while (*(temp_s4 + var_v1) != 0);
    }
    temp_s1 = var_v1 & 0xFF;
    if (temp_s1 == 0) {
        return 0U;      /* the PS2 traps (break 7) on an empty list */
    }
    temp_hi = (s32) (ran_suu(1) & 0xFFFF) % temp_s1;
    var_s3 = 0;
    var_s2 = temp_hi & 0xFF;
    if (temp_s1 > 0) {
loop_13:
        temp_v1 = var_s2 & 0xFF;
        if ((temp_v1 < 0x67) || (temp_v1 >= 0x6B)) {
            temp_s0 = temp_s4 + (var_s2 & 0xFF);
            if (Quest_clear_bit_ck(*temp_s0) == 0) {
                if (*temp_s0 == 0x90) {
                    if (Quest_clear_bit_ck(0x94U) == 1) {
                        goto block_19;
                    }
                    goto block_20;
                }
block_19:
                return *temp_s0;
            }
        }
block_20:
        var_s2 = (var_s2 + 1) & 0xFF;
        if (var_s2 >= temp_s1) {
            var_s2 = 0;
        }
        var_s3 = (var_s3 + 1) & 0xFF;
        if (var_s3 >= temp_s1) {
            goto block_24;
        }
        goto loop_13;
    }
block_24:
    return 0U;
}

u8 get_new_quest(s32 arg0) {
    s32 temp_hi;
    s32 temp_s1;
    s32 var_s2;
    s32 var_s3;
    s32 var_v1;
    u8 *var_v0;  /* was u8 **var_v0 */
    u8 *temp_s0;
    u8 *temp_s4;

    if (arg0 == -1) {
        return 0U;
    }
    if (Online_ck() == 1) {
        var_v0 = quest_lv_tbl + (arg0 * 4);
    } else {
        var_v0 = quest_local_tbl + (arg0 * 4);
    }
    temp_s4 = (*(u8 **)var_v0);
    var_v1 = 0;
    if (*temp_s4 != 0) {
        do {
            var_v1 = (var_v1 + 1) & 0xFF;
        } while (*(temp_s4 + var_v1) != 0);
    }
    temp_s1 = var_v1 & 0xFF;
    if (temp_s1 == 0) {
        return 0U;      /* the PS2 traps (break 7) on an empty list */
    }
    temp_hi = (s32) (ran_suu(1) & 0xFFFF) % temp_s1;
    var_s3 = 0;
    var_s2 = temp_hi & 0xFF;
    if (temp_s1 > 0) {
loop_13:
        temp_s0 = temp_s4 + (var_s2 & 0xFF);
        if ((Quest_clear_bit_ck(*temp_s0) == 0) && ((Online_ck() == 0) || ((s32) *temp_s0 < 0x83)) && (lb_key_quest_ck(*temp_s0) == 0)) {
            if (*temp_s0 == 0x90) {
                if (Quest_clear_bit_ck(0x94U) == 1) {
                    goto block_20;
                }
                goto block_21;
            }
block_20:
            return *temp_s0;
        }
block_21:
        var_s2 = (var_s2 + 1) & 0xFF;
        if (var_s2 >= temp_s1) {
            var_s2 = 0;
        }
        var_s3 = (var_s3 + 1) & 0xFF;
        if (var_s3 >= temp_s1) {
            goto block_25;
        }
        goto loop_13;
    }
block_25:
    return 0U;
}

void Lb_check_newCommer(void) {
    s32 temp_a1;
    s32 var_s1;
    u8 *var_s0;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a2;

    if (M2C_FIELD(cw, s8 *, 0x2C08) != 0) {
        if (lb_member_outCheck() == 1) {
            temp_a0 = *(u8 *)0x3F3404;
            if (temp_a0 != 0x4C) {
                if (temp_a0 == 0x4D) {
                    goto block_5;
                }
            } else {
block_5:
                flCompact(temp_a0);
                M2C_FIELD(lb_sys, u8 *, 0x8D) = 3U;
            }
            M2C_FIELD(lb_sys, s16 *, 0x64) = 0;
            temp_a2 = *(u8 *)0x3F34C1;
            temp_a1 = temp_a2 * 0xA00;
            temp_a0_2 = *(D_3E4C05 + temp_a1);
            if ((temp_a0_2 != 0x2B) && (temp_a0_2 != 0x29) && (temp_a0_2 != 0x2A) && (temp_a0_2 != 0x4E) && (temp_a0_2 != 0x60) && (temp_a0_2 != 0x5E) && (temp_a0_2 != 0x5D) && (temp_a0_2 != 0x5C) && (temp_a0_2 != 0x5A) && (temp_a0_2 != 0x59) && (temp_a0_2 != 0x54) && (temp_a0_2 != 0x4C)) {

            } else {
                Lb_send_myChair(temp_a0_2, temp_a1, temp_a2);
            }
            return;
        }
        if (M2C_FIELD(lb_sys, u8 *, 0x8D) == 0) {
            if (lb_member_changeCheck() == 1) {
                temp_a0_3 = *(u8 *)0x3F3404;
                if (temp_a0_3 != 0x4C) {
                    if (temp_a0_3 == 0x4D) {
                        goto block_27;
                    }
                } else {
block_27:
                    flCompact(temp_a0_3);
                    M2C_FIELD(lb_sys, u8 *, 0x8D) = 3U;
                }
                return;
            }
            if (M2C_FIELD(lb_sys, u8 *, 0x8D) == 0) {
                lb_member_inCheck();
                temp_a0_4 = *(u8 *)0x3F3404;
                if (temp_a0_4 != 0x4D) {
                    if (temp_a0_4 == 0x4C) {
                        goto block_33;
                    }
                } else {
block_33:
                    var_s1 = 0;
                    var_s0 = player_work;
                    do {
                        if ((*var_s0 != 0) && (M2C_FIELD((cw + (((s8)(var_s1)))), s8 *, 0x2BFE) == 0)) {
                            Lb_player_load(var_s0);
                        }
                        var_s1 = ((s8)((var_s1 + 1)));
                        var_s0 += 0xA00;
                    } while (var_s1 < 8);
                }
            }
        }
    }
}

void lb_basic_master(u8 *arg0) {
    f32 sp120[3];
    f32 sp110[3];
    f32 sp100[3];
    s32 spF0[3];
    f32 spE0[3];
    f32 spD0[3];
    s32 spC0[3];
    f32 sp80[16];
    f32 sp40[16];
    s16 var_v1_2;
    s32 temp_a0;
    s32 temp_a0_7;
    s32 temp_a1_2;
    s32 var_a1;
    s32 var_v1;
    s8 temp_a1;
    u16 temp_a0_6;
    u16 temp_a1_3;
    u16 temp_a1_4;
    u16 temp_s0_2;
    u16 temp_v1_3;
    u32 temp_v1;
    u8 *temp_a0_9;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a0_5;
    u8 temp_a0_8;
    u8 temp_a1_5;
    u8 temp_a1_7;
    u8 temp_a1_8;
    u8 temp_a2;
    u8 temp_a2_2;
    u8 temp_v1_2;
    u8 temp_v1_4;
    u8 temp_v1_5;
    u8 *temp_a1_6;
    u8 *temp_a3;
    u8 *temp_a3_2;
    u8 *temp_s0;
    u8 *temp_s1;
    u8 *temp_v0;

    temp_a0 = Lb_stick_pow_get(arg0) & 0xFF;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        break;
    case 3:                                         /* switch 1 */
    case 2:                                         /* switch 1 */
    case 1:                                         /* switch 1 */
        temp_a0_2 = M2C_FIELD(arg0, u8 *, 0x15);
        if ((temp_a0_2 != 2) && (temp_a0_2 != 0x24) && (temp_a0_2 != 0x1F)) {
            if (M2C_FIELD(arg0, u16 *, 0xC) == *(u8 *)0x3F34C1) {
                M2C_FIELD(arg0, u16 *, 0xE) = Lb_stick_dir_set(arg0, 0);
            }
            Lb_Pl_act_set(arg0, 0, 2, 0);
        }
        break;
    case 5:                                         /* switch 1 */
        temp_a0_3 = M2C_FIELD(arg0, u8 *, 0x15);
        if ((temp_a0_3 != 0x1F) && (temp_a0_3 != 0x3F) && (temp_a0_3 != 0x13) && (temp_a0_3 != 0x24)) {
            if (M2C_FIELD(arg0, u16 *, 0xC) == *(u8 *)0x3F34C1) {
                M2C_FIELD(arg0, u16 *, 0xE) = Lb_stick_dir_set(arg0, 0);
            }
            temp_v1 = ((M2C_FIELD(arg0, u16 *, 0xE) + 0x10000) - M2C_FIELD(arg0, s32 *, 0xA4)) & 0xFFFF;
            if ((temp_v1 >= 0x6000U) && (temp_v1 < 0xA001U)) {
                Lb_Pl_act_set(arg0, 0, 0x3F, 0);
            } else {
                Lb_Pl_act_set(arg0, 0, 0x1F, 0);
            }
        }
        break;
    }
    if (M2C_FIELD(arg0, u8 *, 0x8C4) != 0) {
        Lb_Pl_chat_act_set(arg0);
    }
    if (M2C_FIELD(arg0, u16 *, 0xC) == *(u8 *)0x3F34C1) {
        if ((M2C_FIELD(arg0, u16 *, 0x368) & 0x20) && (M2C_FIELD(lb_sys, s32 *, 0x68) == 0)) {
            if (trade_get_ck_005D0750(arg0) != 1) {
                goto block_30;
            }
        } else {
block_30:
            temp_s1 = M2C_FIELD(arg0, u8 **, 0x3B0);
            if ((temp_s1 != NULL) && (M2C_FIELD(arg0, u16 *, 0x368) & 0x20)) {
                if ((M2C_FIELD(temp_s1, u8 *, 0x1E) == 0) && (M2C_FIELD(lb_sys, s32 *, 0x68) == 0)) {
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    M2C_FIELD(lb_sys, s32 *, 0x68) = 4;
                    Lb_pl_status_i();
                    goto block_119;
                }
                if (M2C_FIELD(lb_sys, s32 *, 0x68) == 0) {
                    temp_s0 = temp_s1 + 0x444;
                    if ((M2C_FIELD(temp_s1, s8 *, 0x471) == 1) || (M2C_FIELD(lb_sys, u8 *, 0x87) != 0)) {
                        return;
                    }
                    M2C_FIELD(temp_s1, u8 **, 0x7A0) = arg0;
                    flMemset(lb_pit, 0, 0xC);
                    if (M2C_FIELD(temp_s1, u8 *, 2) == 0) {
                        temp_a2 = M2C_FIELD(temp_s0, u8 *, 0xE);
                        temp_a1 = *(npc_sound_tbl + temp_a2);
                        if ((temp_a1 != -1) && ((s32) temp_a2 < 0x3B)) {
                            sound_call_005D3640(temp_s1, temp_a1, temp_a2);
                        }
                    }
                    if (M2C_FIELD(temp_s0, u8 *, 0xE) == 0x54) {
                        var_a1 = ((s16)((*(u8 *)0x3F3404 - 0x51)));
                        if (var_a1 >= 0) {
                            if (var_a1 >= 0x56) {
                                goto block_48;
                            }
                        } else {
block_48:
                            var_a1 = 0;
                        }
                        if ((M2C_FIELD(temp_s1, u8 *, 0x15) != 0x8C) && (*(lb_sys + 0x88 + (((s16)(var_a1)))) == 0)) {
                            Lb_act_set(temp_s1, 0, 0x64);
                        }
                    } else {
                        temp_a0_4 = M2C_FIELD(temp_s1, u8 *, 0x15);
                        if ((temp_a0_4 != 0x79) && (temp_a0_4 != 0x7A)) {
                            Lb_act_set(temp_s1, 0, 0x64);
                        }
                    }
                    M2C_FIELD(arg0, s8 *, 0x8EC) = 0;
                    temp_a2_2 = M2C_FIELD(temp_s0, u8 *, 0xE);
                    switch (temp_a2_2) {            /* switch 2; irregular */
                    case 0x55:                      /* switch 2 */
                    case 0x4:                       /* switch 2 */
                    case 0x48:                      /* switch 2 */
                    case 0x4B:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        temp_v1_2 = M2C_FIELD(temp_s1, u8 *, 0x15);
                        if (temp_v1_2 != 0x79) {
                            if (temp_v1_2 == 0x7A) {
                                goto block_79;
                            }
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 5;
                        } else {
block_79:
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 6;
                            Lb_act_set(temp_s1, 0, 0x64);
                        }
                        NPCZoomInCameraRequest(temp_s1);
                        goto block_119;
                    case 0x0:                       /* switch 2 */
                    case 0x47:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 2;
                        if (M2C_FIELD(temp_s0, u8 *, 0xE) == 0) {
                            Lb_act_set(temp_s1, 0, 0x65);
                        }
                        M2C_FIELD(lb_sys, s8 *, 6) = 0;
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x2D:                      /* switch 2 */
                    case 0x1:                       /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 3;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x38:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0x22;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x3:                       /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 9;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x4C:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0xA;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x49:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0xB;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x5:                       /* switch 2 */
                        if ((Event_flag_ck(0x51) == 0) && (Event_flag_ck(0x33) == 1)) {
                            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x2B;
                            NPCZoomInCameraRequest(temp_s1);
                        } else {
                            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0xC;
                            Lb_shop_init();
                            NPCZoomInCameraRequest(temp_s1);
                        }
                        return;
                    case 0x2E:                      /* switch 2 */
                    case 0x2:                       /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0xD;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        return;
                    case 0x3A:                      /* switch 2 */
                    case 0x4A:                      /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0xE;
                        Lb_shop_init();
                        NPCZoomInCameraRequest(temp_s1);
                        goto block_119;
                    case 0x54:                      /* switch 2 */
                        var_v1 = ((s16)((*(u8 *)0x3F3404 - 0x51)));
                        if (var_v1 >= 0) {
                            if (var_v1 >= 0x56) {
                                goto block_109;
                            }
                        } else {
block_109:
                            var_v1 = 0;
                        }
                        if (*(lb_sys + 0x88 + (((s16)(var_v1)))) == 0) {
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x12;
                            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                            NPCZoomInCameraRequest(M2C_FIELD(arg0, u8 **, 0x3B0));
                        }
                        return;
                    default:                        /* switch 2 */
                        M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                        temp_a0_5 = M2C_FIELD(temp_s1, u8 *, 0x15);
                        if ((temp_a0_5 == 0x79) || (temp_a0_5 == 0x7A)) {
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 6;
                            Lb_act_set(temp_s1, 0, 0x64);
                        } else {
                            M2C_FIELD(lb_sys, s32 *, 0x68) = 5;
                        }
                        return;
                    }
                } else {
                    goto block_119;
                }
            } else {
block_119:
                temp_a3 = M2C_FIELD(arg0, u8 **, 0x878);
                if ((temp_a3 != NULL) && ((temp_v1_3 = M2C_FIELD(arg0, u16 *, 0x368), temp_a1_2 = temp_v1_3 & 0x200, (temp_a1_2 != 0)) || (temp_v1_3 & 0x40)) && ((M2C_FIELD(lb_sys, s32 *, 0x68) == 0) || (M2C_FIELD(lb_sys, s32 *, 0x68) == 8))) {
                    if (((temp_a1_2 != 0) || (temp_a1_3 = M2C_FIELD(temp_a3, u16 *, 2), (temp_a1_3 == 0x13)) || (temp_a1_3 == 0x14)) && (M2C_FIELD(lb_sys, u8 *, 0x87) == 0) && ((M2C_FIELD(lb_sys, s32 *, 0x68) != 8) || (M2C_FIELD(temp_a3, u16 *, 2) == 6))) {
                        sp120[0] = M2C_FIELD(temp_a3, f32 *, 4);
                        sp120[1] = M2C_FIELD(M2C_FIELD(arg0, u8 **, 0x878), f32 *, 8);
                        sp120[2] = M2C_FIELD(M2C_FIELD(arg0, u8 **, 0x878), f32 *, 0xC);
                        temp_a3_2 = M2C_FIELD(arg0, u8 **, 0x878);
                        temp_a0_6 = M2C_FIELD(temp_a3_2, u16 *, 2);
                        temp_s0_2 = M2C_FIELD(temp_a3_2, u16 *, 0x14);
                        if (temp_a0_6 < 0x1BU) {
                            switch (temp_a0_6) {    /* switch 3 */
                            case 1:                 /* switch 3 */
                                temp_a0_8 = M2C_FIELD(arg0, u8 *, 0x15);
                                if (temp_a0_8 != 0) {
                                    if (temp_a0_8 == 0x55) {
                                        goto block_136;
                                    }
                                } else {
block_136:
                                    temp_a1_4 = M2C_FIELD(temp_a3_2, u16 *, 0);
                                    if (!(M2C_FIELD(lb_sys, u16 *, 0x64) & (1 << temp_a1_4))) {
                                        if (*(u8 *)0x3F3404 == 0x4D) {
                                            temp_v0 = cw;
                                            M2C_FIELD(lb_sys, u16 *, 0x66) = temp_a1_4;
                                            if (memcmp(temp_v0 + 3, temp_v0 + 0x440, 8) == 0) {
                                                M2C_FIELD(lb_sys, s16 *, 0x74) = 0x96;
                                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x18;
                                                M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                                Lb_send_data_to_myself(2, 4, M2C_FIELD(arg0, u8 **, 0x878));
                                            } else {
                                                M2C_FIELD(lb_sys, s16 *, 0x74) = 0x96;
                                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x18;
                                                M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                                Lb_send_chair_req(arg0);
                                            }
                                        } else {
                                            M2C_FIELD(lb_sys, u16 *, 0x66) = temp_a1_4;
                                            Lb_pl_to_chair();
                                        }
                                    }
                                }
                                return;
                            case 26:                /* switch 3 */
                                if (M2C_FIELD(cw, u8 *, 0x35D6) == 0) {
                                    M2C_FIELD(lb_sys, u16 *, 0x66) = (u16) M2C_FIELD(temp_a3_2, u16 *, 0);
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x11;
                                    Lb_pl_to_chair();
                                }
                                return;
                            case 5:                 /* switch 3 */
                                temp_v1_4 = *(u8 *)0x3F3404;
                                switch (temp_v1_4) { /* switch 4 */
                                case 0x4D:          /* switch 4 */
                                    if (M2C_FIELD(cw, u8 *, 0x35D3) == 1) {
                                        Lb_put_set01(1);
                                    } else {
                                        M2C_FIELD(arg0, s16 *, 0x73A) = 0x4C;
                                        M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                        M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                    }
                                    return;
                                case 0x4E:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x4C;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 1;
                                default:            /* switch 4 */
                                    return;
                                case 0x4F:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x4C;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 3;
                                    break;
                                case 0x50:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x4C;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 4;
                                    break;
                                case 0x51:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x50;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 1;
                                    break;
                                case 0x52:          /* switch 4 */
                                case 0x53:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x50;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 2;
                                    break;
                                case 0x54:          /* switch 4 */
                                case 0x55:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x50;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 3;
                                    break;
                                case 0x56:          /* switch 4 */
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x57;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                    break;
                                }
                                break;
                            case 6:                 /* switch 3 */
                                temp_a1_5 = *(u8 *)0x3F3404;
                                switch (temp_a1_5) { /* switch 5; irregular */
                                case 0x4D:          /* switch 5 */
                                    temp_a1_6 = cw;
                                    if (M2C_FIELD(temp_a1_6, u8 *, 0x35D3) != 0) {
                                        temp_a1_7 = M2C_FIELD(temp_a1_6, u8 *, 0x32C5);
                                        if (temp_a1_7 == 1) {
                                            if ((M2C_FIELD(lb_sys, s32 *, 0x68) != 7) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 8)) {
                                                if ((M2C_FIELD(cw, u16 *, 0x32C6) + 1) >= (s32) M2C_FIELD(Lbs_GetRoomInfo(((s16)((M2C_FIELD(ClassInfo, u8 *, 8) - 1))), temp_a1_7), u16 *, 2)) {
                                                    SetDialogData(0x36, 2);
                                                    SetDialogYesNo(0);
                                                    *(s8 *)0x3F36AB = 0;
                                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x30;
                                                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                                } else {
                                                    SetDialogData(0x34, 2);
                                                    SetDialogYesNo(1);
                                                    *(u8 *)0x3F36AB = 0;
                                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x1D;
                                                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                                }
                                            }
                                        } else if (M2C_FIELD(lb_sys, s32 *, 0x68) == 8) {
                                            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x28;
                                            M2C_FIELD(lb_sys, s8 *, 6) = 0;
                                            *(u8 *)0x3F36AB = 0;
                                            SetDialogData(0x35, 2);
                                            SetDialogYesNo(1);
                                        } else {
                                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x1A;
                                            temp_a1_8 = *(u8 *)0x3F34C1;
                                            temp_a0_9 = D_3E5506 + (temp_a1_8 * 0xA00);
                                            *temp_a0_9 |= 0x20;
                                            *temp_a0_9 &= ~0x10;
                                            Lbc_SendMiniData(temp_a0_9, temp_a1_8);
                                            Lb_put_chat(0xD);
                                        }
                                    } else {
                                        cnWrap_SoundRequest(7, temp_a1_6);
                                    }
                                    return;
                                case 0x57:          /* switch 5 */
                                    if (M2C_FIELD(cw, u8 *, 0x35D3) != 0) {
                                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0x1E;
                                        fade_set(1, temp_a1_5);
                                    }
                                    return;
                                case 0x4E:          /* switch 5 */
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x4C;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 2;
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    break;
                                }
                                break;
                            case 18:                /* switch 3 */
                                if (Event_flag_ck(4) == 1) {
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    M2C_FIELD(arg0, s16 *, 0x73A) = 0x51;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                    temp_v1_5 = *(u8 *)0x3C73A8;
                                    if ((s32) temp_v1_5 < 0x96) {
                                        *(u8 *)0x3C73A8 = (u8) (temp_v1_5 + 1);
                                    }
                                }
                                return;
                            case 19:                /* switch 3 */
                                if (Event_flag_ck(4) == 1) {
                                    if (M2C_FIELD(arg0, u16 *, 0x368) & 0x200) {
                                        lb_goto_guest_room(arg0, 0x52);
                                    } else {
                                        lb_goto_guest_room(arg0, 0x53);
                                    }
                                }
                                return;
                            case 20:                /* switch 3 */
                                if (Event_flag_ck(4) == 1) {
                                    if (M2C_FIELD(arg0, u16 *, 0x368) & 0x200) {
                                        lb_goto_guest_room(arg0, 0x54);
                                    } else {
                                        lb_goto_guest_room(arg0, 0x55);
                                    }
                                }
                                return;
                            case 7:                 /* switch 3 */
                                M2C_FIELD(lb_sys, s8 *, 0x73) = 0;
                                M2C_FIELD(lb_pit, s8 *, 8) = 0;
                                M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 1;
                                return;
                            case 8:                 /* switch 3 */
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                M2C_FIELD(arg0, s16 *, 0x73A) = 0x4D;
                                M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                goto block_233;
                            case 11:                /* switch 3 */
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                M2C_FIELD(arg0, s16 *, 0x73A) = 0x4F;
                                M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                goto block_233;
                            case 12:                /* switch 3 */
                                if ((Online_ck(temp_a0_7) == 0) && (M2C_FIELD(cw, u8 *, 0x35D3) == 1)) {
                                    Lb_put_set01(1);
                                } else {
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                    var_v1_2 = 0x50;
                                    if (*(u8 *)0x3F3404 == 0x4C) {

                                    } else {
                                        var_v1_2 = 0x56;
                                    }
                                    M2C_FIELD(arg0, s16 *, 0x73A) = var_v1_2;
                                    M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                    M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                }
                                goto block_233;
                            case 9:                 /* switch 3 */
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                M2C_FIELD(arg0, s16 *, 0x73A) = 0x4E;
                                M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                M2C_FIELD(lb_sys, s8 *, 0x71) = 0;
                                goto block_233;
                            case 10:                /* switch 3 */
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x14;
                                M2C_FIELD(arg0, s16 *, 0x73A) = 0x4E;
                                M2C_FIELD(lb_sys, s8 *, 3) = 5;
                                M2C_FIELD(lb_sys, s8 *, 0x71) = 1;
                                goto block_233;
                            case 13:                /* switch 3 */
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x10;
                                fade_set(0xA);
                                goto block_233;
                            case 14:                /* switch 3 */
                                flvecCopy(arg0 + 0x800, sp120);
                                spF0[1] = temp_s0_2 & 0xFFFF;
                                spF0[0] = 0;
                                spF0[2] = 0;
                                cpRotMatrix(spF0, sp80);
                                SetVector(sp110, 0.0f, 0.0f, 0.0f);
                                flvecApplyMat33(sp100, sp110, sp80);
                                M2C_FIELD(arg0, f32 *, 0x800) = M2C_FIELD(arg0, f32 *, 0x800) + sp100[0];
                                M2C_FIELD(arg0, f32 *, 0x804) = M2C_FIELD(arg0, f32 *, 0x804) + sp100[1];
                                M2C_FIELD(arg0, f32 *, 0x808) = M2C_FIELD(arg0, f32 *, 0x808) + sp100[2];
                                M2C_FIELD(arg0, u16 *, 0xE) = temp_s0_2;
                                Lb_Pl_adj_calc(arg0, 0x14);
                                Lb_Pl_act_set(arg0, 0, 0x33, 0);
                                pl_flag_set(arg0, 0x20000);
                                *(u8 *)0x3F36AB = 0;
                                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x13;
                                goto block_233;
                            case 23:                /* switch 3 */
                                if (Event_flag_ck(1) == 0) {
                                    flvecCopy(arg0 + 0x800, sp120);
                                    spC0[1] = temp_s0_2 & 0xFFFF;
                                    spC0[0] = 0;
                                    spC0[2] = 0;
                                    cpRotMatrix(spC0, sp40);
                                    SetVector(spE0, 0.0f, 0.0f, 0.0f);
                                    flvecApplyMat33(spD0, spE0, sp40);
                                    M2C_FIELD(arg0, f32 *, 0x800) = M2C_FIELD(arg0, f32 *, 0x800) + spD0[0];
                                    M2C_FIELD(arg0, f32 *, 0x804) = M2C_FIELD(arg0, f32 *, 0x804) + spD0[1];
                                    M2C_FIELD(arg0, f32 *, 0x808) = M2C_FIELD(arg0, f32 *, 0x808) + spD0[2];
                                    M2C_FIELD(arg0, u16 *, 0xE) = temp_s0_2;
                                    Lb_Pl_adj_calc(arg0, 0x14);
                                    if (Event_flag_ck(3) == 1) {
                                        if ((Warehouse_search_space(User_data) & 0xFF) == 0xFF) {
                                            Lb_put_set01(8);
                                        } else {
                                            LegendSwordCameraRequest(8);
                                            Lb_Pl_act_set(arg0, 0, 0x52, 0);
                                            pl_flag_set(arg0, 0x20000);
                                            Event_flag_set(1);
                                            Warehouse_equip_stack(User_data, 6, 0xBE, 0);
                                            Lb_reset_talk_count();
                                            M2C_FIELD(lb_sys, s32 *, 0x68) = 0x26;
                                        }
                                    } else {
                                        Lb_Pl_act_set(arg0, 0, 0x51, 0);
                                        pl_flag_set(arg0, 0x20000);
                                        M2C_FIELD(lb_sys, s32 *, 0x68) = 0x26;
                                    }
                                }
                                goto block_233;
                            case 15:                /* switch 3 */
                                if (Lb_ItemBox_open(temp_a0_7) == 1) {
                                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x1B;
                                    Lbc_set_prim(0, 0, 0);
                                } else {
                                    cnWrap_SoundRequest(7);
                                }
                                goto block_233;
                            case 22:                /* switch 3 */
                                if (M2C_FIELD(lb_sys, u8 *, 0x87) == 0) {
                                    M2C_FIELD(lb_sys, s8 *, 6) = 0;
                                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
                                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x27;
                                }
                                goto block_233;
                            }
                        }
                    }
                } else {
block_233:
                    if (M2C_FIELD(arg0, u8 *, 0x908) != 0) {
                        M2C_FIELD(arg0, s8 *, 0x90B) = 1;
                        Lb_Pl_act_set2(arg0, 0, 0x2D, 0x10);
                        Lb_send_trade_start(arg0);
                    }
                }
            }
        }
    }
}

void lb_pl_turn_sub(u8 *arg0) {
    s32 temp_a2;
    s32 var_a1;
    s32 var_t1;
    s32 var_v1;
    u16 temp_a0;
    u16 temp_a0_2;
    u16 temp_a0_3;
    u16 temp_a3;
    u16 var_v1_2;
    u32 temp_t0;

    if (((((s16)(Lb_act_ck(arg0, 0, 1)))) != 0) || ((((s16)(Lb_act_ck(arg0, 0, 0x24)))) != 0)) {
        var_t1 = 0x71C;
    } else {
        var_t1 = 0x5B0;
        if ((((s16)(Lb_act_ck(arg0, 0, 0x1F)))) != 0) {

        } else {
            var_t1 = 0;
            if ((((s16)(Lb_act_ck(arg0, 0, 0x3F)))) != 0) {

            } else {
                var_t1 = 0xFA4;
            }
        }
    }
    temp_a2 = M2C_FIELD(arg0, s32 *, 0xA4);
    temp_a3 = M2C_FIELD(arg0, u16 *, 0xE);
    temp_a0 = M2C_FIELD(arg0, u16 *, 0x2DC);
    temp_t0 = (temp_a3 - (temp_a2 & 0xFFFF)) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 39:
        var_t1 = 0x71C;
        var_a1 = 0x80;
        break;
    case 3:
        var_a1 = 0x80;
        break;
    case 4:
        var_a1 = 0xA0;
        break;
    default:
        var_a1 = 0;
        break;
    }
    if ((u32) ((temp_t0 + var_t1) & 0xFFFF) < (u32) (var_t1 * 2)) {
        M2C_FIELD(arg0, s32 *, 0xA4) = (s32) temp_a3;
        M2C_FIELD(arg0, s8 *, 0x2F8) = 0;
        temp_a0_2 = M2C_FIELD(arg0, u16 *, 0x750);
        var_v1 = temp_a0_2 + 0x300;
        if (var_v1 < 0x601) {
            M2C_FIELD(arg0, u16 *, 0x750) = 0U;
        } else {
            if ((s32) temp_a0_2 < 0x8000) {
                var_v1 = temp_a0_2 - 0x300;
            }
            goto block_29;
        }
    } else {
        if (temp_t0 < 0x8000U) {
            M2C_FIELD(arg0, s32 *, 0xA4) = (s32) ((temp_a2 + var_t1) & 0xFFFF);
            var_v1 = M2C_FIELD(arg0, u16 *, 0x750) - var_a1;
        } else {
            M2C_FIELD(arg0, s32 *, 0xA4) = (s32) ((temp_a2 - var_t1) & 0xFFFF);
            var_v1 = M2C_FIELD(arg0, u16 *, 0x750) + var_a1;
        }
block_29:
        M2C_FIELD(arg0, u16 *, 0x750) = (u16) var_v1;
    }
    temp_a0_3 = M2C_FIELD(arg0, u16 *, 0x750);
    if (((s32) temp_a0_3 < 0xF601) && ((s32) temp_a0_3 >= 0xA00)) {
        var_v1_2 = 0xF600;
        if ((s32) temp_a0_3 >= 0x8000) {

        } else {
            var_v1_2 = 0xA00;
        }
        M2C_FIELD(arg0, u16 *, 0x750) = var_v1_2;
    }
}

/* Lb_St_unique_adr_set (0x5CE8B0): the stage's "unique" spots
 * (Stage_unique_data_get: 0x18-byte records {u16 ?, u16 kind, f32 x, y, z,
 * radius, u16 angle}, x = -1 ends the list) the player stands in: +0x878 and
 * the action hint for the master player */
void Lb_St_unique_adr_set(u8 *pl) {
    u8 *r;
    f32 y, dx, dz;

    M2C_FIELD(pl, u8 **, 0x878) = NULL;
    r = (u8 *)Stage_unique_data_get(M2C_FIELD(pl, u8 *, 0x736));
    if (r == NULL) {
        Lb_put_unique_act_hint(pl, -1);
        M2C_FIELD(pl, u8 **, 0x878) = NULL;
        return;
    }
    for (;; r += 0x18) {
        if (M2C_FIELD(r, f32 *, 4) == -1.0f) {
            Lb_put_unique_act_hint(pl, -1);
            M2C_FIELD(pl, u8 **, 0x878) = NULL;
            return;
        }
        y = M2C_FIELD(pl, f32 *, 0xB0);
        if (y < M2C_FIELD(r, f32 *, 8) - 50.0f || !(y < 50.0f + M2C_FIELD(r, f32 *, 8))) {
            continue;
        }
        dx = M2C_FIELD(pl, f32 *, 0xAC) - M2C_FIELD(r, f32 *, 4);
        dz = M2C_FIELD(pl, f32 *, 0xB4) - M2C_FIELD(r, f32 *, 0xC);
        if (!(flSqrt(dx * dx + dz * dz) <= M2C_FIELD(r, f32 *, 0x10))) {
            continue;
        }
        if (lb_ck_unique_act(pl, r) == 1) {
            M2C_FIELD(pl, u8 **, 0x878) = r;
            if (M2C_FIELD(pl, u16 *, 0xC) == *(u8 *)0x3F34C1) {
                Lb_put_unique_act_hint(pl, M2C_FIELD(r, u16 *, 2));
            }
        } else {
            Lb_put_unique_act_hint(pl, -1);
        }
        return;
    }
}

void lb_pl_mv052(u8 *arg0) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1_2;
    s32 temp_v1_3;
    u8 temp_v1;

    Get_sw2(0);
    if (M2C_FIELD(arg0, u16 *, 0xC) != *(u8 *)0x3F34C1) {
        Lb_act_set(arg0, 0, 0);
        return;
    }
    if ((M2C_FIELD(lb_sys, s32 *, 0x68) != 0x25) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0x24) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0x23)) {
        temp_v1 = M2C_FIELD(arg0, u8 *, 5);
        switch (temp_v1) {                          /* switch 1 */
        case 0:                                     /* switch 1 */
            M2C_FIELD(cw, s8 *, 0x2C08) = 0;
            M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
            Lb_Pl_basic_flagset(arg0, 0, 0, 0);
            M2C_FIELD(arg0, s32 *, 0x39C) = 0;
            Lb_pl_chr_set(arg0, 0x1AB, 0, 0);
            if (Online_ck() == 0) {
                McOperationSet(5);
                SetDialogData(0x30, 2);
                SetDialogYesNo(0);
                return;
            }
            McOperationSet(6);
            M2C_FIELD(arg0, u8 *, 5) = 2U;
            M2C_FIELD(arg0, s32 *, 8) = 0x3C;
            str_fadeout(0, M2C_FIELD(arg0, s32 *, 8));
            return;
        case 1:                                     /* switch 1 */
            M2C_FIELD(pNet, s8 *, 0xC) = 1;
            temp_v0 = Lb_select();
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
                M2C_FIELD(arg0, s32 *, 8) = 0x41;
                str_fadeout(0, M2C_FIELD(arg0, s32 *, 8));
                break;
            case 3:                                 /* switch 2 */
                SetDialogData(0x37, 2);
                SetDialogYesNo(1);
                M2C_FIELD(arg0, u8 *, 5) = 4U;
                break;
            }
            pl_sleeping(arg0);
            return;
        case 2:                                     /* switch 1 */
            temp_v0_2 = M2C_FIELD(arg0, s32 *, 8);
            if (temp_v0_2 == 0) {
                temp_v1_2 = McCardOperation() & 0xFF;
                switch (temp_v1_2) {                /* switch 3; irregular */
                case 1:                             /* switch 3 */
                    if (Online_ck() == 1) {
                        lb_exit_save(arg0);
                    } else {
                        M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
                        M2C_FIELD(cw, s8 *, 0x2C08) = 1;
                        SetDialogData(0x37, 2);
                        SetDialogYesNo(1);
                    }
                    break;
                case 2:                             /* switch 3 */
                    if (Online_ck() == 1) {
                        lb_exit_save(arg0);
                    } else {
                        SetDialogData(0x37, 2);
                        SetDialogYesNo(1);
                        M2C_FIELD(arg0, u8 *, 5) = 4U;
                    }
                    break;
                }
            } else {
                temp_v1_3 = temp_v0_2 - 1;
                M2C_FIELD(arg0, s32 *, 8) = temp_v1_3;
                if (temp_v1_3 == 5) {
                    str_stop(1);
                    str_pause(0, 1);
                    cnWrap_SoundRequest(0xB);
                }
            }
            pl_sleeping(arg0);
            return;
        case 3:                                     /* switch 1 */
            M2C_FIELD(pNet, s8 *, 0xC) = 1;
            temp_v0_3 = Lb_select();
            switch (temp_v0_3) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                if (Online_ck() == 1) {
                    Lbs_LogOutRequest(1);
                } else {
                    M2C_FIELD(lb_sys, s32 *, 0x68) = 0x23;
                    fade_set(1);
                }
            default:                                /* switch 4 */
                pl_sleeping(arg0);
                return;
            case 3:                                 /* switch 4 */
                lb_exit_save(arg0);
                return;
            }
            break;
        case 4:                                     /* switch 1 */
            M2C_FIELD(pNet, s8 *, 0xC) = 1;
            temp_v0_4 = Lb_select();
            switch (temp_v0_4) {                    /* switch 5; irregular */
            case 0:                                 /* switch 5 */
                if (Online_ck() == 1) {
                    Lbs_LogOutRequest();
                } else {
                    M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
                    SetDialogData(0x39, 4);
                }
            default:                                /* switch 5 */
                pl_sleeping(arg0);
                return;
            case 3:                                 /* switch 5 */
                lb_exit_save(arg0);
                return;
            }
            break;
        case 5:                                     /* switch 1 */
            M2C_FIELD(pNet, s8 *, 0xC) = 1;
            temp_v0_5 = Lb_select();
            switch (temp_v0_5) {                    /* switch 6; irregular */
            case 0:                                 /* switch 6 */
                M2C_FIELD(arg0, u8 *, 5) = 2U;
                M2C_FIELD(arg0, s32 *, 8) = 0x3C;
                str_fadeout(0, M2C_FIELD(arg0, s32 *, 8));
                if (Online_ck() == 0) {
                    McOperationSet(5);
                    return;
                }
                McOperationSet(6);
                return;
            case 3:                                 /* switch 6 */
                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x23;
                fade_set(1);
            default:                                /* switch 1 */
                return;
            }
            break;
        }
    } else {
        pl_sleeping(arg0);
    }
}

void lb_pl_mv088(u8 *arg0) {
    s16 temp_a0_2;
    s32 temp_s0;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 temp_a0_4;

    temp_a0 = M2C_FIELD(arg0, u8 *, 5);
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        M2C_FIELD(arg0, u8 *, 5) = (u8) (temp_a0 + 1);
        M2C_FIELD(arg0, s16 *, 0x8CA) = 0;
        Lb_pl_chr_set(arg0, 0x25B, 6, 0);
        adx_se_set(arg0, 0xA);
        str_fadein_vol(0, 0xF, 0x5FU);
        return;
    case 1:                                         /* switch 1 */
        M2C_FIELD(arg0, s16 *, 0x8CA) = (s16) (M2C_FIELD(arg0, s16 *, 0x8CA) + 1);
        ang_894 = Lb_get_angle(arg0, M2C_FIELD(arg0, s32 *, 0x3B0) + 0xAC);
        M2C_FIELD(arg0, u16 *, 0xE) = (u16) (M2C_FIELD(arg0, u16 *, 0xE) + (((ang_894 / 5) + ((u32) ang_894 >> 0x1F)) & 0xFFFF));
        temp_a0_2 = M2C_FIELD(arg0, s16 *, 0x8CA);
        if (temp_a0_2 >= 0x118) {
            M2C_FIELD(arg0, u8 *, 0x8D0) = 3U;
        } else if (temp_a0_2 >= 0x10E) {
            if (temp_a0_2 == 0x10E) {
                Eft25_set(arg0, 8);
            }
            M2C_FIELD(arg0, u8 *, 0x8D0) = 2U;
        } else if (temp_a0_2 >= 0xB4) {
            M2C_FIELD(arg0, u8 *, 0x8D0) = 1U;
        } else {
            M2C_FIELD(arg0, u8 *, 0x8D0) = 0U;
        }
        if ((temp_s0 & 0xFFFF & 0x20) || (M2C_FIELD(arg0, s16 *, 0x8CA) >= 0x12C)) {
            temp_a0_3 = M2C_FIELD(arg0, u8 *, 0x8D0);
            switch (temp_a0_3) {                    /* switch 2; irregular */
            case 1:                                 /* switch 2 */
            case 0:                                 /* switch 2 */
                Lb_act_set(M2C_FIELD(arg0, s32 *, 0x3B0), 0, 0x8F);
                M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
                Lb_pl_chr_set(arg0, 0x26B, 4, 0);
                adx_se_set(arg0, 0xB);
                str_fadein_vol(0, 0xF, *(D_32D471 + (*(u8 *)0x3F3404 * 2)));
                return;
            case 2:                                 /* switch 2 */
                Lb_act_set(M2C_FIELD(arg0, s32 *, 0x3B0), 0, 0x89);
                M2C_FIELD(arg0, u8 *, 5) = (u8) (M2C_FIELD(arg0, u8 *, 5) + 1);
                Lb_pl_chr_set(arg0, 0x26B, 4, 0);
                adx_se_set(arg0, 4);
                str_fadein_vol(0, 0xF, *(D_32D471 + (*(u8 *)0x3F3404 * 2)));
                return;
            case 3:                                 /* switch 2 */
                M2C_FIELD(arg0, u8 *, 5) = 3U;
                Lb_act_set(M2C_FIELD(arg0, s32 *, 0x3B0), 0, 0x88);
                adx_se_set(arg0, 0xD);
                str_fadein_vol(0, 0xF, *(D_32D471 + (*(u8 *)0x3F3404 * 2)));
                return;
            }
        } else {
        case 3:                                     /* switch 1 */
            return;
        }
        break;
    case 2:                                         /* switch 1 */
        if (M2C_FIELD(arg0, s32 *, 0x194) <= 0) {
            Lb_pl_to_normal(arg0, 0, 8, 0);
            temp_a0_4 = M2C_FIELD(arg0, u8 *, 0x8D0);
            if (temp_a0_4 != 2) {
                NPCZoomInCameraCancel(temp_a0_4);
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                M2C_FIELD(lb_sys, s32 *, 0x68) = 0;
            }
        }
        break;
    }
}

/* Lb_npc_mk (0x5C4660): the NPC's world matrix (scale, YXZ rotation,
 * position) and its head turned towards the master player (joint 0xC for
 * cats/pigs (+2 != 0) up to 10 degrees, joint 0x13 for people up to 20),
 * then the skeleton (flCalcTransSI) */
void Lb_npc_mk(u8 *em) {
    f32 scl[16], m[16];
    f32 lim, a, cur, n;
    u8 *w, *mdl;
    void *jm;
    u16 c;

    w = em + 0x444;
    mdl = M2C_FIELD(em, u8 **, 0x50C);
    flmatMakeScale(scl, M2C_FIELD(em, f32 *, 0xB8), M2C_FIELD(em, f32 *, 0xBC), M2C_FIELD(em, f32 *, 0xC0));
    cpRotMatrixYXZ2(em + 0xA0, m);
    flmatSetTrans(m, M2C_FIELD(em, f32 *, 0xAC), M2C_FIELD(em, f32 *, 0xB0), M2C_FIELD(em, f32 *, 0xB4));
    flmatMul33_2(m, scl);
    flmatCopy(em + 0x60, m);
    if (mdl == NULL) {
        return;     /* PC: no NPC model work yet (npc_create_model) */
    }
    c = M2C_FIELD(em, u16 *, 0x2DC);
    if (c != 0x284 && c != 0x286 && M2C_FIELD(w, u8 *, 0xE) != 5) {
        if (M2C_FIELD(em, u8 *, 2) != 0) {
            jm = (void *)get_joint_mat_em(em, 0xC);
            lim = 10.0f;
        } else {
            jm = (void *)get_joint_mat_em(em, 0x13);
            lim = 20.0f;
        }
        a = (180.0f * flConvertStoR(Lb_get_angle(em, player_work + (*(u8 *)0x3F34C1 * 0xA00) + 0xAC) & 0xFFFF)) / 3.1415927f;
        if (!(a <= 180.0f)) {
            a = -(a - 180.0f);
        }
        if (!(a <= -90.0f) || a < 90.0f) {
            if (a < -lim) {
                a = -lim;
            } else if (!(a <= lim)) {
                a = lim;
            }
            cur = M2C_FIELD(w, f32 *, 4);
            n = cur + 0.2f * (a - cur);
            if (jm != NULL) {   /* PC: no NPC joints yet */
                flmatRotY33(jm, (3.1415927f * n) / 180.0f);
            }
            M2C_FIELD(w, f32 *, 4) = n;
        }
    }
    flCalcTransSI(M2C_FIELD(mdl, s32 *, 0x24), m);
}

/* Lb_npc_mv (0x...): NPC step +4: 0 init, 1 move, 2 die, 3 erase; the draw
 * prim (lb_npc_trans) is queued while it is on this stage */
s32 Lb_npc_mv(u8 *em) {
    switch (M2C_FIELD(em, u8 *, 4)) {
    case 0:
        lb_npc_init(em);
        lb_npc_effect_move(em);
        Lb_World_calc(em);
        return 0;
    case 1:
        lb_npc_move(em);
        break;
    case 2:
        lb_npc_die(em);
        break;
    case 3:
        lb_npc_erase(em);
        return 1;
    }
    lb_npc_effect_move(em);
    if ((M2C_FIELD(em, u8 *, 1) != 0) && (M2C_FIELD(em, u8 *, 0x736) == *(u8 *)0x3F3404)) {
        M2C_FIELD(M2C_FIELD(em, u8 **, 0x564), f32 *, 8) = M2C_FIELD(em, f32 *, 0xAC);
        M2C_FIELD(M2C_FIELD(em, u8 **, 0x564), f32 *, 0xC) = M2C_FIELD(em, f32 *, 0xB0);
        M2C_FIELD(M2C_FIELD(em, u8 **, 0x564), f32 *, 0x10) = M2C_FIELD(em, f32 *, 0xB4);
        add_prim(ot1, M2C_FIELD(em, u8 **, 0x564), 0x20, 0);
    }
    Lb_World_calc(em);
    return 0;
}

void lb_npc_init(u8 *arg0) {
    int var_a1;
    int var_a1_2;
    int var_a1_3;
    s16 temp_v1_2;
    s32 temp_a1;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0;
    s32 var_v0_2;
    u8 *var_a0_2;  /* was u16 *var_a0_2 */
    u16 var_v0;
    u8 temp_v0;
    u8 *temp_s0;
    u8 *temp_v0_2;
    u8 *temp_v0_3;
    u8 *temp_v1;

    temp_s0 = arg0 + 0x444;
    lb_set_npc(arg0);
    em_work_set(arg0);
    M2C_FIELD(arg0, u8 *, 4) = (u8) (M2C_FIELD(arg0, u8 *, 4) + 1);
    M2C_FIELD(arg0, s8 *, 0x10) = 0;
    M2C_FIELD(arg0, s8 *, 0x1E) = 1;
    M2C_FIELD(arg0, s8 *, 0x412) = 0;
    M2C_FIELD(arg0, s8 *, 1) = 1;
    M2C_FIELD(arg0, s32 *, 0x1A0) = 0x40000000;
    M2C_FIELD(arg0, s32 *, 0x1F0) = 0x40000000;
    M2C_FIELD(arg0, s32 *, 0x240) = 0x40000000;
    M2C_FIELD(arg0, s32 *, 0x290) = 0x40000000;
    M2C_FIELD(arg0, s32 *, 0x39C) = 0;
    M2C_FIELD(arg0, u8 *, 0x736) = (u8) *(u8 *)0x3F3404;
    if (M2C_FIELD(arg0, u8 *, 2) == 0) {
        M2C_FIELD(arg0, s16 *, 0x300) = 2;
    } else {
        M2C_FIELD(arg0, s8 *, 0x10) = 1;
        M2C_FIELD(arg0, s16 *, 0x300) = 1;
    }
    M2C_FIELD(arg0, s32 *, 0x798) = 0x3F800000;
    var_a0 = 0;
    M2C_FIELD(arg0, s8 *, 0x7D6) = 0;
    M2C_FIELD(arg0, s8 *, 0x4D4) = 1;
    M2C_FIELD(arg0, s16 *, 0x2DC) = 0;
    M2C_FIELD(arg0, s16 *, 0x2DE) = 0;
    M2C_FIELD(arg0, s16 *, 0x2E0) = 0;
    M2C_FIELD(arg0, s16 *, 0x2E2) = 0;
    do {
        temp_v1 = arg0 + var_a0;
        M2C_FIELD(temp_v1, s8 *, 0x4E6) = 0;
        var_a0 += 8;
        M2C_FIELD(temp_v1, s8 *, 0x4E7) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4E8) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4E9) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4EA) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4EB) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4EC) = 0;
        M2C_FIELD(temp_v1, s8 *, 0x4ED) = 0;
    } while (var_a0 < 0x20);
    temp_v0 = M2C_FIELD(arg0, u8 *, 2);
    switch (temp_v0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_a1 = M2C_FIELD(arg0, u8 *, 0x1B) * 4;
        M2C_FIELD((*(s16 *)(npc_disp_parts_00647910 + temp_a1) + arg0), s8 *, 0x4E6) = 1;
        temp_v1_2 = *(s16 *)(npc_disp_parts_00647910 + 2 + (M2C_FIELD(arg0, u8 *, 0x1B) * 4));
        if (temp_v1_2 != 0xFF) {
            M2C_FIELD((temp_v1_2 + arg0), s8 *, 0x4E6) = 1;
        }
        set_se_type(arg0);
        break;
    case 1:                                         /* switch 1 */
        var_a0_2 = *(u8 * *)(lb_npc_jijii_tbl + (M2C_FIELD(arg0, u8 *, 0x1B) * 4));
        var_v0 = (*(u16 *)var_a0_2);
        if (var_v0 != 0xFFFF) {
            do {
                var_a0_2 += 2;
                M2C_FIELD((arg0 + (var_v0 & 0xFFFF)), s8 *, 0x4E6) = 1;
                var_v0 = (*(u16 *)var_a0_2);
            } while (var_v0 != 0xFFFF);
        }
        break;
    }
    lb_npc_init_sub(arg0);
    if (M2C_FIELD(arg0, u8 *, 2) == 0) {
        temp_v0_2 = M2C_FIELD(temp_s0, u8 **, 8);
        if (temp_v0_2 == NULL) {
            if (M2C_FIELD(temp_s0, u8 *, 0xE) != 4) {
                var_a1 = 1;
            } else {
                var_a1 = 0x2A8;
            }
            Lb_pl_chr_set(arg0, var_a1, 0, 0);
        } else {
            temp_v1_3 = M2C_FIELD(temp_v0_2, s32 *, 0xC);
            switch (temp_v1_3) {                    /* switch 2; irregular */
            default:                                /* switch 2 */
                var_a1_2 = 1;
                break;
            case 0x77:                              /* switch 2 */
                var_a1_2 = 0x2A4;
                break;
            case 0x79:                              /* switch 2 */
                var_a1_2 = 0x291;
                break;
            case 0x6A:                              /* switch 2 */
                var_a1_2 = 0x29E;
                break;
            case 0x6B:                              /* switch 2 */
                var_a1_2 = 0x2A0;
                break;
            case 0x6D:                              /* switch 2 */
                var_a1_2 = 0x29C;
                break;
            case 0x7D:                              /* switch 2 */
            case 0x7C:                              /* switch 2 */
                var_a1_2 = 0x295;
                break;
            case 0x7A:                              /* switch 2 */
                var_a1_2 = 0x290;
                break;
            case 0x6F:                              /* switch 2 */
                var_a1_2 = 0x296;
                break;
            case 0x70:                              /* switch 2 */
                var_a1_2 = 0x298;
                break;
            case 0x73:                              /* switch 2 */
                var_a1_2 = 0x261;
                break;
            case 0x7E:                              /* switch 2 */
                var_a1_2 = 0x2A6;
                break;
            case 0x7F:                              /* switch 2 */
                var_a1_2 = 0x27E;
                break;
            case 0x80:                              /* switch 2 */
                var_a1_2 = 0x285;
                break;
            case 0x81:                              /* switch 2 */
                var_a1_2 = 0x284;
                break;
            }
            Lb_pl_chr_set(arg0, var_a1_2, 0, 0);
        }
        frame_init(arg0, M2C_FIELD(arg0, u16 *, 0x2E4), M2C_FIELD(arg0, s16 *, 0x2EC), 0);
        frame_init(arg0, M2C_FIELD(arg0, u16 *, 0x2E6), M2C_FIELD(arg0, s16 *, 0x2EE), 1);
    } else {
        temp_v0_3 = M2C_FIELD(temp_s0, u8 **, 8);
        if (temp_v0_3 == NULL) {
            if (M2C_FIELD(temp_s0, u8 *, 0xE) != 0x48) {
                var_a1_3 = 0x3E9;
            } else {
                var_a1_3 = 0x3FF;
            }
        } else {
            temp_v1_4 = M2C_FIELD(temp_v0_3, s32 *, 0xC);
            switch (temp_v1_4) {                    /* switch 3; irregular */
            case 0x82:                              /* switch 3 */
                var_a1_3 = 0x426;
                break;
            case 0x86:                              /* switch 3 */
                var_a1_3 = 0x432;
                break;
            case 0x8B:                              /* switch 3 */
                var_v0_2 = ((s16)((*(u8 *)0x3F3404 - 0x51)));
                if ((var_v0_2 < 0) || (var_v0_2 >= 0x56)) {
                    var_v0_2 = 0;
                }
                if (*(lb_sys + 0x88 + (((s16)(var_v0_2)))) == 2) {
                    Lb_act_set(arg0, 0, 0x8C);
                    var_a1_3 = 0x3E9;
                } else {
                    var_a1_3 = 0x3E9;
                }
                break;
            default:                                /* switch 3 */
                var_a1_3 = 0x3E9;
                break;
            }
        }
        Lb_pl_chr_set0(arg0, var_a1_3, 0, 0, 0);
        frame_init(arg0, M2C_FIELD(arg0, u16 *, 0x2E4), M2C_FIELD(arg0, s16 *, 0x2EC), 0);
    }
    frame_move(arg0);
    lb_npc_chr_sub(arg0);
    lb_npc_chr_sub(arg0);
    Lb_World_calc(arg0);
}

/* lb_npc_init_sub (0x5C3A40): the NPC program (lb_npc_prog_tbl[+2]: 0
 * person, 1 cat, 2 pig ...), its init (entry 0), a draw prim */
void lb_npc_init_sub(u8 *em) {
    s16 h;

    M2C_FIELD(em, u8 **, 0x3CC) = *(u8 **)(lb_npc_prog_tbl + M2C_FIELD(em, u8 *, 2) * 4);
    (*(void (**)(u8 *))M2C_FIELD(em, u8 **, 0x3CC))(em);
    if (M2C_FIELD(em, u8 *, 1) != 0) {
        M2C_FIELD(em, s16 *, 0x568) = get_prim();
        h = M2C_FIELD(em, s16 *, 0x568);
        if (h != -1) {
            M2C_FIELD(em, u8 **, 0x564) = (u8 *)get_prim_ptr(h);
            M2C_FIELD(M2C_FIELD(em, u8 **, 0x564), u8 **, 0x18) = em;
            M2C_FIELD(M2C_FIELD(em, u8 **, 0x564), void **, 0x14) = (void *)lb_npc_trans;
        }
    }
}

/* lb_npc_move (0x...): timers, hit stop, script move (Lb_npc_move_sub),
 * rotation, scale, motion, ground height (not for sitting / lying NPCs) */
void lb_npc_move(u8 *em) {
    u8 k;
    f32 y;

    M2C_FIELD(em, s16 *, 0x40E) = 0xA;
    Lb_pl_timer_calc(em);
    Lb_hit_stop_calc(em);
    M2C_FIELD(em, f32 *, 0x5A0) = M2C_FIELD(em, f32 *, 0xAC);
    M2C_FIELD(em, f32 *, 0x5A4) = M2C_FIELD(em, f32 *, 0xB0);
    M2C_FIELD(em, f32 *, 0x5A8) = M2C_FIELD(em, f32 *, 0xB4);
    Lb_Em_pos_adj(em);
    Lb_npc_move_sub(em);
    M2C_FIELD(em, s32 *, 0xA0) = M2C_FIELD(em, u16 *, 0xA0);
    M2C_FIELD(em, s32 *, 0xA4) = M2C_FIELD(em, u16 *, 0xA4);
    M2C_FIELD(em, s32 *, 0xA8) = M2C_FIELD(em, u16 *, 0xA8);
    cpRotMatrixYXZ2(em + 0xA0, em + 0x20);
    M2C_FIELD(em, f32 *, 0x1A0) = 2.0f * M2C_FIELD(em, f32 *, 0x930);
    M2C_FIELD(em, f32 *, 0x1F0) = 2.0f * M2C_FIELD(em, f32 *, 0x930);
    M2C_FIELD(em, f32 *, 0x240) = 2.0f * M2C_FIELD(em, f32 *, 0x930);
    M2C_FIELD(em, f32 *, 0x290) = 2.0f * M2C_FIELD(em, f32 *, 0x930);
    lb_npc_chr_sub(em);
    k = M2C_FIELD(em, u8 *, 0x452);
    if (k != 0x2F && k != 0x35 && k != 0x34 && k != 0x2C && k != 0x14 && k != 0x15 && k != 0x48 && k != 0x4D && k != 0x4A) {
        GetGroundHitStatusAreaEm(em, em + 0xAC, em + 0x70C, em + 0x5AC, &y);   /* t0 (water flag) is left over on the PS2 */
        M2C_FIELD(em, f32 *, 0xB0) = M2C_FIELD(em, f32 *, 0x5AC);
    }
}

/* lb_npc_chr_sub (0x...): rotation matrix, start the queued motions,
 * frame_move */
void lb_npc_chr_sub(u8 *em) {
    if (softdip_ck(0x28) == 0) {
        cpRotMatrix(em + 0xA0, em + 0x20);
        if (M2C_FIELD(em, u8 *, 0x40A) == 0) {
            if (M2C_FIELD(em, u8 *, 0x2FC) == 0) {
                M2C_FIELD(em, u8 *, 0x2FC) = 1;
                frame_init(em, M2C_FIELD(em, u16 *, 0x2E4), M2C_FIELD(em, s16 *, 0x2EC), 0);
            }
            if (M2C_FIELD(em, u8 *, 0 == 0x2FD) && M2C_FIELD(em, u16 *, 0x300) >= 2) {
                frame_init(em, M2C_FIELD(em, u16 *, 0x2E6), M2C_FIELD(em, s16 *, 0x2EE), 1);
                M2C_FIELD(em, u8 *, 0x2FD) = 1;
            }
            frame_move(em);
        }
    }
}

void lb_npc_die(u8 *em) {
    M2C_FIELD(em, u8 *, 4) += 1;
}

void lb_npc_erase(u8 *em) {
    push_em_work(em);
}

/* lb_npc_effect_move (0x5C4890) is in src/lobby/lb/lbui_nm.c */

/* lb_set_npc (0x5C4220): the NPC's record (0x44 bytes) from
 * npc_dialog_table+0x60[stage] by its number +0x1B: angle, position,
 * model kind (+2), script (+0x444 = record+0x2D), talk kind (+0x452),
 * scale */
void lb_set_npc(u8 *em) {
    u8 *tbl, *r, *w;

    tbl = *(u8 **)(npc_dialog_table + 0x60 + M2C_FIELD(em, u8 *, 0x736) * 4);
    w = em + 0x444;
    if (tbl != NULL) {
        r = tbl + M2C_FIELD(em, u8 *, 0x1B) * 0x44;
        M2C_FIELD(em, s32 *, 0xA8) = 0;
        M2C_FIELD(em, s32 *, 0xA0) = 0;
        M2C_FIELD(em, s32 *, 0xA4) = M2C_FIELD(r, s32 *, 4);
        M2C_FIELD(em, f32 *, 0xAC) = M2C_FIELD(r, f32 *, 8);
        M2C_FIELD(em, f32 *, 0xB0) = M2C_FIELD(r, f32 *, 0xC);
        M2C_FIELD(em, f32 *, 0xB4) = M2C_FIELD(r, f32 *, 0x10);
        M2C_FIELD(em, u8 *, 0x1B) = M2C_FIELD(r, u8 *, 1);
        M2C_FIELD(em, u8 *, 2) = M2C_FIELD(r, u8 *, 0x2C);
        M2C_FIELD(em, u8 *, 0x34F) = M2C_FIELD(r, u8 *, 0x2C);
        M2C_FIELD(w, u8 **, 0) = r + 0x2D;
        M2C_FIELD(w, u8 *, 0xE) = M2C_FIELD(r, u8 *, 0);
        M2C_FIELD(w, u8 **, 8) = M2C_FIELD(r, u8 **, 0x28);
        M2C_FIELD(w, s32 *, 0x14) = M2C_FIELD(r, s32 *, 0x24);
        M2C_FIELD(em, f32 *, 0x930) = M2C_FIELD(r, f32 *, 0x20);
        M2C_FIELD(em, f32 *, 0xB8) = M2C_FIELD(r, f32 *, 0x14);
        M2C_FIELD(em, f32 *, 0xBC) = M2C_FIELD(r, f32 *, 0x18);
        M2C_FIELD(em, f32 *, 0xC0) = M2C_FIELD(r, f32 *, 0x1C);
        set_event_npc(em);
        if (M2C_FIELD(w, u8 **, 8) != NULL) {
            u8 *p = M2C_FIELD(w, u8 **, 8);
            M2C_FIELD(em, f32 *, 0xAC) = M2C_FIELD(p, f32 *, 0);
            M2C_FIELD(em, f32 *, 0xB0) = M2C_FIELD(p, f32 *, 4);
            M2C_FIELD(em, f32 *, 0xB4) = M2C_FIELD(p, f32 *, 8);
        }
    }
}

void set_event_npc(u8 *arg0) {
    u8 temp_a0;
    u8 *temp_s0;

    temp_a0 = M2C_FIELD(arg0, u8 *, 0x452);
    temp_s0 = arg0 + 0x444;
    switch (temp_a0) {                              /* irregular */
    case 26:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            M2C_FIELD(temp_s0, int **, 8) = (int *)npcMv26_EVENT;
block_17:
            M2C_FIELD(arg0, s32 *, 0xA4) = 0;
        }
        return;
    case 33:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            M2C_FIELD(temp_s0, int **, 8) = (int *)npcMv33_EVENT;
            M2C_FIELD(arg0, s32 *, 0xA4) = 0xE001;
            return;
        }
        break;
    case 34:
        if ((Quest_clear_bit_ck(0x6B) == 1) && (Lb_guild_check_requireF() == 1) && (Lb_check_existF() == 1)) {
            M2C_FIELD(temp_s0, int **, 8) = (int *)npcMv34_EVENT;
            goto block_17;
        }
        break;
    }
}

void lb_npc_trans(u8 *arg0) {
    f32 sp160[4];
    f32 sp120[16];
    f32 spE0[16];
    f32 spA0[16];
    u8 *temp_s3;  /* was int *temp_s3 */
    s16 temp_s7;
    s32 temp_fp;
    s32 var_s5;
    s32 var_s6;
    u8 temp_v0;
    u8 temp_v1;
    u8 *temp_s0;
    u8 *temp_s1;
    u8 *var_s1;
    u8 *var_s4;

    temp_s0 = M2C_FIELD(arg0, u8 **, 0x18);
    temp_s1 = M2C_FIELD(temp_s0, u8 **, 0x50C);
    if (M2C_FIELD(temp_s0, u8 *, 0) != 0 && temp_s1 != NULL) {   /* no model: the PC has no npc_create_model yet */
        if (M2C_FIELD(temp_s0, u8 *, 1) == 0) {

        } else {
            pl_light_change(temp_s0, 1);
            Pl_light_set(temp_s0);
            cpAng2Rad_all(temp_s0 + 0xA0, sp160);
            flmatMakeScale(spA0, M2C_FIELD(temp_s0, f32 *, 0xB8), M2C_FIELD(temp_s0, f32 *, 0xBC), M2C_FIELD(temp_s0, f32 *, 0xC0));
            cpRotMatrixYXZ2(temp_s0 + 0xA0, sp120);
            flmatSetTrans(sp120, M2C_FIELD(temp_s0, f32 *, 0xAC), M2C_FIELD(temp_s0, f32 *, 0xB0), M2C_FIELD(temp_s0, f32 *, 0xB4));
            flmatMul33_2(sp120, spA0);
            flmatCopy(temp_s0 + 0x60, sp120);
            SetFilterMode(1);
            flSetRenderState(0x60, NULL);
            var_s4 = M2C_FIELD(temp_s1, u8 **, 0x30);
            temp_s7 = M2C_FIELD(temp_s1, s16 *, 0x2C);
            temp_fp = M2C_FIELD(temp_s1, s32 *, 0x10);
            flSetSkinTrans(M2C_FIELD(temp_s1, s32 *, 0x24));
            flmatInit(spE0);
            flSetRenderState(0x19, spE0);
            reload_tex(0xA, (M2C_FIELD(temp_s0, u8 *, 2) * 0x14) + 0x9A);
            var_s6 = 0;
            if (temp_s7 > 0) {
                do {
                    if ((M2C_FIELD(temp_s0, u8 *, 2) != 0) || (M2C_FIELD((temp_s0 + var_s6), s8 *, 0x4E6) != 0)) {
                        flSetRenderState(0x67, (int *)-1);
                        if (M2C_FIELD(var_s4, s32 *, 0) != -1) {
                            var_s5 = 0;
                            if (M2C_FIELD(var_s4, s32 *, 4) > 0) {
                                var_s1 = var_s4;
                                do {
                                    temp_v1 = M2C_FIELD(temp_s0, u8 *, 2);
                                    temp_s3 = (u8 *)(temp_fp + (M2C_FIELD(var_s1, s32 *, 8) * 0x4C));
                                    switch (temp_v1) { /* switch 1; irregular */
                                    case 1:         /* switch 1 */
                                        lb_normal_material(temp_s0, temp_s3, var_s5, ((s16)(var_s6)));
                                        if (M2C_FIELD((temp_s0 + var_s5), s8 *, 0x4E6) == 0) {
block_44:
                                            M2C_FIELD(temp_s3, s32 *, 0x10) = 0;
                                        } else {
                                            M2C_FIELD(temp_s3, s32 *, 0x10) = (s32) M2C_FIELD(temp_s0, s32 *, 0x798);
                                        }
                                        break;
                                    case 2:         /* switch 1 */
                                        M2C_FIELD(temp_s3, s32 *, 0x10) = (s32) M2C_FIELD(temp_s0, s32 *, 0x798);
                                        lb_cat_material(temp_s0, temp_s3, var_s5);
                                        break;
                                    case 0:         /* switch 1 */
                                        M2C_FIELD(temp_s3, s32 *, 0x10) = (s32) M2C_FIELD(temp_s0, s32 *, 0x798);
                                        lb_normal_material(temp_s0, temp_s3, var_s5, ((s16)(var_s6)));
                                        temp_v0 = M2C_FIELD((temp_s0 + 0x444), u8 *, 0xE);
                                        switch (temp_v0) { /* switch 2; irregular */
                                        case 0x33:  /* switch 2 */
                                        case 0x2F:  /* switch 2 */
                                        case 0x2D:  /* switch 2 */
                                        case 0x8:   /* switch 2 */
                                        case 0xB:   /* switch 2 */
                                            if ((var_s6 == 1) && (var_s5 == 1)) {
                                                goto block_44;
                                            }
                                            break;
                                        case 0x3A:  /* switch 2 */
                                        case 0x36:  /* switch 2 */
                                        case 0x35:  /* switch 2 */
                                        case 0x34:  /* switch 2 */
                                        case 0x30:  /* switch 2 */
                                        case 0x2E:  /* switch 2 */
                                        case 0x2B:  /* switch 2 */
                                        case 0x1D:  /* switch 2 */
                                        case 0x16:  /* switch 2 */
                                        case 0x15:  /* switch 2 */
                                        case 0x14:  /* switch 2 */
                                        case 0x17:  /* switch 2 */
                                        case 0x13:  /* switch 2 */
                                            if ((var_s6 == 6) && (var_s5 == 3)) {
                                                goto block_44;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    flSetRenderState((var_s5 + 0x3A) & 0xFF, temp_s3);
                                    var_s5 += 1;
                                    var_s1 += 4;
                                } while (var_s5 < M2C_FIELD(var_s4, s32 *, 4));
                            }
                            clay_attr_set(M2C_FIELD(var_s4, s32 *, 0x88));
                            flExecuteClay(M2C_FIELD(var_s4, s32 *, 0), 0);
                        }
                    }
                    var_s6 += 1;
                    var_s4 += 0x8C;
                } while (var_s6 < temp_s7);
            }
            clay_attr_reset();
            lb_npc_item_trans(temp_s0);
            flSetRenderState(0x60, NULL);
        }
    }
}

/* Lb_check_target (0x5CF700 region): the master player's talk target: every
 * NPC within 200 (village houses 0x51-0x56) or 400 and in front (+-60
 * degrees, 0x2AAB), then other players within 300 (+-50 degrees), sorted
 * by lb_insert_target_list; L1/R1 (+0x368 0x800/0x400) cycle it */
void Lb_check_target(void) {
    u8 *list;
    u8 *pl, *em, *p, *old;
    f32 range;
    s32 i;
    u16 sw;
    u8 st;

    list = NULL;
    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    old = M2C_FIELD(pl, u8 **, 0x3B0);
    if (M2C_FIELD(pl, u8 *, 0x15) == 0x58) {
        return;
    }
    if (M2C_FIELD(lb_sys, s32 *, 0x6C) == 1 || M2C_FIELD(lb_sys, s32 *, 0x68) != 0) {
        if ((*(u8 *)0x3F33FE != 0) || (SoftKeyboard_alive_check() != 0) || (M2C_FIELD(lb_sys, s32 *, 0x68) != 0)) {
            M2C_FIELD(lb_sys, s8 *, 0x86) = 1;
        }
        return;
    }
    sw = M2C_FIELD(pl, u16 *, 0x368);
    if (sw & 0x800) {
        sw_flag_1260 = 2;
    } else if (sw & 0x400) {
        sw_flag_1260 = 1;
    }
    st = *(u8 *)0x3F3404;
    range = (st >= 0x51 && st < 0x57) ? 200.0f : 400.0f;
    for (i = 0, em = em_work; i < 0x14; i++, em += 0xA10) {
        if (M2C_FIELD(em, u8 *, 0) == 0) {
            continue;
        }
        st = *(u8 *)0x3F3404;
        if (st >= 0x51 && st < 0x56 && M2C_FIELD(em, u8 *, 2) == 3 && M2C_FIELD(lb_sys + st, u8 *, 0x37) != 0) {
            continue;
        }
        lb_check_target(pl, em, &list, 0x2AAB, sw_flag_1260, range);
    }
    pl = player_work + (*(u8 *)0x3F34C1 * 0xA00);
    for (i = 0, p = player_work; i < 8; i++, p += 0xA00) {
        if (M2C_FIELD(p, u8 *, 0) != 0 && M2C_FIELD(p, u16 *, 0xC) != *(u8 *)0x3F34C1 && (Lb_Pl_stg_ck(p) & 0xFF)) {
            lb_check_target(pl, p, &list, 0x238E, sw_flag_1260, 300.0f);
        }
    }
    if (list == NULL) {
        if (M2C_FIELD(pl, u8 **, 0x3B0) != NULL) {
            M2C_FIELD(M2C_FIELD(pl, u8 **, 0x3B0), s8 *, 0x3D0) = 0;
        }
        M2C_FIELD(pl, u8 **, 0x3B0) = NULL;
        return;
    }
    if (M2C_FIELD(pl, u8 **, 0x3B0) != NULL) {
        M2C_FIELD(M2C_FIELD(pl, u8 **, 0x3B0), s8 *, 0x3D0) = 0;
        if (M2C_FIELD(pl, u16 *, 0x368) & 0xC00) {
            u8 *next = M2C_FIELD(M2C_FIELD(pl, u8 **, 0x3B0), u8 **, 0x3B0);
            M2C_FIELD(pl, u8 **, 0x3B0) = next != NULL ? next : list;
        } else if (M2C_FIELD(M2C_FIELD(pl, u8 **, 0x3B0), s16 *, 0x302) == -1) {
            M2C_FIELD(pl, u8 **, 0x3B0) = list;
        }
        if (old != M2C_FIELD(pl, u8 **, 0x3B0)) {
            cnWrap_SoundRequest(0xA);
        }
    } else {
        M2C_FIELD(pl, u8 **, 0x3B0) = list;
        cnWrap_SoundRequest(0xA);
    }
    M2C_FIELD(M2C_FIELD(pl, u8 **, 0x3B0), s8 *, 0x3D0) = 1;
}

/* lb_check_target (0x5CF600): tgt is a candidate when it is within +-lim
 * of the player's facing (Lb_get_angle) and nearer than range: its
 * distance (+0x4C4) and priority (+0x302, lb_target_angle), into the list;
 * else +0x302 = -1 */
s32 lb_check_target(u8 *pl, u8 *tgt, u8 **list, s32 lim, s32 mode, f32 range) {
    f32 d;
    s32 a, l;

    M2C_FIELD(tgt, u8 **, 0x3B0) = NULL;
    M2C_FIELD(tgt, s8 *, 0x3D0) = 0;
    a = Lb_get_angle(pl, tgt + 0xAC) & 0xFFFF;
    l = lim & 0xFFFF;
    if (a >= l && !(0xFFFF - l < a)) {
        M2C_FIELD(tgt, s16 *, 0x302) = -1;
        return 0;
    }
    d = flvecCalcDistance(pl + 0xAC, tgt + 0xAC);
    if (d < range) {
        M2C_FIELD(tgt, f32 *, 0x4C4) = d;
        lb_target_angle(pl, tgt, a, lim, mode);
        lb_insert_target_list(list, tgt);
        return 1;
    }
    return 0;
}

/* lb_target_angle (0x5CF1C0): priority 0..5 by the angle: 0-2 on one side,
 * 3-5 mirrored; with a target already chosen, L1 (mode 2) / R1 (mode 1)
 * prefer one side */
void lb_target_angle(u8 *pl, u8 *tgt, u32 a, u32 lim, s32 mode) {
    f32 step = 1.0f / ((f32)lim / 3.0f);
    s32 r;

    mode &= 0xFF;
    if (M2C_FIELD(pl, u8 **, 0x3B0) != NULL && mode != 0) {
        if (mode == 1) {
            if ((a & 0xFFFF) < (lim & 0xFFFF)) {
                r = (s32)((f32)a * step);
            } else {
                r = (s32)(u32)(6.0f - (f32)(0xFFFF - (a & 0xFFFF)) * step - 1.0f);
            }
        } else {
            if ((a & 0xFFFF) < (lim & 0xFFFF)) {
                r = (s32)(u32)(6.0f - (f32)a * step - 1.0f);
            } else {
                r = (s32)((f32)(0xFFFF - (a & 0xFFFF)) * step);
            }
        }
    } else if ((a & 0xFFFF) < (lim & 0xFFFF)) {
        r = (s32)((f32)a * step);
    } else {
        r = (s32)((f32)(0xFFFF - (a & 0xFFFF)) * step);
    }
    M2C_FIELD(tgt, s16 *, 0x302) = (s16)(r & 0xFFFF);
}

/* lb_insert_target_list (0x5CF100): insert tgt into the list chained
 * through +0x3B0, ordered by priority (+0x302) then distance (+0x4C4) */
void lb_insert_target_list(u8 **list, u8 *tgt) {
    u8 *head, *prev, *cur;
    s16 pr;
    u16 cp;

    if (list == NULL) {
        return;     /* the PS2 stores through a NULL list here; never happens */
    }
    head = *list;
    prev = NULL;
    cur = head;
    if (head != NULL) {
        pr = M2C_FIELD(tgt, s16 *, 0x302);
        for (;;) {
            cp = M2C_FIELD(cur, u16 *, 0x302);
            if (pr < (s32)cp) {
                break;
            }
            if (cp == pr) {
                if (pr < 3) {
                    if (!(M2C_FIELD(cur, f32 *, 0x4C4) <= M2C_FIELD(tgt, f32 *, 0x4C4))) {
                        break;
                    }
                } else if (!(M2C_FIELD(cur, f32 *, 0x4C4) < M2C_FIELD(tgt, f32 *, 0x4C4))) {
                    /* fall through to advance */
                } else {
                    break;
                }
            }
            prev = cur;
            cur = M2C_FIELD(cur, u8 **, 0x3B0);
            if (cur == NULL) {
                break;
            }
        }
    }
    if (prev == NULL) {
        M2C_FIELD(tgt, u8 **, 0x3B0) = head;
        *list = tgt;
        return;
    }
    M2C_FIELD(tgt, u8 **, 0x3B0) = M2C_FIELD(prev, u8 **, 0x3B0);
    M2C_FIELD(prev, u8 **, 0x3B0) = tgt;
}

/* Lb_ck_target (0x5D79F0): 1 if pos is within +-deg of pl's facing */
s32 Lb_ck_target(u8 *pl, f32 *pos, s32 deg) {
    s32 d, lim;

    d = (((M2C_FIELD(pl, s32 *, 0xA4) - (((calc_vec_ang2(pl + 0xAC, pos) & 0xFFFF) + 0x4000) & 0xFFFF)) & 0xFFFF) - 0x8000) & 0xFFFF;
    lim = (s32)(0.5f + ((65536.0f * (f32)deg) / 360.0f)) & 0xFFFF;
    if ((d > (0xFFFF - lim)) || (d < lim)) {
        return 1;
    }
    return 0;
}

s32 Lb_talk_check_default(s32 arg0) {
    s32 temp_a1;
    s32 temp_v1;
    s32 temp_a2;

    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (M2C_FIELD(lb_pit, s32 *, 0) >= 0) {
        M2C_FIELD(lb_pit, s32 *, 0) = (s32) (M2C_FIELD(lb_pit, s32 *, 0) + 1);
    }
    if (M2C_FIELD(lb_pit, s8 *, 0xB) == 0) {
        temp_a2 = ((s8)(arg0));
        temp_a1 = temp_v1 & 0xFFFF;
        if (!(temp_a2 & 1)) {
            if (temp_a1 & 0x20) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                if (M2C_FIELD(lb_pit, s8 *, 9) != 1) {
                    if (!(temp_a2 & 2)) {
                        cnWrap_SoundRequest(0, temp_a1, temp_a2);
                    }
                } else if (!(temp_a2 & 2)) {
                    cnWrap_SoundRequest(3, temp_a1, temp_a2);
                }
                return 1;
            }
            if (temp_a1 & 0x40) {
                if (*(u16 *)(M2C_FIELD(lb_pit, u8 **, 4) + (M2C_FIELD(lb_pit, s8 *, 8) * 8)) == 1) {
                    if (M2C_FIELD(lb_pit, s8 *, 9) != 1) {
                        M2C_FIELD(lb_pit, s8 *, 9) = 1;
                        cnWrap_SoundRequest(3, temp_a1, temp_a2);
                        return 0;
                    }
                    M2C_FIELD(lb_pit, s32 *, 0) = 0;
                    if (!(temp_a2 & 2)) {
                        cnWrap_SoundRequest(3, temp_a1, temp_a2);
                    }
                    return 1;
                }
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                if (!(temp_a2 & 2)) {
                    cnWrap_SoundRequest(3, temp_a1, temp_a2);
                }
                return 1;
            }
            if (*(u16 *)(M2C_FIELD(lb_pit, u8 **, 4) + (M2C_FIELD(lb_pit, s8 *, 8) * 8)) == 1) {
                if (temp_a1 & 0x800) {
                    if (M2C_FIELD(lb_pit, s8 *, 9) != 0) {
                        cnWrap_SoundRequest(1, temp_a1, temp_a2);
                        M2C_FIELD(lb_pit, s8 *, 9) = 0;
                    }
                } else if ((temp_a1 & 0x400) && (M2C_FIELD(lb_pit, s8 *, 9) != 1)) {
                    cnWrap_SoundRequest(1, temp_a1, temp_a2);
                    M2C_FIELD(lb_pit, s8 *, 9) = 1;
                }
            }
            goto block_34;
        }
        goto block_34;
    }
    if (!((((s8)(arg0))) & 1) && (temp_v1 & 0xFFFF & 0x60)) {
        M2C_FIELD(lb_pit, s32 *, 0) = -1;
        return 0;
    }
block_34:
    return 0;
}

void lb_check_status(void) {
    s16 temp_v1;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_s1;
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 var_a0;
    u32 temp_a2;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_s0;
    u8 *temp_v0_2;

    temp_a1 = *(u8 *)0x3F34C1;
    temp_s1 = Get_sw2(0) & 0xFFFF;
    temp_a2 = M2C_FIELD(lb_sys, u32 *, 0x68);
    var_a0 = temp_a1 * 0xA00;
    temp_s0 = player_work + var_a0;
    {
        switch (temp_a2) {      /* jump table jtbl_561_00665C70: 8 and 32 do nothing, 0x32+ like 0 */
        case 25:                                    /* switch 1 */
            if (SoftKeyboard_alive_check() == 0) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
            }
            break;
        case 21:                                    /* switch 1 */
            M2C_FIELD(lb_sys, u32 *, 0x68) = 0x16U;
            M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
            M2C_FIELD(cw, s8 *, 0x2C35) = 0;
            M2C_FIELD(lb_sys, s8 *, 6) = 0;
            break;
        case 22:                                    /* switch 1 */
            if (Lbs_RoomExit() == 1) {
                M2C_FIELD(temp_s0, s8 *, 0x915) = 0;
                M2C_FIELD(temp_s0, u8 *, 0x916) = 0U;
                Lbc_SendMiniData();
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x17U;
            }
            break;
        case 23:                                    /* switch 1 */
            temp_a0 = pNet;
            M2C_FIELD(temp_a0, u8 *, 0xC) = 1U;
            if (temp_s1 & 0xFFFF & 0x20) {
                *(s8 *)0x3F36AB = 1;
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                NPCZoomInCameraCancel();
            }
            break;
        case 30:                                    /* switch 1 */
            M2C_FIELD(lb_sys, u32 *, 0x68) = 0x1FU;
            str_fadeout(0, 0xF);
            break;
        case 31:                                    /* switch 1 */
            if ((Fade_busy_ck() & 0xFF) != 1) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x20U;
                Lbc_set_prim(0, 0, 0);
                Pit_reset();
            }
            break;
        case 35:                                    /* switch 1 */
            M2C_FIELD(lb_sys, u32 *, 0x68) = 0x24U;
            str_fadeout(0, 0xF);
            break;
        case 36:                                    /* switch 1 */
            if ((Fade_busy_ck() & 0xFF) != 1) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x25U;
                Lbc_set_prim(0, 0, 0);
                Pit_reset();
            }
            break;
        case 15:                                    /* switch 1 */
            if (Lb_menu_move() == 1) {
                *(u8 *)0x3F33FE = 0;
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                Lbc_init_network_work();
            }
            break;
        case 29:                                    /* switch 1 */
            temp_a0_2 = M2C_FIELD(mhRule, s8 *, 0) + 1;
            if ((temp_a0_2 >= 2) && ((M2C_FIELD(cw, u16 *, 0x32C6) + 1) == temp_a0_2)) {
                M2C_FIELD(pNet, u8 *, 0xC) = 0U;
                *(u8 *)0x3F36AB = 1;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
            } else {
                M2C_FIELD(pNet, u8 *, 0xC) = 1U;
                temp_v0 = Lb_select(temp_a0_2, temp_a1, temp_a2);
                switch (temp_v0) {                  /* switch 2; irregular */
                case 0:                             /* switch 2 */
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    *(u8 *)0x3F36AB = 1;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 7U;
                    M2C_FIELD(temp_s0, u8 *, 0x916) = (u8) (M2C_FIELD(temp_s0, u8 *, 0x916) | 0x20);
                    Lbc_SendMiniData();
                    break;
                case 3:                             /* switch 2 */
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                    *(u8 *)0x3F36AB = 1;
                    break;
                }
            }
            break;
        case 48:                                    /* switch 1 */
            temp_v0_2 = (u8 *)Lbs_GetRoomInfo(((s16)((M2C_FIELD(ClassInfo, u8 *, 8) - 1))), (u8 *)temp_a1, temp_a2);
            if (((M2C_FIELD(mhRule, s8 *, 0) + 1) >= 2) && ((M2C_FIELD(cw, u16 *, 0x32C6) + 1) < (s32) M2C_FIELD(temp_v0_2, u16 *, 2))) {
                M2C_FIELD(pNet, u8 *, 0xC) = 0U;
                *(u8 *)0x3F36AB = 1;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
            } else {
                M2C_FIELD(pNet, u8 *, 0xC) = 1U;
                temp_v0_3 = Lb_select();
                switch (temp_v0_3) {                /* switch 3; irregular */
                case 0:                             /* switch 3 */
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    *(u8 *)0x3F36AB = 1;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 7U;
                    M2C_FIELD(temp_s0, u8 *, 0x916) = (u8) (M2C_FIELD(temp_s0, u8 *, 0x916) | 0x20);
                    Lbc_SendMiniData();
                    break;
                case 3:                             /* switch 3 */
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                    *(u8 *)0x3F36AB = 1;
                    break;
                }
            }
            break;
        case 24:                                    /* switch 1 */
            temp_v1 = M2C_FIELD(lb_sys, s16 *, 0x74) - 1;
            M2C_FIELD(lb_sys, s16 *, 0x74) = temp_v1;
            if ((((s16)((s32) temp_v1))) <= 0) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
            }
            break;
        case 3:                                     /* switch 1 */
        case 9:                                     /* switch 1 */
        case 10:                                    /* switch 1 */
        case 12:                                    /* switch 1 */
        case 34:                                    /* switch 1 */
            Lb_shop();
            break;
        case 11:                                    /* switch 1 */
            Lb_mix();
            break;
        case 13:                                    /* switch 1 */
            Lb_armor_shop();
            break;
        case 14:                                    /* switch 1 */
            Lb_process_shop();
            break;
        case 2:                                     /* switch 1 */
            Lb_guild();
            break;
        case 43:                                    /* switch 1 */
            Lb_event_market();
            break;
        case 44:                                    /* switch 1 */
            Lb_event_GH();
            break;
        case 46:                                    /* switch 1 */
            Lb_event_localLv5End();
            break;
        case 45:                                    /* switch 1 */
            Lb_event_guild();
            break;
        case 47:                                    /* switch 1 */
            Lb_event_guildstart();
            break;
        case 49:                                    /* switch 1 */
            Lb_event_localLast();
            break;
        case 1:                                     /* switch 1 */
            Lb_join();
            break;
        case 26:                                    /* switch 1 */
            temp_v0_4 = Lbs_MatchEntry();
            switch (temp_v0_4) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                M2C_FIELD(lb_sys, u32 *, 0x68) = 8U;
                break;
            case 1:                                 /* switch 4 */
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x17U;
                SetDialogData_HTML(cw + 0x32D1);
                break;
            }
            break;
        case 7:                                     /* switch 1 */
            temp_v0_5 = Lbs_MatchEntry();
            switch (temp_v0_5) {                    /* switch 5; irregular */
            case 0:                                 /* switch 5 */
                M2C_FIELD(lb_sys, u32 *, 0x68) = 8U;
                Lb_Matching();
                break;
            case 1:                                 /* switch 5 */
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x17U;
                SetDialogData_HTML(cw + 0x32D1);
                break;
            }
            break;
        case 40:                                    /* switch 1 */
            M2C_FIELD(pNet, u8 *, 0xC) = 1U;
            switch (M2C_FIELD(lb_sys, s8 *, 6)) {  /* switch 6; irregular */
            case 0:                                 /* switch 6 */
                M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
                break;
            case 1:                                 /* switch 6 */
                temp_v0_6 = Lb_select(1, temp_a1, temp_a2);
                switch (temp_v0_6) {                /* switch 7; irregular */
                case 0:                             /* switch 7 */
                    M2C_FIELD(lb_sys, s8 *, 6) = 0;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 0x29U;
                    break;
                case 3:                             /* switch 7 */
                    M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                    M2C_FIELD(lb_sys, s8 *, 6) = 0;
                    *(u8 *)0x3F36AB = 1;
                    M2C_FIELD(lb_sys, u32 *, 0x68) = 8U;
                    break;
                }
                break;
            }
            break;
        case 41:                                    /* switch 1 */
            temp_v0_7 = Lbs_MatchEntryCancel();
            switch (temp_v0_7) {                    /* switch 8; irregular */
            case 0:                                 /* switch 8 */
                *(u8 *)0x3F36AB = 1;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                M2C_FIELD(temp_s0, u8 *, 0x916) = (u8) (M2C_FIELD(temp_s0, u8 *, 0x916) & ~0x20);
                M2C_FIELD(temp_s0, u8 *, 0x916) = (u8) (M2C_FIELD(temp_s0, u8 *, 0x916) | 0x10);
                Lbc_SendMiniData();
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                break;
            case 1:                                 /* switch 8 */
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x2AU;
                break;
            }
            break;
        case 42:                                    /* switch 1 */
            M2C_FIELD(pNet, u8 *, 0xC) = 1U;
            if (temp_s1 & 0xFFFF & 0x20) {
                *(u8 *)0x3F36AB = 1;
                M2C_FIELD(lb_sys, u32 *, 0x68) = 8U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
            }
            break;
        case 4:                                     /* switch 1 */
            if (Lb_pl_status_m(temp_s1, temp_a1, temp_a2) == 1) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
                Lbc_set_prim(0, 0, 0);
            }
            break;
        case 5:                                     /* switch 1 */
        case 6:                                     /* switch 1 */
            temp_v0_8 = Lb_talk();
            if ((temp_v0_8 != 3) && (temp_v0_8 != 0)) {

            } else {
                M2C_FIELD(temp_s0, s8 *, 0x90F) = 0;
                Lb_send_pl_chidori_off(temp_s0);
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s8 *, 0x87) = 0x14;
                NPCZoomInCameraCancel();
                cnWrap_SoundRequest(3);
            }
            break;
        case 17:                                    /* switch 1 */
            Lb_eat();
            break;
        case 27:                                    /* switch 1 */
            M2C_FIELD(lb_sys, u32 *, 0x68) = 0x1CU;
            break;
        case 28:                                    /* switch 1 */
            if (Lb_ItemBox_mv(temp_s1, temp_a1, temp_a2) == 0) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 1;
            }
            break;
        case 39:                                    /* switch 1 */
            if (Lb_gh_board() == 1) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0U;
                M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
            }
            break;
        case 16:                                    /* switch 1 */
            temp_a0_3 = Fade_busy_ck() & 0xFF;
            if ((temp_a0_3 != 1) && (Lbs_LobbyExit(temp_a0_3) == 1)) {
                M2C_FIELD(cw, s8 *, 0x35D5) = 0;
                SoftKeyboard_exit();
                Lbc_set_prim(0, 0, 0);
                To_EnterPlaza2Lobby();
            }
            break;
        case 8:
        case 32:
            break;
        default:
        case 0:                                         /* switch 1 */
    case 18:                                        /* switch 1 */
    case 19:                                        /* switch 1 */
    case 20:                                        /* switch 1 */
    case 33:                                        /* switch 1 */
    case 37:                                        /* switch 1 */
    case 38:                                        /* switch 1 */
        if (temp_a2 == 0) {
            if (SoftKeyboard_alive_check() != 0) {
                M2C_FIELD(lb_sys, u32 *, 0x68) = 0x19U;
            } else if ((temp_s1 & 0xFFFF & 0x8000) && (*(u8 *)0x3F33FE == 0)) {
                Lb_ck_menu();
            }
        }
        }
    }
    if (M2C_FIELD(pNet, u8 *, 0xC) != 0) {
        if (SoftKeyboard_alive_check() != 0) {
            SoftKeyboard_exit();
        }
        Lb_menu_exit();
        *(u8 *)0x3F33FE = 0U;
    }
}

s32 Lb_shop_sw(s32 arg0) {
    s32 temp_a0;
    s32 var_s0;

    var_s0 = 0;
    if ((s32) M2C_FIELD(lb_sys, u8 *, 0x8E) < 3) {
        return 0;
    }
    if (SoftKeyboard_alive_check() == 0) {
        temp_a0 = (((s8)(arg0))) * 0x22;
        var_s0 = ((*(u16 *)(D_3F3728 + temp_a0) & 0x3C00) | *(u16 *)(D_3F3714 + temp_a0)) & 0xFFFF;
    }
    if (var_s0 & 0xFFFF) {
        M2C_FIELD(lb_sys, u8 *, 0x8E) = 0U;
    }
    return var_s0;
}

void Lb_ck_menu(void) {
    s32 temp_a0;

    temp_a0 = M2C_FIELD(lb_sys, s32 *, 0x68);
    if (temp_a0 != 0xF) {
        M2C_FIELD(lb_sys, s32 *, 0x68) = 0xF;
        *(s8 *)0x3F33FE = 1;
        Lbc_init_network_work(temp_a0);
        Lbc_set_prim(0, 0, 0);
        lb_menu_init();
        M2C_FIELD(lbmw, s8 *, 5) = 1;
        M2C_FIELD(lbmw, s16 *, 6) = 0;
        se_req(7, 0x11, 0);
    }
}

s32 Lb_menu_move(void) {
    if (M2C_FIELD(lbmw, u8 *, 5) == 0) {
        Lb_menu_exit();
        return 1;
    }
    M2C_FIELD(lbmw, s16 *, 6) = Get_sw2(0);
    return 0;
}

void Lb_menu_exit(void) {
    M2C_FIELD(lbmw, s8 *, 5) = 0;
    M2C_FIELD(lbmw, s32 *, 0) = 0;
    *(s8 *)0x39DAD0 = 0;
}

void lb_menu_init(void) {
    M2C_FIELD(lbmw, s32 *, 0) = 0;
    *(s8 *)0x39DAD0 = 1;
    *(s8 *)0x39DAD1 = 4;
    *(s16 *)0x39DAD2 = (s16) M2C_FIELD(lbmw, u8 *, 4);
    *(s8 *)0x39DADC = 0;
}

void Lb_guild(void) {
    s32 temp_a0_2;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s8 temp_v0_5;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_a1_3;
    u8 temp_a1_4;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_a3_2;
    u8 temp_a3_3;
    u8 temp_s0_3;
    u8 *temp_s0;
    u8 *temp_s0_2;
    u8 *temp_s0_4;
    u8 *temp_s1;
    u8 *temp_v0;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *var_v0;

    temp_a0 = *(u8 *)0x3F34C1;
    temp_s1 = *(u8 * *)(D_3E4FA0 + (temp_a0 * 0xA00)) + 0x444;
    temp_s0 = (u8 *)get_quest_info(temp_a0);
    Get_sw2(0);
    switch (M2C_FIELD(lb_sys, s8 *, 6)) {          /* switch 1 */
    case 0:                                         /* switch 1 */
        M2C_FIELD(lb_rule_exp, s8 *, 4) = 0;
        M2C_FIELD(lb_rule_exp, s8 *, 0x68) = 0;
        M2C_FIELD(lb_rule_exp, s8 *, 0xCC) = 0;
        M2C_FIELD(lb_rule_exp, s8 *, 0x130) = 0;
        M2C_FIELD(lb_rule_exp, s8 *, 0x194) = 0;
        temp_a0_2 = M2C_FIELD(temp_s1, u8 *, 0xE) * 4;
        M2C_FIELD(lb_pit, s32 *, 4) = *(s32 *)(npc_dialog_table + temp_a0_2);
        if (M2C_FIELD(cw, s8 *, 0x2C2F) != 0) {
            M2C_FIELD(lb_quest_info, u8 *, 0x23) = (u8) M2C_FIELD(temp_s0, u8 *, 0x1D);
        }
        M2C_FIELD(lb_sys, s8 *, 7) = 0;
        M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
        if (Online_ck(temp_a0_2) == 0) {
            if ((Quest_clear_bit_ck(0xAA) == 1) && (Event_flag_ck(0x52) == 0)) {
                M2C_FIELD(lb_sys, s8 *, 6) = 0;
                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x31;
                return;
            }
            if ((Quest_clear_bit_ck(0xAB) == 1) && (Event_flag_ck(0x4D) == 0)) {
                M2C_FIELD(lb_sys, s8 *, 6) = 0;
                M2C_FIELD(lb_sys, s32 *, 0x68) = 0x2E;
                return;
            }
            if (key_quest == 0xAA) {
                key_quest_num = lb_set_key_quest_local();
            }
            goto block_17;
        }
        if (Event_flag_ck(4) == 0) {
            M2C_FIELD(lb_sys, s8 *, 6) = 0;
            M2C_FIELD(lb_sys, s32 *, 0x68) = 5;
            return;
        }
block_17:
        M2C_FIELD(cw, s8 *, 0x2C08) = 0;
    default:                                        /* switch 1 */
block_137:
        lb_guild_talk();
        return;
    case 1:                                         /* switch 1 */
        Lbc_init_network_work();
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, Lb_guild_trans, 0);
        if (Online_ck() == 0) {
            if (Lbs_InRoomCheck() == 0) {
                M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 8;
                M2C_FIELD(lb_pit, s8 *, 9) = 1;
                M2C_FIELD(lb_sys, s8 *, 6) = 0xF;
                M2C_FIELD(mhRule, s32 *, 0x54) = (s32) *(u16 *)0x3F341C;
                lb_set_questpage_info();
            }
        } else if (Lbs_InRoomCheck() == 0) {
            M2C_FIELD(lb_pit, s8 *, 8) = 0;
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            flMemset(mhRule, 0, 0x68);
        } else {
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 0xA;
            M2C_FIELD(lb_pit, s8 *, 9) = 1;
            M2C_FIELD(lb_sys, s8 *, 6) = 0xF;
            M2C_FIELD(mhRule, s32 *, 0x54) = (s32) *(u8 *)0x3F341C;
            lb_set_questpage_info();
        }
        goto block_137;
    case 2:                                         /* switch 1 */
        if (lb_guild_startMsg() == 1) {
            if (Online_ck() == 0) {
                if (Event_flag_ck(0) == 0) {
                    Event_flag_set(0);
                    lb_guild_end();
                    return;
                }
                if ((Event_flag_ck(1) == 0) && (Quest_clear_bit_ck(0x8B) == 1)) {
                    Event_flag_set(3);
                    lb_guild_end();
                    return;
                }
                goto block_36;
            }
block_36:
            cnWrap_SoundRequest(9);
            if (key_quest_num == 0) {
                M2C_FIELD(pNet, s8 *, 8) = lb_get_quest_level(0);
            } else if (Online_ck(0) == 1) {
                M2C_FIELD(pNet, s8 *, 8) = 6;
            } else {
                M2C_FIELD(pNet, s8 *, 8) = 5;
            }
            temp_v0 = pNet;
            if (M2C_FIELD(temp_v0, s8 *, 8) < 0) {
                M2C_FIELD(temp_v0, s8 *, 8) = 5;
            }
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            Gunner_wasure_ck(User_data);
            goto block_137;
        }
        goto block_137;
    case 3:                                         /* switch 1 */
        temp_v0_2 = lb_select_quest_level();
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 1;
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            break;
        case 3:                                     /* switch 2 */
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 6;
            M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
            break;
        }
        goto block_137;
    case 4:                                         /* switch 1 */
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            lb_set_questpage_info();
            cnWrap_SoundRequest(9);
        }
        goto block_137;
    case 5:                                         /* switch 1 */
        temp_v0_3 = lb_select_quest();
        switch (temp_v0_3) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            if (guildPrice > 0) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 2;
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 9;
            }
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            break;
        case 3:                                     /* switch 3 */
            M2C_FIELD(lb_sys, s8 *, 6) = 3;
            M2C_FIELD(pNet, s8 *, 7) = 0;
            temp_v1 = pNet;
            M2C_FIELD(temp_v1, s8 *, 8) = (s8) M2C_FIELD(temp_v1, u8 *, 0x13);
            break;
        }
        goto block_137;
    case 6:                                         /* switch 1 */
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            if (M2C_FIELD(lb_pit, s8 *, 9) == 0) {
                if (Lb_check_money(M2C_FIELD(mhRule, s32 *, 0x54)) == 0) {
                    if (Online_ck() == 0) {
                        M2C_FIELD(cw, s8 *, 0x35D3) = 1;
                        M2C_FIELD(cw, u8 *, 0x32C5) = 1U;
                        *(u8 *)0x3F360A = M2C_FIELD(cw, u8 *, 0x32C5);
                        Lb_menu_quest_info(*(u8 **)(lb_quest_all + (M2C_FIELD(mhRule, s32 *, 0x54) * 4)));
                        M2C_FIELD(lb_pit, s32 *, 0) = 0;
                        M2C_FIELD(lb_pit, s8 *, 8) = 4;
                        M2C_FIELD(lb_sys, s8 *, 6) = 0xA;
                        *(s16 *)0x3F33DC = (s16) M2C_FIELD(mhRule, s32 *, 0x54);
                        temp_s0_2 = *(u8 * *)(lb_quest_all + (M2C_FIELD(mhRule, s32 *, 0x54) * 4));
                        quest_price = (s32) M2C_FIELD(temp_s0_2, s32 *, 4);
                        Lb_menu_quest_info(temp_s0_2);
                        temp_a2 = *(u8 *)0x3F34C1;
                        temp_a1 = Lb_get_quest_type(temp_s0_2) | 0x40;
                        *(D_3E5506 + (temp_a2 * 0xA00)) = temp_a1;
                        Lb_set_mini_data(cw + (temp_a2 * 0x2FC) + 0x1346);
                        temp_a3 = *(u8 *)0x3F34C1;
                        memcpy(lbCommer + (temp_a3 * 0x5C) + 0x1C, cw + (temp_a3 * 0x2FC) + 0x1346, 0x40);
                    } else {
                        M2C_FIELD(lb_pit, s32 *, 0) = 0;
                        M2C_FIELD(lb_pit, s8 *, 8) = 3;
                        M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
                    }
                } else {
                    M2C_FIELD(lb_pit, s32 *, 0) = 0;
                    M2C_FIELD(lb_sys, s8 *, 6) = 0xB;
                    M2C_FIELD(lb_pit, s8 *, 8) = 7;
                    cnWrap_SoundRequest(7);
                }
            } else {
                M2C_FIELD(lb_pit, s8 *, 9) = 0;
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_sys, s8 *, 6) = 0xC;
                M2C_FIELD(lb_pit, s8 *, 8) = 5;
                cnWrap_SoundRequest(3);
            }
        }
        goto block_137;
    case 7:                                         /* switch 1 */
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            if (M2C_FIELD(lb_pit, s8 *, 9) == 0) {
                M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
                cnWrap_SoundRequest(9);
            } else {
                M2C_FIELD(lb_sys, s8 *, 6) = 9;
                cnWrap_SoundRequest(3);
            }
        }
        goto block_137;
    case 8:                                         /* switch 1 */
        if (lb_rule_seet_set() == 1) {
            M2C_FIELD(lb_sys, s8 *, 7) = 0;
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
        }
        goto block_137;
    case 9:                                         /* switch 1 */
        temp_v0_4 = lb_guild_make_room();
        switch (temp_v0_4) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            temp_s0_3 = M2C_FIELD(mhRule, u8 *, 0x58);
            if (temp_s0_3 == ((((s8)(get_questLevelNum()))) + 1)) {
                var_v0 = (u8 *)get_quest_info();
            } else {
                var_v0 = *(u8 * *)(lb_quest_all + (M2C_FIELD(mhRule, s32 *, 0x54) * 4));
            }
            Lb_menu_quest_info(var_v0);
            temp_a1_2 = Lb_get_quest_type(var_v0) | 0x40;
            temp_a0_3 = *(u8 *)0x3F34C1;
            *(D_3E5506 + (temp_a0_3 * 0xA00)) = temp_a1_2;
            Lbc_SendMiniData(temp_a0_3, temp_a1_2);
            Lb_set_mini_data(cw + (*(u8 *)0x3F34C1 * 0x2FC) + 0x1346);
            Lb_set_mini_data(lbCommer + (*(u8 *)0x3F34C1 * 0x5C) + 0x1C);
            if (M2C_FIELD(mhRule, s8 *, 0) == 0) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 8;
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 4;
            }
            M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            *(s16 *)0x3F33DC = (s16) M2C_FIELD(mhRule, s32 *, 0x54);
            *(u16 *)0x3F341C = (u16) M2C_FIELD(mhRule, s32 *, 0x54);
            quest_price = (s32) M2C_FIELD(var_v0, s32 *, 4);
            break;
        case 1:                                     /* switch 4 */
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 6;
            M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
            break;
        }
        goto block_137;
    case 10:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(0)))) != 0) {
            if (Online_ck() == 0) {
                *(s8 *)0x3E5506 = Lb_get_quest_type(*(u8 **)(lb_quest_all + (M2C_FIELD(mhRule, s32 *, 0x54) * 4))) | 0x40;
                Lb_set_mini_data(my_user_mini_data);
                memcpy(cw + 0x45A, my_user_mini_data, 0x40);
                temp_a3_2 = *(u8 *)0x3F34C1;
                memcpy(lbCommer + (temp_a3_2 * 0x5C) + 0x1C, cw + (temp_a3_2 * 0x2FC) + 0x1346, 0x40);
            }
            lb_guild_end();
        }
        goto block_137;
    case 15:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            if (M2C_FIELD(lb_pit, s8 *, 9) == 0) {
                if (Online_ck() == 0) {
                    M2C_FIELD(lb_pit, s32 *, 0) = 0;
                    M2C_FIELD(lb_pit, s8 *, 8) = 3;
                    M2C_FIELD(cw, s8 *, 0x35D3) = 0;
                    M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
                    temp_a1_3 = *(u8 *)0x3F34C1;
                    temp_s0_4 = *(u8 * *)(lb_quest_all + (M2C_FIELD(mhRule, s32 *, 0x54) * 4));
                    *(D_3E5506 + (temp_a1_3 * 0xA00)) = 0;
                    Lb_set_mini_data(cw + (temp_a1_3 * 0x2FC) + 0x1346);
                    temp_a3_3 = *(u8 *)0x3F34C1;
                    memcpy(lbCommer + (temp_a3_3 * 0x5C) + 0x1C, cw + (temp_a3_3 * 0x2FC) + 0x1346, 0x40);
                    if (M2C_FIELD(temp_s0_4, s32 *, 4) > 0) {
                        cnWrap_SoundRequest(8);
                        Gold_add(M2C_FIELD(temp_s0_4, s32 *, 4));
                        quest_price = 0;
                    } else {
                        cnWrap_SoundRequest(3);
                    }
                    goto block_111;
                }
                if (M2C_FIELD(cw, u8 *, 0x32C5) == 1) {
                    M2C_FIELD(lb_pit, s8 *, 9) = 1;
                    M2C_FIELD(lb_pit, s8 *, 8) = 0xB;
                    M2C_FIELD(lb_pit, s32 *, 0) = 0;
                    M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
                    cnWrap_SoundRequest(0);
                } else {
                    M2C_FIELD(lb_sys, s8 *, 6) = 0x11;
                    cnWrap_SoundRequest(0);
                }
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 6;
                M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
            }
        } else {
block_111:
            if (*(u16 *)0x3F3714 & 0x200) {
                temp_v1_2 = pNet;
                temp_v0_5 = (u8) M2C_FIELD(temp_v1_2, s8 *, 8) + 1;
                M2C_FIELD(temp_v1_2, s8 *, 8) = temp_v0_5;
                if ((temp_v0_5 & 0xFF) >= 3) {
                    M2C_FIELD(pNet, s8 *, 8) = 0;
                }
                Lb_num_to_str((u8) M2C_FIELD(pNet, s8 *, 8) + 1, lb_quest_exp + 0xCC);
                cnWrap_SoundRequest(6);
            }
        }
        goto block_137;
    case 16:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(0)))) != 0) {
            if (M2C_FIELD(lb_pit, s8 *, 9) == 0) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 0xB;
                M2C_FIELD(lb_pit, s8 *, 9) = 1;
                M2C_FIELD(lb_sys, s8 *, 6) = (s8) (M2C_FIELD(lb_sys, s8 *, 6) + 1);
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 6;
                M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
            }
        }
        goto block_137;
    case 17:                                        /* switch 1 */
        if (Lbs_RoomExit() == 1) {
            if (M2C_FIELD(cw, u8 *, 0x32C5) == 1) {
                Lb_back_money();
                quest_price = 0;
                M2C_FIELD(cw, u8 *, 0x32C5) = 0U;
            }
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 0xC;
            M2C_FIELD(lb_sys, s8 *, 6) = 0x12;
        }
        goto block_137;
    case 18:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(0)))) != 0) {
            temp_a1_4 = *(u8 *)0x3F34C1;
            temp_a0_4 = temp_a1_4 * 0xA00;
            *(D_3E5505 + temp_a0_4) = 0;
            *(D_3E5506 + temp_a0_4) = 0;
            Lbc_SendMiniData(temp_a0_4, temp_a1_4);
            Lb_set_mini_data(cw + (*(u8 *)0x3F34C1 * 0x2FC) + 0x1346);
            Lb_set_mini_data(lbCommer + (*(u8 *)0x3F34C1 * 0x5C) + 0x1C);
            M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
            M2C_FIELD(lb_sys, s8 *, 0x87) = 0x14;
            M2C_FIELD(lb_sys, s8 *, 6) = 0;
            M2C_FIELD(lb_sys, s32 *, 0x68) = 0;
            Lbc_init_network_work();
            NPCZoomInCameraCancel();
block_136:
            M2C_FIELD(cw, s8 *, 0x2C08) = 1;
        }
        goto block_137;
    case 11:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(0)))) != 0) {
            M2C_FIELD(lb_pit, s32 *, 0) = 0;
            M2C_FIELD(lb_pit, s8 *, 8) = 5;
            M2C_FIELD(lb_sys, s8 *, 6) = 0xC;
            M2C_FIELD(lb_pit, s8 *, 9) = 0;
        }
        goto block_137;
    case 12:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            if (M2C_FIELD(lb_pit, s8 *, 9) == 0) {
                M2C_FIELD(lb_sys, s8 *, 6) = 5;
                cnWrap_SoundRequest(9);
                Lbc_set_prim(0, Lb_guild_trans, 0);
                cnWrap_SoundRequest(9);
            } else {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_sys, s8 *, 6) = 0xD;
                M2C_FIELD(lb_pit, s8 *, 8) = 6;
                cnWrap_SoundRequest(3);
            }
        }
        goto block_137;
    case 13:                                        /* switch 1 */
        if ((((s8)(Lb_talk_check_default(0)))) != 0) {
            M2C_FIELD(lb_sys, s32 *, 0x6C) = 0;
            M2C_FIELD(lb_sys, s8 *, 0x87) = 0x14;
            M2C_FIELD(lb_sys, s8 *, 6) = 0;
            M2C_FIELD(lb_sys, s32 *, 0x68) = 0;
            Lbc_init_network_work();
            NPCZoomInCameraCancel();
            goto block_136;
        }
        goto block_137;
    }
}

void lb_guild_talk(void) {
    u8 *temp_v0;
    u8 *temp_s0;

    if (M2C_FIELD(lb_sys, s8 *, 6) >= 2) {
        temp_s0 = M2C_FIELD(lb_pit, u8 **, 4) + (M2C_FIELD(lb_pit, s8 *, 8) * 8);
        font_set_palette(0);
        switch (M2C_FIELD(lb_sys, s8 *, 6)) {      /* irregular */
        case 6:
            strcpy(guildStr, M2C_FIELD(temp_s0, int **, 4));
            temp_v0 = (u8 *)strrchr(guildStr, 0x24);
            if (temp_v0 != NULL) {
                Lb_num_to_str(guildPrice, temp_v0);
                strcat(guildStr, strrchr(M2C_FIELD(temp_s0, int **, 4), 0x24) + 1);
            }
            M2C_FIELD(lb_pit, s8 *, 0xB) = NPC_Message(guildStr, M2C_FIELD(lb_pit, s32 *, 0), M2C_FIELD(temp_s0, u16 *, 0), M2C_FIELD(lb_pit, s8 *, 9));
            return;
        case 17:
        case 9:
        case 0:
            NPC_Message(NULL, M2C_FIELD(lb_pit, s32 *, 0), 3U, M2C_FIELD(lb_pit, s8 *, 9));
            return;
        case 2:     /* the start message, the level / quest lists and the rule sheet draw their own */
        case 3:
        case 5:
        case 8:
            return;
        default:
            M2C_FIELD(lb_pit, s8 *, 0xB) = NPC_Message(M2C_FIELD(temp_s0, int **, 4), M2C_FIELD(lb_pit, s32 *, 0), M2C_FIELD(temp_s0, u16 *, 0), M2C_FIELD(lb_pit, s8 *, 9));
            break;
        }
    }
}

s32 lb_guild_startMsg(void) {
    s32 temp_a1;
    u8 *temp_s0;

    temp_a1 = M2C_FIELD(lb_pit, s8 *, 8) * 8;
    temp_s0 = M2C_FIELD(lb_pit, u8 **, 4) + temp_a1;
    switch (M2C_FIELD(lb_sys, s8 *, 7)) {          /* irregular */
    case 0:
        M2C_FIELD(lb_sys, s8 *, 7) = (s8) (M2C_FIELD(lb_sys, s8 *, 7) + 1);
        if (Online_ck(M2C_FIELD(lb_pit, s32 *, 4), temp_a1) == 0) {
            if (Event_flag_ck(1) == 1) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 0xB;
                return 0;
            }
            if (Quest_clear_bit_ck(0x8B) == 1) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 0x14;
                M2C_FIELD(lb_pit, s32 *, 4) = (s32) (M2C_FIELD(lb_pit, s32 *, 4) + (M2C_FIELD(lb_pit, s8 *, 8) * 8));
                return 0;
            }
            if (Quest_clear_bit_ck(0x88) == 1) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 0xA;
                return 0;
            }
            if ((Quest_clear_bit_ck(0x83) == 0) && (Event_flag_ck(0) == 0)) {
                M2C_FIELD(lb_pit, s32 *, 0) = 0;
                M2C_FIELD(lb_pit, s8 *, 8) = 0xC;
                M2C_FIELD(lb_pit, s32 *, 4) = (s32) (M2C_FIELD(lb_pit, s32 *, 4) + (M2C_FIELD(lb_pit, s8 *, 8) * 8));
                return 0;
            }
            goto block_17;
        }
block_17:
        M2C_FIELD(lb_pit, s8 *, 8) = 0;
block_32:
    default:
        return 0;
    case 1:
        if (Online_ck(M2C_FIELD(lb_pit, s32 *, 4), temp_a1) == 0) {
            if ((Event_flag_ck(1) == 0) && (Quest_clear_bit_ck(0x8B) == 1)) {
                if (Lb_put_npc_default() == 0) {
                    M2C_FIELD(lb_sys, s8 *, 7) = 0;
                    return 1;
                }
                return 0;
            }
            if ((Quest_clear_bit_ck(0x83) == 0) && (Event_flag_ck(0) == 0)) {
                if (Lb_put_npc_default() == 0) {
                    Gold_add(0x5DC);
                    cnWrap_SoundRequest(8);
                    Lb_put_set01(2);
                    M2C_FIELD(lb_sys, s8 *, 7) = 0;
                    return 1;
                }
                return 0;
            }
            goto block_30;
        }
block_30:
        M2C_FIELD(lb_pit, s8 *, 0xB) = NPC_Message(M2C_FIELD(temp_s0, s32 *, 4), M2C_FIELD(lb_pit, s32 *, 0), M2C_FIELD(temp_s0, u16 *, 0), M2C_FIELD(lb_pit, s8 *, 9));
        if ((((s8)(Lb_talk_check_default(2)))) != 0) {
            M2C_FIELD(lb_sys, s8 *, 7) = 0;
            return 1;
        }
        goto block_32;
    }
}

s32 lb_select_quest(void) {
    s32 temp_s0;
    s32 temp_v1_2;
    u8 temp_s0_2;
    u8 temp_s0_3;
    u8 temp_s0_4;
    u8 temp_s0_5;
    u8 temp_s1;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_4;
    u8 temp_v0_6;
    u8 temp_v0_7;
    u8 temp_v1;
    u8 var_v0_2;
    u8 var_v0_3;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v0_3;
    u8 *temp_v0_5;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *var_v0;

    temp_s1 = M2C_FIELD(mhRule, u8 *, 0x58);
    temp_s0 = Get_sw2(0) & 0xFFFF;
    if (temp_s1 == ((((s8)(get_questLevelNum()))) + 1)) {
        var_v0 = (u8 *)get_quest_info();
    } else {
        var_v0 = *(u8 * *)(lb_quest_all + (*(M2C_FIELD(pNet, u8 *, 7) + (lb_quest_info + ((temp_s1 & 0xFF) * 5))) * 4));
    }
    temp_a0 = pNet;
    temp_v1 = M2C_FIELD(temp_a0, u8 *, 2);
    temp_a1 = temp_a0 + 2;
    switch (temp_v1) {                              /* irregular */
    case 0:
        M2C_FIELD(temp_a0, u8 *, 2) = (u8) (temp_v1 + 1);
        lb_set_questpage_info(temp_a0, temp_a1);
        M2C_FIELD(pNet, u8 *, 8) = 0U;
        Lbc_set_prim(0, Lb_guild_trans, 0);
block_43:
    default:
        return 2;
    case 1:
        temp_v1_2 = temp_s0 & 0xFFFF;
        if (temp_v1_2 & 0x20) {
            *(s16 *)0x3F341C = (s16) M2C_FIELD(var_v0, u8 *, 0x1D);
            M2C_FIELD(mhRule, s32 *, 0x54) = (s32) M2C_FIELD(var_v0, u8 *, 0x1D);
            cnWrap_SoundRequest(0, temp_a1);
            return 0;
        }
        if (temp_v1_2 & 0x40) {
            cnWrap_SoundRequest(3, temp_a1);
            return 3;
        }
        if (temp_v1_2 & 0x400) {
            temp_s0_2 = M2C_FIELD(mhRule, u8 *, 0x58);
            if ((s32) temp_s0_2 < (((s8)(get_questLevelNum(temp_a0, temp_a1))))) {
                temp_v1_3 = pNet;
                temp_v0 = M2C_FIELD(temp_v1_3, u8 *, 7) + 1;
                M2C_FIELD(temp_v1_3, u8 *, 7) = temp_v0;
                if ((temp_v0 & 0xFF) >= 5) {
                    M2C_FIELD(pNet, u8 *, 7) = 0U;
                }
                lb_set_questpage_info();
                M2C_FIELD(mhRule, s32 *, 0x54) = (s32) M2C_FIELD(var_v0, u8 *, 0x1D);
                cnWrap_SoundRequest(1);
            } else if (key_quest_num >= 2) {
                temp_s0_3 = M2C_FIELD(mhRule, u8 *, 0x58);
                if (temp_s0_3 == (((s8)(get_questLevelNum())))) {
                    temp_v1_4 = pNet;
                    temp_v0_2 = M2C_FIELD(temp_v1_4, u8 *, 7) + 1;
                    M2C_FIELD(temp_v1_4, u8 *, 7) = temp_v0_2;
                    if ((temp_v0_2 & 0xFF) >= key_quest_num) {
                        M2C_FIELD(pNet, u8 *, 7) = 0U;
                    }
                    lb_set_questpage_info();
                    M2C_FIELD(mhRule, s32 *, 0x54) = (s32) M2C_FIELD(var_v0, u8 *, 0x1D);
                    cnWrap_SoundRequest(1);
                }
            }
        } else if (temp_v1_2 & 0x800) {
            temp_s0_4 = M2C_FIELD(mhRule, u8 *, 0x58);
            if ((s32) temp_s0_4 < (((s8)(get_questLevelNum(temp_a0, temp_a1))))) {
                temp_v0_3 = pNet;
                temp_v0_4 = M2C_FIELD(temp_v0_3, u8 *, 7);
                if (temp_v0_4 == 0) {
                    var_v0_2 = 4;
                } else {
                    var_v0_2 = temp_v0_4 - 1;
                }
                M2C_FIELD(temp_v0_3, u8 *, 7) = var_v0_2;
                lb_set_questpage_info();
                M2C_FIELD(mhRule, s32 *, 0x54) = (s32) M2C_FIELD(var_v0, u8 *, 0x1D);
                cnWrap_SoundRequest(1);
            } else if (key_quest_num >= 2) {
                temp_s0_5 = M2C_FIELD(mhRule, u8 *, 0x58);
                if (temp_s0_5 == (((s8)(get_questLevelNum())))) {
                    temp_v0_5 = pNet;
                    temp_v0_6 = M2C_FIELD(temp_v0_5, u8 *, 7);
                    if (temp_v0_6 == 0) {
                        var_v0_3 = key_quest_num - 1;
                    } else {
                        var_v0_3 = temp_v0_6 - 1;
                    }
                    M2C_FIELD(temp_v0_5, u8 *, 7) = var_v0_3;
                    lb_set_questpage_info();
                    M2C_FIELD(mhRule, s32 *, 0x54) = (s32) M2C_FIELD(var_v0, u8 *, 0x1D);
                    cnWrap_SoundRequest(1);
                }
            }
        } else if (temp_v1_2 & 0x200) {
            temp_v0_7 = M2C_FIELD(temp_a0, u8 *, 8) + 1;
            M2C_FIELD(temp_a0, u8 *, 8) = temp_v0_7;
            if ((temp_v0_7 & 0xFF) >= 3) {
                M2C_FIELD(pNet, u8 *, 8) = 0U;
            }
            Lb_num_to_str(M2C_FIELD(pNet, u8 *, 8) + 1, lb_quest_exp + 0xCC);
            cnWrap_SoundRequest(6);
        }
        goto block_43;
    }
}

s32 lb_select_quest_level(void) {
    s32 temp_a1;
    s32 temp_s0;
    s8 var_v0;
    u8 *temp_v1;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = temp_s0 & 0xFFFF;
    M2C_FIELD(pNet, s8 *, 0x12) = lb_get_quest_level(0);
    if (temp_a1 & 0x20) {
        if ((((s8)(check_questLevelSelect(M2C_FIELD(pNet, s8 *, 8), temp_a1)))) == 0) {
            temp_v1 = pNet;
            M2C_FIELD(mhRule, u8 *, 0x58) = (u8) M2C_FIELD(temp_v1, s8 *, 8);
            M2C_FIELD(temp_v1, u8 *, 0x13) = (u8) M2C_FIELD(temp_v1, s8 *, 8);
            M2C_FIELD(pNet, s8 *, 8) = 0;
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(7);
        goto block_11;
    }
    if (temp_a1 & 0x40) {
        cnWrap_SoundRequest(3, temp_a1);
        return 3;
    }
    if (Online_ck(3, temp_a1) == 1) {
        var_v0 = Lb_cursorUD((u8) M2C_FIELD(pNet, s8 *, 8), (((s8)(get_questLevelNum()))) + 2);
    } else {
        var_v0 = Lb_cursorUD((u8) M2C_FIELD(pNet, s8 *, 8), (((s8)(get_questLevelNum()))) + 1);
    }
    M2C_FIELD(pNet, s8 *, 8) = var_v0;
block_11:
    return 2;
}

s32 check_questLevelSelect(s32 arg0) {
    s32 temp_s0;
    s32 temp_s1;

    temp_s1 = ((s8)(lb_get_quest_level(0)));
    if (temp_s1 == -1) {
        if (((((s8)(arg0))) == 5) && (Online_ck() == 0)) {
            return 0;
        }
        return 1;
    }
    if ((Online_ck() == 0) && (key_quest == 0x83) && ((((s8)(arg0))) == 0)) {
        return 1;
    }
    temp_s0 = ((s8)(arg0));
    if (temp_s1 >= temp_s0) {
        if ((((s8)(lb_get_quest_level(0)))) >= temp_s0) {
            return 0;
        }
        goto block_22;
    }
    if (temp_s0 == (((s8)(get_questLevelNum(0))))) {
        if (key_quest_num != 0) {
            return 0;
        }
        goto block_22;
    }
    if ((Online_ck() == 1) && (M2C_FIELD(cw, s8 *, 0x2C2F) != 0) && (temp_s0 == 7)) {
        return 0;
    }
block_22:
    return 1;
}

s32 lb_get_quest_level(s32 arg0) {
    s32 var_v0;
    s32 var_v0_2;
    u8 temp_v0;

    if (Online_ck() == 1) {
        temp_v0 = *(u8 *)0x3C733B;
        if ((s32) temp_v0 < 5) {
            return 0;
        }
        if ((s32) temp_v0 < 9) {
            return 1;
        }
        if ((s32) temp_v0 < 0xD) {
            return 2;
        }
        if ((s32) temp_v0 < 0x11) {
            return 3;
        }
        if ((s32) temp_v0 < 0x13) {
            var_v0_2 = 4;
        } else {
            var_v0_2 = 5;
        }
        return var_v0_2;
    }
    if (Quest_clear_bit_ck(0xAB) == 1) {
        var_v0 = 4;
        if ((((s8)(arg0))) == 1) {
            var_v0 = 5;
        }
        return var_v0;
    }
    if (Quest_clear_bit_ck(0x8B) == 1) {
        return 4;
    }
    if (Quest_clear_bit_ck(0x9A) == 1) {
        return 3;
    }
    if (Quest_clear_bit_ck(0x89) == 1) {
        return 2;
    }
    if (Quest_clear_bit_ck(0x88) == 1) {
        return 1;
    }
    return -(Quest_clear_bit_ck(0x83) != 1);
}

void lb_set_questpage_info(void) {
    s32 temp_a1;
    s32 temp_s2_4;
    s32 temp_s2_5;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1_2;
    s32 var_s2;
    u32 temp_a2_2;
    u32 temp_v1;
    u8 temp_s0;
    u8 temp_s2;
    u8 temp_s2_2;
    u8 temp_s2_3;
    u8 temp_s2_6;
    u8 temp_v1_3;
    u8 *temp_a0;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_s0_2;
    u8 *var_s1;

    if (M2C_FIELD(cw, u8 *, 0x35D3) != 0) {
        if ((s32) M2C_FIELD(mhRule, u8 *, 0x54) >= 0xC8) {
            var_s1 = (u8 *)get_quest_info();
        } else {
            var_s1 = *(u8 * *)(lb_quest_all + (M2C_FIELD(mhRule, u8 *, 0x54) * 4));
        }
    } else {
        temp_s0 = M2C_FIELD(mhRule, u8 *, 0x58);
        if (temp_s0 == ((((s8)(get_questLevelNum()))) + 1)) {
            var_s1 = (u8 *)get_quest_info();
        } else {
            var_s1 = *(u8 * *)(lb_quest_all + (*(M2C_FIELD(pNet, u8 *, 7) + (lb_quest_info + ((temp_s0 & 0xFF) * 5))) * 4));
        }
    }
    temp_v0 = Lb_get_quest_type(var_s1);
    temp_a1 = temp_v0 * 8;
    temp_a2 = lb_quest_data_tbl + 0x24;
    temp_a0 = lb_quest_color_tex + temp_a1;
    temp_a3 = lb_quest_color_tex + 4 + temp_a1;
    M2C_FIELD(lb_quest_data_tbl, s16 *, 0x20) = (s16) M2C_FIELD(temp_a0, s16 *, 0);
    M2C_FIELD((lb_quest_data_tbl + 0x20), s16 *, 2) = (s16) M2C_FIELD(temp_a0, s16 *, 2);
    M2C_FIELD(lb_quest_data_tbl, s16 *, 0x24) = (s16) M2C_FIELD(temp_a3, s16 *, 0);
    M2C_FIELD(temp_a2, s16 *, 2) = (s16) M2C_FIELD(temp_a3, s16 *, 2);
    temp_s0_2 = M2C_FIELD(var_s1, u8 **, 0x18);
    sprintf(lb_quest_exp + 4, *(u8 **)(lb_quest_attribute + (temp_v0 * 4)));
    temp_s2 = M2C_FIELD(mhRule, u8 *, 0x58);
    if ((temp_s2 == ((((s8)(get_questLevelNum()))) + 1)) || ((s32) M2C_FIELD(var_s1, u8 *, 0x1D) >= 0xC8)) {
        strcpy(lb_quest_exp + 0x68, Lb_get_quest_str(0));
        strcpy(quest_title, Lb_get_quest_str(0));
    } else {
        strcpy(lb_quest_exp + 0x68, M2C_FIELD(temp_s0_2, s32 *, 0));
        strcpy(quest_title, M2C_FIELD(temp_s0_2, s32 *, 0));
    }
    Lb_num_to_str(M2C_FIELD(pNet, u8 *, 8) + 1, lb_quest_exp + 0xCC);
    Lb_num_to_str(M2C_FIELD(pNet, u8 *, 7) + 1, lb_quest_exp + 0x130);
    temp_s2_2 = M2C_FIELD(mhRule, u8 *, 0x58);
    if (temp_s2_2 != (((s8)(get_questLevelNum())))) {
        if ((Online_ck() == 1) && (temp_s2_3 = M2C_FIELD(mhRule, u8 *, 0x58), (temp_s2_3 == ((((s8)(get_questLevelNum()))) + 1)))) {
            strcpy(M2C_FIELD(lb_quest_message, int **, 0xC), M2C_FIELD(lb_guild_str, s32 *, 0));
        } else {
            strcpy(M2C_FIELD(lb_quest_message, int **, 0xC), M2C_FIELD(lb_guild_str, s32 *, 0x10));
        }
    } else {
        strcpy(M2C_FIELD(lb_quest_message, int **, 0xC), *(u8 **)(lb_quest_all + 0x31C + (key_quest_num * 4)));
    }
    Lb_num_to_str(M2C_FIELD(var_s1, s32 *, 8), lb_quest_exp + 0x194);
    strcat(lb_quest_exp + 0x194, lit_1489_00664A90);
    Lb_num_to_str(M2C_FIELD(var_s1, s32 *, 4), lb_quest_exp + 0x1F8);
    strcat(lb_quest_exp + 0x1F8, lit_1489_00664A90);
    guildPrice = (s32) M2C_FIELD(var_s1, s32 *, 4);
    /* time limit in ticks: minutes (/ 1800) and seconds */
    temp_s2_4 = (M2C_FIELD(var_s1, s32 *, 0x10) / 60) / 30;
    Lb_num_to_str(temp_s2_4, lb_quest_exp + 0x25C);
    temp_s2_5 = (M2C_FIELD(var_s1, s32 *, 0x10) - temp_s2_4 * 0x708) / 30;
    Lb_num_to_str(temp_s2_5, lb_quest_exp + 0x2C0);
    if (temp_s2_5 == 0) {
        strcat(lb_quest_exp + 0x2C0, M2C_FIELD(lb_num_str, int **, 0));
    }
    sprintf(lb_quest_exp + 0x324, *(u8 **)(map_name + (M2C_FIELD(var_s1, s32 *, 0x14) * 4)));
    temp_v1_3 = M2C_FIELD(var_s1, u8 *, 0x1D);
    if (temp_v1_3 != 0x6B) {
        if (((s32) temp_v1_3 >= 0x67) && ((s32) temp_v1_3 < 0x6B)) {
            goto block_26;
        }
        if (temp_v1_3 == 0x65) {
            sprintf(lb_quest_exp + 0x388, M2C_FIELD(lb_guild_str, s32 *, 0x20));
        } else if (((s32) M2C_FIELD(var_s1, u8 *, 2) < 4) || (Online_ck() == 0)) {
            sprintf(lb_quest_exp + 0x388, M2C_FIELD(lb_guild_str, s32 *, 0x14));
        } else {
            sprintf(lb_quest_exp + 0x388, M2C_FIELD(lb_guild_str, s32 *, 0x18));
        }
    } else {
block_26:
        sprintf(lb_quest_exp + 0x388, M2C_FIELD(lb_guild_str, s32 *, 0x24));
    }
    M2C_FIELD(lb_quest_exp, s8 *, 0x3EC) = 0;
    var_s2 = 0;
    if ((s32) M2C_FIELD(var_s1, u8 *, 2) > 0) {
        do {
            strcat(lb_quest_exp + 0x3EC, M2C_FIELD(lb_num_str, int **, 0x28));
            var_s2 += 1;
        } while (var_s2 < (s32) M2C_FIELD(var_s1, u8 *, 2));
    }
    temp_s2_6 = M2C_FIELD(mhRule, u8 *, 0x58);
    if ((temp_s2_6 == ((((s8)(get_questLevelNum()))) + 1)) || ((s32) M2C_FIELD(var_s1, u8 *, 0x1D) >= 0xC8)) {
        strcpy(lb_quest_exp + 0x450, Lb_get_quest_str(1));
        strcpy(lb_quest_exp + 0x4B4, Lb_get_quest_str(2));
        pDetail = (s32)Lb_get_quest_str(3);
        return;
    }
    strcpy(lb_quest_exp + 0x450, M2C_FIELD(temp_s0_2, s32 *, 4));
    strcpy(lb_quest_exp + 0x4B4, M2C_FIELD(temp_s0_2, s32 *, 8));
    pDetail = (s32) M2C_FIELD((temp_s0_2 + 8), s32 *, 4);
}

void lb_questpage_trans(u8 *arg0) {
    u8 *var_s2_2;  /* was int *var_s2_2 */
    u8 *var_s4_2;  /* was int *var_s4_2 */
    u8 *var_s4_4;  /* was int *var_s4_4 */
    u8 *var_s5_2;  /* was int *var_s5_2 */
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    s32 var_s4_3;
    s32 var_s5;
    u8 temp_a0;
    u8 temp_s4;
    u8 *var_s4;

    var_s0 = var_s1 = var_s2 = var_s3 = 0;    /* the PS2 keeps the caller's s0-s3 for other pages */
    if (Lbs_InRoomCheck() == 0) {
        temp_s4 = M2C_FIELD(mhRule, u8 *, 0x58);
        if (temp_s4 == ((((s8)(get_questLevelNum()))) + 1)) {
            var_s4 = (u8 *)get_quest_info();
        } else {
            var_s4 = *(u8 * *)(lb_quest_all + (*(M2C_FIELD(pNet, u8 *, 7) + (lb_quest_info + ((temp_s4 & 0xFF) * 5))) * 4));
        }
    } else if ((u32) M2C_FIELD(mhRule, u32 *, 0x54) >= 0xC8U) {
        var_s4 = (u8 *)get_quest_info();
    } else {
        var_s4 = *(u8 * *)(lb_quest_all + (M2C_FIELD(mhRule, u32 *, 0x54) * 4));
    }
    font_set_stack_no(M2C_FIELD(arg0, s32 *, 0x18));
    if (Quest_clear_bit_ck(M2C_FIELD(var_s4, u8 *, 0x1D)) == 1) {
        flfntSetSize(0x12, 0x12);
        font_set_palette(5);
        flfntLocate(0x20C, 0x46);
        font_print(lit_1576_00664A98, M2C_FIELD(lb_guild_str, s32 *, 0x1C));
    }
    Lb_put_gold();
    flfntSetSize(0x12, 0x12);
    font_set_palette(*(s16 *)(lb_quest_font_color + (Lb_get_quest_type(var_s4) * 2)));
    Lb_put_msg(lb_quest_exp);
    font_set_palette(0);
    var_s5 = 1;
    var_s4_2 = lb_quest_exp + 0x64;
    do {
        if (Lbs_InRoomCheck() != 0) {
            if (var_s5 != 3) {
                goto block_14;
            }
        } else {
block_14:
            Lb_put_msg(var_s4_2);
        }
        var_s5 += 1;
        var_s4_2 += 0x64;
    } while (var_s5 < 4);
    var_s4_3 = 0;
    var_s5_2 = lb_quest_message;
    do {
        if (Lbs_InRoomCheck() != 0) {
            if (var_s4_3 != 1) {
                goto block_20;
            }
        } else {
block_20:
            Lb_put_msg_type2(var_s5_2);
        }
        var_s4_3 += 1;
        var_s5_2 += 8;
    } while (var_s4_3 < 2);
    temp_a0 = M2C_FIELD(pNet, u8 *, 8);
    switch (temp_a0) {                              /* irregular */
    case 0:
        var_s1 = 2;
        var_s3 = 4;
        var_s2 = 0xA;
        var_s0 = 7;
        break;
    case 1:
        var_s3 = 0xA;
        var_s2 = 0xD;
        var_s1 = 7;
        var_s0 = 0xA;
        break;
    case 2:
        var_s1 = 0xA;
        var_s3 = 0xD;
        var_s2 = 0xE;
        var_s0 = 0xA;
        flfntLocate(0x157, 0xA0);
        font_print(lit_1576_00664A98, pDetail);
        break;
    }
    if (var_s3 < var_s2) {
        var_s4_4 = lb_quest_exp + (var_s3 * 0x64);
        do {
            if (Lbs_InRoomCheck() != 0) {
                if (var_s3 != 3) {
                    goto block_37;
                }
            } else {
block_37:
                Lb_put_msg(var_s4_4);
            }
            var_s3 += 1;
            var_s4_4 += 0x64;
        } while (var_s3 < var_s2);
    }
    if (var_s1 < var_s0) {
        var_s2_2 = lb_quest_message + (var_s1 * 8);
        do {
            if (Lbs_InRoomCheck() != 0) {
                if (var_s1 != 1) {
                    goto block_44;
                }
            } else {
block_44:
                Lb_put_msg_type2(var_s2_2);
            }
            var_s1 += 1;
            var_s2_2 += 8;
        } while (var_s1 < var_s0);
    }
    guild_trans_ot0(arg0);
}

/* lb_select_quest_level_trans (0x5C8810): the quest counter's level list:
 * one 2TF bar per level (lb_quest_level_str: {s16 x, s16 y, char *text}),
 * the cursor icon, grey (9) for levels not open yet, a mark (lit_462) for
 * cleared levels, then the key-quest / extra lines */
void lb_select_quest_level_trans(void) {
    u8 tf[0x14];
    u8 *rec, *r2, *done;
    s32 i, n, dx, col;
    s8 sel;

    memcpy(tf, lb_quest_data_tbl + 0x64, 0x14);
#define TFX M2C_FIELD(tf, s16 *, 0)
#define TFY M2C_FIELD(tf, s16 *, 2)
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    flfntSetSize(0x12, 0x12);
    rec = lb_quest_level_str;
    done = lb_quest_clear;
    n = (s8)get_questLevelNum();
    for (i = 0; i < n; i++) {
        sel = M2C_FIELD(pNet, u8 *, 8);
        dx = (i == (u8)sel) ? -0xC : 0;
        if ((M2C_FIELD(pNet, s8 *, 0x12) >= i) && ((i != 0) || (Online_ck() != 0) || (key_quest != 0x83))) {
            col = 5;
        } else {
            col = 9;
        }
        TFX += dx;
        Put_2TF(tf);
        if (i == M2C_FIELD(pNet, u8 *, 8)) {
            Lb_put_icon((s16)(TFX - 0xA), (s16)(TFY + 0xC), 1, 0xFF00FF00);
        }
        TFY += 0x30;
        if (Online_ck() == 1) {
            font_print_double((s16)(M2C_FIELD(rec, s16 *, 0) + dx), M2C_FIELD(rec, s16 *, 2), 1, (s16)col, M2C_FIELD(rec, u8 **, 4));
            r2 = rec + 8;
            font_print_double((s16)(M2C_FIELD(r2, s16 *, 0) + dx), M2C_FIELD(r2, s16 *, 2), 1, (s16)col, M2C_FIELD(r2, u8 **, 4));
        } else {
            font_print_double((s16)(M2C_FIELD(rec, s16 *, 0) + dx), (s16)(M2C_FIELD(rec, s16 *, 2) + 0xA), 1, (s16)col, M2C_FIELD(rec, u8 **, 4));
            r2 = rec + 8;
        }
        if (*done != 0) {
            if (Online_ck() == 0) {
                font_print_double((s16)(dx + 0x228), (s16)(M2C_FIELD(r2, s16 *, 2) - 0x13), 1, 7, lit_462_00664988);
            } else {
                font_print_double((s16)(dx + 0x228), M2C_FIELD(r2, s16 *, 2), 1, 7, lit_462_00664988);
            }
        }
        rec = r2 + 8;
        done += 1;
        TFX -= dx;
    }
    rec = lb_quest_level_str + 0x60;
    if (key_quest_num != 0) {
        col = 2;
        rec += 8;
    } else {
        col = 9;
    }
    dx = (M2C_FIELD(pNet, u8 *, 8) == (s8)get_questLevelNum()) ? -0xC : 0;
    TFX += dx;
    Put_2TF(tf);
    if (M2C_FIELD(pNet, u8 *, 8) == (s8)get_questLevelNum()) {
        Lb_put_icon((s16)(TFX - 0xA), (s16)(TFY + 0xA), 1, 0xFF00FF00);
    }
    if (Online_ck() == 0) {
        font_print_double((s16)(M2C_FIELD(rec, s16 *, 0) + (s16)dx), (s16)(M2C_FIELD(rec, s16 *, 2) - 0x30), 1, (s16)col, M2C_FIELD(rec, u8 **, 4));
        return;
    }
    font_print_double((s16)(M2C_FIELD(rec, s16 *, 0) + (s16)dx), M2C_FIELD(rec, s16 *, 2), 1, (s16)col, M2C_FIELD(rec, u8 **, 4));
    TFX -= dx;
    TFY += 0x30;
    {
        s8 extra = M2C_FIELD(cw, s8 *, 0x2C2F);
        dx = (M2C_FIELD(pNet, u8 *, 8) == (s8)get_questLevelNum() + 1) ? -0xC : 0;
        TFX += dx;
        Put_2TF(tf);
        if (M2C_FIELD(pNet, u8 *, 8) == (s8)get_questLevelNum() + 1) {
            Lb_put_icon((s16)(TFX - 0xA), (s16)(TFY + 0xA), 1, 0xFF00FF00);
        }
        font_print_double((s16)(M2C_FIELD(lb_quest_level_str, s16 *, 0x70) + (s16)dx), M2C_FIELD(lb_quest_level_str, s16 *, 0x72), 1,
                          (s16)(extra == 0 ? 9 : 5), M2C_FIELD(lb_quest_level_str, u8 **, 0x74));
        TFX -= dx;
    }
#undef TFX
#undef TFY
}

void guild_trans_ot0(u8 *arg0) {
    u8 *var_s2;  /* was int *var_s2 */
    s32 temp_s0;
    s32 var_s1;
    u8 temp_s1;

    temp_s0 = Lb_get_cursor_col();
    font_set_stack_no(M2C_FIELD(arg0, s32 *, 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    var_s1 = 0;
    var_s2 = lb_quest_data_tbl;
    do {
        if (Lbs_InRoomCheck() != 0) {
            if (var_s1 != 3) {
                goto block_4;
            }
        } else {
block_4:
            Put_2TF(var_s2);
        }
        var_s1 += 1;
        var_s2 += 0x14;
    } while (var_s1 <= 3);
    Lb_put_button(0x1E6, 0x134, 3);
    if ((M2C_FIELD(lb_sys, s32 *, 0x68) == 2) && (M2C_FIELD(lb_sys, s8 *, 6) < 0xA)) {
        temp_s1 = M2C_FIELD(mhRule, u8 *, 0x58);
        if ((s32) temp_s1 >= (((s8)(get_questLevelNum(M2C_FIELD(lb_sys, s32 *, 0x68)))))) {
            if (key_quest_num > 1) {
                goto block_11;
            }
        } else {
block_11:
            Lb_put_icon(0x13E, 0x1A, 0, temp_s0);
            Lb_put_icon(0x19E, 0x1A, 1, temp_s0);
        }
    }
}

void lb_disp_name(u8 *arg0) {
    f32 sp100[3];
    f32 spF0[4];
    f32 spB0[16];
    u8 *spA0;  /* was u8 **spA0 */
    int var_s3;
    f32 temp_f1;
    f32 temp_f2;
    s32 var_fp;
    s32 var_v0;
    s32 temp_s1_2;
    s32 temp_s2;
    s32 temp_s2_2;
    s32 temp_s5;
    s32 var_s0_2;
    s32 var_s1;
    s32 var_s3_2;
    s32 var_s4;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 *var_s0;  /* was u8 **var_s0 */
    u8 *var_s6;  /* was u8 **var_s6 */
    u8 *temp_s1;
    u8 *temp_v1;

    spA0 = lb_player;
    if ((M2C_FIELD(lb_sys, s32 *, 0x68) != 8) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0xF) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0)) {
        return;
    }
    font_set_stack_no(M2C_FIELD(arg0, s32 *, 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, NULL);
    var_s4 = 0;
    var_s6 = lb_player;
    var_fp = 0;
    do {
        temp_s1 = (*(u8 **)spA0);
        if ((M2C_FIELD(temp_s1, u8 *, 0) != 0) && (M2C_FIELD(temp_s1, u8 *, 1) != 0) && (Lb_Pl_stg_ck(temp_s1) & 0xFF) && (Lb_get_pl_stat2(((s8)(var_s4))) == 0) && ((M2C_FIELD(temp_s1, f32 *, 0xAC) != 0.0f) || (M2C_FIELD(temp_s1, f32 *, 0xB4) != 0.0f))) {
            var_s0 = var_s6 + 4;
            if ((Online_ck() == 1) && (*(u8 *)0x39DAD4 != 0)) {
                var_s0 = var_s6 + 0x24;
            }
            temp_s2 = ((s16)(strlen(var_s0)));
            flmatInit(spB0);
            flSetRenderState(0x1A, spB0);
            sp100[0] = M2C_FIELD(temp_s1, f32 *, 0xAC);
            sp100[1] = 190.0f + M2C_FIELD(temp_s1, f32 *, 0xB0);
            sp100[2] = M2C_FIELD(temp_s1, f32 *, 0xB4);
            flvecrRotTransPers(spF0, sp100);
            if ((spF0[0] < 700.0f) && !(spF0[0] <= -60.0f)) {
                temp_f1 = spF0[1];
                if ((temp_f1 < 500.0f) && !(temp_f1 <= -20.0f) && !(spF0[3] <= 0.0f)) {
                    temp_s2_2 = ((s16)(temp_s2));
                    var_v0 = temp_s2_2 / 2;
                    temp_f2 = (f32) (var_v0 * 8);
                    spF0[0] -= temp_f2;
                    flfntLocate((s32)(1.25f * (f32)(s32)spF0[0]), (s32)spF0[1]);
                    temp_s5 = (s16)(s32)(1.25f * spF0[0]);
                    temp_s1_2 = (s16)(s32)spF0[1];
                    flfntSetSize(0x10, 0x10);
                    temp_v1 = cw + var_fp;
                    temp_v1_2 = M2C_FIELD(temp_v1, u8 *, 0x1347);
                    if (temp_v1_2 == 0x14) {
                        var_s3 = 2;
                    } else if ((s32) temp_v1_2 >= 0xD) {
                        var_s3 = 6;
                    } else {
                        var_s3 = 5;
                    }
                    font_set_stack_no(0);
                    font_print_double((s32)(1.25f * (f32)(s32)spF0[0]), (s32)spF0[1], 1, var_s3, var_s0);
                    if ((((s16)(var_s4))) == *(u8 *)0x3F34C1) {
                        s32 job = (s8)Get_weapon_job(D_3C738C);
                        if (job == 5) {
                            job = 1;
                        }
                        var_s3_2 = ((s16)(temp_s1_2));
                        var_s1 = ((s16)(temp_s5));
                        var_s0_2 = var_s3_2 - 4;
                        Lb_put_job(((s16)((var_s1 - 0x16))), ((s16)(var_s0_2)), 0x16, -1, (s8)job, 0);
                    } else {
                        var_s3_2 = ((s16)(temp_s1_2));
                        var_s1 = ((s16)(temp_s5));
                        var_s0_2 = var_s3_2 - 4;
                        Lb_put_job(((s16)((var_s1 - 0x16))), ((s16)(var_s0_2)), 0x16, -1, M2C_FIELD(temp_v1 + 0x1346, u8 *, 0), 0);
                    }
                    temp_v1_3 = M2C_FIELD((temp_v1 + 0x1346), u8 *, 0x15);
                    if (temp_v1_3 & 0xC0) {
                        if (!(temp_v1_3 & 0x40)) {
                            if (System_timer & 0x10) {
                                goto block_37;
                            }
                        } else {
block_37:
                            Lb_put_icon_free(((s16)(((var_s1 + (temp_s2_2 * 4)) - 0xC))), ((s16)((var_s3_2 - 0x1A))), 0x16, *(s32 *)(lb_quest_color_tbl + ((temp_v1_3 & 0xF) * 4)), -1);
                        }
                    } else if (temp_v1_3 & 0x30) {
                        if ((temp_v1_3 & 0x10) || (System_timer & 0x10)) {
                            Lb_put_icon_free(((s16)(((var_s1 + (temp_s2_2 * 4)) - 0xC))), ((s16)((var_s3_2 - 0x1A))), 0x16, *(s32 *)(lb_quest_color_tbl + ((temp_v1_3 & 0xF) * 4)), -1);
                        }
                    } else {
                        Lb_put_status(((s16)((var_s1 + (temp_s2_2 * 8) + 6))), ((s16)(var_s0_2)), 0x14, -1, M2C_FIELD(temp_v1 + 0x1346, u8 *, 2));
                    }
                }
            }
        }
        var_s6 += 0x38;
        var_s4 = ((s16)((var_s4 + 1)));
        spA0 += 0x38;
        var_fp += 0x2FC;
    } while (var_s4 < 8);
}

void Lb_put_help(u8 *arg0) {
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    u16 temp_a0;
    u8 temp_a3;
    u8 *temp_s0;
    u8 *temp_v1;

    temp_a3 = *(u8 *)0x3F34C1;
    temp_a2 = temp_a3 * 0xA00;
    temp_s0 = player_work + temp_a2;
    if ((M2C_FIELD(lb_sys, s32 *, 0x68) != 8) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 7) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0x27) && (M2C_FIELD(lb_sys, s32 *, 0x68) != 0)) {
        return;
    }
    if (Online_ck(M2C_FIELD(lb_sys, s32 *, 0x68), player_work, temp_a2, temp_a3) == 1) {
        Lb_put_new_mail(0x1A2, 0x1C);
    }
    font_set_stack_no(M2C_FIELD(arg0, s32 *, 0x18));
    font_set_palette(0);
    flfntSetSize(0x14, 0x14);
    if ((((s8)(SoftKeyboard_alive_check()))) == 0) {
        flfntSetSize(0x14, 0x14);
        if (M2C_FIELD(lb_sys, s8 *, 0xA) == 0) {
            if (M2C_FIELD(lb_sys, s32 *, 0x68) == 8) {
                goto block_11;
            }
            goto block_83;
        }
block_11:
        temp_v1 = M2C_FIELD(temp_s0, u8 **, 0x878);
        if (temp_v1 != NULL) {
            temp_a0 = M2C_FIELD(temp_v1, u16 *, 2);
            switch (temp_a0) {                      /* switch 1; irregular */
            case 19:                                /* switch 1 */
                if (M2C_FIELD(temp_s0, u8 *, 0x14) != 1) {
                    temp_v0 = Lb_check_hotel(0x52);
                    switch (temp_v0) {              /* switch 2; irregular */
                    case 0:                         /* switch 2 */
                        M2C_FIELD(lb_sys, s8 *, 0xA) = 0;
                        break;
                    case 3:                         /* switch 2 */
                    case 1:                         /* switch 2 */
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(((s16)(((strlen(lb_sys + 0xA) * 0xA) + 0x50))), 0x1F);
                        font_print(lit_427_00664C20, M2C_FIELD(room_price, u8 **, 4));
                        /* fallthrough */
                    case 2:                         /* switch 2 */
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x1F);
                        font_print(lit_428_00664C30, lb_sys + 0xA);
                        Lb_put_gold();
                        break;
                    }
                    temp_v0_2 = Lb_check_hotel(0x53);
                    switch (temp_v0_2) {            /* switch 3; irregular */
                    case 0:                         /* switch 3 */
                        M2C_FIELD(lb_sys, s8 *, 0x28) = 0;
                        break;
                    case 3:                         /* switch 3 */
                    case 1:                         /* switch 3 */
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(((s16)(((strlen(lb_sys + 0x28) * 0xA) + 0x50))), 0x39);
                        font_print(lit_427_00664C20, M2C_FIELD(room_price, u8 **, 8));
                        /* fallthrough */
                    case 2:                         /* switch 3 */
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x39);
                        font_print(lit_428_00664C30, lb_sys + 0x28);
                        break;
                    }
                    goto block_83;
                }
                break;
            case 20:                                /* switch 1 */
                if (M2C_FIELD(temp_s0, u8 *, 0x14) != 1) {
                    temp_v0_3 = Lb_check_hotel(0x54);
                    switch (temp_v0_3) {            /* switch 4; irregular */
                    case 0:                         /* switch 4 */
                        M2C_FIELD(lb_sys, s8 *, 0xA) = 0;
                        break;
                    case 3:                         /* switch 4 */
                    case 1:                         /* switch 4 */
                        flfntLocate(((s16)(((strlen(lb_sys + 0xA) * 0xA) + 0x50))), 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, M2C_FIELD(room_price, u8 **, 0xC));
                        /* fallthrough */
                    case 2:                         /* switch 4 */
                        flfntLocate(0x30, 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, lb_sys + 0xA);
                        Lb_put_gold();
                        break;
                    }
                    temp_v0_4 = Lb_check_hotel(0x55);
                    switch (temp_v0_4) {            /* switch 5; irregular */
                    case 0:                         /* switch 5 */
                        M2C_FIELD(lb_sys, s8 *, 0x28) = 0;
                        break;
                    case 3:                         /* switch 5 */
                    case 1:                         /* switch 5 */
                        flfntLocate(((s16)(((strlen(lb_sys + 0x28) * 0xA) + 0x50))), 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, M2C_FIELD(room_price, u8 **, 0x10));
                        /* fallthrough */
                    case 2:                         /* switch 5 */
                        flfntLocate(0x30, 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, lb_sys + 0x28);
                        break;
                    }
                    goto block_83;
                }
                break;
            case 18:                                /* switch 1 */
                if (M2C_FIELD(temp_s0, u8 *, 0x14) != 1) {
                    if (Lb_check_hotel(0x51) == 0) {
                        M2C_FIELD(lb_sys, s8 *, 0x28) = 0;
                        return;
                    }
                    flfntSetSize(0x14, 0x14);
                default:                            /* switch 1 */
                    flfntLocate(0x30, 0x1F);
                    if (M2C_FIELD(lb_sys, s32 *, 0x68) != 8) {
                        if (M2C_FIELD(lb_sys, s32 *, 0x68) == 0x28) {
                            goto block_69;
                        }
                        if (M2C_FIELD(temp_s0, u8 *, 0x14) != 1) {
                            font_print(lit_428_00664C30, lb_sys + 0xA);
                            goto block_79;
                        }
                    } else {
block_69:
                        if (M2C_FIELD(M2C_FIELD(temp_s0, u8 **, 0x878), u16 *, 2) == 6) {
                            if (M2C_FIELD(cw, u8 *, 0x32C5) != 0) {
                                goto block_72;
                            }
                            font_set_palette(0);
                            font_print(lit_429_00664C38, M2C_FIELD(lb_rule_msg_etc, u8 **, 0x2C));
                            goto block_79;
                        }
block_72:
                        if (M2C_FIELD(temp_s0, u8 *, 0x14) != 1) {
                            font_set_palette(6);
                            font_print(lit_429_00664C38, M2C_FIELD(lb_rule_msg_etc, u8 **, 0x1C));
block_79:
                            sprintf(lb_sys + 0x28, lit_430_00664C40);
                            goto block_83;
                        }
                    }
                }
                break;
            }
        } else {
            if (M2C_FIELD(lb_sys, s32 *, 0x68) == 8) {
                flfntLocate(0x30, 0x1F);
                font_set_palette(6);
                font_print(lit_429_00664C38, M2C_FIELD(lb_rule_msg_etc, u8 **, 0x1C));
            }
block_83:
            lb_put_sprite();
        }
    }
}

/* Lb_put_button (0x5CC920): button icon idx of lb_button_tbl ({s16 u0, v0,
 * u1, v1}) at (x*0.8, y), size (u1-u0)*0.8 x (v1-v0) (idx 9: 50 x 32);
 * returns x + its width */
s32 Lb_put_button(s32 x, s32 y, s32 idx) {
    s16 q[10];
    u8 *b = lb_button_tbl + idx * 8;

    q[0] = (s16)(s32)(0.8f * (f32)x);
    q[1] = (s16)y;
    q[2] = (s16)(s32)(0.8f * (f32)(M2C_FIELD(b, s16 *, 4) - M2C_FIELD(b, s16 *, 0)));
    q[3] = M2C_FIELD(b, s16 *, 6) - M2C_FIELD(b, s16 *, 2);
    if (idx == 9) {
        q[2] = 0x32;
        q[3] = 0x20;
    }
    *(s32 *)&q[4] = -1;
    q[6] = M2C_FIELD(b, s16 *, 0);
    q[7] = M2C_FIELD(b, s16 *, 2);
    q[8] = M2C_FIELD(b, s16 *, 4);
    q[9] = M2C_FIELD(b, s16 *, 6);
    flps0008(q);
    return (s16)((s16)x + (M2C_FIELD(b, s16 *, 4) - M2C_FIELD(b, s16 *, 0)));
}

/* Lb_draw_square (0x5D7C30): a rectangle outline of four lines, colour
 * col; x (and w when scale_w) scaled by 0.8 */
void Lb_draw_square(s32 x, s32 y, s32 w, s32 h, u32 col, s32 scale_w) {
    s16 q[6];
    s16 sx, y1;

    sx = (s16)(s32)(0.8f * (f32)x);
    if (scale_w != 0) {
        w = (s16)(s32)(0.8f * (f32)w);
    }
    *(u32 *)&q[4] = col;
    y1 = (s16)((s16)y + (s16)h);
    q[0] = sx; q[1] = (s16)y; q[2] = sx; q[3] = y1;
    flps0002(q);
    q[0] = q[2] = (s16)(sx + (s16)w);
    flps0002(q);
    q[0] = sx; q[1] = q[3] = (s16)y;
    flps0002(q);
    q[1] = q[3] = y1;
    flps0002(q);
}

/* Lb_put_2TF (0x5D7B80): a 2TF sprite record (0x14 bytes) with its x (and
 * its width when scale_w) scaled by 0.8 */
void Lb_put_2TF(u8 *rec, s32 scale_w) {
    u8 q[0x14];

    memcpy(q, rec, 0x14);
    M2C_FIELD(q, s16 *, 0) = (s16)(s32)(0.8f * (f32)M2C_FIELD(q, s16 *, 0));
    if (scale_w != 0) {
        M2C_FIELD(q, s16 *, 4) = (s16)(s32)(0.8f * (f32)M2C_FIELD(q, s16 *, 4));
    }
    flps0008(q);
}

/* lb_npc_adr_tbl's entries for the village people (0x5C48A0-0x5C4D40):
 * empty init and program, and the effect hook: +0x46E 0 -> 1 on the first
 * tick, then ef_move_sub (per-motion sounds / dust, not ported) */
void lb_common_local_init(u8 *em) {
}

void lb_dummy_em_prog(u8 *em) {
}

void ef_move_sub_005C49F0();
void lb_npc_ef_move(u8 *em) {
    s8 k = M2C_FIELD(em, s8 *, 0x46E);
    if (k == 1) {
        ef_move_sub_005C49F0(em, em + 0x444);
    } else if (k == 0) {
        M2C_FIELD(em, s8 *, 0x46E) = 1;
    }
}

/* Lbc_set_prim (0x5B7460): the lobby's three screen prims (lb_prim, main
 * 0x3EBC70, 0x20 bytes each) get draw functions a, b, c; pNet+0x14..0x1C
 * point at them */
extern u8 lb_prim[];
void Lbc_set_prim(void *a, void *b, void *c) {
    M2C_FIELD(pNet, u8 **, 0x14) = lb_prim;
    M2C_FIELD(M2C_FIELD(pNet, u8 **, 0x14), void **, 0x14) = a;
    M2C_FIELD(pNet, u8 **, 0x18) = lb_prim + 0x20;
    M2C_FIELD(M2C_FIELD(pNet, u8 **, 0x18), void **, 0x14) = b;
    M2C_FIELD(pNet, u8 **, 0x1C) = lb_prim + 0x40;
    M2C_FIELD(M2C_FIELD(pNet, u8 **, 0x1C), void **, 0x14) = c;
    /* +0x18: the font stack the draw function prints to (Lb_guild_trans:
     * font_set_stack_no(prim+0x18)); ot5/ot6/ot7 are drawn before stacks
     * 0/1/2 (trans()), so prim i -> stack i [guess: no store to these words
     * was found in the code; on the PS2 they are set elsewhere] */
    M2C_FIELD(lb_prim, s32 *, 0x18) = 0;
    M2C_FIELD(lb_prim, s32 *, 0x38) = 1;
    M2C_FIELD(lb_prim, s32 *, 0x58) = 2;
}

/* lbc_text_lobby_trans (0x5B78F0): queue the lobby screen prims that have
 * a draw function: +0x14 on ot5, +0x18 on ot6, +0x1C on ot7, +0x20 on ot2
 * (entry 0 of 16) */
extern u8 ot2[], ot5[], ot6[], ot7[];
int add_prim2();
void lbc_text_lobby_trans(u8 *net) {
    u8 *p;

    p = M2C_FIELD(net, u8 **, 0x14);
    if (p != NULL && M2C_FIELD(p, void **, 0x14) != NULL) {
        add_prim2(ot5, p, 0, 1);
    }
    p = M2C_FIELD(net, u8 **, 0x18);
    if (p != NULL && M2C_FIELD(p, void **, 0x14) != NULL) {
        add_prim2(ot6, p, 0, 1);
    }
    p = M2C_FIELD(net, u8 **, 0x1C);
    if (p != NULL && M2C_FIELD(p, void **, 0x14) != NULL) {
        add_prim2(ot7, p, 0, 1);
    }
    p = M2C_FIELD(net, u8 **, 0x20);
    if (p != NULL && M2C_FIELD(p, void **, 0x14) != NULL) {
        add_prim2(ot2, p, 0, 0x10);
    }
}
