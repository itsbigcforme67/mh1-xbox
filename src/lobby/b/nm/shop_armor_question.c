#include "lobby_s.h"
extern s8 armor_shop_r;
extern char User_data[];
void armor_set_myArmor();
void Lb_put_set01();
void lb_armor_tag_decide01();
s32 shop_armor_question(void) {
    s32 id;
    s32 kind;
    s32 key;
    u8 *e;
    u8 m;
    s32 r;

    key = lbShop.key;
    e = (u8 *)lbShop.tbl + lbShop.cur * 8;
    id = *(s32 *)(e + 4);
    kind = *(s32 *)e;
    if (armor_shop_r == 0) {
        key = key & 0xFFFF;
        if (key & 0x20) {
            r = Warehouse_equip_stack(User_data, kind & 0xFF, id & 0xFFFF, 0) & 0xFF;
            if (lbShop.x78 == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(User_data, r);
                armor_shop_r++;
                return 2;
            }
            cnWrap_SoundRequest(3);
            lb_armor_tag_decide01();
            Lb_put_set01(0xC);
            return 3;
        }
        if (key & 0x40) {
            if (lbShop.x78 != 1) {
                cnWrap_SoundRequest(3);
                lbShop.x78 = 1;
                return 2;
            }
            Warehouse_equip_stack(User_data, kind & 0xFF, id & 0xFFFF, 0);
            cnWrap_SoundRequest(3);
            lb_armor_tag_decide01();
            Lb_put_set01(0xC);
            return 3;
        }
        if (key & 0x800) {
            if (lbShop.x78 != 0) {
                lbShop.x78 = 0;
                cnWrap_SoundRequest(1);
            }
        } else if ((key & 0x400) && lbShop.x78 != 1) {
            lbShop.x78 = 1;
            cnWrap_SoundRequest(1);
        }
        return 2;
    }
    if (kind != 7 && kind != 6) {
        armor_set_myArmor(lbShop.key);
    } else if (*(u8 *)0x3C738D != kind) {
        armor_set_myArmor(lbShop.key);
    } else {
        Set_equip_idx(User_data);
    }
    lb_sys.x78 = 1;
    Set_userdata((u8 *)player_work + game_w.master * 0xA00);
    Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
    m = game_w.master;
    memcpy((u8 *)lbCommer + m * 0x5C + 0x1C, cw + m * 0x2FC + 0x1346, 0x40);
    lb_armor_tag_decide01();
    return 0;
}
