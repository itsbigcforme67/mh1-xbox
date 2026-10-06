/* PC note: the shop-list fields at +0x24 (lh) and +0x26 (lhu) are 16-bit in the asm (0x53B764, 0x53B7B4); reading them as s32 crashed the PC forge screen. */
#include "lobby_s.h"
extern char kakou_tbl[];
extern char shopList2[];
extern char buki_sei_tbl[];
extern char bou_sei_tbl[];
extern char lit_1225_006555D0[];
extern char lb_shop_msg[];
extern char lb_shop_msg[];
extern char lb_shop_msg[];
extern char lit_1226_006555D8[];
extern char lit_1227_006555E0[];
void lb_process_drawHelp(void) {
    s16 var_a1;
    s32 temp_a3;
    s32 temp_v1;
    s32 var_s3;
    s32 var_s4_2;
    int var_s0;
    int var_s1_2;
    int var_s3_2;
    u16 temp_a0;
    u16 temp_v0;
    u16 var_s1;
    u16 var_s2;
    int temp_t1;
    int var_s0_2;
    int var_s2_2;
    int var_s4;
    int var_s5;

    var_a1 = 0;
    var_s5 = 0;
    var_s4 = 0;
    var_s0 = 0;
    temp_v1 = lbShop.cur * 0x28;
    var_s3 = (int)lbShop.list + temp_v1;
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            var_s3 = (int)lbShop.list + (lbShop.x70 * 0x28);
            temp_t1 = (int)lbShop.tbl + (lbShop.x70 * 8);
            var_s1 = F(u16, temp_t1, 4);
            var_s2 = F(u16, temp_t1, 0);
            temp_a3 = var_s1 * 0x18;
            var_s5 = (int)&kakou_tbl + temp_a3;
            if (var_s2 == 7) {
                var_s1 = F(u16, &lbShop, 0x56);
                var_a1 = value_result(F(u16, &lbShop, 0x58), F(u16, (temp_v1 + (int)lbShop.list), 0x26), 7, temp_a3) & 0xFFFF;
                var_s0 = 1;
            }
            if (*(s16 *)(&shopList2[0x24] + (lbShop.x70 * 0x28)) == 2) {
                Lb_draw_square(0x11F, 0xFC, 0x141, 2);
                Lb_put_my_job();
                return;
            }
            goto block_12;
        }
        var_s4 = (int)&buki_sei_tbl + (*(u16 *)((int)&shopList + 0x26 + temp_v1) * 0x18)   /* lhu (asm 0x53B7B4) */;
        var_s1 = F(u16, var_s4, 2);
        var_s2 = (u16) F(u8, var_s4, 0);
        if (*(s16 *)((int)&shopList + 0x24 + (lbShop.x70 * 0x28)) == 2) {
            Lb_draw_square(0x11F, 0xFC, 0x141, 2);
            Lb_put_my_job();
            return;
        }
        goto block_12;
    }
    var_s4 = (int)&bou_sei_tbl + (*(u16 *)((int)&shopList + 0x26 + temp_v1) * 0x18)   /* lhu (asm 0x53B7B4) */;
    var_s1 = F(u16, var_s4, 2);
    var_s2 = (u16) F(u8, var_s4, 0);
    if (*(s16 *)((int)&shopList + 0x24 + (lbShop.x70 * 0x28)) == 2) {
        Lb_draw_square(0x11F, 0xFC, 0x141, 2);
        Lb_put_my_job();
        return;
    }
block_12:
    F(s16, &lbShop, 0x5E) = var_a1;
    F(s8, &lbShop, 0x5B) = (s8) var_s2;
    F(s8, &lbShop, 0x5A) = 1;
    F(u16, &lbShop, 0x5C) = var_s1;
    switch (lbShop.x1C) {       /* irregular */
    case 1:
        if ((var_s1 & 0xFFFF) != 0x3E7) {
            EquipmentCompareWindow((int)&lbShop + 0x54, (int)&lbShop + 0x5A, 0x126, 0x3C);
            return;
        }
    case 0:
        Lb_draw_square(0x11F, 0xFC, 0x141, 2);
        if (((s8)var_s0) == 0) {
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)var_s2);
            var_s4_2 = var_s1 & 0xFFFF;
            if (var_s4_2 == 0x3E7) {
                Lb_put_itemRare(0x12A, 0x136, 4);
            } else {
                Lb_put_itemRare(0x12A, 0x136,  (Get_equip_rare(var_s2 & 0xFF, var_s1) << 0x38) >> 0x38);
            }
        } else {
            Lb_put_armorIcon(0x122, 0x102, 0x36, 7);
            var_s4_2 = var_s1 & 0xFFFF;
            if (var_s4_2 == 0x3E7) {
                Lb_put_itemRare(0x12A, 0x136, 4);
            } else {
                Lb_put_itemRare(0x12A, 0x136,  (Get_equip_rare(var_s2 & 0xFF, var_s1) << 0x38) >> 0x38);
            }
        }
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x168, 0x104);
        if (var_s4_2 == 0x3E7) {
            font_set_palette(Equip_moji_color_rare(4));
        } else {
            font_set_palette(Equip_moji_color_rare(Get_equip_rare(var_s2 & 0xFF, var_s1)));
        }
        font_print(&lit_1225_006555D0, var_s3 + 4);
        font_set_palette(0);
        if (lbShop.x15 != 5) {
            if (lbShop.x15 == 6) {
                goto block_32;
            }
        } else {
block_32:
            if (var_s4_2 != 0x3E7) {
                Lb_put_button(0x212, 0x12F, 3);
                Lb_put_msg_type2(&lb_shop_msg[0x20]);
                Lb_put_job_limit((u8) var_s2);
                font_set_palette(0);
            }
            if (var_s0 == 0) {
                Lb_put_button(0x190, 0x12F, 6);
                Lb_put_msg_type2((int)&lb_shop_msg + 0x30);
            }
        }
        Lb_put_msg_type2((int)&lb_shop_msg + 0x18);
        flfntSetSize(0x1C, 0x14);
        if (var_s4_2 == 0x3E7) {
            font_print_ex(0x1B0, 0x11A, 0, &lit_1226_006555D8);
        } else {
            font_print_ex(0x1B0, 0x11A, 0, &lit_1227_006555E0, Lb_get_armor_num((u8) var_s2, var_s1));  /* asm 0x53BB60: a0 s2, a1 s1, t0 = result */
        }
        flfntSetSize(0x12, 0x12);
        Lb_put_my_job();
        return;
    case 2:
        if ((var_s1 & 0xFFFF) == 0x3E7) {
            font_set_palette(Equip_moji_color_rare(4, lbShop.x1C, 1, 2));
        } else {
            font_set_palette(Equip_moji_color_rare(Get_equip_rare(var_s2 & 0xFF, var_s1, 1, 2)));
        }
        Lb_put_materialBase();
        font_set_palette(0);
        if (((s8)var_s0) == 0) {
            Lb_put_armorIcon(0x130, 0xD0, 0x20, (s16)var_s2);
        }
        var_s3_2 = 0;
        var_s2_2 = var_s5;
        var_s1_2 = 0xF4;
        var_s0_2 = var_s4;
        do {
            if (var_s5 != 0) {
                temp_a0 = F(u16, var_s2_2, 0);
                if ((temp_a0 != 0) && (( (var_s3_2 << 0x30) >> 0x30) != 3)) {
                    Lb_put_materialItem( (var_s1_2 << 0x30) >> 0x30, (s16)temp_a0, F(s16, var_s2_2, 2));
                }
            } else if (var_s4 != 0) {
                temp_v0 = F(u16, var_s0_2, 4);
                if (temp_v0 != 0) {
                    Lb_put_materialItem( (var_s1_2 << 0x30) >> 0x30, (s16)temp_v0, F(s16, var_s0_2, 6));
                }
            }
            var_s2_2 += 4;
            var_s3_2 =  ((var_s3_2 + 1) << 0x30) >> 0x30;
            var_s1_2 += 0x14;
            var_s0_2 += 4;
        } while (var_s3_2 < 4);
        Lb_put_my_job();
        return;
    }
}
