#include "lobby_a.h"
extern char D_3C7004[];
extern char shopList[];
extern char shop_armor_question[];
extern char shop_armor01_tag[8];
extern char lb_armor_tag_decide01[];
extern char shopTbl[];
extern char shop_armor_help[];
extern char shopList[];
extern char shopTbl[];
extern char User_data[];
extern char User_data[];
void lb_armor_tag_decide00(void) {
    int var_s0;
    int var_s1;
    int var_s2;
    s32 var_s3;

    var_s2 = (int)&D_3C7004;
    var_s1 = (int)&shopList;
    if (F(s8, &lbShop, 0x19) == 0) {
        F(s8, &lbShop, 0x18) = 0;
        F(s8, &lbShop, 0x16) = 1;
        F(s8, &lbShop, 0x8F) = 1;
        F(int, &lbShop, 0x38) = (int)&shop_armor_question;
        F(int, &lbShop, 0x48) = (int)&shop_armor01_tag;
        F(int, &lbShop, 0x28) = (int)&lb_armor_tag_decide01;
        return;
    }
    F(s8, &lbShop, 0x16) = 0;
    F(s8, &lbShop, 0x8F) = 0;
    F(int, &lbShop, 0x64) = (int)&shopTbl;
    F(int, &lbShop, 0x38) = 0;
    F(int, &lbShop, 0x28) = 0;
    F(s8, &lbShop, 0x18) = 0;
    F(s32, &lbShop, 0x4C) = F(s32, &shop_armor_help, 0x18);
    memset(&shopList, 0, 0x5000);
    var_s3 = 0;
    var_s0 = (int)&shopTbl;
    do {
        if (Warehouse_space_ck(&User_data, var_s3) == 1) {
            F(s16, var_s1, 0x24) = 2;
            F(s32, var_s0, 0) = 0;
            F(s32, var_s0, 4) = 0;
        } else {
            strcpy(var_s1 + 4, Get_equip_name(F(u8, var_s2, 1), F(u16, var_s2, 2)));
            F(s32, var_s1, 0) = Get_equip_kaitori(F(u8, var_s2, 1), F(u16, var_s2, 2));
            if (Now_equip_ck(&User_data, var_s3) == 1) {
                F(s16, var_s1, 0x24) = 1;
            } else {
                F(s16, var_s1, 0x24) = 0;
            }
            F(s32, var_s0, 0) = F(u8, var_s2, 1);
            F(s32, var_s0, 4) = F(u16, var_s2, 2);
        }
        var_s3 += 1;
        var_s2 += 6;
        var_s1 += 0x28;
        var_s0 += 8;
    } while (var_s3 < 0x40);
    F(s32, &lbShop, 0x80) = 0x40;
    F(s8, &lbShop, 0x6D) = 0xA;
    F(s32, &lbShop, 0x84) = 1;
}
