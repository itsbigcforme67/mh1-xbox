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
#ifdef VERSION_MHG
    t = shopTbl;
#endif
    if (lbShop.mode == 0) {
        lbShop.x18 = 0;
        lbShop.x16 = 1;
        lbShop.x8F = 1;
        lbShop.f38 = (void *)shop_armor_question;
        lbShop.tag = ((s32 *)shop_armor01_tag);
        lbShop.f28 = (void *)lb_armor_tag_decide01;
        return;
    }
#ifdef VERSION_MHG
    /* G (lobby 0x54EB10): the stored equipment holds 100/200/300 entries (UD_WARE_N in ud.h), the list shows all of them; the page count
     * is the number of the item box's expansions + 1 (sub_main 0x4C9220); more fields are reset than in MH1 */
    {
        int n = (1 + (((u8 *)User_data)[0xC97] & 1) + ((((u8 *)User_data)[0xC97] >> 1) & 1)) * 100;   /* UD_WARE_N (include/ud.h) */
        lbShop.x16 = 0;
        lbShop.x8F = 0;
        lbShop.tbl = (s32 *)shopTbl;
        memset(shopList, 0, 20920);
        lbShop.f38 = 0;
        lbShop.f28 = 0;
        lbShop.x18 = 0;
        lbShop.xG7C = 0;
        lbShop.x70 = 0;
        lbShop.x6C = 0;
        lbShop.help = *(s32 *)(shop_armor_help + 0x18);
        for (i = 0; i < n; i++, sl++, t++) {
            u8 *w = (u8 *)User_data + UDMO(0x44) + i * 6;
            if (Warehouse_space_ck(User_data, i) == 1) {
                sl->state = 2;
                t->kind = 0;
                t->id = 0;
            } else {
                strcpy(sl->name, (char *)Get_equip_name(w[1], *(u16 *)(w + 2)));
                sl->price = Get_equip_kaitori(w[1], *(u16 *)(w + 2));
                sl->state = Now_equip_ck(User_data, i) == 1 ? 1 : 0;
                t->kind = w[1];
                t->id = *(u16 *)(w + 2);
            }
        }
        lbShop.count = n;
        lbShop.x6D = 1 + (((u8 *)User_data)[0xC97] & 1) + ((((u8 *)User_data)[0xC97] >> 1) & 1);
        lbShop.x84 = 1;
        return;
    }
#endif
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
