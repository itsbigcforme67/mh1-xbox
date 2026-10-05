/* Lobby item box UI and plaza chat log (SLPM_654.95 lobby overlay 0x609770-0x60E330). Whole file; runs split into lb_ibNN.c */
#include "lobby_f.h"
extern u8 *ib;
/* item box slots: 4 bytes per slot at User_data + 0x37C (u16 item id, s16 amount) */
typedef struct IBS4 { u16 w[2]; } IBS4;
#define IBID(u, i) (((IBS4 *)(u))[i].w[0x37C / 2])
#define IBNUM(u, i) (((IBS4 *)(u))[i].w[0x37E / 2])
extern u8 User_data[];
int Ud_u_item_stack(u16, u16);
void Menu_select_mv();
void itembox_cursor_mv();
void PageSelect();
void se_req();
s32 Lb_ItemBox_open(u16 arg0, s32 arg1) {
    arg0 = 0;
    F(s32, ib, 4) = 0;
    F(s16, ib, 2) = 0;
    F(s16, ib, 8) = 0;
    F(s8, ib, 0xB) = arg0;
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
                F(s8, ib, 0xB) = 0;
                F(s16, ib, 8) = (u8)arg0;
                F(s8, ib, 0x1F) = 0;
                F(s8, ib, 0x1D) = -1;
                F(u8, ib, 0x1E) = 0xFF;
                F(u8, ib, 4) = F(u8, ib, 4) + 1;
                F(s8, ib, 5) = 0;
                se_req(7, 0x13, 0, -1);
    case 1:
                *(u8 *)0x39DAD0 = 1;
                switch (F(u8, ib, 2)) {
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

void item_explanation(int a, int b, int c) {
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
    u8 v;
    u8 hi;
    int p;
    u8 lo;
    u8 nv;
    v = *val;
    if (!(mode & 0xFF)) {
        v = v & 0xFF;
        p = pad & 0xFFFF;
        lo = v % 10;
        hi = v / 10;
        if (p & 0x800) {
            if (!(lo & 0xFF)) {
                lo = 9;
            } else {
                lo = lo - 1;
            }
        }
        if (p & 0x400) {
            if ((lo & 0xFF) >= 9) {
                lo = 0;
            } else {
                lo = lo + 1;
            }
        }
        if (p & 0x2000) {
            if (!(hi & 0xFF)) {
                hi = 9;
            } else {
                hi = hi - 1;
            }
        }
        if (p & 0x1000) {
            if ((hi & 0xFF) >= 9) {
                hi = 0;
            } else {
                hi = hi + 1;
            }
        }
        nv = (lo & 0xFF) % 10 + (hi & 0xFF) % 10 * 10;
    } else {
        lo = v;
        p = pad & 0xFFFF;
        hi = lo >> 3;
        if (p & 0x800) {
            lo = lo - 1;
        }
        if (p & 0x400) {
            lo = lo + 1;
        }
        if (p & 0x2000) {
            hi = hi - 1;
        }
        if (p & 0x1000) {
            hi = hi + 1;
        }
        nv = (lo & 7) | ((hi & 7) << 3);
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
            if (IBID(u, F(u8, ib, 0xB)) != 0) {
                left = Ud_u_item_stack(IBID(u, F(u8, ib, 0xB)), IBNUM(u, F(u8, ib, 0xB))) & 0xFFFF;
                if (left == 0) {
                    IBNUM(u, F(u8, ib, 0xB)) = 0;
                    IBID(u, F(u8, ib, 0xB)) = 0;
                    F(s8, ib, 0x1D) = 1;
                    se_req(7, 0x2C, 0, left);
                } else {
                    IBNUM(u, F(u8, ib, 0xB)) = left;
                    F(s8, ib, 0x1D) = 2;
                    se_req(7, 0x15, 0, left);
                }
                *(s16 *)0x39DAD2 = F(s8, ib, 0x1D);
                F(u8, ib, 0x1F) = 0;
                F(u8, ib, 5) = F(u8, ib, 5) + 1;
            } else {
                se_req(7, 0x15, 0);
            }
        }
        break;
    case 1:
        *(s16 *)0x39DAD2 = F(s8, ib, 0x1D);
        F(u8, ib, 0x20) = F(u8, ib, 0x20) + 1;
        if ((u16)pad & 0x20) {
            *(s16 *)0x39DAD2 = 0;
            F(u8, ib, 5) = 0;
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
