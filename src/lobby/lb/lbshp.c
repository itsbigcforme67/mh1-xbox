/* lbshp, run 1: shop_end_ck .. shop_end_ck (lobby.bin 0x00536E20-0x00536F1C): the matching functions of lb_shop_nm.c. */
#include "lobby.h"
#include "pl.h"

void Lbc_set_prim();
void Lb_shop_tag_init();
int shop_end_ck();
int shop_select_items();
int shop_select_tag();
int Lb_shop_move_x();
int Lb_shop_move_xR();
void Lb_shop_trans2();
void Lb_shop_trans_sub();
void cnWrap_SoundRequest();
void Lb_put_shopCursor(void);
char Lb_cursorUD();

extern s16 shop_tex_tbl[];

int shop_end_ck(void) {
    if (lbShop.key & 0x20) {
        if (lbShop.x78 == 0) return 0;
        cnWrap_SoundRequest(3);
        return 3;
    } else if (lbShop.key & 0x40) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(3);
            lbShop.x78 = 1;
        } else {
            cnWrap_SoundRequest(3);
            return 3;
        }
    } else if (lbShop.key & 0x800) {
        if (lbShop.x78 != 0) {
            cnWrap_SoundRequest(1);
            lbShop.x78 = 0;
        }
    } else if ((lbShop.key & 0x400) && lbShop.x78 != 1) {
        cnWrap_SoundRequest(1);
        lbShop.x78 = 1;
    }
    return 2;
}
