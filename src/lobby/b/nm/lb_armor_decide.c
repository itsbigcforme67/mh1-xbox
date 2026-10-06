#include "lobby_s.h"
extern s8 armor_shop_r;
extern char User_data[];
extern char shop_armor_put_shopHelp[];
extern char shop_armor_question[];
extern char shop_armor_help[];
extern char shop_armor_help[];
extern char User_data[];
extern char shop_armor_help[];
extern char User_data[];
void lb_armor_decide(void) {
    u16 sp3A;
    u8 sp39;
    int sp38;
    u8 temp_v1_2;
    int temp_v1;

    lbShop.x1C = 0;
    switch (lbShop.mode) {       /* irregular */
    case 0:
        lbShop.x78 = 0;
        temp_v1 = (int)lbShop.tbl + (lbShop.cur * 8);
        sp39 = (u8) F(s32, temp_v1, 0);
        sp3A = (u16) F(s32, temp_v1, 4);
        if (Equip_ok_ck(&User_data, &sp38) == 1) {
            lbShop.x8F = 1;
            armor_shop_r = 0;
            lbShop.f40 = (void (*)())shop_armor_put_shopHelp;
            lbShop.f38 = (int (*)())shop_armor_question;
            lbShop.help = F(s32, &shop_armor_help, 8);
            temp_v1_2 = sp39;
            if (temp_v1_2 != 6) {
                if (temp_v1_2 == 7) {
                    goto block_8;
                }
            } else {
block_8:
                if (temp_v1_2 != *(u8 *)0x3C738D) {
                    lbShop.help = F(s32, &shop_armor_help, 0x10);
                }
            }
        } else {
            Warehouse_equip_stack(&User_data, sp39, sp3A, 0);
            lbShop.f40 = (void (*)())0;
            armor_shop_r = 0;
            lbShop.x8F = 0;
            lbShop.f38 = (int (*)())0;
            lbShop.help = F(s32, &shop_armor_help, 0xC);
            Lb_put_set01(0xC);
        }
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur);
        cnWrap_SoundRequest(8);
        return;
    case 1:
        Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur);
        Warehouse_equip_erase(&User_data, (u16) lbShop.cur);
        cnWrap_SoundRequest(8);
        lb_armor_tag_decide00();
        return;
    }
}
