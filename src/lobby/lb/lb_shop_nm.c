/* lb_shop - lobby.bin 0x00536708-0x0053856C. Shared shop menu engine of the
 * lobby shops (forge lb_mix, item shop, lb_process, lb_armor): Lb_shop_move is
 * the step machine (tag select -> item list -> quantity -> yes/no) driven
 * by the handler slots in lbShop; this file also draws the list, help box,
 * tags and yes/no. Near-match: written from m2c, not yet compared. */
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
                if ((lbShop.x8E == 0 || lbShop.x8E == 3) && lbShop.f24 != 0) lbShop.f24(lbShop.x15);
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
                if ((u32)lbShop.x8E < 2 || lbShop.x8E == 2) {
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

int shop_select_items(void) {
    LB_SHOPITEM *it = (LB_SHOPITEM *)((u8 *)lbShop.list + lbShop.cur * 0x28);
    int keys;
    int n;

    if (lbShop.count == 0) {
        if (lbShop.key & 0x40) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        return 2;
    }
    keys = lbShop.key;
    if (keys & 0x20) {
        if (it->state == 0) {
            if (lbShop.f2C == 0) cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(7);
        return 1;
    }
    if (keys & 0x40) {
        cnWrap_SoundRequest(3);
        if (lbShop.x1C == 0) return 3;
        goto clear1C;
    }
    if (keys & 0x200) {
        if (it->state != 2) {
            if (lbShop.x8E == 2 && *(s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8 + 4) == 0x3E7) {
                cnWrap_SoundRequest(7);
            } else {
                if (lbShop.x8E == 0) cnWrap_SoundRequest(0xF);
                else cnWrap_SoundRequest(0xE);
                if (lbShop.x1C != 1) {
                    lbShop.x1C = 1;
                    lbShop.x6E = 0;
                } else {
                    goto clear1C;
                }
            }
        }
    } else if (keys & 0x800) {
        if (lbShop.x1C != 0 && lbShop.x8E != 0) {
            if (lbShop.x8E == 3 && lbShop.x1C != 2) goto pageL;
            if (lbShop.x1C == 1 && lbShop.x8E != 0) {
                cnWrap_SoundRequest(1);
                n = 2;
                if (*(s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8) == 7) n = 4;
                lbShop.x6E--;
                if (lbShop.x6E < 0) lbShop.x6E = n - 1;
            }
        } else {
pageL:
            if (lbShop.x6D >= 2) {
                cnWrap_SoundRequest(1);
                if (lbShop.x84 == 1) {
                    if (lbShop.x70 % 8 == 0) lbShop.x70 += 7;
                    else lbShop.x70--;
                } else {
                    lbShop.x6C--;
                    if (lbShop.x6C < 0) lbShop.x6C = lbShop.x6D - 1;
                    lbShop.x70 = 0;
                }
            }
        }
    } else if (keys & 0x400) {
        if (lbShop.x1C != 0 && lbShop.x8E != 0) {
            if (lbShop.x8E == 3 && lbShop.x1C != 2) goto pageR;
            if (lbShop.x1C == 1 && lbShop.x8E != 0) {
                cnWrap_SoundRequest(1);
                n = 2;
                if (*(s32 *)((u8 *)lbShop.tbl + lbShop.cur * 8) == 7) n = 4;
                lbShop.x6E++;
                if (lbShop.x6E >= n) lbShop.x6E = 0;
            }
        } else {
pageR:
            if (lbShop.x6D >= 2) {
                cnWrap_SoundRequest(1);
                if (lbShop.x84 == 1) {
                    if (lbShop.x70 % 8 == 7) lbShop.x70 -= 7;
                    else lbShop.x70++;
                } else {
                    lbShop.x6C++;
                    if (lbShop.x6C >= lbShop.x6D) lbShop.x6C = 0;
                    lbShop.x70 = 0;
                }
            }
        }
    } else if (keys & 0x2000) {
        if (lbShop.x1C != 0 && lbShop.x8E != 0) {
            if (lbShop.x8E == 3 && lbShop.x1C != 2) goto up;
        } else {
up:
            cnWrap_SoundRequest(1);
            if (lbShop.x84 == 1) {
                if (lbShop.x70 < 8) lbShop.x70 += lbShop.count - 8;
                else lbShop.x70 -= 8;
            } else {
                lbShop.x70--;
                if (lbShop.x70 < 0) {
                    lbShop.x70 = 6;
                    if (lbShop.x70 + lbShop.x6C * 7 >= lbShop.count) lbShop.x70 = lbShop.count % 7 - 1;
                }
            }
        }
    } else if (keys & 0x1000) {
        if (lbShop.x1C != 0 && lbShop.x8E != 0) {
            if (lbShop.x8E == 3 && lbShop.x1C != 2) goto down;
        } else {
down:
            cnWrap_SoundRequest(1);
            if (lbShop.x84 == 1) {
                if (lbShop.x70 >= lbShop.count - 8) lbShop.x70 = lbShop.x70 - lbShop.count + 8;
                else lbShop.x70 += 8;
            } else {
                lbShop.x70++;
                if (lbShop.x70 >= 7) lbShop.x70 = 0;
                if (lbShop.x70 + lbShop.x6C * 7 >= lbShop.count) lbShop.x70 = 0;
            }
        }
    } else if (lbShop.x8E != 0 && (keys & 0x80) && it->state != 2 && (lbShop.mode != 0 || lbShop.x1A != 1)) {
        if (lbShop.x8E != 3) {
            if (lbShop.x8E == 2) goto toggle;
        } else {
toggle:
            cnWrap_SoundRequest(0xE);
            if (lbShop.x1C != 2) {
                lbShop.x1C = 2;
                lbShop.x6E = 0;
            } else {
clear1C:
                lbShop.x1C = 0;
            }
        }
    }
    lbShop.cur = lbShop.x70 + lbShop.x6C * 7;
    return 2;
}

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
