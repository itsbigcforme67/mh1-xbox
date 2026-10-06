/* lb_bz104 - lobby UI/client 0x005B7460-0x005B74B8: Lbc_set_prim (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lb_prim[];
extern char D_3EBC90[];
extern char D_3EBCB0[];

void Lbc_set_prim(s32 arg0, s32 arg1, s32 arg2) {
    F(int, pNet, 0x14) = (int)&lb_prim;
    F(s32, F(int, pNet, 0x14), 0x14) = arg0;
    F(int, pNet, 0x18) = (int)&D_3EBC90;
    F(s32, F(int, pNet, 0x18), 0x14) = arg1;
    F(int, pNet, 0x1C) = (int)&D_3EBCB0;
    F(s32, F(int, pNet, 0x1C), 0x14) = arg2;
}
