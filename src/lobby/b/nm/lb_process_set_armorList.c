#include "lobby_s.h"
extern char User_data[];
extern char bou_sei_tbl[];
extern char lb_process_kyoukaListProg[];
extern s32 shopTbl[];

s32 lb_process_set_armorList(void) {
    u16 spAA;
    u8 spA9;
    int spA8;
    int var_s1;
    int var_s4;
    int var_s5;
    s32 temp_fp;
    s32 temp_s0;
    s32 temp_v0;
    int var_s3;
    int var_s6;
    s8 var_v1;
    u32 var_s2;
    u8 temp_a2;
    var_s5 = (int)&shopList;
    var_s4 = (int)&shopTbl;
    var_s2 = 0;
    temp_a2 = game_w.master;
    switch (lbShop.x1A) {       /* irregular */
    case 0:
        var_s6 = 2;
        break;
    case 1:
        var_s6 = 3;
        break;
    case 2:
        var_s6 = 4;
        break;
    case 3:
        var_s6 = 5;
        break;
    case 4:
        var_s6 = 0;
        break;
    }
    lbShop.count = 0U;
    lbShop.tbl = (void *)shopTbl;
    var_s3 = 0;
    lbShop.list = (void *)shopList;
    var_s1 = (int)&bou_sei_tbl;
    temp_fp = (Warehouse_search_space(&User_data, lbShop.x1A) & 0xFF) == 0xFF;
loop_31:
    if (F(u8, var_s1, 0) != 0xFF) {
        temp_v0 = Seisan_ok_ck(0,  (s16)(var_s3), 0);
        spA9 = F(u8, var_s1, 0);
        spAA = F(u16, var_s1, 2);
        if ((temp_v0 != 0) && ((1 << F(u8, ((int)&player_work + (temp_a2 * 0xA00)), 0x11)) & (Get_equip_bit(&User_data, &spA8) & 0xFF)) && (F(u8, var_s1, 0) == var_s6)) {
            temp_s0 = Seisan_ok_ck(0,  (s16)(var_s3), 1);
            strcpy(var_s5 + 4, Get_equip_name(F(u8, var_s1, 0), F(u16, var_s1, 2)));
            F(u32, var_s5, 0) = (u32) (Get_equip_price(F(u8, var_s1, 0), F(u16, var_s1, 2)) >> 1);
            F(s16, var_s5, 0x26) = (s16) var_s3;
            F(s32, var_s4, 0) = F(u8, var_s1, 0);
            F(s32, var_s4, 4) = F(u16, var_s1, 2);
            if (temp_fp == 1) {
                F(s16, var_s5, 0x24) = 1;
            } else if (((u32) *(u32 *)0x3C6FE0 < (u32) F(u32, var_s5, 0)) || (temp_s0 != 2)) {
                if ((Seisan_ok_ck(0,  (s16)(var_s3), 0) == 2) && (F(s16, var_s5, 0x24) == 0)) {
                    F(s16, var_s5, 0x24) = 3;
                } else {
                    F(s16, var_s5, 0x24) = 1;
                }
            } else {
                F(s16, var_s5, 0x24) = 0;
            }
            if (((u32) *(u8 *)0x3C6FE0 < (u32) F(u32, var_s5, 0)) && (F(s16, var_s5, 0x24) == 3)) {
                F(s16, var_s5, 0x24) = 4;
            }
            var_s5 += 0x28;
            var_s2 += 1;
            var_s4 += 8;
        }
        var_s1 += 0x18;
        var_s3 += 1;
        goto loop_31;
    }
    lbShop.count = var_s2;
    lbShop.x84 = 0;
    var_v1 = (var_s2 / 7) + (var_s2 >> 0x1F);
    lbShop.x18 = 0;
    if ((var_s2 % 7) != 0) {
        var_v1 += 1;
    }
    lbShop.x6D = var_v1;
    lbShop.f30 = (int (*)())lb_process_kyoukaListProg;
    return  (s16)(var_s6);
}
