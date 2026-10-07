/* lb_arm - one translation unit 0x0053C520-0x0053D7CC (lbtu3). */
#include "lobby_s.h"
extern char User_data[];
extern char my_user_id[];
typedef struct { u8 x0; u8 kind; u16 id; u16 x4; } EQREC;
typedef struct { s32 kind; s32 id; } SHTBL;
extern EQREC D_3C7004[];
extern SHTBL shopTbl[];
extern char shop_armor_question_o[];
extern char shop_armor01_tag[8];
extern char shop_armor_help[];
extern char lb_armor_tag_decide01_o[];
typedef struct { u8 x0; s8 kind; s16 id; u8 x4[4]; } EQB;
typedef struct { s32 kind; s32 id; } SHENT;
extern s32 shop_armor_help_c2[];
extern SHENT *armor_shop_tbl[];
extern char shop_default_help[];
extern void Lb_put_shopYesNo();
extern s8 armor_shop_r;
typedef struct { u8 x0; u8 id; u16 qty; u16 x4; } EQK;
extern u8 D_3C7004_c10[];
extern char lit_543_00655878[];
extern char *shop_warning[];
extern char *my_job_str[2];
extern char lit_551_00655880[];
extern char lit_585_00655888[];
extern char lb_shop_msg[];
void Lb_put_armorIcon(int x, int y, int z, s16 kind, s16 id);
void armor_set_myArmor();
void lb_armor_tag_decide00();
void lb_armor_tag_decide01();
s32 lb_armor_select();
s32 lb_armor_itemBuy();
s32 lb_armor_itemSell();
s32 lb_armor_sel2Prog();
void lb_armor_decide();
s32 shop_armor_question();
void shop_armor_put_shopHelp();
s32 Lb_get_armor_num(s32 a, s32 b);
void Lb_put_job_limit(u16 kind, s16 id);
void Lb_put_my_job();
void lb_armor_put_itemDetail();
void lb_armor_listItem(int arg0, int arg1, int arg2, s16 arg3);
void armor_shop_trans();
int Lb_get_armor_num_k();
int Lb_put_job_limit_k();
void armor_set_myArmor() {
    u8 *pl;

    pl = (u8 *)&player_work[game_w.master];
    Set_equip_idx(&User_data);
    Lb_player_release(pl);
    flCompact();
    lb_sys.x8D = 3;
    Set_userdata(pl);
    Lb_set_player(game_w.master, &my_user_id, &my_user_handle);
    Lb_player_load(pl);
    lb_sys.x78 = 1;
    Lb_set_mini_data((u8 *)&lbCommer[*(u16 *)(pl + 0xC)] + 0x1C);
    Lb_set_mini_data((u8 *)cw + *(u16 *)(pl + 0xC) * 0x2FC + 0x1346);
}

void lb_armor_tag_decide00() {
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
        lbShop.f38 = (void *)shop_armor_question_o;
        lbShop.tag = (s32 *)shop_armor01_tag;
        lbShop.f28 = (void *)lb_armor_tag_decide01_o;
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

void lb_armor_tag_decide01() {
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
    lbShop.help = shop_armor_help_c2[lbShop.x1A];
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

s32 lb_armor_select() {
    switch (F(s8, &lbShop, 0x19)) {       /* irregular */
    case 0:
        F(s32, &lbShop, 0x4C) = F(s32, &shop_default_help, 8);
        return lb_armor_itemBuy();
    case 1:
        F(s32, &lbShop, 0x4C) = F(s32, &shop_default_help, 0xC);
        return lb_armor_itemSell();
    default:
        return 0;
    }
}

s32 lb_armor_itemBuy() {
    s32 id;
    s32 qty;
    s32 *e;

    if ((Warehouse_search_space(&User_data) & 0xFF) == 0xFF) {
        return 0;
    }
    lbShop.x6E = 0;
    lbShop.x84 = 2;
    id = lbShop.tbl[lbShop.cur * 2];
    qty = lbShop.tbl[lbShop.cur * 2 + 1];
    Lb_make_mySrcEquip((s16)id);
    *(s16 *)&lbShop.x5A[2] = qty;
    lbShop.x5A[0] = 1;
    lbShop.x5A[1] = id;
    *(s16 *)&lbShop.x5A[4] = 0;
    lbShop.f30 = 0;
    lbShop.x18 = 0;
    if ((id != 7) && (id != 6) && (Equip_ok_ck(&User_data, lbShop.x5A) == 0)) {
        lbShop.f30 = lb_armor_sel2Prog;
        lbShop.x18 = 1;
        lbShop.f40 = Lb_put_shopYesNo;
        lbShop.help = ((s32 *)&shop_armor_help)[5];
    }
    cnWrap_SoundRequest(0);
    return 1;
}

s32 lb_armor_itemSell() {
    Lb_make_mySrcEquip(*(s16 *)((char *)lbShop.tbl + lbShop.cur * 8));
    cnWrap_SoundRequest(0);
    return 1;
}

s32 lb_armor_sel2Prog() {
    if (lbShop.key & 0x20) {
        if (lbShop.x78 == 0) {
            cnWrap_SoundRequest(0);
            lbShop.help = F(s32, &shop_default_help, 8);
            return 0;
        }
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (lbShop.key & 0x40) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(3);
            lbShop.x78 = 1;
            goto block_16;
        }
        cnWrap_SoundRequest(3);
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

void lb_armor_decide() {
    EQK q;
    int new_var;
    s32 *e;
    int sp38;
    s32 new_var2;

    lbShop.x1C = 0;
    switch (lbShop.mode) {
    case 0:
        new_var2 = lbShop.cur;
        lbShop.x78 = 0;
        e = (s32 *)(new_var2 * 8 + (int)lbShop.tbl);
        q.id = e[0];
        q.qty = e[1];
        if (Equip_ok_ck(&User_data, &q) == 1) {
            lbShop.x8F = 1;
            armor_shop_r = 0;
            lbShop.f40 = shop_armor_put_shopHelp;
            lbShop.f38 = shop_armor_question;
            lbShop.help = ((s32 *)&shop_armor_help)[2];
            if (q.id == 6 || q.id == 7) {
                new_var = *(u8 *)0x3C738D;
                if (q.id != new_var) {
                    lbShop.help = ((s32 *)&shop_armor_help)[4];
                }
            }
        } else {
            Warehouse_equip_stack(&User_data, q.id, q.qty, 0);
            lbShop.f40 = 0;
            armor_shop_r = 0;
            lbShop.x8F = 0;
            lbShop.f38 = 0;
            lbShop.help = ((s32 *)&shop_armor_help)[3];
            Lb_put_set01(0xC);
        }
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur);
        cnWrap_SoundRequest(8);
        break;
    case 1:
        Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur);
        Warehouse_equip_erase(&User_data, (u16)lbShop.cur);
        cnWrap_SoundRequest(8);
        lb_armor_tag_decide00();
        break;
    }
}

s32 shop_armor_question() {
    s32 id;
    s32 kind;
    int key;
    s32 k;
    s32 c;
    u8 m;
    s32 r;

    key = lbShop.key;
    c = lbShop.cur;
    kind = lbShop.tbl[c * 2];
    id = lbShop.tbl[c * 2 + 1];
    if (armor_shop_r == 0) {
        k = key & 0xFFFF;
        if (k & 0x20) {
            r = Warehouse_equip_stack(User_data, kind & 0xFF, id & 0xFFFF, 0) & 0xFF;
            if (lbShop.x78 == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(User_data, r);
                armor_shop_r++;
            } else {
                cnWrap_SoundRequest(3);
                lb_armor_tag_decide01();
                Lb_put_set01(0xC);
                return 3;
            }
        } else if (k & 0x40) {
            if (lbShop.x78 != 1) {
                cnWrap_SoundRequest(3);
                lbShop.x78 = 1;
            } else {
                Warehouse_equip_stack(User_data, kind & 0xFF, id & 0xFFFF, 0);
                cnWrap_SoundRequest(3);
                lb_armor_tag_decide01();
                Lb_put_set01(0xC);
                return 3;
            }
        } else if (k & 0x800) {
            if (lbShop.x78 != 0) {
                lbShop.x78 = 0;
                cnWrap_SoundRequest(1);
            }
        } else if (k & 0x400) {
            if (lbShop.x78 != 1) {
                lbShop.x78 = 1;
                cnWrap_SoundRequest(1);
            }
        }
    } else {
    if (kind != 7 && kind != 6) {
        armor_set_myArmor(key);
    } else if (*(u8 *)0x3C738D != kind) {
        armor_set_myArmor(key);
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
    return 2;
}

void shop_armor_put_shopHelp() {
    Lb_put_shopYesNo();
}

/* number of stocked armor pieces (6-byte records at D_3C7004_c10) with the given kind (u8 at +1) and id (u16 at +2) */

s32 Lb_get_armor_num(s32 a, s32 b) {
    int cnt;
    int i;
    u8 *p;

    cnt = 0;
    p = D_3C7004_c10;
    for (i = 0; i < 0x40; i++) {
        if (p[1] == (a & 0xFFFF) && *(u16 *)(p + 2) == (b & 0xFFFF)) {
            cnt++;
        }
        p += 6;
    }
    return cnt;
}

void Lb_put_job_limit(u16 kind, s16 id) {
    EQB q;
    u8 b;
    u8 v;

    if (kind != 7 && kind != 6) {
        q.kind = kind;
        q.id = id;
        b = Get_equip_bit(User_data, &q);
        v = 0xFF;
        if ((b & 3) != 3) {
            if (b & 1) {
                switch (b & 0xC) {
                case 4:
                    v = 7;
                    break;
                case 8:
                    v = 5;
                    break;
                default:
                    v = 3;
                    break;
                }
            } else {
                switch (b & 0xC) {
                case 4:
                    v = 6;
                    break;
                case 8:
                    v = 4;
                    break;
                default:
                    v = 2;
                    break;
                }
            }
        } else if ((b & 0xC) != 0xC) {
            if (b & 4) {
                v = 1;
            } else {
                v = 0;
            }
        }
        if (v != 0xFF) {
            font_set_palette(5);
            flfntLocate(0x1D0, 0x11B);
            font_print(lit_543_00655878, shop_warning[v]);
        }
    }
}

void Lb_put_my_job() {
    font_set_palette(5);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x124, 0x3C);
    if (*(u8 *)0x3C738D == 7) {
        font_print(lit_551_00655880, my_job_str[0]);
    } else {
        font_print(lit_551_00655880, my_job_str[1]);
    }
}

void lb_armor_put_itemDetail() {
    int name;
    int kind;
    int id;
    u8 *ud;
    u8 *e;
    int c;

    c = lbShop.cur;
    e = (u8 *)lbShop.tbl + c * 8;
    ud = (u8 *)User_data + c * 12 + 0x44;
    if (lbShop.mode == 0) {
        kind = *(u16 *)e;
        id = *(u16 *)(e + 4);
    } else {
        kind = ud[1];
        id = *(u16 *)(ud + 2);
    }
    if (lbShop.x1C == 0) {
        Lb_draw_square(0x11F, 0xFC, 0x141, 2, 0xFF602020, 1);
        if ((kind & 0xFFFF) == 7 || (kind & 0xFFFF) == 6) {
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)kind, (s16)id);
            Lb_put_itemRare(0x12A, 0x136, (s8)Get_equip_rare((u8)kind, id));
        } else {
            reload_tex(1, 0x118);
            SetTextureStage(0x118);
            Lb_put_armorIcon(0x122, 0x102, 0x36, (s16)kind, (s16)id);
            Lb_put_itemRare(0x12A, 0x136, (s8)Get_equip_rare(kind & 0xFF, id));
            reload_tex(1, 0x157);
            SetTextureStage(0x157);
        }
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x168, 0x104);
        name = Get_equip_name((u8)kind, id);
        font_set_palette(Equip_moji_color_rare(Get_equip_rare(kind & 0xFF, id)));
        font_print(lit_551_00655880, name);
        font_set_palette(0);
        Lb_put_msg_type2(lb_shop_msg + 0x18);
        flfntSetSize(0x1C, 0x14);
        font_print_ex(0x1B0, 0x11A, 0, lit_585_00655888, Lb_get_armor_num_k(kind, id));
        flfntSetSize(0x12, 0x12);
        if (lbShop.x15 == 5) {
            Lb_put_button(0x212, 0x12F, 3);
            Lb_put_msg_type2(lb_shop_msg + 0x20);
        }
        Lb_put_job_limit_k(kind, id);
        Lb_put_my_job();
    } else {
        Lb_make_mySrcEquip((s16)kind);
        lbShop.x5A[1] = kind;
        lbShop.x5A[0] = 1;
        *(u16 *)&lbShop.x5A[2] = id;
        *(s16 *)&lbShop.x5A[4] = 0;
        EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C, *(u8 *)&lbShop.x6E);
    }
}

void lb_armor_listItem(int arg0, int arg1, int arg2, s16 arg3) {
    int p;

    p = (int)lbShop.tbl + (arg3 + lbShop.x6C * 7) * 8;
    Lb_put_armorIcon(arg0, arg1, arg2, *(s16 *)p, *(s16 *)(p + 4));
}

void armor_shop_trans() {
    u8 *e;

    e = lb_pit.pos + lb_pit.x08 * 8;
    switch (lbShop.step) {
    case 0:
    case 2:
        break;
    default:
            *(s8 *)((u8 *)&lb_pit + 0xB) = NPC_Message(*(s32 *)(e + 4), lb_pit.x0, *(u16 *)e, lb_pit.x09);
        break;
    }
}

