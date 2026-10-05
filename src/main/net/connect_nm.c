/* connect_nm - f_connect (SLPM_654.95 0x00271110-0x00271EA0, main.bin): network connection sequence
 * (modem/session/server connect steps and the connection error dialogs). Step machines on the byte
 * counters COM_R_No_0..7; COM_RET is the result of the last connection attempt (negative = error, see the
 * switch in connect_error). Near-match C, not built. */
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

int server_connect(void) {
    int ret = 0;

    switch (COM_R_No_0) {
    case 0:
        switch (func_5B5980()) {
        case 1:
            COM_R_No_0 = COM_R_No_0 + 1;
            break;
        case -1:
            COM_R_No_1 = 0;
            ret = -2;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_0 = 1;
            COM_R_No_4 = 0;
            break;
        }
        break;
    case 1:
        switch (func_5B5D10()) {
        case 1:
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            COM_R_No_0 = COM_R_No_0 + 1;
            break;
        case -1:
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_0 = 1;
            COM_R_No_1 = 1;
            COM_R_No_4 = 0;
            break;
        case -2:
            COM_R_No_1 = 0;
            ret = -2;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_0 = 1;
            COM_R_No_4 = 0;
            break;
        case -3:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            MMBB_LOGIN = 2;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case -4:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            ret = -1;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case -5:
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        }
        break;
    case 2:
        func_593CB0();
        switch (func_5B5C50()) {
        case 1:
            ret = 1;
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            break;
        case -1:
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_0 = 1;
            COM_R_No_1 = 1;
            COM_R_No_4 = 0;
            break;
        }
        break;
    }
    return ret;
}

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
            COM_R_No_2 = 0;
            COM_R_No_1 = 3;
            COM_R_No_0++;
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

void cnnect_err_set(int a) {
    switch (a) {
    case 0:
        COM_R_No_1 = 0;
        game_w[1] = 4;
        COM_R_No_2 = 0;
        COM_R_No_0 = 1;
        COM_R_No_3 = 0;
        COM_R_No_4 = 0;
        COM_R_No_5 = 0;
        COM_R_No_6 = 0;
        COM_R_No_7 = 0;
        break;
    }
}

int connect_error_sub00(void) {
    int ret = 0;

    switch (COM_R_No_2) {
    case 0:
        if (mem_tex.x534 != 0) {
            if (net_common_w.x2C != 0) {
                COM_R_No_2 = 2;
                Vs_Cnt_0 = 0x14;
                Ncm_spr_kill_all_ex_BG();
            }
        } else {
            COM_R_No_2 = 1;
            COM_R_No_3 = 0;
            all_reset();
            Net_all_reset(0);
        }
        break;
    case 1:
        switch (COM_R_No_3) {
        case 0:
            if (Ncm_mmbb_spr_load() != 0) {
                COM_R_No_3 = COM_R_No_3 + 1;
            }
            break;
        case 1:
            if (Ncm_mmbb_spr_create() != 0) {
                COM_R_No_3 = 0;
                Vs_Cnt_0 = 0x14;
                COM_R_No_2 = COM_R_No_2 + 1;
                Ncm_spr_BG_set();
            }
            break;
        }
        break;
    case 2:
        if (--Vs_Cnt_0 <= 0) {
            COM_R_No_2 = COM_R_No_2 + 1;
            Vs_Cnt_0 = 0x258;
            Ncm_spr_set_diarog_b();
        }
        break;
    case 3:
        switch (COM_RET) {
        case -2:
            Ncm_err_mssage_disp_req(3);
            break;
        case -3:
            Ncm_err_mssage_disp_req(4);
            break;
        case -5:
            Ncm_err_mssage_disp_req(5);
            break;
        case -0x51:
            Ncm_err_mssage_disp_req(1);
            break;
        case -0x52:
            Ncm_err_mssage_disp_req(2);
            break;
        case -0x53:
            Ncm_err_mssage_disp_req(6);
            break;
        case -0x54:
            Ncm_err_mssage_disp_req(7);
            break;
        case -0x55:
            Ncm_err_mssage_disp_req(8);
            break;
        case -0x56:
            Ncm_err_mssage_disp_req(0xC);
            break;
        }
        if (--Vs_Cnt_0 <= 0) {
            COM_R_No_2 = COM_R_No_2 + 1;
        } else if (Vs_Cnt_0 < 0x1E1) {
            if (net_shot_ok_ck(2) != 0) {
                COM_R_No_2 = COM_R_No_2 + 1;
            }
        }
        break;
    case 4:
        COM_R_No_2 = COM_R_No_2 + 1;
        return_to_net_top_menu();
        Ncm_spr_kill_all();
    case 5:
        ret = 1;
        break;
    }
    return ret;
}

int connect_error_sub01(void) {
    int ret = 0;

    switch (COM_R_No_2) {
    case 0:
        COM_R_No_2 = COM_R_No_2 + 1;
        COM_R_No_3 = 0;
        all_reset();
        Net_all_reset(0);
        break;
    case 1:
        switch (COM_R_No_3) {
        case 0:
            if (Ncm_mmbb_spr_load() != 0) {
                COM_R_No_3 = COM_R_No_3 + 1;
                Ncm_spr_kill(0x200);
            }
            break;
        case 1:
            if (Ncm_mmbb_spr_create() != 0) {
                COM_R_No_3 = 0;
                COM_R_No_2 = COM_R_No_2 + 1;
                Ncm_spr_kill(0x200);
            }
            break;
        }
        break;
    case 2:
        COM_R_No_2 = COM_R_No_2 + 1;
        return_to_net_top_menu();
        Ncm_spr_kill_all();
    case 3:
        ret = 1;
        break;
    }
    return ret;
}

int connect_error_sub02(void) {
    func_5B2330();
    return 1;
}

int connect_error_sub03(void) {
    int ret = 0;

    switch (COM_R_No_2) {
    case 0:
        COM_R_No_2 = COM_R_No_2 + 1;
        COM_R_No_3 = 0;
        all_reset();
        Net_all_reset(0);
        break;
    case 1:
        switch (COM_R_No_3) {
        case 0:
            if (Ncm_mmbb_spr_load() != 0) {
                COM_R_No_3 = COM_R_No_3 + 1;
            }
            break;
        case 1:
            if (Ncm_mmbb_spr_create() != 0) {
                COM_R_No_3 = 0;
                Vs_Cnt_0 = 0x14;
                COM_R_No_2 = COM_R_No_2 + 1;
                Ncm_spr_BG_set();
            }
            break;
        }
        break;
    case 2:
        if (--Vs_Cnt_0 <= 0) {
            COM_R_No_2 = COM_R_No_2 + 1;
            Vs_Cnt_0 = 0x258;
            Ncm_spr_set_diarog_b();
        }
        break;
    case 3:
        Ncm_err_mssage_disp_req(0xD);
        if (--Vs_Cnt_0 <= 0) {
            COM_R_No_2 = COM_R_No_2 + 1;
            Ncm_spr_kill(0x200);
        } else if (Vs_Cnt_0 < 0x1E1) {
            if (net_shot_ok_ck(2) != 0) {
                COM_R_No_2 = COM_R_No_2 + 1;
                Ncm_spr_kill(0x200);
            }
        }
        break;
    case 4:
        COM_R_No_2 = COM_R_No_2 + 1;
        Ncm_spr_kill_all();
    case 5:
        COM_RET = 0;
        ret = 1;
        break;
    }
    return ret;
}
