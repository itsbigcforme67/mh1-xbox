#include "lobby_s.h"
extern s8 randTblNo;
extern char randTbl00[];
extern char randTbl01[];
extern char randTbl02[];
extern char randTbl03[];
extern char randTbl04[];
void random_stack(void) {
    int var_s1;
    s32 temp_hi;
    s32 var_a1;
    s8 temp_v1;
    s8 var_s0;
    temp_v1 = randTblNo;
    switch (temp_v1) {                              /* irregular */
    case 0:
        var_s1 = (int)&randTbl00;
        var_s0 = 6;
        break;
    case 1:
        var_s1 = (int)&randTbl01;
        var_s0 = 6;
        break;
    case 2:
        var_s1 = (int)&randTbl02;
        var_s0 = 6;
        break;
    case 3:
        var_s1 = (int)&randTbl03;
        var_s0 = 6;
        break;
    case 4:
        var_s1 = (int)&randTbl04;
        var_s0 = 7;
        break;
    }
    temp_hi = (ran_suu(1) & 0xFFFF) % 100;
    if (temp_hi < 5) {
        var_a1 = F(s32, var_s1, 8);
    } else if (temp_hi < 0x1E) {
        var_a1 = F(s32, var_s1, 4);
    } else {
        var_a1 = F(s32, var_s1, 0);
    }
    F(s8, &lbShop, 0x5B) = var_s0;
    F(s16, &lbShop, 0x5C) = (s16) var_a1;
    lbShop.tbl[(lbShop.cur) * 2] = var_s0;
    F(s32, ((lbShop.cur * 8) + (int)lbShop.tbl), 4) = var_a1;
}
