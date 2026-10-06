/* lb_by162 - agent B 0x0053D450-0x0053D718: lb_armor_put_itemDetail. */
#include "lobby_s.h"
extern char User_data[];
extern char lit_551_00655880[];
extern char lit_585_00655888[];
extern char lb_shop_msg[];
void Lb_put_armorIcon(int x, int y, int z, s16 kind, s16 id);
void lb_armor_put_itemDetail(void) {
    int name;
    int kind;
    int id;
    u8 *ud;
    u8 *e;
    int c;

    c = lbShop.cur;
    e = (u8 *)lbShop.tbl + c * 8;
    ud = (u8 *)User_data + c * 12 + 0x44;
    if (lbShop.mode == 0) {
        kind = *(u16 *)e;
        id = *(u16 *)(e + 4);
    } else {
        kind = ud[1];
        id = *(u16 *)(ud + 2);
    }
    if (lbShop.x1C == 0) {
        Lb_draw_square(0x11F, 0xFC, 0x141, 2, 0xFF602020, 1);
        if ((kind & 0xFFFF) == 7 || (kind & 0xFFFF) == 6) {
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)kind, (s16)id);
            Lb_put_itemRare(0x12A, 0x136, (s8)Get_equip_rare((u8)kind, id));
        } else {
            reload_tex(1, 0x118);
            SetTextureStage(0x118);
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)kind, (s16)id);
            Lb_put_itemRare(0x12A, 0x136, (s8)Get_equip_rare(kind & 0xFF, id));
            reload_tex(1, 0x157);
            SetTextureStage(0x157);
        }
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x168, 0x104);
        name = Get_equip_name((u8)kind, id);
        font_set_palette(Equip_moji_color_rare(Get_equip_rare(kind & 0xFF, id)));
        font_print(lit_551_00655880, name);
        font_set_palette(0);
        Lb_put_msg_type2(lb_shop_msg + 0x18);
        flfntSetSize(0x1C, 0x14);
        font_print_ex(0x1B0, 0x11A, 0, lit_585_00655888, Lb_get_armor_num(kind, id));
        flfntSetSize(0x12, 0x12);
        if (lbShop.x15 == 5) {
            Lb_put_button(0x212, 0x12F, 3);
            Lb_put_msg_type2(lb_shop_msg + 0x20);
        }
        Lb_put_job_limit(kind, id);
        Lb_put_my_job();
    } else {
        Lb_make_mySrcEquip((s16)kind);
        lbShop.x5A[1] = kind;
        lbShop.x5A[0] = 1;
        *(u16 *)&lbShop.x5A[2] = id;
        *(s16 *)&lbShop.x5A[4] = 0;
        EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C, *(u8 *)&lbShop.x6E);
    }
}
