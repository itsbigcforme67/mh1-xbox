/* lb_by128 - agent B 0x0053C7F0-0x0053CA14: lb_armor_tag_decide01 (armor shop: builds the buy list for the hunter's job from the shop table). */
#include "lobby_s.h"
unsigned char Warehouse_search_space();   /* its real return type: was implicitly int (tools/check_kr_returns.py) */
typedef struct { u8 x0; s8 kind; s16 id; u8 x4[4]; } EQB;
typedef struct { s32 kind; s32 id; } SHENT;
extern s32 shop_armor_help[];
extern SHENT *armor_shop_tbl[];
extern char User_data[];
#ifdef VERSION_MHG
/* G (lobby 0x54ED50): the shop table is chosen by the story (get_questLevelNum 34/22/12/10/6/4 online, 50/46/43 offline) and the
 * hunter's job (blademaster = job 1 gets the odd table); main's armor_shop_tbl holds pairs of pointers (shop 0/1) per group.
 * The list holds 523 entries (memset 20920) and a full item box shows message 10. */
extern SHENT *armor_shop_tbl[];
extern int get_questLevelNum();
extern void Lb_put_set01();
static int armor_tbl_group(int job1) {
    if (Online_ck() == 1) {
        if (get_questLevelNum(34)) return job1 ? 0x88 : 0x60;
        if (get_questLevelNum(22)) return job1 ? 0x80 : 0x58;
        if (get_questLevelNum(12)) return job1 ? 0x78 : 0x50;
        if (get_questLevelNum(10)) return job1 ? 0x70 : 0x48;
        if (get_questLevelNum(6)) return job1 ? 0x68 : 0x40;
        return get_questLevelNum(4) ? 0x38 : 0x30;
    }
    if (get_questLevelNum(50)) return job1 ? 0x28 : 0x18;
    if (get_questLevelNum(46)) return job1 ? 0x20 : 0x10;
    return get_questLevelNum(43) ? 0x08 : 0x00;
}
void lb_armor_tag_decide01(void) {
    EQB q;
    LB_SHOPITEM *sl;
    u8 *pl;
    SHENT *e;
    int i, cnt, full, pages;

    sl = shopList;
    cnt = 0;
    pl = (u8 *)player_work + game_w.master * 0xA00;
    lbShop.help = shop_armor_help[lbShop.x1A];
    memset(shopList, 0, 20920);
    lbShop.tbl = (s32 *)((SHENT **)((u8 *)armor_shop_tbl + armor_tbl_group(pl[0x11] == 1)))[lbShop.x1A];
    e = (SHENT *)lbShop.tbl;
    full = (Warehouse_search_space(User_data) & 0xFFFF) == 0xFFFF;
    if (full) Lb_put_set01(10);
    for (i = 0; i < 100; i++, e++) {
        if (e->kind == 0xFFFF || e->id == 0xFFFF) break;
        q.kind = e->kind;
        q.id = e->id;
        if ((1 << pl[0x11]) & (Get_equip_bit(User_data, &q) & 0xFF)) {
            strcpy(sl->name, (char *)Get_equip_name((u8)e->kind, (u16)e->id));
            sl->price = Get_equip_price((u8)e->kind, (u16)e->id);
            sl->state = ((u32)sl->price > *(u32 *)(User_data + 0x20) || full == 1) ? 1 : 0;
            cnt++;
            sl++;
        }
    }
    pages = cnt / 7;
    lbShop.count = cnt;
    if (cnt % 7 != 0) pages++;
    lbShop.x6D = pages;
    lbShop.x84 = 0;
}
#else
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
#endif
