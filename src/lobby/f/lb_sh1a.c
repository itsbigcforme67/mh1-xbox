/* lb_sh1a - one translation unit 0x00537C10-0x00539A90 (lbtu3). */
#include "lobby_s.h"
extern f32 tagMoveY[];
#define flfntLocate_a2 flfntLocate_hdr   /* lobby_a.h declares it K&R; the original caller sign-extends y */
#undef flfntLocate_a2
void flfntLocate_a2(int x, s16 y);
extern char lit_836_00654DD0[];
extern char lit_837_00654DF0[];
extern char lit_838_00654DF8[];
extern char lit_835_00654DB0[];
extern char lb_shop_msg[];
extern char lit_869_00654E00[];
extern char lit_870_00654E10[];
extern char lit_871_00654E28[];
extern char lit_872_00654E40[];
extern char shopStr[];
typedef struct { s16 x; s16 y; u8 pad[0x10]; } SPR20;
extern SPR20 shop_tex_tbl[];
extern char D_3F3728[];
extern char D_3F3714[];
extern s32 shop_process00_tag[1];
extern s8 r_no_process;
extern char D_3E4FA0[];
extern s32 armor_shop_tmp[7];
extern u8 *npc_dialog_table[];
int lb_process_select();
void lb_process_decide();
void lb_process_drawHelp();
int shop_process_after();
void lb_armor2_listItem(int x, int y, int z, s16 n);
void armor_shop2_trans();
s8 Lb_talk_check_default();
extern s32 armorIndex;
extern s32 shop_process01_tag[3];
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
extern char shop_process2_help_c11[];
extern char D_3C738C[];
typedef struct { s16 a, b, c; } S3;
typedef struct { s32 kind; s32 id; } SHTBL;
extern SHTBL shopTbl_c14[];
extern u8 kakou_tbl[];
extern char *gun_kyouka_tbl[];
extern s32 lvup_price[];
extern LB_SHOPITEM shopList2[];
int Get_Gun_level(char *, s16);
int Gun_option_ck(char *, s16, int);
s32 Lb_shop_move_x();
s32 Lb_shop_move_xR();
void Lb_put_shopCursor();
void lb_put_shopList();
void lb_put_shopHelp();
void Lb_put_shopYesNo();
char * Lb_make_price_str(char *arg0, int arg1);
void lb_put_mk_tags();
s32 Lb_shop_sw();
void lb_process_init();
void Lb_process_shop();
void lb_process_tag_decide00();
void lb_process_set_weaponList();
s16 lb_process_set_armorList();
void lb_process_tag_decide01();
void Lb_make_mySrcEquip(int arg0);
s32 check_items(u8 *p, s8 stocked);
void lb_process_make_kyoukaList();
int Lb_make_mySrcEquip_k();
int Lb_make_price_str_k();
int check_items_k();
s32 Lb_shop_move_x() {
    s16 *p;
    u32 i;

    i = 0;
    p = &lbShop.pos[0][0];
    do {
        if (lbShop.x16 != 0) {
            if (i == lbShop.x1A) {
                p[0] = (s16)(-25.0f + (f32)p[0]);
                p[1] = (s16)((f32)p[1] + tagMoveY[lbShop.x1A]);
            } else {
                p[0] += 0x22;
            }
        } else if (i == lbShop.mode) {
            p[0] = (s16)(-25.0f + (f32)p[0]);
            p[1] = (s16)((f32)p[1] + tagMoveY[lbShop.mode]);
        } else {
            p[0] += 0x22;
        }
        i++;
        p += 2;
    } while (i < 5U);
    if (--lbShop.wait == 0) {
        lbShop.x70 = 0;
        lbShop.wait = 6;
        return 1;
    }
    return 0;
}

s32 Lb_shop_move_xR() {
    s16 *p;
    u32 i;

    i = 0;
    p = &lbShop.pos[0][0];
    do {
        if (lbShop.x16 != 0) {
            if (i == lbShop.x1A) {
                p[0] = (s16)((f32)p[0] - -25.0f);
                p[1] = (s16)((f32)p[1] - (f32)(s32)tagMoveY[lbShop.x1A]);
            } else {
                p[0] -= 0x22;
            }
        } else if (i == lbShop.mode) {
            p[0] = (s16)((f32)p[0] - -25.0f);
            p[1] = (s16)((f32)p[1] - (f32)(s32)tagMoveY[lbShop.mode]);
        } else {
            p[0] -= 0x22;
        }
        i++;
        p += 2;
    } while (i < 5U);
    if (--lbShop.wait == 0) {
        lbShop.x70 = 0;
        lbShop.wait = 6;
        return 1;
    }
    return 0;
}

void Lb_put_shopCursor() {
    Sel_csr_disp(0x1C1, (s16)(lbShop.x70 * 0x18 + 0x4E), 0x17C, 0x18, 0xB0008000);
}

void lb_put_shopList() {
    char buf[0x20];
    LB_SHOPITEM *it;
    int i;
    int y1;
    int y0;

    it = (LB_SHOPITEM *)((u8 *)lbShop.list + lbShop.x6C * 0x118);
    Draw_menu_square(0x118, 0x30, 0x152, 0x122, 0, 0);
    if (lbShop.count == 0) {
        if (lbShop.x8E == 3) {
            flfntSetSize(0x14, 0x14);
            font_print_double(0x140, 0x96, 1, 4, lit_835_00654DB0);
        }
        return;
    }
    Lb_put_shopCursor();
    i = 0;
    y1 = 0x50;
    y0 = 0x4E;
    for (; i < 7; i++, it++, y1 += 0x18, y0 += 0x18) {
        if (i + lbShop.x6C * 7 >= lbShop.count) {
            break;
        }
        if (it->state == 0) {
            font_set_palette(0);
        } else if (it->state == 3) {
            font_set_palette(6);
        } else if (it->state == 4) {
            font_set_palette(6);
        } else {
            font_set_palette(0xA);
        }
        if (it->state == 2) {
            flfntSetSize(0x14, 0x14);
            flfntSetSize(0x14, 0x14);
            flfntLocate_a2(0x122, y1);
            font_print(&lit_836_00654DD0);
        } else {
            flfntSetSize(0x14, 0x14);
            flfntLocate_a2(0x13C, y1);
            font_print(&lit_837_00654DF0, it->name);
            flfntSetSize(0x14, 0x14);
            flfntLocate_a2(0x208, y1);
            sprintf(buf, &lit_838_00654DF8, it->price);
            font_print(&lit_837_00654DF0, buf);
            if (lbShop.f44 != 0) {
                ((void (*)(int, s16, int, s16))lbShop.f44)(0x11C, y0, 0x18, i);
            }
        }
    }
}

void lb_put_shopHelp() {
    if (lbShop.count != 0) {
        if (lbShop.help != 0) {
            Draw_menu_square(0x118, 0x160, 0x152, 0x50, 1, 0x7030100B);
            flfntSetSize(0x12, 0x12);
            font_set_palette(0);
            flfntLocate(0x122, 0x16C);
            font_print_sp(Lb_make_price_str_k(lbShop.help, lbShop.qty * ((LB_SHOPITEM *)lbShop.list)[lbShop.cur].price, lbShop.cur));
            if (lbShop.x15 == 7) {
                Lb_put_shopYesNo(lbShop.x15);
            }
        }
        if ((lbShop.f40 != 0) && ((lbShop.x15 == 6) || (lbShop.x15 == 8))) {
            lbShop.f40(lbShop.x15, lbShop.f40);
        }
        if ((lbShop.x84 == 1) && (lbShop.x15 == 5)) {
            Lb_put_button(0x212, 0x190, 3);
            Lb_put_msg2(0x230, 0x194, F(s32, &lb_shop_msg, 0x24));
        }
    }
}

void Lb_put_shopYesNo() {
    if (F(s8, &lbShop, 0x78) == 0) {
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x1B0, 0x194);
        font_set_palette(2);
        font_print(&lit_869_00654E00);
        font_set_palette(0);
        font_print(&lit_870_00654E10);
        return;
    }
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x1B0, 0x194);
    font_set_palette(0);
    font_print(&lit_871_00654E28);
    font_set_palette(2);
    font_print(&lit_872_00654E40);
}

char *Lb_make_price_str(char *arg0, int arg1) {
    char *temp_v0;

    strcpy(shopStr, arg0);
    temp_v0 = (char *)strrchr(shopStr, 0x24);
    if (temp_v0 == 0) {
        return arg0;
    }
    Lb_num_to_str(arg1, temp_v0);
    strcat(shopStr, (char *)strrchr(arg0, 0x24) + 1);
    return shopStr;
}

void lb_put_mk_tags() {
    SPR20 *tx;
    s16 *pos;
    s32 *tag;
    int i;

    tag = lbShop.tag;
    tx = shop_tex_tbl;
    pos = &lbShop.pos[0][0];
    flfntSetSize(0x14, 0x14);
    for (i = 0; i < lbShop.x17; i++) {
        tx[0].x = pos[0] + 0x8C;
        tx[0].y = pos[1] + 2;
        Lb_put_2TF(&tx[0], 1);
        tx[1].x = pos[0];
        tx[1].y = pos[1] + 2;
        Lb_put_2TF(&tx[1], 1);
        flfntLocate((s16)(pos[0] + 0x10), (s16)(pos[1] + 8));
        font_set_palette(0);
        font_print(&lit_837_00654DF0, *tag);
        tag++;
        pos += 2;
    }
}

s32 Lb_shop_sw(arg0)
int arg0;
{
    s32 v;
    s32 t;

    v = 0;
    if (lb_sys.x8E < 3) {
        return 0;
    }
    if (SoftKeyboard_alive_check() == 0) {
        t = ((s8)arg0) * 0x22;
        v = ((*(u16 *)((u8 *)&D_3F3728 + t) & 0x3C00) | *(u16 *)((u8 *)&D_3F3714 + t)) & 0xFFFF;
    }
    if (v & 0xFFFF) {
        lb_sys.x8E = 0;
    }
    return v;
}

void lb_process_init() {
    lbShop.tag = shop_process00_tag;
    lbShop.x17 = 2;
}

void Lb_process_shop() {
    int pl;
    int i;
    int c;
    int r;

    pl = *(s32 *)(D_3E4FA0 + *(u8 *)0x3F34C1 * 0xA00) + 0x444;
    switch (lbShop.step) {
    case 0:
        lbShop.x8E = 2;
        lbShop.x16 = 1;
        lbShop.x8F = 1;
        r_no_process = 0;
        lbShop.f20 = lb_process_init;
        lbShop.f24 = lb_process_tag_decide00;
        lbShop.f28 = lb_process_tag_decide01;
        lbShop.f2C = lb_process_select;
        lbShop.f34 = lb_process_decide;
        lbShop.f3C = lb_process_drawHelp;
        lbShop.f38 = shop_process_after;
        lbShop.list = shopList;
        lbShop.f44 = lb_armor2_listItem;
        lbShop.step++;
        lbShop.x18 = 0;
        lbShop.f30 = 0;
        lbShop.x15 = 0;
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[*(u8 *)(pl + 0xE)];
        lb_pit.x08 = 0;
        armor_shop_tmp[0] = -1;
        armor_shop_tmp[1] = -1;
        armor_shop_tmp[2] = -1;
        armor_shop_tmp[3] = -1;
        armor_shop_tmp[4] = -1;
        armor_shop_tmp[5] = -1;
        armor_shop_tmp[6] = -1;
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, 0, 0);
        break;
    case 1:
        if (Lb_talk_check_default(0) != 0) lbShop.step++;
        break;
    case 2:
        r = Lb_shop_move();
        switch (r) {
        case 0:
        case 3:
            i = 0;
            c = 0;
            for (; i < 7; i++) {
                if (armor_shop_tmp[i] != -1) c++;
            }
            if (c != 0) {
                lb_pit.x0 = 0;
                lb_pit.x08 = 2;
                lbShop.step++;
            } else {
                lb_pit.x0 = 0;
                lbShop.step = 4;
                lb_pit.x08 = 3;
            }
            Lbc_set_prim(0, 0, 0);
            break;
        }
        break;
    case 3:
        if (Lb_talk_check_default(0) != 0) {
            if (lb_pit.x09 == 0) {
                lbShop.step = 0;
                lb_pit.x08 = 0;
                lb_pit.x0 = 0;
                Lb_shop_tag_init();
            } else {
                lb_pit.x0 = 0;
                lbShop.step++;
                lb_pit.x08 = 3;
            }
        }
        break;
    case 4:
        if (Lb_talk_check_default(0) != 0) {
            lbShop.x1B = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x68 = 0;
            lb_sys.x6C = 0;
            lbShop.step = 0;
            lbShop.mode = 0;
            NPCZoomInCameraCancel();
            Lbc_set_prim(0, 0, 0);
        }
        break;
    }
    armor_shop2_trans();
}

void lb_process_tag_decide00() {
    if (lbShop.mode == 0) {
        lbShop.tag = shop_process01_tag;
        lbShop.x17 = 2;
    } else {
        lbShop.tag = shop_process01_tag + 2;
        lbShop.x17 = 5;
    }
    lbShop.x70 = 0;
    armorIndex = 0;
    lbShop.f40 = 0;
}

void lb_process_set_weaponList() {
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

s16 lb_process_set_armorList() {
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

void lb_process_tag_decide01() {
    int a0;

    memset(&shopList, 0, 0x5000);
    if (lbShop.mode == 0) {
        lbShop.help = ((s32 *)&shop_process2_help_c11)[lbShop.x1A];
        lb_process_set_weaponList();
        a0 = (s16)*(u8 *)0x3C738D;
    } else {
        lbShop.help = ((s32 *)&shop_process2_help_c11)[2];
        a0 = (s16)lb_process_set_armorList();
    }
    if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
        lbShop.x70 = armorIndex;
    } else {
        if (armorIndex >= lbShop.count) {
            armorIndex = lbShop.count - 1;
        }
        lbShop.x70 = armorIndex % 7;
    }
    lbShop.f40 = 0;
    lbShop.x1C = 0;
    if (lbShop.x6C >= lbShop.x6D) {
        lbShop.x6C = lbShop.x6D - 1;
    }
    Lb_make_mySrcEquip_k(a0);
}

void Lb_make_mySrcEquip(int arg0) {
    F(s8, &lbShop, 0x55) = (s8) arg0;
    switch ((s16)arg0) {
    case 6:
    case 7:
        *(S3 *)((u8 *)&lbShop + 0x54) = *(S3 *)&D_3C738C;
        break;
    case 2:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7393;
        break;
    case 3:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7394;
        break;
    case 4:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7395;
        break;
    case 5:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7396;
        break;
    case 0:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7392;
        break;
    }
    F(s8, &lbShop, 0x54) = 1;
}

s32 check_items(u8 *p, s8 stocked) {
    s16 have;
    s16 stock;

    stock = 0;
    have = Ud_item_num_ck(*(u16 *)p);
    if (stocked == 0) {
        stock = Ud_stock_item_num_ck3(*(u16 *)p);
    }
    if (have + stock >= *(s16 *)(p + 2)) {
        return 1;
    }
    return 0;
}

void lb_process_make_kyoukaList() {
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
    t = shopTbl_c14;
    lbShop.tbl = (s32 *)shopTbl_c14;
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
                    if (*(u16 *)p != 0 && check_items_k(p, 1) != 1) {
                        if (check_items_k(p, 0) == 1 && sl->state != 1) {
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
    shopTbl_c14[0].id = 0;
    shopTbl_c14[1].id = 0;
    shopTbl_c14[0].kind = 7;
    shopTbl_c14[1].kind = 7;
    shopTbl_c14[2].kind = 7;
    shopTbl_c14[2].id = 0;
    shopTbl_c14[3].kind = 7;
    shopTbl_c14[3].id = 0;
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

