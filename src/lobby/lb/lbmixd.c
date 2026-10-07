/* lbmix, run 4: lb_mix_listIcon .. CheckItemPrice (lobby.bin 0x00536620-0x00536708): the matching functions of lb_mix_nm.c. */
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
