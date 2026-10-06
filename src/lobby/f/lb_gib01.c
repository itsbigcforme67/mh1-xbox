/* lb_gib01 - item box / plaza chat / eft25 0x0060BC10-0x0060BE14: ib_select_sub. Whole file in lb_ib.c. */
#include "lobby_f.h"
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
void ListSelect();
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
void flfntLocate();
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
