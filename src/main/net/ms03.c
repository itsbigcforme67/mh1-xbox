/* ms03 - f_ms 0x002685C0-0x002687B4: ms_network_bb_last_time_save. Whole file in ms_nm.c. */
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
