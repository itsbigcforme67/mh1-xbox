/* lbshp, run 2: shop_select_tag .. shop_select_tag (lobby.bin 0x005376C0-0x00537854): the matching functions of lb_shop_nm.c. */
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

int shop_select_tag(void) {
    if (lbShop.key & 0x20) {
        lbShop.wait = 6;
        cnWrap_SoundRequest(0);
        return 0;
    }
    if (lbShop.key & 0x40) {
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (lbShop.x15 == 1) lbShop.mode = Lb_cursorUD(lbShop.mode, lbShop.x17);
    else lbShop.x1A = Lb_cursorUD(lbShop.x1A, lbShop.x17);
    if (lbShop.x15 == 1) {
        shop_tex_tbl[20] = (f32)(lbShop.pos[lbShop.mode][0] - 0xC);
        shop_tex_tbl[21] = lbShop.pos[lbShop.mode][1] + 6;
    } else {
        shop_tex_tbl[20] = (f32)(lbShop.pos[lbShop.x1A][0] - 0xC);
        shop_tex_tbl[21] = lbShop.pos[lbShop.x1A][1] + 6;
    }
    lbShop.x1B = 1;
    return 2;
}
