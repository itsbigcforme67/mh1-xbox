/* lb_tu_ib - one translation unit 0x00609700-0x0060D6E0: Lb_ItemBox_init, Lb_ItemBox_open, Lb_ItemBox_mv, itembox_cursor_mv, itembox_stock, itembox_pickup, itembox_equipchange, itembox_sortup, yes_no_select, itembox_sellout, u_item_chk, u_equip_chk, item_kosuu_sel_chk, pick_kosuu_sel_chk, kosuu_select, sortup_idx_chk, ib_select_sub, equip_ok_chk, ItemboxWindow, ItemboxWindowX, ItemboxWindowCursor, ItemboxWindowCursorX, disp_itembox_cmd, Disp_lb_item_box, item_explanation, kosuu_disp_sub, selling_price_disp_sub, yes_no_disp_sub. Built by tools/lbtu.py from the per-run files; functions that are
   not C yet stay original bytes (asm stubs, build/raw/*.inc). */
#include "lobby_f.h"
extern u8 * ib;
extern char item_box[];
typedef struct ITEMSLOT { u16 id; s16 num; } ITEMSLOT;
extern u8 *ib;
extern u8 User_data[];
extern ITEMSLOT D_3C7184[];
extern u8 D_3C7186[];
extern u8 D_3396D3[];
extern u8 D_3396D5[];
void se_req();
u8 *Get_equip_data_ptr();
/* item box slots: 4 bytes per slot at User_data + 0x37C (u16 item id, s16 amount) */
typedef struct IBS4 { u16 w[2]; } IBS4;
/* pouch (User_data + 0x1C4) and item box (User_data + 0x37C) slots, 4 bytes each: u16 item id, s16 amount */
#define UPID(i) F(u16, User_data + (i) * 4, 0x1C4)
#define UPNUM(i) F(s16, User_data + (i) * 4, 0x1C6)
#define IBID2(i) F(u16, User_data + (i) * 4, 0x37C)
#define IBNUM2(i) F(s16, User_data + (i) * 4, 0x37E)
#define IBID(u, i) (((IBS4 *)(u))[i].w[0x37C / 2])
#define IBNUM(u, i) (((IBS4 *)(u))[i].w[0x37E / 2])
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
static int u_equip_chk();
int Get_equip_kaitori();
void Gold_add();
int Warehouse_equip_out();
int Warehouse_equip();
void Lb_equip_set();
void armor_set_myArmor();
static int u_item_chk();
static int pick_kosuu_sel_chk();
static int item_kosuu_sel_chk();
void kosuu_select();
void yes_no_select(u16 pad);
int Ud_u_item_stack2();
int Chk_lb_status();
void Disp_menu_help();
void PutButtonICON();
void ItemListWindow();
int ItemboxWindow();
void EquipmentCompareWindow();
void EquipmentDescriptionWindow();
int ItemboxWindowCursor();
extern char verify_button_0038A008[8];
extern char frame_itembox_item_equip[];
extern u16 System_timer;
f32 flSin(f32);
u8 *sortup_idx_chk();
int itembox_stock();
int itembox_pickup();
int itembox_equipchange();
int itembox_sortup();
int itembox_sellout();
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
void font_set_palette();
void font_print_uf();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void PutArrow();
typedef struct IBSPR { s16 x, y, w, h; s32 color; s32 z; s32 size; } IBSPR;
typedef struct SW4 { s16 a, b; } SW4;            /* pouch item (id, amount) */
typedef struct SW6 { s16 a, b, c; } SW6;         /* equipment slot */
typedef struct IBQUAD { s16 x, y, x2, y2; s32 color; } IBQUAD;   /* flps0004 filled rectangle */
typedef struct IBICON { s16 x, y, w, h; s32 color; s16 u0, v0, u1, v1; } IBICON;   /* flps0008 textured icon */
extern u8 *bsw;
extern u8 ParseCk_ret[4];
extern s8 ParseReq;
extern u8 *Bs_work_free_head;
void CpInetInterfaceProblemEnable();
void *_zlib_calloc();
int inflateInit2_();
int ItemboxWindowX(f32, int, int);
int ItemboxWindowCursorX(f32, int, int, int);

void Lb_ItemBox_init(void) {
    ib = (u8 *)&item_box;
    F(s32, ib, 4) = 0;
    F(s16, ib, 2) = 0;
}

s32 Lb_ItemBox_open(void) {
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


/* original bytes: build/raw/Lb_ItemBox_mv.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int Lb_ItemBox_mv()
{
#include "Lb_ItemBox_mv.inc"
}
#endif

/* original bytes: build/raw/itembox_cursor_mv.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm void itembox_cursor_mv()
{
#include "itembox_cursor_mv.inc"
}
#endif

/* original bytes: build/raw/itembox_stock.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int itembox_stock()
{
#include "itembox_stock.inc"
}
#endif

/* original bytes: build/raw/itembox_pickup.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int itembox_pickup()
{
#include "itembox_pickup.inc"
}
#endif

/* original bytes: build/raw/itembox_equipchange.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int itembox_equipchange()
{
#include "itembox_equipchange.inc"
}
#endif

/* original bytes: build/raw/itembox_sortup.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int itembox_sortup()
{
#include "itembox_sortup.inc"
}
#endif

void yes_no_select(u16 pad) {
    u8 *t;
    u8 *q;
    t = ib;
    q = t + 0x21;
    if (*q == 0) {
        if (pad & 0x400) {
            *q = 1;
            se_req(7, 0x16, 0);
        }
    } else if (pad & 0x800) {
        *q = 0;
        se_req(7, 0x16, 0);
    }
}

/* original bytes: build/raw/itembox_sellout.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int itembox_sellout()
{
#include "itembox_sellout.inc"
}
#endif

static int u_item_chk(int a) {
    u16 id;
    id = D_3C7184[a & 0xFF].id;
    if (id == 0) {
        return 0;
    }
    return D_3396D5[id * 0x10] != 0xFF ? 1 : 0;
}


static int u_equip_chk(int a) {
    u8 *u;
    int v;
    u = User_data;
    v = a & 0xFF;
    if (*(u8 *)(v * 6 + u + 0x44) == 0) {
        return -1;
    }
    if (v == u[0x457] || v == u[0x458] || v == u[0x459] || v == u[0x45A] || v == u[0x45B] || v == u[0x456]) {
        return 0;
    }
    return 1;
}


static int item_kosuu_sel_chk() {
    int a0;
    u8 *p;
    u8 *u = User_data;
    u8 *new_var2;
    unsigned long long new_var;
    a0 = ib[8] * 4;
    new_var2 = D_3396D3;
    new_var = new_var2[(*((u16 *)(((u8 *)D_3C7184) + a0))) * 0x10];
    if (new_var == 0xFF) {
        return 0;
    }
    p = (u8 *)(a0 + (int)u);
    if (new_var2[(*((u16 *)(p + 0x1C4))) * 0x10] == 1) {
        return 0;
    }
    return *(s16 *)(p + 0x1C6) != 1 ? 1 : 0;
}


static int pick_kosuu_sel_chk() {
    u8 *v;
    int i;
    u8 *u = User_data;
    if (item_kosuu_sel_chk() == 0) {
        return 0;
    }
    i = ib[0xB] * 4;
    v = (u8 *)(i + (int)u);
    return (D_3396D3[*(u16 *)(v + 0x37C) * 0x10] - *(s16 *)(v + 0x37E)) >= 2;
}


void kosuu_select(int pad, int c) {
    int a3;
    int a0;
    s16 e;
    u8 *t0;
    u8 *p;
    int mx;
    int a1;
    u8 *u;
    s16 *q;
    int t;
    t0 = ib;
    u = User_data;
    mx = D_3C7186[t0[8] * 4];
    if (!(c & 0xFF)) {
        a1 = t0[0xB] * 4;
        p = (u8 *)(a1 + (int)u);
        a1 = D_3396D3[*(u16 *)(p + 0x37C) * 0x10] - *(s16 *)(p + 0x37E);
        if (a1 < (mx & 0xFF)) {
            mx = a1 & 0xFF;
        }
    }
    t = pad & 0xFFFF;
    t0[0x1C] = 0;
    p = ib;
    q = (s16 *)(p + 0x1A);
    a3 = *q;
    if (t & 0x800) {
        *q = 1;
    } else if (t & 0x400) {
        *q = mx & 0xFF;
    } else {
        a0 = mx & 0xFF;
        if (t & 0x2000) {
            if (a3 >= a0) {
                *q = a0;
                a3 = -1;
            } else {
                *q = a3 + 1;
            }
        } else if (t & 0x1000) {
            if (a3 > 1) {
                *q = a3 - 1;
            } else {
                a3 = -1;
            }
        }
    }
    e = a3;
    if (e < 0) {
        se_req(7, 0x15, 0);
    } else if (e != *(s16 *)(ib + 0x1A)) {
        se_req(7, 0x16, 0);
    }
    p = ib;
    if (*(s16 *)(p + 0x1A) >= (mx & 0xFF)) {
        p[0x1C] = 1;
    }
}



u8 *sortup_idx_chk(int a) {
    u8 *u;
    int v;
    u = User_data;
    v = a & 0xFF;
    if (v == User_data[0x457]) {
        return u + 0x457;
    }
    if (v == u[0x458]) {
        return u + 0x458;
    }
    if (v == u[0x459]) {
        return u + 0x459;
    }
    if (v == u[0x45A]) {
        return u + 0x45A;
    }
    if (v == u[0x45B]) {
        return u + 0x45B;
    }
    return v == u[0x456] ? u + 0x456 : 0;
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

/* original bytes: build/raw/equip_ok_chk.inc (config/c_rawfuncs.txt) */
int equip_ok_chk(u8 *e) {
  int r;
  int new_var[2];
  u8 *u;
  new_var[1] = e[1];
  r = 6;
  if ((e[1] == r) || (new_var[1] == 7))
  {
    return 1;
  }
  u = User_data;
  new_var[0] = Get_equip_data_ptr()[2];
  if (!(new_var[0] & ((((*((u8 *) 0x3C6FC1)) == 0) ? (1) : (2)) & 0xFF)))
  {
    return 0;
  }
  r = (u[(u[0x456] * r) + 0x45] == 6) ? (4) : (8);
  if (!(new_var[0] & (r & 0xFF)))
  {
    return 0;
  }
  return 1;
}


int ItemboxWindow(a, b)
int a;
int b;
{
    return ItemboxWindowX(306.0f, a, b);
}

/* original bytes: build/raw/ItemboxWindowX.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int ItemboxWindowX(f32 base, int cur, int flags)
{
#include "ItemboxWindowX.inc"
}
#endif

int ItemboxWindowCursor(int a, int b, int c) {
    return ItemboxWindowCursorX(306.0f, a, b, c);
}

int ItemboxWindowCursorX(f32 base, int idx, int color, int mode) {
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


/* flps0008 textured icon */


int disp_itembox_cmd(int a) {
    if (!(a & 0xFF)) {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0xA9182;
    } else {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0x808080;
    }
    return DispFrameList(frame_itembox_cmd, 0, F(u8, ib, 2));
}

/* original bytes: build/raw/Disp_lb_item_box.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__   /* PS2 only; the PC build takes the near-match C */
asm int Disp_lb_item_box()
{
#include "Disp_lb_item_box.inc"
}
#endif

/* flps0008 textured icon */


int item_explanation(a, b, c)
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

/* amount selector: two-digit number with up/down arrows */
int kosuu_disp_sub(void) {
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
int selling_price_disp_sub(void) {
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

int yes_no_disp_sub(void) {
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x1B0, 0x18E);
    return font_print_sp(yes_or_no[F(u8, ib, 0x21)]);
}

