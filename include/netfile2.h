#ifndef NETFILE2_H
#define NETFILE2_H
/* Shared declarations for the net save file code (src/main/mc/netfile2*.c, SLPM_654.95 0x2869A0-0x28BEC0). */
#include "types.h"
#include "netcw.h"
#include "sysw.h"

extern u8 card_w[];
extern u8 CNFile[];
extern u8 *data_load_ptr;

int Def_net_data_set();
int McActAvailSet();
int McActCheckSet();
int McActConChk();
int McActFormatSet();
int McActInit();
int McActLoadSet();
int McActMain();
int McActNewChk();
int McActResult();
int McActSave0Set();
int McActSaveSet();
int McActStopSet();
int McReadClock();
int Ncm_menu_disp_req();
int Ncm_mssage_disp_option_req();
int Ncm_mssage_disp_req();
int Ncm_spr_D_MENU_set();
int Ncm_spr_kill();
int Ncm_spr_kill2();
int Net_McWorkInit();
int check_sum_ck();
int check_sum_set();
int load_file_mdl();
int mc_copy_patch();
void *memcpy();
void *memset();
int net_joy_ok_ck_each();
int net_set_se_cur();
int net_shot_ng_ck();
int net_shot_ok_ck();
int net_swdata();
int net_yesno_operation_move();

int Net_Icon_Data_Load();
void dialog_open_set();
void dialog_close_set();
void dialog_limit_disp();
int save_data_load_game_for_net();
void save_data_store_sys_foe_net();
int decode_data_for_net();
int mc_bs_chg();
int check_data_cn_file();
extern s32 Last_sel_drive;
extern s32 net_sel_drive;
int func_591BE0();
int func_5E5D20();
int func_5E5D30();
void check_sum_set_cn_file();
#endif
