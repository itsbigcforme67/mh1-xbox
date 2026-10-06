/* lbshop2 - lobby.bin 0x005AE8D0-0x005AFFA0: item shop (Lb_shop and its list/detail/decide handlers), the copy of the
 * forge shop code (lb_mix_nm.c) for buying/selling items. Near-match file in address order (tools/lbmerge.py). */
#pragma readonly_strings on
#include "lbshop2_proto.h"

void Lb_shop_init_member()
{
    lbShop.x1B = 0;
    Lb_shop_tag_init();
    switch (lb_sys.x68) {
    case 0xA:
        lbShop.tbl = material_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 9:
        lbShop.tbl = tool_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 0xC:
        lbShop.tbl = foods_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 3:
        if (*(u8 *)0x3F3404 == 0x57) {
            lbShop.tbl = goods_shop_local;
        } else {
            lbShop.tbl = goods_shop_tbl;
        }
        break;
    case 0x22:
        lbShop.tbl = goods_shop_local2;
        break;
    case 0xD:
        break;
    }
}

void Lb_shop(void) {
    EMW *pl = ((EMW **)((u8 *)D_3E4FA0 + *(u8 *)0x3F34C1 * 0xA00))[0];
    LB_NPCW *npc = (LB_NPCW *)((u8 *)pl + 0x444);
    int r;

    switch (lbShop.step) {
    case 0:
        Lb_shop_init_member();
        lbShop.x16 = 0;
        lbShop.x17 = 2;
        lbShop.x18 = 1;
        lbShop.f20 = lb_shop_init;
        lbShop.f24 = lb_shop_tag_decide;
        lbShop.f2C = lb_shop_select;
        lbShop.f30 = lb_shop_item_select;
        lbShop.f34 = lb_shop_decide;
        lbShop.f3C = lb_shop_put_itemDetail;
        lbShop.list = shopList;
        lbShop.f44 = lb_shop_listIcon;
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
        r = Lb_shop_move();
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
        lb_sys.x6C = 0;
        lbShop.step = 0;
        lbShop.mode = 0;
        NPCZoomInCameraCancel();
        lb_sys.x87 = 0x14;
        break;
    }
    Lb_shop_talk();
}

int lb_shop_select(void) {
    int id;

    if (lbShop.mode == 0) {
        id = lbShop.tbl[lbShop.cur];
        if (lb_monster_list_check(id) == 1) {
            if (Monster_list_chk((id - 0x125) & 0xFF, 1) == 1) {
                return 0;
            }
        } else if (Lb_shop_item_checkMax(id & 0xFFFF, 1) == 0) {
            return 0;
        }
        if (Item_data[id].type == 4 && *(u8 *)0x3C738D != 7) {
            Lb_put_set01(0);
        } else if (id == 0x69 && *(u8 *)0x3C738D != 6) {
            Lb_put_set01(0xB);
        }
        lbShop.help = shop_default_help[0];
        lbShop.f40 = lb_shop_put_shopHelp;
    } else {
        if (User_data[0].item[lbShop.cur].num <= 0) {
            return 0;
        }
        if (lbShop.mode == 1) {
            lbShop.help = shop_default_help[1];
        }
        lbShop.f40 = lb_shop_put_shopHelp;
    }
    cnWrap_SoundRequest(0);
    shop_tex_rotate[2] &= 0xFFFFFF;
    shop_tex_rotate[7] &= 0xFFFFFF;
    return 1;
}

void lb_shop_put_itemDetail(void) {
    s16 have = 0;
    int t;
    int id;
    s16 i;
    int off;

    Lb_draw_square(0x11F, 0xFC, 0x141, 2, 0xFF602020, 1);
    if (lbShop.mode == 0) {
        id = lbShop.tbl[lbShop.cur];
    } else {
        if (User_data[0].item[lbShop.cur].num <= 0) {
            return;
        }
        id = User_data[0].item[lbShop.cur].id;
    }
    flfntSetSize(0x12, 0x12);
    if (lbShop.x1C == 0) {
        off = id * 0x10;
        font_print_ex(0x168, 0x104, Equip_moji_color_rare(*((u8 *)&Item_data[0].rare + off)), shopList[lbShop.cur].name);
        font_set_palette(0);
        Lb_put_msg_type2(lb_shop_msg + 0x18);
        if (lbShop.x15 == 5) {
            Lb_put_msg_type2(lb_shop_msg + 0x20);
            Lb_put_button(0x212, 0x12F, 3);
        }
        flfntSetSize(0x1C, 0x14);
        if (lb_monster_list_check(id) == 1) {
            if (Monster_list_chk((id - 0x125) & 0xFF) == 1) {
                have = 1;
            } else {
                have = 0;
            }
        } else {
            for (i = 0; i < 20; i++) {
                if (User_data[0].item[i].id == id) {
                    have = User_data[0].item[i].num;
                    break;
                }
            }
        }
        t = have;
        if (t == 0xFF) {
            font_print_ex(0x1B0, 0x11A, 2, lit_324_0065E1D0, t);   /* t0 = t (asm 0x5AF160) */
        } else if (t >= *((u8 *)&Item_data[0].max + off)) {
            font_print_ex(0x1B0, 0x11A, 2, lit_325_0065E1D8, t);
        } else {
            font_print_ex(0x1B0, 0x11A, 0, lit_325_0065E1D8, t);
        }
    } else {
        flfntLocate(0x168, 0x108);
        font_set_palette(0);
        font_print_sp(*(s32 *)((id * 4) + *(int *)0x351E84 + 0x60));
    }
    Lb_put_itemIcon(0x122, 0x102, 0x36, id);
    Lb_put_itemRare(0x12A, 0x136, (s8)Item_data[id].rare);
}

void lb_shop_listIcon(int x, int y, int z, s16 n) {
    int v;

    if (lbShop.mode == 0) {
        v = lbShop.tbl[n + lbShop.x6C * 7];
    } else {
        int k = n + lbShop.x6C * 7;
        if (User_data[0].item[k].num <= 0) return;
        v = User_data[0].item[k].id;
    }
    Lb_put_itemIcon(x, y, z, v);
}

void lb_shop_tag_decide(void) {
    LB_SHOPITEM *sl = shopList;
    UD_ITEM *it = User_data[0].item;
    int cnt;
    int i;
    int v;
    int pages;
    int n;

    memset(shopList, 0, 0x5000);
    if (lbShop.mode == 0) {
        cnt = 0;
        if (Ud_item_search_space() != 0) {
            lbShop.help = shop_default_help[5];
        } else {
            lbShop.help = shop_default_help[6];
        }
        n = 0;
        for (i = 0; ; ) {
            v = lbShop.tbl[i];
            if (v == 0xFFFF) {
                break;
            }
            strcpy(sl->name, item_str[v]);
            if (Online_ck() == 1 && *(u32 *)((u8 *)cw + 0xBF3C) >= 4 && lb_sys.x68 != 3) {
                sl->price = Item_data[v].buy >> 1;
            } else {
                sl->price = Item_data[v].buy;
            }
            if (lb_monster_list_check(v) == 1) {
                if (Monster_list_chk((v - 0x125) & 0xFF) == 1 || *(u32 *)0x3C6FE0 < (u32)sl->price) {
                    sl->state = 1;
                } else {
                    sl->state = 0;
                }
            } else if (Lb_shop_item_checkMax(v & 0xFFFF, 1) == 0) {
                sl->state = 1;
            } else {
                sl->state = 0;
            }
            n++;
            cnt++;
            sl++;
            i++;
            if (n >= 0x1F4) {
                break;
            }
        }
        pages = cnt / 7;
        lbShop.count = cnt;
        if (cnt % 7 != 0) {
            pages++;
        }
    } else {
        lbShop.help = shop_default_help[7];
        for (i = 0; i < 20; i++, sl++, it++) {
            if (it->num <= 0) {
                sl->state = 2;
            } else {
                v = it->id;
                strcpy(sl->name, item_str[v]);
                sl->price = Item_data[v].sell;
                sl->state = 0;
            }
        }
        pages = 3;
        lbShop.count = 0x14;
    }
    lbShop.x6D = pages;
}

int lb_shop_item_select(void) {
    int id;
    int keys;
    int q;
    int i;

    keys = lbShop.key & 0xFFFF;
    if (lbShop.mode == 0) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    if (keys & 0x20) {
        lbShop.x78 = 0;
        lbShop.help = shop_default_help[2 + lbShop.mode];
        if (lbShop.mode == 1 && Item_data[id].sell == 0) {
            lbShop.help = shop_default_help[4];
        }
        cnWrap_SoundRequest(0);
        return 0;
    }
    if (keys & 0x40) {
        lbShop.help = 0;
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (keys & 0x1000) {
        q = lbShop.qty - 1;
        lbShop.qty = q;
        if (q <= 0) {
            cnWrap_SoundRequest(7);
            lbShop.qty = 1;
        } else {
            cnWrap_SoundRequest(1);
        }
    } else if (keys & 0x2000) {
        if (Lb_shop_item_checkMax(id & 0xFFFF, (s8)(lbShop.qty + 1)) == 0) cnWrap_SoundRequest(7);
        else cnWrap_SoundRequest(1);
        if (Lb_shop_item_checkMax(id & 0xFFFF, (s8)(lbShop.qty + 1)) == 1) lbShop.qty++;
    } else if (keys & 0x800) {
        if (lbShop.qty >= 2) {
            lbShop.qty = 1;
            cnWrap_SoundRequest(1);
        }
    } else if (keys & 0x400) {
        i = 0;
        if (Lb_shop_item_checkMax(id & 0xFFFF, (s8)(lbShop.qty + 1)) != 0) {
            for (;;) {
                s16 k = i;
                if (Lb_shop_item_checkMax(id & 0xFFFF, (s8)(lbShop.qty + k)) == 0) {
                    lbShop.qty += k - 1;
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
    if (Lb_shop_item_checkMax(id & 0xFFFF, (s8)(lbShop.qty + 1)) == 0) shop_tex_rotate[7] &= 0xFFFFFF;
    else shop_tex_rotate[7] |= 0xFF000000;
    return 2;
}

int Lb_shop_item_checkMax(id, qty)
s32 id;
int qty;
{
    short cnt = Ud_item_num_ck3(id);
    int i;
    u8 *p;

    switch (lbShop.mode) {
    case 0:
        if (cnt == -1) return 0;
        if (cnt == 0xFF) {
            if ((s8)qty == 1 && Ud_item_num_ck(id) == 0 && CheckItemPrice_005AFEE0(id, (s8)qty) == 1 &&
                (short)Ud_item_search_space() == 1) {
                return 1;
            }
        } else if (cnt >= (s8)qty && CheckItemPrice_005AFEE0(id, (s8)qty) == 1) {
            return 1;
        }
        break;
    default:
        p = (u8 *)User_data;
        for (i = 0; i < 20; i++, p += 4) {
            if (*(u16 *)(p + 0x37C) == (u16)id) {
                if (*(s16 *)(p + 0x37E) == 0xFF) {
                    if ((s8)qty < 2) return 1;
                } else if (*(s16 *)(p + 0x37E) >= (s8)qty) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void lb_shop_decide(void) {
    int id;

    if (lbShop.mode == 0) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    switch (lbShop.mode) {
    case 0:
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur, lbShop.mode);
        if (lb_monster_list_check(id) == 1) {
            Add_to_Monster_list((id - 0x125) & 0xFF);
            Lb_put_set01(5);
        } else {
            Ud_item_stack(id & 0xFFFF, (s16)lbShop.qty);
        }
        cnWrap_SoundRequest(8);
        break;
    case 1:
        Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur, lbShop.mode);
        cnWrap_SoundRequest(8);
        if (Item_data[id].max != 0xFF) {
            Ud_item_stack(id & 0xFFFF, (s16)-lbShop.qty);
        } else {
            Ud_item_erase((s16)lbShop.cur);
        }
        break;
    }
    lbShop.help = 0;
}

int CheckItemPrice_005AFEE0(id, qty)
u16 id;
s16 qty;
{
    if (*(u32 *)((u8 *)cw + 0xBF3C) >= 4 && lb_sys.x68 != 3) {
        if (*(s32 *)0x3C6FE0 < qty * (Item_data[id].buy >> 1)) {
            return 0;
        }
        return 1;
    }
    if (*(s32 *)0x3C6FE0 < qty * Item_data[id].buy) {
        return 0;
    }
    return 1;
}
