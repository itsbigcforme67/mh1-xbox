/* netbgm04 - network menu glue 0x002677B0-0x00267FC4: ms_network_bb_edit_mmbbid, ms_network_bb_regulation. Whole file in netbgm_nm.c. */
#include "types.h"

typedef struct NETCW {
    u8 pad00;
    u8 step;            /* 0x01 */
    s8 x02;             /* 0x02 */
    u8 pad03;
    s16 timer;          /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 x08;            /* 0x08 menu cursor */
    s16 sel;            /* 0x0A */
    u8 pad0C[0x11 - 0x0C];
    u8 x11;             /* 0x11 */
    u8 pad12[0x2C - 0x12];
    s32 x2C;            /* 0x2C */
} NETCW;
extern NETCW net_common_w;
typedef struct MEMTEX {
    u8 pad[0x470];
    s32 x470;
} MEMTEX;
extern MEMTEX mem_tex;
extern void *NET_CON_TEX[];

int se_stat();
int se_req_bgm_vol();
int NetLoadWait();
int Net_all_reset();
int Net_work_init_all();
int Net_put_overlay_data();
int Net_demo_camera_set();
int load_texlist();
int SoftkeyLoad();
int Ncm_mssage_disp_req();
int Ncm_mssage_disp_option_req();
int Ncm_menu_disp_req();
int Ncm_spr_set_diarog_m();
int Ncm_spr_set_diarog_b();
int Ncm_spr_set_diarog_s();
int Ncm_spr_D_MENU_set();
int Ncm_spr_kill();
int Ncm_spr_kill2();
int Net_fade_kill();
int Net_fade_check();
int Net_disp_net_name_req();
int Net_McWorkInt();
int Net_McWorkInit();
int net_set_se_cur();
int net_shot_ok_ck();
int net_shot_ng_ck();
int net_swdata();
int SaveNetFile();
void *memcpy(void *, const void *, int);
void *memset(void *, int, int);
extern u8 system_w[];
extern u8 CNFile[];
extern u8 keep_mmbb_id_0052FA10[];
extern u8 keep_mmbb_password[];
extern void *cw;
extern s8 MMBB_LOGIN;
int Net_setBGcolor();
int Ncm_spr_BG_set();
int Ncm_spr_TITLE_set();
int Ncm_spr_MESS_SET();
int Ncm_spr_MENU_SET();
int Net_fade_execute();
extern u8 D_6DD7E0[];
int Ncm_mmbb_spr_load(void);
int Ncm_mmbb_spr_create(void);
void net_bgm_set(void);







int Ncm_spr_kill_all();
int Ncm_spr_kill_all_ex_BG();



int ms_network_bb_edit_mmbbid(void) {
    int ret = 0;
    int sw;
    int r;
    s16 t;

    switch (net_common_w.step) {
    case 0:
        Ncm_mssage_disp_req(0x1F);
        if (Ncm_mmbb_spr_load() != 0) {
            net_common_w.step++;
            Net_fade_kill();
        }
        break;
    case 1:
        if (Ncm_mmbb_spr_create() != 0) {
            Ncm_mssage_disp_req(0x1F);
            net_common_w.timer = 0xA;
            net_common_w.step++;
            net_common_w.sel = 1;
            Ncm_spr_set_diarog_m();
            Ncm_spr_D_MENU_set(4, 2);
            net_bgm_set();
        }
        break;
    case 2:
        Ncm_mssage_disp_req(0x1F);
        Ncm_mssage_disp_req(0x17);
        Ncm_mssage_disp_req(0x18);
        Ncm_mssage_disp_req(0x19);
        Net_disp_net_name_req(1);
        Net_disp_net_name_req(2);
        Ncm_menu_disp_req(5);
        if (net_common_w.timer == 0) {
            sw = net_swdata();
            if (sw & 0x2000) {
                if (net_common_w.sel != 0) {
                    net_set_se_cur();
                    net_common_w.sel = 0;
                }
            } else if ((sw & 0x1000) && net_common_w.sel != 1) {
                net_set_se_cur();
                net_common_w.sel = 1;
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x200);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (net_common_w.sel == 0) {
                    net_common_w.timer = 5;
                    net_common_w.step++;
                } else {
                    net_common_w.step = 0x64;
                }
            } else if (net_shot_ng_ck() != 0) {
                Ncm_spr_kill(0x200);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                net_common_w.step = 0x64;
            }
        } else {
            net_common_w.timer = net_common_w.timer - 1;
        }
        break;
    case 3:
        Ncm_mssage_disp_req(0x1F);
        if (net_common_w.timer != 0) {
            net_common_w.timer = net_common_w.timer - 1;
        } else {
            Net_McWorkInit(2);
            system_w[0x3C] = 1;
            memcpy(keep_mmbb_id_0052FA10, CNFile + 0x960, 0xB);
            memcpy(keep_mmbb_password, CNFile + 0x96B, 9);
            memset(CNFile + 0x960, 0, 0xB);
            memset(CNFile + 0x96B, 0, 9);
            net_common_w.x02 = 0;
            cw = D_6DD7E0;
            net_common_w.step++;
            Ncm_spr_set_diarog_b();
        }
        break;
    case 4:
        Ncm_mssage_disp_req(0x1F);
        r = SaveNetFile();
        switch (r) {
        case 0:
            break;
        case 1:
            net_common_w.step++;
            Ncm_spr_set_diarog_s();
            system_w[0x3C] = 0;
            net_common_w.timer = 0xB4;
            break;
        case -1:
            net_common_w.step = 0x64;
            memcpy(CNFile + 0x960, keep_mmbb_id_0052FA10, 0xB);
            memcpy(CNFile + 0x96B, keep_mmbb_password, 9);
            system_w[0x3C] = 0;
            break;
        }
        break;
    case 5:
        Ncm_mssage_disp_req(0x1F);
        Ncm_mssage_disp_req(0x1A);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.step++;
            Ncm_spr_kill(0x200);
        } else if (net_common_w.timer < 0x78) {
            Ncm_mssage_disp_option_req(0x1A);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.step++;
                Ncm_spr_kill(0x200);
            }
        }
        break;
    case 6:
        ret = 1;
        break;
    case 0x64:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
    case 0x65:
            ret = -1;
        }
        break;
    }
    return ret;
}

int ms_network_bb_regulation(void) {
    int ret = 0;
    int sw;
    s16 t;

    switch (net_common_w.step) {
    case 0:
        Ncm_mssage_disp_req(0x1F);
        if (Ncm_mmbb_spr_load() != 0) {
            net_common_w.step++;
            Net_fade_kill();
            Net_setBGcolor(0);
        }
        break;
    case 1:
        Ncm_mssage_disp_req(0x1F);
        if (Ncm_mmbb_spr_create() != 0) {
            net_common_w.timer = 0xA;
            net_common_w.step++;
            net_common_w.x08 = 1;
            Ncm_spr_set_diarog_b();
            Ncm_spr_D_MENU_set(3, 2);
            net_bgm_set();
        }
        break;
    case 2:
        Ncm_mssage_disp_req(0x13);
        Ncm_mssage_disp_req(0x14);
        Ncm_mssage_disp_req(0x16);
        Ncm_mssage_disp_req(0x15);
        if (net_common_w.timer == 0) {
            sw = net_swdata();
            if (sw & 0x1000) {
                if (net_common_w.x08 != 1) {
                    net_set_se_cur();
                    net_common_w.x08 = 1;
                }
            } else if (sw & 0x2000) {
                if (net_common_w.x08 != 0) {
                    net_set_se_cur();
                    net_common_w.x08 = 0;
                }
            }
            if (net_shot_ok_ck(1) != 0) {
                net_common_w.step++;
                net_common_w.x06 = 0xA;
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
            } else if (net_shot_ng_ck() != 0) {
                net_common_w.step = 6;
                Ncm_spr_kill(0x200);
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
            }
        } else {
            net_common_w.timer = net_common_w.timer - 1;
        }
        break;
    case 3:
        Ncm_mssage_disp_req(0x13);
        Ncm_mssage_disp_req(0x14);
        Ncm_mssage_disp_req(0x16);
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            if (net_common_w.x08 == 0) {
                Ncm_spr_kill_all_ex_BG();
                net_common_w.step++;
                Net_fade_execute(0, 0x14, 1);
            } else {
                Ncm_spr_kill(0x200);
                net_common_w.step = 6;
            }
        }
        break;
    case 4:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
            Ncm_spr_kill_all();
    case 5:
            ret = 1;
        }
        break;
    case 6:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
    case 7:
            ret = -1;
        }
        break;
    }
    return ret;
}
