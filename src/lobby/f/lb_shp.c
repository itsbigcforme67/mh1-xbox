/* lb_shp - one translation unit 0x00536730-0x00537B98 (lbtu3). */
#define F(T, p, o) (*(T *)((u8 *)(p) + (o)))
#include "lobby.h"
#include "pl.h"
void Lbc_set_prim();
void Lb_shop_tag_init();
int Lb_shop_move_x();
int Lb_shop_move_xR();
void cnWrap_SoundRequest();
void Lb_put_shopCursor(void);
char Lb_cursorUD();
extern s16 shop_tex_tbl[];
/* original bytes: build/raw/shop_select_items.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
#else
#endif
#include "lobby.h"
#include "pl.h"
extern u8 shop_tex_tbl_c3[];
void ItemboxWindowX(f32 x, s16 y, int flags);
int Lb_shop_move();
int shop_end_ck();
asm int shop_select_items();
int shop_select_items();
int shop_select_tag();
void Lb_shop_trans2(int arg0);
void Lb_shop_trans_sub(int arg0);
int Lb_shop_move() {
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

int shop_end_ck() {
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

#ifdef __MWERKS__
asm int shop_select_items()
{
#include "shop_select_items.inc"
}
#endif

int shop_select_tag() {
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

void Lb_shop_trans2(int arg0) {
    u16 v;

    if (*(u8 *)&lbShop.x1B != 0) {
        font_set_stack_no(F(s32, arg0, 0x18));
        flfntSetSize(0x14, 0x14);
        font_set_palette(0);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        if (lbShop.x15 >= 5) {
            switch (lbShop.x84) {
            case 0:
                if (lbShop.x8E == 0 || lbShop.x8E == 3 || lbShop.x1C != 1) {
                    if (lbShop.x6D != 0 || lbShop.x8E == 3) {
                        lb_put_shopList();
                        if (lbShop.x6D != 0) {
                            Put_page_num(0x20E, 0x38, lbShop.x6C, lbShop.x6D, 1);
                        }
                    }
                } else {
                    if (lbShop.f3C != 0) {
                        lbShop.f3C();
                    }
                }
                Lb_put_gold();
                break;
            case 1:
                if (lbShop.mode == 0 && lbShop.x1A == 1) {
                    v = 0x100;
                } else {
                    v = 0xC0;
                }
                if (lbShop.x1C != 0) {
                    ItemboxWindowX(292.0f, lbShop.x70, v | 0xC | (lbShop.x6E & 3) | 0x10);
                } else {
                    ItemboxWindowX(292.0f, lbShop.x70, v | 8);
                }
                break;
            case 2:
                Lb_put_gold();
                flfntSetSize(0x12, 0x12);
                EquipmentCompareWindow(lbShop.x54, lbShop.x5A, 0x126, 0x3C, 0x80);
                break;
            }
            lb_put_shopHelp();
        }
        if (lbShop.x84 != 1) {
            lb_put_mk_tags();
            if (lbShop.x15 < 5 && lbShop.x15 < 3) {
                Lb_put_2TF(&shop_tex_tbl_c3[0x28], 1);
            }
        }
    }
}

void Lb_shop_trans_sub(int arg0) {
    if ((lbShop.x15 >= 5) && (lbShop.x84 == 0)) {
        if ((lbShop.x8E != 0) && (lbShop.x8E != 3)) {
            if (lbShop.x1C != 1) {
                goto block_6;
            }
        } else {
block_6:
            reload_tex(1, 0x157);
            SetTextureStage(0x157);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            font_set_stack_no(F(s32, arg0, 0x18));
            flfntSetSize(0x14, 0x14);
            font_set_palette(0);
            if (lbShop.f3C != 0) {
                lbShop.f3C();
            }
        }
    }
}

