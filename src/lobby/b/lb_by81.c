/* lb_by81 - agent B promoted near-match 0x0053A710-0x0053A9A4: random_stack, lb_process_decide (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern s8 randTblNo;
extern u8 randTbl00[];
extern u8 randTbl01[];
extern u8 randTbl02[];
extern u8 randTbl03[];
extern u8 randTbl04[];
extern s8 armor_shop_r;
extern LB_SHOPITEM shopList2[];
extern void shop_armor_put_shopHelp();
extern int shop_process_after();
extern char User_data[];

void random_stack(void) {
    u8 *tbl;
    int id;
    s32 qty;
    int r;

    switch (randTblNo) {
    case 0:
        tbl = randTbl00;
        id = 6;
        break;
    case 1:
        tbl = randTbl01;
        id = 6;
        break;
    case 2:
        tbl = randTbl02;
        id = 6;
        break;
    case 3:
        tbl = randTbl03;
        id = 6;
        break;
    case 4:
        tbl = randTbl04;
        id = 7;
        break;
    }
    r = (u16)ran_suu(1) % 100;
    if (r < 5) {
        qty = *(s32 *)(tbl + 8);
    } else if (r < 0x1E) {
        qty = *(s32 *)(tbl + 4);
    } else {
        qty = *(s32 *)(tbl + 0);
    }
    lbShop.x5A[1] = id;
    *(s16 *)&lbShop.x5A[2] = qty;
    lbShop.tbl[lbShop.cur * 2] = id;
    lbShop.tbl[lbShop.cur * 2 + 1] = qty;
}

void lb_process_decide(void) {
    u8 buf[6];
    s32 id;
    s32 qty;
    s32 *e;

    e = (s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8);
    id = e[0];
    qty = e[1];
    if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
        Gold_add(-shopList2[lbShop.cur].price, lbShop.cur);
    } else {
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur);
    }
    lb_process_use_item(lbShop.cur);
    lbShop.f40 = shop_armor_put_shopHelp;
    cnWrap_SoundRequest(8);
    lbShop.x8F = 1;
    lbShop.f38 = shop_process_after;
    armor_shop_r = 0;
    if ((id != 7) && (id != 6)) {
        buf[1] = id;
        *(s16 *)&buf[2] = qty;
        if (Equip_ok_ck(&User_data, buf) == 0) {
            lbShop.f40 = 0;
        }
    }
}
