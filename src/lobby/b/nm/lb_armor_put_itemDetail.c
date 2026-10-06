#include "lobby_s.h"
extern char User_data[];
extern char lit_551_00655880[];
extern char lb_shop_msg[];
extern char lit_585_00655888[];
extern char lb_shop_msg[];
void lb_armor_put_itemDetail(void) {
    s32 temp_a2;
    s32 temp_s2;
    s32 temp_v1_2;
    u16 var_s0;
    u16 var_s1;
    int temp_a1;
    int temp_v1;

    temp_a2 = lbShop.cur * 8;
    temp_v1 = (int)&User_data + (lbShop.cur * 0xC) + 0x44;
    temp_a1 = (int)lbShop.tbl + temp_a2;
    if (lbShop.mode == 0) {
        var_s0 = F(u16, temp_a1, 4);
        var_s1 = F(u16, temp_a1, 0);
    } else {
        var_s1 = (u16) F(u8, temp_v1, 1);
        var_s0 = F(u16, temp_v1, 2);
    }
    if (lbShop.x1C == 0) {
        Lb_draw_square(0x11F, 0xFC, 0x141, 2);
        temp_v1_2 = var_s1 & 0xFFFF;
        if (temp_v1_2 != 7) {
            if (temp_v1_2 == 6) {
                goto block_7;
            }
            reload_tex(1, 0x118);
            SetTextureStage(0x118);
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)var_s1);
            Lb_put_itemRare(0x12A, 0x136,  (Get_equip_rare(var_s1 & 0xFF,  var_s0) << 0x38) >> 0x38);
            reload_tex(1, 0x157);
            SetTextureStage(0x157);
        } else {
block_7:
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)var_s1);
            Lb_put_itemRare(0x12A, 0x136,  (Get_equip_rare(var_s1 & 0xFF,  var_s0) << 0x38) >> 0x38);
        }
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x168, 0x104);
        temp_s2 = Get_equip_name(var_s1 & 0xFF,  var_s0);
        font_set_palette(Equip_moji_color_rare(Get_equip_rare(var_s1 & 0xFF,  var_s0)));
        font_print(&lit_551_00655880, temp_s2);
        font_set_palette(0);
        Lb_put_msg_type2((int)&lb_shop_msg + 0x18);
        flfntSetSize(0x1C, 0x14);
        Lb_get_armor_num( var_s1,  var_s0);
        font_print_ex(0x1B0, 0x11A, 0, &lit_585_00655888);
        flfntSetSize(0x12, 0x12);
        if (lbShop.x15 == 5) {
            Lb_put_button(0x212, 0x12F, 3);
            Lb_put_msg_type2((int)&lb_shop_msg + 0x20);
        }
        Lb_put_job_limit( var_s1,  var_s0);
        Lb_put_my_job();
        return;
    }
    Lb_make_mySrcEquip((s16)var_s1, temp_a1, temp_a2, lbShop.cur);
    F(s8, &lbShop, 0x5B) = (s8) var_s1;
    F(s8, &lbShop, 0x5A) = 1;
    F(u16, &lbShop, 0x5C) = var_s0;
    F(s16, &lbShop, 0x5E) = 0;
    EquipmentCompareWindow((int)&lbShop + 0x54, (int)&lbShop + 0x5A, 0x126, 0x3C);
}
