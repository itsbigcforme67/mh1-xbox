/* Lobby item box UI and plaza chat log (SLPM_654.95 lobby overlay 0x609770-0x60E330). Whole file; runs split into lb_ibNN.c */
#include "lobby_f.h"
extern s8 D_39DAD1[16];
extern s8 D_39DAD0[16];
extern s16 D_39DAD2[16];
extern u8 *ib;
/* item box slots: 4 bytes per slot at User_data + 0x37C (u16 item id, s16 amount) */
typedef struct IBS4 { u16 w[2]; } IBS4;
/* pouch (User_data + 0x1C4) and item box (User_data + 0x37C) slots, 4 bytes each: u16 item id, s16 amount */
#define UPID(i) F(u16, User_data + (i) * 4, 0x1C4)
#define UPNUM(i) F(s16, User_data + (i) * 4, 0x1C6)
#define IBID2(i) F(u16, User_data + (i) * 4, 0x37C)
#define IBNUM2(i) F(s16, User_data + (i) * 4, 0x37E)
#define IBID(u, i) (((IBS4 *)(u))[i].w[0x37C / 2])
#define IBNUM(u, i) (((IBS4 *)(u))[i].w[0x37E / 2])
extern u8 User_data[];
int Ud_u_item_stack(u16, u16);
void Menu_select_mv();
void itembox_cursor_mv();
void PageSelect();
void flps0008();
void ListSelect();
extern char lb_item_box_base[];
extern u32 item_col_tbl[];
extern char *item_str[];
extern char *category_name_str[2];
extern char lit_1923_00668480[];
extern char lit_1924[];
extern char lit_1925[];
extern char lit_1926[];
extern char lit_1927[];
extern char lit_1928[];
void DispFrameMessageA();
void flps0004();
void Get_equip_icon_uv();
int Now_equip_ck();
int Equip_icon_color_rare();
int equip_ok_chk();
int Get_equip_rare();
char *Get_equip_name();
void Lb_put_gold();
void flfntLocate();
extern u8 D_3396DE[];
int u_equip_chk();
int Get_equip_kaitori();
void Gold_add();
u8 *Get_equip_data_ptr();
int Warehouse_equip_out();
int Warehouse_equip();
void Lb_equip_set();
void armor_set_myArmor();
extern u8 D_3396D3[];
int u_item_chk();
int pick_kosuu_sel_chk();
int item_kosuu_sel_chk();
int kosuu_select();
int yes_no_select();
int Ud_u_item_stack2();
int Chk_lb_status();
void Disp_menu_help();
void PutButtonICON();
void ItemListWindow();
void ItemboxWindow();
void EquipmentCompareWindow();
void EquipmentDescriptionWindow();
void ItemboxWindowCursor();
extern char verify_button_0038A008[8];
extern char frame_itembox_item_equip[];
extern u16 System_timer;
f32 flSin(f32);
u8 *sortup_idx_chk();
void se_req();
s32 Lb_ItemBox_open() {
    F(s32, ib, 4) = 0;
    F(s16, ib, 2) = 0;
    F(s8, ib, 0xB) = F(u16, ib, 8) = 0;
    F(s8, ib, 0x1F) = 0;
    F(u8, ib, 0x20) = 0xFF;
    *(s8 *)0x39DAD1 = 5;
    *(s8 *)0x39DAD0 = 0;
    *(s16 *)0x39DAD2 = 0;
    se_req(7, 0x11, 0, 0xFF);
    return 1;
}
void ListSelect();
int itembox_stock();
int itembox_pickup();
int itembox_equipchange();
int itembox_sortup();
int itembox_sellout();
s32 Lb_ItemBox_mv(int arg0) {
    s32 s0;
    u8 *a;
    s0 = arg0 & 0xFFFF;
    F(s8, ib, 0) = 0;
    a = ib;
    switch (a[4]) {
    case 0:
        *(s8 *)0x39DAD0 = 0;
        if (!(s0 & 0x40)) {
            ListSelect(a + 2, arg0, 5);
            if (s0 & 0x20) {
                arg0 = 0;
                F(s8, ib, 3) = 0;
                F(s16, ib, 8) = F(u8, ib, 0xB) = 0;
                F(s8, ib, 0x1F) = 0;
                F(s8, ib, 0x1D) = -1;
                F(u8, ib, 0x1E) = 0xFF;
                F(u8, ib, 4) = F(u8, ib, 4) + 1;
                F(s8, ib, 5) = 0;
                se_req(7, 0x13, 0, -1);
    case 1:
                a = ib;
                *(u8 *)0x39DAD0 = 1;
                switch (a[2]) {
                case 0:
                    s0 = itembox_stock(arg0) & 0xFFFF;
                    break;
                case 1:
                    s0 = itembox_pickup(arg0) & 0xFFFF;
                    break;
                case 2:
                    s0 = itembox_equipchange(arg0) & 0xFFFF;
                    break;
                case 3:
                    s0 = itembox_sortup(arg0) & 0xFFFF;
                    break;
                case 4:
                    s0 = itembox_sellout(arg0) & 0xFFFF;
                    break;
                }
                if ((u16)s0 & 0x40) {
                    s0 = (u16)(s0 & 0xFFBF);
                    *(u8 *)0x39DAD0 = 0;
                    F(u8, ib, 4) = 0;
                    se_req(7, 0x14, 0);
                }
            }
        }
        break;
    }
    if ((u16)s0 & 0x40) {
        se_req(7, 0x14, 0);
        return 0;
    }
    F(s8, ib, 0) = 1;
    return 1;
}

extern char frame_itembox_cmd[];
extern u32 D_3C733C[];
extern char *yes_or_no[2];
extern char lit_2148[];
extern char lit_2149[];
extern char lit_2150[];
int sprintf(char *, const char *, ...);
int DispFrameList();
int Disp_help_mess();
int Put_shousai();
int font_print_sp();
void flfntSetSize();
void flfntLocate();
void font_set_palette();
void font_print_uf();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void PutArrow();

int disp_itembox_cmd(int a) {
    if (!(a & 0xFF)) {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0xA9182;
    } else {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0x808080;
    }
    return DispFrameList(frame_itembox_cmd, 0, F(u8, ib, 2));
}

void item_explanation(a, b, c)
int a;
int b;
int c;
{
    if ((a & 0xFF) == 1) {
        Disp_help_mess(1, (u16)(*(u16 *)&D_3C733C[b & 0xFF] + 0x18));
    } else {
        Put_shousai();
    }
}

int yes_no_disp_sub(void) {
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x1B0, 0x18E);
    return font_print_sp(yes_or_no[F(u8, ib, 0x21)]);
}

/* amount selector: two-digit number with up/down arrows */
void kosuu_disp_sub(void) {
    s8 buf[8];
    u8 *w;
    if (F(s16, ib, 0x1A) != 0x270F) {
        flfntSetSize(0x12, 0x12);
        font_set_palette(0);
        flfntLocate(0x22E, 0x17A);
        w = ib;
        buf[0] = 0x82;
        buf[1] = F(s16, w, 0x1A) / 10 + 0x4F;
        buf[2] = 0x82;
        buf[3] = F(s16, w, 0x1A) % 10 + 0x4F;
        buf[4] = 0;
        font_print_uf(buf, 0xA, w, -0x7E);
        SetFilterMode(1);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        if (F(u8, ib, 0x1C) == 0) {
            PutArrow(0x234, 0x168, 0x18, 0x12, 0xFF20FF28, 2);
        }
        if (F(s16, ib, 0x1A) != 1) {
            PutArrow(0x234, 0x18E, 0x18, 0x12, 0xFF20FF28, 3);
        }
    }
}

/* selling price (value + two-byte full-width digits) and the yes/no label */
void selling_price_disp_sub(void) {
    u16 wide[16];
    s8 txt[16];
    u16 *d;
    s8 *s;
    int c;
    int price;
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x132, 0x166);
    price = F(s32, ib, 0xC);
    if (price != 0) {
        sprintf((char *)txt, lit_2148, price);
        c = txt[0];
        d = wide;
        s = txt;
        if (c != 0) {
            do {
                s += 1;
                *d = ((c + 0x1F) << 8) | 0x82;
                c = *s;
                d += 1;
            } while (c != 0);
        }
        d[0] = 0x9A82;
        d[1] = 0;
        font_set_palette(5);
        font_print_sp(lit_2149, wide);
    } else {
        font_set_palette(0);
        font_print_uf(lit_2150);
    }
    flfntLocate(0x1B0, 0x18E);
    font_print_sp(yes_or_no[F(u8, ib, 0x21)]);
}

/* edit a quantity byte with the d-pad: mode 0 = two decimal digits, otherwise two octal digits (hi << 3 | lo) */
void itembox_cursor_mv(u8 *val, int pad, int mode) {
    int v;
    int hi;
    int p;
    int lo;
    u8 nv;
    v = *val;
    if (!(mode & 0xFF)) {
        v = v & 0xFF;
        p = pad & 0xFFFF;
        lo = v % 10 & 0xFF;
        hi = v / 10 & 0xFF;
        if (p & 0x800) {
            if (!(lo & 0xFF)) {
                lo = (u8)9;
            } else {
                lo = (lo - 1) & 0xFF;
            }
        }
        if (p & 0x400) {
            if ((lo & 0xFF) >= 9) {
                lo = 0;
            } else {
                lo = (lo + 1) & 0xFF;
            }
        }
        if (p & 0x2000) {
            if (!(hi & 0xFF)) {
                hi = (u8)9;
            } else {
                hi = (hi - 1) & 0xFF;
            }
        }
        if (p & 0x1000) {
            if ((hi & 0xFF) >= 9) {
                hi = 0;
            } else {
                hi = (hi + 1) & 0xFF;
            }
        }
        nv = (lo & 0xFF) % 10 + (hi & 0xFF) % 10 * 10;
    } else {
        v = lo = v & 0xFF;
        p = pad & 0xFFFF;
        hi = (lo >> 3) & 0xFF;
        if (p & 0x800) {
            lo = (lo - 1) & 0xFF;
        }
        if (p & 0x400) {
            lo = (lo + 1) & 0xFF;
        }
        if (p & 0x2000) {
            hi = (hi - 1) & 0xFF;
        }
        if (p & 0x1000) {
            hi = (hi + 1) & 0xFF;
        }
        nv = ((u8)lo & 7) | (((u8)hi & 7) << 3);
    }
    if (v != nv) {
        *val = nv;
        se_req(7, 0x16, 0);
    }
}

/* item box "stock" tab: take the item under the cursor into the pouch */
s32 itembox_stock(s32 pad) {
    u16 left;
    u8 *w;
    u8 *u;
    u8 *v;
    int k;
    u = User_data;
    w = ib;
    switch (F(u8, w, 5)) {
    case 0:
        *(s16 *)0x39DAD2 = 0;
        if (F(u8, w, 0x1F) != 0) {
            if ((u16)pad & 0x240) {
                F(u8, w, 0x1F) = 0;
                se_req(7, 0x14, 0);
            }
            pad = (u16)(pad & 0xFFBF);
        } else if ((u16)pad & 0x200) {
            F(u8, w, 0x1F) = 1;
            se_req(7, 9, 0);
        }
        Menu_select_mv(ib + 0xB, pad, 0x14);
        if ((u16)pad & 0x20) {
            k = F(u8, ib, 0xB) * 4;
            if (*(u16 *)(k + (int)u + 0x37C) != 0) {
                left = Ud_u_item_stack(*(u16 *)(k + (int)u + 0x37C), *(u16 *)(k + (int)u + 0x37E)) & 0xFFFF;
                if (left == 0) {
                    k = F(u8, ib, 0xB) * 4;
                    *(s16 *)(k + (int)u + 0x37E) = 0;
                    k = F(u8, ib, 0xB) * 4;
                    *(u16 *)(k + (int)u + 0x37C) = 0;
                    F(s8, ib, 0x1D) = 1;
                    se_req(7, 0x2C, 0, left);
                } else {
                    k = F(u8, ib, 0xB) * 4;
                    *(s16 *)(k + (int)u + 0x37E) = left;
                    F(s8, ib, 0x1D) = 2;
                    se_req(7, 0x15, 0, left);
                }
                v = ib;
                *(s16 *)0x39DAD2 = F(s8, v, 0x1D);
                F(u8, v, 0x1F) = 0;
                F(u8, ib, 5) = F(u8, ib, 5) + 1;
            } else {
                se_req(7, 0x15, 0);
            }
        }
        break;
    case 1:
        *(s16 *)0x39DAD2 = F(s8, w, 0x1D);
        F(u8, w, 0x20) = F(u8, w, 0x20) + 1;
        if ((u16)pad & 0x20) {
            v = ib;
            *(s16 *)0x39DAD2 = 0;
            F(u8, v, 5) = 0;
            F(u8, ib, 0x1F) = 0;
            F(u8, ib, 0x20) = 0xFF;
            se_req(7, 9, 0);
        }
        pad = 0;
        break;
    }
    return pad;
}

/* number / page selection sub-state shared by the item box tabs (returns the pad with consumed bits cleared, 0x40 = cancel) */
s32 ib_select_sub(s32 pad) {
    s32 p;
    u8 *w;
    w = ib;
    if (F(u8, w, 3) == 0) {
        if (F(u8, w, 0x1F) != 0) {
            if ((u16)pad & 0x240) {
                F(u8, w, 0x1F) = 0;
                se_req(7, 0x14, 0);
            }
            pad = (u16)(pad & 0xFFBF);
        } else {
            p = (u16)pad;
            if (p & 0x200) {
                F(u8, w, 0x1F) = 1;
                se_req(7, 9, 0);
            }
            if (p & 0x40) {
                se_req(7, 0x14, 0);
                return 0x40;
            }
        }
        itembox_cursor_mv(ib + 8, pad, 0);
        goto done;
    }
    if (F(u8, w, 0x1F) != 0) {
        if ((u16)pad & 0x240) {
            F(u8, w, 0x1F) = 0;
            se_req(7, 0x14, 0);
        } else {
            PageSelect(w + 0x18, pad, F(u8, w, 0x19));
        }
        return 0;
    }
    p = (u16)pad;
    if (p & 0x40) {
        se_req(7, 0x14, 0);
        return 0x40;
    }
    itembox_cursor_mv(w + 9, pad, 1);
    if (!(p & 0x20) && (p & 0x200)) {
        F(u8 *, ib, 0x14) = User_data + F(u8, ib, 9) * 6 + 0x44;
        if (*F(u8 *, ib, 0x14) != 0) {
            F(u8, ib, 0x1F) = 2;
            F(s8, ib, 0x18) = 0;
            F(u8, ib, 0x19) = 4;
            se_req(7, 0x11, 0);
        } else {
            se_req(7, 0x15, 0);
        }
        return 0;
    }
done:
    return pad;
}

typedef struct IBSPR { s16 x, y, w, h; s32 color; s32 z; s32 size; } IBSPR;

/* quantity / slot cursor frame: mode 0 = decimal grid (10 per row), else 8 per row */
void ItemboxWindowCursorX(f32 base, int idx, int color, int mode) {
    IBSPR r;
    int i;
    SetFilterMode(0);
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    if (!(mode & 0xFF)) {
        i = (s16)idx;
        r.x = 0.8f * (153.0f + base - 146.0f + 28.8f * (f32)(i % 10));
        r.y = i / 10 * 0x19 + 0x3B;
        r.w = 0x19;
        r.h = 0x19;
    } else {
        i = (s16)idx;
        r.x = 0.8f * (153.0f + base - 146.0f + 36.0f * (f32)(i & 7));
        r.y = (i >> 3 << 5) + 0x3B;
        r.w = 0x20;
        r.h = 0x20;
    }
    r.size = 0x200020;
    r.color = color;
    r.z = 0;
    flps0008(&r);
}

typedef struct SW4 { s16 a, b; } SW4;            /* pouch item (id, amount) */
typedef struct SW6 { s16 a, b, c; } SW6;         /* equipment slot */

/* item box "sort up" tab: pick an entry, then pick a second one and swap the two */
s32 itembox_sortup(s32 pad) {
    SW4 tmp4;
    SW6 tmp6;
    u8 *w;
    u8 *i2;
    u8 *u;
    u8 *i1;
    u8 *p3;
    u8 *p10;
    u8 first;
    u8 second;
    u8 col;
    w = ib;
    *(s16 *)D_39DAD2 = F(u8, w, 3) + 0x14;
    p3 = w + 3;
    switch (F(u8, w, 5)) {
    case 0:
        ListSelect(p3, pad, 2);
        if ((u16)pad & 0x20) {
            F(s8, ib, 0x1F) = 0;
            F(u8, ib, 5) = F(u8, ib, 5) + 1;
            F(u8, ib, 6) = 0;
            se_req(7, 0x13, 0);
        } else {
            *(s8 *)D_39DAD0 = 0;
        }
        break;
    case 1:
        switch (F(u8, w, 6)) {
        case 0:
            pad = ib_select_sub(pad) & 0xFFFF;
            if (pad & 0x40) {
                pad = (u16)(pad & 0xFFBF);
                *(u8 *)D_39DAD0 = 0;
                F(u8, ib, 5) = 0;
            } else if (pad & 0x20) {
                F(u8, ib, 0xA) = F(u8, F(u8, ib, 3) + (int)ib, 8);
                F(u8, ib, 6) = F(u8, ib, 6) + 1;
                se_req(7, 0x25, 0);
            }
            break;
        case 1:
            pad = ib_select_sub(pad) & 0xFFFF;
            if (pad & 0x40) {
                pad = (u16)(pad & 0xFFBF);
                F(u8, ib, 6) = 0;
            } else if (pad & 0x20) {
                second = F(u8, ib, 0xA);
                first = *(u8 *)((int)(ib + 8) + F(u8, ib, 3));
                col = F(u8, ib, 3);
                if (first != F(u8, ib, 0xA)) {
                    u = User_data;
                    if (col == 0) {
                        tmp4 = *(SW4 *)((first << 2) + (int)u + 0x1C4);
                        *(SW4 *)((first << 2) + (int)u + 0x1C4) = *(SW4 *)((second << 2) + (int)u + 0x1C4);
                        *(SW4 *)((F(u8, ib, 0xA) << 2) + (int)u + 0x1C4) = tmp4;
                    } else {
                        i1 = sortup_idx_chk(first, second);
                        i2 = sortup_idx_chk(F(u8, ib, 0xA));
                        w = ib;
                        p10 = w + 0xA;
                        tmp6 = ((SW6 *)(u + 0x44))[F(u8, F(u8, w, 3) + (int)w, 8)];
                        ((SW6 *)(u + 0x44))[F(u8, F(u8, w, 3) + (int)w, 8)] = ((SW6 *)(u + 0x44))[F(u8, w, 0xA)];
                        ((SW6 *)(u + 0x44))[F(u8, w, 0xA)] = tmp6;
                        if (i1 != 0) {
                            *i1 = *p10;
                        }
                        if (i2 != 0) {
                            *i2 = F(u8, F(u8, ib, 3) + (int)ib, 8);
                        }
                    }
                    F(u8, ib, 6) = 0;
                    se_req(7, 0x26, 0);
                } else {
                    se_req(7, 0x15, 0);
                }
            }
            break;
        }
        break;
    }
    return pad;
}

/* item box screen: tab frames, item lists, help text and the blinking cursor */
void Disp_lb_item_box(void) {
    f32 fa;
    u32 ang;
    f32 new_var;
    int pos;
    u8 *w;
    u8 a;
    if (F(u8, ib, 0) != 0 && Chk_lb_status(0x1C) != 0) {
        if (*(u8 *)0x39DAD0 != 0) {
            a = F(u8, ib, 0x1F);
            if (a != 1) {
                Disp_menu_help(a);
                if ((F(u8, ib, 0x20) & 0x1F) < 0x14) {
                    PutButtonICON(verify_button_0038A008, 1);
                }
            }
        }
        switch (F(u8, ib, 4)) {
        case 0:
            disp_itembox_cmd(0);
            return;
        case 1:
            disp_itembox_cmd(1);
            w = ib;
            pos = 4;
            switch (F(u8, w, 2)) {
            case 0:
                switch (F(u8, w, 5)) {
                case 0:
                    ItemListWindow(F(u8, w, 0xB), 0xA9182, 8, 3);
                    item_explanation(F(u8, ib, 0x1F), F(u8, ib, 0xB));
                    return;
                case 1:
                    ItemboxWindow(-1, 0, 2, 3);
                    return;
                }
                break;
            case 1:
                switch (F(u8, w, 5)) {
                case 0:
                    if (F(u8, w, 0x1F) != 0) {
                    } else {
                        pos = 0;
                        Put_shousai(F(u8, w, 5), 1, 2, 3);
                    }
                    ItemboxWindow(F(u8, ib, 8), pos);
                    return;
                case 1:
                    ItemboxWindow(F(u8, w, 8), 0x10, 2, 3);
                    ItemListWindow(F(u8, ib, 0xB), 0xA9182, 8);
                    item_explanation(F(u8, ib, 0x1F), F(u8, ib, 0xB));
                    return;
                case 2:
                    kosuu_disp_sub();
                    ItemboxWindow(F(u8, ib, 8), 0x10);
                    ItemListWindow(F(u8, ib, 0xB), 0x808080, 0);
                    return;
                case 3:
                    yes_no_disp_sub();
                case 4:
                    ItemboxWindow(F(u8, ib, 8), 0x10);
                    ItemListWindow(F(u8, ib, 0xB), 0x808080, 0);
                    return;
                }
                break;
            case 2:
                a = F(u8, w, 5);
                switch (a) {
                case 0:
                    if (F(u8, w, 0x1F) != 0) {
                        pos = (F(u8, w, 0x18) & 3) | 0x21C;
                    } else {
                        pos = 0x208;
                        if (F(s8, w, 0x1D) < 0) {
                            Put_shousai(1, a, 2, 3);
                        }
                    }
                    ItemboxWindow(F(u8, ib, 9), pos);
                    return;
                case 1:
                    EquipmentCompareWindow(F(s32, w, 0x10), F(s32, w, 0x14), 0x132, 0x38);
                    return;
                case 2:
                    EquipmentDescriptionWindow(F(s32, w, 0x14), 0x132, 0x38, F(u8, w, 0x18));
                    return;
                }
                break;
            case 3:
                a = F(u8, w, 5);
                switch (a) {
                case 0:
                    *(s16 *)(frame_itembox_item_equip + 2) = 0x74;
                    DispFrameList(frame_itembox_item_equip, 0, F(u8, w, 3), 3);
                    return;
                case 1:
                    if (F(u8, w, 0x1F) != 0) {
                        if (F(u8, w, 3) == 0) {
                        } else {
                            pos = (F(u8, w, 0x18) & 3) | 0x1C;
                        }
                    } else {
                        pos = (F(u8, w, 3) & 1) * 8;
                        Put_shousai(1, a, 2, 3);
                    }
                    w = ib;
                    ItemboxWindow(F(u8, w + F(u8, w, 3), 8), pos);
                    w = ib;
                    if (F(u8, w, 0x1F) != 0) {
                        if (F(u8, w, 3) == 0) {
                            goto cursor;
                        }
                    } else {
cursor:
                        a = F(u8, w, 6);
                        if (a == 1) {
                            ang = ((System_timer & 0x1F) << 11) & 0xFFFF;
                            fa = (f32)ang;
                            new_var = flSin(0.0000958738f * fa);
                            w = ib;
                            ItemboxWindowCursor(F(u8, w, 0xA), (((((s8)(int)(96.0f * new_var)) + 0x9F) & 0xFF) << 24) | 0xFF0000, F(u8, w, 3));
                            return;
                        }
                    }
                    break;
                }
                break;
            case 4:
                a = F(u8, w, 5);
                switch (a) {
                case 0:
                    *(s16 *)(frame_itembox_item_equip + 2) = 0x8A;
                    DispFrameList(frame_itembox_item_equip, 0, F(u8, w, 3), 1);
                    return;
                case 1: {
                    u8 col = F(u8, w, 3);
                    u8 st = F(u8, w, 6);
                    pos = col != 0 ? 0xE8 : 0x60;
                    switch (st) {
                    case 0:
                        if (F(u8, w, 0x1F) != 0) {
                            pos = 0x64;
                            if (col == 0) {
                            } else {
                                pos = (F(u8, w, 0x18) & 3) | 0xFC;
                            }
                        } else if (F(s8, w, 0x1D) < 0) {
                            Put_shousai(st, 0xE8, col, 1);
                        }
                        break;
                    case 1:
                        kosuu_disp_sub();
                        break;
                    case 2:
                        selling_price_disp_sub();
                        break;
                    }
                    w = ib;
                    ItemboxWindow(F(u8, w + F(u8, w, 3), 8), pos);
                    break;
                }
                }
                break;
            }
            break;
        }
    }
}

/* item box "pickup" tab: move an item from the pouch into the item box (stacking onto an existing slot when possible) */
s32 itembox_pickup(s32 pad) {
    SW4 tmp4;
    u8 *u;
    u8 *w;
    u8 *p5;
    int sel;
    int cur;
    int id16;
    int mx;
    int e;
    int tmp;
    int k;
    int j;
    u = User_data;
    w = ib;
    p5 = w + 5;
    switch (*p5) {
    case 0:
        *(s16 *)D_39DAD2 = 3;
        if (F(u8, w, 0x1F) != 0) {
            if ((u16)pad & 0x240) {
                F(u8, w, 0x1F) = 0;
                se_req(7, 0x14, 0);
            }
            pad = (u16)(pad & 0xFFBF);
        } else if ((u16)pad & 0x200) {
            F(u8, w, 0x1F) = 1;
            se_req(7, 9, 0);
        }
        itembox_cursor_mv(ib + 8, pad, 0);
        if ((u16)pad & 0x20) {
            if (u_item_chk(F(u8, ib, 8)) == 0) {
                se_req(7, 0x15, 0);
            } else {
                F(u8, ib, 0x1F) = 0;
                F(u8, ib, 0xB) = 0;
                while (F(u8, ib, 0xB) < 0x14) {
                    if (*(u16 *)(u + (F(u8, ib, 0xB) & 0xFF) * 4 + 0x37C) == *(u16 *)(u + F(u8, ib, 8) * 4 + 0x1C4)) {
                        break;
                    }
                    F(u8, ib, 0xB) += 1;
                }
                if (F(u8, ib, 0xB) < 0x14) {
                    cur = F(u8, ib, 0xB);
                    id16 = *(u16 *)(cur * 4 + (int)u + 0x37C) * 0x10;
                    mx = D_3396D3[id16];
                    if (mx != 0xFF && *(s16 *)(F(u8, ib, 0xB) * 4 + (int)u + 0x37E) < mx) {
                        if (pick_kosuu_sel_chk() == 0) {
                            k = F(u8, ib, 0xB) * 4;
                            *(s16 *)(k + (int)u + 0x37E) += 1;
                            k = F(u8, ib, 8) * 4;
                            *(s16 *)(k + (int)u + 0x1C6) -= 1;
                            k = F(u8, ib, 8) * 4;
                            if (*(s16 *)(k + (int)u + 0x1C6) == 0) {
                                k = F(u8, ib, 8) * 4;
                                *(u16 *)(k + (int)u + 0x1C4) = 0;
                            }
                            F(s8, ib, 0x1D) = 7;
                            *(s16 *)D_39DAD2 = 7;
                            F(u8, ib, 5) = 4;
                            se_req(7, 0x2C, 0);
                        } else {
                            *(s16 *)D_39DAD2 = 5;
                            F(s16, ib, 0x1A) = 1;
                            F(s8, ib, 0x1C) = 0;
                            F(u8, ib, 5) = 2;
                            F(u8, ib, 6) = 0;
                            F(s8, ib, 0x1D) = 3;
                            se_req(7, 0x13, 0);
                        }
                    } else {
                        F(s8, ib, 0x1D) = 6;
                        *(s16 *)D_39DAD2 = 6;
                        F(u8, ib, 5) = 4;
                        se_req(7, 0x15, 0);
                    }
                } else {
                    F(u8, ib, 0xB) = 0;
                    pad = 0;
                    for (e = 0; e < 0x14; e++) {
                        if (*(u16 *)(u + e * 4 + 0x37C) == 0) {
                            F(u8, ib, 0xB) = e;
                            break;
                        }
                    }
                    F(u8, ib, 5) = 1;
                    se_req(7, 0x13, 0);
                }
            }
        }
        break;
    case 1:
case1:
        w = ib;
        if (F(u8, w, 0x1F) != 0) {
            if ((u16)pad & 0x240) {
                F(u8, w, 0x1F) = 0;
                se_req(7, 0x14, 0);
            }
            pad = (u16)(pad & 0xFFBF);
            goto menu;
        }
        tmp = (u16)pad;
        if (tmp & 0x40) {
            *(s16 *)D_39DAD2 = 3;
            F(u8, w, 5) = 0;
            pad = 0;
            se_req(7, 0x14, 0);
        } else {
            if (tmp & 0x200) {
                F(u8, w, 0x1F) = 1;
                se_req(7, 9, 0);
            }
menu:
            *(s16 *)D_39DAD2 = 4;
            Menu_select_mv(ib + 0xB, pad, 0x14);
            if ((u16)pad & 0x20) {
                F(u8, ib, 0x1F) = 0;
                w = ib;
                k = F(u8, w, 0xB) * 4;
                if (*(u16 *)(k + (int)u + 0x37C) != 0) {
                    *(s16 *)D_39DAD2 = 8;
                    F(u8, w, 5) = 3;
                    F(u8, ib, 0x21) = 0;
                    se_req(7, 0x13, 0);
                } else if (item_kosuu_sel_chk() == 0) {
                    j = F(u8, ib, 8);
                    k = F(u8, ib, 0xB);
                    *(u16 *)((k * 4) + (int)u + 0x37C) = *(u16 *)((j * 4) + (int)u + 0x1C4);
                    j = F(u8, ib, 8);
                    k = F(u8, ib, 0xB);
                    *(s16 *)((k * 4) + (int)u + 0x37E) = *(s16 *)((j * 4) + (int)u + 0x1C6);
                    k = F(u8, ib, 8) * 4;
                    *(s16 *)(k + (int)u + 0x1C6) = 0;
                    k = F(u8, ib, 8) * 4;
                    *(u16 *)(k + (int)u + 0x1C4) = 0;
                    F(s8, ib, 0x1D) = 7;
                    *(s16 *)D_39DAD2 = 7;
                    F(u8, ib, 5) = 4;
                    se_req(7, 0x2C, 0);
                } else {
                    pad = 0;
                    F(s16, ib, 0x1A) = 1;
                    F(s8, ib, 0x1C) = 0;
                    F(u8, ib, 5) = 2;
                    se_req(7, 0x13, 0);
                    F(u8, ib, 6) = 1;
                    F(s8, ib, 0x1D) = 4;
    case 2:
                    sel = (u16)pad;
                    if (sel & 0x40) {
                        w = ib;
                        pad = (u16)(pad & 0xFFBF);
                        *(s16 *)D_39DAD2 = F(s8, w, 0x1D);
                        F(u8, w, 5) = F(u8, w, 6);
                        se_req(7, 0x14, 0);
                    } else {
                        *(s16 *)D_39DAD2 = 5;
                        kosuu_select(pad, F(u8, ib, 6));
                        if (sel & 0x20) {
                            j = F(u8, ib, 8);
                            k = F(u8, ib, 0xB);
                            *(u16 *)((k * 4) + (int)u + 0x37C) = *(u16 *)((j * 4) + (int)u + 0x1C4);
                            k = F(u8, ib, 0xB) * 4;
                            *(s16 *)(k + (int)u + 0x37E) = *(s16 *)(k + (int)u + 0x37E) + F(s16, ib, 0x1A);
                            k = F(u8, ib, 8) * 4;
                            *(s16 *)(k + (int)u + 0x1C6) = *(s16 *)(k + (int)u + 0x1C6) - F(s16, ib, 0x1A);
                            k = F(u8, ib, 8) * 4;
                            if (*(s16 *)(k + (int)u + 0x1C6) == 0) {
                                k = F(u8, ib, 8) * 4;
                                *(u16 *)(k + (int)u + 0x1C4) = 0;
                            }
                            F(s8, ib, 0x1D) = 7;
                            *(s16 *)D_39DAD2 = 7;
                            F(u8, ib, 5) = 4;
                            se_req(7, 0x2C, 0);
                        }
                    }
                }
            }
        }
        break;
    case 3:
        sel = (u16)pad;
        if (sel & 0x40) {
            *(s16 *)D_39DAD2 = 4;
            *p5 = 1;
            pad = (u16)(pad & 0xFFBF);
            se_req(7, 0x14, 0);
        } else {
            *(s16 *)D_39DAD2 = 8;
            yes_no_select(3);
            if (sel & 0x20) {
                w = ib;
                pad = 0;
                if (F(u8, w, 0x21) != 0) {
                    *(s16 *)D_39DAD2 = 4;
                    F(u8, w, 5) = 1;
                    se_req(7, 0x14, 0);
                } else {
                    k = F(u8, w, 0xB) * 4;
                    tmp4 = *(SW4 *)(k + (int)u + 0x37C);
                    j = F(u8, w, 8);
                    k = F(u8, w, 0xB);
                    *(u16 *)((k * 4) + (int)u + 0x37C) = *(u16 *)((j * 4) + (int)u + 0x1C4);
                    *(s16 *)((k * 4) + (int)u + 0x37E) = *(s16 *)((j * 4) + (int)u + 0x1C6);
                    k = F(u8, ib, 8) * 4;
                    *(u16 *)(k + (int)u + 0x1C4) = 0;
                    k = F(u8, ib, 8) * 4;
                    *(s16 *)(k + (int)u + 0x1C6) = 0;
                    Ud_u_item_stack(tmp4.a, tmp4.b);
                    F(s8, ib, 0x1D) = 7;
                    F(u8, ib, 5) = 4;
                    se_req(7, 0x2C, 0);
    case 4:
                    w = ib;
                    *(s16 *)D_39DAD2 = F(s8, w, 0x1D);
                    F(u8, w, 0x20) = F(u8, w, 0x20) + 1;
                    if ((u16)pad & 0x20) {
                        F(u8, ib, 5) = 0;
                        F(u8, ib, 0x20) = 0xFF;
                        se_req(7, 9, 0);
                    }
                    pad = 0;
                }
            }
        }
        break;
    }
    return pad;
}

/* item box "equipment change" tab: move equipment between the equipment box (User_data + 0x44, 6 byte slots) and the character */
s32 itembox_equipchange(s32 pad) {
    u8 *u;
    int ok;
    u8 *w;
    u8 *e1;
    u8 *e2;
    u8 st;
    u8 kind;
    u8 eq;
    int r;
    int mask;
    u8 *p5;
    u = User_data;
    w = ib;
    p5 = w + 5;
    switch (*p5) {
    case 0:
        if (F(s8, w, 0x1D) >= 0) {
            if ((u16)pad & 0x20) {
                F(s8, w, 0x1D) = -1;
                pad = 0;
                F(u8, ib, 0x20) = 0xFF;
                se_req(7, 9, 0);
                goto select;
            }
            F(u8, w, 0x20) = F(u8, w, 0x20) + 1;
            *(s16 *)0x39DAD2 = F(s8, ib, 0x1D);
            return 0;
        }
select:
        *(s16 *)0x39DAD2 = 0xA;
        if (F(u8, ib, 0x1F) != 0) {
            if ((u16)pad & 0x240) {
                F(u8, ib, 0x1F) = 0;
                se_req(7, 0x14, 0);
            } else {
                PageSelect(ib + 0x18, pad, F(u8, ib, 0x19));
            }
            pad = 0;
        }
        itembox_cursor_mv(ib + 9, pad, 1);
        r = (u16)pad;
        if (r & 0x20) {
            w = ib;
            F(u8 *, w, 0x14) = u + F(u8, w, 9) * 6 + 0x44;
            w = ib;
            ok = 0;
            if (*F(u8 *, w, 0x14) != 0) {
                F(s8, w, 0x18) = 0;
                F(u8, ib, 0x19) = 2;
                F(u8, ib, 1) = 0;
                w = ib;
                kind = F(u8 *, w, 0x14)[1];
                ok = 1;
                switch (kind) {
                case 0:
                    eq = u[0x457];
                    break;
                case 2:
                    eq = u[0x458];
                    break;
                case 3:
                    eq = u[0x459];
                    break;
                case 4:
                    eq = u[0x45A];
                    break;
                case 5:
                    eq = u[0x45B];
                    break;
                case 6:
                case 7:
                    eq = u[0x456];
                    F(u8, w, 1) = 1;
                    break;
                }
                if ((eq & 0xFF) != 0xFF) {
                    F(u8 *, ib, 0x10) = u + (eq & 0xFF) * 6 + 0x44;
                } else {
                    F(u8 *, ib, 0x10) = 0;
                }
                w = ib;
                e2 = F(u8 *, w, 0x14);
                e1 = F(u8 *, w, 0x10);
                if (e1 == e2) {
                    if (F(u8, w, 1) != 0) {
                        ok = 0;
                        F(s8, w, 0x1D) = 0xC;
                        F(u8, ib, 0x1E) = F(u8, ib, 9);
                    }
                    F(u8 *, ib, 0x14) = 0;
                } else if (F(u8, w, 1) == 0) {
                    st = Get_equip_data_ptr(e2, w)[2];
                    if (!(st & ((u[1] == 0 ? 1 : 2) & 0xFF))) {
                        ok = 0;
                        F(s8, ib, 0x1D) = 0xE;
                        F(u8, ib, 0x1E) = F(u8, ib, 9);
                    } else {
                        if (u[u[0x456] * 6 + 0x45] == 6) {
                            mask = 4;
                        } else {
                            mask = 8;
                        }
                        if (!(st & (mask & 0xFF))) {
                            ok = 0;
                            F(s8, ib, 0x1D) = 0xD;
                            F(u8, ib, 0x1E) = F(u8, ib, 9);
                        }
                    }
                } else {
                    if (e1[1] != 7) {
                        if (e2[1] == 7) {
                            goto set4;
                        }
                    } else {
set4:
                        F(u8, w, 0x19) = 4;
                    }
                    w = ib;
                    if (F(u8 *, w, 0x10)[1] != F(u8 *, w, 0x14)[1]) {
                        F(u8, w, 1) = 2;
                    }
                }
            }
            if (ok == 1) {
                F(u8, ib, 5) = F(u8, ib, 5) + 1;
                se_req(7, 0x13, 0);
            } else {
                se_req(7, 0x15, 0);
            }
        } else if (r & 0x200) {
            w = ib;
            F(u8 *, w, 0x14) = u + F(u8, w, 9) * 6 + 0x44;
            w = ib;
            if (*F(u8 *, w, 0x14) != 0) {
                *(s16 *)0x39DAD2 = 0xA;
                F(u8, w, 0x1F) = 2;
                F(s8, ib, 0x18) = 0;
                F(u8, ib, 0x19) = 4;
                se_req(7, 0x11, 0);
            } else {
                se_req(7, 0x15, 0);
            }
        }
    default:
        break;
    case 1:
        r = (u16)pad;
        if (r & 0x40) {
            *(s16 *)0x39DAD2 = 0xA;
            *p5 = 0;
            pad = (u16)(pad & 0xFFBF);
            se_req(7, 0x14, 0);
        } else {
            if (F(u8 *, w, 0x14) == 0) {
                *(s16 *)0x39DAD2 = 0x10;
            } else if (F(u8 *, w, 0x10) == 0) {
                *(s16 *)0x39DAD2 = 0x11;
            } else if (F(u8, w, 1) != 2) {
                *(s16 *)0x39DAD2 = 0xF;
            } else {
                *(s16 *)0x39DAD2 = 0xB;
            }
            w = ib;
            PageSelect(w + 0x18, pad, F(u8, w, 0x19));
            if (r & 0x20) {
                w = ib;
                if (F(u8 *, w, 0x14) == 0) {
                    r = Warehouse_equip_out(u, F(u8 *, w, 0x10)[1]);
                } else {
                    r = Warehouse_equip(u, F(u8, w, 9));
                }
                if (r == 1) {
                    *(s16 *)0x39DAD2 = 0x12;
                    F(s8, ib, 0x18) = 0;
                    F(u8, ib, 5) = F(u8, ib, 5) + 1;
                    F(u8, ib, 6) = 0;
                    se_req(7, 0x2D, 0);
                    se_req(7, 0x13, 0);
                } else {
                    se_req(7, 0x15, 0);
                }
            }
        }
        break;
    case 2:
        *(s16 *)0x39DAD2 = 0x12;
        switch (F(u8, w, 6)) {
        case 0:
            if (!(F(u8, w, 1) & 1)) {
                F(u8, w, 6) = F(u8, w, 6) + 1;
            } else {
                Lb_equip_set((u8 *)player_work + game_w.master * 0xA00, u, w + 6);
                F(u8, ib, 6) = 2;
            }
            break;
        case 1:
            armor_set_myArmor(2, w, w + 6);
            F(u8, ib, 6) = F(u8, ib, 6) + 1;
            break;
        case 2:
            *(s16 *)0x39DAD2 = 0x13;
            F(u8, w, 0x20) = F(u8, w, 0x20) + 1;
            w = ib;
            if (F(u8 *, w, 0x14) != 0) {
                PageSelect(w + 0x18, pad, F(u8, w, 0x19));
            }
            if ((u16)pad & 0x20) {
                F(u8, ib, 0x20) = 0xFF;
                *(s16 *)0x39DAD2 = 0xA;
                F(u8, ib, 5) = 0;
                se_req(7, 9, 0, 0xFF);
            }
            break;
        }
        pad = (u16)(pad & 0xFFBF);
        break;
    }
    return pad;
}

/* item box "sell" tab: pick an item or equipment, choose the amount, confirm and add the gold */
s32 itembox_sellout(s32 pad) {
    u8 *w;
    u8 *u;
    u8 *p3;
    int k;
    u8 *s;
    int ok;
    int r;
    int sel;
    int mx;
    u = User_data;
    w = ib;
    switch (F(u8, w, 5)) {
    case 0:
        ListSelect(w + 3, pad, 2);
        if ((u16)pad & 0x20) {
            w = ib;
            *(s16 *)D_39DAD2 = F(u8, w, 3) + 0x16;
            F(s8, w, 0x1D) = -1;
            F(u8, ib, 5) = F(u8, ib, 5) + 1;
            F(u8, ib, 6) = 0;
            se_req(7, 0x13, 0);
        } else {
            *(s8 *)D_39DAD0 = 0;
        }
    default:
        break;
    case 1:
        switch (F(u8, w, 6)) {
        case 0:
            if (F(s8, w, 0x1D) >= 0) {
                if ((u16)pad & 0x20) {
                    F(s8, w, 0x1D) = -1;
                    pad = 0;
                    F(u8, ib, 0x20) = 0xFF;
                    se_req(7, 9, 0);
                    goto sel;
                }
                F(u8, w, 0x20) = F(u8, w, 0x20) + 1;
                *(s16 *)D_39DAD2 = F(s8, ib, 0x1D);
                return 0;
            }
sel:
            pad = ib_select_sub(pad) & 0xFFFF;
            if (pad & 0x40) {
                *(u8 *)D_39DAD0 = 0;
                pad = 0;
                F(u8, ib, 5) = 0;
            } else {
                w = ib;
                p3 = w + 3;
                *(s16 *)D_39DAD2 = *p3 + 0x16;
                if (pad & 0x20) {
                    if (*p3 == 0) {
                        ok = u_item_chk(F(u8, w, 8));
                        if (ok == 1) {
                            if (item_kosuu_sel_chk() == 0) {
                                *(s16 *)D_39DAD2 = 0x19;
                                F(u8, ib, 6) = 2;
                                F(u8, ib, 0x21) = 0;
                                w = ib;
                                k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                                F(s32, w, 0xC) = *(u16 *)(D_3396DE + F(u16, k + (int)u, 0x1C4) * 0x10);
                            } else {
                                F(s16, ib, 0x1A) = 1;
                                *(s16 *)D_39DAD2 = 0x18;
                                F(u8, ib, 6) = 1;
                            }
                        }
                    } else {
                        r = (s8)u_equip_chk(F(u8, w, 9));
                        if (r > 0) {
                            *(s16 *)D_39DAD2 = 0x19;
                            F(u8, ib, 6) = 2;
                            F(u8, ib, 0x21) = 0;
                            w = ib;
                            k = F(u8, F(u8, w, 3) + (int)w, 8) * 6;
                            s = (u8 *)(k + (int)u);
                            F(s32, ib, 0xC) = Get_equip_kaitori(F(u8, s, 0x45), F(u16, s, 0x46));
                            ok = 1;
                        } else {
                            ok = 0;
                            if (r == 0) {
                                F(s8, ib, 0x1D) = 0x1A;
                            }
                        }
                    }
                    if (ok == 1) {
                        F(s8, ib, 0x1F) = 0;
                        F(s16, ib, 0x1A) = 1;
                        F(s8, ib, 0x1C) = 0;
                        se_req(7, 0x13, 0);
                    } else {
                        se_req(7, 0x15, 0);
                    }
                }
            }
            break;
        case 1:
            sel = (u16)pad;
            if (sel & 0x40) {
                *(s16 *)D_39DAD2 = 0x16;
                pad = (u16)(pad & 0xFFBF);
                F(u8, w, 6) = 0;
                se_req(7, 0x14, 0);
            } else {
                *(s16 *)D_39DAD2 = 0x18;
                kosuu_select(1);
                if (sel & 0x20) {
                    w = ib;
                    pad = 0;
                    k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                    mx = F(u16, k + (int)u, 0x1C4) * 0x10;
                    F(s32, w, 0xC) = F(s16, w, 0x1A) * *(u16 *)(D_3396DE + mx);
                    F(u8, ib, 6) = F(u8, ib, 6) + 1;
                    F(u8, ib, 0x21) = 0;
                    se_req(7, 0x13, 0);
    case 2:
                    sel = (u16)pad;
                    if (sel & 0x40) {
                        if (F(u8, ib, 3) == 0 && item_kosuu_sel_chk() == 1) {
                            *(s16 *)D_39DAD2 = 0x18;
                            F(u8, ib, 6) = 1;
                        } else {
                            w = ib;
                            *(s16 *)D_39DAD2 = F(u8, w, 3) + 0x16;
                            F(u8, w, 6) = 0;
                        }
                        pad = (u16)(pad & 0xFFBF);
                        se_req(7, 0x14, 0);
                    } else {
                        *(s16 *)D_39DAD2 = 0x19;
                        yes_no_select(pad);
                        if (sel & 0x20) {
                            w = ib;
                            if (F(u8, w, 0x21) != 0) {
                                if (F(u8, w, 3) == 0 && item_kosuu_sel_chk() == 1) {
                                    *(s16 *)D_39DAD2 = 0x18;
                                    F(u8, ib, 6) = 1;
                                } else {
                                    w = ib;
                                    *(s16 *)D_39DAD2 = F(u8, w, 3) + 0x16;
                                    F(u8, w, 6) = 0;
                                }
                                se_req(7, 0x14, 0);
                            } else {
                                if (F(u8, w, 3) == 0) {
                                    k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                                    s = (u8 *)(k + (int)u);
                                    if (*(D_3396D3 + F(u16, s, 0x1C4) * 0x10) == 0xFF) {
                                        F(u16, s, 0x1C4) = 0;
                                        w = ib;
                                        k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                                        F(s16, k + (int)u, 0x1C6) = 0;
                                    } else {
                                        F(s16, s, 0x1C6) = F(s16, s, 0x1C6) - F(s16, w, 0x1A);
                                        w = ib;
                                        k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                                        if (F(s16, k + (int)u, 0x1C6) == 0) {
                                            k = F(u8, F(u8, w, 3) + (int)w, 8) * 4;
                                            F(u16, k + (int)u, 0x1C4) = 0;
                                        }
                                    }
                                } else {
                                    k = F(u8, F(u8, w, 3) + (int)w, 8) * 6;
                                    F(s16, k + (int)u, 0x46) = 0;
                                    w = ib;
                                    k = F(u8, F(u8, w, 3) + (int)w, 8) * 6;
                                    F(s8, k + (int)u, 0x44) = 0;
                                }
                                Gold_add(F(s32, ib, 0xC));
                                w = ib;
                                *(s16 *)D_39DAD2 = F(u8, w, 3) + 0x16;
                                F(u8, w, 6) = 0;
                                se_req(7, 0x1A, 0);
                            }
                        }
                    }
                }
            }
            break;
        }
        break;
    }
    return pad;
}

typedef struct IBQUAD { s16 x, y, x2, y2; s32 color; } IBQUAD;   /* flps0004 filled rectangle */
typedef struct IBICON { s16 x, y, w, h; s32 color; s16 u0, v0, u1, v1; } IBICON;   /* flps0008 textured icon */

/* item box window: the slot grid (100 pouch slots or 64 equipment slots), the cursor and the name/price line.
   cur = cursor slot (negative: none), flags: 4 help, 8 equipment grid, 0x10 dimmed, 0x20 gold, 0x40 price, 0x80/0x100/0x200 equipment modes */
void ItemboxWindowX(int cur, int flags, f32 base) {
    IBQUAD q;
    IBICON ic;
    char text[0x40];
    int equip;
    int dim;
    int alpha;
    int i;
    f32 x0;
    s16 xx;
    u16 *p;
    u8 *d;
    u8 *e;
    int k;
    int rare;
    int price;
    int amount;
    int col;
    int mode80;
    int mode100;
    int mode200;
    int cat;
    u8 kd;
    s16 cs;
    equip = (flags & 8) != 0;
    dim = flags & 0x10;
    alpha = (dim != 0 ? 0x60 : 0xFF) & 0xFF;
    *(s16 *)lb_item_box_base = base;
    DispFrameMessageA(lb_item_box_base, 0, alpha);
    SetFilterMode(1);
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    x0 = 153.0f + base;
    if (!equip) {
        for (i = 0; i < 100; i = (s16)(i + 1)) {
            xx = 0.8f * ((x0 - 146.0f) + 28.8f * (f32)(i % 10)) + 3;
            q.x = xx;
            q.x2 = 20.48f + (f32)xx;
            q.y = i / 10 * 0x19 + 0x3D;
            q.y2 = q.y + 0x15;
            q.color = (alpha << 24) | 0x200000;
            flps0004(&q);
        }
        ic.w = 0x19;
        ic.h = 0x19;
        p = (u16 *)(User_data + 0x1C4);
        for (i = 0; i < 100; i = (s16)(i + 1), p += 2) {
            if (*p != 0) {
                d = (u8 *)Item_data + *p * 0x10;
                cat = d[5];
                k = cat + 1;
                if (cat != 0xFF) {
                    ic.x = 0.8f * ((x0 - 146.0f) + 28.8f * (f32)(i % 10));
                    ic.y = i / 10 * 0x19 + 0x3B;
                    ic.u0 = ((k & 7) << 5) + 1;
                    ic.v0 = ((k >> 3) << 5) + 1;
                    ic.u1 = ((k & 7) << 5) + 0x1F;
                    ic.v1 = ((k >> 3) << 5) + 0x1F;
                    ic.color = item_col_tbl[d[6]];
                    ic.color = (ic.color & 0xFFFFFF) | (alpha << 24);
                    flps0008(&ic);
                }
            }
        }
    } else {
        for (i = 0; i < 0x40; i = (s16)(i + 1)) {
            q.color = (alpha << 24) | 0x200000;
            if (equip && Now_equip_ck(User_data, (s16)i) == 1) {
                q.color = (alpha << 24) | 0x83C6A;
            }
            xx = 0.8f * ((x0 - 146.0f) + 36.0f * (f32)(i & 7)) + 4;
            q.x = xx;
            q.x2 = 25.6f + (f32)xx;
            q.y = (i >> 3 << 5) + 0x3D;
            q.y2 = q.y + 0x1C;
            flps0004(&q);
        }
        ic.w = 0x20;
        ic.h = 0x20;
        mode80 = flags & 0x80;
        mode100 = flags & 0x100;
        mode200 = flags & 0x200;
        e = User_data + 0x44;
        for (i = 0; i < 0x40; i = (s16)(i + 1), e += 6) {
            if (e[0] != 0) {
                ic.x = 0.8f * ((x0 - 146.0f) + 36.0f * (f32)(i & 7));
                ic.y = (i >> 3 << 5) + 0x3B;
                Get_equip_icon_uv(e, &ic.u0, &ic.u1, 0.8f);
                rare = Get_equip_rare(e[1], *(u16 *)(e + 2)) & 0xFF;
                if (mode80 != 0) {
                    if (Now_equip_ck(User_data, (s16)i) == 1) {
                        ic.color = Equip_icon_color_rare(rare, alpha, 1);
                        flps0008(&ic);
                        ic.u0 = 0xC0;
                        ic.v0 = 0xE0;
                        ic.u1 = 0xE0;
                        ic.v1 = 0x100;
                        ic.color = (alpha << 24) | 0x909010;
                    } else {
                        ic.color = Equip_icon_color_rare(rare, alpha, 0);
                    }
                } else if (mode100 != 0) {
                    kd = 0;
                    if (e[1] != 6 && e[1] != 7) {
                        kd = 1;
                    }
                    ic.color = Equip_icon_color_rare(rare, alpha, kd);
                    if (Now_equip_ck(User_data, (s16)i) == 1) {
                        flps0008(&ic);
                        ic.u0 = 0xC0;
                        ic.v0 = 0xE0;
                        ic.u1 = 0xE0;
                        ic.v1 = 0x100;
                        if (kd == 0) {
                            ic.color = (alpha << 24) | 0xFFFF00;
                        } else {
                            ic.color = (alpha << 24) | 0x808010;
                        }
                    }
                } else if (mode200 != 0) {
                    ic.color = Equip_icon_color_rare(rare, alpha, 0);
                    if (Now_equip_ck(User_data, (s16)i) == 1) {
                        if (e[1] == 6 || e[1] == 7) {
                            ic.color = Equip_icon_color_rare(rare, alpha, 1);
                            flps0008(&ic);
                            ic.color = (alpha << 24) | 0x808010;
                        } else {
                            flps0008(&ic);
                            ic.color = (alpha << 24) | 0xFFFF00;
                        }
                        ic.u0 = 0xC0;
                        ic.v0 = 0xE0;
                        ic.u1 = 0xE0;
                        ic.v1 = 0x100;
                    } else if (equip_ok_chk(e) == 0) {
                        ic.color = Equip_icon_color_rare(rare, alpha, 1);
                    }
                } else {
                    ic.color = Equip_icon_color_rare(rare, alpha, 0);
                    if (Now_equip_ck(User_data, (s16)i) == 1) {
                        flps0008(&ic);
                        ic.u0 = 0xC0;
                        ic.v0 = 0xE0;
                        ic.u1 = 0xE0;
                        ic.v1 = 0x100;
                        ic.color = (alpha << 24) | 0xFFFF00;
                    }
                }
                flps0008(&ic);
            }
        }
    }
    if (cur >= 0) {
        if (!equip) {
            cs = cur / 10 * 0x19 + 0x3B;
            ic.x = 0.8f * ((x0 - 146.0f) + 28.8f * (f32)(cur % 10));
        } else {
            cs = (cur >> 3 << 5) + 0x3B;
            ic.x = 0.8f * ((x0 - 146.0f) + 36.0f * (f32)(cur & 7));
        }
        ic.y = cs;
        ic.u0 = 0;
        ic.u1 = 0x20;
        ic.v1 = 0x20;
        ic.color = (alpha << 24) | 0xFFFFFF;
        flps0008(&ic);
        flfntSetSize(0x12, 0x12);
        col = 0;
        if (dim == 0) {
            amount = 2;
        } else {
            col = 0xA;
            amount = 0xD;
        }
        font_set_palette(col);
        if (!equip) {
            d = User_data + cur * 4;
            if (*(u16 *)(d + 0x1C4) == 0) {
                sprintf(text, lit_1923_00668480, *(s32 *)0x33AB40);
                price = -1;
            } else {
                k = *(u16 *)(d + 0x1C4);
                switch (D_3396D3[k * 0x10]) {
                case 1:
                    sprintf(text, lit_1923_00668480, item_str[k]);
                    break;
                case 0xFF:
                    sprintf(text, lit_1924, item_str[k]);
                    break;
                default:
                    cs = *(s16 *)(d + 0x1C6);
                    if (cs >= D_3396D3[k * 0x10]) {
                        sprintf(text, lit_1925, item_str[k], (s16)amount);
                    } else {
                        sprintf(text, lit_1926, item_str[k], cs);
                    }
                    break;
                }
                price = *(s32 *)(D_3396DE + *(u16 *)(d + 0x1C4) * 0x10);
            }
        } else {
            e = User_data + cur * 6;
            if (e[0x44] == 0) {
                sprintf(text, lit_1923_00668480, *(s32 *)0x33AB40);
                price = -1;
            } else {
                sprintf(text, lit_1923_00668480, Get_equip_name(e[0x45], *(u16 *)(e + 0x46)));
                price = Get_equip_kaitori(e[0x45], *(u16 *)(e + 0x46));
            }
        }
        flfntLocate(5.0f + base, 0x13F);
        if (!(flags & 0x40)) {
            font_print_sp(lit_1927, category_name_str[equip], text);
        } else {
            font_print_sp(lit_1923_00668480, text);
            if (price >= 0) {
                flfntSetSize(0x18, 0x12);
                flfntLocate(198.0f + base, 0x13F);
                font_print(lit_1928, price);
                flfntSetSize(0x12, 0x12);
            }
        }
        if (flags & 4) {
            if (!equip) {
                Disp_help_mess(1, (u16)(*(u16 *)(User_data + cur * 4 + 0x1C4) + 0x18));
            } else {
                if (*(User_data + cur * 6 + 0x44) != 0) {
                    EquipmentDescriptionWindow(User_data + cur * 6 + 0x44, 0xB8, flags & 3);
                }
            }
        }
    }
    if (flags & 0x20) {
        Lb_put_gold();
    }
}
