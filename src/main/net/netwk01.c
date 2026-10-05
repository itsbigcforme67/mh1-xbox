/* SLPM_654.95 0x0026A2F0-0x0026AA94: network_work_init .. ms_network_save_game. See netwk_nm.c. */
#include "types.h"

typedef struct NETCW {
    u8 x00;
    u8 step;            /* 0x01 */
    u8 sub;             /* 0x02 */
    u8 x03;
    s16 timer;          /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 x08;            /* 0x08 */
    s16 x0A;            /* 0x0A */
    u8 x0C;
    u8 x0D;
    u8 x0E;
    u8 pad0F;
    u8 x10;
    u8 x11;             /* 0x11 */
    u8 x12;
    u8 x13;
    u8 pad14;
    u8 x15;
    u8 pad16[0x28 - 0x16];
    u8 x28;
    u8 x29;             /* 0x29 yes/no cursor (0 yes) */
    u8 pad2A[2];
    s32 x2C;            /* 0x2C sprite request mask */
    u8 x30;
    u8 pad31[3];
    u8 x34;
    u8 x35;
    u8 x36;
    u8 x37;
    u8 x38;
    u8 x39;
    u8 x3A;
    u8 x3B;
    u8 x3C;
    u8 x3D;
    u8 x3E;
    u8 x3F;
    u8 pad40[0x79 - 0x40];
    s8 x79;
    u8 pad7A[0x94 - 0x7A];
} NETCW;
extern NETCW net_common_w;
extern u8 system_w[];
extern u8 game_w[];
typedef struct PSW {
    u16 x00, x02, x04, x06, x08, x0A, x0C, x0E, x10, x12, x14, x16, x18, x1A, x1C, x1E, x20;
} PSW;
extern PSW Psw[];
extern u8 SoftKeyWork[];
extern u8 CNFile[];
extern s8 MMBB_LOGIN;
extern s32 net_sel_drive;
extern s32 Last_sel_drive;
extern u8 *data_load_ptr;
int McActSave0Set();
int McActMain();
int McActResult();
int NetFileLoad();
int NetFileCreate();
int net_yesno_operation_move(void);
int net_shot_ok_ck(int);
int net_shot_ng_ck(void);
int net_swdata(void);
int Net_fade_execute();
int func_535310();
int func_535340();
extern int (*ms_network_jp_142[])();
void cnnect_err_set();
void Net_work_move();
void Net_trans_set();
extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 COM_R_No_5;
extern u8 COM_R_No_Disconnect;
extern s16 Vs_Cnt_0;
int dcon_task_init();
int session_connect_init();
int session_connect();
int func_5B5380();
extern u8 card_w[];
int func_59DB00();
int Lbs_se_load();
int net_bgm_set();
int Ncm_mmbb_spr_load();
int Ncm_mmbb_spr_create();
int Ncm_mssage_disp_req();
int Ncm_menu_disp_req();
void Ncm_spr_kill(int m);
void Net_McWorkInit();
int Net_fade_execute();
void Net_work_init_all();
void Net_setBGcolor();
void Net_all_reset();
void Ncm_spr_BG_set(void);
void Ncm_spr_TITLE_set(void);
void Ncm_spr_set_diarog_m(void);
void Ncm_spr_SVAE_GAME_set(void);
void Ncm_spr_kill_all(void);
void Net_fade_kill(void);
void Ncm_spr_D_MENU_set(s8 a, s8 b);
void Ncm_spr_kill2(int m);
int Net_fade_check(void);

void *memset(void *, int, int);
char *strcpy(char *, const char *);
int se_req();
int net_connect_draw();
int Fade_busy_ck();
int fade_reset();
int fade_set();
int release_texture();
int McActInit();
int Load_overlay();
int kbdExecServer();
int SoftKeyboard_move();
int SoftKeyboard_pos_set();
int SoftKeyboard_set();
int DispSoftkeyboard();
int load_file_mdl();

void SoftKey_onoff(int on);
void Ncm_spr_kill_all(void);
void Net_fade_kill(void);
void Ncm_spr_D_MENU_set(s8 a, s8 b);
void Ncm_spr_kill2(int m);

























































void network_work_init(void) {
    memset(&net_common_w, 0, 0x94);
}

int ms_network_sub(void) {
    int ret = 0;
    int r;

    r = ms_network_jp_142[net_common_w.x00]();
    if (r != 0) {
        switch (net_common_w.x00) {
        case 0:
            net_common_w.x11 = 1;
            net_common_w.x00 = 1;
            break;
        case 1:
            if (r == 1) {
                net_common_w.step = 0;
                net_common_w.x00 = 2;
                net_common_w.x11 = 0;
            } else {
                net_common_w.step = 0;
                ret = -1;
                net_common_w.x11 = 0;
                net_common_w.x00 = 0xE;
            }
            break;
        case 2:
            if (r == 1) {
                net_common_w.step = 0;
                net_common_w.x00 = 3;
                net_common_w.x11 = 0;
            } else {
                net_common_w.step = 0;
                ret = -1;
                net_common_w.x11 = 0;
                net_common_w.x00 = 0xE;
            }
            break;
        case 3:
            switch (r) {
            case 1:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 4;
                net_common_w.x10 = 0;
                break;
            case 2:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 8;
                break;
            case 3:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 7;
                break;
            default:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 0xE;
                break;
            }
            break;
        case 4:
            switch (r) {
            case 1:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 5;
                net_common_w.x0E = 3;
                break;
            default:
                net_common_w.step = 0;
                net_common_w.x11 = 0;
                net_common_w.x00 = 3;
                break;
            }
            break;
        case 5:
            if (r == 1) {
                net_common_w.x11 = 1;
                net_common_w.x00 = 6;
                net_common_w.step = 0;
                net_common_w.sub = 0;
                net_common_w.x0E = 0;
            } else {
                net_common_w.x11 = 1;
                net_common_w.step = 0;
                net_common_w.x00 = 3;
                net_common_w.x0E = 0;
            }
            break;
        case 6:
            if (r == 1) {
                net_common_w.x11 = 1;
                net_common_w.x00 = 9;
                net_common_w.step = 0;
            } else {
                cnnect_err_set(0);
            }
            break;
        case 7:
            net_common_w.step = 0;
            net_common_w.x11 = 0;
            net_common_w.x00 = 3;
            break;
        case 8:
            net_common_w.step = 0;
            net_common_w.x11 = 0;
            net_common_w.x00 = 4;
            break;
        case 9:
            net_common_w.step = 0;
            net_common_w.x00 = 0xA;
            net_common_w.x11 = 1;
            break;
        case 10:
            if (r == 1) {
                net_common_w.x11 = 1;
                net_common_w.x00 = 0xB;
                net_common_w.step = 0;
            } else {
                net_common_w.x11 = 1;
                net_common_w.step = 0;
                net_common_w.x00 = 0xC;
            }
            break;
        case 11:
            ret = 1;
            break;
        case 12:
            net_common_w.step = 0;
            net_common_w.x00 = 0xE;
            break;
        case 13:
            net_common_w.step = 0;
            net_common_w.x00 = 0xE;
            break;
        case 14:
            ret = -1;
            break;
        }
    }
    if ((net_common_w.x0E & 1) == 0) {
        Net_work_move(0);
    }
    if ((net_common_w.x0E & 2) == 0) {
        Net_trans_set(0);
    }
    return ret;
}

int ms_network_init(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        net_common_w.step++;
        Net_all_reset(0);
        Net_work_init_all();
        Net_setBGcolor(0);
        net_common_w.x11 = 1;
        func_59DB00();
        Lbs_se_load();
        break;
    case 1:
        if (Ncm_mmbb_spr_load() != 0) {
            net_common_w.x2C = 0;
            net_common_w.step++;
        }
        break;
    case 2:
        if (Ncm_mmbb_spr_create() != 0) {
            net_common_w.step = 0;
            net_common_w.sub = 0;
            net_common_w.x03 = 0;
            net_common_w.x12 = 0;
            net_common_w.x28 = 0;
            net_common_w.x15 = 0;
            net_common_w.x0C = 0;
            Net_work_init_all();
            Net_setBGcolor(0);
            net_bgm_set();
            ret = 1;
        }
        break;
    }
    return ret;
}

int ms_network_save_game(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        net_common_w.step++;
        Ncm_spr_BG_set();
        Ncm_spr_TITLE_set();
        Ncm_spr_set_diarog_m();
        net_common_w.x06 = 5;
        Net_McWorkInit(2);
        system_w[0x3C] = 1;
        net_common_w.x79 = *(s32 *)(card_w + 0x18);
        Net_fade_execute(0, 0x1E, 0);
        break;
    case 1:
        if (net_common_w.x06 > 0) {
            net_common_w.x06 = net_common_w.x06 - 1;
            if (net_common_w.x06 == 0) {
                Ncm_spr_SVAE_GAME_set();
            }
        } else {
            Ncm_mssage_disp_req(0x1F);
            Ncm_mssage_disp_req(1);
            Ncm_menu_disp_req(3);
            if (net_shot_ok_ck(1) != 0) {
                net_common_w.step = 3;
                Ncm_spr_kill(0x8000);
                Net_McWorkInit(2);
                net_common_w.timer = 0xA;
            } else if (net_shot_ng_ck() != 0) {
                Ncm_spr_kill(0x8000);
                net_common_w.timer = 0xA;
                net_common_w.step = 0xA;
                Net_fade_execute(0, 0x1E, 1);
            }
        }
        break;
    case 3:
        Ncm_mssage_disp_req(0x1F);
        if (net_common_w.timer > 0) {
            net_common_w.timer = net_common_w.timer - 1;
        }
        net_common_w.step++;
    case 4:
        system_w[0x3C] = 0;
        ret = 1;
        break;
    case 10:
        Ncm_mssage_disp_req(0x1F);
        if (Net_fade_check() == 0) {
            net_common_w.step++;
            Ncm_spr_kill_all();
    case 11:
            system_w[0x3C] = 0;
            ret = -1;
        }
        break;
    }
    return ret;
}
