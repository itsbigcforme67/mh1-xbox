/* SLPM_654.95 0x0026B850-0x0026BB48: ms_network_bb_connect_driver .. ms_net_end_wait. See netwk_nm.c. */
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























































int ms_network_bb_connect_driver(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        switch (net_common_w.sub) {
        case 0:
            if (Ncm_mmbb_spr_load() != 0) {
                net_common_w.sub++;
            }
            break;
        case 1:
            if (Ncm_mmbb_spr_create() != 0) {
                net_common_w.sub = 0;
                net_common_w.step++;
                dcon_task_init();
                session_connect_init();
                COM_R_No_0 = 0;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                COM_R_No_5 = 0;
            }
            break;
        }
        break;
    case 1:
        switch (session_connect()) {
        case 1:
            net_common_w.step = 2;
            break;
        case -1:
            net_common_w.step = 3;
            break;
        case -2:
            net_common_w.step = 4;
            break;
        }
        break;
    case 2:
        ret = 1;
        break;
    case 3:
        COM_R_No_0 = 0;
        COM_R_No_1 = 0;
        ret = -1;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        COM_R_No_4 = 0;
        break;
    case 4:
        COM_R_No_1 = 0;
        ret = -2;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        COM_R_No_0 = 1;
        COM_R_No_4 = 0;
        break;
    }
    return ret;
}

s8 ms_network_connect_last_setting(void) {
    int v = ((s8 *)CNFile)[0xC80];

    if (v == 1) {
        net_common_w.x0C = 1;
    }
    net_common_w.x10 = 1;
    return v;
}

int ms_network_matching(void) {
    return 1;
}

int ms_network_end(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        if (Net_fade_execute(0, 0x14, 1) != 0) {
            net_common_w.step++;
        }
        break;
    case 1:
        if ((s8)Net_fade_check() == 0) {
            Net_work_init_all();
            COM_R_No_Disconnect = 0;
            Vs_Cnt_0 = 0;
            net_common_w.step++;
        }
        break;
    case 2:
        if (func_5B5380() == 0) {
            return 0;
        }
        net_common_w.step++;
    case 3:
        ret = 1;
        break;
    }
    return ret;
}

int ms_net_end_wait(void) {
    return 1;
}
