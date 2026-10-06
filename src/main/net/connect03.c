/* connect03 - network connection sequence 0x002714C0-0x0027187C: connect_error. Whole file in connect_nm.c. */
#include "types.h"

extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 COM_R_No_5;
extern u8 COM_R_No_6;
extern u8 COM_R_No_7;
extern s8 COM_RET;
extern u8 MMBB_LOGIN;
extern s16 Vs_Cnt_0;
extern u8 game_w[];
typedef struct { u8 pad[0x534]; s32 x534; } MEMTEX;
extern MEMTEX mem_tex;
typedef struct { u8 pad[0x2C]; s32 x2C; } NETCW;
extern NETCW net_common_w;

int all_reset();
int Net_all_reset();
int Net_trans_set();
int Ncm_mmbb_spr_load();
int Ncm_mmbb_spr_create();
int Ncm_spr_kill();
int Ncm_spr_kill_all();
int Ncm_spr_kill_all_ex_BG();
int Ncm_spr_BG_set();
int Ncm_spr_set_diarog_b();
int Ncm_err_mssage_disp_req();
int net_shot_ok_ck();
int return_to_net_top_menu();
int func_5B52A0();
int func_5B5980();
int func_5B5D10();
int func_593CB0();
int func_5B5C50();
int func_5B6F20();
int func_5B2330();
int connect_error_sub00();
int connect_error_sub01();
int connect_error_sub02();
int connect_error_sub03();
int dummy_00();
int modem_connect_wait();












int connect_error(void) {
    int ret = 0;

    switch (COM_R_No_0) {
    case 0:
        switch (func_5B5980()) {
        case 1:
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            COM_R_No_5 = 0;
            COM_R_No_6 = 0;
            COM_R_No_7 = 0;
            COM_R_No_0 = COM_R_No_0 + 1;
            break;
        case -1:
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            COM_R_No_5 = 0;
            COM_R_No_6 = 0;
            COM_R_No_7 = 0;
            COM_R_No_0 = COM_R_No_0 + 1;
            break;
        }
        break;
    case 1:
        switch (func_5B6F20()) {
        case 1:
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            COM_R_No_5 = 0;
            COM_R_No_6 = 0;
            COM_R_No_7 = 0;
            COM_R_No_0 = COM_R_No_0 + 1;
            break;
        }
        break;
    case 2:
        switch (COM_RET) {
        case -2:
        case -3:
        case -5:
        case -0x51:
        case -0x52:
        case -0x53:
        case -0x54:
        case -0x55:
        case -0x50:
        case -0x56:
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_0++;
            break;
        case -9:
            COM_R_No_0 = COM_R_No_0 + 1;
            COM_R_No_1 = 1;
            COM_R_No_2 = 0;
            break;
        case -0xB:
            COM_R_No_0 = COM_R_No_0 + 1;
            COM_R_No_1 = 2;
            COM_R_No_2 = 0;
            break;
        case -0x32:
        case -0x11:
            COM_R_No_0++;
            COM_R_No_2 = 0;
            COM_R_No_1 = 3;
            break;
        default:
            COM_R_No_0 = 4;
            break;
        }
        break;
    case 3:
        switch (COM_R_No_1) {
        case 0:
            if (connect_error_sub00() != 0) {
                COM_R_No_0 = 0;
                ret = 3;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                COM_R_No_4 = 0;
            }
            break;
        case 1:
            if (connect_error_sub01() != 0) {
                COM_R_No_0 = 0;
                ret = 3;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                COM_R_No_4 = 0;
            }
            break;
        case 2:
            if (connect_error_sub02() != 0) {
                COM_R_No_0 = 0;
                ret = 1;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                COM_R_No_4 = 0;
            }
            break;
        case 3:
            if (connect_error_sub03() != 0) {
                ret = 2;
                COM_R_No_0 = 0;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                COM_R_No_4 = 0;
                MMBB_LOGIN = 2;
            }
            break;
        }
        break;
    case 4:
        switch (modem_connect_wait()) {
        case 1:
            ret = 2;
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case 2:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            ret = 1;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case 3:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            ret = 1;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        }
        break;
    }
    Net_trans_set(0);
    return ret;
}
