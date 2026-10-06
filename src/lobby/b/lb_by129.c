/* lb_by129 - agent B 0x0053C610-0x0053C7EC: lb_armor_tag_decide00 (armor shop: sell list from the hunter's equipment / first menu tag). */
#include "lobby_s.h"
typedef struct { u8 x0; u8 kind; u16 id; u16 x4; } EQREC;
typedef struct { s32 kind; s32 id; } SHTBL;
extern EQREC D_3C7004[];
extern SHTBL shopTbl[];
extern char User_data[];
extern char shop_armor_question[];
extern char shop_armor01_tag[8];
extern char shop_armor_help[];
extern char lb_armor_tag_decide01[];
void lb_armor_tag_decide00(void) {
    int i;
    EQREC *r;
    LB_SHOPITEM *sl;
    SHTBL *t;

    r = D_3C7004;
    sl = shopList;
    if (lbShop.mode == 0) {
        lbShop.x18 = 0;
        lbShop.x16 = 1;
        lbShop.x8F = 1;
        lbShop.f38 = (void *)shop_armor_question;
        lbShop.tag = (s32 *)shop_armor01_tag;
        lbShop.f28 = (void *)lb_armor_tag_decide01;
        return;
    }
    lbShop.x16 = 0;
    lbShop.x8F = 0;
    lbShop.tbl = (s32 *)shopTbl;
    lbShop.f38 = 0;
    lbShop.f28 = 0;
    lbShop.x18 = 0;
    lbShop.help = *(s32 *)(shop_armor_help + 0x18);
    memset(shopList, 0, 0x5000);
    i = 0;
    t = shopTbl;
    do {
        if (Warehouse_space_ck(User_data, i) == 1) {
            sl->state = 2;
            t->kind = 0;
            t->id = 0;
        } else {
            strcpy(sl->name, Get_equip_name(r->kind, r->id));
            sl->price = Get_equip_kaitori(r->kind, r->id);
            if (Now_equip_ck(User_data, i) == 1) {
                sl->state = 1;
            } else {
                sl->state = 0;
            }
            t->kind = r->kind;
            t->id = r->id;
        }
        i++;
        r++;
        sl++;
        t++;
    } while (i < 0x40);
    lbShop.count = 0x40;
    lbShop.x6D = 0xA;
    lbShop.x84 = 1;
}
