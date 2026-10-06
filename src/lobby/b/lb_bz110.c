/* lb_bz110 - lobby UI/client 0x005BA550-0x005BA604: Lbc_init_network_work (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lb_prim[];
extern char D_3EBC90[];
extern char D_3EBCB0[];
extern char D_3EBCD0[];
extern char text_lobby_trans_ot3[];

void Lbc_init_network_work(void) {
    memset(pNet, 0, 0x2C);
    F(int, pNet, 0x14) = (int)&lb_prim;
    F(int, pNet, 0x18) = (int)&D_3EBC90;
    F(int, pNet, 0x1C) = (int)&D_3EBCB0;
    F(int, pNet, 0x20) = (int)&D_3EBCD0;
    F(int, F(int, pNet, 0x20), 0x14) = (int)&text_lobby_trans_ot3;
    F(s32, F(int, pNet, 0x14), 0x18) = 0;
    F(s32, F(int, pNet, 0x18), 0x18) = 1;
    F(s32, F(int, pNet, 0x1C), 0x18) = 2;
    F(s32, F(int, pNet, 0x20), 0x18) = 3;
}
