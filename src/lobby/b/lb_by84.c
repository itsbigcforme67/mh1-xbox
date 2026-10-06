/* lb_by84 - agent B promoted near-match 0x0053CD50-0x0053CF5C: lb_armor_decide (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern s8 armor_shop_r;
extern char User_data[];
extern void shop_armor_put_shopHelp();
extern int shop_armor_question();
extern char shop_armor_help[];
typedef struct { u8 x0; u8 id; u16 qty; u16 x4; } EQK;

void lb_armor_decide(void) {
    EQK q;
    int new_var;
    s32 *e;
    int sp38;
    s32 new_var2;

    lbShop.x1C = 0;
    switch (lbShop.mode) {
    case 0:
        new_var2 = lbShop.cur;
        lbShop.x78 = 0;
        e = (s32 *)(new_var2 * 8 + (int)lbShop.tbl);
        q.id = e[0];
        q.qty = e[1];
        if (Equip_ok_ck(&User_data, &q) == 1) {
            lbShop.x8F = 1;
            armor_shop_r = 0;
            lbShop.f40 = shop_armor_put_shopHelp;
            lbShop.f38 = shop_armor_question;
            lbShop.help = ((s32 *)&shop_armor_help)[2];
            if (q.id == 6 || q.id == 7) {
                new_var = *(u8 *)0x3C738D;
                if (q.id != new_var) {
                    lbShop.help = ((s32 *)&shop_armor_help)[4];
                }
            }
        } else {
            Warehouse_equip_stack(&User_data, q.id, q.qty, 0);
            lbShop.f40 = 0;
            armor_shop_r = 0;
            lbShop.x8F = 0;
            lbShop.f38 = 0;
            lbShop.help = ((s32 *)&shop_armor_help)[3];
            Lb_put_set01(0xC);
        }
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur);
        cnWrap_SoundRequest(8);
        break;
    case 1:
        Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur);
        Warehouse_equip_erase(&User_data, (u16)lbShop.cur);
        cnWrap_SoundRequest(8);
        lb_armor_tag_decide00();
        break;
    }
}
