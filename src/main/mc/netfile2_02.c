/* SLPM_654.95 0x0028A3B0-0x0028A3C4: SaveNetFile_init .. SaveNetFile_init. See netfile2_nm.c. */
#include "types.h"

#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))
extern u8 card_w[];
extern u8 CNFile[];
extern u8 net_common_w[];
extern u8 system_w[];
extern u8 *data_load_ptr;
extern s32 Last_sel_drive;
extern s32 net_sel_drive;
extern u8 zen_num[];
extern s8 err_status_ng_err;
extern s8 err_status_no_card;
extern s8 err_status_no_file;

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
int flfntLocate();
int flfntPrintf();
int flfntSetSize();
int flfntSetZ();
int func_591BE0();
int func_5E5D20();
int func_5E5D30();
int load_file_mdl();
int mc_copy_patch();
int memcpy();
int memset();
int net_joy_ok_ck_each();
int net_set_se_cur();
int net_shot_ng_ck();
int net_shot_ok_ck();
int net_swdata();
int net_yesno_operation_move();
int ran_suu();
int strcpy();
int strlen();

s32 NetFileCreate(void);
s32 Net_Icon_Data_Load(s32 arg0);
s32 SaveNetFile(void);
s32 SaveGameFileNet2(void);
void dialog_open_set(s8 arg0, s8 arg1);
void dialog_close_set(s8 arg0);
void dialog_limit_disp();
void SaveNetFile_init(void);
s32 SaveNetFile_ForLobby(void);
s32 SaveNetFileBr(void);
s32 mc_bs_chg(s16 arg0);
s32 check_data_cn_file(s32 arg0);
void check_sum_set_cn_file(u8 *arg0);
s32 save_data_load_game_for_net(s32 arg0);
s32 decode_data_for_net(u8 *arg0);
void save_data_store_sys_foe_net(void);
void encode_data_0028B4B0(u8 *arg0);
s32 NetAutoLoad(void);



































void SaveNetFile_init(void) {
    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0;
    M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
}
