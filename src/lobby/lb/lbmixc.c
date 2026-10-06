/* lbmix, run 3: lb_mix_tag_decide .. lb_mix_tag_decide (lobby.bin 0x00535E80-0x00536140): the matching functions of lb_mix_nm.c. */
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

void lb_mix_tag_decide(void) {
    UD_ITEM *it = User_data[0].item;
    int i;
    int cnt;
    LB_SHOPITEM *sl = shopList;
    int v;
    int pages;

    memset(shopList, 0, 0x5000);
    switch (lbShop.mode) {
    case 0:
        cnt = lb_mix_makeMixList(lbShop.mode);
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
            if (Lb_mix_item_checkMax(v & 0xFFFF, 1) == 0) sl->state = 1;
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
