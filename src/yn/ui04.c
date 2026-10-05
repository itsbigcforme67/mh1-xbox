/* ui04 - yn.bin screens and text 0x0053A100-0x0053A1D0: yn_strcpy, yn_port_init, yn_key_repeat. Whole file in ui_strip_tmp.c. */
#include "yn.h"

int ADXM_ShutdownThrd();
int ADXPS2_LoadFcacheDvd();
int ADXT_Finish();
int DIntr();
int DisableDmac();
int DisableIntc();
int EIntr();
int InetIPAddrFromString();
int LoadExecPS2();
int Net_kb_input_init2();
int Net_kb_input_sub();
int RemoveDmacHandler();
int RemoveIntcHandler();
int SetFilterMode();
int SoftKey_Getstr();
int SoftKey_onoff();
int SoftKey_trans();
int YnFile_Data_load();
int flCreateTextureFromTim2_mem();
int flPADDestroy();
int flPS2DmaWait();
int flReleasePaletteHandle();
int flReleaseTextureHandle();
int flSetRenderState();
int flSin();
int flfntDraw();
int flfntInit();
int flfntLocate();
int flfntPrintf();
int flfntSetHalftype();
int flfntSetPalette();
int flfntSetSize();
void flfntSetZ(f32 z);
int net_flps0008();
int printf();
int sceCdInit();
int sceGsSyncPath();
int sceGsSyncVCallback();
int sceNetcnfifConvAuthname();
int sceSifExitCmd();
int str_stop_all();
int yn_can_sd();
int yn_cur1_sd();
int yn_cur2_sd();
int yn_dec_sd();
int yn_hard_init();
int yn_hard_select_set();
int yn_log_sd();
int yn_mc_device_check_all();
int yn_mc_format();
int yn_mc_get_current();
int yn_mc_gmfile_check();
int yn_mc_gmfile_current_set();
int yn_mc_gmfile_save();
int yn_mc_init();
int yn_mc_set_current();
int yn_mc_ynfile_check();
int yn_netcnf_dev_to_work();
int yn_netcnf_exit();
int yn_netcnf_get_filename();
int yn_netcnf_get_list();
int yn_netcnf_get_num();
int yn_netcnf_ifc_to_work();
int yn_netcnf_init1();
int yn_netcnf_init2();
int yn_netcnf_magicno_check();
int yn_netcnf_net_allload();
int yn_netcnf_pastdata_check();
int yn_netcnf_pastproxy_check();
int yn_netcnf_search_usr_name();
int yn_netcnf_set_current();
int yn_netcnf_setup_devwork();
int yn_ng_sd();
int yn_set_init();
int yn_stop_bgm();
int yn_utf8_to_sjis();
extern u8 BsProxyUrlstr[];
extern u8 BsProxyUrlstr_S[];
extern u8 D_530394[];
extern u8 D_5306B0[];
extern u8 D_5310D0[];
extern u8 DeviceWork[];
extern u8 MyDialNumber[];
extern u8 MyDialOutline[];
extern u8 MyDomain[];
extern u8 MyPassword[];
extern u8 MyUserName[];
extern u8 Psw[];
extern u8 SrvDomain[];
extern u8 Yn_temp[];
extern u8 lit_160_0053EA40[];
extern u8 lit_4084[];
extern u8 lit_4085[];
extern u8 lit_4086[];
extern u8 lit_4087[];
extern u8 lit_4118[];
extern u8 lit_4217[];
extern u8 lit_4218[];
extern u8 lit_4358[];
extern u8 lit_4372[];
extern u8 lit_4679[];
extern u8 lit_4680[];
extern u8 lit_4681[];
extern u8 lit_4682[];
extern u8 lit_4683[];
extern u8 lit_4684[];
extern u8 lit_4685[];
extern u8 lit_4686[];
extern u8 lit_4687[];
extern u8 lit_4688[];
extern u8 lit_4994[];
extern u8 lit_5208[];
extern u8 lit_5209[];
extern u8 lit_5387[];
extern u8 yn_dialog_cnt_tbl[];
extern char *yn_dialog_mes_tbl[];
extern char *yn_hard_more_mes_tbl[];
extern u8 yn_help_cnt_tbl[];
extern char *yn_help_mes_tbl[];
extern char *yn_memcard_mes_tbl[];
extern u8 yn_mess_cnt_tbl[];
extern char *yn_mess_mes_tbl[];
extern u8 yn_parts_data[];
extern u8 yn_spr_data[];
extern char *yn_title_mes_tbl[];
extern u8 yn_uv_data[];
extern s8 BsProxyUseFlag;
extern s8 MyDialType;
extern s32 MyDns1;
extern s32 MyDns2;
extern s8 MyEtherInitMode;
extern s32 MyGateway;
extern s32 MyIPAddr;
extern s32 MyNetmask;
extern s8 NdgNegoMode;
extern s8 PppRecognize;
extern s32 SrvIPAddress;
extern s16 SrvPort;
extern s8 SrvType;
extern s32 flPs2GsHandler;
extern s32 yn_r_no;
extern s32 yn_type;
extern u8 * ynw;

s32 yn_set_main();
void yn_set_exit_sub();
s32 yn_file_search();
s32 yn_auto_connect();
s32 yn_select_provider();
void yn_common_shot_cancel();
s32 yn_common_memcard_out();
void yn_title_font();
void yn_message_font_sub();
void yn_connect_font_sub();
void yn_hard_font_sub();
void yn_hard_more_font_sub();
void yn_memcard_font_sub();
void yn_id_pw_font_sub();
void yn_ipadrs_font_sub();
void yn_port_font_sub();
void yn_prname_font_sub();
void yn_adname_font_sub();
void yn_dialog_draw();
void yn_dialog_font_sub();
void yn_dialog_font_without_yesno();
void yn_dialog_font_once();
void yn_dialog_font_memcard();
void yn_dialog_font_setting();
void yn_dialog_font_ip_sub();
void yn_help_font();
void yn_printf();
void yn_set_pal();
void yn_set_size();
void yn_set_z(f32 z);
void yn_set_halftype();
s32 yn_get_halftype();
s16 yn_center_x();
int yn_strlen();
void yn_strconv();
void yn_strconv2();
void yn_strcpy();
void yn_port_init();
void yn_key_repeat();
void yn_keyboard_init();
s32 yn_keyboard_sub();
void yn_keyboard_server();
void yn_draw();
void yn_view_set();
void yn_sprite_draw_sub();
void yn_sprite_draw_each();
void yn_setup_allwork();
void yn_backup_allwork();
void yn_proxy_wk_load();
void yn_proxy_wk_save();
int yn_load_texfile();
void yn_scecom_reboot();








/*
Decompilation failure in function yn_sprite_draw:

Unable to determine jump table for jr instruction at tmpgya281qg.s line 18.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/

/*
Decompilation failure in function yn_button_draw:

Unable to determine jump table for jr instruction at tmpfh_ao2x3.s line 18.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/

/*
Decompilation failure in function yn_button_font:

Unable to determine jump table for jr instruction at tmpqoo3mo3s.s line 21.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/


/*
Decompilation failure in function yn_message_font:

Unable to determine jump table for jr instruction at tmpwqzzu240.s line 21.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/












/*
Decompilation failure in function yn_dialog_font:

Unable to determine jump table for jr instruction at tmpujrimknu.s line 23.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/




















typedef struct { s32 a; s32 b; } PR8;

/* Copy the 0x41 pairs of proxy settings (0x208 bytes) in the work area. */
#define PROXY_COPY(dst, src) \
    do { \
        PR8 *d_ = (PR8 *)(dst); \
        PR8 *s_ = (PR8 *)(src); \
        int n_ = 0x41; \
        s32 t_; \
        do { \
            n_--; \
            t_ = s_->b; \
            d_->a = s_->a; \
            s_++; \
            d_->b = t_; \
            d_++; \
        } while (n_ > 0); \
    } while (0)

typedef struct YNW_K { u8 _p[0x28]; s32 x28; u32 rep; } YNW_K;














extern s32 flPs2VIF1Control[];
extern char *args[2];



void yn_strcpy(s8 *arg0, s32 arg1) {
    if (arg1 == 0) {
        *arg0 = 0;
        return;
    }
    strcpy();
}

void yn_port_init(u16 *arg0) {
    M2C_FIELD(ynw, u16 **, 0x3C) = arg0;
    M2C_FIELD(ynw, u16 *, 0x3A) = (u16) *arg0;
    M2C_FIELD(ynw, s16 *, 0x40) = 1;
    M2C_FIELD(ynw, s8 *, 0x38) = 0;
}

/* Key repeat: x28 = pad bits to act on this frame, rep = frames held. */
void yn_key_repeat(u8 *pad) {
    ((YNW_K *)ynw)->x28 = 0;
    if (M2C_FIELD(pad, u16 *, 2) == M2C_FIELD(pad, u16 *, 0)) {
        ((YNW_K *)ynw)->rep++;
        if (((YNW_K *)ynw)->rep > 20 && ((YNW_K *)ynw)->rep > 22) {
            ((YNW_K *)ynw)->rep = 0x14;
            ((YNW_K *)ynw)->x28 = M2C_FIELD(pad, u16 *, 0);
        }
    } else {
        ((YNW_K *)ynw)->rep = 0;
    }
}
