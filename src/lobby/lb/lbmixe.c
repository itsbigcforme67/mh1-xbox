/* lbmix, run 5 (lobby.bin 0x00535840-0x00535BF4): lb_mix_put_itemDetail. Whole file context: lb_mix - 0x00535240-0x00536708. Lobby forge/item-trade shop
 * (Lb_mix): list building, item select, buy/sell/make. lbShop.mode 0 = make
 * (mix recipes), 1 = buy, 2 = sell. */
#include "lobby.h"
#include "pl.h"
#include "em.h"
#include "ud.h"
extern u8 lb_shop_msg[];
extern char lit_359_00654D68[];
extern char lit_360_00654D70[];

void Lb_shop_tag_init();
void Lb_shop_talk();
s8 Lb_talk_check_default();
int Lb_shop_move();
void Lbc_set_prim();
void NPCZoomInCameraCancel();
void cnWrap_SoundRequest();
void flMemset();
void Lb_mix_init_member(EMW *pl);
int Lb_mix_item_checkMax(s32 id, s8 qty);
extern s32 shop_default_tag_00610A68[];
extern u8 *npc_dialog_table[];
extern s32 shopTbl[];

void lb_mix_tag_decide();
int lb_mix_select();
int lb_mix_item_select();
void lb_mix_decide();
void lb_mix_put_itemDetail();
extern s32 *pit_help_str_tbl[];
void lb_mix_listIcon(int x, int y, int z, s16 n);


void lb_mix_put_itemDetail(void) {
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
