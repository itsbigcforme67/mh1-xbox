/* lb_bz60 - lobby UI/client 0x005C26E0-0x005C2754: server_select_sub_00 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void server_select_sub_00(u8 *arg0) {
    Ncm_mmbb_spr_load();
    Ncm_mmbb_spr_create();
    F(u8, arg0, 2) = (u8) (F(u8, arg0, 2) + 1);
    F(s8, arg0, 8) = 0;
    Lbc_set_prim(0, 0, 0);
    fade_set(2);
    cnLbc_DispNetConnectTime();
    SetSceneTitle(0, 2);
    SetHelpLineMsg(0, 3);
}
