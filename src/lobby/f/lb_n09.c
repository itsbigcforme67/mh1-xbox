/* lb_n09 - lobby misc 0x005D58C0-0x005D59D8: Clear_lobby_ram (reset the lobby work areas: lb_sys, player table, client/network work).
   Whole file in lb_n.c. The status bytes written through D_3F3415 are an extern object of their own: that keeps the compiler from
   re-reading cw after the store (same trick as D_39DAD2 in lb_tu_ib.c). */
#include "lobby_f.h"
extern u8 D_3E55F0[], D_3E5FF0[], D_3E69F0[], D_3E73F0[], D_3E7DF0[], D_3E87F0[], D_3E91F0[];
extern u8 D_3F3415[16];
extern PLW player_work[];

void Clear_lobby_ram(void) {
    flMemset(&lb_sys, 0, 0x90);
    client_work[0x2BFE] = 0;
    lb_player[0].pl = &player_work[0];
    lb_player[1].pl = (PLW *)D_3E55F0;
    lb_player[2].pl = (PLW *)D_3E5FF0;
    lb_player[3].pl = (PLW *)D_3E69F0;
    lb_player[4].pl = (PLW *)D_3E73F0;
    lb_player[5].pl = (PLW *)D_3E7DF0;
    lb_player[6].pl = (PLW *)D_3E87F0;
    lb_player[7].pl = (PLW *)D_3E91F0;
    client_work[0x2BFF] = 0;
    client_work[0x2C00] = 0;
    client_work[0x2C01] = 0;
    client_work[0x2C02] = 0;
    client_work[0x2C03] = 0;
    client_work[0x2C04] = 0;
    client_work[0x2C05] = 0;
    init_set_work();
    pNet = network_work;
    Lbc_init_network_work();
    D_3F3415[0] = 0;
    cw = client_work;
    lbSendInterval = 0xF;
}

