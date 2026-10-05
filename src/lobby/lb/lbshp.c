/* lbshp, run 1: Lb_shop_move .. shop_end_ck (lobby.bin 0x00536730-0x00536F1C): the matching functions of lb_shop_nm.c. */
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

int Lb_shop_move(void) {
    int r;

    switch (lbShop.x15) {
    case 0:
        Lbc_set_prim(0, Lb_shop_trans2, Lb_shop_trans_sub);
        Lb_shop_tag_init();
        lbShop.x15++;
        if (lbShop.f20 != 0) lbShop.f20();
        /* fall through */
    case 1:
        r = shop_select_tag();
        switch (r) {
        case 0:
            lbShop.help = 0;
            lbShop.x1A = 0;
            if (lbShop.f24 != 0) lbShop.f24();
            if (lbShop.x16 != 0) lbShop.x15++;
            else lbShop.x15 = 3;
            break;
        case 3:
            return 3;
        }
        break;
    case 2:
        r = shop_select_tag(lbShop.x15);
        switch (r) {
        case 0:
            lbShop.help = 0;
            lbShop.x84 = 0;
            if (lbShop.f28 != 0) lbShop.f28();
            lbShop.x15++;
            break;
        case 3:
            Lb_shop_tag_init();
            if (lbShop.f20 != 0) lbShop.f20();
            lbShop.x15--;
            break;
        }
        break;
    case 3:
        if (Lb_shop_move_x(lbShop.x15) == 1) lbShop.x15 = 5;
        break;
    case 4:
        if (Lb_shop_move_xR(lbShop.x15) == 1) {
            Lb_shop_tag_init();
            if (lbShop.x16 == 0) {
                if (lbShop.f20 != 0) lbShop.f20();
                lbShop.x15 = 1;
            } else {
                if (lbShop.f24 != 0) lbShop.f24();
                lbShop.x15 = 2;
            }
        }
        break;
    case 5:
        r = shop_select_items(lbShop.x15);
        switch (r) {
        case 0:
            lbShop.qty = 1;
            if (lbShop.f2C != 0) {
                if (lbShop.f2C() == 0) break;
            }
            if (lbShop.x18 == 0) {
                lbShop.x78 = 0;
                lbShop.x15 = 7;
            } else {
                lbShop.x15++;
            }
            break;
        case 3:
            lbShop.x6C = 0;
            lbShop.cur = 0;
            lbShop.x15 = 4;
            lbShop.x84 = 0;
            lbShop.x1C = 0;
            if (lbShop.x8E != 0) {
                if (lbShop.x8E == 3) goto call24;
            } else {
call24:
                if (lbShop.f24 != 0) lbShop.f24();
            }
            break;
        }
        break;
    case 6:
        if (lbShop.f30 == 0) {
            lbShop.x78 = 0;
            lbShop.x15++;
        } else {
            r = lbShop.f30(lbShop.x15);
            switch (r) {
            case 0:
                lbShop.x78 = 0;
                lbShop.x15++;
                break;
            case 3:
                if (lbShop.f28 != 0) lbShop.f28();
                lbShop.x15--;
                if (lbShop.x8E != 2) lbShop.help = 0;
                if ((lbShop.x8E == 0 || lbShop.x8E == 3) && lbShop.f24 != 0) lbShop.f24();
                break;
            }
        }
        break;
    case 7:
        r = shop_end_ck(lbShop.x15);
        switch (r) {
        case 0:
            if (lbShop.f34 != 0) lbShop.f34();
            if (lbShop.x8F == 1) {
                lbShop.x78 = 0;
                lbShop.x15++;
                break;
            }
            if (lbShop.x16 == 1) {
                if (lbShop.f28 != 0) lbShop.f28();
                else if (lbShop.f24 != 0) lbShop.f24();
            } else if (lbShop.f24 != 0) {
                lbShop.f24();
            }
            /* fall through */
        case 3:
            if (lb_sys.x68 == 0xE && lbShop.mode == 0 && lbShop.x1A == 1) {
                lbShop.x15--;
                if (lbShop.x8E == 0 && lbShop.f28 != 0) lbShop.f28();
            } else {
                lbShop.x15 = 5;
                if (lbShop.x8E == 0 || lbShop.x8E == 1 || lbShop.x8E == 2) {
                    if (lbShop.f28 != 0) lbShop.f28();
                    else if (lbShop.f24 != 0) lbShop.f24();
                }
            }
            if (lbShop.x84 == 2) lbShop.x84 = 0;
            if ((lbShop.x8E == 0 || lbShop.x8E == 3) && lbShop.f24 != 0) lbShop.f24();
            break;
        }
        break;
    case 8:
        if (lbShop.f38 == 0 || lbShop.f38(lbShop.x15) != 2) {
            if (lbShop.x16 == 1) {
                if (lbShop.f28 != 0) lbShop.f28();
                else if (lbShop.f24 != 0) lbShop.f24();
            } else if (lbShop.f24 != 0) {
                lbShop.f24();
            }
            lbShop.x15 = 5;
        }
        break;
    }
    return 2;
}

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
