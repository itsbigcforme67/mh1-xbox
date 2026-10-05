/* lb_e03 - lobby members/cockpit/icons 0x005CB830-0x005CB980: Lb_equip_set, Lb_cockpit_move. Whole file in lb_e.c. */
#include "lobby_f.h"










void Lb_equip_set(u16 *p, int a1) {
    Set_equip_idx(a1);
    Set_userdata(p);
    ((u8 *)&lb_sys)[0x78] = 1;
    Lb_set_mini_data((u8 *)&lbCommer[p[6]] + 0x1C);
    Lb_set_mini_data(CWPLAYER(p[6]) + 0x1346);
}

void Lb_cockpit_move(void) {
    flSetRenderState(0x6D, 7);
    flSetRenderState(0x6C, 1);
    flSetRenderState(1, 1);
    InitRenderState(1);
    switch (lb_sys.x68) {
    case 0:
    case 0xF:
        lb_sys.x6C = 0;
        if (lb_sys.x68 == 0) {
            Lbc_set_prim(lb_disp_name, Lb_put_help, 0);
        } else {
            Lbc_set_prim(lb_disp_name, 0, 0);
        }
        break;
    }
    flSetRenderState(0x6D, 3);
}
