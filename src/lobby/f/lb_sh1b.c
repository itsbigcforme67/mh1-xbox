/* lb_sh1b - one translation unit 0x00539E40-0x0053AB50 (lbtu3). */
#include "lobby_s.h"
extern char shop_process2_help[];
extern s32 armorIndex;
extern LB_SHOPITEM shopList2[];
extern u8 D_3C7005[];
extern s32 shop_process2_help_c1[];
extern char User_data[];
int Gun_option_ck();
void lb_process_tag_decide01();
/* original bytes: build/raw/lb_process_kyoukaListProg.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
#else
#endif
extern u8 buki_sei_tbl[];
extern u8 kakou_tbl[];
extern u8 bou_sei_tbl[];
/* original bytes: build/raw/lb_process_use_item.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
#else
#endif
extern s8 randTblNo;
extern u8 randTbl00[];
extern u8 randTbl01[];
extern u8 randTbl02[];
extern u8 randTbl03[];
extern u8 randTbl04[];
extern s8 armor_shop_r;
extern void shop_armor_put_shopHelp();
extern int shop_process_after();
extern s16 armorIndex_c4;
s32 lb_armor2_sel2Prog();
asm s32 lb_process_kyoukaListProg();
s32 lb_process_kyoukaListProg();
asm void lb_process_use_item(int n);
void lb_process_use_item(int n);
void random_stack();
void lb_process_decide();
s32 item_to_stack(s32 arg0, s32 arg1);
int lb_process_use_item_k();
s32 lb_armor2_sel2Prog() {
    if (lbShop.key & 0x20) {
        if (lbShop.x78 == 0) {
            cnWrap_SoundRequest(0);
            lbShop.f38 = 0;
            lbShop.help = F(s32, &shop_process2_help, 0x10);
            return 0;
        }
        cnWrap_SoundRequest(3);
        lbShop.help = F(s32, &shop_process2_help, 8);
        return 3;
    }
    if (lbShop.key & 0x40) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(3);
            lbShop.x78 = 1;
            goto block_16;
        }
        cnWrap_SoundRequest(3);
        lbShop.help = F(s32, &shop_process2_help, 8);
        return 3;
    }
    if (lbShop.key & 0x400) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(1);
            lbShop.x78 = 1;
        }
    } else if ((lbShop.key & 0x800) && (lbShop.x78 != 0)) {
        cnWrap_SoundRequest(1);
        lbShop.x78 = 0;
    }
block_16:
    return 2;
}

#ifdef __MWERKS__
asm s32 lb_process_kyoukaListProg()
{
#include "lb_process_kyoukaListProg.inc"
}
#endif

#ifdef __MWERKS__
asm void lb_process_use_item(int n)
{
#include "lb_process_use_item.inc"
}
#endif

void random_stack() {
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

void lb_process_decide() {
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
    lb_process_use_item_k(lbShop.cur);
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

s32 item_to_stack(s32 arg0, s32 arg1) {
    s32 var_s0;
    u16 temp_v0;
    u16 *lp;
    int c;
    if (arg1 != 0x3E7) {
        if ((lbShop.x1A == 1) && (arg0 == 7)) {
            lp = (u16 *)lbShop.list;
            c = lbShop.cur;
            temp_v0 = lp[c * 20 + 0x13];
            switch (temp_v0) {
            case 0:
                Gun_level_up(&User_data, armorIndex_c4, 1);
                break;
            case 1:
                Gun_Silencer_set(&User_data, armorIndex_c4, 0);
                break;
            case 2:
                Gun_Silencer_set(&User_data, armorIndex_c4, 1);
                break;
            case 3:
                Gun_barrel_set(&User_data, armorIndex_c4, 0);
                break;
            case 4:
                Gun_barrel_set(&User_data, armorIndex_c4, 1);
                break;
            case 5:
                Gun_Scope_set(&User_data, armorIndex_c4, 0);
                break;
            case 6:
                Gun_Scope_set(&User_data, armorIndex_c4, 1);
                break;
            }
        } else {
            var_s0 = Warehouse_equip_stack(&User_data, arg0 & 0xFF, arg1 & 0xFFFF, 0) & 0xFF;
        }
    } else {
        var_s0 = Warehouse_equip_stack(&User_data, F(u8, &lbShop, 0x5B), F(u16, &lbShop, 0x5C), 0) & 0xFF;
    }
    return var_s0;
}

