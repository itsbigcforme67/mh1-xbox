#include "lobby_a.h"
extern char D_3C738C[];
extern char D_3C738C[];
extern char D_3C738C[];
void Lb_make_mySrcEquip(int arg0) {
    int var_at;
    int temp_v1;
    u8 var_v1;
    int temp_a2;

    F(s8, &lbShop, 0x55) = (s8) arg0;
    temp_v1 =  (arg0 << 0x30) >> 0x30;
    switch (temp_v1) {
    case 6:
    case 7:
        temp_a2 = (int)&lbShop + 0x54;
        F(s16, &lbShop, 0x54) = (s16) F(s16, &D_3C738C, 0);
        F(s16, temp_a2, 2) = (s16) F(s16, &D_3C738C, 2);
        F(s16, temp_a2, 4) = (s16) F(s16, &D_3C738C, 4);
        break;
    case 2:
        var_v1 = *(u8 *)0x3C7393;
        var_at = (int)&lbShop + 0x56;
block_13:
        (*(s16 *)var_at) = (s16) var_v1;
        break;
    case 3:
        var_v1 = *(u8 *)0x3C7394;
        var_at = (int)&lbShop + 0x56;
        goto block_13;
    case 4:
        var_v1 = *(u8 *)0x3C7395;
        var_at = (int)&lbShop + 0x56;
        goto block_13;
    case 5:
        var_v1 = *(u8 *)0x3C7396;
        var_at = (int)&lbShop + 0x56;
        goto block_13;
    case 0:
        var_v1 = *(u8 *)0x3C7392;
        var_at = (int)&lbShop + 0x56;
        goto block_13;
    }
    F(s16, &lbShop, 0x54) = 1;
}
