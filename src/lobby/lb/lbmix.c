/* lbmix, run 1: Lb_mix_init_member .. lb_mix_select (lobby.bin 0x00535240-0x0053583C): the matching functions of lb_mix_nm.c. */
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
void lb_mix_listIcon(int x, int y, int z, s16 n);


short Ud_item_num_ck3();
short Ud_item_num_ck();
int CheckItemPrice(int id, int qty);
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



/* can `qty` of item `id` be bought / is it held (mode 2) */

void Lb_mix_init_member(EMW *pl) {
    lbShop.x1B = 0;
    Lb_shop_tag_init();
}

void lb_mix_init(void) {
    lbShop.tag = shop_default_tag_00610A68;
}

void Lb_mix(void) {
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

int lb_mix_select(void) {
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
