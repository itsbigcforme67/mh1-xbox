/* lb_by128 - agent B 0x0053C7F0-0x0053CA14: lb_armor_tag_decide01 (armor shop: builds the buy list for the hunter's job from the shop table). */
#include "lobby_s.h"
typedef struct { u8 x0; s8 kind; s16 id; u8 x4[4]; } EQB;
typedef struct { s32 kind; s32 id; } SHENT;
extern s32 shop_armor_help[];
extern SHENT *armor_shop_tbl[];
extern char User_data[];
void lb_armor_tag_decide01(void) {
    EQB q;
    LB_SHOPITEM *sl;
    u8 *pl;
    SHENT *e;
    int i;
    int cnt;
    int full;
    int pages;

    sl = shopList;
    cnt = 0;
    pl = (u8 *)player_work + game_w.master * 0xA00;
    lbShop.help = shop_armor_help[lbShop.x1A];
    memset(shopList, 0, 0x5000);
    lbShop.tbl = (s32 *)armor_shop_tbl[lbShop.x1A];
    e = (SHENT *)lbShop.tbl;
    full = (Warehouse_search_space(User_data) & 0xFF) == 0xFF;
    i = 0;
    do {
        if (e->kind == 0xFFFF || e->id == 0xFFFF) break;
        q.kind = e->kind;
        q.id = e->id;
        if ((1 << pl[0x11]) & (Get_equip_bit(User_data, &q) & 0xFF)) {
            strcpy(sl->name, Get_equip_name((u8)e->kind, (u16)e->id));
            sl->price = Get_equip_price((u8)e->kind, (u16)e->id);
            if ((u32)sl->price > *(u32 *)0x3C6FE0 || full == 1) {
                sl->state = 1;
            } else {
                sl->state = 0;
            }
            cnt++;
            sl++;
        }
        i++;
        e++;
    } while (i < 0x64);
    pages = cnt / 7;
    lbShop.count = cnt;
    if (cnt % 7 != 0) {
        pages++;
    }
    lbShop.x6D = pages;
    lbShop.x84 = 0;
}
