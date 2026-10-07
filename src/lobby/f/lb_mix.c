/* lb_mix - one translation unit 0x00535240-0x00536724 (lbtu3). */
#include "lobby.h"
#include "pl.h"
#include "em.h"
#include "ud.h"
void Lb_shop_tag_init();
void Lb_shop_talk();
s8 Lb_talk_check_default();
int Lb_shop_move();
void Lbc_set_prim();
void NPCZoomInCameraCancel();
void cnWrap_SoundRequest();
void flMemset();
extern s32 shop_default_tag_00610A68[];
extern u8 *npc_dialog_table[];
extern s32 shopTbl[];
short Ud_item_num_ck3();
short Ud_item_num_ck();
int Ud_item_search_space();
/* can `qty` of item `id` be bought / is it held (mode 2) */
extern s32 shop_mix_help[];
void lb_shop_put_shopHelp();
extern u8 D_2E8892[];
extern char *item_str[];
void *Item_preparation_get();
int Item_preparation_check_list();
char *strcpy();
void Lb_draw_square();
void Lb_put_button();
void Lb_put_itemIcon();
void Lb_put_itemRare();
void Lb_put_materialBase();
void Lb_put_materialItem();
void Lb_put_msg_type2();
void flfntLocate();
void flfntSetSize();
void font_print_ex();
void font_print_sp();
void font_set_palette();
int Equip_moji_color_rare();
extern u8 lb_shop_msg[];
extern char lit_359_00654D68[];
extern char lit_360_00654D70[];
extern u8 **item_exp;
extern s32 mix_shop_tbl[];
void *memset();
int Ud_item_search_space_();
extern void *Lb_mix_item_checkMax_p;
void Gold_add();
int Item_preparation();
extern s32 *pit_help_str_tbl[];
/* can `qty` of item `id` be bought / is it held (mode 2) */
/* can `qty` of item `id` be bought / is it held (mode 2) */
/* can `qty` of item `id` be bought / is it held (mode 2) */
/* can `qty` of item `id` be bought / is it held (mode 2) */
void Lb_mix_init_member(EMW *pl);
void lb_mix_init();
void Lb_mix();
int Lb_mix_item_checkMax(s32 id, s8 qty);
int lb_mix_select();
void lb_mix_put_itemDetail();
int lb_mix_checkItemMake(LB_MIXDATA *m, int result);
int lb_mix_makeMixList(s8 mode);
void lb_mix_tag_decide();
int lb_mix_item_select();
void lb_mix_decide();
void lb_mix_listIcon(int x, int y, int z, s16 n);
int CheckItemPrice(int id, int qty);
void Lb_shop_init();
int Lb_mix_item_checkMax_k();
int Lb_mix_item_checkMax_u(u16 id, s8 qty);
int lb_mix_checkItemMake_k();
int lb_mix_makeMixList_k();
/* can `qty` of item `id` be bought / is it held (mode 2) */

void Lb_mix_init_member(EMW *pl) {
    lbShop.x1B = 0;
    Lb_shop_tag_init();
}

void lb_mix_init() {
    lbShop.tag = shop_default_tag_00610A68;
}

void Lb_mix() {
    EMW *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    LB_NPCW *npc = (LB_NPCW *)pl->ex;
    int r;

    switch (lbShop.step) {
    case 0:
        Lb_mix_init_member(pl);
        lbShop.x16 = 0;
        lbShop.x17 = 3;
        lbShop.x18 = 1;
        lbShop.f20 = lb_mix_init;
        lbShop.f24 = lb_mix_tag_decide;
        lbShop.f2C = lb_mix_select;
        lbShop.f30 = lb_mix_item_select;
        lbShop.f34 = lb_mix_decide;
        lbShop.f3C = lb_mix_put_itemDetail;
        lbShop.f44 = lb_mix_listIcon;
        lbShop.list = shopList;
        lbShop.tbl = shopTbl;
        lbShop.x8E = 0;
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[npc->kind];
        lbShop.step++;
        lb_pit.x08 = 0;
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, 0, 0);
        break;
    case 1:
        if (Lb_talk_check_default(0) != 0) lbShop.step++;
        break;
    case 2:
        r = Lb_shop_move(pl);
        switch (r) {
        case 0:
        case 3:
            lb_pit.x0 = 0;
            lb_pit.x08 = 2;
            lbShop.step++;
            Lbc_set_prim(0, 0, 0);
            break;
        }
        break;
    case 3:
        if (Lb_talk_check_default(0) != 0) lbShop.step++;
        break;
    case 4:
        lbShop.x1B = 0;
        lb_sys.x68 = 0;
        lb_sys.x87 = 0x14;
        lb_sys.x6C = 0;
        lbShop.step = 0;
        lbShop.mode = 0;
        NPCZoomInCameraCancel();
        Lbc_set_prim(0, 0, 0);
        break;
    }
    Lb_shop_talk();
}

int Lb_mix_item_checkMax(s32 id, s8 qty) {
    short cnt = Ud_item_num_ck3(id);
    int i;
    u8 *p;

    switch (lbShop.mode) {
    case 0:
    case 1:
        if (cnt == -1) return 0;
        if (cnt == 0xFF) {
            if (qty == 1 && Ud_item_num_ck(id) == 0 && CheckItemPrice(id, qty) == 1 &&
                (short)Ud_item_search_space() == 1) {
                return 1;
            }
        } else if (qty <= cnt && CheckItemPrice(id, qty) == 1) {
            return 1;
        }
        break;
    default:
        for (i = 0, p = (u8 *)User_data; i < 20; i++, p += 4) {
            if (*(u16 *)(p + 0x37C) == (u16)id) {
                if (*(s16 *)(p + 0x37E) == 0xFF) {
                    if (qty < 2) return 1;
                } else if (*(s16 *)(p + 0x37E) >= qty) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int lb_mix_select() {
    switch (lbShop.mode) {
    case 0:
        lbShop.help = shop_mix_help[3 + lbShop.mode];
        break;
    case 1:
        if (Lb_mix_item_checkMax(*(u16 *)(lbShop.tbl + lbShop.cur), 1) == 0) return 0;
        lbShop.help = shop_mix_help[lbShop.mode];
        break;
    case 2:
        if (User_data[0].item[lbShop.cur].num <= 0) return 0;
        lbShop.help = shop_mix_help[lbShop.mode];
        break;
    }
    cnWrap_SoundRequest(0);
    lbShop.f40 = lb_shop_put_shopHelp;
    shop_tex_rotate[2] &= 0xFFFFFF;
    shop_tex_rotate[7] &= 0xFFFFFF;
    return 1;
}

void lb_mix_put_itemDetail() {
    int have = 0;
    int t;
    int id;
    s16 i;
    int off;
    u8 *rp;
    LB_MIXDATA *md = mixData;

    if (lbShop.count == 0) return;
    Lb_draw_square(0x11F, 0xFC, 0x141, 2, 0xFF602020, 1);
    if (lbShop.mode == 1 || lbShop.mode == 0) {
        id = lbShop.tbl[lbShop.cur];
    } else {
        if (User_data[0].item[lbShop.cur].num <= 0) return;
        id = User_data[0].item[lbShop.cur].id;
    }
    flfntSetSize(0x12, 0x12);
    switch (lbShop.x1C) {
    case 0:
        off = id * 0x10;
        rp = (u8 *)&Item_data[0].rare + off;
        font_print_ex(0x168, 0x104, Equip_moji_color_rare(*rp), shopList[lbShop.cur].name);
        font_set_palette(0);
        Lb_put_msg_type2(lb_shop_msg + 0x18);
        if (lbShop.x15 == 5) {
            Lb_put_button(0x212, 0x12F, 3);
            Lb_put_msg_type2(lb_shop_msg + 0x20);
            if (lbShop.mode == 0) {
                Lb_put_button(0x190, 0x12F, 6);
                Lb_put_msg_type2(lb_shop_msg + 0x30);
            }
        }
        flfntSetSize(0x1C, 0x14);
        for (i = 0; i < 20; i++) {
            if (User_data[0].item[i].id == id) {
                have = User_data[0].item[i].num;
                break;
            }
        }
        t = (s16)have;
        if (t == 0xFF) {
            font_print_ex(0x1B0, 0x11A, 2, lit_359_00654D68, t);   /* count in t0, as lb_shop_put_itemDetail [same layout, not checked in this asm] */
        } else if (t >= *((u8 *)&Item_data[0].max + off)) {
            font_print_ex(0x1B0, 0x11A, 2, lit_360_00654D70, t);
        } else {
            font_print_ex(0x1B0, 0x11A, 0, lit_360_00654D70, t);
        }
        Lb_put_itemIcon(0x122, 0x102, 0x36, id);
        Lb_put_itemRare(0x12A, 0x136, (s8)*rp);
        return;
    case 1:
        flfntLocate(0x168, 0x108);
        font_set_palette(0);
        font_print_sp(pit_help_str_tbl[1][id + 0x18]);
        Lb_put_itemIcon(0x122, 0x102, 0x36, id);
        Lb_put_itemRare(0x12A, 0x136, (s8)Item_data[id].rare);
        return;
    case 2: {
        u8 *rec;
        md += lbShop.cur;
        rec = md->rec;
        Lb_put_materialBase(lbShop.x1C);
        flfntLocate(0x168, 0x108);
        Lb_put_materialItem(0x104, md->no, 1);
        flfntLocate(0x134, 0x118);
        Lb_put_materialItem(0x118, *(s16 *)rec, 1);
        Lb_put_itemIcon(0x130, 0xD0, 0x20, *(s16 *)(rec + 2));
        break;
    }
    }
}

/* can `qty` of item `id` be bought / is it held (mode 2) */

int lb_mix_checkItemMake(LB_MIXDATA *m, int result) {
    int ok = Ud_item_num_ck3(*(u16 *)(m->rec + 2)) > 0;
    if (!ok) return 0;
    if (Ud_item_num_ck(*(u16 *)m) == 0) return 0;
    return Ud_item_num_ck(*(u16 *)m->rec) != 0;
}

/* can `qty` of item `id` be bought / is it held (mode 2) */

int lb_mix_makeMixList(s8 mode) {
    int num;
    u8 *rt;
    int idx;
    LB_SHOPITEM *sl;
    u8 *rec;
    LB_MIXDATA *md;
    int j;
    int no;
    s32 *tb;
    int cnt;

    num = 0;
    rt = D_2E8892;
    sl = shopList;
    md = mixData;
    no = 1;
    tb = shopTbl;
    lbShop.tbl = tb;
    do {
        cnt = rt[0];
        if (cnt != 0) {
            idx = rt[1];
            for (j = 0; j < cnt; j++, idx++) {
                u32 price;
                rec = Item_preparation_get((s16)idx);
                if (Item_preparation_check_list((s8)rec[5]) == 1) {
                    md->no = no;
                    price = Item_data[*(s16 *)(rec + 2)].buy;
                    md->price = 0.5f * price;
                    md->rec = rec;
                    strcpy(sl->name, item_str[*(s16 *)(rec + 2)]);
                    *tb = *(s16 *)(rec + 2);
                    sl->price = md->price;
                    if (lb_mix_checkItemMake_k(md, *(s16 *)(rec + 2)) == 0 || md->price > *(s32 *)0x3C6FE0) {
                        sl->state = 1;
                    } else {
                        sl->state = 0;
                    }
                    tb++;
                    num++;
                    md++;
                    sl++;
                }
            }
        }
        no++;
        rt += 2;
    } while (no < 0x147);
    return num;
}

/* can `qty` of item `id` be bought / is it held (mode 2) */

void lb_mix_tag_decide() {
    UD_ITEM *it = User_data[0].item;
    int i;
    int cnt;
    LB_SHOPITEM *sl = shopList;
    int v;
    int pages;

    memset(shopList, 0, 0x5000);
    switch (lbShop.mode) {
    case 0:
        cnt = lb_mix_makeMixList_k(lbShop.mode);
        lbShop.x18 = 0;
        lbShop.x8E = 3;
        if (Ud_item_search_space() != 0) lbShop.help = shop_mix_help[6];
        else lbShop.help = shop_mix_help[7];
        break;
    case 1:
        lbShop.tbl = mix_shop_tbl;
        if (Ud_item_search_space(lbShop.mode) != 0) lbShop.help = shop_mix_help[8];
        else lbShop.help = shop_mix_help[9];
        cnt = 0;
        Ud_item_search_space();
        for (i = 0; i < 100; i++) {
            v = lbShop.tbl[i];
            if (v == 0xFFFF) break;
            strcpy(sl->name, item_str[v]);
            sl->price = Item_data[v].buy;
            if (Lb_mix_item_checkMax_k(v & 0xFFFF, 1) == 0) sl->state = 1;
            else sl->state = 0;
            cnt++;
            sl++;
        }
        lbShop.x8E = 0;
        lbShop.x18 = 1;
        break;
    case 2:
        lbShop.help = shop_mix_help[10];
        for (i = 0; i < 20; i++, sl++, it++) {
            if (it->num <= 0) {
                sl->state = 2;
            } else {
                u16 id = it->id;
                strcpy(sl->name, item_str[id]);
                sl->price = Item_data[id].sell;
                sl->state = 0;
            }
        }
        lbShop.x8E = 0;
        cnt = 0x14;
        lbShop.x18 = 1;
        break;
    }
    pages = cnt / 7;
    lbShop.count = cnt;
    if (cnt % 7 != 0) pages++;
    lbShop.x6D = pages;
}

int lb_mix_item_select() {
    s16 kk;
    int i;
    int q;
    int keys;
    int k;
    int id;

    keys = lbShop.key;
    if (lbShop.mode == 1) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    k = keys & 0xFFFF;
    if (k & 0x20) {
        lbShop.x78 = 0;
        lbShop.help = shop_mix_help[3 + lbShop.mode];
        cnWrap_SoundRequest(0);
        return 0;
    }
    if (k & 0x40) {
        lbShop.help = 0;
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (k & 0x1000) {
        q = lbShop.qty - 1;
        lbShop.qty = q;
        if (q <= 0) {
            cnWrap_SoundRequest(7);
            lbShop.qty = 1;
        } else {
            cnWrap_SoundRequest(1);
        }
    } else if (k & 0x2000) {
        if (Lb_mix_item_checkMax_u((u16)id, (s8)(lbShop.qty + 1)) == 0) cnWrap_SoundRequest(7);
        else cnWrap_SoundRequest(1);
        if (Lb_mix_item_checkMax_u((u16)id, (s8)(lbShop.qty + 1)) == 1) lbShop.qty++;
    } else if (k & 0x800) {
        if (lbShop.qty > 1) {
            lbShop.qty = 1;
            cnWrap_SoundRequest(1);
        }
    } else if (k & 0x400) {
        if (Lb_mix_item_checkMax_u((u16)id, (s8)(lbShop.qty + 1)) != 0) {
            i = 0;
            for (;;) {
                kk = i;
                if (Lb_mix_item_checkMax_u((u16)id, (s8)(lbShop.qty + kk)) == 0) {
                    lbShop.qty += kk - 1;
                    break;
                }
                i = (s16)(i + 1);
                if (i >= 0xFF) break;
            }
            cnWrap_SoundRequest(1);
        }
    }
    if (lbShop.qty == 1) shop_tex_rotate[2] &= 0xFFFFFF;
    else shop_tex_rotate[2] |= 0xFF000000;
    if (Lb_mix_item_checkMax_u((u16)id, (s8)(lbShop.qty + 1)) == 0) shop_tex_rotate[7] &= 0xFFFFFF;
    else shop_tex_rotate[7] |= 0xFF000000;
    return 2;
}

#ifdef __MWERKS__
asm void lb_mix_decide()
{
#include "lb_mix_decide.inc"
}
#endif

/* can `qty` of item `id` be bought / is it held (mode 2) */

void lb_mix_listIcon(int x, int y, int z, s16 n) {
    int v;

    if (lbShop.mode != 2) {
        v = lbShop.tbl[n + lbShop.x6C * 7];
    } else {
        int k = n + lbShop.x6C * 7;
        if (User_data[0].item[k].num <= 0) return;
        v = User_data[0].item[k].id;
    }
    Lb_put_itemIcon(x, y, z, v);
}

int CheckItemPrice(id, qty)
u16 id;
s16 qty;
{
    return *(s32 *)0x3C6FE0 >= qty * Item_data[id].buy;
}

void Lb_shop_init() {
    memset(&lbShop, 0, 0x90);
}

