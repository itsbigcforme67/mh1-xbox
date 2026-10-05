/* Network connect steps (SLPM_654.95 0x002710A0-0x002712BC): dummy_00 .. session_connect. See connect_nm.c. */
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












int dummy_00(void) {
    return 1;
}

int modem_connect_wait(void) {
    return MMBB_LOGIN ? 1 : 2;
}

int session_connect_init(void) {
    COM_RET = 0;
    return 1;
}

int session_connect(void) {
    int ret = 0;

    switch (COM_R_No_0) {
    case 0:
        if (dummy_00() == 1) {
            COM_R_No_1 = 0;
            COM_R_No_0 = COM_R_No_0 + 1;
        }
        break;
    case 1:
        switch (COM_R_No_1) {
        case 0:
            if (mem_tex.x534 != 0) {
                COM_R_No_1 = 0;
                COM_R_No_0 = COM_R_No_0 + 1;
            } else {
                COM_R_No_1 = COM_R_No_1 + 1;
                all_reset();
                Net_all_reset(0);
            }
            break;
        case 1:
            if (Ncm_mmbb_spr_load() != 0) {
                COM_R_No_1 = COM_R_No_1 + 1;
            }
            break;
        case 2:
            if (Ncm_mmbb_spr_create() != 0) {
                COM_R_No_1 = 0;
                COM_R_No_0 = COM_R_No_0 + 1;
            }
            break;
        }
        break;
    case 2:
        switch (func_5B52A0()) {
        case -1:
            ret = -1;
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case 1:
            ret = 1;
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case 2:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case 3:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
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
