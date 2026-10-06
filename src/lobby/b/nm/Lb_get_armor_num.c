#include "lobby_a.h"
extern char D_3C7004[];
void Lb_get_armor_num(s32 arg0, s32 arg1) {
    int var_a1;
    s32 var_a3;

    var_a3 = 0;
    var_a1 = (int)&D_3C7004;
    do {
        if ((F(u8, var_a1, 1) == (arg0 & 0xFFFF)) && (F(u16, var_a1, 2) == (arg1 & 0xFFFF))) {

        }
        var_a3 += 1;
        var_a1 += 6;
    } while (var_a3 < 0x40);
}
