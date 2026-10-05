/* ms04 - f_ms 0x002687E0-0x0026A2E8: ms_net_patch_set. Whole file in ms_nm.c. */
#include "types.h"
#include "netcw.h"

typedef struct CNF {
    u8 pad[0xC80];
    s8 last;            /* 0xC80 last connection setting saved */
} CNF;
extern CNF CNFile;

int net_swdata3();
int net_overlay_request();
int net_overlay_init();
int net_shot_ok_ck();
int Net_setBGcolor();
int Net_fade_kill();
int Net_fade_execute();
int Net_fade_check();
int Ncm_spr_BG_set();
int Ncm_spr_DNAS_ERR_set();
int Ncm_spr_DNAS_set();
int Ncm_spr_PRG_BAR_set();
int Ncm_spr_kill();
int Ncm_spr_kill_all();
int Ncm_spr_set_diarog_b();
int Ncm_mmbb_spr_load();
int Ncm_mmbb_spr_create();
int SaveNetFile();
int se_stop_all();
int func_A769B0();
int func_A769C0();





/* ms_net_patch_set (0x2687E0, 6920 bytes): the "patch download" menu state machine (state in net_common_w+0x89,
 * sub step +0x8A). Written from the m2c draft; field meanings are guesses. */
#define NCU8(o) (*((u8 *)&net_common_w + (o)))
#define NCS8(o) (*((s8 *)&net_common_w + (o)))
#define NCS16(o) (*(s16 *)((u8 *)&net_common_w + (o)))
#define NCS32(o) (*(s32 *)((u8 *)&net_common_w + (o)))
#define NCP(o) ((u8 *)&net_common_w + (o))
#define SYSS8_3C (*((s8 *)system_w + 0x3C))
#define PBW(o) (*(s32 *)(patch_buff + (o)))

extern u8 patch_buff[];
extern u8 system_w[];
extern u8 D_6E9704[];
extern u8 D_6E9714[];
extern s32 D_6E9700[];
extern s32 Last_sel_drive;
extern s32 data_load_ptr;
extern s32 keep_last_sel_drive;
void *memcpy(void *, const void *, int);
void *strncpy(void *, const void *, int);
int strncmp(const void *, const void *, int);
int Net_all_reset();
int Net_work_init_all();
int Ncm_spr_kill2();
int Ncm_spr_kill_all_ex_BG();
int Ncm_spr_D_MENU_set();
int Ncm_mssage_disp_req();
int Ncm_mssage_disp_option_req();
int Ncm_menu_disp_req();
int Net_disp_net_name_req();
int net_set_se_cur();
int net_shot_ng_ck();
int net_swdata();
int NetAutoLoad();
int SaveGameFileNet2();
int PatchExecCS();
int PatchInitCS();
int Net_McWorkInit();
int McActInit();
int McActMain();
int McActResult();
int McActSave0Set();
int Net_Icon_Data_Load();
int func_A2FBC0();
int func_A2FBD0();
int func_A2FD30();
int func_A769B0();
int func_A769D0();


int ms_net_patch_set(void) {
    int ret = 0;
    s16 t;
    int r;
    u8 *pb;

    pb = patch_buff + 0x20000;
    switch (net_common_w.x89) {
    case 0:
        switch (net_common_w.x8A) {
        case 0:
            net_common_w.x8A = net_common_w.x8A + 1;
            net_common_w.x15 = 0;
            net_common_w.x7E = 0;
            net_common_w.x80 = 0;
            Net_all_reset(0);
            Net_work_init_all();
            Net_setBGcolor(0);
            Net_fade_kill();
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
            break;
        case 1:
            if (Ncm_mmbb_spr_load() != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 2:
            if (Ncm_mmbb_spr_create() != 0) {
                net_common_w.x8A = 0;
                net_common_w.x89 = net_common_w.x89 + 1;
                Ncm_spr_BG_set();
                Ncm_spr_set_diarog_b();
                net_common_w.timer = 0xA;
            }
            break;
        }
        break;
    case 1:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x8A = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x8B = 0;
            net_common_w.sub = 0;
            net_common_w.x03 = 0;
            net_common_w.timer = 0;
        }
        break;
    case 2:
        r = NetAutoLoad();
        switch (r) {
        case 0:
            break;
        case 1:
            if (strncmp(D_6E9704, pb, 0xA) == 0) {
                net_common_w.x7F = 0;
                net_common_w.x89 = net_common_w.x89 + 1;
            } else {
                net_common_w.x89 = 0xA;
                net_common_w.x7F = 1;
            }
            net_common_w.x8A = 0;
            net_common_w.timer = 0xA;
            net_common_w.x11 = 1;
            keep_last_sel_drive = Last_sel_drive;
            Net_fade_execute(0, 0xA, 1);
            break;
        case -1:
            net_common_w.x89 = 0x28;
            net_common_w.timer = 0xA;
            break;
        }
        break;
    case 3:
        switch (net_common_w.x8A) {
        case 0:
            if (Net_fade_check() == 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
                net_common_w.x11 = 1;
                Ncm_spr_kill_all();
            }
            break;
        case 1:
            if (net_overlay_request(2) != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 2:
            if (net_overlay_init(2) != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 3:
            if (Ncm_mmbb_spr_load() != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
        case 4:
            if (Ncm_mmbb_spr_create() != 0) {
                net_common_w.x8A = 0;
                net_common_w.timer = 0xA;
                net_common_w.x89 = net_common_w.x89 + 1;
                Ncm_spr_BG_set();
                Ncm_spr_set_diarog_b();
                Net_fade_execute(0, 0x14, 0);
            }
            break;
        }
        break;
    case 4:
        Ncm_mssage_disp_req(0x3F);
        Net_disp_net_name_req(7);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x8B = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x84 = *(s32 *)(pb + 0x14);
            net_common_w.x15 = 1;
            net_common_w.x06 = 0;
            net_common_w.x80 = 0;
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
        }
        break;
    case 5:
        if (net_common_w.x7F == 0) {
            Ncm_mssage_disp_req(0x3F);
            Net_disp_net_name_req(7);
        } else {
            Ncm_mssage_disp_req(0x49);
        }
        r = func_A2FD30((u8 *)&net_common_w.x8B, (u8 *)&net_common_w.x06, patch_buff + 0x10000, net_common_w.x84, 0x10000);
        if (0 < r) {
            PatchExecCS(0, 0);
            PatchExecCS(1, 3);
            net_common_w.x8A = 0;
            net_common_w.timer = 0x5A;
            net_common_w.x15 = 3;
            net_common_w.x8B = 0;
            net_common_w.x06 = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
        } else if (r < 0) {
            net_common_w.timer = 0;
            net_common_w.x89 = 0x22;
            net_common_w.x06 = 0x20;
            net_common_w.x15 = 2;
            net_common_w.x8A = 0;
            net_common_w.x8B = 0;
            net_common_w.x80 = func_A2FBC0();
        }
        break;
    case 6:
        if (net_common_w.x7F == 0) {
            Ncm_mssage_disp_req(0x3F);
            Net_disp_net_name_req(7);
        } else {
            Ncm_mssage_disp_req(0x49);
        }
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            Net_fade_execute(0, 0x14, 1);
        } else if (net_common_w.timer < 0x3C) {
            if (net_common_w.x7F == 0) {
                Ncm_mssage_disp_option_req(0x3F);
            } else {
                Ncm_mssage_disp_option_req(0x49);
            }
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.x89 = net_common_w.x89 + 1;
                Net_fade_execute(0, 0xA, 1);
            }
        }
        break;
    case 7:
        if (Net_fade_check() == 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            Ncm_spr_kill_all();
    case 8:
            ret = 1;
        }
        break;
    case 9:
        switch (net_common_w.x8A) {
        case 1:
            if (net_overlay_request(1) != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 2:
            if (net_overlay_init(1) != 0) {
                net_common_w.x8A = 0;
                net_common_w.x89 = 0xC;
                Ncm_spr_D_MENU_set(0xB, 2);
            }
            break;
        }
        break;
    case 0xA:
        switch (net_common_w.x8A) {
        case 0:
            if (Net_fade_check() == 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
                net_common_w.x11 = 1;
                Ncm_spr_kill_all();
            }
            break;
        case 1:
            if (net_overlay_request(1) != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 2:
            if (net_overlay_init(1) != 0) {
                net_common_w.x8A = 0;
                net_common_w.timer = 0xF;
                net_common_w.x0A = 0;
                net_common_w.x89 = net_common_w.x89 + 1;
                Ncm_spr_BG_set();
                Ncm_spr_set_diarog_b();
                Ncm_spr_D_MENU_set(0xB, 2);
                Net_fade_execute(0, 0xA, 0);
            }
            break;
        }
        break;
    case 0xB:
        Ncm_mssage_disp_req(0x40);
        Ncm_menu_disp_req(4);
        Net_disp_net_name_req(8);
        Net_disp_net_name_req(9);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.timer = 0xE10;
            net_common_w.x89 = net_common_w.x89 + 1;
        }
        break;
    case 0xC:
        Ncm_mssage_disp_req(0x40);
        Ncm_menu_disp_req(4);
        Net_disp_net_name_req(8);
        Net_disp_net_name_req(9);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.x89 = 0x32;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
        } else {
            r = net_swdata();
            if (r & 0x2000) {
                if (net_common_w.x0A != 0) {
                    net_common_w.x0A = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && net_common_w.x0A != 1) {
                net_common_w.x0A = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (net_common_w.x0A == 0) {
                    net_common_w.x89 = net_common_w.x89 + 1;
                    net_common_w.timer = 0xA;
                } else {
                    net_common_w.x89 = 0x32;
                    net_common_w.timer = 0xA;
                }
            } else if (net_shot_ng_ck() != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                net_common_w.x89 = 0x32;
                net_common_w.timer = 0xA;
            }
        }
        break;
    case 0xD:
        Ncm_mssage_disp_req(0x40);
        Net_disp_net_name_req(8);
        Net_disp_net_name_req(9);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            SYSS8_3C = 1;
        }
        break;
    case 0xE:
        Ncm_mssage_disp_req(0x41);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x8B = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            Net_McWorkInit(0);
            McActInit(0);
            SYSS8_3C = 1;
        }
        break;
    case 0xF:
        Ncm_mssage_disp_req(0x41);
        if (Net_Icon_Data_Load(1) != 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x79 = keep_last_sel_drive;
            McActSave0Set(net_common_w.x79, data_load_ptr, 0);
        }
        break;
    case 0x10:
        Ncm_mssage_disp_req(0x41);
        McActMain();
        r = McActResult();
        switch (r) {
        case -251:
        case 0:
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_kill_all_ex_BG();
            SYSS8_3C = 0;
            break;
        case -254:
        case -256:
        case -253:
        case -252:
            SYSS8_3C = 0;
            net_common_w.x89 = 0x20;
            net_common_w.timer = 0xA;
            net_common_w.x7E = 6;
            break;
        case -255:
            SYSS8_3C = 0;
            net_common_w.x89 = 0x1E;
            net_common_w.timer = 0xA;
            net_common_w.x7E = 5;
            break;
        }
        break;
    case 0x11:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_set_diarog_b();
            SYSS8_3C = 0;
        }
        break;
    case 0x12:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x8B = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x15 = 1;
            net_common_w.timer = 0;
            net_common_w.x06 = 0;
            net_common_w.x0A = 0;
            net_common_w.x80 = 0;
            Ncm_spr_DNAS_set();
            Ncm_spr_PRG_BAR_set();
        }
        break;
    case 0x13:
        if (start_cancel_check(&net_common_w.x0A) != 0) {
            net_common_w.x06 = 0x1E;
        }
        r = func_A769D0((u8 *)&net_common_w.x8B, (u8 *)&net_common_w.x06, (u8 *)&net_common_w.timer);
        if (r < 0) {
            net_common_w.x8A = 0;
            net_common_w.x89 = 0x22;
            net_common_w.x06 = 0x20;
            net_common_w.x15 = 2;
            net_common_w.x8B = 0;
            net_common_w.timer = 0;
            net_common_w.x80 = func_A769B0();
        } else if (r > 0) {
            net_common_w.x8A = 0;
            net_common_w.x8B = 0;
            net_common_w.x06 = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x84 = D_6E9700[0];
            memcpy(patch_buff, patch_buff + 0x10000, 0x10000);
        }
        break;
    case 0x14:
        switch (net_common_w.x8A) {
        case 0:
            if (net_overlay_request(2) != 0) {
                net_common_w.x8A = net_common_w.x8A + 1;
            }
            break;
        case 1:
            if (net_overlay_init(2) != 0) {
                net_common_w.x8A = 0;
                net_common_w.x89 = net_common_w.x89 + 1;
            }
            break;
        }
        break;
    case 0x15:
        r = func_A2FBD0((u8 *)&net_common_w.x8B, (u8 *)&net_common_w.x06, patch_buff, net_common_w.x84, 0x10000);
        if (0 < r) {
            net_common_w.timer = r;
            net_common_w.x89 = 0x16;
            net_common_w.x06 = 0x72;
            net_common_w.x15 = 3;
            net_common_w.x8A = 0;
            net_common_w.x8B = 0;
            *(s32 *)(pb + 0x14) = r;
        } else if (r < 0) {
            net_common_w.timer = 0;
            net_common_w.x89 = 0x22;
            net_common_w.x06 = 0x20;
            net_common_w.x15 = 2;
            net_common_w.x8A = 0;
            net_common_w.x8B = 0;
            net_common_w.x80 = func_A2FBC0();
        }
        break;
    case 0x16:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x1000);
            Ncm_spr_kill(0x200);
        } else if (net_common_w.x06 < 0x3D && net_shot_ok_ck(2) != 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x1000);
            Ncm_spr_kill(0x200);
        }
        break;
    case 0x17:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.sub = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x03 = 0;
            Ncm_spr_set_diarog_b();
            SYSS8_3C = 1;
            strncpy(pb, D_6E9704, 0xA);
            strncpy(pb + 0xA, D_6E9714, 4);
        }
        break;
    case 0x18:
        r = SaveGameFileNet2();
        switch (r) {
        case 1:
            net_common_w.x8B = 0;
            net_common_w.x89 = 5;
            net_common_w.x84 = *(s32 *)(pb + 0x14);
            net_common_w.x06 = 0;
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
            net_common_w.x80 = 0;
            net_common_w.x15 = 1;
            SYSS8_3C = 0;
            break;
        case -1:
            SYSS8_3C = 0;
            net_common_w.x89 = 0x19;
            net_common_w.timer = 0xA;
            break;
        }
        break;
    case 0x19:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.x89 = 9;
            net_common_w.timer = 0xA;
            Ncm_spr_D_MENU_set(0xB, 2);
        }
        break;
    case 0x1E:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0x96;
            net_common_w.x7E = 5;
        }
        break;
    case 0x1F:
        Ncm_mssage_disp_req((net_common_w.x7E + 0x3F) & 0xFF);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = 0x28;
            net_common_w.timer = 0xA;
        } else if (net_common_w.timer < 0x5B) {
            Ncm_mssage_disp_option_req((net_common_w.x7E + 0x3F) & 0xFF);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.x89 = 0x28;
                net_common_w.timer = 0xA;
            }
        }
        break;
    case 0x20:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = 0x1F;
            net_common_w.timer = 0x96;
            net_common_w.x7E = 6;
        }
        break;
    case 0x21:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = 0x1F;
            net_common_w.timer = 0x96;
            net_common_w.x7E = 7;
        }
        break;
    case 0x22:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.x15 = 3;
            net_common_w.timer = 0x2D;
            Ncm_spr_kill(0x1000);
        }
        break;
    case 0x23:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x200);
        }
        break;
    case 0x24:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 5;
            Ncm_spr_set_diarog_b();
            Ncm_spr_DNAS_ERR_set();
        }
        break;
    case 0x25:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0x13B;
            net_common_w.x06 = 0xD2;
        }
        break;
    case 0x26:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.x89 = 0x32;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x02000000);
        } else if (net_common_w.timer <= net_common_w.x06 && net_shot_ok_ck(2) != 0) {
            net_common_w.x89 = 0x32;
            net_common_w.timer = 0xA;
            Ncm_spr_kill(0x02000000);
        }
        break;
    case 0x28:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_D_MENU_set(0xB, 1);
        }
        break;
    case 0x29:
        Ncm_mssage_disp_req(0x43);
        Ncm_menu_disp_req(4);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xE10;
        }
        break;
    case 0x2A:
        Ncm_mssage_disp_req(0x43);
        Ncm_menu_disp_req(4);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            net_common_w.x0A = 0;
            net_common_w.timer = 0xA;
            net_common_w.x89 = net_common_w.x89 + 1;
            Net_fade_execute(0, 0x14, 1);
        } else {
            r = net_swdata();
            if (r & 0x2000) {
                if (net_common_w.x0A != 0) {
                    net_common_w.x0A = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && net_common_w.x0A != 1) {
                net_common_w.x0A = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (net_common_w.x0A == 0) {
                    net_common_w.x89 = net_common_w.x89 + 1;
                    net_common_w.timer = 0xA;
                    Net_fade_execute(0, 0x14, 1);
                } else {
                    net_common_w.x89 = 0x2D;
                    net_common_w.timer = 0xA;
                }
            } else if (net_shot_ng_ck() != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                net_common_w.x89 = 0x2D;
                net_common_w.timer = 0xA;
            }
        }
        break;
    case 0x2B:
        if (Net_fade_check() == 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            Ncm_spr_kill_all();
            PatchInitCS();
        }
        break;
    case 0x2C:
        ret = -1;
        break;
    case 0x2D:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = 2;
            net_common_w.timer = 0xA;
        }
        break;
    case 0x32:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_D_MENU_set(0xB, 2);
        }
        break;
    case 0x33:
        Ncm_mssage_disp_req(0x42);
        Ncm_menu_disp_req(4);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            net_common_w.timer = 0xE10;
        }
        break;
    case 0x34:
        Ncm_mssage_disp_req(0x42);
        Ncm_menu_disp_req(4);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x0A = 0;
            net_common_w.timer = 0xA;
            net_common_w.x89 = net_common_w.x89 + 1;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
        } else {
            r = net_swdata();
            if (r & 0x2000) {
                if (net_common_w.x0A != 0) {
                    net_common_w.x0A = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && net_common_w.x0A != 1) {
                net_common_w.x0A = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (net_common_w.x0A == 0) {
                    Ncm_spr_kill(0x200);
                    net_common_w.x89 = net_common_w.x89 + 1;
                    net_common_w.timer = 0xA;
                } else {
                    net_common_w.x89 = 0x37;
                    net_common_w.timer = 0xA;
                }
            } else if (net_shot_ng_ck() != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                net_common_w.x89 = 0x37;
                net_common_w.timer = 0xA;
            }
        }
        break;
    case 0x35:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x89 = net_common_w.x89 + 1;
            Ncm_spr_kill_all();
            PatchInitCS();
        }
        break;
    case 0x36:
        ret = -1;
        break;
    case 0x37:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.x8A = 0;
            net_common_w.x89 = 9;
            net_common_w.timer = 0xA;
            net_common_w.x0A = 0;
        }
        break;
    }
    return ret;
}
