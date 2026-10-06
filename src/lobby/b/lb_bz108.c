/* lb_bz108 - lobby UI/client 0x005B9BB0-0x005B9C40: lbc_top_menu_00 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char ClassInfo[];
extern char PlazaInfo[];

void lbc_top_menu_00(void) {
    int var_v1;
    s32 var_a0;

    F(s8, (u8 *)cw, 0x2C33) = 5;
    F(s8, &ClassInfo, 0) = 0;
    F(s8, &ClassInfo, 4) = 0;
    F(s8, &ClassInfo, 8) = 0;
    memset(&PlazaInfo, 0, 0xD98);
    var_a0 = 0;
    var_v1 = (int)&PlazaInfo;
    do {
        F(s8, var_v1, 0x10) = 0;
        F(s8, var_v1, 0x16C) = 0;
        var_a0 += 5;
        F(s8, var_v1, 0x2C8) = 0;
        F(s8, var_v1, 0x424) = 0;
        F(s8, var_v1, 0x580) = 0;
        var_v1 += 0x6CC;
    } while (var_a0 < 0xA);
    Lbc_init_network_work(var_a0);
    F(s8, pNet, 6) = 0;
}
