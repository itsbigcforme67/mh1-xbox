/* Memory card "net file" (online game save) functions, SLPM_654.95 main 0x2875E0-0x28BEC0: NetFileCreate (create the
 * net save file), Net_Icon_Data_Load, SaveNetFile/SaveGameFileNet2/SaveNetFile_ForLobby/SaveNetFileBr (save screens of
 * the online lobby), dialog_* (message dialog), mc_bs_chg, check/encode/decode helpers and NetAutoLoad.
 * Cleaned m2c output (online-only code, not used by the port); field offsets of net_common_w are by hex offset. */
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


s32 NetFileCreate(void) {
    s16 *var_at;
    s16 temp_v0;
    s16 temp_v0_10;
    s16 temp_v0_11;
    s16 temp_v0_12;
    s16 temp_v0_14;
    s16 temp_v0_15;
    s16 temp_v0_17;
    s16 temp_v0_18;
    s16 temp_v0_20;
    s16 temp_v0_21;
    s16 temp_v0_23;
    s16 temp_v0_24;
    s16 temp_v0_25;
    s16 temp_v0_26;
    s16 temp_v0_28;
    s16 temp_v0_29;
    s16 temp_v0_30;
    s16 temp_v0_32;
    s16 temp_v0_33;
    s16 temp_v0_35;
    s16 temp_v0_36;
    s16 temp_v0_37;
    s16 temp_v0_39;
    s16 temp_v0_3;
    s16 temp_v0_40;
    s16 temp_v0_42;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 temp_v0_7;
    s16 temp_v0_8;
    s16 temp_v0_9;
    s16 var_v0;
    s32 temp_v0_13;
    s32 temp_v0_16;
    s32 temp_v0_19;
    s32 temp_v0_22;
    s32 temp_v0_27;
    s32 temp_v0_2;
    s32 temp_v0_31;
    s32 temp_v0_34;
    s32 temp_v0_38;
    s32 temp_v0_41;
    s32 temp_v0_6;
    s32 var_s0;
    u8 temp_a1;

    temp_a1 = M2C_FIELD(&net_common_w, u8 *, 2);
    var_s0 = 0;
    switch (temp_a1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, u8 *, 2) = 0x7FU;
        M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        M2C_FIELD(&net_common_w, s16 *, 0xA) = 0;
        break;
    case 0x7F:                                      /* switch 1 */
        if (Net_Icon_Data_Load(0) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
block_323:
            *var_at = var_v0;
        }
        break;
    case 0x1:                                       /* switch 1 */
        temp_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0;
        if (((s16) temp_v0) <= 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x31) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (temp_a1 + 1);
            Net_McWorkInit(2);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
            Def_net_data_set();
            Ncm_spr_D_MENU_set(0xB, 1);
        }
        break;
    case 0x2:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x50);
        Ncm_menu_disp_req(0xB);
        temp_v0_2 = net_swdata();
        if (temp_v0_2 & 0x1000) {
            if (M2C_FIELD(&net_common_w, s16 *, 0xA) != 1) {
                net_set_se_cur();
                M2C_FIELD(&net_common_w, s16 *, 0xA) = 1;
            } else {
                net_set_se_cur();
                M2C_FIELD(&net_common_w, s16 *, 0xA) = 0;
            }
        } else if (temp_v0_2 & 0x2000) {
            if (M2C_FIELD(&net_common_w, s16 *, 0xA) != 0) {
                net_set_se_cur();
                M2C_FIELD(&net_common_w, s16 *, 0xA) = 0;
            } else {
                net_set_se_cur();
                M2C_FIELD(&net_common_w, s16 *, 0xA) = 1;
            }
        }
        if (net_shot_ok_ck(1) != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            M2C_FIELD(&net_common_w, u8 *, 0xD) = (u8) M2C_FIELD(&net_common_w, s16 *, 0xA);
            M2C_FIELD(&net_common_w, s8 *, 0x79) = (s8) M2C_FIELD(&net_common_w, s16 *, 0xA);
            M2C_FIELD(&net_common_w, s16 *, 0x7A) = McActAvailSet(data_load_ptr + 0x12000, M2C_FIELD(&net_common_w, s16 *, 0xA));
            McActSave0Set(M2C_FIELD(&net_common_w, s8 *, 0x79), data_load_ptr, 0);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 1;
        } else if (net_shot_ng_ck(0x40000) != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x32U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x3:                                       /* switch 1 */
        Ncm_mssage_disp_req(8);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_3 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_3;
            if (((s16) temp_v0_3) <= 0) {
                goto block_67;
            }
        } else {
block_67:
            McActMain();
            temp_v0_4 = McActResult();
            M2C_FIELD(&net_common_w, s16 *, 6) = temp_v0_4;
            switch (temp_v0_4) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                M2C_FIELD(&net_common_w, s8 *, 0x31) = 1;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_D_MENU_set(0, 2);
                McActCheckSet();
                break;
            case -253:                              /* switch 2 */
                M2C_FIELD(&net_common_w, s8 *, 0x31) = 0;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_D_MENU_set(0, 2);
                McActCheckSet();
                break;
            case -255:                              /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0xAU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                Ncm_spr_D_MENU_set(0, 2);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                break;
            case -254:                              /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x28U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                Ncm_spr_D_MENU_set(0, 2);
                McActCheckSet();
                break;
            case -252:                              /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x14U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                Ncm_spr_D_MENU_set(0, 2);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                break;
            case -256:                              /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x3CU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                Ncm_spr_D_MENU_set(0, 2);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                break;
            case 117:                               /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x50U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                Ncm_spr_D_MENU_set(0, 2);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                break;
            }
        }
        break;
    case 0x4:                                       /* switch 1 */
        McActMain();
        if (McActConChk(M2C_FIELD(&net_common_w, s8 *, 0x79)) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x29) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x46U;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
        } else {
            if (M2C_FIELD(&net_common_w, s8 *, 0x31) == 1) {
                Ncm_mssage_disp_req(0x54);
            } else {
                Ncm_mssage_disp_req(0x51);
            }
            Ncm_menu_disp_req(4);
            if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
                temp_v0_5 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
                M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_5;
                if (((s16) temp_v0_5) <= 0) {
                    goto block_96;
                }
            } else {
block_96:
                temp_v0_6 = net_yesno_operation_move();
                switch (temp_v0_6) {                /* switch 3; irregular */
                case 1:                             /* switch 3 */
                    M2C_FIELD(&net_common_w, s8 *, 3) = 0;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Ncm_spr_kill(0x100000);
                    memset(data_load_ptr, 0, 0x1BEC);
                    memcpy(data_load_ptr, &CNFile, 0x1BEC);
                    check_sum_set_cn_file(data_load_ptr);
                    McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
                    break;
                case -1:                            /* switch 3 */
                    M2C_FIELD(&net_common_w, u8 *, 2) = 9U;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    if (net_shot_ok_ck(0) != 0) {
                        Ncm_spr_kill(0x100000);
                    } else {
                        Ncm_spr_kill2(0x100000);
                    }
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                    break;
                }
            }
        }
        break;
    case 0x5:                                       /* switch 1 */
        Ncm_mssage_disp_req(9);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_7 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_7;
            if (((s16) temp_v0_7) <= 0) {
                goto block_110;
            }
        } else {
block_110:
            McActMain();
            temp_v0_8 = McActResult();
            switch (temp_v0_8) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                M2C_FIELD(&net_common_w, s8 *, 3) = 0;
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                break;
            case -254:                              /* switch 4 */
                M2C_FIELD(&net_common_w, s8 *, 3) = 0;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x28U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                Ncm_spr_D_MENU_set(0, 2);
                McActCheckSet();
                break;
            case 117:                               /* switch 4 */
            case -256:                              /* switch 4 */
            case -255:                              /* switch 4 */
                M2C_FIELD(&net_common_w, s8 *, 3) = 0;
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x1EU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                Ncm_spr_D_MENU_set(0, 2);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                break;
            }
        }
        break;
    case 0x6:                                       /* switch 1 */
        Ncm_mssage_disp_req(0xA);
        temp_v0_9 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_9;
        if (((s16) temp_v0_9) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) < 0x10F) {
                Ncm_mssage_disp_option_req(0xA);
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                }
            }
        } else {
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        break;
    case 0x7:                                       /* switch 1 */
        Ncm_mssage_disp_req(7);
        temp_v0_10 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_10;
        if (((s16) temp_v0_10) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xD3) {
                Ncm_mssage_disp_option_req(7);
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                }
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        break;
    case 0x8:                                       /* switch 1 */
        var_s0 = 1;
        break;
    case 0x9:                                       /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            Ncm_mssage_disp_req(0x54);
        } else {
            Ncm_mssage_disp_req(0x51);
        }
        temp_v0_11 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_11;
        if (((s16) temp_v0_11) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0xA:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x52);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_12 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_12;
            if (((s16) temp_v0_12) <= 0) {
                goto block_143;
            }
        } else {
block_143:
            temp_v0_13 = net_yesno_operation_move();
            switch (temp_v0_13) {                   /* switch 5; irregular */
            case 1:                                 /* switch 5 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 5 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0xCU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0xB:                                       /* switch 1 */
        var_s0 = -1;
        break;
    case 0xC:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x52);
        temp_v0_14 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_14;
        if (((s16) temp_v0_14) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x14:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x53);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_15 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_15;
            if (((s16) temp_v0_15) <= 0) {
                goto block_159;
            }
        } else {
block_159:
            temp_v0_16 = net_yesno_operation_move();
            switch (temp_v0_16) {                   /* switch 6; irregular */
            case 1:                                 /* switch 6 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 6 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x16U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x15:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x16:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x53);
        temp_v0_17 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_17;
        if (((s16) temp_v0_17) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x1E:                                      /* switch 1 */
        if (M2C_FIELD(&net_common_w, s8 *, 0x31) == 0) {
            Ncm_mssage_disp_req(0x55);
        } else {
            Ncm_mssage_disp_req(0x70);
        }
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_18 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_18;
            if (((s16) temp_v0_18) <= 0) {
                goto block_178;
            }
        } else {
block_178:
            temp_v0_19 = net_yesno_operation_move();
            switch (temp_v0_19) {                   /* switch 7; irregular */
            case 1:                                 /* switch 7 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 7 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x20U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x1F:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x20:                                      /* switch 1 */
        if (M2C_FIELD(&net_common_w, s8 *, 0x31) == 0) {
            Ncm_mssage_disp_req(0x55);
        } else {
            Ncm_mssage_disp_req(0x70);
        }
        temp_v0_20 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_20;
        if (((s16) temp_v0_20) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x28:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x56);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_21 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_21;
            if (((s16) temp_v0_21) <= 0) {
                goto block_197;
            }
        } else {
block_197:
            McActMain();
            if (McActConChk(M2C_FIELD(&net_common_w, s8 *, 0x79)) == 0) {
                M2C_FIELD(&net_common_w, s8 *, 0x29) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x46U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
            } else {
                temp_v0_22 = net_yesno_operation_move();
                switch (temp_v0_22) {               /* switch 8; irregular */
                case 1:                             /* switch 8 */
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Ncm_spr_kill(0x100000);
                    McActFormatSet(M2C_FIELD(&net_common_w, u8 *, 0xD));
                    break;
                case -1:                            /* switch 8 */
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x31U;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    if (net_shot_ok_ck(0) != 0) {
                        Ncm_spr_kill(0x100000);
                    } else {
                        Ncm_spr_kill2(0x100000);
                    }
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                    break;
                }
            }
        }
        break;
    case 0x29:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x57);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            var_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        McActMain();
        temp_v0_23 = McActResult();
        switch (temp_v0_23) {                       /* switch 9; irregular */
        case 0:                                     /* switch 9 */
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            McActCheckSet();
            break;
        case -256:                                  /* switch 9 */
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
            var_v0 = 0x14;
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x2DU;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x2A:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x6C);
        McActMain();
        if (McActConChk(M2C_FIELD(&net_common_w, s8 *, 0x79)) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x29) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x46U;
            var_v0 = 0x1E;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        temp_v0_24 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_24;
        if (((s16) temp_v0_24) <= 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 5U;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            memset(data_load_ptr, 0, 0x1BEC);
            memcpy(data_load_ptr, &CNFile, 0x1BEC);
            check_sum_set_cn_file(data_load_ptr);
            McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0x10E) {
            Ncm_mssage_disp_option_req(0x6C);
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&net_common_w, s8 *, 3) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 5U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                memset(data_load_ptr, 0, 0x1BEC);
                memcpy(data_load_ptr, &CNFile, 0x1BEC);
                check_sum_set_cn_file(data_load_ptr);
                McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
            }
        }
        break;
    case 0x2D:                                      /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_25 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_25;
            if (((s16) temp_v0_25) <= 0) {
                goto block_234;
            }
        } else {
block_234:
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            Ncm_spr_D_MENU_set(0, 2, 0x2D);
        }
        break;
    case 0x2E:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x6F);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_26 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_26;
            if (((s16) temp_v0_26) <= 0) {
                goto block_239;
            }
        } else {
block_239:
            temp_v0_27 = net_yesno_operation_move();
            switch (temp_v0_27) {                   /* switch 10; irregular */
            case 1:                                 /* switch 10 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 10 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x2F:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x31:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x56);
        temp_v0_28 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_28;
        if (((s16) temp_v0_28) <= 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x32:                                      /* switch 1 */
        temp_v0_29 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_29;
        if (((s16) temp_v0_29) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (temp_a1 + 1);
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
            Ncm_spr_D_MENU_set(0, 2, 0x32);
        }
        break;
    case 0x33:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x5C);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_30 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_30;
            if (((s16) temp_v0_30) <= 0) {
                goto block_257;
            }
        } else {
block_257:
            temp_v0_31 = net_yesno_operation_move();
            switch (temp_v0_31) {                   /* switch 11; irregular */
            case 1:                                 /* switch 11 */
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 11 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x34:                                      /* switch 1 */
        temp_v0_32 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_32;
        if (((s16) temp_v0_32) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (temp_a1 + 1);
        case 0x35:                                  /* switch 1 */
            var_s0 = -1;
        }
        break;
    case 0x3C:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x5B);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_33 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_33;
            if (((s16) temp_v0_33) <= 0) {
                goto block_271;
            }
        } else {
block_271:
            temp_v0_34 = net_yesno_operation_move();
            switch (temp_v0_34) {                   /* switch 12; irregular */
            case 1:                                 /* switch 12 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 12 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x3EU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x3D:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x3E:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x5B);
        temp_v0_35 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_35;
        if (((s16) temp_v0_35) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x46:                                      /* switch 1 */
        temp_v0_36 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_36;
        if (((s16) temp_v0_36) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (temp_a1 + 1);
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            M2C_FIELD(&net_common_w, s8 *, 0x29) = 1;
            Ncm_spr_D_MENU_set(0, 2, 0x3E);
        }
        break;
    case 0x47:                                      /* switch 1 */
        if (M2C_FIELD(&net_common_w, s8 *, 0x31) == 0) {
            Ncm_mssage_disp_req(0x6B);
        } else {
            Ncm_mssage_disp_req(0x71);
        }
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_37 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_37;
            if (((s16) temp_v0_37) <= 0) {
                goto block_292;
            }
        } else {
block_292:
            temp_v0_38 = net_yesno_operation_move();
            switch (temp_v0_38) {                   /* switch 13; irregular */
            case 1:                                 /* switch 13 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 13 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x49U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x48:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x49:                                      /* switch 1 */
        if (M2C_FIELD(&net_common_w, s8 *, 0x31) == 0) {
            Ncm_mssage_disp_req(0x6B);
        } else {
            Ncm_mssage_disp_req(0x71);
        }
        temp_v0_39 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_39;
        if (((s16) temp_v0_39) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    case 0x50:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x77);
        Ncm_menu_disp_req(4);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            temp_v0_40 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
            M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_40;
            if (((s16) temp_v0_40) <= 0) {
                goto block_311;
            }
        } else {
block_311:
            temp_v0_41 = net_yesno_operation_move();
            switch (temp_v0_41) {                   /* switch 14; irregular */
            case 1:                                 /* switch 14 */
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                break;
            case -1:                                /* switch 14 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x52U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                if (net_shot_ok_ck(0) != 0) {
                    Ncm_spr_kill(0x100000);
                } else {
                    Ncm_spr_kill2(0x100000);
                }
                break;
            }
        }
        break;
    case 0x51:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x52:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x77);
        temp_v0_42 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_42;
        if (((s16) temp_v0_42) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
            var_v0 = 0x14;
            var_at = (s16 *)(net_common_w + 4);
            goto block_323;
        }
        break;
    }
    return var_s0;
}


s32 Net_Icon_Data_Load(s32 arg0) {
    s32 var_s0;

    var_s0 = 0;
    switch (M2C_FIELD(&net_common_w, u8 *, 3)) {    /* irregular */
    case 0:
        M2C_FIELD(&net_common_w, u8 *, 3) = (u8) (M2C_FIELD(&net_common_w, u8 *, 3) + 1);
        if (arg0 == 0) {
            load_file_mdl(data_load_ptr + 0x12000, 0x6D4);
        } else {
            load_file_mdl(data_load_ptr + 0x12000, 0x6D3);
        }
        break;
    case 1:
        M2C_FIELD(&net_common_w, u8 *, 3) = (u8) (M2C_FIELD(&net_common_w, u8 *, 3) + 1);
        /* fallthrough */
    case 2:
        var_s0 = 1;
        break;
    }
    return var_s0;
}


s32 SaveNetFile(void) {
    s16 temp_v0_3;
    s16 temp_v0_5;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 var_s0;
    u8 temp_a1;
    u8 temp_a3;

    var_s0 = 0;
    switch (M2C_FIELD(&net_common_w, u8 *, 2)) {    /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        Net_McWorkInit(2);
        M2C_FIELD(&net_common_w, u8 *, 0xD) = (u8) net_sel_drive;
        M2C_FIELD(&net_common_w, u8 *, 0x79) = (u8) M2C_FIELD(&net_common_w, u8 *, 0xD);
        McActCheckSet();
        McActMain();
        M2C_FIELD(&system_w, s8 *, 0x3C) = 1;
        break;
    case 0x1:                                       /* switch 1 */
        if (Net_Icon_Data_Load(0) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            temp_a1 = M2C_FIELD(&net_common_w, u8 *, 2) + 1;
            M2C_FIELD(&net_common_w, u8 *, 2) = temp_a1;
            M2C_FIELD(&net_common_w, s16 *, 0x7A) = McActAvailSet(data_load_ptr + 0x12000, temp_a1);
            McActSave0Set(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr, 0);
        }
        break;
    case 0x2:                                       /* switch 1 */
        Ncm_mssage_disp_req(8);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
        } else {
            McActMain();
            temp_v0 = McActResult();
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                temp_a3 = M2C_FIELD(&net_common_w, u8 *, 2);
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (temp_a3 + 1);
                M2C_FIELD(&net_common_w, u8 *, 0x79) = (u8) M2C_FIELD(&net_common_w, u8 *, 0xD);
                memset(data_load_ptr, 0, 0x1BEC, temp_a3);
                memcpy(data_load_ptr, &CNFile, 0x1BEC);
                check_sum_set_cn_file(data_load_ptr);
                McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
                break;
            case -255:                              /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x14U;
                M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                McActCheckSet();
                break;
            case -256:                              /* switch 2 */
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x28U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                break;
            case -251:                              /* switch 2 */
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x32U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                break;
            default:                                /* switch 2 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x1EU;
                M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                McActCheckSet();
                break;
            }
        }
        break;
    case 0x3:                                       /* switch 1 */
        Ncm_mssage_disp_req(9);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
        } else {
            McActMain();
            temp_v0_2 = McActResult();
            switch (temp_v0_2) {                    /* switch 3; irregular */
            case -1:                                /* switch 3 */
                break;
            case 0:                                 /* switch 3 */
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                break;
            case -255:                              /* switch 3 */
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x14U;
                M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x1E;
                McActCheckSet();
                break;
            case -251:                              /* switch 3 */
            case -256:                              /* switch 3 */
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x28U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                break;
            }
        }
        break;
    case 0x4:                                       /* switch 1 */
        Ncm_mssage_disp_req(0xA);
        temp_v0_3 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_3;
        if (((s16) temp_v0_3) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) < 0x10F) {
                Ncm_mssage_disp_option_req(0xA);
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                }
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        break;
    case 0x5:                                       /* switch 1 */
        var_s0 = 1;
        break;
    case 0x14:                                      /* switch 1 */
        Ncm_mssage_disp_req(0xD);
        Ncm_mssage_disp_req(0xE);
        dialog_limit_disp(0);
        if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                } else {
                    goto block_62;
                }
            } else {
                M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
block_62:
                McActMain();
                temp_v0_4 = McActConChk(M2C_FIELD(&net_common_w, u8 *, 0xD));
                switch (temp_v0_4) {                /* switch 4; irregular */
                case 0:                             /* switch 4 */
                    break;
                case 3:                             /* switch 4 */
                case 2:                             /* switch 4 */
                case 1:                             /* switch 4 */
                    McActStopSet();
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x19U;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0xA;
                    break;
                }
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            McActStopSet();
            Ncm_spr_kill(0x200);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        }
        break;
    case 0x15:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x19:                                      /* switch 1 */
        temp_v0_5 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_5;
        if (((s16) temp_v0_5) > 0) {
            McActMain();
        } else {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = 2U;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            McActSave0Set(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr, 0);
        }
        break;
    case 0x1E:                                      /* switch 1 */
        Ncm_mssage_disp_req(0xF);
        Ncm_mssage_disp_req(0x10);
        dialog_limit_disp(0);
        if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x15U;
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                } else {
                    goto block_83;
                }
            } else {
                M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
block_83:
                McActMain();
                temp_v0_6 = McActConChk(M2C_FIELD(&net_common_w, u8 *, 0xD));
                switch (temp_v0_6) {                /* switch 5; irregular */
                case 3:                             /* switch 5 */
                case 1:                             /* switch 5 */
                case 2:                             /* switch 5 */
                    break;
                case 0:                             /* switch 5 */
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x23U;
                    McActCheckSet();
                    break;
                }
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x15U;
            McActStopSet();
            Ncm_spr_kill(0x200);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        }
        break;
    case 0x23:                                      /* switch 1 */
        Ncm_mssage_disp_req(0xF);
        Ncm_mssage_disp_req(0x10);
        dialog_limit_disp(0);
        if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x15U;
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                } else {
                    goto block_97;
                }
            } else {
                M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
block_97:
                McActMain();
                temp_v0_7 = McActConChk(M2C_FIELD(&net_common_w, u8 *, 0xD));
                switch (temp_v0_7) {                /* switch 6; irregular */
                case 0:                             /* switch 6 */
                    break;
                case 3:                             /* switch 6 */
                case 2:                             /* switch 6 */
                case 1:                             /* switch 6 */
                    McActStopSet();
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x19U;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0xA;
                    break;
                }
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x15U;
            McActStopSet();
            Ncm_spr_kill(0x200);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        }
        break;
    case 0x28:                                      /* switch 1 */
        Ncm_mssage_disp_req(0xB);
        Ncm_mssage_disp_req(0xC);
        dialog_limit_disp(0);
        if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x29U;
                    Ncm_spr_kill(0x200);
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                }
            } else {
                M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x29U;
            Ncm_spr_kill(0x200);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        }
        break;
    case 0x29:                                      /* switch 1 */
        var_s0 = -1;
        break;
    case 0x32:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x75);
        Ncm_mssage_disp_req(0xC);
        dialog_limit_disp(0);
        if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    M2C_FIELD(&net_common_w, u8 *, 2) = 0x33U;
                    Ncm_spr_kill(0x200);
                    M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                }
            } else {
                M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
            }
        } else {
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x33U;
            Ncm_spr_kill(0x200);
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        }
        break;
    case 0x33:                                      /* switch 1 */
        var_s0 = -1;
        break;
    }
    return var_s0;
}


s32 SaveGameFileNet2(void) {
    s16 temp_v0;
    s32 var_s0;
    s8 temp_a0;

    temp_a0 = M2C_FIELD(&net_common_w, s8 *, 0x7C);
    var_s0 = 0;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        Net_McWorkInit(0);
        memset(data_load_ptr, 0, 0x16800);
        M2C_FIELD(&system_w, s8 *, 0x3C) = 1;
        break;
    case 0x1:                                       /* switch 1 */
        Ncm_mssage_disp_req(8);
        if (Net_Icon_Data_Load(1) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
            M2C_FIELD(&net_common_w, s16 *, 0x7A) = McActAvailSet(data_load_ptr);
            McActSave0Set(M2C_FIELD(&net_common_w, s8 *, 0x79), data_load_ptr, 1);
        }
        break;
    case 0x2:                                       /* switch 1 */
        Ncm_mssage_disp_req(8);
        if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = (s16) (M2C_FIELD(&net_common_w, s16 *, 4) - 1);
        } else {
            McActMain();
            M2C_FIELD(&net_common_w, s16 *, 6) = McActResult();
            switch (M2C_FIELD(&net_common_w, s16 *, 6)) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if (McActNewChk(M2C_FIELD(&net_common_w, s8 *, 0x79)) != 0) {
                    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x14;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                } else if (save_data_load_game_for_net(0) != 0) {
                    M2C_FIELD(&net_common_w, s16 *, 6) = -0x100;
                default:                            /* switch 2 */
                    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x3C;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                } else {
                    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 3;
                    M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                }
                break;
            case -255:                              /* switch 2 */
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x1E;
                break;
            case -254:                              /* switch 2 */
            case -253:                              /* switch 2 */
            case -252:                              /* switch 2 */
                M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0xA;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                break;
            }
        }
        break;
    case 0x3:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = 4;
        save_data_store_sys_foe_net();
        McActSaveSet(M2C_FIELD(&net_common_w, s8 *, 0x79), data_load_ptr);
        break;
    case 0x4:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x48);
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 6) = McActResult();
        switch (M2C_FIELD(&net_common_w, s16 *, 6)) { /* switch 3; irregular */
        case -1:                                    /* switch 3 */
            break;
        case 0:                                     /* switch 3 */
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 5;
            decode_data_for_net((void *)data_load_ptr);
            check_sum_set(&card_w);
            break;
        default:                                    /* switch 3 */
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x3C;
            break;
        }
        break;
    case 0x5:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        /* fallthrough */
    case 0x6:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x49);
        temp_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0;
        if (((s16) temp_v0) < 0xF1) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
                if (net_shot_ok_ck(2) != 0) {
                    goto block_60;
                }
            } else {
block_60:
                M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
                M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5A;
            }
        }
        break;
    case 0xA:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        break;
    case 0xB:                                       /* switch 1 */
        Ncm_mssage_disp_req(0x4A);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5B;
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5C;
        }
        break;
    case 0x14:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        break;
    case 0x15:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x69);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5B;
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5C;
        }
        break;
    case 0x1E:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        break;
    case 0x1F:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x4B);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5B;
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5C;
        }
        break;
    case 0x3C:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a0 + 1);
        M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        break;
    case 0x3D:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x4C);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5B;
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x5C;
        }
        break;
    case 0x5A:                                      /* switch 1 */
        Ncm_mssage_disp_req(0x49);
        var_s0 = 1;
block_90:
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0;
        break;
    case 0x5B:                                      /* switch 1 */
        var_s0 = -1;
        goto block_90;
    case 0x5C:                                      /* switch 1 */
        var_s0 = -1;
        goto block_90;
    }
    return var_s0;
}


void dialog_open_set(s8 arg0, s8 arg1) {
    M2C_FIELD(&net_common_w, s8 *, 0x7D) = arg0;
    M2C_FIELD(&net_common_w, s8 *, 0x7E) = arg1;
    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x64;
    M2C_FIELD(&net_common_w, s16 *, 4) = 8;
    M2C_FIELD(&net_common_w, s16 *, 6) = 0x744;
}


void dialog_close_set(s8 arg0) {
    M2C_FIELD(&net_common_w, s8 *, 0x7D) = arg0;
    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x6E;
}


void dialog_limit_disp(arg0, arg1)
s32 arg0;
s32 arg1;
{
    int sp28;
    int *var_v0;
    s32 temp_a0;
    s32 temp_a0_3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_v0_2;
    s32 var_v0_3;
    u32 temp_a2;
    u32 temp_a2_2;
    u8 *temp_a0_2;
    u8 *temp_a2_3;

    if (M2C_FIELD(&net_common_w, s16 *, 6) > 0) {
        M2C_FIELD(&net_common_w, s16 *, 6) = (s16) (M2C_FIELD(&net_common_w, s16 *, 6) - 1);
    }
    temp_a2 = (u32) M2C_FIELD(&net_common_w, s16 *, 6) >> 0x1F;
    temp_a0 = M2C_FIELD(&net_common_w, s16 *, 6) / 60;
    temp_a2_2 = temp_a0 + temp_a2;
    temp_v1 = ((temp_a0 + temp_a2) / 10) + (temp_a2_2 >> 0x1F);
    var_v0 = &sp28;
    if (temp_v1 > 0) {
        temp_a0_2 = zen_num + (temp_v1 * 2);
        M2C_FIELD(var_v0, s8 *, 0) = (s8) M2C_FIELD(temp_a0_2, s8 *, 0);
        M2C_FIELD(var_v0, s8 *, 1) = (s8) M2C_FIELD(temp_a0_2, s8 *, 1);
        var_v0 += 2;
    }
    temp_a2_3 = zen_num + (((s32) temp_a2_2 % 10) * 2);
    M2C_FIELD(var_v0, s8 *, 0) = (s8) M2C_FIELD(temp_a2_3, s8 *, 0);
    M2C_FIELD(var_v0, s8 *, 1) = (s8) M2C_FIELD(temp_a2_3, s8 *, 1);
    M2C_FIELD(var_v0, s8 *, 2) = 0;
    flfntSetSize(0x16, 0x16, temp_a2_3, M2C_FIELD(&net_common_w, s16 *, 6));
    temp_v1_2 = 0x280 - (((s16) strlen(&sp28)) * 0xB);
    var_v0_2 = temp_v1_2 >> 1;
    if (temp_v1_2 < 0) {
        var_v0_2 = (s32) (temp_v1_2 + 1) >> 1;
    }
    flfntLocate((s16) var_v0_2, 0x160);
    temp_a0_3 = arg0 & 0xFF;
    switch (temp_a0_3) {                            /* irregular */
    case 0:
        flfntSetZ(0x44228000, temp_a0_3);
        flfntPrintf(&sp28);
        flfntSetZ(0x44548000);
        return;
    case 1:
        temp_v1_3 = 0x280 - (((s16) strlen(&sp28)) * 0xB);
        var_v0_3 = temp_v1_3 >> 1;
        if (temp_v1_3 < 0) {
            var_v0_3 = (s32) (temp_v1_3 + 1) >> 1;
        }
        flfntLocate((s16) var_v0_3, 0x14C);
        flfntSetZ(0x44228000);
        flfntPrintf(&sp28);
        flfntSetZ(0x44548000);
        return;
    case 2:
        memset((net_common_w + 0x8D), 0, 5);
        strcpy((net_common_w + 0x8D), &sp28);
        M2C_FIELD(&net_common_w, s8 *, 0x8C) = 1;
        return;
    }
}


void SaveNetFile_init(void) {
    M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0;
    M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
}


s32 SaveNetFile_ForLobby(void) {
    s16 temp_v0;
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s32 var_s0;
    s8 temp_a1;

    var_s0 = 0;
    switch (M2C_FIELD(&net_common_w, s8 *, 0x7C)) { /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        Net_McWorkInit(2);
        Last_sel_drive = M2C_FIELD(&card_w, s32 *, 0x18);
        M2C_FIELD(&net_common_w, u8 *, 0xD) = (u8) net_sel_drive;
        M2C_FIELD(&net_common_w, u8 *, 0x79) = (u8) M2C_FIELD(&net_common_w, u8 *, 0xD);
        M2C_FIELD(&card_w, s32 *, 0x18) = (s32) (s8) M2C_FIELD(&net_common_w, u8 *, 0x79);
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, &CNFile, 0x1BEC);
        dialog_open_set(1, 0x3A);
        break;
    case 0x1:                                       /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        if (Net_Icon_Data_Load(0) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 3) = 0;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            temp_a1 = M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1;
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = temp_a1;
            M2C_FIELD(&net_common_w, s16 *, 0x7A) = McActAvailSet(data_load_ptr + 0x12000, temp_a1);
            McActSave0Set(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr, 0);
        }
        break;
    case 0x2:                                       /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 0xA) = McActResult();
        switch (M2C_FIELD(&net_common_w, s16 *, 0xA)) { /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            dialog_close_set(3);
            break;
        case -251:                                  /* switch 2 */
        case -256:                                  /* switch 2 */
            dialog_close_set(0x3C);
            break;
        case -255:                                  /* switch 2 */
            dialog_close_set(0x1E);
            break;
        case -254:                                  /* switch 2 */
        case -253:                                  /* switch 2 */
        case -252:                                  /* switch 2 */
            dialog_close_set(0x46);
            break;
        }
        break;
    case 0x3:                                       /* switch 1 */
        dialog_open_set(4, 0x3B);
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, &CNFile, 0x1BEC);
        check_sum_set_cn_file(data_load_ptr);
        McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
        break;
    case 0x4:                                       /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 0xA) = McActResult();
        if (M2C_FIELD(&net_common_w, s16 *, 0xA) != -1) {
            dialog_close_set(5);
        }
        break;
    case 0x5:                                       /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 0xA) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x3C;
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        } else {
            dialog_open_set(6, 0x3C);
        }
        break;
    case 0x6:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
        /* fallthrough */
    case 0x7:                                       /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        temp_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0;
        if ((((s16) temp_v0) < 0xF1) && ((M2C_FIELD(&net_common_w, s16 *, 4) == 0) || (net_joy_ok_ck_each(0, 0x3C) != 0))) {
            dialog_close_set(0x5A);
        }
        break;
    case 0x1E:                                      /* switch 1 */
        dialog_open_set(0x1F, 0x3D);
        McActCheckSet();
        break;
    case 0x1F:                                      /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1, 0x3C);
        McActMain();
        if (McActConChk(M2C_FIELD(&net_common_w, u8 *, 0xD)) != 0) {
            dialog_close_set(0);
        } else if (net_joy_ok_ck_each(0) == 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
                goto block_65;
            }
        } else {
block_65:
            dialog_close_set(0x5C);
        }
        break;
    case 0x29:                                      /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1, 0x3C);
        McActMain();
        if (McActConChk(M2C_FIELD(&net_common_w, u8 *, 0xD)) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x1F;
        } else if (net_joy_ok_ck_each(0) == 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
                goto block_72;
            }
        } else {
block_72:
            dialog_close_set(0x5C);
        }
        break;
    case 0x33:                                      /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1, 0x3C);
        *(s8 *)0x6B2A2C = 1;
        if (net_joy_ok_ck_each(0) != 0) {
            dialog_close_set(0x5B);
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            dialog_close_set(0x5C);
        }
        break;
    case 0x3C:                                      /* switch 1 */
        dialog_open_set(0x33, 0x3E);
        break;
    case 0x46:                                      /* switch 1 */
        dialog_open_set(0x29, 0x3F);
        McActCheckSet();
        break;
    case 0x5A:                                      /* switch 1 */
        var_s0 = 1;
block_83:
        M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0;
        M2C_FIELD(&card_w, s32 *, 0x18) = (s32) Last_sel_drive;
        break;
    case 0x5B:                                      /* switch 1 */
        var_s0 = -1;
        goto block_83;
    case 0x5C:                                      /* switch 1 */
        var_s0 = -1;
        goto block_83;
    case 0x64:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        func_591BE0(M2C_FIELD(&net_common_w, s8 *, 0x7E), 0);
        break;
    case 0x65:                                      /* switch 1 */
        *(s8 *)0x6B2A2C = 1;
        temp_v0_2 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_2;
        if (((s16) temp_v0_2) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x6E:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        M2C_FIELD(&net_common_w, s16 *, 4) = 8;
        break;
    case 0x6F:                                      /* switch 1 */
        temp_v0_3 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_3;
        if (((s16) temp_v0_3) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x78:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        break;
    case 0x79:                                      /* switch 1 */
        temp_v0_4 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_4;
        if (((s16) temp_v0_4) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x82:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
        break;
    case 0x83:                                      /* switch 1 */
        temp_v0_5 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_5;
        if (((s16) temp_v0_5) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x6E;
        }
        break;
    }
    return var_s0;
}


s32 SaveNetFileBr(void) {
    s16 temp_v0;
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s32 var_s0;
    s8 temp_a1;
    s8 temp_a1_2;

    temp_a1 = M2C_FIELD(&net_common_w, s8 *, 0x7C);
    var_s0 = 0;
    switch (temp_a1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        Net_McWorkInit(2);
        M2C_FIELD(&net_common_w, s8 *, 0x8C) = 0;
        M2C_FIELD(&net_common_w, u8 *, 0xD) = (u8) net_sel_drive;
        M2C_FIELD(&net_common_w, u8 *, 0x79) = (u8) M2C_FIELD(&net_common_w, u8 *, 0xD);
        dialog_open_set(1, 0x1D);
        break;
    case 0x1:                                       /* switch 1 */
        if (Net_Icon_Data_Load(0) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
            temp_a1_2 = M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1;
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = temp_a1_2;
            M2C_FIELD(&net_common_w, s16 *, 0x7A) = McActAvailSet(data_load_ptr + 0x12000, temp_a1_2);
            McActSave0Set(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr, 0);
        }
        break;
    case 0x2:                                       /* switch 1 */
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 0xA) = McActResult();
        switch (M2C_FIELD(&net_common_w, s16 *, 0xA)) { /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x7C) + 1);
            break;
        case -251:                                  /* switch 2 */
        case -256:                                  /* switch 2 */
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x3C;
            break;
        case -255:                                  /* switch 2 */
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x1E;
            break;
        case -254:                                  /* switch 2 */
        case -253:                                  /* switch 2 */
        case -252:                                  /* switch 2 */
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x46;
            break;
        }
        M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        break;
    case 0x3:                                       /* switch 1 */
        dialog_open_set(4, 0x31);
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, &CNFile, 0x1BEC);
        check_sum_set_cn_file(data_load_ptr);
        McActSaveSet(M2C_FIELD(&net_common_w, u8 *, 0xD), data_load_ptr);
        break;
    case 0x4:                                       /* switch 1 */
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 0xA) = McActResult();
        if (M2C_FIELD(&net_common_w, s16 *, 0xA) != -1) {
            dialog_close_set(5);
        }
        break;
    case 0x5:                                       /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 0xA) != 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x3C;
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        } else {
            dialog_open_set(6, 0x32);
        }
        break;
    case 0x6:                                       /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a1 + 1);
        M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
        /* fallthrough */
    case 0x7:                                       /* switch 1 */
        temp_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0;
        if (((s16) temp_v0) < 0xF1) {
            if (M2C_FIELD(&net_common_w, s16 *, 4) != 0) {
                if (net_shot_ok_ck(2, temp_a1) != 0) {
                    goto block_59;
                }
            } else {
block_59:
                dialog_close_set(0x5A);
            }
        }
        break;
    case 0x1E:                                      /* switch 1 */
        dialog_open_set(0x1F, 0x2C);
        McActCheckSet();
        break;
    case 0x1F:                                      /* switch 1 */
        dialog_limit_disp(2, temp_a1);
        McActMain();
        if (McActConChk((s8) M2C_FIELD(&net_common_w, u8 *, 0x79)) != 0) {
            dialog_close_set(0);
        } else if (net_shot_ok_ck(2) == 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
                goto block_67;
            }
        } else {
block_67:
            dialog_close_set(0x5C);
        }
        break;
    case 0x3C:                                      /* switch 1 */
        dialog_open_set(0x3D, 0x23);
        break;
    case 0x3D:                                      /* switch 1 */
        dialog_limit_disp(2, temp_a1);
        if (net_shot_ok_ck(2) != 0) {
            dialog_close_set(0x5B);
        } else if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            dialog_close_set(0x5C);
        }
        break;
    case 0x46:                                      /* switch 1 */
        dialog_open_set(0x47, 0x72);
        McActCheckSet();
        break;
    case 0x47:                                      /* switch 1 */
        dialog_limit_disp(2, temp_a1);
        McActMain();
        if (McActConChk((s8) M2C_FIELD(&net_common_w, u8 *, 0x79)) == 0) {
            dialog_close_set(0x1F);
        } else if (net_shot_ok_ck(2) == 0) {
            if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
                goto block_80;
            }
        } else {
block_80:
            dialog_close_set(0x5C);
        }
        break;
    case 0x5A:                                      /* switch 1 */
        var_s0 = 1;
block_82:
        M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0;
        M2C_FIELD(&net_common_w, s16 *, 4) = 0;
        M2C_FIELD(&net_common_w, s16 *, 6) = 0;
        break;
    case 0x5B:                                      /* switch 1 */
        var_s0 = -1;
        goto block_82;
    case 0x5C:                                      /* switch 1 */
        var_s0 = -1;
        goto block_82;
    case 0x64:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a1 + 1);
        func_5E5D20(mc_bs_chg(M2C_FIELD(&net_common_w, s8 *, 0x7E)));
        break;
    case 0x65:                                      /* switch 1 */
        temp_v0_2 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_2;
        if (((s16) temp_v0_2) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x6E:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a1 + 1);
        M2C_FIELD(&net_common_w, s16 *, 4) = 8;
        func_5E5D30(temp_a1);
        M2C_FIELD(&net_common_w, s8 *, 0x7E) = 0;
        break;
    case 0x6F:                                      /* switch 1 */
        temp_v0_3 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_3;
        if (((s16) temp_v0_3) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x78:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a1 + 1);
        break;
    case 0x79:                                      /* switch 1 */
        temp_v0_4 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_4;
        if (((s16) temp_v0_4) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) M2C_FIELD(&net_common_w, s8 *, 0x7D);
            M2C_FIELD(&net_common_w, s8 *, 0x7D) = 0;
        }
        break;
    case 0x82:                                      /* switch 1 */
        M2C_FIELD(&net_common_w, s8 *, 0x7C) = (s8) (temp_a1 + 1);
        M2C_FIELD(&net_common_w, s8 *, 0x7E) = 0;
        break;
    case 0x83:                                      /* switch 1 */
        temp_v0_5 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_5;
        if (((s16) temp_v0_5) == 0) {
            M2C_FIELD(&net_common_w, s8 *, 0x7C) = 0x6E;
        }
        break;
    }
    return var_s0;
}

s32 mc_bs_chg(s16 arg0) {
    s8 temp_a0;

    temp_a0 = (s8) arg0;
    switch (temp_a0) {                              /* irregular */
    case 0x1D:
        return 0x14;
    case 0x31:
        return 0x28;
    case 0x32:
        return 0x29;
    case 0x2C:
        return -0xA;
    case 0x23:
        return -0x15;
    case 0x72:
        return -0x17;
    default:
        return 0;
    }
}

s32 check_data_cn_file(s32 arg0) {
    s32 var_v1;
    s8 temp_v0;

    var_v1 = 0;
loop_1:
    temp_v0 = M2C_FIELD((arg0 + var_v1), s8 *, 0xC7D);
    if ((temp_v0 < 0) || (temp_v0 >= 3)) {
        return -1;
    }
    var_v1 += 1;
    if (var_v1 >= 0x10) {
        return 0;
    }
    goto loop_1;
}

void check_sum_set_cn_file(u8 *arg0) {
    s32 i = 0;
    s8 v;
    u8 *p;

    do {
        p = arg0 + i;
        v = M2C_FIELD(p, s8 *, 0xC7D);
        if (v < 0 || v > 2) {
            M2C_FIELD(p, s8 *, 0xC7D) = 0;
        }
        i += 1;
    } while (i < 0x10);
}


s32 save_data_load_game_for_net(s32 arg0) {
    if (decode_data_for_net((void *)data_load_ptr) != 0) {
        return -1;
    }
    if (check_sum_ck(&card_w) == 0) {
        return -1;
    }
    if (arg0 & 0xFF) {
        mc_copy_patch(0);
    }
    return 0;
}

/* Image layout (same scrambling as the save image, see mcsave_nm.c): u16 version 0x100, u16 key seed, u16 checksum,
 * u16 0x5963, then 0x8A20 u16 words xored with the key stream key = key * 0xB0 % 65363. Returns 0 ok, -1 bad. */
s32 decode_data_for_net(u8 *arg0)
{
    u16 *buf = (u16 *)arg0;
    u16 key;
    u16 stored;
    int sum = 0;
    int i = 0;

    if (buf[0] != 0x100) {
        return -1;
    }
    key = buf[1];
    stored = buf[2];
    buf += 4;
    do {
        *buf ^= key;
        sum = (sum + *buf) & 0xFFFF;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        i++;
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
    } while (i < 0x8A20);
    return -((stored & 0xFFFF) != (sum & 0xFFFF));
}


void save_data_store_sys_foe_net(void) {
    u8 *temp_s0;

    temp_s0 = data_load_ptr;
    McReadClock(temp_s0 + 8);
    mc_copy_patch(1);
    encode_data_0028B4B0(temp_s0);
}


void encode_data_0028B4B0(u8 *arg0)
{
    u16 *buf = (u16 *)arg0;
    int key = (s16)ran_suu(0) & 0xFFFF;
    int i = 0;

    buf[0] = 0x100;
    buf[1] = key;
    buf[2] = 0;
    buf[3] = 0x5963;
    buf += 4;
    do {
        ((u16 *)arg0)[2] += *buf;
        *buf ^= key;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        i++;
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
    } while (i < 0x8A20);
}


s32 NetAutoLoad(void) {
    s16 temp_v0;
    s16 temp_v0_10;
    s16 temp_v0_11;
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 temp_v0_6;
    s16 temp_v0_7;
    s16 temp_v0_8;
    s16 temp_v0_9;
    s32 var_s0;

    var_s0 = 0;
    switch (M2C_FIELD(&net_common_w, u8 *, 2)) {    /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
        M2C_FIELD(&net_common_w, s8 *, 3) = 0;
        err_status_no_card = 0;
        M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        err_status_no_file = 0;
        M2C_FIELD(&net_common_w, s8 *, 0x79) = 0;
        err_status_ng_err = 0;
        McActInit(0);
        M2C_FIELD(&system_w, s8 *, 0x3C) = 1;
block_118:
    default:                                        /* switch 1 */
        return var_s0;
    case 1:                                         /* switch 1 */
        Ncm_mssage_disp_req(0x5F);
        temp_v0 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0;
        if (((s16) temp_v0) > 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            McActSave0Set(M2C_FIELD(&net_common_w, s8 *, 0x79), data_load_ptr, 1);
        }
        goto block_118;
    case 2:                                         /* switch 1 */
        Ncm_mssage_disp_req(0x5F);
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 6) = McActResult();
        switch (M2C_FIELD(&net_common_w, s16 *, 6)) { /* switch 2; irregular */
        case 0:                                     /* switch 2 */
        case -251:                                  /* switch 2 */
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            /* fallthrough */
        case -1:                                    /* switch 2 */
            return 0;
        case -255:                                  /* switch 2 */
            err_status_no_card |= (s8) (1 << M2C_FIELD(&net_common_w, s8 *, 0x79));
        default:                                    /* switch 2 */
block_37:
            if (M2C_FIELD(&net_common_w, s8 *, 0x79) == 0) {
                M2C_FIELD(&net_common_w, s8 *, 0x79) = (s8) (M2C_FIELD(&net_common_w, s8 *, 0x79) + 1);
                M2C_FIELD(&net_common_w, u8 *, 2) = 1U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0xA;
            } else if (err_status_no_card == 3) {
                M2C_FIELD(&net_common_w, u8 *, 2) = 0xAU;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            } else if ((err_status_no_card | err_status_no_file) == 3) {
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x14U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            } else {
                M2C_FIELD(&net_common_w, u8 *, 2) = 0x28U;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
                M2C_FIELD(&net_common_w, s8 *, 0x79) = (s8) ((err_status_ng_err & 1) == 0);
            }
            goto block_118;
        case -252:                                  /* switch 2 */
        case -253:                                  /* switch 2 */
        case -254:                                  /* switch 2 */
            err_status_no_file |= (s8) (1 << M2C_FIELD(&net_common_w, s8 *, 0x79));
            goto block_37;
        case -256:                                  /* switch 2 */
            err_status_ng_err |= (s8) (1 << M2C_FIELD(&net_common_w, s8 *, 0x79));
            goto block_37;
        }
        break;
    case 3:                                         /* switch 1 */
        Ncm_mssage_disp_req(0x60);
        temp_v0_2 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_2;
        if (((s16) temp_v0_2) > 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            McActLoadSet(M2C_FIELD(&net_common_w, s8 *, 0x79), data_load_ptr);
        }
        goto block_118;
    case 4:                                         /* switch 1 */
        McActMain();
        M2C_FIELD(&net_common_w, s16 *, 6) = McActResult();
        switch (M2C_FIELD(&net_common_w, s16 *, 6)) { /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            if (save_data_load_game_for_net(1) == 0) {
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
                Last_sel_drive = (s32) M2C_FIELD(&net_common_w, s8 *, 0x79);
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            case -1:                                /* switch 3 */
                return 0;
            }
        case -256:                                  /* switch 3 */
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x1EU;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            goto block_118;
        case -255:                                  /* switch 3 */
            M2C_FIELD(&net_common_w, u8 *, 2) = 0xAU;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            goto block_118;
        case -252:                                  /* switch 3 */
        case -253:                                  /* switch 3 */
        case -254:                                  /* switch 3 */
            M2C_FIELD(&net_common_w, u8 *, 2) = 0x14U;
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x14;
            goto block_118;
        }
        break;
    case 5:                                         /* switch 1 */
        Ncm_mssage_disp_req(0x61);
        temp_v0_3 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_3;
        if (((s16) temp_v0_3) <= 0) {
            M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xF0) {
            Ncm_mssage_disp_option_req(0x61);
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            }
        }
        goto block_118;
    case 6:                                         /* switch 1 */
        var_s0 = 1;
        goto block_118;
    case 10:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x64);
        temp_v0_4 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_4;
        if (((s16) temp_v0_4) <= 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        goto block_118;
    case 11:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x64);
        temp_v0_5 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_5;
        if (((s16) temp_v0_5) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xF0) {
            Ncm_mssage_disp_option_req(0x64);
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            }
        }
        goto block_118;
    case 12:                                        /* switch 1 */
        M2C_FIELD(&system_w, s8 *, 0x3C) = 0;
        var_s0 = -1;
        goto block_118;
    case 20:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x63);
        temp_v0_6 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_6;
        if (((s16) temp_v0_6) <= 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        goto block_118;
    case 21:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x63);
        temp_v0_7 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_7;
        if (((s16) temp_v0_7) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xF0) {
            Ncm_mssage_disp_option_req(0x63);
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            }
        }
        goto block_118;
    case 22:                                        /* switch 1 */
        var_s0 = -1;
        goto block_118;
    case 30:                                        /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            Ncm_mssage_disp_req(0x78);
        } else {
            Ncm_mssage_disp_req(0x62);
        }
        temp_v0_8 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_8;
        if (((s16) temp_v0_8) <= 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        goto block_118;
    case 31:                                        /* switch 1 */
        if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
            Ncm_mssage_disp_req(0x78);
        } else {
            Ncm_mssage_disp_req(0x62);
        }
        temp_v0_9 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_9;
        if (((s16) temp_v0_9) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xF0) {
            if (M2C_FIELD(&net_common_w, s16 *, 6) == 0) {
                Ncm_mssage_disp_option_req(0x78);
            } else {
                Ncm_mssage_disp_option_req(0x62);
            }
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            }
        }
        goto block_118;
    case 32:                                        /* switch 1 */
        var_s0 = -1;
        goto block_118;
    case 40:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x6A);
        temp_v0_10 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_10;
        if (((s16) temp_v0_10) <= 0) {
            M2C_FIELD(&net_common_w, s16 *, 4) = 0x12C;
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        }
        goto block_118;
    case 41:                                        /* switch 1 */
        Ncm_mssage_disp_req(0x6A);
        temp_v0_11 = M2C_FIELD(&net_common_w, s16 *, 4) - 1;
        M2C_FIELD(&net_common_w, s16 *, 4) = temp_v0_11;
        if (((s16) temp_v0_11) <= 0) {
            M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
        } else if (M2C_FIELD(&net_common_w, s16 *, 4) < 0xF0) {
            Ncm_mssage_disp_option_req(0x6A);
            if (net_shot_ok_ck(2) != 0) {
                M2C_FIELD(&net_common_w, u8 *, 2) = (u8) (M2C_FIELD(&net_common_w, u8 *, 2) + 1);
            }
        }
        goto block_118;
    case 42:                                        /* switch 1 */
        var_s0 = -1;
        goto block_118;
    }
}


