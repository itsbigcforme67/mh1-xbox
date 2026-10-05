/* ms_nm - f_ms (SLPM_654.95 0x00267FD0-, main.bin): network menu sub screens: DNAS authentication, saving the
 * last connection time, patch download. Near-match C, not built. */
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

int start_cancel_check(s16 *p) {
    int ret = 0;

    if (net_swdata3(0) & 0x8000) {
        *p += 1;
        if (*p >= 0x3C) {
            ret = 1;
        }
    } else {
        *p = 0;
    }
    return ret;
}

int ms_network_bb_authentication(void) {
    int ret = 0;
    int r;
    s16 t;

    switch (net_common_w.step) {
    case 0:
        if (net_overlay_request(1) != 0) {
            net_common_w.step++;
            Net_setBGcolor(0);
            Net_fade_kill();
        }
        break;
    case 1:
        if (net_overlay_init(1) != 0) {
            net_common_w.x03 = 0;
            net_common_w.timer = 0;
            net_common_w.x15 = 0;
            net_common_w.step++;
        }
        break;
    case 2:
        net_common_w.step++;
        break;
    case 3:
        net_common_w.step++;
        se_stop_all(0x64);
        break;
    case 4:
        if (Net_fade_execute(0, 0xA, 0) != 0) {
            net_common_w.timer = 0;
            net_common_w.step = 0xA;
            net_common_w.x06 = 0xA;
            net_common_w.x03 = 0;
            Ncm_spr_BG_set();
            Ncm_spr_set_diarog_b();
            Ncm_spr_DNAS_set();
            Ncm_spr_PRG_BAR_set();
        }
        break;
    case 0xA:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.step = 5;
            net_common_w.x15 = 1;
            net_common_w.x06 = 0;
            net_common_w.x0A = 0;
        }
        break;
    case 5:
        if (start_cancel_check(&net_common_w.x0A) != 0) {
            net_common_w.x06 = 0x3D;
        }
        r = func_A769C0(&net_common_w.x03, &net_common_w.x06, &net_common_w.timer);
        if (r < 0) {
            net_common_w.sub = 0;
            net_common_w.step = 0xF;
            net_common_w.x06 = 0x38;
            net_common_w.x15 = 2;
            net_common_w.x03 = 0;
            net_common_w.x80 = func_A769B0();
        } else if (r > 0) {
            net_common_w.sub = 0;
            net_common_w.step = 0xB;
            net_common_w.x06 = 0x18;
            net_common_w.x15 = 3;
            net_common_w.x03 = 0;
        }
        break;
    case 9:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.step = 8;
            net_common_w.x06 = 0xA;
            net_common_w.timer = 0;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x1000);
            Ncm_spr_kill(0x200);
        }
        break;
    case 0xB:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.step = 0x14;
            net_common_w.timer = 0;
            net_common_w.x06 = 0;
            Ncm_spr_kill(0x1000);
        }
        break;
    case 8:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.step = 0x64;
            net_common_w.timer = 0x13B;
            net_common_w.x06 = 0xD2;
            Ncm_spr_set_diarog_b();
            Ncm_spr_DNAS_ERR_set();
        }
        break;
    case 0xF:
        t = net_common_w.x06 - 1;
        net_common_w.x06 = t;
        if (t <= 0) {
            net_common_w.timer = 0;
            net_common_w.step = 9;
            net_common_w.x06 = 0x2D;
            net_common_w.x15 = 3;
        }
        break;
    case 0x64:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.step++;
        } else if (net_common_w.timer <= net_common_w.x06 && net_shot_ok_ck(2) != 0) {
            net_common_w.step++;
        }
        break;
    case 0x65:
        if (Net_fade_execute(0, 0xA, 1) != 0) {
            net_common_w.step++;
        }
        break;
    case 0x66:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
            Ncm_spr_kill_all();
    case 0x67:
            ret = -1;
        }
        break;
    case 0x14:
        if (Net_fade_execute(0, 0xA, 1) != 0) {
            net_common_w.step++;
        }
        break;
    case 0x15:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
            Ncm_spr_kill_all();
    case 0x16:
            ret = 1;
        }
        break;
    }
    return ret;
}

int ms_network_bb_last_time_save(void) {
    int ret = 0;

    switch (net_common_w.step) {
    case 0:
        if (CNFile.last == 1 && net_common_w.x28 == 0) {
            net_common_w.step = 6;
            break;
        }
        CNFile.last = 1;
        net_common_w.step++;
    case 1:
        if (Ncm_mmbb_spr_load() != 0) {
            net_common_w.x08 = 0;
            net_common_w.step++;
            Net_setBGcolor(0);
            Net_fade_kill();
        }
        break;
    case 2:
        if (Ncm_mmbb_spr_create() != 0) {
            net_common_w.step = net_common_w.step + 1;
            net_common_w.timer = 0xA;
            Ncm_spr_BG_set();
            Ncm_spr_set_diarog_b();
            Net_fade_execute(0, 0x14, 0);
        }
        break;
    case 3:
        net_common_w.step++;
        break;
    case 4:
        net_common_w.step++;
        net_common_w.sub = 0;
        break;
    case 5:
        switch (SaveNetFile()) {
        case 0:
            break;
        case 1:
            net_common_w.x28 = 0;
        case -1:
            net_common_w.step++;
            Net_fade_execute(0, 0x14, 1);
            break;
        }
        break;
    case 6:
        if (Net_fade_check() == 0) {
            net_common_w.step++;
            Ncm_spr_kill_all();
    case 7:
            ret = 1;
        }
        break;
    }
    return ret;
}

void ms_net_patch_set_init(void) {
    net_common_w.x89 = 0;
    net_common_w.x8A = 0;
    net_common_w.x11 = 1;
}

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
    switch (NCU8(0x89)) {
    case 0:
        switch (NCU8(0x8A)) {
        case 0:
            NCU8(0x8A) = NCU8(0x8A) + 1;
            NCS8(0x15) = 0;
            NCS8(0x7E) = 0;
            NCS32(0x80) = 0;
            Net_all_reset(0);
            Net_work_init_all();
            Net_setBGcolor(0);
            Net_fade_kill();
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
            break;
        case 1:
            if (Ncm_mmbb_spr_load() != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 2:
            if (Ncm_mmbb_spr_create() != 0) {
                NCU8(0x8A) = 0;
                NCU8(0x89) = NCU8(0x89) + 1;
                Ncm_spr_BG_set();
                Ncm_spr_set_diarog_b();
                NCS16(4) = 0xA;
            }
            break;
        }
        break;
    case 1:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x8A) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS8(0x8B) = 0;
            NCS8(2) = 0;
            NCS8(3) = 0;
            NCS16(4) = 0;
        }
        break;
    case 2:
        r = NetAutoLoad();
        switch (r) {
        case 0:
            break;
        case 1:
            if (strncmp(D_6E9704, pb, 0xA) == 0) {
                NCS8(0x7F) = 0;
                NCU8(0x89) = NCU8(0x89) + 1;
            } else {
                NCU8(0x89) = 0xA;
                NCS8(0x7F) = 1;
            }
            NCU8(0x8A) = 0;
            NCS16(4) = 0xA;
            NCS8(0x11) = 1;
            keep_last_sel_drive = Last_sel_drive;
            Net_fade_execute(0, 0xA, 1);
            break;
        case -1:
            NCU8(0x89) = 0x28;
            NCS16(4) = 0xA;
            break;
        }
        break;
    case 3:
        switch (NCU8(0x8A)) {
        case 0:
            if (Net_fade_check(2) == 0) {
                NCS8(0x11) = 1;
                NCU8(0x8A) = NCU8(0x8A) + 1;
                Ncm_spr_kill_all();
            }
            break;
        case 1:
            if (net_overlay_request(2) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 2:
            if (net_overlay_init(2) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 3:
            if (Ncm_mmbb_spr_load(2) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
        case 4:
            if (Ncm_mmbb_spr_create() != 0) {
                NCU8(0x8A) = 0;
                NCS16(4) = 0xA;
                NCU8(0x89) = NCU8(0x89) + 1;
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
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS8(0x8B) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS32(0x84) = PBW(0x20014);
            NCS8(0x15) = 1;
            NCS16(6) = 0;
            NCS32(0x80) = 0;
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
        }
        break;
    case 5:
        if (NCS8(0x7F) == 0) {
            Ncm_mssage_disp_req(0x3F);
            Net_disp_net_name_req(7);
        } else {
            Ncm_mssage_disp_req(0x49);
        }
        r = func_A2FD30(NCP(0x8B), NCP(6), patch_buff + 0x10000, NCS32(0x84));
        if (r > 0) {
            PatchExecCS(0, 0);
            PatchExecCS(1, 3);
            NCU8(0x8A) = 0;
            NCS16(4) = 0x5A;
            NCS8(0x15) = 3;
            NCS8(0x8B) = 0;
            NCS16(6) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
        } else if (r < 0) {
            NCS16(4) = 0;
            NCU8(0x89) = 0x22;
            NCS16(6) = 0x20;
            NCS8(0x15) = 2;
            NCU8(0x8A) = 0;
            NCS8(0x8B) = 0;
            NCS32(0x80) = func_A2FBC0(0);
        }
        break;
    case 6:
        if (NCS8(0x7F) == 0) {
            Ncm_mssage_disp_req(0x3F);
            Net_disp_net_name_req(7);
        } else {
            Ncm_mssage_disp_req(0x49);
        }
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            Net_fade_execute(0, 0x14, 1);
        } else if (NCS16(4) < 0x3C) {
            if (NCS8(0x7F) == 0) {
                Ncm_mssage_disp_option_req(0x3F);
            } else {
                Ncm_mssage_disp_option_req(0x49);
            }
            if (net_shot_ok_ck(2) != 0) {
                NCU8(0x89) = NCU8(0x89) + 1;
                Net_fade_execute(0, 0xA, 1);
            }
        }
        break;
    case 7:
        if (Net_fade_check(1) == 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill_all();
    case 8:
            ret = 1;
        }
        break;
    case 9:
        switch (NCU8(0x8A)) {
        case 1:
            if (net_overlay_request(1) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 2:
            if (net_overlay_init(1) != 0) {
                NCU8(0x8A) = 0;
                NCU8(0x89) = 0xC;
                Ncm_spr_D_MENU_set(0xB, 2);
            }
            break;
        }
        break;
    case 0xA:
        switch (NCU8(0x8A)) {
        case 0:
            if (Net_fade_check(1) == 0) {
                NCS8(0x11) = 1;
                NCU8(0x8A) = NCU8(0x8A) + 1;
                Ncm_spr_kill_all();
            }
            break;
        case 1:
            if (net_overlay_request(1) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 2:
            if (net_overlay_init(1) != 0) {
                NCU8(0x8A) = 0;
                NCS16(4) = 0xF;
                NCS16(0xA) = 0;
                NCU8(0x89) = NCU8(0x89) + 1;
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
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCS16(4) = 0xE10;
            NCU8(0x89) = NCU8(0x89) + 1;
        }
        break;
    case 0xC:
        Ncm_mssage_disp_req(0x40);
        Ncm_menu_disp_req(4);
        Net_disp_net_name_req(8);
        Net_disp_net_name_req(9);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCU8(0x89) = 0x32;
            NCS16(4) = 0xA;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
        } else {
            r = net_swdata();
            if (r & 0x2000) {
                if (NCS16(0xA) != 0) {
                    NCS16(0xA) = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && NCS16(0xA) != 1) {
                NCS16(0xA) = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (NCS16(0xA) == 0) {
                    NCS16(4) = 0xA;
                    NCU8(0x89) = NCU8(0x89) + 1;
                } else {
                    NCU8(0x89) = 0x32;
                    NCS16(4) = 0xA;
                }
            } else if (net_shot_ng_ck(0x40000) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                NCU8(0x89) = 0x32;
                NCS16(4) = 0xA;
            }
        }
        break;
    case 0xD:
        Ncm_mssage_disp_req(0x40);
        Net_disp_net_name_req(8);
        Net_disp_net_name_req(9);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(4) = 0xA;
            SYSS8_3C = 1;
            NCU8(0x89) = NCU8(0x89) + 1;
        }
        break;
    case 0xE:
        Ncm_mssage_disp_req(0x41);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS8(0x8B) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            Net_McWorkInit(0);
            McActInit(0);
            SYSS8_3C = 1;
        }
        break;
    case 0xF:
        Ncm_mssage_disp_req(0x41);
        if (Net_Icon_Data_Load(1) != 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS8(0x79) = keep_last_sel_drive;
            McActSave0Set(NCS8(0x79), data_load_ptr, 0);
        }
        break;
    case 0x10:
        Ncm_mssage_disp_req(0x41);
        McActMain();
        r = McActResult();
        switch (r) {
        case 0:
        case -251:
            NCS16(4) = 0xA;
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill_all_ex_BG();
            SYSS8_3C = 0;
            break;
        case -252:
        case -253:
        case -256:
        case -254:
            SYSS8_3C = 0;
            NCU8(0x89) = 0x20;
            NCS16(4) = 0xA;
            NCS8(0x7E) = 6;
            break;
        case -255:
            SYSS8_3C = 0;
            NCU8(0x89) = 0x1E;
            NCS16(4) = 0xA;
            NCS8(0x7E) = 5;
            break;
        }
        break;
    case 0x11:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0xA;
            Ncm_spr_set_diarog_b();
            SYSS8_3C = 0;
        }
        break;
    case 0x12:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS8(0x8B) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS8(0x15) = 1;
            NCS16(4) = 0;
            NCS16(6) = 0;
            NCS16(0xA) = 0;
            NCS32(0x80) = 0;
            Ncm_spr_DNAS_set();
            Ncm_spr_PRG_BAR_set();
        }
        break;
    case 0x13:
        if (start_cancel_check((s16 *)NCP(0xA)) != 0) {
            NCS16(6) = 0x1E;
        }
        r = func_A769D0(NCP(0x8B), NCP(6), NCP(4));
        if (r < 0) {
            NCU8(0x8A) = 0;
            NCU8(0x89) = 0x22;
            NCS16(6) = 0x20;
            NCS8(0x15) = 2;
            NCS8(0x8B) = 0;
            NCS16(4) = 0;
            NCS32(0x80) = func_A769B0();
        } else if (r > 0) {
            NCU8(0x8A) = 0;
            NCS8(0x8B) = 0;
            NCS16(6) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS32(0x84) = *(s32 *)0x6E9700;
            memcpy(patch_buff, patch_buff + 0x10000, 0x10000);
        }
        break;
    case 0x14:
        switch (NCU8(0x8A)) {
        case 0:
            if (net_overlay_request(2) != 0) {
                NCU8(0x8A) = NCU8(0x8A) + 1;
            }
            break;
        case 1:
            if (net_overlay_init(2) != 0) {
                NCU8(0x8A) = 0;
                NCU8(0x89) = NCU8(0x89) + 1;
            }
            break;
        }
        break;
    case 0x15:
        r = func_A2FBD0(NCP(0x8B), NCP(6), patch_buff, NCS32(0x84));
        if ((s16)r > 0) {
            NCS16(4) = r;
            NCU8(0x89) = 0x16;
            NCS16(6) = 0x72;
            NCS8(0x15) = 3;
            NCU8(0x8A) = 0;
            NCS8(0x8B) = 0;
            PBW(0x20014) = r;
        } else if ((s16)r < 0) {
            NCS16(4) = 0;
            NCU8(0x89) = 0x22;
            NCS16(6) = 0x20;
            NCS8(0x15) = 2;
            NCU8(0x8A) = 0;
            NCS8(0x8B) = 0;
            NCS32(0x80) = func_A2FBC0();
        }
        break;
    case 0x16:
        t = NCS16(6) - 1;
        NCS16(6) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0xA;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x1000);
            Ncm_spr_kill(0x200);
        } else if (NCS16(6) < 0x3D && net_shot_ok_ck(2) != 0) {
            NCS16(4) = 0xA;
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x1000);
            Ncm_spr_kill(0x200);
        }
        break;
    case 0x17:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS8(2) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS8(3) = 0;
            Ncm_spr_set_diarog_b();
            SYSS8_3C = 1;
            strncpy(pb, D_6E9704, 0xA);
            strncpy(patch_buff + 0x2000A, D_6E9714, 4);
        }
        break;
    case 0x18:
        r = SaveGameFileNet2();
        switch (r) {
        case 1:
            NCS8(0x8B) = 0;
            NCU8(0x89) = 5;
            NCS32(0x84) = PBW(0x20014);
            NCS16(6) = 0;
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
            NCS32(0x80) = 0;
            NCS8(0x15) = 1;
            SYSS8_3C = 0;
            break;
        case -1:
            SYSS8_3C = 0;
            NCU8(0x89) = 0x19;
            NCS16(4) = 0xA;
            break;
        }
        break;
    case 0x19:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCU8(0x89) = 9;
            NCS16(4) = 0xA;
            Ncm_spr_D_MENU_set(0xB, 2);
        }
        break;
    case 0x1E:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0x96;
            NCS8(0x7E) = 5;
        }
        break;
    case 0x1F:
        Ncm_mssage_disp_req((NCS8(0x7E) + 0x3F) & 0xFF);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = 0x28;
            NCS16(4) = 0xA;
        } else if (NCS16(4) < 0x5B) {
            Ncm_mssage_disp_option_req((NCS8(0x7E) + 0x3F) & 0xFF);
            if (net_shot_ok_ck(2) != 0) {
                NCU8(0x89) = 0x28;
                NCS16(4) = 0xA;
            }
        }
        break;
    case 0x20:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = 0x1F;
            NCS16(4) = 0x96;
            NCS8(0x7E) = 6;
        }
        break;
    case 0x21:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = 0x1F;
            NCS16(4) = 0x96;
            NCS8(0x7E) = 7;
        }
        break;
    case 0x22:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS8(0x15) = 3;
            NCS16(4) = 0x2D;
            Ncm_spr_kill(0x1000);
        }
        break;
    case 0x23:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0xA;
            Ncm_spr_kill(0x2000);
            Ncm_spr_kill(0x4000);
            Ncm_spr_kill(0x01000000);
            Ncm_spr_kill(0x200);
        }
        break;
    case 0x24:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 5;
            Ncm_spr_set_diarog_b();
            Ncm_spr_DNAS_ERR_set();
        }
        break;
    case 0x25:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0x13B;
            NCS16(6) = 0xD2;
        }
        break;
    case 0x26:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCU8(0x89) = 0x32;
            NCS16(4) = 0xA;
            Ncm_spr_kill(0x02000000);
        } else if (NCS16(6) >= NCS16(4) && net_shot_ok_ck(2) != 0) {
            NCU8(0x89) = 0x32;
            NCS16(4) = 0xA;
            Ncm_spr_kill(0x02000000);
        }
        break;
    case 0x28:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0xA;
            Ncm_spr_D_MENU_set(0xB, 1);
        }
        break;
    case 0x29:
        Ncm_mssage_disp_req(0x43);
        Ncm_menu_disp_req(4);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(4) = 0xE10;
            NCU8(0x89) = NCU8(0x89) + 1;
        }
        break;
    case 0x2A:
        Ncm_mssage_disp_req(0x43);
        Ncm_menu_disp_req(4);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            NCS16(0xA) = 0;
            NCS16(4) = 0xA;
            NCU8(0x89) = NCU8(0x89) + 1;
            Net_fade_execute(0, 0x14, 1);
        } else {
            r = net_swdata(0x40000);
            if (r & 0x2000) {
                if (NCS16(0xA) != 0) {
                    NCS16(0xA) = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && NCS16(0xA) != 1) {
                NCS16(0xA) = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (NCS16(0xA) == 0) {
                    NCS16(4) = 0xA;
                    NCU8(0x89) = NCU8(0x89) + 1;
                    Net_fade_execute(0, 0x14, 1);
                } else {
                    NCU8(0x89) = 0x2D;
                    NCS16(4) = 0xA;
                }
            } else if (net_shot_ng_ck(0x40000) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                NCU8(0x89) = 0x2D;
                NCS16(4) = 0xA;
            }
        }
        break;
    case 0x2B:
        if (Net_fade_check(1) == 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill_all();
            PatchInitCS();
        }
        break;
    case 0x2C:
        ret = -1;
        break;
    case 0x2D:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = 2;
            NCS16(4) = 0xA;
        }
        break;
    case 0x32:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCU8(0x89) = NCU8(0x89) + 1;
            NCS16(4) = 0xA;
            Ncm_spr_D_MENU_set(0xB, 2);
        }
        break;
    case 0x33:
        Ncm_mssage_disp_req(0x42);
        Ncm_menu_disp_req(4);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(4) = 0xE10;
            NCU8(0x89) = NCU8(0x89) + 1;
        }
        break;
    case 0x34:
        Ncm_mssage_disp_req(0x42);
        Ncm_menu_disp_req(4);
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCS16(0xA) = 0;
            NCS16(4) = 0xA;
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
        } else {
            r = net_swdata();
            if (r & 0x2000) {
                if (NCS16(0xA) != 0) {
                    NCS16(0xA) = 0;
                    net_set_se_cur();
                }
            } else if ((r & 0x1000) && NCS16(0xA) != 1) {
                NCS16(0xA) = 1;
                net_set_se_cur();
            }
            if (net_shot_ok_ck(1) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill(0x100000);
                if (NCS16(0xA) == 0) {
                    Ncm_spr_kill(0x200);
                    NCS16(4) = 0xA;
                    NCU8(0x89) = NCU8(0x89) + 1;
                } else {
                    NCU8(0x89) = 0x37;
                    NCS16(4) = 0xA;
                }
            } else if (net_shot_ng_ck(0x40000) != 0) {
                Ncm_spr_kill(0x40000);
                Ncm_spr_kill(0x80000);
                Ncm_spr_kill2(0x100000);
                NCU8(0x89) = 0x37;
                NCS16(4) = 0xA;
            }
        }
        break;
    case 0x35:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x89) = NCU8(0x89) + 1;
            Ncm_spr_kill_all();
            PatchInitCS();
        }
        break;
    case 0x36:
        ret = -1;
        break;
    case 0x37:
        t = NCS16(4) - 1;
        NCS16(4) = t;
        if (t <= 0) {
            NCU8(0x8A) = 0;
            NCU8(0x89) = 9;
            NCS16(4) = 0xA;
            NCS16(0xA) = 0;
        }
        break;
    }
    return ret;
}
