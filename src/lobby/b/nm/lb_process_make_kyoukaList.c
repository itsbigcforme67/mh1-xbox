#include "lobby_s.h"
typedef struct { s32 kind; s32 id; } SHTBL;
typedef struct { u8 x0; u8 kind; u16 id; u16 x4; } EQREC;
extern s32 armorIndex;
extern EQREC D_3C7004[];
extern SHTBL shopTbl[];
extern u8 kakou_tbl[];
extern char *gun_kyouka_tbl[];
extern s32 lvup_price[];
extern char User_data[];
extern LB_SHOPITEM shopList2[];
char *Get_equip_name();
u32 Get_equip_price();
int check_items();
int Get_Gun_level(char *, s16);
int Gun_option_ck(char *, s16, int);
void lb_process_make_kyoukaList(void) {
    int i;
    int j;
    u16 *kk;
    LB_SHOPITEM *sl;
    SHTBL *t;
    s32 *p;
    u32 price;
    int lvl;
    int ai;
    int id;

    sl = shopList2;
    ai = armorIndex;
    id = D_3C7004[ai].id;
    memset(shopList2, 0, 0x5000);
    lbShop.count = 0;
    lbShop.list = shopList2;
    lbShop.x6D = 1;
    lbShop.x6C = 0;
    t = shopTbl;
    lbShop.tbl = (s32 *)shopTbl;
    kk = (u16 *)(kakou_tbl + id * 0x18);
    if (D_3C7004[ai].kind == 6) {
        i = 0;
        do {
            sl->state = 0;
            if (kk[6] != 0) {
                strcpy(sl->name, Get_equip_name(6, kk[6]));
                sl->price = Get_equip_price(6, kk[6]) >> 1;
                sl->state = 0;
                *(u16 *)&sl->_pad26 = kk[6];
                p = (s32 *)(kakou_tbl + kk[6] * 0x18);
                j = 0;
                do {
                    if (*(u16 *)p != 0 && check_items(p, 1) != 1) {
                        if (check_items(p, 0) == 1 && sl->state != 1) {
                            sl->state = 3;
                        } else {
                            sl->state = 1;
                        }
                    }
                    j++;
                    p++;
                } while (j < 3);
                if ((u32)sl->price > *(u32 *)0x3C6FE0) {
                    if (sl->state == 3) {
                        sl->state = 4;
                    } else {
                        sl->state = 1;
                    }
                }
                t->kind = 6;
                t->id = kk[6];
            } else {
                sl->state = 2;
            }
            i++;
            sl++;
            kk++;
            t++;
            lbShop.count = lbShop.count + 1;
        } while (i < 5);
        return;
    }
    shopTbl[0].id = 0;
    shopTbl[1].id = 0;
    shopTbl[0].kind = 7;
    shopTbl[1].kind = 7;
    shopTbl[2].kind = 7;
    shopTbl[2].id = 0;
    shopTbl[3].kind = 7;
    shopTbl[3].id = 0;
    price = Get_equip_price(7, D_3C7004[ai].id);
    lvl = Get_Gun_level(User_data, ai);
    if (lvl >= 4) {
        lbShop.count = 3;
    } else {
        lbShop.count = 4;
        sl->price = price / 10 * lvup_price[lvl];
        if ((u32)sl->price > *(u32 *)0x3C6FE0) {
            sl->state = 1;
        } else {
            sl->state = 0;
        }
        *(u16 *)&sl->_pad26 = 0;
        sprintf(sl->name, gun_kyouka_tbl[0]);
        sl++;
    }
    if (Gun_option_ck(User_data, ai, 0x10) == 1) {
        sl->price = 10;
        *(u16 *)&sl->_pad26 = 2;
        sprintf(sl->name, gun_kyouka_tbl[2]);
    } else {
        sl->price = price / 10 * 4;
        *(u16 *)&sl->_pad26 = 1;
        sprintf(sl->name, gun_kyouka_tbl[1]);
    }
    if ((u32)sl->price > *(u32 *)0x3C6FE0) {
        sl->state = 1;
    } else {
        sl->state = 0;
    }
    if (Gun_option_ck(User_data, ai, 0x20) == 1) {
        sl[1].price = 10;
        *(u16 *)&sl[1]._pad26 = 4;
        sprintf(sl[1].name, gun_kyouka_tbl[4]);
    } else {
        sl[1].price = price / 10 * 3;
        *(u16 *)&sl[1]._pad26 = 3;
        sprintf(sl[1].name, gun_kyouka_tbl[3]);
    }
    if ((u32)sl[1].price > *(u32 *)0x3C6FE0) {
        sl[1].state = 1;
    } else {
        sl[1].state = 0;
    }
    sl++;
    sl++;
    if (Gun_option_ck(User_data, ai, 0x40) == 1) {
        sl->price = 10;
        *(u16 *)&sl->_pad26 = 6;
        sprintf(sl->name, gun_kyouka_tbl[6]);
    } else {
        sl->price = price / 10 * 3;
        *(u16 *)&sl->_pad26 = 5;
        sprintf(sl->name, gun_kyouka_tbl[5]);
    }
    if ((u32)sl->price > *(u32 *)0x3C6FE0) {
        sl->state = 1;
    } else {
        sl->state = 0;
    }
}
