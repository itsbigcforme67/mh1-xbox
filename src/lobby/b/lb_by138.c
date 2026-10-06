/* lb_by138 - agent B 0x0053B230-0x0053B580: shop_armor2_question (armor shop, list variant: buy / equip question with yes-no cursor; returns 2 waiting, 3 done, 0 equipped). */
#include "lobby_s.h"
extern s8 armor_shop_r;
extern u8 buki_sei_tbl[];
extern u8 bou_sei_tbl[];
extern char User_data[];
void Lb_put_set01();
void armor_set_myArmor();
s32 shop_armor2_stack();
s32 shop_armor2_question(void) {
    s32 key;
    s32 k;
    s32 r;
    int kind;
    int id;
    u16 *lp;
    int c;
    u8 *e;
    u8 *f;
    u8 m;

    c = lbShop.cur;
    e = (u8 *)lbShop.tbl + c * 8;
    key = lbShop.key;
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            kind = *(u16 *)e;
            id = *(u16 *)(e + 4);
        } else {
            f = buki_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[c * 20] * 0x18;
            kind = f[0];
            id = *(u16 *)(f + 2);
            if (id == 0x3E7) {
                kind = lbShop.x5A[1];
                id = *(u16 *)&lbShop.x5A[2];
            }
        }
    } else {
        f = bou_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[c * 20] * 0x18;
        kind = f[0];
        id = *(u16 *)(f + 2);
    }
    if (armor_shop_r == 0) {
        k = key & 0xFFFF;
        if (k & 0x20) {
            r = shop_armor2_stack(kind & 0xFFFF, id & 0xFFFF) & 0xFF;
            if (lbShop.x78 == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(User_data, r);
                armor_shop_r++;
            } else {
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
            }
        } else if (k & 0x40) {
            if (lbShop.x78 != 1) {
                cnWrap_SoundRequest(3);
                lbShop.x78 = 1;
            } else {
            shop_armor2_stack(kind & 0xFFFF, id & 0xFFFF);
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
            }
        } else if (k & 0x800) {
            if (lbShop.x78 != 0) {
                lbShop.x78 = 0;
                cnWrap_SoundRequest(1);
            }
        } else if ((k & 0x400) && lbShop.x78 != 1) {
            lbShop.x78 = 1;
            cnWrap_SoundRequest(1);
        }
    } else {
    if ((u16)kind != 7 && (u16)kind != 6) {
        armor_set_myArmor(kind, id);
    } else if (*(u8 *)0x3C738D != (u16)kind) {
        armor_set_myArmor(kind, id);
    } else {
        Set_equip_idx(User_data, id);
    }
    lb_sys.x78 = 1;
    Set_userdata((u8 *)player_work + game_w.master * 0xA00);
    Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
    m = game_w.master;
    memcpy((u8 *)lbCommer + m * 0x5C + 0x1C, cw + m * 0x2FC + 0x1346, 0x40);
    return 0;
    }
    return 2;
}
