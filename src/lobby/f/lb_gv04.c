/* lb_gv04 - village near-match run 0x005C44A0-0x005C45B0: Lb_npc_mv. Whole file in lb_village_nm.c. */
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








/* copy 8 bytes (an ld/sd pair of two floats) without going through a float */
#define CP8(d, s) memcpy((u8 *)(d), (u8 *)(s), 8)




























/* lb_npc_effect_move (0x5C4890) is in src/lobby/lb/lbui_nm.c */


































void ef_move_sub_005C49F0();

/* Lbc_set_prim (0x5B7460): the lobby's three screen prims (lb_prim, main
 * 0x3EBC70, 0x20 bytes each) get draw functions a, b, c; pNet+0x14..0x1C
 * point at them */
extern u8 lb_prim[];

/* lbc_text_lobby_trans (0x5B78F0): queue the lobby screen prims that have
 * a draw function: +0x14 on ot5, +0x18 on ot6, +0x1C on ot7, +0x20 on ot2
 * (entry 0 of 16) */
extern u8 ot2[], ot5[], ot6[], ot7[];
int add_prim2();

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
