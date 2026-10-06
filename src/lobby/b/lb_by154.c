/* lb_by154 - agent B 0x00538A70-0x00538ED0 / 0x00538ED0-0x00539220: lb_process_set_weaponList and lb_process_set_armorList (forge shop: build the list of weapons / armor that can be upgraded). */
#include "lobby_s.h"
typedef struct { u8 x0; u8 kind; u16 id; u8 x4[4]; } EQB;
typedef struct { u8 kind; u8 x1; u16 id; u8 x4[0x14]; } SEIENT;
typedef struct { u8 x0; u8 kind; u16 id; u16 x4; } EQREC;
typedef struct { s32 kind; s32 id; } SHENT;
extern SEIENT buki_sei_tbl[];
extern SEIENT bou_sei_tbl[];
extern SHENT shopTbl[];
extern EQREC D_3C7004[];
extern char User_data[];
extern s32 shop_process2_help[];
extern char lb_process_kyoukaListProg[];
int Seisan_ok_ck(int, s16, int);
int Get_equip_bit();
char *Get_equip_name();
u32 Get_equip_price();
int Warehouse_search_space();
int Warehouse_space_ck();
int Now_equip_ck();
char *strcpy(char *, const char *);
void lb_process_set_weaponList(void) {
    LB_SHOPITEM *sl;
    SHENT *t;
    int n;
    int full;
    EQREC *r;
    int cnt;
    u8 *pl;
    EQB q;
    int pages;
    char *name;
    u8 *ud;
    SHENT *t2;
    SEIENT *p;
    int rs;

    sl = shopList;
    t = shopTbl;
    r = D_3C7004;
    cnt = 0;
    pl = (u8 *)player_work + game_w.master * 0xA00;
    full = (Warehouse_search_space(User_data) & 0xFF) == 0xFF;
    memset(shopList, 0, 0x5000);
    if (lbShop.x1A == 0) {
        lbShop.count = 0;
        lbShop.tbl = (s32 *)shopTbl;
        n = 0;
        if (buki_sei_tbl[0].kind != 0xFF) {
            p = buki_sei_tbl;
            do {
                rs = Seisan_ok_ck(1, n, 0);
                q.kind = p->kind;
                q.id = p->id;
                if (rs != 0 && ((1 << pl[0x11]) & (Get_equip_bit(User_data, &q) & 0xFF))) {
                    rs = Seisan_ok_ck(1, n, 1);
                    if (p->id == 0x3E7) {
                        name = (char *)shop_process2_help[7];
                        sl->price = 0x3E8;
                    } else {
                        name = Get_equip_name(p->kind, p->id);
                        sl->price = Get_equip_price(p->kind, p->id) >> 1;
                    }
                    strcpy(sl->name, name);
                    *(s16 *)((u8 *)sl + 0x26) = n;
                    t->kind = p->kind;
                    t->id = p->id;
                    if ((s8)full == 1) {
                        sl->state = 1;
                    } else if ((u32)sl->price > *(u32 *)0x3C6FE0 || rs != 2) {
                        if (Seisan_ok_ck(1, n, 0) == 2 && sl->state == 0) {
                            sl->state = 3;
                        } else {
                            sl->state = 1;
                        }
                    } else {
                        sl->state = 0;
                    }
                    if ((u32)sl->price > *(u32 *)0x3C6FE0 && sl->state == 3) {
                        sl->state = 4;
                    }
                    sl++;
                    cnt++;
                    t++;
                }
                p++;
                n++;
            } while (p->kind != 0xFF);
        }
        lbShop.count = cnt;
        lbShop.x84 = 0;
        pages = cnt / 7;
        lbShop.x18 = 0;
        if (cnt % 7 != 0) {
            pages++;
        }
        lbShop.x6D = pages;
        lbShop.f30 = (void *)lb_process_kyoukaListProg;
        lbShop.list = shopList;
        return;
    }
    t2 = shopTbl;
    lbShop.tbl = (s32 *)t2;
    n = 0;
    ud = (u8 *)User_data;
    do {
        if (Warehouse_space_ck(User_data, n) == 1) {
            sl->state = 2;
        } else if (ud[0x45] == 6 || ud[0x45] == 7) {
            strcpy(sl->name, Get_equip_name(r->kind, r->id));
            sl->price = -1;
            Now_equip_ck(User_data, n);
            sl->state = 0;
        } else {
            strcpy(sl->name, Get_equip_name(r->kind, r->id));
            sl->price = -1;
            sl->state = 1;
        }
        t2->kind = r->kind;
        n++;
        sl++;
        ud += 6;
        t2->id = r->id;
        r++;
        t2++;
    } while (n < 0x40);
    lbShop.count = 0x40;
    lbShop.x6D = 0xA;
    lbShop.x84 = 1;
    lbShop.f30 = (void *)lb_process_kyoukaListProg;
    lbShop.list = shopList;
    lbShop.x18 = 1;
}

s16 lb_process_set_armorList(void) {
    EQB q;
    LB_SHOPITEM *sl;
    SHENT *t;
    u8 *pl;
    int n;
    int cnt;
    int kind;
    int full;
    SEIENT *p;
    int r;
    int pages;

    sl = shopList;
    t = shopTbl;
    cnt = 0;
    pl = (u8 *)player_work + game_w.master * 0xA00;
    switch (lbShop.x1A) {
    case 0:
        kind = 2;
        break;
    case 1:
        kind = 3;
        break;
    case 2:
        kind = 4;
        break;
    case 3:
        kind = 5;
        break;
    case 4:
        kind = 0;
        break;
    }
    lbShop.count = 0;
    lbShop.tbl = (s32 *)shopTbl;
    lbShop.list = shopList;
    n = 0;
    goto init;
loop:
    {
        r = Seisan_ok_ck(0, n, 0);
        q.kind = p->kind;
        q.id = p->id;
        if (r != 0 && ((1 << pl[0x11]) & (Get_equip_bit(User_data, &q) & 0xFF)) && p->kind == kind) {
            r = Seisan_ok_ck(0, n, 1);
            strcpy(sl->name, Get_equip_name(p->kind, p->id));
            sl->price = Get_equip_price(p->kind, p->id) >> 1;
            *(s16 *)((u8 *)sl + 0x26) = n;
            t->kind = p->kind;
            t->id = p->id;
            if (full == 1) {
                sl->state = 1;
            } else if ((u32)sl->price > *(u32 *)0x3C6FE0 || r != 2) {
                if (Seisan_ok_ck(0, n, 0) == 2 && sl->state == 0) {
                    sl->state = 3;
                } else {
                    sl->state = 1;
                }
            } else {
                sl->state = 0;
            }
            if ((u32)sl->price > *(u32 *)0x3C6FE0 && sl->state == 3) {
                sl->state = 4;
            }
            sl++;
            cnt++;
            t++;
        }
    }
    p++;
    n++;
test:
    if (p->kind != 0xFF) goto loop;
    lbShop.count = cnt;
    lbShop.x84 = 0;
    pages = cnt / 7;
    lbShop.x18 = 0;
    if (cnt % 7 != 0) {
        pages++;
    }
    lbShop.x6D = pages;
    lbShop.f30 = (void *)lb_process_kyoukaListProg;
    return kind;
init:
    full = (Warehouse_search_space(User_data) & 0xFF) == 0xFF;
    p = bou_sei_tbl;
    goto test;
}
