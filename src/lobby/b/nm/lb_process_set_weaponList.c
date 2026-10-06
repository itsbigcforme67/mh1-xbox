#include "lobby_a.h"
extern char shopList[];
extern char shopTbl[];
extern char D_3C7004[];
extern char User_data[];
extern char shopList[];
extern char shopTbl[];
extern char buki_sei_tbl[];
extern char User_data[];
extern char shop_process2_help[];
extern char lb_process_kyoukaListProg[];
extern char shopList[];
extern char shopTbl[];
extern char shopTbl[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char lb_process_kyoukaListProg[];
extern char shopList[];
void lb_process_set_weaponList(void) {
    u16 spAA;
    u8 spA9;
    int spA8;
    int var_s1;
    int var_s2_2;
    int var_s3_2;
    int var_s4;
    int var_s5;
    s16 var_v1;
    s32 temp_s6;
    s32 temp_v0;
    s32 var_s0_2;
    s32 var_s1_2;
    int temp_s0;
    int var_s3;
    s8 var_a0;
    u16 temp_a1_2;
    u32 var_s2;
    u32 var_v0;
    int var_s0;
    u8 temp_a1;
    u8 temp_v1;

    var_s5 = (int)&shopList;
    var_s4 = (int)&shopTbl;
    temp_a1 = game_w.master;
    var_s1 = (int)&D_3C7004;
    var_s2 = 0;
    temp_s0 = (Warehouse_search_space(&User_data, temp_a1) & 0xFF) == 0xFF;
    memset(&shopList, 0, 0x5000);
    if (F(s8, &lbShop, 0x1A) == 0) {
        F(u32, &lbShop, 0x80) = 0U;
        F(int, &lbShop, 0x64) = (int)&shopTbl;
        var_s3 = 0;
        if (*(u8 *)0x333030 != 0xFF) {
            var_s0 = (int)&buki_sei_tbl;
            do {
                temp_v0 = Seisan_ok_ck(1,  (var_s3 << 0x30) >> 0x30, 0);
                spA9 = F(u8, var_s0, 0);
                spAA = F(u16, var_s0, 2);
                if ((temp_v0 != 0) && ((1 << F(u8, ((int)&player_work + (temp_a1 * 0xA00)), 0x11)) & (Get_equip_bit(&User_data, &spA8) & 0xFF))) {
                    temp_a1_2 = F(u16, var_s0, 2);
                    temp_s6 = Seisan_ok_ck(1,  (var_s3 << 0x30) >> 0x30, 1);
                    if (temp_a1_2 == 0x3E7) {
                        var_v0 = 0x3E8;
                        var_s1_2 = F(s32, &shop_process2_help, 0x1C);
                    } else {
                        var_s1_2 = Get_equip_name(F(u8, var_s0, 0));
                        var_v0 = Get_equip_price(F(u8, var_s0, 0), F(u16, var_s0, 2)) >> 1;
                    }
                    F(u32, var_s5, 0) = var_v0;
                    strcpy(var_s5 + 4, var_s1_2);
                    F(s16, var_s5, 0x26) = (s16) var_s3;
                    F(s32, var_s4, 0) = F(u8, var_s0, 0);
                    F(s32, var_s4, 4) = F(u16, var_s0, 2);
                    if (((s8)temp_s0) == 1) {
                        F(s16, var_s5, 0x24) = 1;
                    } else if (((u32) *(u32 *)0x3C6FE0 < (u32) F(u32, var_s5, 0)) || (temp_s6 != 2)) {
                        if ((Seisan_ok_ck(1,  (var_s3 << 0x30) >> 0x30, 0) == 2) && (F(s16, var_s5, 0x24) == 0)) {
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
                var_s0 += 0x18;
                var_s3 += 1;
            } while ((*(u8 *)var_s0) != 0xFF);
        }
        F(u32, &lbShop, 0x80) = var_s2;
        F(s32, &lbShop, 0x84) = 0;
        var_a0 = (var_s2 / 7) + (var_s2 >> 0x1F);
        F(s8, &lbShop, 0x18) = 0;
        if ((var_s2 % 7) != 0) {
            var_a0 += 1;
        }
        F(s8, &lbShop, 0x6D) = var_a0;
        F(int, &lbShop, 0x30) = (int)&lb_process_kyoukaListProg;
        F(int, &lbShop, 0x50) = (int)&shopList;
        return;
    }
    var_s2_2 = (int)&shopTbl;
    F(int, &lbShop, 0x64) = (int)&shopTbl;
    var_s0_2 = 0;
    var_s3_2 = (int)&User_data;
    do {
        var_v1 = 2;
        if (Warehouse_space_ck(&User_data, var_s0_2) == 1) {
            goto block_38;
        }
        temp_v1 = F(u8, var_s3_2, 0x45);
        if ((temp_v1 == 6) || (temp_v1 == 7)) {
            strcpy(var_s5 + 4, Get_equip_name(F(u8, var_s1, 1), F(u16, var_s1, 2)));
            F(s32, var_s5, 0) = -1;
            Now_equip_ck(&User_data, var_s0_2);
            F(s16, var_s5, 0x24) = 0;
        } else {
            strcpy(var_s5 + 4, Get_equip_name(F(u8, var_s1, 1), F(u16, var_s1, 2)));
            var_v1 = 1;
            F(s32, var_s5, 0) = -1;
block_38:
            F(s16, var_s5, 0x24) = var_v1;
        }
        var_s0_2 += 1;
        var_s5 += 0x28;
        var_s3_2 += 6;
        F(s32, var_s2_2, 0) = F(u8, var_s1, 1);
        F(s32, var_s2_2, 4) = F(u16, var_s1, 2);
        var_s1 += 6;
        var_s2_2 += 8;
    } while (var_s0_2 < 0x40);
    F(u32, &lbShop, 0x80) = 0x40U;
    F(s8, &lbShop, 0x6D) = 0xA;
    F(int, &lbShop, 0x30) = (int)&lb_process_kyoukaListProg;
    F(int, &lbShop, 0x50) = (int)&shopList;
    F(s32, &lbShop, 0x84) = 1;
    F(s8, &lbShop, 0x18) = 1;
}
