#include "lobby_s.h"
extern s32 armorIndex;
extern char shopList2[];
extern char D_3C7006[];
extern char shopList2[];
extern char shopTbl[];
extern char shopList2[];
extern char shopTbl[];
extern char kakou_tbl[];
extern char D_3C7005[];
extern char kakou_tbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char shopTbl[];
extern char D_3C7006[];
extern char User_data[];
extern char shopList2[];
extern char shopList2[];
extern char shopList2[];
extern char shopList2[];
extern char shopList2[];
extern char shopList2[];
extern char shopList2[];
extern char User_data[];
extern char gun_kyouka_tbl[];
extern char gun_kyouka_tbl[];
extern char User_data[];
extern char gun_kyouka_tbl[];
extern char gun_kyouka_tbl[];
extern char User_data[];
extern char gun_kyouka_tbl[];
extern char gun_kyouka_tbl[];
extern char lvup_price[];
void lb_process_make_kyoukaList(void) {
    int var_s1;
    int var_s2;
    s16 var_v1;
    s16 var_v1_2;
    s32 temp_a1;
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s4;
    s32 var_s5;
    int var_s0;
    u16 temp_a1_2;
    u16 temp_s3;
    u32 temp_s1;
    int temp_s2;
    int var_s3;

    var_s2 = (int)&shopList2;
    temp_s0 = armorIndex;
    temp_s3 = *(s32 *)((int)&D_3C7006 + (temp_s0 * 6));
    memset(&shopList2, 0, 0x5000);
    lbShop.count = 0;
    var_s1 = (int)&shopTbl;
    lbShop.list = (void *)shopList2;
    lbShop.x6D = 1;
    lbShop.x6C = 0;
    lbShop.tbl = (void *)shopTbl;
    var_s3 = (int)&kakou_tbl + (temp_s3 * 0x18);
    temp_a1 = temp_s0 * 6;
    var_s5 = 0;
    if (*(s32 *)((int)&D_3C7005 + temp_a1) == 6) {
        do {
            F(s16, var_s2, 0x24) = 0;
            temp_a1_2 = F(u16, var_s3, 0xC);
            if (temp_a1_2 != 0) {
                strcpy(var_s2 + 4, Get_equip_name(6, temp_a1_2));
                F(u32, var_s2, 0) = (u32) (Get_equip_price(6, F(u16, var_s3, 0xC)) >> 1);
                F(s16, var_s2, 0x24) = 0;
                var_s4 = 0;
                F(u16, var_s2, 0x26) = (u16) F(u16, var_s3, 0xC);
                var_s0 = (int)&kakou_tbl + (F(u16, var_s3, 0xC) * 0x18);
                do {
                    if (((*(u16 *)var_s0) != 0) && (check_items(var_s0, 1) != 1)) {
                        if ((check_items(var_s0, 0) == 1) && (var_v1 = 3, (F(s16, var_s2, 0x24) != 1))) {

                        } else {
                            var_v1 = 1;
                        }
                        F(s16, var_s2, 0x24) = var_v1;
                    }
                    var_s4 += 1;
                    var_s0 += 4;
                } while (var_s4 < 3);
                if ((u32) *(u32 *)0x3C6FE0 < (u32) F(u32, var_s2, 0)) {
                    var_v1_2 = 4;
                    if (F(s16, var_s2, 0x24) == 3) {

                    } else {
                        var_v1_2 = 1;
                    }
                    F(s16, var_s2, 0x24) = var_v1_2;
                }
                F(s32, var_s1, 0) = 6;
                F(s32, var_s1, 4) = F(u16, var_s3, 0xC);
            } else {
                F(s16, var_s2, 0x24) = 2;
            }
            var_s5 += 1;
            var_s2 += 0x28;
            var_s3 += 2;
            var_s1 += 8;
            lbShop.count = (lbShop.count + 1);
        } while (var_s5 < 5);
        return;
    }
    F(s32, &shopTbl, 4) = 0;
    F(s32, &shopTbl, 0xC) = 0;
    F(s32, &shopTbl, 0) = 7;
    F(s32, &shopTbl, 8) = 7;
    F(s32, &shopTbl, 0x10) = 7;
    F(s32, &shopTbl, 0x14) = 0;
    F(s32, &shopTbl, 0x18) = 7;
    F(s32, &shopTbl, 0x1C) = 0;
    temp_s1 = Get_equip_price(7, *(s32 *)((int)&D_3C7006 + temp_a1));
    temp_v0 = Get_Gun_level(&User_data, (s16)temp_s0);
    if (temp_v0 >= 4) {
        lbShop.count = 3;
    } else {
        lbShop.count = 4;
        F(u32, &shopList2, 0) = (u32) ((temp_s1 / 10) * *(s32 *)((int)&lvup_price + (temp_v0 * 4)));
        if ((u32) *(u8 *)0x3C6FE0 < (u32) F(u32, &shopList2, 0)) {
            F(s16, &shopList2, 0x24) = 1;
        } else {
            F(s16, &shopList2, 0x24) = 0;
        }
        F(s16, &shopList2, 0x26) = 0;
        sprintf((int)&shopList2 + 4, F(s32, &gun_kyouka_tbl, 0));
        var_s2 = (int)&shopList2 + 0x28;
    }
    if (Gun_option_ck(&User_data, (s16)temp_s0, 0x10) == 1) {
        F(u32, var_s2, 0) = 0xAU;
        F(s16, var_s2, 0x26) = 2;
        sprintf(var_s2 + 4, F(s32, &gun_kyouka_tbl, 8));
    } else {
        F(u32, var_s2, 0) = (u32) ((temp_s1 / 10) * 4);
        F(s16, var_s2, 0x26) = 1;
        sprintf(var_s2 + 4, F(s32, &gun_kyouka_tbl, 4));
    }
    if ((u32) *(u8 *)0x3C6FE0 < (u32) F(u32, var_s2, 0)) {
        F(s16, var_s2, 0x24) = 1;
    } else {
        F(s16, var_s2, 0x24) = 0;
    }
    if (Gun_option_ck(&User_data, (s16)temp_s0, 0x20) == 1) {
        F(u32, var_s2, 0x28) = 0xAU;
        F(s16, var_s2, 0x4E) = 4;
        sprintf(var_s2 + 0x2C, F(s32, &gun_kyouka_tbl, 0x10));
    } else {
        F(u32, var_s2, 0x28) = (u32) ((temp_s1 / 10) * 3);
        F(s16, var_s2, 0x4E) = 3;
        sprintf(var_s2 + 0x2C, F(s32, &gun_kyouka_tbl, 0xC));
    }
    if ((u32) *(u8 *)0x3C6FE0 < (u32) F(u32, var_s2, 0x28)) {
        F(s16, var_s2, 0x4C) = 1;
    } else {
        F(s16, var_s2, 0x4C) = 0;
    }
    temp_s2 = var_s2 + 0x50;
    if (Gun_option_ck(&User_data, (s16)temp_s0, 0x40) == 1) {
        F(u32, var_s2, 0x50) = 0xAU;
        F(s16, temp_s2, 0x26) = 6;
        sprintf(temp_s2 + 4, F(s32, &gun_kyouka_tbl, 0x18));
    } else {
        F(u32, var_s2, 0x50) = (u32) ((temp_s1 / 10) * 3);
        F(s16, temp_s2, 0x26) = 5;
        sprintf(temp_s2 + 4, F(s32, &gun_kyouka_tbl, 0x14));
    }
    if ((u32) *(u8 *)0x3C6FE0 < (u32) F(u32, var_s2, 0x50)) {
        F(s16, temp_s2, 0x24) = 1;
        return;
    }
    F(s16, temp_s2, 0x24) = 0;
}
