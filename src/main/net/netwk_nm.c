/* netwk_nm - f_network_work_init (SLPM_654.95 0x0026A2E8-0x0026CD60, main.bin): network menu work (net_common_w
 * 0x94 bytes), pad helpers (net_*), fade/overlay glue and the Ncm_spr_* sprite request bits (net_common_w.x2C
 * is the sprite request mask: 1 BG, 2 title, 4 message, 0x40/0x80/0x100 menu, 0x200 dialog, 0x800 connect
 * animation, 0x1000 progress bar, 0x2000/0x4000/0x1000000 DNAS, 0x8000 save game, 0x40000-0x100000 dialog
 * menu, 0x400000/0x800000 menu released, 0x2000000 DNAS error). Near-match C, not built. */
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
int SoftKeyboard_pos_set(f32, s16);
int SoftKeyboard_set();
int DispSoftkeyboard();
int load_file_mdl();

void SoftKey_onoff(int on);
void Ncm_spr_kill_all(void);
void Net_fade_kill(void);
void Ncm_spr_D_MENU_set(s8 a, s8 b);
void Ncm_spr_kill2(int m);

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

int ms_network_net_file(void) {
    int ret = 0;
    int r;
    s16 t;

    {
        switch (net_common_w.step) {
        case 0:
            Ncm_mssage_disp_req(0x1F);
            if (net_common_w.timer == 0) {
                system_w[0x3C] = 1;
                net_common_w.step++;
                Net_McWorkInit(2);
                net_sel_drive = 0;
                net_common_w.x06 = 5;
            } else {
                net_common_w.timer = net_common_w.timer - 1;
            }
            break;
        case 2:
            Ncm_mssage_disp_req(0x1F);
            Ncm_mssage_disp_req(2);
        case 5:
            if (net_common_w.timer == 0) {
                net_common_w.step++;
            } else {
                net_common_w.timer = net_common_w.timer - 1;
            }
            break;
        case 1:
            Ncm_mssage_disp_req(0x1F);
            Ncm_mssage_disp_req(2);
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t <= 0) {
                switch (net_common_w.sub) {
                case 0:
                    net_common_w.sub++;
                    net_common_w.x08 = 0;
                    net_common_w.x0A = 0;
                    net_common_w.x0D = 0;
                    net_common_w.x79 = 0;
                    Net_McWorkInit(2);
                    net_sel_drive = 0;
                    McActSave0Set(0, data_load_ptr, 0);
                    break;
                case 1:
                    McActMain(2);
                    r = McActResult();
                    if (r != -1) {
                        if (r == 0) {
                            net_common_w.sub = 0;
                            net_common_w.step = 3;
                            net_common_w.x06 = 0xA;
                            net_common_w.x0D = 0;
                            net_common_w.x79 = 0;
                            Last_sel_drive = net_common_w.x0D;
                        } else {
                            net_common_w.x08 = r;
                            net_common_w.sub++;
                            McActSave0Set(1, data_load_ptr, 0);
                        }
                    }
                    break;
                case 2:
                    McActMain(2);
                    r = McActResult();
                    if (r != -1) {
                        if (r == 0) {
                            net_common_w.sub = 0;
                            net_common_w.step = 3;
                            net_common_w.x0D = 1;
                            net_common_w.x79 = 1;
                            net_common_w.x06 = 0xA;
                            system_w[0x3C] = 0;
                            Last_sel_drive = net_common_w.x0D;
                        } else {
                            net_common_w.x0A = r;
                            net_common_w.sub++;
                        }
                    }
                    break;
                case 3:
                    if (net_common_w.x08 == -0xFF && net_common_w.x0A == -0xFF) {
                        net_common_w.sub = 0;
                        net_common_w.step = 0xA;
                        net_common_w.x06 = 0xA;
                        system_w[0x3C] = 0;
                        break;
                    }
                    net_common_w.sub = 0;
                    system_w[0x3C] = 0;
                    net_common_w.step = 0x14;
                    net_common_w.x06 = 0xA;
                    break;
                }
            }
            break;
        case 3:
            Ncm_mssage_disp_req(0x1F);
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t == 0) {
                net_common_w.x29 = 0;
                net_common_w.step = 4;
                net_common_w.x06 = 0x14;
                Ncm_spr_D_MENU_set(0, 2);
            }
            break;
        case 4:
            Ncm_mssage_disp_req(0x1F);
            Ncm_mssage_disp_req(3);
            Ncm_menu_disp_req(4);
            if (net_common_w.x06 > 0) {
                net_common_w.x06 = net_common_w.x06 - 1;
            } else {
                r = net_yesno_operation_move();
                switch (r) {
                case 0:
                    break;
                case 1:
                    net_common_w.step = 6;
                    net_common_w.timer = 0x14;
                    Ncm_spr_kill(0x100000);
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Net_McWorkInit(2);
                    system_w[0x3C] = 1;
                    break;
                case -1:
                    net_common_w.step = 0xD;
                    net_common_w.x06 = 0x14;
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    if (net_shot_ok_ck(0) != 0) {
                        Ncm_spr_kill(0x100000);
                    } else {
                        Ncm_spr_kill2(0x100000);
                    }
                    Net_fade_execute(0, 0x14, 1);
                    break;
                }
            }
            break;
        case 6:
            if (net_common_w.timer == 0) {
                net_common_w.step++;
            } else {
                net_common_w.timer = net_common_w.timer - 1;
            }
            break;
        case 7:
            Ncm_mssage_disp_req(0x1F);
            r = NetFileLoad();
            switch (r) {
            case 0:
                break;
            case 1:
                system_w[0x3C] = 0;
                net_sel_drive = net_common_w.x0D;
                net_common_w.step++;
                break;
            case -1:
                net_common_w.x03 = 0;
                net_common_w.step = 0xD;
                Net_fade_execute(0, 0x14, 1);
                system_w[0x3C] = 0;
                break;
            }
            break;
        case 8:
            net_common_w.step = 0x64;
            net_common_w.timer = 0xA;
            Net_fade_execute(0, 0x14, 1);
            break;
        case 0xA:
            Ncm_mssage_disp_req(0x1F);
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t == 0) {
                net_common_w.x29 = 1;
                net_common_w.x06 = 0xA;
                net_common_w.step++;
                Ncm_spr_D_MENU_set(0, 2);
            }
            break;
        case 0xB:
            Ncm_mssage_disp_req(0x1F);
            Ncm_mssage_disp_req(4);
            Ncm_menu_disp_req(4);
            if (net_common_w.x06 > 0) {
                net_common_w.x06 = net_common_w.x06 - 1;
            } else {
                r = net_yesno_operation_move();
                switch (r) {
                case 0:
                    break;
                case 1:
                    net_common_w.x06 = 0xA;
                    net_common_w.step += 2;
                    Ncm_spr_kill(0x100000);
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Net_fade_execute(0, 0x1E, 1);
                    break;
                case -1:
                    net_common_w.x06 = 0xA;
                    net_common_w.step++;
                    if (net_shot_ok_ck(0) != 0) {
                        Ncm_spr_kill(0x100000);
                    } else {
                        Ncm_spr_kill2(0x100000);
                    }
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    break;
                }
            }
            break;
        case 0xC:
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t == 0) {
                net_common_w.step = 0;
            }
            break;
        case 0xD:
            if (Net_fade_check() == 0) {
                net_common_w.step++;
                Ncm_spr_kill_all();
        case 0xE:
                net_common_w.step = 0;
                ret = -1;
            }
            break;
        case 0x14:
            Ncm_mssage_disp_req(0x1F);
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t == 0) {
                net_common_w.x29 = 0;
                net_common_w.x06 = 0xA;
                net_common_w.step++;
                Ncm_spr_D_MENU_set(0, 2);
            }
            break;
        case 0x15:
            Ncm_mssage_disp_req(5);
            Ncm_menu_disp_req(4);
            Ncm_mssage_disp_req(0x1F);
            if (net_common_w.x06 > 0) {
                net_common_w.x06 = net_common_w.x06 - 1;
            } else {
                r = net_yesno_operation_move();
                switch (r) {
                case 0:
                    break;
                case 1:
                    net_common_w.step++;
                    Ncm_spr_kill(0x100000);
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Net_fade_execute(0, 0x1E, 1);
                    break;
                case -1:
                    net_common_w.x06 = 0x14;
                    net_common_w.step = 0xD;
                    if (net_shot_ok_ck(0) != 0) {
                        Ncm_spr_kill(0x100000);
                    } else {
                        Ncm_spr_kill2(0x100000);
                    }
                    Ncm_spr_kill(0x40000);
                    Ncm_spr_kill(0x80000);
                    Net_fade_execute(0, 0x14, 1);
                    break;
                }
            }
            break;
        case 0x16:
            Ncm_mssage_disp_req(0x1F);
            if (Net_fade_check() == 0) {
                net_common_w.step++;
                Net_McWorkInit(2);
                system_w[0x3C] = 1;
                Net_fade_kill();
            }
            break;
        case 0x17:
            Ncm_mssage_disp_req(0x1F);
            r = NetFileCreate();
            switch (r) {
            case 0:
                break;
            case 1:
                system_w[0x3C] = 0;
                net_sel_drive = net_common_w.x0D;
                net_common_w.step++;
                break;
            case -1:
                net_common_w.step = 0xD;
                net_common_w.x06 = 0xA;
                Net_fade_execute(0, 0x14, 1);
                system_w[0x3C] = 0;
                break;
            }
            break;
        case 0x18:
            net_common_w.step++;
            break;
        case 0x19:
            if (Net_fade_check() == 0) {
                net_common_w.step = 8;
            }
            break;
        case 0x5A:
            Ncm_mssage_disp_req(0x1F);
            net_common_w.x06 = 5;
            net_common_w.step++;
            break;
        case 0x5B:
            Ncm_mssage_disp_req(0x1F);
            if (net_common_w.x06 > 0) {
                net_common_w.x06 = net_common_w.x06 - 1;
            } else {
                Ncm_mssage_disp_req(6);
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.x06 = 0xA;
                    net_common_w.step++;
                }
            }
            break;
        case 0x5C:
            Ncm_mssage_disp_req(0x1F);
            t = net_common_w.x06 - 1;
            net_common_w.x06 = t;
            if (t == 0) {
                net_common_w.step = 0;
            }
            break;
        case 0x64:
        case 0x65:
        case 0x66:
            Ncm_mssage_disp_req(0x1F);
            if (Net_fade_check() == 0) {
                net_common_w.step = 0;
                ret = 1;
                net_common_w.timer = 0xA;
                Net_work_init_all();
                Ncm_spr_kill_all();
                system_w[0x3C] = 0;
            }
            break;
        }
    }
    return ret;
}

int ms_network_yn_file(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        net_common_w.step++;
        Net_all_reset(0);
        Net_work_init_all();
        Net_fade_kill();
        game_w[0x1DC] = 0;
        Load_overlay(4, 1);
        break;
    case 1:
        net_common_w.step++;
        func_535310(net_common_w.x10);
        break;
    case 2:
        switch (func_535340()) {
        case 0:
            break;
        case 2:
            net_common_w.x28 = 1;
        case 1:
            net_common_w.step = 3;
            fade_set(0xA);
            break;
        case -1:
            net_common_w.step = 5;
            fade_set(0xA);
            break;
        }
        break;
    case 3:
    case 5:
        if ((Fade_busy_ck() & 0xFF) == 2) {
            net_common_w.step++;
            Load_overlay(3, 1);
            game_w[0x1DC] = 1;
        }
        break;
    case 4:
        Net_fade_kill();
        ret = 1;
        break;
    case 6:
        Net_fade_kill();
        ret = -1;
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

void network_work_init(void) {
    memset(&net_common_w, 0, 0x94);
}

int ms_net_end_wait(void) {
    return 1;
}

void return_to_net_top_menu(void)
{
  net_common_w.sub = (net_common_w.step = 0);
  MMBB_LOGIN = 0;
  net_common_w.x00 = 3;
  net_common_w.x11 = 1;
  net_common_w.x03 = 0;
  net_common_w.x0E = 0;
  net_common_w.x10 = 0;
  net_common_w.x13 = 0;
  net_common_w.x12 = 0;
  net_common_w.x15 = 0;
}

void net_set_se_cur(void) {
    se_req(7, 0x17, 0);
}

int net_swdata(void) {
    int sw = 0;
    int o;
    int f = system_w[0];

    if (f & 1) {
        o = game_w[0x20];
        sw = 0 | (Psw[o].x04 | Psw[o].x18);
    }
    if (f & 2) {
        o = game_w[0x21];
        sw |= Psw[o].x04 | Psw[o].x18;
    }
    return sw;
}

int net_swdata3(s8 p) {
    int sw = 0;
    int idx;

    if (system_w[0] & (1 << p)) {
        idx = game_w[0x20 + p];
        sw = 0 | Psw[idx].x00;
    }
    return sw;
}

int net_joy_ok_ck_each(s8 p) {
    return (Psw[game_w[0x20 + p]].x04 & 0x20) != 0;
}

int net_joy_cancel_ck_each(s8 p) {
    return (Psw[game_w[0x20 + p]].x04 & 0x40) != 0;
}

int net_shot_ok_ck(int kind) {
    int r = 0;

    if ((system_w[0] & 1) && net_joy_ok_ck_each(0) != 0) {
        switch (kind) {
        case 0:
            break;
        case 1:
            se_req(7, 0x13, 0);
            break;
        case 2:
            se_req(7, 9, 0);
            break;
        }
        r = 1;
    }
    if ((system_w[0] & 2) && net_joy_ok_ck_each(1) != 0) {
        switch (kind) {
        case 0:
            break;
        case 1:
            se_req(7, 0x13, 0);
            break;
        case 2:
            se_req(7, 9, 0);
            break;
        }
        r = 1;
    }
    return r;
}

int net_shot_ng_ck(void) {
    int r = 0;

    if ((system_w[0] & 1) && net_joy_cancel_ck_each(0) != 0) {
        se_req(7, 0x14, 0);
        r = 1;
    }
    if ((system_w[0] & 2) && net_joy_cancel_ck_each(1) != 0) {
        se_req(7, 0x14, 0);
        r = 1;
    }
    return r;
}

int net_yesno_operation_move(void) {
    int sw = net_swdata();

    if (sw & 0x3000) {
        if ((sw & 0x2000) && net_common_w.x29 != 0) {
            se_req(7, 0x17, 0);
            net_common_w.x29 = 0;
        }
        if ((sw & 0x1000) && net_common_w.x29 == 0) {
            se_req(7, 0x17, 0);
            net_common_w.x29 = 1;
        }
    }
    if (net_shot_ok_ck(1) != 0) {
        return (net_common_w.x29 != 0) ? -1 : 1;
    }
    return -(net_shot_ng_ck() != 0);
}

void Net_work_move() {
    /* empty */
}

void Net_trans_set() {
    net_connect_draw();
}

int Net_fade_check(void) {
    switch (Fade_busy_ck() & 0xFF) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 0;
    }
}

void Net_fade_kill(void) {
    fade_reset();
}

int Net_fade_execute(int a, int b, int mode) {
    if ((s16)mode == 1) {
        fade_set(1);
    } else {
        fade_set(2);
    }
    return 1;
}

void Net_all_reset() {
    Ncm_spr_kill_all();
    release_texture(0x14D, 8);
}

void Net_work_init_all() {
    /* empty */
}

void Net_setBGcolor() {
    /* empty */
}

void Net_McWorkInit() {
    McActInit();
}

void Net_demo_camera_set(void) {
    /* empty */
}

void Ncm_spr_kill_all(void) {
    net_common_w.x2C = 0;
}

void Ncm_spr_kill_all_ex_BG(void) {
    net_common_w.x2C = 1;
}

void Ncm_spr_kill(int m) {
    net_common_w.x2C = net_common_w.x2C & ~m;
    if (m == 0x100) {
        net_common_w.x3E = 0;
        net_common_w.x3F = 1;
        net_common_w.x2C = net_common_w.x2C | 0x400000;
    } else if (m == 0x100000) {
        net_common_w.x3E = 0;
        net_common_w.x3F = 1;
        net_common_w.x2C = net_common_w.x2C | 0x800000;
    }
}

void Ncm_spr_kill2(int m) {
    net_common_w.x2C = net_common_w.x2C & ~m;
}

void Ncm_spr_BG_set(void) {
    net_common_w.x2C = net_common_w.x2C | 1;
}

void Ncm_spr_TITLE_set(void) {
    net_common_w.x2C = net_common_w.x2C | 2;
}

void Ncm_spr_set_diarog_b(void) {
    net_common_w.x30 = 0;
    net_common_w.x2C = net_common_w.x2C | 0x200;
}

void Ncm_spr_set_diarog_m(void) {
    net_common_w.x2C = net_common_w.x2C | 0x200;
    net_common_w.x30 = 1;
}

void Ncm_spr_set_diarog_s(void) {
    net_common_w.x2C = net_common_w.x2C | 0x200;
    net_common_w.x30 = 2;
}

void Ncm_spr_MESS_SET(void) {
    net_common_w.x2C = net_common_w.x2C | 4;
}

void Ncm_spr_MENU_SET(s8 a, s8 b) {
    net_common_w.x34 = b;
    net_common_w.x35 = a;
    net_common_w.x2C = net_common_w.x2C | 0x40;
    net_common_w.x2C = net_common_w.x2C | 0x80;
    net_common_w.x2C = net_common_w.x2C | 0x100;
}

void Ncm_spr_D_MENU_set(s8 a, s8 b) {
    net_common_w.x36 = b;
    net_common_w.x37 = a;
    net_common_w.x2C = net_common_w.x2C | 0x40000;
    net_common_w.x2C = net_common_w.x2C | 0x80000;
    net_common_w.x2C = net_common_w.x2C | 0x100000;
}

void Ncm_spr_CON_AN_set(void) {
    net_common_w.x38 = 0;
    net_common_w.x39 = 0;
    net_common_w.x3A = 0;
    net_common_w.x3B = 0;
    net_common_w.x3C = 0;
    net_common_w.x3D = 0;
    net_common_w.x2C = net_common_w.x2C | 0x800;
}

void Ncm_spr_PRG_BAR_set(void) {
    net_common_w.x2C = net_common_w.x2C | 0x1000;
}

void Ncm_spr_DNAS_set(void) {
    net_common_w.x38 = 0;
    net_common_w.x39 = 0;
    net_common_w.x3A = 0;
    net_common_w.x3B = 0;
    net_common_w.x3C = 0;
    net_common_w.x3D = 0;
    net_common_w.x2C = net_common_w.x2C | 0x2000;
    net_common_w.x2C = net_common_w.x2C | 0x4000;
    net_common_w.x2C = net_common_w.x2C | 0x01000000;
}

void Ncm_spr_DNAS_ERR_set(void) {
    net_common_w.x2C = net_common_w.x2C | 0x02000000;
}

void Ncm_spr_SVAE_GAME_set(void) {
    net_common_w.x2C = net_common_w.x2C | 0x8000;
}

void Net_put_overlay_data(int n) {
    Load_overlay(n & 0xFF, 2);
}

int NetLoadWait(void) {
    return 0;
}

int Net_kb_input_sub(void) {
    int r = 0;

    kbdExecServer();
    if (SoftKeyWork[0] == 1) {
        r = (s8)SoftKeyboard_move(SoftKeyWork + 4, (s16)Psw[0].x00, (s16)Psw[0].x04);
    }
    return r;
}

typedef struct SKMODE {
    s32 a;
    s32 b;
} SKMODE;
extern SKMODE skey_mode_tbl_910[];

void Net_kb_input_init2(int x, int y, char *str, int w, int mode)
{
  char *new_var;
  long long new_var2;
  SoftKeyboard_pos_set((f32) x, (s16) y);
  new_var2 = skey_mode_tbl_910[mode].a;
  SoftKeyboard_set(new_var2, (u8) skey_mode_tbl_910[mode].b, w & 0xFFFF, str);
  memset(SoftKeyWork + 4, 0, 0x100);
  new_var = (char *) SoftKeyWork;
  strcpy(new_var + 4, str);
  SoftKey_onoff(1);
}

u8 *SoftKey_Getstr(void) {
    return SoftKeyWork + 4;
}

void SoftKey_onoff(int on) {
    switch (on & 0xFF) {
    case 0:
        SoftKeyWork[0] = 0;
        break;
    case 1:
        SoftKeyWork[0] = 1;
        break;
    }
}

void SoftKey_trans(void) {
    if (SoftKeyWork[0] == 1) {
        DispSoftkeyboard(1);
    }
}

void Def_net_data_set(void) {
    memset(CNFile, 0, 0x1BEC);
}

void Net_Fade_Sub(void) {
    /* empty */
}

void YnFile_Data_load(int a, int b) {
    load_file_mdl(b, a + 2313);
}
