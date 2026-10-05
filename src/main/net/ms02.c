/* SLPM_654.95 0x002687C0-0x002687E0: ms_net_patch_set_init .. ms_net_patch_set_init. See ms_nm.c. */
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





void ms_net_patch_set_init(void) {
    net_common_w.x89 = 0;
    net_common_w.x8A = 0;
    net_common_w.x11 = 1;
}
