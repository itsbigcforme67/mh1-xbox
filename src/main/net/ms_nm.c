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
